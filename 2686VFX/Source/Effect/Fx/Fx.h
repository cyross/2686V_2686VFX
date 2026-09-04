// [SimpleEffects.h]
#pragma once
#include <JuceHeader.h>
#include <array>
#include <algorithm>
#include <vector>

#include "../../Core/Synth/SynthParams.h"

// ======================================================
// エフェクトの種類を定義 (マジックナンバー撲滅の鍵)
// ======================================================
enum class FxType
{
    Filter = 0, // フィルターは最初にかけるのが一般的
    Eq3b,
    Tremolo,
    Vibrato,
    ModernBitCrusher,
    Delay,
    Reverb,
    SfcEcho,
    PcmBitCrusher,
    Count // Total Count
};

// 型変換の手間を省くための定数
static constexpr int NumEffects = static_cast<int>(FxType::Count);

// ファイルへ書くときの名前。
//
// 順番を番号で持つと、効果を足したときに位置がずれ、他のプラグインで
// 書いたファイルが別の効果へ化ける。名前なら、足しても消しても
// 取り違えない。画面に出す名前と違い、訳さないし変えないこと。
static inline const juce::String fxTypeNames[NumEffects] = {
    "filter",
    "eq3b",
    "tremolo",
    "vibrato",
    "modernBitCrusher",
    "delay",
    "reverb",
    "sfcEcho",
    "pcmBitCrusher",
};

static inline juce::String fxTypeName(int index)
{
    return (index >= 0 && index < NumEffects) ? fxTypeNames[index] : juce::String();
}

// 知らない名前は -1 を返す。他のプラグインにしかない効果のため。
static inline int fxTypeFromName(const juce::String& name)
{
    for (int i = 0; i < NumEffects; ++i)
    {
        if (fxTypeNames[i] == name) return i;
    }

    return -1;
}

class FxCore
{
public:
    virtual void prepare(double sampleRate){}
    virtual void setParameters(float mode, float rate, float mix){}
    virtual void process(juce::AudioBuffer<float>& buffer) {}
    virtual void setOrder(int newOrder){ order = newOrder; }
    virtual int getOrder(){ return order; }
    virtual void setBypass(bool bp) { bypass = bp; }
    virtual bool isBypass() { return bypass; }
    virtual void clear() {}
protected:
    bool bypass = false; // バイパス管理
    float wetLevel = 0.0f;
    int order = 1; // エフェクト実行順
};

// ======================================================
// 1. Tremolo (Amplitude Modulation)
// ======================================================
class FxTremolo : public FxCore
{
public:
    void prepare(double sampleRate) override;
    void setParameters(float rate, float depth, float mix) override;
    void process(juce::AudioBuffer<float>& buffer) override;
private:
    double fs = 44100.0;
    double phase = 0.0;
    float freq = 1.0f;
    float dep = 0.0f;
};

// ======================================================
// 2. Vibrato (Pitch Modulation)
// ======================================================
class FxVibrato : public FxCore
{
public:
    void prepare(double sampleRate) override;
    void setParameters(float rate, float depth, float mix) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void clear() override;
private:
    juce::AudioBuffer<float> delayBuffer;
    double fs = 44100.0;
    int writePos = 0;
    double phase = 0.0;
    float freq = 5.0f;
    float dep = 0.0f;
};

// ======================================================
// 3. Modern Bit Crusher (Modern Decimator)
// ======================================================
class FxMBC : public FxCore
{
public:
    void prepare(double sampleRate) override;
    void setParameters(float rateReduction, float bitDepth, float mix) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void clear() override;
private:
    int stepSize = 1;
    float quantizeStep = 65536.0f;

    // ステレオ用の状態保持
    int counter[2] = { 0, 0 };
    float heldSample[2] = { 0.0f, 0.0f };
};

// ======================================================
// 4. Stereo Delay
// ======================================================
class FxDelay : public FxCore
{
public:
    void prepare(double sampleRate) override;
    void setParameters(float timeMs, float feedback, float mix) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void clear() override;
private:
    juce::AudioBuffer<float> delayBuffer;
    double fs = 44100.0;
    int writePos = 0;
    int delayTimeSamples = 0;
    float fb = 0.0f;
    int maxDelayMs = 2000;
};

// ======================================================
// 5. Reverb
// ======================================================
class FxReverb : public FxCore
{
public:
    void prepare(double sampleRate) override;
    void setParameters(float size, float damp, float width, float mix);
    void process(juce::AudioBuffer<float>& buffer);
    void clear() override;
private:
    juce::Reverb reverb;
};

// ======================================================
// 6. Filter (State Variable Filter)
// ======================================================
class FxFilter : public FxCore
{
public:
    void prepare(double sampleRate) override;
    // mode=Type(0:LPF, 1:HPF, 2:BPF, 3:BSF), rate=Freq, mix=Q, wetLevel=Mix (引数を流用)
    void setParameters(float type, float freq, float q, float mix);
    void process(juce::AudioBuffer<float>& buffer) override;
    void clear() override;
private:
    juce::dsp::StateVariableTPTFilter<float> filterL;
    juce::dsp::StateVariableTPTFilter<float> filterR;
    int currentType = 0;
    float currentFreq = 20000.0f;
    float currentQ = 0.707f;
    // 上の 3 つで係数を作った後かどうか。prepare で寝かせて作り直させる。
    bool coefsReady = false;
};

// ======================================================
// 7. 3-Band EQ (Low Shelf, Mid Bell, High Shelf)
// ======================================================
class FxEq3b : public FxCore
{
public:
    void prepare(double sampleRate) override;

    // Low, Mid, High のゲイン(dB)と、Midの周波数を設定できるようにする
    void setParameters(float lowGainDb, float midFreq, float midGainDb, float highGainDb, float mix);

    void process(juce::AudioBuffer<float>& buffer) override;
    void clear() override;

private:
    double fs = 44100.0;

    // ステレオ用の IIR フィルター (L / R)
    using IIRFilter = juce::dsp::IIR::Filter<float>;

    IIRFilter lowShelfL, lowShelfR;
    IIRFilter midBellL, midBellR;
    IIRFilter highShelfL, highShelfR;

    // 係数を作ったときの値。同じ値で呼ばれたら作り直さない。
    bool coefsReady = false;
    float lastLowGainDb = 0.0f;
    float lastMidFreq = 0.0f;
    float lastMidGainDb = 0.0f;
    float lastHighGainDb = 0.0f;
};

// ======================================================
// 8. SFC Echo (SFC(SPC-700 Like)-style Delay with 8-tap FIR filter)
// ======================================================
class FxSfcEcho : public FxCore
{
public:
    void prepare(double sampleRate) override;

    // timeMs: 0 - 240ms 程度
    // feedback: -1.0 to 1.0 (SFCは位相反転の負のフィードバックも可能)
    // mix: 0.0 to 1.0 (wet level)
    // firCoefs: 8つのFIR係数 (通常 -1.0 〜 1.0)
    void setParameters(float timeMs, float feedback, float mix, const std::array<float, 8>& firCoefs);
    void setParameters(float timeMs, float feedback, float mix) override; // 互換用 (係数デフォルト)

    void process(juce::AudioBuffer<float>& buffer) override;
    void clear() override;
private:
    juce::AudioBuffer<float> delayBuffer;
    double fs = 44100.0;
    int writePos = 0;
    int delayTimeSamples = 0;
    float fb = 0.0f;
    int maxDelayMs = 500; // SFCは240msが上限ですが、少し余裕を持たせます

    // デフォルトは単なるディレイ（タップ0のみ出力）
    std::array<float, 8> firCoefficients = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
};

// --- Effect Manager ---
// ======================================================
// 9. 2686V PCM Bit Crusher (実機のレートとビットで落とす)
// ======================================================
// 音源の QUALITY と同じ刻みを使う。指定のレートまで間引いてから
// 指定のビット数へ丸め、読み戻すときに選んだ補間で埋める。
//
// 間引いたものを補間するには前後の値が要る。流れてくる音では先の値が
// まだ来ていないので、間引いた 2 つぶんだけ遅らせて出す。16kHz なら
// 0.1 ミリ秒ほどで、耳では分からない。
//
// ビットの一覧のうち ADPCM などの圧縮は入れていない。あれは曲の頭から
// 順に符号化するもので、塊ごとに切ると継ぎ目で音が飛ぶ。
class FxPcm : public FxCore
{
public:
    void prepare(double sampleRate) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void clear() override;

    void setPcmParameters(int bitIndex, int rateIndex, int interpMode, float mix);
private:
    // 間引いた直近 4 つ。補間はこの 4 点を使う。
    float history[2][4] = { { 0.0f } };

    double hostRate = 44100.0;
    double stepPerSample = 1.0;
    double phase[2] = { 0.0, 0.0 };

    int bitIndex = 12;
    int rateIndex = 9;
    int interpMode = 1;
};

class EffectChain
{
public:
    EffectChain();
    void setTremoloParams(float rate, float depth, float mix);
    void setVibratoParams(float rate, float depth, float mix);
    void setModernBitCrusherParams(float rate, float bits, float mix);
    void setDelayParams(float time, float fb, float mix);
    void setReverbParams(float size, float damp, float width, float mix);
    void setFilterParams(int type, float freq, float q, float mix);
    void setEq3bParams(float lowGainDb, float midFreq, float midGainDb, float highGainDb, float mix);
    void setSfcEchoParams(float time, float fb, float mix, const std::array<float, 8>& firCoefs);
    void setPcmBitCrusherParams(int bit, int rate, int interp, float mix);

    void prepare(double sampleRate);
    void process(juce::AudioBuffer<float>& buffer);
    void setBypasses(bool fl, bool e3, bool t, bool v, bool mc, bool d, bool r, bool sfc, bool pcm);
    void updateOrder(const std::vector<int>& newOrders);
    std::vector<int> getOrder();
    int getEffectsNumber();
    void clear();
private:
    // 各エフェクトオブジェクト
    FxFilter filter;
    FxEq3b eq3b;
    FxTremolo tremolo;
    FxVibrato vibrato;
    FxMBC modernBitCrusher;
    FxDelay delay;
    FxReverb reverb;
    FxSfcEcho sfcEcho;
    FxPcm pcmBitCrusher;

    // エフェクトの適応順
    std::array<int, NumEffects> orderIndex{ { 0, 1, 2, 3, 4, 5, 6, 7, 8 } };
    std::vector<FxCore*> fxs{ &filter, &eq3b, &tremolo, &vibrato, &modernBitCrusher, &delay, &reverb, &sfcEcho, &pcmBitCrusher };

    std::array<FxCore*, NumEffects> fxMap;
    std::array<FxCore*, NumEffects> processChain;
};
