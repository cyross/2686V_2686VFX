#include "./Core.h"

void OpnOperator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_phase = m_params.phaseOffset;
    m_ssgPhase = 0.0;
    m_noteNumber = noteNumber;
    //m_currentLevel = 0.0f;

    m_lfoDelayCounter = m_params.lfoSyncDelay / 1000.0f;

    m_lfoCycleCount = 0;

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    // --- 通常の波形（Sineなど）の場合 ---
    if (m_params.fixedMode) {
        baseFreq = m_params.fixedFreq;
    }

    // 基本周波数にデチューン成分を加算
    float detunedBaseFreq = baseFreq + baseFreq * dtScales[m_params.detune & 7];

    // Multi & Detune
    float mul = (m_params.multiple == 0) ? 0.5f : (float)m_params.multiple;

    // Final Frequency = (Base + DT1) * MUL
    float finalFreq = detunedBaseFreq * mul;

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

    // TL Calculation
    float tlGain = 0.0f;
    if (m_params.regEnable)
    {
        // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
        // Opn/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
        float attenuationDb = m_params.rtl * 0.75f;
        tlGain = std::pow(10.0f, -attenuationDb / 20.0f);
    }
    else
    {
        // 従来モード: 0.0〜1.0 を 0dB〜-96dB にマッピング
        if (m_params.totalLevel < 0.99f) {
            float attenuationDb = m_params.totalLevel * 96.0f;
            tlGain = std::pow(10.0f, -attenuationDb / 20.0f);
        }
        else {
            tlGain = 0.0f;
        }
    }

    float kslAttenuation = 1.0f;
    m_targetLevel = velocity * tlGain * kslAttenuation;
    m_state = State::Attack;

    m_fb1 = 0.0f; m_fb2 = 0.0f;

    updateIncrementsWithKeyScale();

    m_currentReleaseDec = m_releaseDec;
}

void OpnOperator::getSample(float& output, float modulator, float amLfoVal, float pmLfoVal,
    bool globalPm, bool globalAm, float globalPms, float globalAms, float globalPmd, float globalAmd, float modWheel)
{
    if (m_state == State::Idle) { output = 0.0f; return; }

    updateEnvelopeState();
    float envVal = m_currentLevel;

    // Sync Delay 更新
    if (m_lfoDelayCounter > 0.0f) {
        m_lfoDelayCounter -= 1.0f / (float)m_hostSampleRate;
        if (m_lfoDelayCounter < 0.0f) m_lfoDelayCounter = 0.0f;
    }

    // ========================================================
    // 1. Amplitude Modulation (Tremolo / Wah) の計算
    // ========================================================
    float totalAmpMod = 1.0f;

    if (globalAm) {
        // グローバルAMS (-127〜127) と OP毎のAMD (0〜15) を合成
        float amSign = (globalAmd < 0) ? -1.0f : 1.0f;
        float amDepthNorm = (std::abs((float)globalAmd) / 127.0f) * ((float)m_params.ams / 15.0f);

        // amLfoVal は 0.0 ~ 1.0。AMSがマイナスの場合は波形を反転(1.0 - x)させる
        float unipolarLfo = (amSign > 0.0f) ? amLfoVal : (1.0f - amLfoVal);

        // 最大減衰量を 95.25dB (TLレジスタの最大値 127 * 0.75dB) として計算
        float attenuationDb = unipolarLfo * amDepthNorm * 95.25f;

        // デシベルをリニアな音量倍率に変換
        totalAmpMod = std::pow(10.0f, -attenuationDb / 20.0f);
    }

    envVal *= totalAmpMod;

    // ========================================================
    // 2. Pitch Modulation (Vibrato) の計算
    // ========================================================
    float lfoPitchMod = 1.0f;

    if (globalPm) {
        // グローバルPMS (-127〜127) と グローバルPMD (0〜15) を合成
        // PMSがマイナスならそのまま掛け算結果もマイナスになり、位相が反転する
        float pmDepthNorm = ((float)globalPmd / 127.0f) * (globalPms / 15.0f);

        // 最大で ±1オクターブ (1200セント) の揺れ幅と定義する
        float pitchCentDeviation = pmLfoVal * pmDepthNorm * 1200.0f;

        // セント値を周波数の倍率に変換 (2 ^ (cent / 1200))
        lfoPitchMod = std::pow(2.0f, pitchCentDeviation / 1200.0f);
    }

    // モジュレーションホイール分を足し込む (最大200セントの揺れ)
    float wheelCent = pmLfoVal * (modWheel * 200.0f);

    lfoPitchMod *= std::pow(2.0f, wheelCent / 1200.0f);

    // ========================================================
    // 3. 位相と波形の生成
    // ========================================================
    float feedbackMod = 0.0f;
    if (m_feedback > 0.0f) {
        float fbScale = std::pow(2.0f, m_feedback) / 64.0f;
        feedbackMod = (m_fb1 + m_fb2) * 0.5f * fbScale * juce::MathConstants<float>::pi;
    }

    float currentPhaseDelta = m_phaseDelta * m_pitchBendRatio * lfoPitchMod;

    // --------------------------------------------------------
    // PCM波形への過剰な位相変調を抑え、音量低下を防ぐスケーリング
    // --------------------------------------------------------
    float fmModIndex = 4.0f * juce::MathConstants<float>::pi;

    float modulatedPhase = m_phase + (modulator * fmModIndex) + feedbackMod;

    float rawWave = calcWaveform(modulatedPhase, m_params.waveSelect);

    output = rawWave * envVal * m_targetLevel;

    if (!m_isExternalFeedback) {
        m_fb2 = m_fb1;
        m_fb1 = output;
    }

    m_phase += currentPhaseDelta;
    if (m_phase >= 2.0 * juce::MathConstants<float>::pi) m_phase -= 2.0 * juce::MathConstants<float>::pi;
}

#include "../../Core/Const/PrValues.h"

// 実機(YM2151/2608)の挙動を模倣するため、定数加算ではなく周波数比例させます。
// これにより「キーによって周波数値が変わる（高音ほど変化Hzが大きい）」挙動になります。
// 値は実機の数値を参考に調整した近似値です。
// 0: 0
// 1: -0.45%
// 2: -0.25%
// 3: -0.1% (approx)
// 4: 0
// 5: +0.1% (approx)
// 6: +0.25%
// 7: +0.45%
const std::array<float, 8> OpnOperator::dtScales = { 0.0f, -0.0045f, -0.0025f, -0.001f, 0.0f, 0.001f, 0.0025f, 0.0045f };

// DT2 (OPM Coarse Detune)
// YM2151: 0=0, 1=+approx 1.414, 2=+approx 1.58, 3=+approx 1.73
// 0: x1.0
// 1: x1.41 (600 cent up)
// 2: x1.58 (780 cent up)
// 3: x1.78 (950 cent up)
const std::array<float, 4> OpnOperator::dt2Scales = { 1.0f, 1.414f, 1.581f, 1.781f };

const std::array<OpnOperator::SsgWaveCalculator, 16> OpnOperator::ssgWaveStrategies = { {
    [](double p) { // 00: normal
        return 1.0f;
    },
    [](double p) { // 01
        return 1.0f;
    },
    [](double p) { // 02: Saw Down
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return 1.0f - (float)subPos;
    },
    [](double p) { // 03
        return 1.0f;
    },
    [](double p) { // 04: Saw Down & Hold
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return (cycle == 0) ? (1.0f - (float)subPos) : 0.0f;
    },
    [](double p) { // 05
        return 1.0f;
    },
    [](double p) { // 06: Triangle
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return isEven ? (1.0f - (float)subPos) : (float)subPos;
    },
    [](double p) { // 07
        return 1.0f;
    },
    [](double p) { // 08: Alt Saw Down & Hold
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return (cycle == 0) ? (1.0f - (float)subPos) : 0.0f;
    },
    [](double p) { // 09: Saw Up
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return (float)subPos;
    },
    [](double p) { // 10
        return 1.0f;
    },
    [](double p) { // 11: Saw Up & Hopd
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return (cycle == 0) ? (float)subPos : 1.0f;
    },
    [](double p) { // 12
        return 1.0f;
    },
    [](double p) { // 13: Triangle & Invert
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return isEven ? (float)subPos : (1.0f - (float)subPos);
    },
    [](double p) { // 14
        return 1.0f;
    },
    [](double p) { // 15: Alt Saw Up & Hold
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return (cycle == 0) ? (float)subPos : 1.0f;
    },
} };

void OpnOperator::setParameters(const FmOpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = 1.0f;
    m_params.ssgEg = 0;
    m_params.waveSelect = 0;
}

void OpnOperator::noteOff()
{
    m_state = State::Release;

    m_currentReleaseDec = m_releaseDec;
}

float OpnOperator::calcWaveform(double phase, int wave)
{
    // 1. まず phase を 0.0 ～ 2π の範囲に丸め込む (ラジアン)
    float p = std::fmod((float)phase, 2.0f * juce::MathConstants<float>::pi);
    if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;

    // サイン波はラジアンで計算
    float s = std::sin(p);

    return s; // Default Sine
}

void OpnOperator::updateEnvelopeState()
{
    if (state == State::Attack) {
        currentLevel += attackInc;
        if (currentLevel >= 1.0f) { currentLevel = 1.0f; state = State::Decay; }
    }
    else if (state == State::Decay) {
        float limitLevel = params.sustain;

        if (currentLevel > limitLevel) {
            // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
            if (decayDec > 0.0f) {
                currentLevel -= decayDec;
                if (currentLevel <= limitLevel) {
                    currentLevel = limitLevel;
                    state = State::Sustain;
                }
            }
        }
        else {
            currentLevel = limitLevel;
            state = State::Sustain;
        }
    }
    else if (state == State::Sustain) {
        // SR(Sustain Rate / OPMではD2R) でゆっくり減衰する
        if (sustainRateDec > 0.0f) {
            currentLevel -= sustainRateDec;
            if (currentLevel <= 0.0f) { currentLevel = 0.0f; state = State::Idle; }
        }
    }
    else if (state == State::Release) {
        currentLevel -= currentReleaseDec;
        if (currentLevel <= 0.0f) { currentLevel = 0.0f; state = State::Idle; }
    }
}

void OpnOperator::updateIncrementsWithKeyScale()
{
    if (sampleRate <= 0.0) return;

    // ====================================================================
    // レジスタモード (RG-EN = ON) : 実機のアルゴリズムで増減量を計算
    // ====================================================================
    if (params.regEnable)
    {
        // 1. キースケールレート (KSR) の算出
        int ksrValue = 0;

        int octave = (noteNumber / 12) - 1;
        if (octave < 0) octave = 0;
        if (octave > 7) octave = 7;

        int noteOffset = noteNumber % 12;
        int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
        ksrValue = keyRate >> (3 - std::clamp(params.keyScale, 0, 3));

        // 2. レジスタ値から実効レート(0~63)を算出し、インクリメントに変換する関数
        // isRRフラグを追加し、RRの時だけスケールを調整する
        auto calcRegRate = [&](int regVal, bool isRR) -> float {
            // RR以外のRate0は停止（サステイン維持など）。
            if (regVal == 0 && !isRR) return 0.0f;

            int baseRate = regVal;

            // OPL系（全て4bit）、およびOPN系のRR（4bit）は、5bit(0-31)スケールに補正する
            if (isRR) {
                // 15の時に31になるように (val * 2 + 1)
                baseRate = (regVal * 2) + 1;

                // DAW向け安全装置: RRが0（baseRateが1）の場合でも、永遠に鳴り止まないのを防ぐため
                // 非常にゆっくり（約20秒）減衰して消えるようにする。
                if (baseRate <= 1) {
                    return 1.0f / (20.0f * (float)sampleRate);
                }
            }

            // 実効レート = 基本レート(0-31) * 2 + KSR (0-3)
            int effectiveRate = (baseRate * 2) + ksrValue;
            if (effectiveRate > 63) effectiveRate = 63;

            float timeInSeconds = 0.0f;
            if (effectiveRate >= 60) {
                // Rate 60以上はほぼ瞬時（1ミリ秒）
                timeInSeconds = 0.001f;
            }
            else {
                // レートが4下がるごとに時間が約2倍になる実機カーブの近似
                float powFactor = (60.0f - (float)effectiveRate) / 4.0f;
                timeInSeconds = 0.0015f * std::pow(2.0f, powFactor);
            }

            return 1.0f / (timeInSeconds * (float)sampleRate);
            };

        // 各レートの計算（第2引数に、それがRRかどうかのフラグを渡す）
        attackInc = calcRegRate(params.rar, false);
        decayDec = calcRegRate(params.rdr, false);
        sustainRateDec = (params.rsr == 0) ? 0.0f : calcRegRate(params.rsr, false);

        // Release Rate の計算時のみ isRR を true にする
        releaseDec = calcRegRate(params.rrr, true);

        susReleaseDec = calcRegRate(5, true);

        // 3. サステインレベル (SL) の計算
        if (params.rsl == 15) {
            params.sustain = 0.0f; // SL=15 は一気に0まで落ちる
        }
        else {
            // SL 1ステップにつき 3dB の減衰 (Opn/OPL共通)
            float slDb = params.rsl * 3.0f;
            params.sustain = std::pow(10.0f, -slDb / 20.0f);
        }
    }
    // ====================================================================
    // 従来モード (RG-EN = OFF) : 既存の秒数ベースの計算
    // ====================================================================
    else
    {
        // KeyScaleによるスケーリング計算のバグを修正
        float rateScale = 1.0f;
        if (params.keyScale > 0) {
            // m_noteNumber(通常0〜127) を使ってスケールを計算するが、
            // 係数を小さくして急激な倍率変化を防ぐ
            float noteFactor = (float)(noteNumber) / 127.0f;
            rateScale = 1.0f + ((float)params.keyScale * noteFactor * 0.5f);
        }

        // param(秒数) に対してスケーリングを行う。
        // param が 0 の時（0.001fの時）に正しく 1ms になるように計算式を修正。
        auto calcInc = [&](float paramInSeconds) -> float {
            // スケールを適用した実際の秒数（短くなる）
            float scaledSeconds = paramInSeconds / rateScale;
            // 最低でも 1ms (0.001秒) は保証する
            float finalSeconds = std::max(0.001f, scaledSeconds);
            // サンプルレートから「1サンプルあたりに進む量」を返す
            return 1.0f / (finalSeconds * (float)sampleRate);
            };

        attackInc = calcInc(params.attack);
        decayDec = calcInc(params.decay);
        releaseDec = calcInc(params.release);

        susReleaseDec = calcInc(1.5f);

        if (params.sustainRate <= 0.001f) {
            sustainRateDec = 0.0f;
        }
        else {
            // Sustain Rate は値(0.0~1.0)が小さいほど遅い（長い）という特殊な仕様
            float srTime = 5.0f * (1.0f - params.sustainRate);
            sustainRateDec = calcInc(srTime);
        }

        // 従来モードのサステインレベルはそのまま適用する
        params.sustain = m_params.sustain;
    }
}
