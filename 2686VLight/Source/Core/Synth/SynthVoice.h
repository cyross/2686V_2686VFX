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
#include "../../Synth/Wt2/SynthWt2.h"
#include "../../Synth/WtPlus/SynthWtPlus.h"
#include "../../Synth/Rhythm/SynthRhythm.h"
#include "../../Synth/Adpcm/SynthAdpcm.h"
#include "../../Synth/Beep/SynthBeep.h"

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

    void setOpzx7PcmBuffer(int opIndex, std::vector<float>* pcmData); 

    void setOpzx7WtBuffer(int opIndex, std::vector<float>* wtData);

    // WT+ の波形メモリはプロセッサが所有する。ボイスには参照だけを渡す。
    void setWtPlusWaveSlots(const WtPlusWaveSlots* slots);

    void setOpzx7Wt2Buffer(int opIndex, std::vector<float>* wtData);

    void clearOpzx7PcmBuffer(int opIndex);

    void clearOpzx7WtBuffer(int opIndex);

    void clearOpzx7Wt2Buffer(int opIndex);

    bool isPlaying();

    std::map<OscMode, SynthCore *> coreMap;

    // ユニゾン・ハーモニー用
    void setUnisonParams(int index, int total, float detune, float spread,
                         float paraDetune = 0.0f, float paraDistance = 0.0f)
    {
        coreMap[m_mode]->setUnisonParams(index, total, detune, spread, paraDetune, paraDistance);
    }

    // 疑似高速アルペジオ用
    // ユニゾンの各ボイスを arpFreq[Hz] のスロットで順番に鳴らす。
    // ボイスは全て voiceUnison() の同一ループで発音されるため、
    // ここで位相をリセットしておけば各ボイスのスロットが自動的に揃う。
    void setArpParams(bool enable, int freq, bool smooth)
    {
        m_arpEnable = enable;
        m_arpFreq = freq;
        m_arpSmooth = smooth;
        m_arpPhase = 0.0;
    }
private:
    // 現在のサンプルがこのボイスのスロットかどうかを 0.0〜1.0 のゲインで返す
    float getArpGain() const;

    bool m_arpEnable = false;
    int m_arpFreq = 60;
    bool m_arpSmooth = true;
    double m_arpPhase = 0.0; // スロット単位の位相 (1.0 で次のボイスへ)

    OscMode m_mode = OscMode::OPNA;
    OpnaCore m_opnaCore;
    OpnCore m_opnCore;
    OplCore m_oplCore;
    Opl3Core m_opl3Core;
    OpmCore  m_opmCore;
    Opzx7Core m_opzx7Core;
    SsgCore m_ssgCore;
    WtCore m_wtCore;
    WtPlusCore m_wtPlusCore;
    Wt2Core m_wt2Core;
    RhythmCore m_rhythmCore;
    AdpcmCore m_adpcmCore;
    BeepCore m_beepCore;
};
