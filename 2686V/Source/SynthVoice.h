#pragma once
#include <JuceHeader.h>
#include "Mode.h"
#include "OpnCore.h"
#include "OpnaCore.h"
#include "OplCore.h"
#include "OpllCore.h"
#include "Opl3Core.h"
#include "OpmCore.h"
#include "SSGCore.h"
#include "WavetableCore.h"
#include "RhythmCore.h"
#include "AdpcmCore.h"

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice() {}

    void setParameters(const SynthParams& params)
    {
        m_mode = params.mode;

        switch (m_mode)
        {
            case OscMode::OPNA:
                m_opnaCore.setParameters(params);
                break;
            case OscMode::OPN:
                m_opnCore.setParameters(params);
                break;
            case OscMode::OPL:
                m_oplCore.setParameters(params);
                break;
            case OscMode::OPLL:
                m_opll.setParameters(params);
                break;
            case OscMode::OPL3:
                m_opl3.setParameters(params);
                break;
            case OscMode::OPM:
                m_opm.setParameters(params);
                break;
            case OscMode::SSG:
                m_ssgCore.setParameters(params);
                break;
            case OscMode::WAVETABLE:
                m_wtCore.setParameters(params);
                break;
            case OscMode::RHYTHM:
                m_rhythmCore.setParameters(params);
                break;
            case OscMode::ADPCM:
                m_adpcmCore.setParameters(params);
                break;
            default:
				break;
        }
    }

    AdpcmCore* getAdpcmCore() { return &m_adpcmCore; }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }

    void startNote(int midiNote, float velocity, juce::SynthesiserSound*, int) override
    {
        // 周波数計算
        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNote);

        switch(m_mode) {
            case OscMode::OPNA:
                m_opnaCore.noteOn(cyclesPerSecond, velocity);
                break;
            case OscMode::OPN:
                m_opnCore.noteOn(cyclesPerSecond, velocity);
                break;
            case OscMode::OPL:
                m_oplCore.noteOn(cyclesPerSecond, velocity);
                break;
            case OscMode::OPLL:
                m_opll.noteOn(cyclesPerSecond, velocity);
                break;
            case OscMode::OPL3:
                m_opl3.noteOn(cyclesPerSecond, velocity);
                break;
            case OscMode::OPM:
                m_opm.noteOn(cyclesPerSecond, velocity);
                break;
            case OscMode::SSG:
                m_ssgCore.noteOn(cyclesPerSecond);
                break;
            case OscMode::WAVETABLE:
                m_wtCore.noteOn(cyclesPerSecond);
                break;
            case OscMode::RHYTHM:
                m_rhythmCore.noteOn(midiNote, velocity);
                break;
            case OscMode::ADPCM:
                m_adpcmCore.noteOn(cyclesPerSecond);
                break;
            default:
                break;
        }
    }

    void stopNote(float, bool allowTailOff) override
    {
        m_opnaCore.noteOff();
        m_opnCore.noteOff();
        m_oplCore.noteOff();
        m_opll.noteOff();
        m_opl3.noteOff();
        m_opm.noteOff();
        m_ssgCore.noteOff();
        m_wtCore.noteOff();
        m_rhythmCore.noteOff();
        m_adpcmCore.noteOff();

        if (!allowTailOff) clearCurrentNote();
    }

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (!isVoiceActive()) return;

        float* outL = outputBuffer.getWritePointer(0);
        float* outR = outputBuffer.getWritePointer(1);

        for (int i = 0; i < numSamples; ++i)
        {
            float sample = 0.0f;

            bool isActive = false;

            if (m_mode == OscMode::OPNA)
            {
                sample = m_opnaCore.getSample();
                isActive = m_opnaCore.isPlaying();
            }
            else if (m_mode == OscMode::OPN)
            {
                sample = m_opnCore.getSample();
                isActive = m_opnCore.isPlaying();
            }
            else if (m_mode == OscMode::OPL)
            {
                sample = m_oplCore.getSample();
                isActive = m_oplCore.isPlaying();
            }
            else if (m_mode == OscMode::OPLL)
            {
                sample = m_opll.getSample();
                isActive = m_opll.isPlaying();
            }
            else if (m_mode == OscMode::OPL3)
            {
                sample = m_opl3.getSample();
                isActive = m_opl3.isPlaying();
            }
            else if (m_mode == OscMode::OPM)
            {
                sample = m_opm.getSample();
                isActive = m_opm.isPlaying();
            }
            else if (m_mode == OscMode::SSG)
            {
                sample = m_ssgCore.getSample();
                isActive = m_ssgCore.isPlaying();
            }
            else if (m_mode == OscMode::WAVETABLE)
            {
                sample = m_wtCore.getSample();
                isActive = m_wtCore.isPlaying();
            }
            else if (m_mode == OscMode::RHYTHM)
            {
                sample = m_rhythmCore.getSample();
                isActive = m_rhythmCore.isPlaying();
            }
            else if (m_mode == OscMode::ADPCM)
            {
                sample = m_adpcmCore.getSample();
                isActive = m_adpcmCore.isPlaying();
            }

            outL[startSample + i] += sample;
            outR[startSample + i] += sample;

            if (!isActive)
            {
                clearCurrentNote();
                break;
            }
        }
    }

    void setCurrentPlaybackSampleRate(double newRate) override
    {
        juce::SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);

        if (newRate > 0.0)
        {
            m_opnaCore.prepare(newRate);
            m_opnCore.prepare(newRate);
            m_oplCore.prepare(newRate);
            m_opll.prepare(newRate);
            m_opl3.prepare(newRate);
            m_opm.prepare(newRate);
            m_ssgCore.prepare(newRate);
            m_wtCore.prepare(newRate);
            m_rhythmCore.prepare(newRate);
            m_adpcmCore.prepare(newRate);
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

private:
    OscMode m_mode = OscMode::OPNA;
    OpnaCore m_opnaCore;
    OpnCore m_opnCore;
    OplCore m_oplCore;
    OpllCore m_opll;
    Opl3Core m_opl3;
    OpmCore  m_opm;
    SsgCore m_ssgCore;
    WavetableCore m_wtCore;
    RhythmCore m_rhythmCore;
    AdpcmCore m_adpcmCore;
};
