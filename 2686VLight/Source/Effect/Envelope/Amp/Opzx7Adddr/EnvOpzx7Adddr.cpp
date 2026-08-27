#include <algorithm>
#include <cmath>

#include "./EnvOpzx7Adddr.h"
#include "../../../../Core/Const/ConstGlobal.h"

Opzx7Adddr::Opzx7Adddr()
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

void Opzx7Adddr::prepare(int posIndex, double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void Opzx7Adddr::updateTargetSampleRate(double newSampleRate)
{
    this->m_sampleRate = newSampleRate;
}

void Opzx7Adddr::updateSampleRate(double newSampleRate) {
    this->m_sampleRate = newSampleRate;
}

void Opzx7Adddr::setParameters(const Opzx7AdddrParams& params) {
    this->m_rgEnable = params.rgEnable;

    this->m_real.ar = params.real.ar;
    this->m_real.d1r = params.real.d1r;
    this->m_real.d2r = params.real.d2r;
    this->m_real.d1l = params.real.d1l;
    this->m_real.rr = params.real.rr;
    this->m_real.tl = params.real.tl;

    this->m_rg.ar = params.rg.ar;
    this->m_rg.d1r = params.rg.d1r;
    this->m_rg.d2r = params.rg.d2r;
    this->m_rg.d1l = params.rg.d1l;
    this->m_rg.rr = params.rg.rr;
    this->m_rg.tl = params.rg.tl;

    this->m_ksEn = params.ksEn;
    this->m_ksMode = params.ksMode;
    this->m_ksMA7.setParameters(params.ksMA7);
    this->m_ksOPZ.setParameters(params.ksOPZ);
    this->m_ksOPS.setParameters(params.ksOPS);

    this->m_sus = params.sus;
    this->m_xof = params.xof;
    this->m_kor = params.kor;

    this->m_bypass = params.bypass;

    if (this->m_rgEnable)
    {
        this->m_zeroDecay = this->m_rg.d1r == 0;
        // サステインレベル (SL) の計算
        if (this->m_rg.d1l == 15) {
            this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
        }
        else {
            // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
            float slDb = this->m_rg.d1l * 3.0f;
            this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
        }
    }
    else
    {
        this->m_zeroDecay = this->m_real.d1r == 0;
        // 従来モードのサステインレベルはそのまま適用する
        this->m_sustain = this->m_real.d1l;
    }

    this->updateIncrements(m_noteNumber);
}

// ============================================================================
// レートスケーリング (KSR) の計算
// ============================================================================
int Opzx7Adddr::calcRateScaling() const
{
    if (!this->m_ksEn && !this->m_rgEnable) return 0; // KSRが無効な場合

    switch (m_ksMode) {
        case Opzx7AdddrKeyScaleMode::OPS:
            return m_ksOPS.calcKeyScaleRate(m_noteNumber);
        case Opzx7AdddrKeyScaleMode::MA7:
            return m_ksMA7.calcKeyScaleRate(m_noteNumber);
        case Opzx7AdddrKeyScaleMode::OPZ:
            return m_ksOPZ.calcKeyScaleRate(m_noteNumber);
    }

    return 0;
}

// ============================================================================
// レベルスケーリング (KSL) の計算 (dBで返す)
// ============================================================================
float Opzx7Adddr::calcLevelScalingDb() const
{
    if (!this->m_ksEn && !this->m_rgEnable) return 0.0f; // KSLが無効な場合

    switch (m_ksMode) {
    case Opzx7AdddrKeyScaleMode::OPS:
        return m_ksOPS.calcLevelScalingDb(m_noteNumber);
    case Opzx7AdddrKeyScaleMode::MA7:
        return m_ksMA7.calcLevelScalingDb(m_noteNumber);
    case Opzx7AdddrKeyScaleMode::OPZ:
        return m_ksOPZ.calcLevelScalingDb(m_noteNumber);
    }

    return 0.0f;
}

float Opzx7Adddr::noteOn(float velocity, int noteNumber) {
    // KSL はノートナンバーに依存するため、ここで先に確定させる。
    // (オペレータは noteOn() を updateIncrements() より先に呼ぶので、
    //  ここで受け取らないと 1音前のノートナンバーで KSL を計算してしまう)
    m_noteNumber = noteNumber;

    this->m_phaseProgress = 0.0f;

    if (this->m_bypass) {
        return velocity;
    }

    m_state = State::Attack;

    float attenuationDb = 0.0f;

    if (this->m_rgEnable) {
        // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
        // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
        attenuationDb = (float)this->m_rg.tl * 0.75f;
    }
    else {
        // 従来モード: TL はレジスタ値ではなく 0.0〜1.0 の「レベル」(1.0 = 減衰なし)。
        // 0.75dB/step を掛けるとほとんど効かないうえ向きも逆になるため、
        // レベルをそのまま dB 減衰へ変換する。
        float level = std::clamp(this->m_real.tl, 0.0f, 1.0f);

        if (level <= 0.0f) {
            return 0.0f; // レベル0は無音
        }

        attenuationDb = -20.0f * std::log10(level);
    }

    float kslDb = calcLevelScalingDb();

    // マイナス値(+LIN, +EXP等による増幅)でも、最終ゲインが1.0を超えないようサチュレーションさせる。
    // TL はこの totalDb に一度だけ含める。tlGain を別に掛けると TL が二重に効いてしまう。
    //
    // 実機のレベルスケーリングは TL(出力レベルレジスタ)から引く形で働くので、
    // TL レジスタの全域 (tlMax × 0.75dB) より深くは減衰しない。同じ上限で頭打ちにする。
    float maxAttenDb = (this->m_rgMax.tl > 0) ? ((float)this->m_rgMax.tl * 0.75f) : 47.25f;
    float totalDb = std::clamp(attenuationDb + kslDb, 0.0f, maxAttenDb);
    float finalGain = std::pow(10.0f, -totalDb / 20.0f);

    return velocity * finalGain;
}

void Opzx7Adddr::noteOff() {
    // XOF/Bypassが有効なときはノートオフ処理を無効化
    if (m_xof || m_bypass)
    {
        return;
    }

    m_state = State::Release;

    this->m_phaseProgress = 0.0f; // フェーズ時間のリセット

    m_currentReleaseDec = getReleaseDec();
}

void Opzx7Adddr::updateIncrements(int noteNumber)
{
    m_noteNumber = noteNumber;

    if (m_sampleRate <= 0.0) {
        m_currentReleaseDec = getReleaseDec();

        return;
    }

    int ksrValue = calcRateScaling();

    // ====================================================================
    // レジスタモード (RG-EN = ON) : 実機のアルゴリズムで増減量を計算
    // ====================================================================
    if (m_rgEnable)
    {
        // 2. レジスタ値から実効レート(0~63)を算出し、インクリメントに変換する関数
        // isAttack 引数を追加し、アタックと減衰で時間を調整する
        auto calcRegRate = [&](int regVal, int regMax, bool isRR, bool isAttack) -> float {
            // RR以外のRate0は停止（サステイン維持など）。
            if (regVal == 0 && !isRR) return 0.0f;

            // DAW向け安全装置: RRが0（baseRateが1）の場合でも、永遠に鳴り止まないのを防ぐため
            // ゆっくり（約5秒）減衰して消えるようにする。
            if (regVal <= 1 && isRR) {
                return 1.0f / (Global::RateMaxSeconds::reg * (float)m_sampleRate);
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

            return 1.0f / (timeInSeconds * (float)m_sampleRate);
            };

        // 各レートの計算
        m_attackInc = calcRegRate(m_rg.ar, m_rgMax.ar, false, true);
        m_decayDec = calcRegRate(m_rg.d1r, m_rgMax.d1r, false, false);
        m_sustainRateDec = (m_rg.d2r == 0) ? 0.0f : calcRegRate(m_rg.d2r, m_rgMax.d2r, false, false);
        m_releaseTimeInc = calcRegRate(this->m_sus ? 5 : m_rg.rr, m_rgMax.rr, true, false);;
        m_releaseDec = m_kor ? 0.0f : m_releaseTimeInc;
    }
    // ====================================================================
    // 従来モード (RG-EN = OFF) : 既存の秒数ベースの計算
    // ====================================================================
    else
    {
        float rateScale = 1.0f;

        if (this->m_ksEn) {
            rateScale = 1.0f + ((float)ksrValue * 0.1f);
        }

        // param(秒数) に対してスケーリングを行う。
        // param が 0 の時（0.001fの時）に正しく 1ms になるように計算式を修正。
        auto calcInc = [&](float paramInSeconds, bool isRR = false) -> float {
            float scaledSeconds = paramInSeconds / rateScale;
            // リリース時(isRR)は最低 5ms、それ以外は 1ms を保証
            float minSeconds = isRR ? 0.005f : 0.001f;
            float finalSeconds = std::max(minSeconds, scaledSeconds);

            // サンプルレートから「1サンプルあたりに進む量」を返す
            return 1.0f / (finalSeconds * (float)m_sampleRate);
            };

        m_attackInc = calcInc(m_real.ar);
        m_decayDec = calcInc(m_real.d1r);
        m_releaseTimeInc = calcInc(this->m_sus ? 1.5f : m_real.rr, true);
        m_releaseDec = m_kor ? 0.0f : m_releaseTimeInc;

        if (m_real.d2r <= 0.001f) {
            m_sustainRateDec = 0.0f;
        }
        else {
            // Sustain Rate は値(0.0~1.0)が小さいほど遅い（長い）という特殊な仕様
            float srTime = Global::RateMaxSeconds::real * (1.0f - m_real.d2r);
            m_sustainRateDec = calcInc(srTime);
        }
    }

    m_currentReleaseDec = getReleaseDec();
}

float Opzx7Adddr::updateEnvelopeState(float currentLevel)
{
    if (this->m_bypass) {
        return 1.0f;
    }

    float limitLevel = 0.0f;

    switch (this->m_state) {
    case State::Idle:
        return currentLevel;
    case State::Attack:
        currentLevel += m_attackInc;

        if (currentLevel >= 1.0f) {
            currentLevel = 1.0f;
            m_state = State::Decay;
        }

        return currentLevel;
    case State::Decay:
        limitLevel = m_sustain;

        // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
        if (m_zeroDecay)
        {
            currentLevel = 1.0;
            m_state = State::Sustain;
        }
        else if (currentLevel > limitLevel) {
            if (m_decayDec > 0.0f) {
                currentLevel -= m_decayDec;
                if (currentLevel <= limitLevel) {
                    currentLevel = limitLevel;
                    m_state = State::Sustain;
                }
            }
        }
        else {
            currentLevel = limitLevel;
            m_state = State::Sustain;
        }

        return currentLevel;
    case State::Sustain:
        // SR(Sustain Rate / OPMではD2R) でゆっくり減衰する
        if (m_sustainRateDec > 0.0f) {
            currentLevel -= m_sustainRateDec;

            if (currentLevel <= 0.001f) {
                currentLevel = 0.0f;
                m_state = State::Idle;
            }
        }

        return currentLevel;
    case State::Release:
        // kor向けに時間を進める
        this->m_phaseProgress += this->m_releaseTimeInc;

        if (this->m_phaseProgress >= 1.0f) {
            this->m_phaseProgress = 0.0f;
            this->m_state = State::Idle;
            currentLevel = 0.0f;
            return 0.0f;
        }

        currentLevel -= m_currentReleaseDec;

        if (currentLevel <= 0.001f) {
            currentLevel = 0.0f;
            m_state = State::Idle;
        }

        return currentLevel;
    }

    return currentLevel; // 念のため、どこにも当てはまらない場合は現在のレベルを維持
}

void Opzx7Adddr::setParamMax(int ar, int d1r, int d2r, int d1l, int rr, int tl) {
    m_rgMax.ar = ar;
    m_rgMax.d1r = d1r;
    m_rgMax.d2r = d2r;
    m_rgMax.d1l = d1l;
    m_rgMax.rr = rr;
    m_rgMax.tl = tl;
}

void Opzx7Adddr::bypassedReleasedProcess() {
    this->m_state = State::Idle;
}
