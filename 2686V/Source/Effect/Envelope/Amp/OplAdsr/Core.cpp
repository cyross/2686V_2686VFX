#include <algorithm>
#include <cmath>

#include "./Core.h"

const std::array<float, 4> OplAdsr::dbPerOcts = { 0.0f, 1.5f, 3.0f, 6.0f };

void OplAdsr::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void OplAdsr::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = newSampleRate;
}

void OplAdsr::setParameters(const OplAdsrParams& params) {
    this->ar = params.ar;
    this->dr = params.dr;
    this->sl = params.sl;
    this->rr = params.rr;
	this->ksr = params.ksr;
	this->ksl = params.ksl;
	this->sus = params.sus;
	this->egType = params.egType;

    this->m_zeroDecay = this->dr == 0;
    // サステインレベル (SL) の計算
    if (this->sl == 15) {
        this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float slDb = this->sl * 3.0f;
        this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }
}

float OplAdsr::noteOn(float velocity, int noteNumber) {
    // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = tl * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    float kslAttenuation = 1.0f;
    if (this->ksl > 0)
    {
        float octaveDiff = (float)(noteNumber - 48) / 12.0f;

        if (octaveDiff < 0) octaveDiff = 0;

        float totalDb = dbPerOcts[std::clamp(this->ksl, 0, 3)] * octaveDiff;

        kslAttenuation = std::pow(10.0f, -totalDb / 20.0f);
    }

    this->state = State::Attack;

    return velocity * tlGain * kslAttenuation;
}

void OplAdsr::noteOff() {
    this->state = State::Release;

    // キーを離した瞬間、SUSがONならリリースレートをゆっくり(RR=5)にする
    this->currentReleaseDec = this->getReleaseDec();
}

void OplAdsr::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
}

void OplAdsr::updateIncrementsWithKeyScale(int noteNumber)
{
    if (sampleRate <= 0.0)
    {
        this->currentReleaseDec = this->getReleaseDec();
 
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
    ksrValue = this->ksr ? keyRate : (keyRate >> 2);

    // 2. レジスタ値から実効レート(0~63)を算出し、インクリメントに変換する関数
    // isRRフラグを追加し、RRの時だけスケールを調整する
    auto calcRegRate = [&](int regVal, bool isRR) -> float {
        // RR以外のRate0は停止（サステイン維持など）。
        if (regVal == 0 && !isRR) return 0.0f;

        int baseRate = regVal;

        // OPL系（全て4bit）、およびOPN系のRR（4bit）は、5bit(0-31)スケールに補正する
        // 15の時に31になるように (val * 2 + 1)
        baseRate = (regVal * 2) + 1;

        // DAW向け安全装置: RRが0（baseRateが1）の場合でも、永遠に鳴り止まないのを防ぐため
        // 非常にゆっくり（約20秒）減衰して消えるようにする。
        if (baseRate <= 1 && isRR) {
            return 1.0f / (20.0f * (float)this->sampleRate);
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

        return 1.0f / (timeInSeconds * (float)this->sampleRate);
        };

    // 各レートの計算（第2引数に、それがRRかどうかのフラグを渡す）
    this->attackInc = calcRegRate(this->ar, false);
    this->decayDec = calcRegRate(this->dr, false);
    this->sustainRateDec = 0.0f;

    // Release Rate の計算時のみ isRR を true にする
    this->releaseDec = calcRegRate(this->rr, true);

    this->susReleaseDec = calcRegRate(5, true);

    // 3. サステインレベル (SL) の計算
    if (sl == 15) {
        this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float slDb = this->sl * 3.0f;
        this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }

    this->currentReleaseDec = this->getReleaseDec();
}

float OplAdsr::updateEnvelopeState(float currentLevel)
{
    if (this->state == State::Attack) {
        currentLevel += this->attackInc;
        if (currentLevel >= 1.0f) { currentLevel = 1.0f; this->state = State::Decay; }
    }
    else if (this->state == State::Decay) {
        float limitLevel = this->m_sustain;

        // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
        if (this->m_zeroDecay)
        {
            currentLevel = 1.0;
            this->state = State::Sustain;
        }
        else if (currentLevel > limitLevel) {
            if (this->decayDec > 0.0f) {
                currentLevel -= this->decayDec;
                if (currentLevel <= limitLevel) {
                    currentLevel = limitLevel;
                    this->state = State::Sustain;
                }
            }
        }
        else {
            currentLevel = limitLevel;
            this->state = State::Sustain;
        }
    }
    else if (this->state == State::Sustain) {
        // ====================================================================
        // パーカッシブモード(EG-TYP=OFF)の判定
        // ====================================================================
        if (!this->egType) {
            currentLevel -= this->currentReleaseDec;
            if (currentLevel <= 0.0f) { currentLevel = 0.0f; this->state = State::Idle; }
        }
        // OPN/OPM/OPZX7、または OPL系の持続モード(EG-TYP=ON) の場合
        else {
            // SR(Sustain Rate) でゆっくり減衰する
            if (this->sustainRateDec > 0.0f) {
                currentLevel -= this->sustainRateDec;
                if (currentLevel <= 0.0f) { currentLevel = 0.0f; this->state = State::Idle; }
            }
        }
    }
    else if (this->state == State::Release) {
        currentLevel -= this->currentReleaseDec;
        if (currentLevel <= 0.0f) { currentLevel = 0.0f; this->state = State::Idle; }
    }

    return currentLevel;
}

float OplAdsr::getReleaseDec() const
{
    if (this->sus) {
        return this->susReleaseDec;
    }
    else {
        return this->releaseDec;
    }
}
