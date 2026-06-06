#pragma once

#include <JuceHeader.h>

#include "./SynthMode.h"
#include "./SynthParams.h"
#include "./SynthCore.h"

#include "../../Synth/Opna/SynthOpna.h"
#include "../../Synth/Opn/SynthOpn.h"
#include "../../Synth/Opl/SynthOpl.h"
#include "../../Synth/Opl3/SynthOpl3.h"
#include "../../Synth/Opm/SynthOpm.h"
#include "../../Synth/Opzx7/SynthOpzx7.h"
#include "../../Synth/Ssg/SynthSsg.h"
#include "../../Synth/Wavetable/SynthWt.h"
#include "../../Synth/Rhythm/SynthRhythm.h"
#include "../../Synth/Adpcm/SynthAdpcm.h"
#include "../../Synth/Beep/SynthBeep.h"
#include "../../Advanced/Curve/AdvancedCurve.h"

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

    void prepare(double sampleRate);
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

    void setOpzx7PcmBuffer(int opIndex, const std::vector<float>* pcmData); 

    void setOpzx7WtBuffer(int opIndex, const std::vector<float>* wtData);

    void setCurveCore(CurveCore* p_curveCore);

    std::map<OscMode, SynthCore *> coreMap;

    // ユニゾン・ハーモニー用
    void setUnisonParams(int index, int total, float detune, float spread) 
    {
        coreMap[m_mode]->setUnisonParams(index, total, detune, spread);
    }
private:
    OscMode m_mode = OscMode::OPNA;
    OpnaCore m_opnaCore;
    OpnCore m_opnCore;
    OplCore m_oplCore;
    Opl3Core m_opl3Core;
    OpmCore  m_opmCore;
    Opzx7Core m_opzx7Core;
    SsgCore m_ssgCore;
    WtCore m_wtCore;
    RhythmCore m_rhythmCore;
    AdpcmCore m_adpcmCore;
    BeepCore m_beepCore;
};
