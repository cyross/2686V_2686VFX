#include "./SynthVoice.h"

SynthVoice::SynthVoice()
{
    coreMap[OscMode::OPNA] = &m_opnaCore;
    coreMap[OscMode::SSG] = &m_ssgCore;
    coreMap[OscMode::RHYTHM] = &m_rhythmCore;
    coreMap[OscMode::ADPCM] = &m_adpcmCore;
}

void SynthVoice::prepare(double sampleRate) {
    m_opnaCore.prepare(sampleRate);
    m_ssgCore.prepare(sampleRate);
    m_rhythmCore.prepare(sampleRate);
    m_adpcmCore.prepare(sampleRate);
}

void SynthVoice::setParameters(const SynthParams& params)
{
    m_mode = params.mode;
    m_opnaCore.setParameters(params);
    m_ssgCore.setParameters(params);
    m_rhythmCore.setParameters(params);
    m_adpcmCore.setParameters(params);
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
        m_ssgCore.noteOff();
        m_rhythmCore.noteOff();
        m_adpcmCore.noteOff();
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
        m_ssgCore.prepare(newRate);
        m_rhythmCore.prepare(newRate);
        m_adpcmCore.prepare(newRate);
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

bool SynthVoice::isPlaying()
{
    return coreMap[m_mode]->isPlaying();
}
