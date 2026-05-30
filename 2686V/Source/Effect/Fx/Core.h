// [SimpleEffects.h]
#pragma once
#include <JuceHeader.h>
#include <array>
#include <algorithm>
#include <vector>

#include "../../Core/Synth/SynthParams.h"
#include "../../Generator/Pcm/Core.h"

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
    Count // Total Count
};

// 型変換の手間を省くための定数
static constexpr int NumEffects = static_cast<int>(FxType::Count);

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
};

// --- Effect Manager ---
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

    void prepare(double sampleRate);
    void process(juce::AudioBuffer<float>& buffer);
    void setBypasses(bool fl, bool e3, bool t, bool v, bool mc, bool d, bool r);
    void updateOrder(const std::array<int, NumEffects>& newOrders);
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

    // エフェクトの適応順
    int fxSize = 7;
    std::array<int, 7> orderIndex{ { 0, 1, 2, 3, 4, 5, 6 } };
    std::vector<FxCore*> fxs{ &filter, &eq3b, &tremolo, &vibrato, &modernBitCrusher, &delay, &reverb };

    std::array<FxCore*, NumEffects> fxMap;
    std::array<FxCore*, NumEffects> processChain;
};
