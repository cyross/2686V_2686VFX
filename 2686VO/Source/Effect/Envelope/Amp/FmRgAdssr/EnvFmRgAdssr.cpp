#include <algorithm>
#include <cmath>

#include "./EnvFmRgAdssr.h"

FmRgAdssr::FmRgAdssr()
{
    auto calcTimeInSecond = [&](int effectiveRate, bool isAttack) -> float {
        if (effectiveRate >= 60) {
            // Rate 60以上はほぼ瞬時（1ミリ秒）
            return 0.001f;
        }

        // 実機(OPN/OPM)仕様に忠実な「レートが4下がるごとに時間が2倍」の式
        float diff = 60.0f - (float)effectiveRate;
        float timeFactor = std::pow(2.0f, diff / 4.0f);
        float baseTime = 0.0003f;
        float timeInSeconds = baseTime * timeFactor;

        if (isAttack) {
            timeInSeconds *= 0.33f;
        }

        return std::min(timeInSeconds, 15.0f) * 1.5f;
        };

    for (int i = 0; i < 64; i++) {
        timeInSecondsLut[i] = calcTimeInSecond(i, false);
        attcckTimeInSecondsLut[i] = calcTimeInSecond(i, true);
    }
}

void FmRgAdssr::prepare(int posIndex, double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void FmRgAdssr::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = newSampleRate;
}

void FmRgAdssr::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
}

void FmRgAdssr::setParameters(const FmRgAdssrParams& params) {
    this->ar = params.ar;
    this->dr = params.dr;
    this->sl = params.sl;
    this->rr = params.rr;
    this->tl = params.tl;

    this->m_ksOPN.setParameters(params.ksOPN);

    this->xof = params.xof;
    this->kor = params.kor;

    this->bypass = params.bypass;

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

    this->updateIncrementsWithKeyScale(m_noteNumber);
}

float FmRgAdssr::noteOn(float velocity) {
    this->m_phaseProgress = 0.0f;

    if (this->bypass) {
        return velocity;
    }

    state = State::Attack;

    // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = tl * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    return velocity * tlGain;
}

void FmRgAdssr::noteOff() {
    // XOF/Bypassが有効なときはノートオフ処理を無効化
    if (xof)
    {
        return;
    }

    if (bypass) {
        return;
    }

    state = State::Release;

    this->m_phaseProgress = 0.0f; // フェーズ時間のリセット
}

void FmRgAdssr::updateIncrementsWithKeyScale(int noteNumber)
{
    m_noteNumber = noteNumber;

    if (sampleRate <= 0.0) {
        return;
    }

    // ====================================================================
    // 実機のアルゴリズムで増減量を計算
    // ====================================================================
    // 1. キースケールレート (KSR) の算出
    int ksrValue = m_ksOPN.calcKeyScaleRate(m_noteNumber);

    // 2. レジスタ値から実効レート(0~63)を算出し、インクリメントに変換する関数
    // isAttack 引数を追加し、アタックと減衰で時間を調整する
    auto calcRegRate = [&](int regVal, int regMax, bool isRR, bool isAttack) -> float {
        // RR以外のRate0は停止（サステイン維持など）。
        if (regVal == 0 && !isRR) return 0.0f;

        // DAW向け安全装置: RRが0（baseRateが1）の場合でも、永遠に鳴り止まないのを防ぐため
        // ゆっくり（約5秒）減衰して消えるようにする。
        if (regVal <= 1 && isRR) {
            return 1.0f / (5.0f * (float)sampleRate);
        }

        // 実効レート = 基本レート(0-31) * 2 + KSR (0-3)
        // (regMax が 31 であることを前提とした 0-63 へのマッピング)
        int effectiveRate = (int)((float)regVal * 63.0 / (float)regMax) + ksrValue;
        if (effectiveRate > 63) effectiveRate = 63;

        float timeInSeconds = isAttack ? attcckTimeInSecondsLut[effectiveRate] : timeInSecondsLut[effectiveRate];

        // 最終的なデクリック保証
        if (isRR) {
            // リリース時は最低 5ms (0.005秒) かけてフェードアウトし、プチプチを消す
            timeInSeconds = std::max(timeInSeconds, 0.005f);
        }
        else if (isAttack) {
            // アタック時も 1ms は最低保証
            timeInSeconds = std::max(timeInSeconds, 0.001f);
        }
        else {
            // それ以外は極小値
            timeInSeconds = std::max(timeInSeconds, 0.0001f);
        }

        return 1.0f / (timeInSeconds * (float)sampleRate);
        };

    // 各レートの計算
    attackInc = calcRegRate(ar, arMax, false, true);
    decayDec = calcRegRate(dr, drMax, false, false);
    sustainRateDec = (sr == 0) ? 0.0f : calcRegRate(sr, srMax, false, false);
    releaseTimeInc = calcRegRate(rr, rrMax, true, false);
    releaseDec = kor ? 0.0f : releaseTimeInc;
}

float FmRgAdssr::updateEnvelopeState(float currentLevel)
{
    if (this->bypass) {
        return 1.0f;
    }

    float limitLevel = 0.0f;

    switch (this->state) {
    case State::Idle:
        return currentLevel;
    case State::Attack:
        currentLevel += attackInc;

        if (currentLevel >= 1.0f) {
            currentLevel = 1.0f;
            state = State::Decay;
        }

        return currentLevel;
    case State::Decay:
        limitLevel = m_sustain;

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

        return currentLevel;
    case State::Sustain:
        // SR(Sustain Rate / OPMではD2R) でゆっくり減衰する
        if (sustainRateDec > 0.0f) {
            currentLevel -= sustainRateDec;

            if (currentLevel <= 0.001f) {
                currentLevel = 0.0f;
                state = State::Idle;
            }
        }

        return currentLevel;
    case State::Release:
        // kor向けに時間を進める
        this->m_phaseProgress += this->releaseTimeInc;

        if (this->m_phaseProgress >= 1.0f) {
            this->m_phaseProgress = 0.0f;
            this->state = State::Idle;
            currentLevel = 0.0f;
            return 0.0f;
        }

        currentLevel -= releaseDec;

        if (currentLevel <= 0.001f) {
            currentLevel = 0.0f;
            state = State::Idle;
        }

        return currentLevel;
    }

    return currentLevel; // 念のため、どこにも当てはまらない場合は現在のレベルを維持
}

void FmRgAdssr::setParamMax(int ar, int dr, int sr, int sl, int rr, int tl) {
    arMax = ar;
    drMax = dr;
    srMax = sr;
    slMax = sl;
    rrMax = rr;
    tlMax = tl;
}

void FmRgAdssr::bypassedReleasedProcess() {
    this->state = State::Idle;
}
