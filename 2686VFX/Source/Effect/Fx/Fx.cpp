#include <vector>
#include <algorithm>

#include "./Fx.h"

#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Shared/Core/Synth/SynthHelpers.h"
#include "Shared/Generator/Pcm/Helper/GenPcmHelper.h"

namespace
{
    // 間引いたものを読み戻すときの埋め方。音源の QUALITY と同じ式を使う。
    // 音源では波形を読む場所に直接書いてあり、まとまった形になっていない
    // ので、ここでは効果の中だけで持つ。
    //
    // hist は間引いた直近 4 つ。frac は hist[1] と hist[2] の間のどこか。
    float interpolateHistory(const float* hist, float frac, int mode)
    {
        float s_m1 = hist[0];
        float s_0 = hist[1];
        float s_1 = hist[2];
        float s_2 = hist[3];

        switch (mode)
        {
        case 0: // 補間なし。エイリアスノイズが出るオールドスクール
            return (frac < 0.5f) ? s_0 : s_1;

        case 2: // Gaussian/Cubic。SFC 風の丸みのある補間
        {
            float c0 = s_0;
            float c1 = 0.5f * (s_1 - s_m1);
            float c2 = s_m1 - 2.5f * s_0 + 2.0f * s_1 - 0.5f * s_2;
            float c3 = 0.5f * (s_2 - s_m1) + 1.5f * (s_0 - s_1);

            return ((c3 * frac + c2) * frac + c1) * frac + c0;
        }

        case 3: // Zero-Order Hold。最も粗い Lo-Fi サンプラー風
            return s_0;

        case 4: // Cosine。Linear と Cubic の中間的な滑らかさ
        {
            float mu2 = (1.0f - std::cos(frac * juce::MathConstants<float>::pi)) / 2.0f;

            return s_0 * (1.0f - mu2) + s_1 * mu2;
        }

        case 5: // B-Spline。強烈なローパス効果でこもり感を強調
        {
            float c0 = (s_m1 + 4.0f * s_0 + s_1) / 6.0f;
            float c1 = (s_1 - s_m1) / 2.0f;
            float c2 = (s_m1 - 2.0f * s_0 + s_1) / 2.0f;
            float c3 = (s_2 - 3.0f * s_1 + 3.0f * s_0 - s_m1) / 6.0f;

            return ((c3 * frac + c2) * frac + c1) * frac + c0;
        }

        case 6: // Lagrange。4 点補間で Cubic とは異なる倍音特性
        {
            float l_m1 = -frac * (frac - 1.0f) * (frac - 2.0f) / 6.0f;
            float l_0 = (frac + 1.0f) * (frac - 1.0f) * (frac - 2.0f) / 2.0f;
            float l_1 = -(frac + 1.0f) * frac * (frac - 2.0f) / 2.0f;
            float l_2 = (frac + 1.0f) * frac * (frac - 1.0f) / 6.0f;

            return s_m1 * l_m1 + s_0 * l_0 + s_1 * l_1 + s_2 * l_2;
        }

        default: // 1: Linear。線形補間
            return s_0 * (1.0f - frac) + s_1 * frac;
        }
    }
}

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

void FxVibrato::clear()
{
    delayBuffer.clear();
    writePos = 0;
    phase = 0.0;
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

void FxMBC::clear()
{
    for (int i = 0; i < 2; ++i) {
        counter[i] = 0;
        heldSample[i] = 0.0f;
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
    delayTimeSamples = std::max(1, (int)(fs * timeMs / 1000.0));
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
            else if (nextVal < -2.0f) nextVal = -2.0f;
            else if (std::abs(nextVal) < 1e-5f) nextVal = 0.0f;

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

void FxDelay::clear()
{
    delayBuffer.clear();
    writePos = 0;
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

void FxReverb::clear()
{
    reverb.reset();
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

    // 標本化周波数が変わると係数も変わる。次の setParameters で作り直させる。
    coefsReady = false;
}

void FxFilter::setParameters(float type, float freq, float q, float mix)
{
    wetLevel = mix;

    // 係数は type / freq / q だけで決まる。この関数は毎ブロック呼ばれるが、
    // 値が変わっていなければ何もしなくてよい。setCutoffFrequency と
    // setResonance は中で毎回 tan を計算するため、素通しできると効く。
    if (coefsReady && (int)type == currentType && freq == currentFreq && q == currentQ) {
        return;
    }

    currentType = (int)type;
    currentFreq = freq;
    currentQ = q;
    coefsReady = true;

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

// --- Filter ---
void FxFilter::clear()
{
    filterL.reset();
    filterR.reset();
}

void FxEq3b::prepare(double sampleRate)
{
    fs = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = 4096;
    spec.numChannels = 1;

    lowShelfL.prepare(spec); lowShelfR.prepare(spec);
    midBellL.prepare(spec);  midBellR.prepare(spec);
    highShelfL.prepare(spec); highShelfR.prepare(spec);

    // fs が変わると係数も変わる。次の setParameters で作り直させる。
    coefsReady = false;

    clear();
}

void FxEq3b::setParameters(float lowGainDb, float midFreq, float midGainDb, float highGainDb, float mix)
{
    wetLevel = mix; // EQの場合、Mixは全体のDry/Wetバランスとして使用

    // 係数は下の 4 つと fs だけで決まる。この関数は毎ブロック呼ばれるが、
    // makeLowShelf などは中で new するので、そのたびにオーディオスレッドで
    // ヒープの確保と解放が起きていた。値が同じなら素通しする。
    if (coefsReady
        && lowGainDb == lastLowGainDb
        && midFreq == lastMidFreq
        && midGainDb == lastMidGainDb
        && highGainDb == lastHighGainDb) {
        return;
    }

    lastLowGainDb = lowGainDb;
    lastMidFreq = midFreq;
    lastMidGainDb = midGainDb;
    lastHighGainDb = highGainDb;
    coefsReady = true;

    // Q値（帯域幅）は固定値（0.707 など）にしておくとシンプルです
    float q = 0.707f;
    float midQ = 1.0f; // Midは少し狭めにすると使いやすい

    // Low Shelf (固定周波数 例: 200Hz)
    auto lowCoefs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(fs, 200.0f, q, juce::Decibels::decibelsToGain(lowGainDb));
    lowShelfL.coefficients = lowCoefs;
    lowShelfR.coefficients = lowCoefs;

    // Mid Bell (可変周波数)
    auto midCoefs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(fs, midFreq, midQ, juce::Decibels::decibelsToGain(midGainDb));
    midBellL.coefficients = midCoefs;
    midBellR.coefficients = midCoefs;

    // High Shelf (固定周波数 例: 5000Hz)
    auto highCoefs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(fs, 5000.0f, q, juce::Decibels::decibelsToGain(highGainDb));
    highShelfL.coefficients = highCoefs;
    highShelfR.coefficients = highCoefs;
}

void FxEq3b::process(juce::AudioBuffer<float>& buffer)
{
    if (wetLevel < 0.01f) return;

    int numSamples = buffer.getNumSamples();
    float* outL = buffer.getWritePointer(0);
    float* outR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : outL;

    for (int i = 0; i < numSamples; ++i)
    {
        float dryL = outL[i];
        float dryR = outR[i];

        // L channel: Low -> Mid -> High と直列に処理
        float wetL = lowShelfL.processSample(dryL);
        wetL = midBellL.processSample(wetL);
        wetL = highShelfL.processSample(wetL);

        // R channel
        float wetR = dryR;
        if (buffer.getNumChannels() > 1) {
            wetR = lowShelfR.processSample(dryR);
            wetR = midBellR.processSample(wetR);
            wetR = highShelfR.processSample(wetR);
        }

        // Mix (Dry/Wet)
        outL[i] = (dryL * (1.0f - wetLevel)) + (wetL * wetLevel);
        if (buffer.getNumChannels() > 1) {
            outR[i] = (dryR * (1.0f - wetLevel)) + (wetR * wetLevel);
        }
    }
}

void FxEq3b::clear()
{
    lowShelfL.reset(); lowShelfR.reset();
    midBellL.reset();  midBellR.reset();
    highShelfL.reset(); highShelfR.reset();
}

// ======================================================
// 8. SFC Echo (SFC(SPC-700 Like)-style Delay with 8-tap FIR filter)
// ======================================================
void FxSfcEcho::prepare(double sampleRate)
{
    fs = sampleRate;
    int maxSamples = (int)(fs * maxDelayMs / 1000.0);
    delayBuffer.setSize(2, maxSamples);
    delayBuffer.clear();
    writePos = 0;
}

void FxSfcEcho::setParameters(float timeMs, float feedback, float mix)
{
    // FIR係数なしで呼ばれた場合のデフォルト的挙動 (タップ0のみ出力)
    setParameters(timeMs, feedback, mix, { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f });
}

void FxSfcEcho::setParameters(float timeMs, float feedback, float mix, const std::array<float, 8>& firCoefs)
{
    // Timeが0の時に過去の最大バッファを読まないよう、最低1サンプルの遅延を保証する
    delayTimeSamples = std::max(1, (int)(fs * timeMs / 1000.0));

    fb = juce::jlimit(-0.95f, 0.95f, feedback); // SFCエコーは位相反転のフィードバックも可能
    wetLevel = juce::jlimit(0.0f, 1.0f, mix);

    // FIR係数の合計絶対値を計算し、安全なレベルに正規化する
    float sum = 0.0f;
    for (float v : firCoefs) sum += std::abs(v);

    if (sum > 1.0f) {
        // 合計が1.0を超えるとフィードバックが発振するため、比率を保ったまま縮小する
        for (int i = 0; i < 8; ++i) {
            firCoefficients[i] = firCoefs[i] / sum;
        }
    }
    else {
        firCoefficients = firCoefs;
    }
}

void FxSfcEcho::process(juce::AudioBuffer<float>& buffer)
{
    if (wetLevel < 0.01f) return;

    const int numSamples = buffer.getNumSamples();
    const int delayBufLen = delayBuffer.getNumSamples();
    const int numChannels = std::min(buffer.getNumChannels(), delayBuffer.getNumChannels());
    const int startWritePos = writePos;

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* channelData = buffer.getWritePointer(ch);
        auto* delayData = delayBuffer.getWritePointer(ch);

        int currentWritePos = startWritePos;

        for (int i = 0; i < numSamples; ++i)
        {
            float dry = channelData[i];

            // 1. FIRフィルタによる過去データの畳み込み計算
            float filteredWet = 0.0f;

            // SFCのエコーFIRは、最新のディレイ音から順に過去8つのサンプルを使用する
            // 忠実さを増すならタップ間隔を fs/32000 などのオフセットにする手もありますが、
            // 近似としては1サンプルごとの畳み込みでも「こもった共鳴音」を十分に再現できます。
            for (int tap = 0; tap < 8; ++tap) {
                int rPos = currentWritePos - delayTimeSamples - tap;
                while (rPos < 0) rPos += delayBufLen;
                while (rPos >= delayBufLen) rPos -= delayBufLen;

                filteredWet += delayData[rPos] * firCoefficients[tap];
            }

            // 2. ディレイバッファへの書き込み (Feedbackあり)
            float nextVal = dry + (filteredWet * fb);

            // 簡易リミッター（過激なフィードバック時の発振をある程度防ぐ）
            if (nextVal > 2.0f) nextVal = 2.0f;
            else if (nextVal < -2.0f) nextVal = -2.0f;
            else if (std::abs(nextVal) < 1e-5f) nextVal = 0.0f; // 音量が十分に小さくなったら完全に0にする

            delayData[currentWritePos] = nextVal;

            // 3. ミックスして出力
            channelData[i] = (dry * (1.0f - wetLevel)) + (filteredWet * wetLevel);

            currentWritePos++;
            if (currentWritePos >= delayBufLen) currentWritePos = 0;
        }
    }

    writePos += numSamples;
    while (writePos >= delayBufLen) writePos -= delayBufLen;
}

void FxSfcEcho::clear()
{
    delayBuffer.clear();
    writePos = 0;
}


// --- EffectChain への組み込み ---
// ======================================================
// 2686V PCM Bit Crusher
// ======================================================
void FxPcm::prepare(double sampleRate)
{
    hostRate = sampleRate;

    setPcmParameters(bitIndex, rateIndex, interpMode, wetLevel);
    updatePreFilter();

    clear();
}

void FxPcm::clear()
{
    for (auto& ch : history)
    {
        for (auto& v : ch) v = 0.0f;
    }

    phase[0] = 0.0;
    phase[1] = 0.0;

    clearPreFilter();
}

void FxPcm::clearPreFilter()
{
    for (auto& ch : preZ)
    {
        for (auto& stage : ch)
        {
            stage[0] = 0.0f;
            stage[1] = 0.0f;
        }
    }
}

void FxPcm::setPcmParameters(int newBitIndex, int newRateIndex, int newInterpMode, float mix)
{
    const double oldStep = stepPerSample;

    bitIndex = newBitIndex;
    rateIndex = newRateIndex;
    interpMode = newInterpMode;
    wetLevel = mix;

    double targetRate = getTargetRate(rateIndex);

    // 出力 1 つにつき、間引いた側がどれだけ進むか。
    // 元より高いレートを選んだときは 1 を超え、間引かれなくなる。
    stepPerSample = (hostRate > 0.0) ? (targetRate / hostRate) : 1.0;

    // 前段の切れ目はレートで決まる。変わったときだけ組み直す。
    if (stepPerSample != oldStep) updatePreFilter();
}

void FxPcm::setResample(bool resample)
{
    // 毎ブロック呼ばれる。変わったときだけ組み直す。
    if (resample == nrResample) return;

    nrResample = resample;

    updatePreFilter();
}

void FxPcm::updatePreFilter()
{
    constexpr double pi = 3.14159265358979323846;

    // 音源の resampleClean と同じく、目的のレートのナイキストの 0.92 倍で切る。
    // 出力のナイキストに近すぎると係数が崩れるうえ、そこまでは間引いても
    // 折り返さないので、そのときは掛けない。
    const double nyquist = hostRate * 0.5;
    const double fc = getTargetRate(rateIndex) * 0.5 * 0.92;

    const bool wasActive = preActive;

    preActive = nrResample && hostRate > 0.0 && fc < nyquist * 0.9;

    // 切れていたあいだの古い値から始めると、入れた瞬間に音が跳ねる
    if (preActive && !wasActive) clearPreFilter();

    if (preActive)
    {
        // 8 次 Butterworth の各段の Q (1 / (2 cos((2k - 1)π / 16)))
        static constexpr double qs[preStages] = { 0.50979558, 0.60134489, 0.89997622, 2.56291545 };

        const double w0 = 2.0 * pi * fc / hostRate;
        const double cw = std::cos(w0);
        const double sw = std::sin(w0);

        for (int s = 0; s < preStages; ++s)
        {
            // RBJ の低域通過
            const double alpha = sw / (2.0 * qs[s]);
            const double a0 = 1.0 + alpha;

            auto& bq = pre[(size_t)s];
            bq.b0 = (float)(((1.0 - cw) * 0.5) / a0);
            bq.b1 = (float)((1.0 - cw) / a0);
            bq.b2 = bq.b0;
            bq.a1 = (float)((-2.0 * cw) / a0);
            bq.a2 = (float)((1.0 - alpha) / a0);
        }
    }
}

void FxPcm::process(juce::AudioBuffer<float>& buffer)
{
    int channels = juce::jmin(buffer.getNumChannels(), 2);
    int samples = buffer.getNumSamples();

    for (int ch = 0; ch < channels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto* hist = history[ch];
        auto* z = preZ[ch];

        for (int i = 0; i < samples; ++i)
        {
            float dry = data[i];

            // 間引く前に、目的のレートで表せない高い成分を落とす。
            // 落とさないと、間引いたときに低いところへ折り返して濁る。
            float in = dry;

            if (preActive)
            {
                for (int s = 0; s < preStages; ++s)
                {
                    const auto& bq = pre[(size_t)s];

                    // 直接形 II 転置
                    const float y = bq.b0 * in + z[s][0];

                    z[s][0] = bq.b1 * in - bq.a1 * y + z[s][1];
                    z[s][1] = bq.b2 * in - bq.a2 * y;

                    in = y;
                }
            }

            phase[ch] += stepPerSample;

            // 進んだぶんだけ新しい値を取り込む。取り込むときに
            // ビットを落とすので、間引きと丸めが同じ場所で起きる。
            while (phase[ch] >= 1.0)
            {
                phase[ch] -= 1.0;

                hist[0] = hist[1];
                hist[1] = hist[2];
                hist[2] = hist[3];
                hist[3] = GenPcmHelper::bitReduction(in, bitIndex);
            }

            float wet = interpolateHistory(hist, (float)phase[ch], interpMode);

            data[i] = dry * (1.0f - wetLevel) + wet * wetLevel;
        }
    }
}

// ======================================================
// Noise Reduction
// ======================================================
void FxNr::prepare(double sampleRate)
{
    hostRate = sampleRate;

    update();
    clear();
}

void FxNr::clear()
{
    reducer[0].reset();
    reducer[1].reset();
}

void FxNr::setNrParameters(int newRateIndex, bool newRateBypass, bool newGate, float newGateDb, int newLpfLevel, float mix)
{
    wetLevel = mix;

    // 毎ブロック呼ばれる。変わったときだけ組み直す。
    if (newRateIndex == rateIndex && newRateBypass == rateBypass && newGate == gate
        && newGateDb == gateDb && newLpfLevel == lpfLevel) return;

    rateIndex = newRateIndex;
    rateBypass = newRateBypass;
    gate = newGate;
    gateDb = newGateDb;
    lpfLevel = newLpfLevel;

    update();
}

void FxNr::update()
{
    // 帯域の上端は RATE のナイキスト。出力のナイキストに対する割合で渡す。
    // RATE を通さないときは、出力のナイキストそのもの (割合 1) にする。
    const double contentRatio = (rateBypass || hostRate <= 0.0) ? 1.0 : (getTargetRate(rateIndex) / hostRate);

    for (auto& r : reducer)
    {
        const bool wasActive = r.isActive();

        r.setup(hostRate, contentRatio, gate, gateDb, lpfLevel);

        // 切れていたあいだの古い値から始めると、入れた瞬間に音が跳ねる
        if (!wasActive && r.isActive()) r.reset();
    }
}

void FxNr::process(juce::AudioBuffer<float>& buffer)
{
    int channels = juce::jmin(buffer.getNumChannels(), 2);
    int samples = buffer.getNumSamples();

    for (int ch = 0; ch < channels; ++ch)
    {
        auto& nr = reducer[ch];

        // GATE も LPF も切れていれば素通し
        if (!nr.isActive()) continue;

        auto* data = buffer.getWritePointer(ch);

        for (int i = 0; i < samples; ++i)
        {
            const float dry = data[i];
            const float wet = nr.process(dry);

            data[i] = dry * (1.0f - wetLevel) + wet * wetLevel;
        }
    }
}

EffectChain::EffectChain()
{
    fxMap[static_cast<int>(FxType::Filter)] = &filter;
    fxMap[static_cast<int>(FxType::Eq3b)] = &eq3b;
    fxMap[static_cast<int>(FxType::Tremolo)] = &tremolo;
    fxMap[static_cast<int>(FxType::Vibrato)] = &vibrato;
    fxMap[static_cast<int>(FxType::ModernBitCrusher)] = &modernBitCrusher;
    fxMap[static_cast<int>(FxType::Delay)] = &delay;
    fxMap[static_cast<int>(FxType::Reverb)] = &reverb;
    fxMap[static_cast<int>(FxType::SfcEcho)] = &sfcEcho;
    fxMap[static_cast<int>(FxType::PcmBitCrusher)] = &pcmBitCrusher;
    fxMap[static_cast<int>(FxType::NoiseReduction)] = &noiseReduction;

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
void EffectChain::setDelayParams(float time, float fb, float mix) { delay.setParameters(time, fb, mix); }
void EffectChain::setReverbParams(float size, float damp, float width, float mix) { reverb.setParameters(size, damp, width, mix); }
void EffectChain::setFilterParams(int type, float freq, float q, float mix) { filter.setParameters((float)type, freq, q, mix); }
void EffectChain::setEq3bParams(float lowGainDb, float midFreq, float midGainDb, float highGainDb, float mix) { eq3b.setParameters(lowGainDb, midFreq, midGainDb, highGainDb, mix); }
void EffectChain::setSfcEchoParams(float time, float fb, float mix, const std::array<float, 8>& firCoefs) { sfcEcho.setParameters(time, fb, mix, firCoefs); }
void EffectChain::setPcmBitCrusherParams(int bit, int rate, int interp, float mix) { pcmBitCrusher.setPcmParameters(bit, rate, interp, mix); }
void EffectChain::setPcmBitCrusherResample(bool resample) { pcmBitCrusher.setResample(resample); }
void EffectChain::setNoiseReductionParams(int rate, bool rateBypass, bool gate, float gateDb, int lpf, float mix) { noiseReduction.setNrParameters(rate, rateBypass, gate, gateDb, lpf, mix); }

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

// バイパス状態のセット
void EffectChain::setBypasses(bool fl, bool e3, bool t, bool v, bool mc, bool d, bool r, bool sfc, bool pcm, bool nr)
{
    filter.setBypass(fl);
    eq3b.setBypass(e3);
    tremolo.setBypass(t);
    vibrato.setBypass(v);
    modernBitCrusher.setBypass(mc);
    delay.setBypass(d);
    reverb.setBypass(r);
    sfcEcho.setBypass(sfc);
    pcmBitCrusher.setBypass(pcm);
    noiseReduction.setBypass(nr);
}

// 順番更新
void EffectChain::updateOrder(const std::vector<int>& newOrders)
{
    // 渡された数が足りないことがある。効果の数はプラグインごとに違い、
    // 他のプラグインで書いた順番のファイルは短いことがあるため。
    int count = std::min((int)newOrders.size(), NumEffects);

    for (int i = 0; i < count; ++i)
    {
        if (newOrders[i] >= 0 && newOrders[i] < NumEffects)
        {
            orderIndex[i] = newOrders[i];
            processChain[i] = fxMap[newOrders[i]];
        }
    }
}

std::vector<int> EffectChain::getOrder() {
    std::vector<int> order;

    for (auto o : orderIndex) {
        order.push_back(o);
    }

    return order;
}

int EffectChain::getEffectsNumber() {
    return NumEffects;
}

// バッファクリア
void EffectChain::clear()
{
    for (auto* fx : fxMap)
    {
        fx->clear();
    }
}
