#include <algorithm>
#include <cmath>

#include "./Core.h"

FmRgAdssr::FmRgAdssr()
{
	this->setParameterFunctions = std::array<std::function<void(const FmRgAdssrParams&)>, 2>{
		[this](const FmRgAdssrParams& params) { this->setParametersLinear(params); },
		[this](const FmRgAdssrParams& params) { this->setParametersCurve(params); }
	};
	this->noteOnFunctions = std::array<std::function<float(float)>, 2>{
		[this](float velocity) { return this->noteOnLinear(velocity); },
		[this](float velocity) { return this->noteOnCurve(velocity); }
	};
	this->noteOffFunctions = std::array<std::function<void()>, 2>{
		[this]() { this->noteOffLinear(); },
		[this]() { this->noteOffCurve(); }
	};
	this->updateIncrementsWithKeyScaleFunctions = std::array<std::function<void(int)>, 2>{
		[this](int noteNumber) { this->updateIncrementsWithKeyScaleLinear(noteNumber); },
		[this](int noteNumber) { this->updateIncrementsWithKeyScaleCurve(noteNumber); }
	};
	this->updateEnvelopeStateFunctions = std::array<std::function<float(float)>, 2>{
		[this](float currentLevel) { return this->updateEnvelopeStateLinear(currentLevel); },
		[this](float currentLevel) { return this->updateEnvelopeStateCurve(currentLevel); }
	};
}

void FmRgAdssr::prepare(int targetIndex, double sampleRate) {
    this->targetIndex = targetIndex;

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
    if (this->m_curveCore == nullptr) {
        this->setParametersLinear(params);

        return;
    }

    this->setParameterFunctions[this->m_curveCore->index](params);
}

float FmRgAdssr::noteOn(float velocity) {
    if (this->m_curveCore == nullptr) {
        return this->noteOnLinear(velocity);
    }

    return this->noteOnFunctions[this->m_curveCore->index](velocity);
}

void FmRgAdssr::noteOff() {
    if (this->m_curveCore == nullptr) {
        this->noteOffLinear();

        return;
    }

    this->noteOffFunctions[this->m_curveCore->index]();
}

void FmRgAdssr::updateIncrementsWithKeyScale(int noteNumber)
{
    if (this->m_curveCore == nullptr) {
        this->updateIncrementsWithKeyScaleLinear(noteNumber);

        return;
    }

    this->updateIncrementsWithKeyScaleFunctions[this->m_curveCore->index](noteNumber);
}

float FmRgAdssr::updateEnvelopeState(float currentLevel)
{
    if (this->m_curveCore == nullptr) {
        return this->updateEnvelopeStateLinear(currentLevel);
    }

    return this->updateEnvelopeStateFunctions[this->m_curveCore->index](currentLevel);
}

void FmRgAdssr::setParametersLinear(const FmRgAdssrParams& params) {
    this->ar = params.ar;
    this->dr = params.dr;
    this->sl = params.sl;
    this->rr = params.rr;
    this->tl = params.tl;
    this->ks = params.ks;

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

float FmRgAdssr::noteOnLinear(float velocity) {
    // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = tl * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    float kslAttenuation = 1.0f;

    state = State::Attack;

    return velocity * tlGain * kslAttenuation;
}

void FmRgAdssr::noteOffLinear() {
    state = State::Release;

    currentReleaseDec = releaseDec;
}

void FmRgAdssr::updateIncrementsWithKeyScaleLinear(int noteNumber)
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
    decayDec = calcRegRate(dr, false);
    sustainRateDec = (sr == 0) ? 0.0f : calcRegRate(sr, false);

    // Release Rate の計算時のみ isRR を true にする
    releaseDec = calcRegRate(rr, true);

    susReleaseDec = calcRegRate(5, true);

    // 3. サステインレベル (SL) の計算
    if (sl == 15) {
        m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float slDb = sl * 3.0f;
        m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }

    currentReleaseDec = releaseDec;
}

float FmRgAdssr::updateEnvelopeStateLinear(float currentLevel)
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

void FmRgAdssr::setParametersCurve(const FmRgAdssrParams& params) {
    this->ar = params.ar;
    this->dr = params.dr;
    this->sl = params.sl;
    this->rr = params.rr;
    this->tl = params.tl;
    this->ks = params.ks;

    auto calcLevel = [this](int prmIdx, int value, float maxValue) -> float {
        float normRate = (float)value / maxValue;

        return m_curveCore->process(targetIndex, (int)CurveParams::Target::RegValue, prmIdx, normRate);

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
}

float FmRgAdssr::noteOnCurve(float velocity) {
    // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = (this->totalLevel * 127.0f) * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    float kslAttenuation = 1.0f;

    this->m_phaseProgress = 0.0f;

    state = State::Attack;

    return velocity * tlGain * kslAttenuation;
}

void FmRgAdssr::noteOffCurve() {
    state = State::Release;

    this->m_phaseProgress = 0.0f; // フェーズ時間のリセット
}

void FmRgAdssr::updateIncrementsWithKeyScaleCurve(int noteNumber)
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
    auto calcRegRate = [&](int regVal, int prmIdx, bool isRR) -> float {
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

        // 1. 実機ベースの標準的な時間を算出
        float timeInSeconds = 0.0f;
        if (effectiveRate >= 60) {
            timeInSeconds = 0.001f;
        }
        else {
            float powFactor = (60.0f - (float)effectiveRate) / 4.0f;
            timeInSeconds = 0.0015f * std::pow(2.0f, powFactor);
        }

        // 2. カーブコアの出力(0.0~1.0)を使って、実機時間を「拡大・縮小」する
        // カーブの結果(0.0~1.0)が 0.5 なら、時間を半分(速く)にするなど
        float normRate = (float)effectiveRate / 63.0f;
        float curveFactor = m_curveCore->process(targetIndex, (int)CurveParams::Target::RegValue, prmIdx, normRate);

        // カーブの影響を反映させる (例えば 0.5倍〜2.0倍の範囲で変動させる)
        float modulatedTime = timeInSeconds * (2.0f - (curveFactor * 2.0f));

        // 安全装置: 0除算防止
        modulatedTime = std::max(0.00001f, modulatedTime);

        return std::min(0.1f, 1.0f / (modulatedTime * (float)sampleRate));
        };

    // 各レートの計算（第2引数に、それがRRかどうかのフラグを渡す）
    attackInc = calcRegRate(ar, (int)CurveParams::TargetRegValue::Ar, false);
    decayDec = calcRegRate(dr, (int)CurveParams::TargetRegValue::Dr, false);
    sustainRateDec = (sr == 0) ? 0.0f : calcRegRate(sr, (int)CurveParams::TargetRegValue::Sr, false);

    // Release Rate の計算時のみ isRR を true にする
    releaseDec = calcRegRate(rr, (int)CurveParams::TargetRegValue::Rr, true);

    susReleaseDec = calcRegRate(5, (int)CurveParams::TargetRegValue::Sr, true);

    // 3. サステインレベル (SL) の計算
    if (sl == 15) {
        m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float slDb = sl * 3.0f;
        m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }

    currentReleaseDec = releaseDec;
}

float FmRgAdssr::updateEnvelopeStateCurve(float currentLevel)
{
    if (this->m_curveCore == nullptr) {
        return updateEnvelopeStateLinear(currentLevel);
    }

    int targetIdx = (int)CurveParams::Target::RegValue;
    int posIdx = this->targetIndex; // 1, 2, 3, 4

    // -------------------------------------------------------------
    // Attack Phase (0.0 -> 1.0)
    // -------------------------------------------------------------
    if (this->state == State::Attack) {
        // 1. 時間を進める
        this->m_phaseProgress += this->attackInc;

        if (this->m_phaseProgress >= 1.0f) {
            this->m_phaseProgress = 1.0f;
            this->state = State::Decay;
        }

        // 2. カーブ取得
        int prmIdx = (int)CurveParams::TargetRegValue::Ar;
        float y = this->m_curveCore->process(posIdx, targetIdx, prmIdx, this->m_phaseProgress);

        return y; // 0.0 から 1.0 へ向かう
    }
    // -------------------------------------------------------------
    // Decay 1 Phase (1.0 -> SL)
    // -------------------------------------------------------------
    else if (this->state == State::Decay) {
        float limitLevel = this->m_sustain;

        if (this->m_zeroDecay) {
            this->state = State::Sustain;
            this->m_phaseProgress = 0.0f;
            return 1.0f;
        }

        float totalDecayRange = 1.0f - limitLevel;
        if (totalDecayRange <= 0.0f) {
            this->state = State::Sustain;
            this->m_phaseProgress = 0.0f;
            return limitLevel;
        }

        // 1. 時間を進める (decayDec から 0〜1 の進行割合を逆算)
        float deltaX = this->decayDec / totalDecayRange;
        this->m_phaseProgress += deltaX;

        if (this->m_phaseProgress >= 1.0f) {
            this->m_phaseProgress = 1.0f;
            this->state = State::Sustain;
            this->m_phaseProgress = 0.0f; // 次のフェーズのためにリセット
        }

        // 2. カーブ取得
        int prmIdx = (int)CurveParams::TargetRegValue::Dr; // D1R
        float y = this->m_curveCore->process(posIdx, targetIdx, prmIdx, this->m_phaseProgress);

        return 1.0f - (y * totalDecayRange); // 1.0 から SL へ向かって減衰
    }
    // -------------------------------------------------------------
    // Decay 2 / Sustain Phase (SL -> 0.0)
    // -------------------------------------------------------------
    else if (this->state == State::Sustain) {
        if (this->sustainRateDec <= 0.0f || m_sustain <= 0.0001f) {
            return currentLevel; // 減衰しない場合はそのまま維持
        }

        // Sustainフェーズの開始時(m_phaseProgress=0)に、減衰開始の始点を決める
        // ※通常は m_sustain と同じですが、何らかの理由でレベルがずれていた時の安全策
        float startLevel = this->m_sustain;

        if (startLevel <= 0.0f) {
            this->state = State::Idle;
            return 0.0f;
        }

        // 1. 時間を進める
        float deltaX = this->sustainRateDec / startLevel;
        this->m_phaseProgress += deltaX;

        if (this->m_phaseProgress >= 1.0f) {
            this->m_phaseProgress = 1.0f;
            this->state = State::Idle;
            return 0.0f;
        }

        // 2. カーブ取得
        int prmIdx = (int)CurveParams::TargetRegValue::Sr; // D2R / SR
        float y = this->m_curveCore->process(posIdx, targetIdx, prmIdx, this->m_phaseProgress);

        return startLevel - (y * startLevel); // SL から 0.0 へ向かって減衰
    }
    // -------------------------------------------------------------
    // Release Phase (Current -> 0.0)
    // -------------------------------------------------------------
    else if (this->state == State::Release) {
        if (this->m_phaseProgress == 0.0f) {
            this->m_releaseStartLevel = currentLevel; // 離鍵時のレベルを記録
        }

        if (this->m_releaseStartLevel <= 0.0f) {
            this->state = State::Idle;
            return 0.0f;
        }

        // 1. 時間を進める
        float deltaX = this->currentReleaseDec / this->m_releaseStartLevel;
        this->m_phaseProgress += deltaX;

        if (this->m_phaseProgress >= 1.0f) {
            this->m_phaseProgress = 1.0f;
            this->state = State::Idle;
            return 0.0f;
        }

        // 2. カーブ取得
        int prmIdx = (int)CurveParams::TargetRegValue::Rr;
        float y = this->m_curveCore->process(posIdx, targetIdx, prmIdx, this->m_phaseProgress);

        return this->m_releaseStartLevel - (y * this->m_releaseStartLevel);
    }

    return currentLevel;
}
