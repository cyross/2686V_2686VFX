// [SimpleEffects.h]
#pragma once
#include <JuceHeader.h>
#include <array>
#include <algorithm>

#include "../synth/SynthParams.h"
#include "../synth/Pcm.h"

// ======================================================
// エフェクトの種類を定義 (マジックナンバー撲滅の鍵)
// ======================================================
enum class FxType
{
    Tremolo = 0,
    Vibrato,
    ModernBitCrusher,
    RetroBitCrusher,
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
    void reset();
private:
    juce::Reverb reverb;
};

// ======================================================
// 6. Retro Bit Crusher (ADPCM / PCM Decimator)
// ======================================================
class FxRBC : public FxCore
{
public:
    void prepare(double sampleRate) override;
    void setParameters(int rateIdx, int mode, float mix);
    void process(juce::AudioBuffer<float>& buffer) override;
private:
    double fs = 44100.0;
    int bitsMode = 6;
    int stepSize = 1;
    float maxVal = 0.0f;

    int counter[2] = { 0, 0 };
    float heldSample[2] = { 0.0f, 0.0f };
    Ym2608AdpcmCodec codec[2];
};

// --- Effect Manager ---
class EffectChain
{
public:
    EffectChain();
    void prepare(double sampleRate);
    void setTremoloParams(float rate, float depth, float mix);
    void setVibratoParams(float rate, float depth, float mix);
    void setModernBitCrusherParams(float rate, float bits, float mix);
    void setRetroBitCrusherParams(int mode, int rate, float mix);
    void setDelayParams(float time, float fb, float mix);
    void setReverbParams(float size, float damp, float width, float mix);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();
    void setBypasses(bool t, bool v, bool mc, bool rc, bool d, bool r);
    void updateOrder(const std::array<int, NumEffects>& newOrders);
private:
    // 各エフェクトオブジェクト
    FxTremolo tremolo;
    FxVibrato vibrato;
    FxMBC modernBitCrusher;
    FxDelay delay;
    FxReverb reverb;
    FxRBC retroBitCrusher;

    // エフェクトの適応順
    int fxSize = 6;
    std::array<int, 6> orderIndex{ { 0, 1, 2, 3, 4, 5 } };
    std::vector<FxCore*> fxs{ &tremolo, &vibrato, &modernBitCrusher, &retroBitCrusher, &delay, &reverb };

    std::array<FxCore*, NumEffects> fxMap;
    std::array<FxCore*, NumEffects> processChain;
};
