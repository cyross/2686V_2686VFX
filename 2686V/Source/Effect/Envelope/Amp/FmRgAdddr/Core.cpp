#include <algorithm>
#include <cmath>

#include "./Core.h"

FmRgAdddr::FmRgAdddr()
{
	this->setParameterFunctions = std::array<std::function<void(const FmRgAdddrParams&)>, 2>{
		[this](const FmRgAdddrParams& params) { this->setParametersLinear(params); },
		[this](const FmRgAdddrParams& params) { this->setParametersCurve(params); }
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

void FmRgAdddr::prepare(int posIndex, double sampleRate) {
    this->positionIndex = posIndex;

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
    if (this->m_curveCore == nullptr) {
        this->setParametersLinear(params);

        return;
    }

    this->setParameterFunctions[this->m_curveCore->index](params);
}

float FmRgAdddr::noteOn(float velocity) {
    if (this->m_curveCore == nullptr) {
        return this->noteOnLinear(velocity);
    }

    return this->noteOnFunctions[this->m_curveCore->index](velocity);
}

void FmRgAdddr::noteOff() {
    if (this->m_curveCore == nullptr) {
        this->noteOffLinear();

        return;
    }

    this->noteOffFunctions[this->m_curveCore->index]();
}

void FmRgAdddr::updateIncrementsWithKeyScale(int noteNumber)
{
    m_noteNumber = noteNumber;

    if (this->m_curveCore == nullptr) {
        this->updateIncrementsWithKeyScaleLinear(noteNumber);

        return;
    }

    this->updateIncrementsWithKeyScaleFunctions[this->m_curveCore->index](noteNumber);
}

float FmRgAdddr::updateEnvelopeState(float currentLevel)
{
    if (this->m_curveCore == nullptr) {
        return this->updateEnvelopeStateLinear(currentLevel);
    }

    return this->updateEnvelopeStateFunctions[this->m_curveCore->index](currentLevel);
}

void FmRgAdddr::setParametersLinear(const FmRgAdddrParams& params) {
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

    updateIncrementsWithKeyScaleLinear(m_noteNumber);
}

float FmRgAdddr::noteOnLinear(float velocity) {
    // TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = tl * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    state = State::Attack;

    return velocity * tlGain;
}

void FmRgAdddr::noteOffLinear() {
    state = State::Release;

    currentReleaseDec = releaseDec;
}

void FmRgAdddr::updateIncrementsWithKeyScaleLinear(int noteNumber)
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

            // 安全装置: 最大時間を10秒程度でクリップする
            // (レジスタ値1で5秒を想定するなら、最大10〜15秒あれば十分)
            timeInSeconds = std::min(timeInSeconds, 15.0f);
        }

        return 1.0f / (timeInSeconds * (float)sampleRate);
        };

    // 各レートの計算
    attackInc = calcRegRate(ar, arMax, false, true);
    decayDec = calcRegRate(d1r, d1rMax, false, false);
    sustainRateDec = (d2r == 0) ? 0.0f : calcRegRate(d2r, d2rMax, false, false);
    releaseDec = calcRegRate(rr, rrMax, true, false);
    susReleaseDec = calcRegRate(5, d2rMax, true, false);

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

float FmRgAdddr::updateEnvelopeStateLinear(float currentLevel)
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

void FmRgAdddr::setParametersCurve(const FmRgAdddrParams& params) {
    this->ar = params.ar;
    this->d1r = params.d1r;
    this->d1l = params.d1l;
    this->d2r = params.d2r;
    this->rr = params.rr;
    this->ks = params.ks;

    auto calcLevel = [this](int prmIdx, int value, float maxValue) -> float {
        float normRate = (float)value / maxValue;

        return m_curveCore->process(positionIndex, (int)CurveParams::Target::RegValue, prmIdx, normRate);

        };

    this->totalLevel = calcLevel((int)CurveParams::TargetRegValue::Tl, this->tl, 127.0f);

    this->m_zeroDecay = this->d1r == 0;
    // サステインレベル (SL) の計算
    if (this->d1l == 15) {
        this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        float baseSustainLevel = calcLevel((int)CurveParams::TargetRegValue::Sl, this->d1l, 15.0f);
        float slDb = (baseSustainLevel * 15.0f) * 3.0f;
        this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }

    updateIncrementsWithKeyScaleCurve(m_noteNumber);
}

float FmRgAdddr::noteOnCurve(float velocity) {
    // TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = (this->totalLevel * 127.0f) * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    this->m_phaseProgress = 0.0f;

    state = State::Attack;

    return velocity * tlGain;
}

void FmRgAdddr::noteOffCurve() {
    state = State::Release;

    this->m_phaseProgress = 0.0f; // フェーズ時間のリセット
}

void FmRgAdddr::updateIncrementsWithKeyScaleCurve(int noteNumber)
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
    auto calcRegRate = [&](int regVal, int regMax, int prmIdx, bool isRR, bool isAttack) -> float {
        if (regVal == 0 && !isRR) return 0.0f;

        if (regVal <= 1 && isRR) {
            return 1.0f / (5.0f * (float)sampleRate);
        }

        int effectiveRate = (int)((float)regVal * 63.0 / (float)regMax) + ksrValue;
        if (effectiveRate > 63) effectiveRate = 63;

        float timeInSeconds = 0.0f;
        if (effectiveRate >= 60) {
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

            timeInSeconds = std::min(timeInSeconds, 15.0f);
        }

        float normRate = (float)effectiveRate / 63.0f;
        float curveFactor = m_curveCore->process(positionIndex, (int)CurveParams::Target::RegValue, prmIdx, normRate);

        // カーブの影響を反映 (0.5倍〜2.0倍の範囲など、調整可能)
        float modulatedTime = timeInSeconds * (2.0f - (curveFactor * 2.0f));
        modulatedTime = std::max(0.00001f, modulatedTime);

        return 1.0f / (modulatedTime * (float)sampleRate);
        };

    // 各レートの計算
    attackInc = calcRegRate(ar, arMax, (int)CurveParams::TargetRegValue::Ar, false, true);
    decayDec = calcRegRate(d1r, d1rMax, (int)CurveParams::TargetRegValue::Dr, false, false);
    sustainRateDec = (d2r == 0) ? 0.0f : calcRegRate(d2r, d2rMax, (int)CurveParams::TargetRegValue::Sr, false, false);
    releaseDec = calcRegRate(rr, rrMax, (int)CurveParams::TargetRegValue::Rr, true, false);

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

float FmRgAdddr::updateEnvelopeStateCurve(float currentLevel)
{
    if (this->m_curveCore == nullptr) {
        return updateEnvelopeStateLinear(currentLevel);
    }

    int targetIdx = (int)CurveParams::Target::RegValue;
    int posIdx = this->positionIndex; // 1, 2, 3, 4

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

        float deltaX = this->sustainRateDec / m_sustain;
        this->m_phaseProgress += deltaX;

        if (this->m_phaseProgress >= 1.0f) {
            this->state = State::Idle;
            return 0.0f;
        }

        int prmIdx = (int)CurveParams::TargetRegValue::Sr;
        float y = this->m_curveCore->process(posIdx, targetIdx, prmIdx, this->m_phaseProgress);
        return m_sustain - (y * m_sustain);
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

void FmRgAdddr::setParamMax(int ar, int d1r, int d2r, int d1l, int rr, int tl) {
    arMax = ar;
    d1rMax = d1r;
    d2rMax = d2r;
    d1lMax = d1l;
    rrMax = rr;
    tlMax = tl;
}
