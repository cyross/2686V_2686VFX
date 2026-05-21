#include <algorithm>
#include <cmath>

#include "./Core.h"

void Opzx7Adddr::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void Opzx7Adddr::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = newSampleRate;
}

void Opzx7Adddr::setParameters(const Opzx7AdddrParams& params) {
	this->rgEnable = params.rgEnable;

    this->real.ar = params.real.ar;
	this->real.d1r = params.real.d1r;
	this->real.d2r = params.real.d2r;
	this->real.d1l = params.real.d1l;
	this->real.rr = params.real.rr;
	this->real.tl = params.real.tl;

	this->rg.ar = params.rg.ar;
	this->rg.d1r = params.rg.d1r;
	this->rg.d2r = params.rg.d2r;
	this->rg.d1l = params.rg.d1l;
	this->rg.rr = params.rg.rr;
	this->rg.tl = params.rg.tl;

    this->ks = params.ks;
    this->sus = params.sus;
	this->xof = params.xof;

    if (this->rgEnable)
    {
        this->m_zeroDecay = this->rg.d1r == 0;
        // サステインレベル (SL) の計算
        if (this->rg.d1l == 15) {
            this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
        }
        else {
            // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
            float slDb = this->rg.d1l * 3.0f;
            this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
        }
    }
    else
    {
        this->m_zeroDecay = this->real.d1r == 0;
        // 従来モードのサステインレベルはそのまま適用する
        this->m_sustain = this->real.d1l;
    }
}

float Opzx7Adddr::noteOn(float velocity) {
    // TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = (rgEnable ? rg.tl : real.tl) * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    state = State::Attack;

    return velocity * tlGain;
}

void Opzx7Adddr::noteOff() {
    // XOFが有効なときはノートオフ処理を無効化
    if (xof)
    {
        return;
    }

    state = State::Release;

    currentReleaseDec = getReleaseDec();
}

void Opzx7Adddr::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
}

void Opzx7Adddr::updateIncrementsWithKeyScale(int noteNumber)
{
    if (sampleRate <= 0.0) {
        currentReleaseDec = getReleaseDec();

        return;
    }

    // ====================================================================
    // レジスタモード (RG-EN = ON) : 実機のアルゴリズムで増減量を計算
    // ====================================================================
    if (rgEnable)
    {
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
        attackInc = calcRegRate(rg.ar, false);
        decayDec = calcRegRate(rg.d1r, false);
        sustainRateDec = (rg.d1r == 0) ? 0.0f : calcRegRate(rg.d1r, false);

        // Release Rate の計算時のみ isRR を true にする
        releaseDec = calcRegRate(rg.rr, true);

        susReleaseDec = calcRegRate(5, true);
    }
    // ====================================================================
    // 従来モード (RG-EN = OFF) : 既存の秒数ベースの計算
    // ====================================================================
    else
    {
        // KeyScaleによるスケーリング計算のバグを修正
        float rateScale = 1.0f;
        if (ks > 0) {
            // m_noteNumber(通常0〜127) を使ってスケールを計算するが、
            // 係数を小さくして急激な倍率変化を防ぐ
            float noteFactor = (float)(noteNumber) / 127.0f;
            rateScale = 1.0f + ((float)ks * noteFactor * 0.5f);
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

        attackInc = calcInc(real.ar);
        decayDec = calcInc(real.d1r);
        releaseDec = calcInc(real.rr);

        susReleaseDec = calcInc(1.5f);

        if (real.d1r <= 0.001f) {
            sustainRateDec = 0.0f;
        }
        else {
            // Sustain Rate は値(0.0~1.0)が小さいほど遅い（長い）という特殊な仕様
            float srTime = 5.0f * (1.0f - real.d2r);
            sustainRateDec = calcInc(srTime);
        }
    }

    currentReleaseDec = getReleaseDec();
}

float Opzx7Adddr::updateEnvelopeState(float currentLevel)
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
