#pragma once
#include <JuceHeader.h>
#include "Mode.h"
#include "OpnCore.h"
#include "OpnaCore.h"
#include "OplCore.h"
#include "Opl3Core.h"
#include "OpmCore.h"
#include "Opzx3Core.h"
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
            case OscMode::OPL3:
                m_opl3Core.setParameters(params);
                break;
            case OscMode::OPM:
                m_opmCore.setParameters(params);
                break;
            case OscMode::OPZX3:
                m_opzx3Core.setParameters(params);
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
    RhythmCore* getRhythmCore() { return &m_rhythmCore; }

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
            case OscMode::OPL3:
                m_opl3Core.noteOn(cyclesPerSecond, velocity);
                break;
            case OscMode::OPM:
                m_opmCore.noteOn(cyclesPerSecond, velocity);
                break;
            case OscMode::OPZX3:
                m_opzx3Core.noteOn(cyclesPerSecond, velocity);
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
        if (allowTailOff)
        {
            m_opnaCore.noteOff();
            m_opnCore.noteOff();
            m_oplCore.noteOff();
            m_opl3Core.noteOff();
            m_opmCore.noteOff();
            m_opzx3Core.noteOff();
            m_ssgCore.noteOff();
            m_wtCore.noteOff();
            m_rhythmCore.noteOff();
            m_adpcmCore.noteOff();
        }
        else
        {
            clearCurrentNote();
        }
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

                outL[startSample + i] += sample;
                outR[startSample + i] += sample;
            }
            else if (m_mode == OscMode::OPN)
            {
                sample = m_opnCore.getSample();
                isActive = m_opnCore.isPlaying();

                outL[startSample + i] += sample;
                outR[startSample + i] += sample;
            }
            else if (m_mode == OscMode::OPL)
            {
                sample = m_oplCore.getSample();
                isActive = m_oplCore.isPlaying();

                outL[startSample + i] += sample;
                outR[startSample + i] += sample;
            }
            else if (m_mode == OscMode::OPL3)
            {
                sample = m_opl3Core.getSample();
                isActive = m_opl3Core.isPlaying();

                outL[startSample + i] += sample;
                outR[startSample + i] += sample;
            }
            else if (m_mode == OscMode::OPM)
            {
                sample = m_opmCore.getSample();
                isActive = m_opmCore.isPlaying();

                outL[startSample + i] += sample;
                outR[startSample + i] += sample;
            }
            else if (m_mode == OscMode::OPZX3)
            {
                sample = m_opzx3Core.getSample();
                isActive = m_opzx3Core.isPlaying();

                outL[startSample + i] += sample;
                outR[startSample + i] += sample;
            }
            else if (m_mode == OscMode::SSG)
            {
                sample = m_ssgCore.getSample();
                isActive = m_ssgCore.isPlaying();

                outL[startSample + i] += sample;
                outR[startSample + i] += sample;
            }
            else if (m_mode == OscMode::WAVETABLE)
            {
                sample = m_wtCore.getSample();
                isActive = m_wtCore.isPlaying();

                outL[startSample + i] += sample;
                outR[startSample + i] += sample;
            }
            else if (m_mode == OscMode::RHYTHM)
            {
                float padOutL = 0.0f;
                float padOutR = 0.0f;

                // RhythmCore 内部で Pan 適用済みのステレオミックスを受け取る
                m_rhythmCore.getSampleStereo(padOutL, padOutR);
                isActive = m_rhythmCore.isPlaying();

                outL[startSample + i] += padOutL;
                outR[startSample + i] += padOutR;
            }
            else if (m_mode == OscMode::ADPCM)
            {
                sample = m_adpcmCore.getSample();
                isActive = m_adpcmCore.isPlaying();

                float pan = m_adpcmCore.getCurrentPan();
                outL[startSample + i] += sample * (1.0f - pan);
                outR[startSample + i] += sample * pan;
            }

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
            m_opl3Core.prepare(newRate);
            m_opmCore.prepare(newRate);
            m_opzx3Core.prepare(newRate);
            m_ssgCore.prepare(newRate);
            m_wtCore.prepare(newRate);
            m_rhythmCore.prepare(newRate);
            m_adpcmCore.prepare(newRate);
        }
    }

    // ピッチベンド
    void pitchWheelMoved(int newPitchWheelValue) override
    {
        // 各モードのCoreに伝達
        switch (m_mode) {
        case OscMode::OPNA:
            m_opnaCore.setPitchBend(newPitchWheelValue);
            break;
        case OscMode::OPN:
            m_opnCore.setPitchBend(newPitchWheelValue);
            break;
        case OscMode::OPL:
            m_oplCore.setPitchBend(newPitchWheelValue);
            break;
        case OscMode::OPL3:
            m_opl3Core.setPitchBend(newPitchWheelValue);
            break;
        case OscMode::OPM:
            m_opmCore.setPitchBend(newPitchWheelValue);
            break;
        case OscMode::OPZX3:
            m_opzx3Core.setPitchBend(newPitchWheelValue);
            break;
        case OscMode::SSG:
            m_ssgCore.setPitchBend(newPitchWheelValue);
            break;
        case OscMode::WAVETABLE:
            m_wtCore.setPitchBend(newPitchWheelValue);
            break;
        case OscMode::RHYTHM:
            m_rhythmCore.setPitchBend(newPitchWheelValue);
            break;
        case OscMode::ADPCM:
            m_adpcmCore.setPitchBend(newPitchWheelValue);
            break;
        }
    }

    // コントローラー (CC)
    void controllerMoved(int controllerNumber, int newControllerValue) override
    {
        // CC #1 = Modulation Wheel
        if (controllerNumber == 1)
        {
            // 各モードのCoreに伝達
            switch (m_mode) {
            case OscMode::OPNA:
                m_opnaCore.setModulationWheel(newControllerValue);
                break;
            case OscMode::OPN:
                m_opnCore.setModulationWheel(newControllerValue);
                break;
            case OscMode::OPL:
                m_oplCore.setModulationWheel(newControllerValue);
                break;
            case OscMode::OPL3:
                m_opl3Core.setModulationWheel(newControllerValue);
                break;
            case OscMode::OPM:
                m_opmCore.setModulationWheel(newControllerValue);
                break;
            case OscMode::OPZX3:
                m_opzx3Core.setModulationWheel(newControllerValue);
                break;
            case OscMode::SSG:
                m_ssgCore.setModulationWheel(newControllerValue);
                break;
            case OscMode::WAVETABLE:
                m_wtCore.setModulationWheel(newControllerValue);
                break;
            case OscMode::RHYTHM:
                m_rhythmCore.setModulationWheel(newControllerValue);
                break;
            case OscMode::ADPCM:
                m_adpcmCore.setModulationWheel(newControllerValue);
                break;
            }
        }
    }
private:
    OscMode m_mode = OscMode::OPNA;
    OpnaCore m_opnaCore;
    OpnCore m_opnCore;
    OplCore m_oplCore;
    Opl3Core m_opl3Core;
    OpmCore  m_opmCore;
    Opzx3Core m_opzx3Core;
    SsgCore m_ssgCore;
    WavetableCore m_wtCore;
    RhythmCore m_rhythmCore;
    AdpcmCore m_adpcmCore;
};
