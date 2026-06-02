#include <algorithm>
#include <cmath>

#include "./Core.h"

FmRgAdssr::FmRgAdssr()
{
    this->setParameterFunctions = std::array<std::function<void(const FmRgAdssrParams&)>, 2>{
		[this](const FmRgAdssrParams& params) {
            // サステインレベル (SL) の計算
            if (this->sl == 15) {
                this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
            }
            else {
                // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
                float slDb = this->sl * 3.0f;
                this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
            }

            this->updateIncrementsWithKeyScaleFunctions[0](m_noteNumber);
        },
		[this](const FmRgAdssrParams& params) {
            auto calcLevel = [this](int prmIdx, int value, float maxValue) -> float {
                float normRate = (float)value / maxValue;

                return m_curveCore->process(positionIndex, (int)CurveParams::Target::RegValue, prmIdx, normRate);

                };

            this->totalLevel = calcLevel((int)CurveParams::TargetRegValue::Tl, this->tl, 127.0f);

            this->m_zeroDecay = this->dr == 0;
            // サステインレベル (SL) の計算
            if (this->sl == 15) {
                this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
            }
            else {
                float baseSustainLevel = calcLevel((int)CurveParams::TargetRegValue::Sl, this->sl, 15.0f);
                float slDb = (baseSustainLevel * 15.0f) * 3.0f;
                this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
            }

            this->updateIncrementsWithKeyScaleFunctions[1](m_noteNumber);
        }
	};

    this->noteOnFunctions = std::array<std::function<float(float)>, 2>{
		[this](float velocity) {
            // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
            // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
            float attenuationDb = tl * 0.75f;
            float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

            return velocity * tlGain;
        },
		[this](float velocity) { 
            // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
            // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
            float attenuationDb = (this->totalLevel * 127.0f) * 0.75f;
            float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

            return velocity * tlGain;
        }
	};

    this->updateIncrementsWithKeyScaleFunctions = std::array<std::function<void(int)>, 2>{
		[this](int noteNumber) {
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

                float timeInSeconds = 0.0f;
                if (effectiveRate >= 60) {
                    // Rate 60以上はほぼ瞬時（1ミリ秒）
                    timeInSeconds = 0.001f;
                }
                else {
                    // 実機(OPN/OPM)仕様に忠実な「レートが4下がるごとに時間が2倍」の式
                    // effectiveRate が 60 のときに 0.001秒(1ms) とする基準から、
                    // ご要望の「regVal=1(実効レート2〜3)の時に約5秒」に合うように基準時間を調整します。

                    // diff は 60 を基準とした「どれだけレートが低いか」
                    float diff = 60.0f - (float)effectiveRate;

                    // レートが4下がるごとに2倍（= pow(2.0, diff / 4.0)）
                    float timeFactor = std::pow(2.0f, diff / 4.0f);

                    // 基準時間 (Rate60 の時の時間) を調整。
                    // 0.0003f 程度にすると、effectiveRate=2 の時 (diff=58, factor≒23170) で
                    // 0.0003 * 23170 = 約 6.9秒 になります。
                    float baseTime = 0.0003f;

                    timeInSeconds = baseTime * timeFactor;

                    // アタック時はディケイよりも早く完了するように調整 (例: 1/3の時間)
                    if (isAttack) {
                        timeInSeconds *= 0.33f;
                    }

                    timeInSeconds = std::min(timeInSeconds, 15.0f) * 1.5f;
                }

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
            releaseDec = calcRegRate(rr, rrMax, true, false);
        },
		[this](int noteNumber) {
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
            auto calcRegRate = [&](int regVal, int regMax, int prmIdx, bool isRR, bool isAttack) -> float {
                if (regVal == 0 && !isRR) return 0.0f;

                if (regVal <= 1 && isRR) {
                    return 1.0f / (5.0f * (float)sampleRate);
                }

                int effectiveRate = (int)((float)regVal * 63.0 / (float)regMax) + ksrValue;
                if (effectiveRate > 63) effectiveRate = 63;

                float timeInSeconds = 0.0f;
                if (effectiveRate >= 60) {
                    // Rate 60以上はほぼ瞬時（1ミリ秒）
                    timeInSeconds = 0.001f;
                }
                else {
                    // 実機(OPN/OPM)仕様に忠実な「レートが4下がるごとに時間が2倍」の式
                    float diff = 60.0f - (float)effectiveRate;
                    float timeFactor = std::pow(2.0f, diff / 4.0f);
                    float baseTime = 0.0003f;
                    timeInSeconds = baseTime * timeFactor;

                    if (isAttack) {
                        timeInSeconds *= 0.33f;
                    }

                    timeInSeconds = std::min(timeInSeconds, 15.0f) * 1.5f;
                }

                float normRate = (float)effectiveRate / 63.0f;
                float curveFactor = m_curveCore->process(positionIndex, (int)CurveParams::Target::RegValue, prmIdx, normRate);

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

                return 1.0f / (modulatedTime * (float)sampleRate);
                };

            // 各レートの計算
            attackInc = calcRegRate(ar, arMax, (int)CurveParams::TargetRegValue::Ar, false, true);
            decayDec = calcRegRate(dr, drMax, (int)CurveParams::TargetRegValue::Dr, false, false);
            sustainRateDec = (sr == 0) ? 0.0f : calcRegRate(sr, srMax, (int)CurveParams::TargetRegValue::Sr, false, false);
            releaseDec = calcRegRate(rr, rrMax, (int)CurveParams::TargetRegValue::Rr, true, false);
        }
	};

    this->updateEnvelopeStateFunctions = std::array<std::array<std::function<float(float)>, 5>, 2>{
        std::array<std::function<float(float)>, 5>{
            [this](float newLevel) { // Idle
                return newLevel;
            },
            [this](float newLevel) { // Attack
                newLevel += attackInc;

                if (newLevel >= 1.0f) {
                    newLevel = 1.0f;
                    state = State::Decay;
                }

                return newLevel;
            },
            [this](float newLevel) { // Decay
                float limitLevel = m_sustain;

                // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
                if (m_zeroDecay)
                {
                    newLevel = 1.0;
                    state = State::Sustain;
                }
                else if (newLevel > limitLevel) {
                    if (decayDec > 0.0f) {
                        newLevel -= decayDec;
                        if (newLevel <= limitLevel) {
                            newLevel = limitLevel;
                            state = State::Sustain;
                        }
                    }
                }
                else {
                    newLevel = limitLevel;
                    state = State::Sustain;
                }

                return newLevel;
            },
            [this](float newLevel) { // Sustain
                // SR(Sustain Rate / OPMではD2R) でゆっくり減衰する
                if (sustainRateDec > 0.0f) {
                    newLevel -= sustainRateDec;

                    if (newLevel <= 0.001f) {
                        newLevel = 0.0f;
                        state = State::Idle;
                    }
                }

                return newLevel;
            },
            [this](float newLevel) { // Release
                newLevel -= releaseDec;

                if (newLevel <= 0.001f) {
                    newLevel = 0.0f;
                    state = State::Idle;
                }

                return newLevel;
            }
        },
        std::array<std::function<float(float)>, 5>{
            [this](float newLevel) { // Idle
                return newLevel;
            },
            [this](float newLevel) { // Attack
                if (this->m_phaseProgress == 0.0f) {
                    this->m_attackStartLevel = newLevel;
                }

                float y = 0.0f;

                // 開始の瞬間は確実に 0.0f を保証し、カーブ計算の誤差ジャンプを防ぐ
                if (this->m_phaseProgress > 0.0f) {
                    y = this->m_curveCore->process(
                        this->positionIndex,
                        (int)CurveParams::Target::AmpEnv,
                        (int)CurveParams::TargetAmpEnv::Ar,
                        this->m_phaseProgress
                    );
                }

                float outLevel = this->m_attackStartLevel + (1.0f - this->m_attackStartLevel) * y;

                this->m_phaseProgress += this->attackInc;

                if (this->m_phaseProgress >= 1.0f) {
                    this->m_phaseProgress = 0.0f; // Decayに向けて確実に進行度を0にリセット！
                    this->state = State::Decay;
                    outLevel = 1.0f;
                }

                return outLevel;
            },
            [this](float newLevel) { // Decay
                float limitLevel = this->m_sustain;

                if (this->m_zeroDecay) {
                    this->state = State::Sustain;
                    this->m_phaseProgress = 0.0f;
                    return 1.0f;
                }

                float totalDecayRange = 1.0f - limitLevel;

                if (totalDecayRange <= 0.001f) {
                    this->state = State::Sustain;
                    this->m_phaseProgress = 0.0f;
                    return limitLevel;
                }

                // 1. 時間を進める
                float deltaX = this->decayDec;
                this->m_phaseProgress += deltaX;

                if (this->m_phaseProgress >= 1.0f) {
                    this->m_phaseProgress = 0.0f; // Sustainに向けて確実に0にリセット！
                    this->state = State::Sustain;

                    return limitLevel; // 最後のサンプルはターゲットレベルをきっちり返す
                }

                // 2. カーブ取得
                float y = this->m_curveCore->process(
                    this->positionIndex,
                    (int)CurveParams::Target::AmpEnv,
                    (int)CurveParams::TargetAmpEnv::Dr,
                    this->m_phaseProgress
                );

                return 1.0f - (y * totalDecayRange); // 1.0 から SL へ向かって減衰
            },
            [this](float newLevel) { // Sustain
                if (this->sustainRateDec <= 0.0f || m_sustain <= 0.0001f) {
                    return newLevel; // 減衰しない場合はそのまま維持
                }

                // Sustainフェーズの開始時(m_phaseProgress=0)に、減衰開始の始点を決める
                // ※通常は m_sustain と同じですが、何らかの理由でレベルがずれていた時の安全策
                float startLevel = this->m_sustain;

                // ゼロ除算防止
                if (startLevel <= 0.001f) {
                    this->state = State::Idle;
                    this->m_phaseProgress = 0.0f;

                    return 0.0f;
                }

                // 1. 時間を進める
                float deltaX = this->sustainRateDec;
                this->m_phaseProgress += deltaX;

                if (this->m_phaseProgress >= 1.0f) {
                    this->m_phaseProgress = 0.0f;
                    this->state = State::Idle;
                    return 0.0f;
                }

                // 2. カーブ取得
                float y = this->m_curveCore->process(
                    this->positionIndex,
                    (int)CurveParams::Target::AmpEnv,
                    (int)CurveParams::TargetAmpEnv::Sr,
                    this->m_phaseProgress
                );

                return startLevel - (y * startLevel); // SL から 0.0 へ向かって減衰
            },
            [this](float newLevel) { // Release
                if (this->m_phaseProgress == 0.0f) {
                    this->m_releaseStartLevel = newLevel; // 離鍵時のレベルを記録
                }

                // 0.0f ではなく、0.001f (1000分の1の音量) 以下なら即座に消す。
                // 人間の耳には聞こえないレベルであり、かつゼロ除算(Infinity爆発)を完全に防ぐための必須の措置です。
                if (this->m_releaseStartLevel <= 0.001f) {
                    this->state = State::Idle;
                    this->m_phaseProgress = 0.0f;
                    return 0.0f;
                }

                // 1. 時間を進める
                float deltaX = this->releaseDec;
                this->m_phaseProgress += deltaX;

                if (this->m_phaseProgress >= 1.0f) {
                    this->m_phaseProgress = 0.0f;
                    this->state = State::Idle;
                    return 0.0f;
                }

                // 2. カーブ取得
                float y = this->m_curveCore->process(
                    this->positionIndex,
                    (int)CurveParams::Target::AmpEnv,
                    (int)CurveParams::TargetAmpEnv::Rr,
                    this->m_phaseProgress
                );

                return this->m_releaseStartLevel - (y * this->m_releaseStartLevel);
            }
        }
    };
}

void FmRgAdssr::prepare(int posIndex, double sampleRate) {
    this->positionIndex = posIndex;

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
    this->ks = params.ks;
    this->xof = params.xof;

    this->bypass = params.bypass;

    this->m_zeroDecay = this->dr == 0;

    this->setParameterFunctions[this->m_curveCore == nullptr ? 0 : this->m_curveCore->index](params);
}

float FmRgAdssr::noteOn(float velocity) {
    this->m_phaseProgress = 0.0f;

    if (this->bypass) {
        return velocity;
    }

    state = State::Attack;

    return this->noteOnFunctions[this->m_curveCore == nullptr ? 0 : this->m_curveCore->index](velocity);
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

    this->updateIncrementsWithKeyScaleFunctions[this->m_curveCore == nullptr ? 0 : this->m_curveCore->index](noteNumber);
}

float FmRgAdssr::updateEnvelopeState(float currentLevel)
{
    if (this->bypass) {
        return 1.0f;
    }

    return this->updateEnvelopeStateFunctions[this->m_curveCore == nullptr ? 0 : this->m_curveCore->index][(int)this->state](currentLevel);
}

void FmRgAdssr::setParamMax(int ar, int dr, int sr, int sl, int rr, int tl) {
    arMax = ar;
    drMax = dr;
    srMax = sr;
    slMax = sl;
    rrMax = rr;
    tlMax = tl;
}
