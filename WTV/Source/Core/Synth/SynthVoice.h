#pragma once

#include <JuceHeader.h>

#include "./SynthMode.h"
#include "./SynthParams.h"
#include "Shared/Core/Synth/SynthCore.h"

#include "Shared/Synth/Wavetable/SynthWt.h"
#include "Shared/Synth/Wt2/SynthWt2.h"
#include "Shared/Synth/WtPlus/SynthWtPlus.h"
#include "Shared/Advanced/Curve/AdvancedCurve.h"

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
    // 次に鳴らす音の TAP AMP / PITCH SEQUENCER の値。段はシンセ (RetroSynthesiser) が
    // 打鍵ごとに 1 回だけ取り出し、ユニゾンの声すべてへ同じ値を配る。ボイスごとに
    // 取り出すと、ユニゾンの声の数だけ段が進んでしまう。
    void setTapValues(float gain, float cents)
    {
        m_tapGain = gain;
        m_tapCents = cents;
    }

    // 次に鳴らす音の TAP PARAM SEQUENCER の組。TAP AMP / PITCH と同じく、シンセが打鍵ごとに
    // 1 回だけ取り出してユニゾンの声すべてへ配る
    void setTapParams(const ParamMod::TapStep& step) { m_tapParam = step; }

    // PARAM ENV / TAP PARAM SEQUENCER を始める。押したとき (startNote) と、モノで
    // 鳴り残っているコアを直に叩くとき (レガート) に呼ぶ
    void beginParamMod(bool isLegato);

    void setArpParams(bool enable, int freq, bool smooth)
    {
        m_arpEnable = enable;
        m_arpFreqBase = freq;
        m_arpFreq = freq;
        m_arpSmooth = smooth;
        m_arpPhase = 0.0;
    }
private:
    // 現在のサンプルがこのボイスのスロットかどうかを 0.0〜1.0 のゲインで返す
    float getArpGain() const;

    bool m_arpEnable = false;
    int m_arpFreq = 60;

    // パラメータの ARP FREQ。PARAM ENV / TAP PARAM SEQUENCER が置き換えていないときは m_arpFreq もこれ
    int m_arpFreqBase = 60;

    // TAP PARAM SEQUENCER の、打鍵 1 回ぶんの組
    ParamMod::TapStep m_tapParam;

    // PARAM ENV を進める間隔 (サンプル)
    static constexpr int paramModChunk = 16;

    void applyArpOverride();
    void advanceParamMod(SynthCore* core, int n);
    void renderCore(SynthCore* core, float* outR, float* outL, int startSample, int numSamples, bool& isActive);
    bool m_arpSmooth = true;
    double m_arpPhase = 0.0; // スロット単位の位相 (1.0 で次のボイスへ)

    OscMode m_mode = OscMode::WAVETABLE;

    // TAP AMP / PITCH SEQUENCER の、打鍵 1 回ぶんの値 (割合とセント)。
    // 音量は鳴らしているあいだ出力へ掛け、音程は押したときの周波数へ掛ける。
    float m_tapGain = 1.0f;
    float m_tapCents = 0.0f;

    // 出力へ倍率を掛けるときの、描く前の値の控え
    std::vector<float> m_tapKeepL;
    std::vector<float> m_tapKeepR;
    WtCore m_wtCore;
    WtPlusCore m_wtPlusCore;
    Wt2Core m_wt2Core;
};
