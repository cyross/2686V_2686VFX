#include "./SynthVoice.h"

SynthVoice::SynthVoice()
{
    coreMap[OscMode::OPZX7] = &m_opzx7Core;
}

void SynthVoice::prepare(double sampleRate) {
    m_opzx7Core.prepare(sampleRate);
}

void SynthVoice::setParameters(const SynthParams& params)
{
    m_mode = params.mode;
    m_opzx7Core.setParameters(params);
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
        m_opzx7Core.noteOff();
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
        m_opzx7Core.prepare(newRate);
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
    m_opzx7Core.setCurveCore(p_curveCore);
}

bool SynthVoice::isPlaying()
{
    return coreMap[m_mode]->isPlaying();
}
