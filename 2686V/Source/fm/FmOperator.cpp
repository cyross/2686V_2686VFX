#include <array>

#include "FmOperator.h"
#include "../core/PrValues.h"

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
const std::array<float, 8> FmOperator::dtScales = {0.0f, -0.0045f, -0.0025f, -0.001f, 0.0f, 0.001f, 0.0025f, 0.0045f };

// DT2 (OPM Coarse Detune)
// YM2151: 0=0, 1=+approx 1.414, 2=+approx 1.58, 3=+approx 1.73
// 0: x1.0
// 1: x1.41 (600 cent up)
// 2: x1.58 (780 cent up)
// 3: x1.78 (950 cent up)
const std::array<float, 4> FmOperator::dt2Scales = { 1.0f, 1.414f, 1.581f, 1.781f };

const std::array<FmOperator::SsgWaveCalculator, 16> FmOperator::ssgWaveStrategies = { {
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

void FmOperator::setParameters(const FmOpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = 1.0f;
    m_params.ssgEg = 0;
    m_params.waveSelect = 0;
}

void FmOperator::noteOff()
{
    m_state = State::Release;

    m_currentReleaseDec = m_releaseDec;
}

float FmOperator::calcWaveform(double phase, int wave)
{
    // 1. まず phase を 0.0 ～ 2π の範囲に丸め込む (ラジアン)
    float p = std::fmod((float)phase, 2.0f * juce::MathConstants<float>::pi);
    if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;

    // サイン波はラジアンで計算
    float s = std::sin(p);

    return s; // Default Sine
}

void FmOperator::updateEnvelopeState()
{
    if (m_state == State::Attack) {
        m_currentLevel += m_attackInc;
        if (m_currentLevel >= 1.0f) { m_currentLevel = 1.0f; m_state = State::Decay; }
    }
    else if (m_state == State::Decay) {
        float limitLevel = m_params.sustain;

        if (m_currentLevel > limitLevel) {
            // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
            if (m_decayDec > 0.0f) {
                m_currentLevel -= m_decayDec;
                if (m_currentLevel <= limitLevel) {
                    m_currentLevel = limitLevel;
                    m_state = State::Sustain;
                }
            }
        }
        else {
            m_currentLevel = limitLevel;
            m_state = State::Sustain;
        }
    }
    else if (m_state == State::Sustain) {
        // ====================================================================
        // パーカッシブモード(EG-TYP=OFF)の判定を OPL/OPL3 系統のみに限定する
        // ====================================================================
        if (m_params.isOplMode && !m_params.egType) {
            m_currentLevel -= m_releaseDec;
            if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
        }
        // OPN/OPM/OPZX3、または OPL系の持続モード(EG-TYP=ON) の場合
        else {
            // SR(Sustain Rate / OPMではD2R) でゆっくり減衰する
            if (m_sustainRateDec > 0.0f) {
                m_currentLevel -= m_sustainRateDec;
                if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
            }
        }
    }
    else if (m_state == State::Release) {
        m_currentLevel -= m_currentReleaseDec;
        if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
    }
}

void FmOperator::updateIncrementsWithKeyScale()
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

        if (m_params.isOplMode)
        {
            int noteOffset = m_noteNumber % 12;
            int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
            ksrValue = m_params.keyScale > 0 ? keyRate : (keyRate >> 2);
        }
        else
        {
            int noteOffset = m_noteNumber % 12;
            int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
            ksrValue = keyRate >> (3 - std::clamp(m_params.keyScale, 0, 3));
        }

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
