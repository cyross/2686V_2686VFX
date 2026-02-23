#pragma once

#include <JuceHeader.h>
#include "Mode.h"
#include "SynthParams.h"
#include "../OPN/OpnCore.h"
#include "../OPNA/OpnaCore.h"
#include "../OPL/OplCore.h"
#include "../OPL3/Opl3Core.h"
#include "../OPM/OpmCore.h"
#include "../OPZX3/Opzx3Core.h"
#include "../SSG/SSGCore.h"
#include "../WT/WtCore.h"
#include "../RHYTHM/RhythmCore.h"
#include "../ADPCM/AdpcmCore.h"

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

    void setParameters(const SynthParams& params);

    AdpcmCore* getAdpcmCore() { return &m_adpcmCore; }
    RhythmCore* getRhythmCore() { return &m_rhythmCore; }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }

    void startNote(int midiNote, float velocity, juce::SynthesiserSound*, int) override;

    void stopNote(float, bool allowTailOff) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setCurrentPlaybackSampleRate(double newRate) override;

    // ピッチベンド
    void pitchWheelMoved(int newPitchWheelValue) override;

    // コントローラー (CC)
    void controllerMoved(int controllerNumber, int newControllerValue) override;
private:
    OscMode m_mode = OscMode::OPNA;
    OpnaCore m_opnaCore;
    OpnCore m_opnCore;
    OplCore m_oplCore;
    Opl3Core m_opl3Core;
    OpmCore  m_opmCore;
    Opzx3Core m_opzx3Core;
    SsgCore m_ssgCore;
    WtCore m_wtCore;
    RhythmCore m_rhythmCore;
    AdpcmCore m_adpcmCore;
};
