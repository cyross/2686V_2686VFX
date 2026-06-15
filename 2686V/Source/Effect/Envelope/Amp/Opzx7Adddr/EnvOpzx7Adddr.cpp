#include <algorithm>
#include <cmath>

#include "./EnvOpzx7Adddr.h"
#include "../../../../Core/Const/ConstGlobal.h"

// KSLの減衰カーブ定義 (MA-7用: 4段階)
const std::array<float, 4> Opzx7Adddr::m_dbPerOctsMA7 = { 0.0f, 1.5f, 3.0f, 6.0f };
// KSLの減衰カーブ定義 (OPZ用: 8段階) ※値は仮の近似値です
const std::array<float, 8> Opzx7Adddr::m_dbPerOctsOPZ = { 0.0f, 1.5f, 3.0f, 4.5f, 6.0f, 7.5f, 9.0f, 12.0f };

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
    this->m_positionIndex = posIndex;

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
    this->m_ksrMA7 = params.ksrMA7;
    this->m_kslMA7 = params.kslMA7;
    this->m_ksrOPZ = params.ksrOPZ;
    this->m_kslOPZ = params.kslOPZ;
    this->m_ksBp = params.ksBp;
    this->m_ksLc = params.ksLc;
    this->m_ksRc = params.ksRc;
    this->m_ksLd = params.ksLd;
    this->m_ksRd = params.ksRd;
    this->m_ksRs = params.ksRs;

    this->m_sus = params.sus;
    this->m_xof = params.xof;
    this->m_kor = params.kor;

    this->m_bypass = params.bypass;

    if (this->m_curveCore == nullptr || this->m_curveCore->index == 0) {
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
    else {
        if (this->m_rgEnable)
        {
            auto calcLevel = [this](int prmIdx, int value, float maxValue) -> float {
                float normRate = (float)value / maxValue;

                return m_curveCore->process(m_positionIndex, (int)CurveParams::Target::RegValue, prmIdx, normRate);

                };

            this->m_totalLevel = calcLevel((int)CurveParams::TargetRegValue::Tl, this->m_rg.tl, 63.0f);

            this->m_zeroDecay = this->m_rg.d1r == 0;
            // サステインレベル (SL) の計算
            if (this->m_rg.d1l == 15) {
                this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
            }
            else {
                // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
                float baseSustainLevel = calcLevel((int)CurveParams::TargetRegValue::Sl, this->m_rg.d1l, 15.0f);
                float slDb = (baseSustainLevel * 15.0f) * 3.0f;
                this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
            }
        }
        else
        {
            this->m_totalLevel = this->m_real.tl;

            this->m_zeroDecay = this->m_real.d1r == 0;
            // 従来モードのサステインレベルはそのまま適用する
            this->m_sustain = this->m_real.d1l;
        }

        this->updateIncrements(m_noteNumber);
    }
}

// ============================================================================
// レートスケーリング (KSR) の計算
// ============================================================================
int Opzx7Adddr::calcRateScaling() const
{
    if (!this->m_ksEn && !this->m_rgEnable) return 0; // KSRが無効な場合

    if (m_ksMode == Opzx7AdddrKeyScaleMode::OPS)
    {
        // --- OPS (DX7) モード ---
        // Rate Scaling は 0〜7 の値を持つ。高音になるほどレートに加算される
        if (m_ksRs == 0) return 0;
        int octave = (m_noteNumber / 12) - 1;
        if (octave < 0) octave = 0;
        // OPSのKSRの近似計算
        return (octave * m_ksRs) >> 2;
    }
    else if(m_ksMode == Opzx7AdddrKeyScaleMode::MA7)
    {
        // --- MA-7 モード ---
        int octave = (m_noteNumber / 12) - 1;
        if (octave < 0) octave = 0;
        if (octave > 7) octave = 7;

        int noteOffset = m_noteNumber % 12;
        int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
        return this->m_ksrMA7 ? keyRate : (keyRate >> 2);
    }
    else
    {
        // --- OPZ モード ---
        int octave = (m_noteNumber / 12) - 1;
        if (octave < 0) octave = 0;
        if (octave > 7) octave = 7;

        int noteOffset = m_noteNumber % 12;
        int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);

        // OPZのKSR(0〜3)に基づくシフト計算:
        // 0 なら >> 3,  1 なら >> 2,  2 なら >> 1,  3 なら >> 0
        int shift = 3 - std::clamp(this->m_ksrOPZ, 0, 3);
        return keyRate >> shift;
    }
}

// ============================================================================
// レベルスケーリング (KSL) の計算 (dBで返す)
// ============================================================================
float Opzx7Adddr::calcLevelScalingDb() const
{
    if (!this->m_ksEn && !this->m_rgEnable) return 0.0f; // KSLが無効な場合

    if (m_ksMode == Opzx7AdddrKeyScaleMode::OPS)
    {
        // --- OPS (DX7) モード ---
        if (m_noteNumber == m_ksBp) return 0.0f;

        float diff = (float)(m_noteNumber - m_ksBp);
        float x = std::abs(diff) / 12.0f; // 1オクターブを1.0とする

        int curve = (diff < 0.0f) ? (m_ksLc-1) : (m_ksRc-1);
        float depth = (diff < 0.0f) ? m_ksLd : m_ksRd;

        if (depth <= 0.0f) return 0.0f;

        float maxDbPerOct = 48.0f; // 1オクターブあたりの最大変化量
        float db = 0.0f;

        switch (curve) {
        case 0: // -LIN (距離に応じて直線的に減衰)
            db = maxDbPerOct * x * depth;
            break;
        case 1: // -EXP (距離に応じて指数的に減衰)
            db = maxDbPerOct * (std::pow(2.0f, x) - 1.0f) * depth;
            break;
        case 2: // +EXP (距離に応じて指数的に増幅)
            db = -maxDbPerOct * (std::pow(2.0f, x) - 1.0f) * depth;
            break;
        case 3: // +LIN (距離に応じて直線的に増幅)
            db = -maxDbPerOct * x * depth;
            break;
        }
        return db;
    }
    else if(m_ksMode == Opzx7AdddrKeyScaleMode::MA7)
    {
        // --- MA-7 モード ---
        if (this->m_kslMA7 <= 0) return 0.0f;

        // C3(48) を基準とし、それより高い音符で音量を減衰させる
        float octaveDiff = (float)(m_noteNumber - 48) / 12.0f;
        if (octaveDiff < 0.0f) octaveDiff = 0.0f;

        return m_dbPerOctsMA7[std::clamp(this->m_kslMA7, 0, 3)] * octaveDiff;
    }
    else
    {
        // --- OPZ モード ---
        if (this->m_kslOPZ <= 0) return 0.0f;

        // C3(48) を基準とし、それより高い音符で音量を減衰させる
        float octaveDiff = (float)(m_noteNumber - 48) / 12.0f;
        if (octaveDiff < 0.0f) octaveDiff = 0.0f;

        // 0〜99 の値を 0.0〜1.0 の深度(Depth)に変換
        float depth = std::clamp(this->m_kslOPZ, 0, 99) / 99.0f;

        // KSL=99の時の1オクターブあたりの最大減衰量
        // TX81Zは最大でかなり急激に減衰するため 24.0dB / oct 程度で設定
        float maxDbPerOct = 24.0f;

        return maxDbPerOct * octaveDiff * depth;
    }
}

float Opzx7Adddr::noteOn(float velocity) {
    this->m_phaseProgress = 0.0f;

    if (this->m_bypass) {
        return velocity;
    }

    m_state = State::Attack;

    float attenuationDb = 0.0f;

    if (this->m_curveCore == nullptr || this->m_curveCore->index == 0) {
        // TLレジスタ値から直接減衰量(dB)を計算
        // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
        attenuationDb = (m_rgEnable ? m_rg.tl : m_real.tl) * 0.75f;
    }
    else {
        // TLレジスタ値から直接減衰量(dB)を計算
        // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
        attenuationDb = (this->m_totalLevel * 63.0f) * 0.75f;
    }

    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);
    float kslDb = calcLevelScalingDb();

    // マイナス値(+LIN, +EXP等による増幅)でも、最終ゲインが1.0を超えないようサチュレーションさせる
    float totalDb = std::max(0.0f, attenuationDb + kslDb);
    float finalGain = std::pow(10.0f, -totalDb / 20.0f);

    return velocity * tlGain * finalGain;
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

    if (this->m_curveCore == nullptr || this->m_curveCore->index == 0) {
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
    else {
        // ====================================================================
        // レジスタモード (RG-EN = ON) : 実機のアルゴリズムで増減量を計算
        // ====================================================================
        if (m_rgEnable)
        {
            // 2. レジスタ値から実効レート(0~63)を算出し、インクリメントに変換する関数
            auto calcRegRate = [&](int regVal, int regMax, int prmIdx, bool isRR, bool isAttack) -> float {
                if (regVal == 0 && !isRR) return 0.0f;

                if (regVal <= 1 && isRR) {
                    return 1.0f / (Global::RateMaxSeconds::reg * (float)m_sampleRate);
                }

                int effectiveRate = (int)((float)regVal * 63.0 / (float)regMax) + ksrValue;
                if (effectiveRate > 63) effectiveRate = 63;

                float timeInSeconds = isAttack ? attcckTimeInSecondsLut[effectiveRate] : timeInSecondsLut[effectiveRate];
                float normRate = (float)effectiveRate / 63.0f;
                float curveFactor = m_curveCore->process(m_positionIndex, (int)CurveParams::Target::RegValue, prmIdx, normRate);

                // カーブの影響を反映 (0.5倍〜2.0倍の範囲など、調整可能)
                float modulatedTime = timeInSeconds * (2.0f - (curveFactor * 2.0f));
                modulatedTime = std::max(0.00001f, modulatedTime);

                // 最終的なデクリック保証
                if (isRR) {
                    // リリース時は最低 5ms (0.005秒) かけてフェードアウトし、プチプチを消す
                    modulatedTime = std::max(modulatedTime, 0.005f);
                }
                else if (isAttack) {
                    modulatedTime = std::max(modulatedTime, 0.001f);
                }
                else {
                    modulatedTime = std::max(modulatedTime, 0.00001f);
                }

                return 1.0f / (modulatedTime * (float)m_sampleRate);
                };

            // 各レートの計算
            m_attackInc = calcRegRate(m_rg.ar, m_rgMax.ar, (int)CurveParams::TargetRegValue::Ar, false, true);
            m_decayDec = calcRegRate(m_rg.d1r, m_rgMax.d1r, (int)CurveParams::TargetRegValue::Dr, false, false);
            m_sustainRateDec = (m_rg.d2r == 0) ? 0.0f : calcRegRate(m_rg.d2r, m_rgMax.d2r, (int)CurveParams::TargetRegValue::Sr, false, false);
            m_releaseTimeInc = calcRegRate(this->m_sus ? 5 : m_rg.rr, m_rgMax.rr, (int)CurveParams::TargetRegValue::Rr, true, false);
            m_releaseDec = m_kor ? 0.0f : m_releaseTimeInc;
        }
        // ====================================================================
        // 従来モード (RG-EN = OFF) : 既存の秒数ベースの計算
        // ====================================================================
        else
        {
            float rateScale = 1.0f;

            if (this->m_ksEn) {
                // OPLライクなキースケールの計算
                int octave = (noteNumber / 12) - 1;
                if (octave < 0) octave = 0;
                if (octave > 7) octave = 7;

                int noteOffset = noteNumber % 12;
                int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);

                // ksr(1bitフラグ)の状態に応じてスケーリングの強さを変える
                int ksrValue = (this->m_ksrMA7 || this->m_ksrOPZ) ? keyRate : (keyRate >> 2);

                // 実効レート(0-63)のような絶対的なインクリメント加算ではなく、
                // 秒数ベースのモードに合わせて「倍率」として適用するアプローチ

                // 例: ksrValue が大きい（高音）ほど、rateScaleが大きくなり時間が短くなる
                // ※ ここの倍率(0.1fなど)は、聴感上で自然に減衰が早くなるように適宜調整してください。
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
}

float Opzx7Adddr::updateEnvelopeState(float currentLevel)
{
    if (this->m_bypass) {
        return 1.0f;
    }

    if (this->m_curveCore == nullptr || this->m_curveCore->index == 0) {
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
    }
    else {
        float limitLevel = 0.0f;
        float y = 0.0f;
        float startLevel = 0.0f;
        float outLevel = 0.0f;
        float totalDecayRange = 0.0f;

        switch (this->m_state) {
        case State::Idle:
            return currentLevel;
        case State::Attack:
            if (this->m_phaseProgress == 0.0f) {
                this->m_attackStartLevel = currentLevel;
            }

            // ユニゾン・ハーモニー対応

            // 1. まず現在の進行度でカーブを計算する！ (初回は必ず y(0.0) になる)
            y = 0.0f;

            // 開始の瞬間は確実に 0.0f を保証し、カーブ計算の誤差ジャンプを防ぐ
            if (this->m_phaseProgress > 0.0f) {
                y = this->m_curveCore->process(
                    this->m_positionIndex,
                    (int)CurveParams::Target::AmpEnv,
                    (int)CurveParams::TargetAmpEnv::Ar,
                    this->m_phaseProgress);
            }

            outLevel = this->m_attackStartLevel + (1.0f - this->m_attackStartLevel) * y;

            // 2. その後で時間を進める
            this->m_phaseProgress += this->m_attackInc;

            if (this->m_phaseProgress >= 1.0f) {
                this->m_phaseProgress = 0.0f; // Decayに向けて確実に進行度を0にリセット！
                this->m_state = State::Decay;
                outLevel = 1.0f;
            }

            return outLevel;
        case State::Decay:
            limitLevel = this->m_sustain;

            if (this->m_zeroDecay) {
                this->m_state = State::Sustain;
                this->m_phaseProgress = 0.0f;

                return 1.0f;
            }

            totalDecayRange = 1.0f - limitLevel;

            if (totalDecayRange <= 0.001f) {
                this->m_state = State::Sustain;
                this->m_phaseProgress = 0.0f;

                return limitLevel;
            }

            // 1. 時間を進める (decayDec から 0〜1 の進行割合を逆算)
            this->m_phaseProgress += this->m_decayDec;

            if (this->m_phaseProgress >= 1.0f) {
                this->m_phaseProgress = 0.0f; // Sustainに向けて確実に0にリセット！
                this->m_state = State::Sustain;

                return limitLevel; // 最後のサンプルはターゲットレベルをきっちり返す
            }

            // 2. カーブ取得
            y = this->m_curveCore->process(
                this->m_positionIndex,
                (int)CurveParams::Target::AmpEnv,
                (int)CurveParams::TargetAmpEnv::Dr,
                this->m_phaseProgress);

            return 1.0f - (y * totalDecayRange); // 1.0 から SL へ向かって減衰
        case State::Sustain:
            if (this->m_sustainRateDec <= 0.0f || m_sustain <= 0.0001f) {
                return currentLevel; // 減衰しない場合はそのまま維持
            }

            // Sustainフェーズの開始時(m_phaseProgress=0)に、減衰開始の始点を決める
            // ※通常は m_sustain と同じですが、何らかの理由でレベルがずれていた時の安全策
            startLevel = this->m_sustain;

            // ゼロ除算防止
            if (startLevel <= 0.001f) {
                this->m_state = State::Idle;
                this->m_phaseProgress = 0.0f;

                return 0.0f;
            }

            // 1. 時間を進める
            this->m_phaseProgress += this->m_sustainRateDec;

            if (this->m_phaseProgress >= 1.0f) {
                this->m_phaseProgress = 0.0f;
                this->m_state = State::Idle;
                return 0.0f;
            }

            // 2. カーブ取得
            y = this->m_curveCore->process(
                this->m_positionIndex,
                (int)CurveParams::Target::AmpEnv,
                (int)CurveParams::TargetAmpEnv::Sr,
                this->m_phaseProgress
            );

            return startLevel - (y * startLevel); // SL から 0.0 へ向かって減衰
        case State::Release:
            if (this->m_phaseProgress == 0.0f) {
                this->m_releaseStartLevel = currentLevel; // 離鍵時のレベルを記録
            }

            // 0.0f ではなく、0.001f (1000分の1の音量) 以下なら即座に消す。
            // 人間の耳には聞こえないレベルであり、かつゼロ除算(Infinity爆発)を完全に防ぐための必須の措置です。
            if (this->m_releaseStartLevel <= 0.001f) {
                this->m_state = State::Idle;
                this->m_phaseProgress = 0.0f;
                return 0.0f;
            }

            // 1. 時間を進める
            this->m_phaseProgress += this->m_releaseDec;

            if (this->m_phaseProgress >= 1.0f) {
                this->m_phaseProgress = 0.0f;
                this->m_state = State::Idle;
                return 0.0f;
            }

            if (m_kor) {
                return this->m_releaseStartLevel;
            }

            // 2. カーブ取得
            y = this->m_curveCore->process(
                this->m_positionIndex,
                (int)CurveParams::Target::AmpEnv,
                (int)CurveParams::TargetAmpEnv::Rr,
                this->m_phaseProgress);

            return this->m_releaseStartLevel - (y * this->m_releaseStartLevel);
        }
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
