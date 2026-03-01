// [SimpleEffects.h]
#pragma once
#include <JuceHeader.h>
#include <array>
#include <algorithm>
#include <vector>

#include "../synth/SynthParams.h"
#include "../synth/Pcm.h"

// ======================================================
// エフェクトの種類を定義 (マジックナンバー撲滅の鍵)
// ======================================================
enum class FxType
{
    Filter = 0, // フィルターは最初にかけるのが一般的
    RetroLfo,
    Tremolo,
    Vibrato,
    ModernBitCrusher,
    RetroBitCrusher,
    Delay,
    Reverb,
    SoftClipper, // ソフトクリッパーは最後にかけるのが一般的
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
// Retro LFO (Global LFO Emulator)
// ======================================================
class FxRetroLfo : public FxCore
{
public:
    void prepare(double sampleRate) override;
    void setParameters(int wave, float freq, float ams, float pms, float amd, float pmd, float mix);
    void process(juce::AudioBuffer<float>& buffer) override;
private:
    juce::AudioBuffer<float> delayBuffer;
    double fs = 44100.0;
    int writePos = 0;
    double lfoPhase = 0.0;
    int currentWave = 2;
    float currentFreq = 5.0f;
    float currentAms = 0.0f;
    float currentPms = 0.0f;
    float currentAmd = 0.0f;
    float currentPmd = 0.0f;

    unsigned int lfsr = 0x1FFFF;
    float noisePhase = 0.0f;
    float currentNoise = 0.0f;

    // PM用のスムーズLFO変数
    float pmSmoothLfo = 0.0f;
    float currentPmDelay = 0.0f;
    float amSmoothLfo = 0.0f;
    float pmNoiseInt = 0.0f;
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

// ======================================================
// 7. Filter (State Variable Filter)
// ======================================================
class FxFilter : public FxCore
{
public:
    void prepare(double sampleRate) override;
    // mode=Type(0:LPF, 1:HPF, 2:BPF, 3:BSF), rate=Freq, mix=Q, wetLevel=Mix (引数を流用)
    void setParameters(float type, float freq, float q, float mix);
    void process(juce::AudioBuffer<float>& buffer) override;
private:
    juce::dsp::StateVariableTPTFilter<float> filterL;
    juce::dsp::StateVariableTPTFilter<float> filterR;
    int currentType = 0;
    float currentFreq = 20000.0f;
    float currentQ = 0.707f;
};

// ======================================================
// 8. Soft Clipper (tanh Saturation)
// ======================================================
class FxSoftClipper : public FxCore
{
public:
    void prepare(double sampleRate) override {} // 何もしない
    void setParameters(float dummy1, float dummy2, float mix) override;
    void process(juce::AudioBuffer<float>& buffer) override;
};

// --- Effect Manager ---
class EffectChain
{
public:
    EffectChain();
    void setRetroLfoParams(int wave, float freq, float ams, float pms, float amd, float pmd, float mix);
    void setTremoloParams(float rate, float depth, float mix);
    void setVibratoParams(float rate, float depth, float mix);
    void setModernBitCrusherParams(float rate, float bits, float mix);
    void setRetroBitCrusherParams(int mode, int rate, float mix);
    void setDelayParams(float time, float fb, float mix);
    void setReverbParams(float size, float damp, float width, float mix);
    void setFilterParams(int type, float freq, float q, float mix);
    void setSoftClipperParams(float mix);

    void prepare(double sampleRate);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();
    void setBypasses(bool fl, bool rlfo, bool t, bool v, bool mc, bool rc, bool d, bool r, bool sc);
    void updateOrder(const std::array<int, NumEffects>& newOrders);
private:
    // 各エフェクトオブジェクト
    FxFilter filter;
    FxRetroLfo retroLfo;
    FxTremolo tremolo;
    FxVibrato vibrato;
    FxMBC modernBitCrusher;
    FxDelay delay;
    FxReverb reverb;
    FxRBC retroBitCrusher;
    FxSoftClipper softClipper;

    // エフェクトの適応順
    int fxSize = 9;
    std::array<int, 9> orderIndex{ { 0, 1, 2, 3, 4, 5, 6, 7, 8 } };
    std::vector<FxCore*> fxs{ &filter, &retroLfo, &tremolo, &vibrato, &modernBitCrusher, &retroBitCrusher, &delay, &reverb, &softClipper };

    std::array<FxCore*, NumEffects> fxMap;
    std::array<FxCore*, NumEffects> processChain;
};
