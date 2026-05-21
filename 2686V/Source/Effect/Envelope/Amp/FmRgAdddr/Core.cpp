#include <algorithm>
#include <cmath>

#include "./Core.h"

void FmRgAdddr::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void FmRgAdddr::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = newSampleRate;
}

void FmRgAdddr::setParameters(const FmRgAdddrParams& params) {
    this->ar = params.ar;
    this->d1r = params.d1r;
    this->d1l = params.d1l;
    this->d2r = params.d2r;
    this->rr = params.rr;
    this->ks = params.ks;

    this->m_zeroDecay = this->d1r == 0;
    // サステインレベル (SL) の計算
    if (this->d1l == 15) {
        this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float slDb = this->d1l * 3.0f;
        this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }
}

float FmRgAdddr::noteOn(float velocity) {
    // TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = tl * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    state = State::Attack;

    return velocity * tlGain;
}

void FmRgAdddr::noteOff() {
    state = State::Release;

    currentReleaseDec = releaseDec;
}

void FmRgAdddr::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
}

void FmRgAdddr::updateIncrementsWithKeyScale(int noteNumber)
{
    if (sampleRate <= 0.0) {
        currentReleaseDec = releaseDec;

        return;
    }

    // ====================================================================
    // 実機のアルゴリズムで増減量を計算
    // ====================================================================
    // 1. キースケールレート (KSR) の算出
    int ksrValue = 0;

    int octave = (noteNumber / 12) - 1;
    if (octave < 0) octave = 0;
    if (octave > 7) octave = 7;

    int noteOffset = noteNumber % 12;
    int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
    ksrValue = keyRate >> (3 - std::clamp(ks, 0, 3));

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
        }

        // DAW向け安全装置: RRが0（baseRateが1）の場合でも、永遠に鳴り止まないのを防ぐため
        // 非常にゆっくり（約20秒）減衰して消えるようにする。
        if (baseRate <= 1 && isRR) {
            return 1.0f / (20.0f * (float)sampleRate);
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
    attackInc = calcRegRate(ar, false);
    decayDec = calcRegRate(d1r, false);
    sustainRateDec = (d2r == 0) ? 0.0f : calcRegRate(d2r, false);

    // Release Rate の計算時のみ isRR を true にする
    releaseDec = calcRegRate(rr, true);

    susReleaseDec = calcRegRate(5, true);

    // 3. サステインレベル (SL) の計算
    if (d1l == 15) {
        m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float slDb = d1l * 3.0f;
        m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }

    currentReleaseDec = releaseDec;
}

float FmRgAdddr::updateEnvelopeState(float currentLevel)
{
    if (state == State::Attack) {
        currentLevel += attackInc;
        if (currentLevel >= 1.0f) { currentLevel = 1.0f; state = State::Decay; }
    }
    else if (state == State::Decay) {
        float limitLevel = m_sustain;

        // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
        if (m_zeroDecay)
        {
            currentLevel = 1.0;
            state = State::Sustain;
        }
        else if (currentLevel > limitLevel) {
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

    return currentLevel;
}
