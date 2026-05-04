#pragma once

#include <JuceHeader.h>

#include "./Mode.h"
#include "./SynthParams.h"
#include "./SynthCore.h"

#include "../../Synth/Opn/OpnCore.h"
#include "../../Synth/Opna/OpnaCore.h"
#include "../../Synth/Opl/OplCore.h"
#include "../../Synth/Opl3/Opl3Core.h"
#include "../../Synth/Opm/OpmCore.h"
#include "../../Synth/Opzx3/Opzx3Core.h"
#include "../../Synth/Ssg/SSGCore.h"
#include "../../Synth/Wavetable/WtCore.h"
#include "../../Synth/Rhythm/RhythmCore.h"
#include "../../Synth/Adpcm/AdpcmCore.h"
#include "../../Synth/Beep/BeepCore.h"

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();

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

    void setOpzx3PcmBuffer(int opIndex, const std::vector<float>* pcmData); 

    std::map<OscMode, SynthCore *> coreMap;
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
    BeepCore m_beepCore;
};
