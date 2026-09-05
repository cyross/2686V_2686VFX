#pragma once

#include <JuceHeader.h>

#include "./SynthMode.h"
#include "./SynthParams.h"
#include "./SynthCore.h"

#include "../../Synth/Wavetable/SynthWt.h"
#include "../../Synth/Wt2/SynthWt2.h"
#include "../../Synth/WtPlus/SynthWtPlus.h"
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



    // WT+ の波形メモリはプロセッサが所有する。ボイスには参照だけを渡す。
    void setWtPlusWaveSlots(const WtPlusWaveSlots* slots);





    void setCurveCore(CurveCore* p_curveCore);

    bool isPlaying();

    // 音源はモード番号で引くだけなので、木ではなく素の配列にする。
    // 1 サンプルごとに引く場所があり、std::map では毎回木をたどることになる。
    std::array<SynthCore*, (size_t)OscMode::Count> coreMap{};

    // いま鳴らすコア。モードは列挙の範囲に収まっている前提だが、
    // 外れていたら先頭 (既定の音源) を返して落ちないようにする。
    SynthCore* activeCore() const
    {
        const size_t i = (size_t)m_mode;

        return (i < coreMap.size() && coreMap[i] != nullptr) ? coreMap[i] : coreMap[0];
    }

    // ユニゾン・ハーモニー用
    void setUnisonParams(int index, int total, float detune, float spread,
                         float paraDetune = 0.0f, float paraDistance = 0.0f)
    {
        activeCore()->setUnisonParams(index, total, detune, spread, paraDetune, paraDistance);
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

    OscMode m_mode = OscMode::WAVETABLE;
    WtCore m_wtCore;
    WtPlusCore m_wtPlusCore;
    Wt2Core m_wt2Core;
};
