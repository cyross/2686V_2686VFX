#include "FxCore.h"
#include "../core/GuiConstants.h"
#include "../core/LabelConstants.h"
#include "../core/OpConstants.h"

void FxTremolo::prepare(double sampleRate)
{
    fs = sampleRate;
    phase = 0.0;
}

void FxTremolo::setParameters(float rate, float depth, float mix)
{
    // rate: 0.1Hz - 20Hz
    freq = rate;
    // depth: 0.0 - 1.0
    dep = depth;
    wetLevel = mix;
}

void FxTremolo::process(juce::AudioBuffer<float>& buffer)
{
    if (wetLevel < 0.01f) return;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Phase increment per sample
    double phaseInc = (juce::MathConstants<double>::twoPi * freq) / fs;

    for (int i = 0; i < numSamples; ++i) {
        // LFO: -1.0 ~ 1.0 -> 0.0 ~ 1.0
        float lfo = (std::sin(phase) + 1.0f) * 0.5f;

        // Gain calculation:
        // Depth=0 -> Gain=1
        // Depth=1 -> Gain=0~1 (Oscillate)
        float gain = (1.0f - dep) + (dep * lfo);

        // Update phase
        phase += phaseInc;
        if (phase >= juce::MathConstants<double>::twoPi)
            phase -= juce::MathConstants<double>::twoPi;

        for (int ch = 0; ch < numChannels; ++ch) {
            float* data = buffer.getWritePointer(ch);
            float dry = data[i];
            float wet = dry * gain;
            data[i] = (dry * (1.0f - wetLevel)) + (wet * wetLevel);
        }
    }
}

void FxVibrato::prepare(double sampleRate)
{
    fs = sampleRate;
    // 20ms buffer is enough for vibrato
    int bufferSize = (int)(sampleRate * 0.02) + 1;
    delayBuffer.setSize(2, bufferSize);
    delayBuffer.clear();
    writePos = 0;
    phase = 0.0;
}

void FxVibrato::setParameters(float rate, float depth, float mix)
{
    freq = rate;
    dep = depth; // 0.0 - 1.0
    wetLevel = mix;
}

void FxVibrato::process(juce::AudioBuffer<float>& buffer)
{
    // Vibrato always runs to update write pointer, even if Mix=0
    // (to avoid click when mix increases), but we can optimize output mix.

    const int numSamples = buffer.getNumSamples();
    const int numChannels = std::min(buffer.getNumChannels(), delayBuffer.getNumChannels());
    const int delayBufLen = delayBuffer.getNumSamples();

    double phaseInc = (juce::MathConstants<double>::twoPi * freq) / fs;

    // Base delay (center point of swing) ~ 5ms
    float baseDelay = fs * 0.005f;
    // Swing amount ~ 2ms
    float swing = fs * 0.002f * dep;

    int startWritePos = writePos;

    for (int ch = 0; ch < numChannels; ++ch) {
        auto* chData = buffer.getWritePointer(ch);
        auto* dData = delayBuffer.getWritePointer(ch);
        int currentWritePos = startWritePos;
        double currentPhase = phase;

        for (int i = 0; i < numSamples; ++i) {
            float dry = chData[i];

            // Write to delay buffer
            dData[currentWritePos] = dry;

            // Calculate Read Position
            // LFO modulates delay time -> Pitch shift
            // Use slight phase offset for Stereo width if desired (optional)
            float lfo = std::sin(currentPhase);
            if (ch == 1) lfo = std::sin(currentPhase + 0.5); // Stereo offset

            float currentDelay = baseDelay + (lfo * swing);

            // Linear Interpolation
            float readPos = (float)currentWritePos - currentDelay;
            while (readPos < 0) readPos += delayBufLen;
            while (readPos >= delayBufLen) readPos -= delayBufLen;

            int indexA = (int)readPos;
            int indexB = (indexA + 1) % delayBufLen;
            float frac = readPos - indexA;

            float wet = dData[indexA] * (1.0f - frac) + dData[indexB] * frac;

            // Output
            if (wetLevel > 0.0f) {
                chData[i] = (dry * (1.0f - wetLevel)) + (wet * wetLevel);
            }

            // Increment
            currentPhase += phaseInc;
            if (currentPhase >= juce::MathConstants<double>::twoPi)
                currentPhase -= juce::MathConstants<double>::twoPi;

            currentWritePos++;
            if (currentWritePos >= delayBufLen) currentWritePos = 0;
        }
    }

    // Update global state
    writePos += numSamples;
    while (writePos >= delayBufLen) writePos -= delayBufLen;

    phase += phaseInc * numSamples;
    while (phase >= juce::MathConstants<double>::twoPi)
        phase -= juce::MathConstants<double>::twoPi;
}

void FxMBC::prepare(double sampleRate)
{
    // 状態のリセット
    for (int i = 0; i < 2; ++i) {
        counter[i] = 0;
        heldSample[i] = 0.0f;
    }
}

void FxMBC::setParameters(float rateReduction, float bitDepth, float mix)
{
    // rateReduction: 1.0 = 原音, 20.0 = 1/20のレート
    stepSize = (int)juce::jmax(1.0f, rateReduction);

    // bitDepth: 4.0 ~ 24.0
    // 量子化ステップ数を計算 (例: 4bit -> 16段階)
    quantizeStep = std::pow(2.0f, bitDepth);

    wetLevel = mix;
}

void FxMBC::process(juce::AudioBuffer<float>& buffer)
{
    if (wetLevel < 0.01f && stepSize == 1) return;

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);

        // チャンネルごとの状態維持
        int& cnt = counter[ch];
        float& hold = heldSample[ch];

        for (int i = 0; i < numSamples; ++i)
        {
            float dry = data[i];
            float processed = dry;

            // 1. Downsampling (Sample & Hold)
            if (cnt >= stepSize)
            {
                cnt = 0;
                hold = dry; // 新しいサンプルを取得
            }

            // 現在のホールド音を使用
            processed = hold;
            cnt++; // カウンタを進める

            // 2. Bit Reduction (Quantization)
            // 値を整数の階段状にする処理
            if (quantizeStep < 65536.0f) // 16bit未満なら処理
            {
                processed = std::floor(processed * quantizeStep) / quantizeStep;
            }

            // Mix (Dry/Wet)
            data[i] = (dry * (1.0f - wetLevel)) + (processed * wetLevel);
        }
    }
}

void FxDelay::prepare(double sampleRate)
{
    fs = sampleRate;
    int maxSamples = (int)(fs * maxDelayMs / 1000.0);
    delayBuffer.setSize(2, maxSamples);
    delayBuffer.clear();
    writePos = 0;
}

void FxDelay::setParameters(float timeMs, float feedback, float mix)
{
    // Smooth parameter changes could be added here
    delayTimeSamples = (int)(fs * timeMs / 1000.0);
    fb = juce::jlimit(0.0f, 0.95f, feedback);
    wetLevel = juce::jlimit(0.0f, 1.0f, mix);
}

void FxDelay::process(juce::AudioBuffer<float>& buffer)
{
    if (wetLevel < 0.01f) return; // Skip if mix is 0

    const int numSamples = buffer.getNumSamples();
    const int delayBufLen = delayBuffer.getNumSamples();
    const int numChannels = std::min(buffer.getNumChannels(), delayBuffer.getNumChannels());
    const int startWritePos = writePos;

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* channelData = buffer.getWritePointer(ch);
        auto* delayData = delayBuffer.getWritePointer(ch);

        // チャンネルごとのループでは、一時的な位置変数を使う
        // これにより、Lchの処理が終わってもRchは正しい位置からスタートできる
        int currentWritePos = startWritePos;

        for (int i = 0; i < numSamples; ++i)
        {
            float dry = channelData[i];

            // 読み込み位置の計算 (循環バッファ)
            int rPos = currentWritePos - delayTimeSamples;
            while (rPos < 0) rPos += delayBufLen;
            while (rPos >= delayBufLen) rPos -= delayBufLen;

            // ディレイ音の読み出し
            float wet = delayData[rPos];

            // バッファへの書き込み (Feedbackあり)
            // 発振防止のため tanh 等を入れるのも良いですが、まずは単純なクリップ防止のみ
            float nextVal = dry + (wet * fb);

            // 簡易リミッター (過大入力時のバリバリ音防止)
            if (nextVal > 2.0f) nextVal = 2.0f;
            if (nextVal < -2.0f) nextVal = -2.0f;

            delayData[currentWritePos] = nextVal;

            // ミックスして出力
            channelData[i] = (dry * (1.0f - wetLevel)) + (wet * wetLevel);

            // 一時ポインタを進める
            currentWritePos++;
            if (currentWritePos >= delayBufLen) currentWritePos = 0;
        }
    }

    // 全チャンネルの処理が終わってから、メインの書き込み位置を更新する
    writePos += numSamples;
    while (writePos >= delayBufLen) writePos -= delayBufLen;
}

void FxReverb::prepare(double sampleRate)
{
    reverb.setSampleRate(sampleRate);
}

void FxReverb::setParameters(float size, float damp, float width, float mix)
{
    juce::Reverb::Parameters p;
    p.roomSize = size;
    p.damping = damp;
    p.width = width;
    p.wetLevel = mix;
    p.dryLevel = 1.0f - mix;
    p.freezeMode = 0;
    reverb.setParameters(p);
}

void FxReverb::process(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() == 2) reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
    else reverb.processMono(buffer.getWritePointer(0), buffer.getNumSamples());
}

void FxReverb::reset()
{
    reverb.reset();
}

void FxRBC::prepare(double sampleRate)
{
    fs = sampleRate;

    for (int i = 0; i < 2; ++i) {
        counter[i] = 0;
        heldSample[i] = 0.0f;
        codec[i].reset();
    }
}

void FxRBC::setParameters(int rateIdx, int mode, float mix)
{
    bitsMode = mode;
    wetLevel = mix;

    double targetRate = 16000.0;
    switch (rateIdx) {
    case 1: targetRate = 96000.0; break;
    case 2: targetRate = 55500.0; break;
    case 3: targetRate = 48000.0; break;
    case 4: targetRate = 44100.0; break;
    case 5: targetRate = 22050.0; break;
    case 6: targetRate = 16000.0; break;
    case 7: targetRate = 8000.0;  break;
    default: targetRate = 16000.0; break;
    }

    if (targetRate >= fs) {
        stepSize = 1;
    }
    else {
        stepSize = (int)juce::jmax(1.0, fs / targetRate);
    }

    switch (bitsMode) {
    case 1: maxVal = 0.0f; break;        // 32bit Float
    case 2: maxVal = 8388607.0f; break;  // 24bit
    case 3: maxVal = 32767.0f; break;    // 16bit
    case 4: maxVal = 127.0f; break;      // 8bit
    case 5: maxVal = 15.0f; break;       // 5bit
    case 6: maxVal = 7.0f; break;        // 4bit Linear
    case 7: maxVal = 0.0f; break;        // 4bit ADPCM
    default: maxVal = 0.0f; break;
    }
}

void FxRBC::process(juce::AudioBuffer<float>& buffer)
{
    if (wetLevel < 0.01f && stepSize == 1) return;

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        int& cnt = counter[ch];
        float& hold = heldSample[ch];

        for (int i = 0; i < numSamples; ++i)
        {
            float dry = data[i];
            float processed = dry;

            if (cnt >= stepSize)
            {
                cnt = 0;
                float sampleToHold = dry;

                if (bitsMode == 7)
                {
                    // ADPCM Emulation
                    int16_t input = (int16_t)(std::clamp(sampleToHold, -1.0f, 1.0f) * 32767.0f);
                    uint8_t enc = codec[ch].encode(input);
                    sampleToHold = codec[ch].decode(enc) / 32768.0f;
                }
                else if (maxVal > 0.0f)
                {
                    // PCM Bit Reduction
                    sampleToHold = std::floor(sampleToHold * maxVal) / maxVal;
                }
                hold = sampleToHold;
            }

            processed = hold;
            cnt++;
            data[i] = (dry * (1.0f - wetLevel)) + (processed * wetLevel);
        }
    }
}

// --- Filter ---
void FxFilter::prepare(double sampleRate)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = 4096;
    spec.numChannels = 1; // 1chずつ処理するため1で初期化

    filterL.prepare(spec);
    filterR.prepare(spec);
    filterL.reset();
    filterR.reset();
}

void FxFilter::setParameters(float type, float freq, float q, float mix)
{
    currentType = (int)type;
    currentFreq = freq;
    currentQ = q;
    wetLevel = mix;

    using FType = juce::dsp::StateVariableTPTFilterType;
    FType fType = FType::lowpass;
    if (currentType == 2) fType = FType::highpass;
    else if (currentType == 3) fType = FType::bandpass;

    filterL.setType(fType);
    filterR.setType(fType);
    filterL.setCutoffFrequency(currentFreq);
    filterR.setCutoffFrequency(currentFreq);
    filterL.setResonance(currentQ);
    filterR.setResonance(currentQ);
}

void FxFilter::process(juce::AudioBuffer<float>& buffer)
{
    if (wetLevel < 0.01f) return;

    int numSamples = buffer.getNumSamples();
    float* outL = buffer.getWritePointer(0);
    float* outR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : outL;

    for (int i = 0; i < numSamples; ++i)
    {
        float dryL = outL[i];
        float wetL = filterL.processSample(0, dryL);
        outL[i] = (dryL * (1.0f - wetLevel)) + (wetL * wetLevel);

        if (buffer.getNumChannels() > 1) {
            float dryR = outR[i];
            float wetR = filterR.processSample(0, dryR);
            outR[i] = (dryR * (1.0f - wetLevel)) + (wetR * wetLevel);
        }
    }
}

// --- Soft Clipper ---
void FxSoftClipper::setParameters(float dummy1, float dummy2, float mix)
{
    wetLevel = mix;
}

void FxSoftClipper::process(juce::AudioBuffer<float>& buffer)
{
    if (wetLevel < 0.01f) return;

    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* data = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            float dry = data[i];
            float wet = std::tanh(dry); // アナログクリッピング
            data[i] = (dry * (1.0f - wetLevel)) + (wet * wetLevel);
        }
    }
}

// --- EffectChain への組み込み ---
EffectChain::EffectChain()
{
    fxMap[static_cast<int>(FxType::Filter)] = &filter;
    fxMap[static_cast<int>(FxType::Tremolo)] = &tremolo;
    fxMap[static_cast<int>(FxType::Vibrato)] = &vibrato;
    fxMap[static_cast<int>(FxType::ModernBitCrusher)] = &modernBitCrusher;
    fxMap[static_cast<int>(FxType::RetroBitCrusher)] = &retroBitCrusher;
    fxMap[static_cast<int>(FxType::Delay)] = &delay;
    fxMap[static_cast<int>(FxType::Reverb)] = &reverb;
    fxMap[static_cast<int>(FxType::SoftClipper)] = &softClipper;

    // 処理順序配列の初期化 (デフォルトは定義順)
    processChain = fxMap;
}

void EffectChain::prepare(double sampleRate)
{
    for (auto* fx : fxMap) fx->prepare(sampleRate);
}

// Parameters
// Delay: Time(ms), Feedback(0-1), Mix(0-1)
// Reverb: Size(0-1), Damping(0-1), Mix(0-1)
void EffectChain::setTremoloParams(float rate, float depth, float mix) { tremolo.setParameters(rate, depth, mix); }
void EffectChain::setVibratoParams(float rate, float depth, float mix) { vibrato.setParameters(rate, depth, mix); }
void EffectChain::setModernBitCrusherParams(float rate, float bits, float mix) { modernBitCrusher.setParameters(rate, bits, mix); }
void EffectChain::setRetroBitCrusherParams(int mode, int rate, float mix) { retroBitCrusher.setParameters(mode, rate, mix); }
void EffectChain::setDelayParams(float time, float fb, float mix) { delay.setParameters(time, fb, mix); }
void EffectChain::setReverbParams(float size, float damp, float width, float mix) { reverb.setParameters(size, damp, width, mix); }
void EffectChain::setFilterParams(int type, float freq, float q, float mix) { filter.setParameters((float)type, freq, q, mix); }
void EffectChain::setSoftClipperParams(float mix) { softClipper.setParameters(0.0f, 0.0f, mix); }

void EffectChain::process(juce::AudioBuffer<float>& buffer)
{
    for (auto* fx : processChain)
    {
        if (!fx->isBypass())
        {
            fx->process(buffer);
        }
    }
}

void EffectChain::reset()
{
    // 簡易リセット
    modernBitCrusher.prepare(44100);
    retroBitCrusher.prepare(44100);
    reverb.reset();
}

// バイパス状態のセット
void EffectChain::setBypasses(bool fl, bool t, bool v, bool mc, bool rc, bool d, bool r, bool sc)
{
    filter.setBypass(fl);
    tremolo.setBypass(t);
    vibrato.setBypass(v);
    modernBitCrusher.setBypass(mc);
    retroBitCrusher.setBypass(rc);
    delay.setBypass(d);
    reverb.setBypass(r);
    softClipper.setBypass(sc);
}

// 順番更新
void EffectChain::updateOrder(const std::array<int, NumEffects>& newOrders)
{
    for (int i = 0; i < NumEffects; ++i)
    {
        if (newOrders[i] >= 0 && newOrders[i] < NumEffects)
        {
            processChain[i] = fxMap[newOrders[i]];
        }
    }
}
