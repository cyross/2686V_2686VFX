#include "./SynthVoice.h"

SynthVoice::SynthVoice()
{
    coreMap[OscMode::OPNA] = &m_opnaCore;
    coreMap[OscMode::OPN] = &m_opnCore;
    coreMap[OscMode::OPL] = &m_oplCore;
    coreMap[OscMode::OPL3] = &m_opl3Core;
    coreMap[OscMode::OPM] = &m_opmCore;
    coreMap[OscMode::OPZX7] = &m_opzx7Core;
    coreMap[OscMode::SSG] = &m_ssgCore;
    coreMap[OscMode::WAVETABLE] = &m_wtCore;
    coreMap[OscMode::WT2] = &m_wt2Core;
    coreMap[OscMode::RHYTHM] = &m_rhythmCore;
    coreMap[OscMode::ADPCM] = &m_adpcmCore;
    coreMap[OscMode::BEEP] = &m_beepCore;
}

void SynthVoice::prepare(double sampleRate) {
    m_opnaCore.prepare(sampleRate);
    m_opnCore.prepare(sampleRate);
    m_oplCore.prepare(sampleRate);
    m_opl3Core.prepare(sampleRate);
    m_opmCore.prepare(sampleRate);
    m_opzx7Core.prepare(sampleRate);
    m_ssgCore.prepare(sampleRate);
    m_wtCore.prepare(sampleRate);
    m_wt2Core.prepare(sampleRate);
    m_rhythmCore.prepare(sampleRate);
    m_adpcmCore.prepare(sampleRate);
    m_beepCore.prepare(sampleRate);
}

void SynthVoice::setParameters(const SynthParams& params)
{
    m_mode = params.mode;
    m_opnaCore.setParameters(params);
    m_opnCore.setParameters(params);
    m_oplCore.setParameters(params);
    m_opl3Core.setParameters(params);
    m_opmCore.setParameters(params);
    m_opzx7Core.setParameters(params);
    m_ssgCore.setParameters(params);
    m_wtCore.setParameters(params);
    m_wt2Core.setParameters(params);
    m_rhythmCore.setParameters(params);
    m_adpcmCore.setParameters(params);
    m_beepCore.setParameters(params);
}

void SynthVoice::startNote(int midiNote, float velocity, juce::SynthesiserSound*, int)
{
    // 周波数計算
    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNote);

    coreMap[m_mode]->noteOn(cyclesPerSecond, velocity, midiNote);
}

void SynthVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
    {
        m_opnaCore.noteOff();
        m_opnCore.noteOff();
        m_oplCore.noteOff();
        m_opl3Core.noteOff();
        m_opmCore.noteOff();
        m_opzx7Core.noteOff();
        m_ssgCore.noteOff();
        m_wtCore.noteOff();
        m_wt2Core.noteOff();
        m_rhythmCore.noteOff();
        m_adpcmCore.noteOff();
        m_beepCore.noteOff();
    }
    else
    {
        clearCurrentNote();
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isVoiceActive()) return;

    float* outL = outputBuffer.getWritePointer(0);
    float* outR = outputBuffer.getWritePointer(1);

    for (int i = 0; i < numSamples; ++i)
    {
        float sample = 0.0f;

        bool isActive = false;

        coreMap[m_mode]->renderNextBlock(outR, outL, startSample, i, isActive);

        if (!isActive)
        {
            clearCurrentNote();
            break;
        }
    }
}

void SynthVoice::setCurrentPlaybackSampleRate(double newRate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);

    if (newRate > 0.0)
    {
        m_opnaCore.prepare(newRate);
        m_opnCore.prepare(newRate);
        m_oplCore.prepare(newRate);
        m_opl3Core.prepare(newRate);
        m_opmCore.prepare(newRate);
        m_opzx7Core.prepare(newRate);
        m_ssgCore.prepare(newRate);
        m_wtCore.prepare(newRate);
        m_wt2Core.prepare(newRate);
        m_rhythmCore.prepare(newRate);
        m_adpcmCore.prepare(newRate);
        m_beepCore.prepare(newRate);
    }
}

// ピッチベンド
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    coreMap[m_mode]->setPitchBend(newPitchWheelValue);
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    // CC #1 = Modulation Wheel
    if (controllerNumber == 1)
    {
        coreMap[m_mode]->setModulationWheel(newControllerValue);
    }
}

void SynthVoice::setOpzx7PcmBuffer(int opIndex, std::vector<float>* pcmData)
{
    m_opzx7Core.setPcmBuffer(opIndex, pcmData);
}

void SynthVoice::setOpzx7WtBuffer(int opIndex, std::vector<float>* wtData)
{
    m_opzx7Core.setWtBuffer(opIndex, wtData);
}

void SynthVoice::setOpzx7Wt2Buffer(int opIndex, std::vector<float>* wtData)
{
    m_opzx7Core.setWt2Buffer(opIndex, wtData);
}

void SynthVoice::clearOpzx7PcmBuffer(int opIndex)
{
    m_opzx7Core.clearPcmBuffer(opIndex);
}

void SynthVoice::clearOpzx7WtBuffer(int opIndex)
{
    m_opzx7Core.clearWtBuffer(opIndex);
}

void SynthVoice::clearOpzx7Wt2Buffer(int opIndex)
{
    m_opzx7Core.clearWt2Buffer(opIndex);
}

void SynthVoice::setCurveCore(CurveCore* p_curveCore)
{
    m_opnaCore.setCurveCore(p_curveCore);
    m_opnCore.setCurveCore(p_curveCore);
    m_oplCore.setCurveCore(p_curveCore);
    m_opl3Core.setCurveCore(p_curveCore);
    m_opmCore.setCurveCore(p_curveCore);
    m_opzx7Core.setCurveCore(p_curveCore);
    m_ssgCore.setCurveCore(p_curveCore);
    m_wtCore.setCurveCore(p_curveCore);
    m_wt2Core.setCurveCore(p_curveCore);
    m_rhythmCore.setCurveCore(p_curveCore);
    m_adpcmCore.setCurveCore(p_curveCore);
    m_beepCore.setCurveCore(p_curveCore);
}
