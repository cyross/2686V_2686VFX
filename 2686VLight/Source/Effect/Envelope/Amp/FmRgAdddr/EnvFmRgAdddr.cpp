#include <algorithm>
#include <cmath>

#include "./EnvFmRgAdddr.h"

FmRgAdddr::FmRgAdddr()
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

void FmRgAdddr::prepare(int posIndex, double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void FmRgAdddr::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = newSampleRate;
}

void FmRgAdddr::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
}

void FmRgAdddr::setParameters(const FmRgAdddrParams& params) {
    this->ar = params.ar;
    this->d1r = params.d1r;
    this->d1l = params.d1l;
    this->d2r = params.d2r;
    this->rr = params.rr;
    this->tl = params.tl;
    this->m_ksMode = params.ksMode;
    this->m_ksOPM.setParameters(params.ksOPM);
    this->m_ksOPP.setParameters(params.ksOPP);

    this->xof = params.xof;
    this->kor = params.kor;

    this->bypass = params.bypass;

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

    this->updateIncrementsWithKeyScale(m_noteNumber);
}

// ============================================================================
// レートスケーリング (KSR) の計算
// ============================================================================
int FmRgAdddr::calcRateScaling() const
{
    switch (m_ksMode) {
    case FmRgAdddrKeyScaleMode::OPM:
        return m_ksOPM.calcKeyScaleRate(m_noteNumber);
    case FmRgAdddrKeyScaleMode::OPP:
        return m_ksOPP.calcKeyScaleRate(m_noteNumber);
    }

    return 0;
}

// ============================================================================
// レベルスケーリング (KSL) の計算 (dBで返す)
// ============================================================================
float FmRgAdddr::calcLevelScalingDb() const
{
    switch (m_ksMode) {
    case FmRgAdddrKeyScaleMode::OPM:
        return 0.0f;
    case FmRgAdddrKeyScaleMode::OPP:
        return m_ksOPP.calcLevelScalingDb(m_noteNumber);
    }

    return 0.0f;
}

float FmRgAdddr::noteOn(float velocity, int noteNumber) {
    // KSL はノートナンバーに依存するため、ここで先に確定させる。
    // (オペレータは noteOn() を updateIncrementsWithKeyScale() より先に呼ぶので、
    //  ここで受け取らないと 1音前のノートナンバーで KSL を計算してしまう)
    m_noteNumber = noteNumber;

    this->m_phaseProgress = 0.0f;

    if (this->bypass) {
        return velocity;
    }

    state = State::Attack;

    // TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = this->tl * 0.75f;
    float kslDb = calcLevelScalingDb();

    // マイナス値(+LIN, +EXP等による増幅)でも、最終ゲインが1.0を超えないようサチュレーションさせる。
    // TL はこの totalDb に一度だけ含める。tlGain を別に掛けると TL が二重に効いてしまう。
    //
    // 実機のレベルスケーリングは TL(出力レベルレジスタ)から引く形で働くので、
    // TL レジスタの全域 (tlMax × 0.75dB) より深くは減衰しない。同じ上限で頭打ちにする。
    float maxAttenDb = (this->tlMax > 0) ? ((float)this->tlMax * 0.75f) : 95.25f;
    float totalDb = std::clamp(attenuationDb + kslDb, 0.0f, maxAttenDb);
    float finalGain = std::pow(10.0f, -totalDb / 20.0f);

    return velocity * finalGain;
}

void FmRgAdddr::noteOff() {
    // XOF/Bypassが有効なときはノートオフ処理を無効化
    if (xof || bypass)
    {
        return;
    }

    state = State::Release;

    this->m_phaseProgress = 0.0f; // フェーズ時間のリセット
}

void FmRgAdddr::updateIncrementsWithKeyScale(int noteNumber)
{
    m_noteNumber = noteNumber;

    if (sampleRate <= 0.0) {
        return;
    }

    // ====================================================================
    // 実機のアルゴリズムで増減量を計算
    // ====================================================================
    // 1. キースケールレート (KSR) の算出
    int ksrValue = calcRateScaling();

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
    decayDec = calcRegRate(d1r, d1rMax, false, false);
    sustainRateDec = (d2r == 0) ? 0.0f : calcRegRate(d2r, d2rMax, false, false);
    releaseTimeInc = calcRegRate(rr, rrMax, true, false);
    releaseDec = kor ? 0.0f : releaseTimeInc;
}

float FmRgAdddr::updateEnvelopeState(float currentLevel)
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
            // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する

            limitLevel = m_sustain;

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

void FmRgAdddr::setParamMax(int ar, int d1r, int d2r, int d1l, int rr, int tl) {
    arMax = ar;
    d1rMax = d1r;
    d2rMax = d2r;
    d1lMax = d1l;
    rrMax = rr;
    tlMax = tl;
}

void FmRgAdddr::bypassedReleasedProcess() {
    this->state = State::Idle;
}
