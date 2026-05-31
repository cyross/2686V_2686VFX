#include "SynthVoice.h"

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
    coreMap[OscMode::RHYTHM] = &m_rhythmCore;
    coreMap[OscMode::ADPCM] = &m_adpcmCore;
    coreMap[OscMode::BEEP] = &m_beepCore;
}

void SynthVoice::setParameters(const SynthParams& params)
{
    m_mode = params.mode;

    for (auto& pair : coreMap)
    {
        pair.second->setParameters(params);
    }
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
        for (int i = 0; i < (int)OscMode::Count; i++)
        {
            coreMap[(OscMode)i]->noteOff();
        }
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
        for (int i = 0; i < (int)OscMode::Count; i++)
        {
            coreMap[(OscMode)i]->prepare(newRate);
        }
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

void SynthVoice::setOpzx7PcmBuffer(int opIndex, const std::vector<float>* pcmData)
{
    m_opzx7Core.setPcmBuffer(opIndex, pcmData);
}

void SynthVoice::setOpzx7WtBuffer(int opIndex, const std::vector<float>* wtData)
{
    m_opzx7Core.setWtBuffer(opIndex, wtData);
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
    m_rhythmCore.setCurveCore(p_curveCore);
    m_adpcmCore.setCurveCore(p_curveCore);
    m_beepCore.setCurveCore(p_curveCore);
}
