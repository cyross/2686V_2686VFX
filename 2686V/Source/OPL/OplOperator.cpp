#include "OplOperator.h"

#include <array>

const std::array<float, 4> OplOperator::dbPerOcts = { 0.0f, 1.5f, 3.0f, 6.0f };

namespace {
    // =================================================================
    // 波形ストラテジー配列の定義
    // (引数: ラジアン位相 p, 正規化位相 n, サイン波 s)
    // =================================================================
    using WaveCalculator = float(*)(float p, float n, float s);

    const std::array<WaveCalculator, 4> waveStrategies = { {
        // 0: Sine
        [](float p, float n, float s) { return s; },
        // 1: Half Sine
        [](float p, float n, float s) { return n < 0.5f ? s : 0.0f; },
        // 2: Abs Sine
        [](float p, float n, float s) { return std::abs(s); },
        // 3: Pulse Sine
        [](float p, float n, float s) { return n < 0.25f ? s : 0.0f; },
    } };
}

void OplOperator::setParameters(const FmOpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = 1.0f;
    m_params.ssgEg = 0;
    m_params.waveSelect = params.waveSelect;
}

void OplOperator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_phase = m_params.phaseOffset;
    m_ssgPhase = 0.0;
    m_noteNumber = noteNumber;
    //m_currentLevel = 0.0f;

    m_lfoDelayCounter = 0.0f;

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    // Multi & Detune
    float mul = (m_params.multiple == 0) ? 0.5f : (float)m_params.multiple;

    // Final Frequency = Base * MUL
    float finalFreq = baseFreq * mul;

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

    // TL Calculation
    float tlGain = 0.0f;
    if (m_params.regEnable)
    {
        // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
        // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
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
    if (m_params.keyScaleLevel > 0)
    {
        float octaveDiff = (float)(noteNumber - 48) / 12.0f;

        if (octaveDiff < 0) octaveDiff = 0;

        float totalDb = dbPerOcts[m_params.keyScaleLevel] * octaveDiff;

        kslAttenuation = std::pow(10.0f, -totalDb / 20.0f);
    }
    m_targetLevel = velocity * tlGain * kslAttenuation;
    m_state = State::Attack;

    m_fb1 = 0.0f; m_fb2 = 0.0f;

    updateIncrementsWithKeyScale();

    m_currentReleaseDec = m_releaseDec;
}

void OplOperator::noteOff()
{
    m_state = State::Release;

    // キーを離した瞬間、SUSがONならリリースレートをゆっくり(RR=5)にする
    if (m_params.susEnable) {
        m_currentReleaseDec = m_susReleaseDec;
    }
    else {
        m_currentReleaseDec = m_releaseDec;
    }
}
void OplOperator::getSample(float& output, float modulator, float lfoAmp, float lfoPitch)
{
    if (m_state == State::Idle) { output = 0.0f; return; }

    updateEnvelopeState();
    float envVal = m_currentLevel;

    // AM適用 (無条件。変調がない場合はコア側から 1.0 が渡ってくる)
    envVal *= lfoAmp;

    float feedbackMod = 0.0f;
    if (m_feedback > 0.0f) {
        float fbScale = std::pow(2.0f, m_feedback) / 64.0f;
        feedbackMod = (m_fb1 + m_fb2) * 0.5f * fbScale * juce::MathConstants<float>::pi;
    }

    // PM適用 (無条件。変調がない場合は 1.0 が渡ってくる)
    float currentPhaseDelta = m_phaseDelta * m_pitchBendRatio * lfoPitch;

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

float OplOperator::calcWaveform(double phase, int wave)
{
    // 1. まず phase を 0.0 ～ 2π の範囲に丸め込む (ラジアン)
    float p = std::fmod((float)phase, 2.0f * juce::MathConstants<float>::pi);
    if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;

    // サイン波はラジアンで計算
    float s = std::sin(p);

    // 波形生成ロジック用に、0.0 ～ 1.0 に正規化された位相を作る！
    float normPhase = p / (2.0f * juce::MathConstants<float>::pi);

    int safeWave = std::clamp(wave, 0, 4);

    return waveStrategies[safeWave](p, normPhase, s);
}

void OplOperator::updateIncrementsWithKeyScale()
{
    if (m_sampleRate <= 0.0) return;

    // ====================================================================
    // レジスタモード (RG-EN = ON) : 実機のアルゴリズムで増減量を計算
    // ====================================================================
    if (m_params.regEnable)
    {
        // 1. キースケールレート (KSR) の算出
        int ksrValue = 0;

        int octave = (m_noteNumber / 12) - 1;
        if (octave < 0) octave = 0;
        if (octave > 7) octave = 7;

        int noteOffset = m_noteNumber % 12;
        int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
        ksrValue = m_params.keyScale > 0 ? keyRate : (keyRate >> 2);

        // 2. レジスタ値から実効レート(0~63)を算出し、インクリメントに変換する関数
        // isRRフラグを追加し、RRの時だけスケールを調整する
        auto calcRegRate = [&](int regVal, bool isRR) -> float {
            // RR以外のRate0は停止（サステイン維持など）。
            if (regVal == 0 && !isRR) return 0.0f;

            int baseRate = regVal;

            // OPL系（全て4bit）、およびOPN系のRR（4bit）は、5bit(0-31)スケールに補正する
            if (m_params.isOplMode || isRR) {
                // 15の時に31になるように (val * 2 + 1)
                baseRate = (regVal * 2) + 1;
            }

            // DAW向け安全装置: RRが0（baseRateが1）の場合でも、永遠に鳴り止まないのを防ぐため
            // 非常にゆっくり（約20秒）減衰して消えるようにする。
            if (baseRate <= 1 && isRR) {
                return 1.0f / (20.0f * (float)m_sampleRate);
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

            return 1.0f / (timeInSeconds * (float)m_sampleRate);
            };

        // 各レートの計算（第2引数に、それがRRかどうかのフラグを渡す）
        m_attackInc = calcRegRate(m_params.rar, false);
        m_decayDec = calcRegRate(m_params.rdr, false);
        m_sustainRateDec = (m_params.rsr == 0) ? 0.0f : calcRegRate(m_params.rsr, false);

        // Release Rate の計算時のみ isRR を true にする
        m_releaseDec = calcRegRate(m_params.rrr, true);

        m_susReleaseDec = calcRegRate(5, true);

        // 3. サステインレベル (SL) の計算
        if (m_params.rsl == 15) {
            m_params.sustain = 0.0f; // SL=15 は一気に0まで落ちる
        }
        else {
            // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
            float slDb = m_params.rsl * 3.0f;
            m_params.sustain = std::pow(10.0f, -slDb / 20.0f);
        }
    }
    // ====================================================================
    // 従来モード (RG-EN = OFF) : 既存の秒数ベースの計算
    // ====================================================================
    else
    {
        // KeyScaleによるスケーリング計算のバグを修正
        float rateScale = 1.0f;
        if (m_params.keyScale > 0) {
            // m_noteNumber(通常0〜127) を使ってスケールを計算するが、
            // 係数を小さくして急激な倍率変化を防ぐ
            float noteFactor = (float)(m_noteNumber) / 127.0f;
            rateScale = 1.0f + ((float)m_params.keyScale * noteFactor * 0.5f);
        }

        // param(秒数) に対してスケーリングを行う。
        // param が 0 の時（0.001fの時）に正しく 1ms になるように計算式を修正。
        auto calcInc = [&](float paramInSeconds) -> float {
            // スケールを適用した実際の秒数（短くなる）
            float scaledSeconds = paramInSeconds / rateScale;
            // 最低でも 1ms (0.001秒) は保証する
            float finalSeconds = std::max(0.001f, scaledSeconds);
            // サンプルレートから「1サンプルあたりに進む量」を返す
            return 1.0f / (finalSeconds * (float)m_sampleRate);
            };

        m_attackInc = calcInc(m_params.attack);
        m_decayDec = calcInc(m_params.decay);
        m_releaseDec = calcInc(m_params.release);

        m_susReleaseDec = calcInc(1.5f);

        if (m_params.sustainRate <= 0.001f) {
            m_sustainRateDec = 0.0f;
        }
        else {
            // Sustain Rate は値(0.0~1.0)が小さいほど遅い（長い）という特殊な仕様
            float srTime = 5.0f * (1.0f - m_params.sustainRate);
            m_sustainRateDec = calcInc(srTime);
        }

        // 従来モードのサステインレベルはそのまま適用する
        m_params.sustain = m_params.sustain;
    }
}
