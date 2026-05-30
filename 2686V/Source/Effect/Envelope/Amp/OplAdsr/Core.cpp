#include <algorithm>
#include <cmath>

#include "./Core.h"

const std::array<float, 4> OplAdsr::dbPerOcts = { 0.0f, 1.5f, 3.0f, 6.0f };

OplAdsr::OplAdsr()
{
	this->setParameterFunctions = std::array<std::function<void(const OplAdsrParams&)>, 2>{
		[this](const OplAdsrParams& params) { this->setParametersLinear(params); },
		[this](const OplAdsrParams& params) { this->setParametersCurve(params); }
	};
	this->noteOnFunctions = std::array<std::function<float(float, int)>, 2>{
		[this](float velocity, int noteNumber) { return this->noteOnLinear(velocity, noteNumber); },
		[this](float velocity, int noteNumber) { return this->noteOnCurve(velocity, noteNumber); }
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

void OplAdsr::prepare(int posIndex, double sampleRate) {
    this->positionIndex = posIndex;

    updateTargetSampleRate(sampleRate);
}

void OplAdsr::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = newSampleRate;
}

void OplAdsr::updateSampleRate(double newSampleRate) {
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
    this->xof = params.xof;

    this->bypass = params.bypass;
    this->state = this->bypass ? State::Bypass : State::Idle;

    this->m_zeroDecay = this->dr == 0;

    if (this->m_curveCore == nullptr) {
        this->setParametersLinear(params);

        return;
    }

    this->setParameterFunctions[this->m_curveCore->index](params);
}

float OplAdsr::noteOn(float velocity, int noteNumber) {
    m_noteNumber = noteNumber;

    this->m_phaseProgress = 0.0f;

    if (this->state == State::Bypass) {
        return velocity;
    }

    this->state = State::Attack;

    if (this->m_curveCore == nullptr) {
        return this->noteOnLinear(velocity, noteNumber);
    }

    return this->noteOnFunctions[this->m_curveCore->index](velocity, noteNumber);
}

void OplAdsr::noteOff() {
    // XOF/Bypassが有効なときはノートオフ処理を無効化
    if (xof || bypass)
    {
        return;
    }

    this->state = State::Release;

    this->m_phaseProgress = 0.0f; // フェーズ時間のリセット
}

void OplAdsr::updateIncrementsWithKeyScale(int noteNumber)
{
    m_noteNumber = noteNumber;

    if (sampleRate <= 0.0)
    {
        return;
    }

    if (this->m_curveCore == nullptr) {
        this->updateIncrementsWithKeyScaleLinear(noteNumber);

        return;
    }

    this->updateIncrementsWithKeyScaleFunctions[this->m_curveCore->index](noteNumber);
}

float OplAdsr::updateEnvelopeState(float currentLevel)
{
    if (this->bypass) {
        return 1.0f;
    }

    if (this->m_curveCore == nullptr) {
        return this->updateEnvelopeStateLinear(currentLevel);
    }

    return this->updateEnvelopeStateFunctions[this->m_curveCore->index](currentLevel);
}

void OplAdsr::setParametersLinear(const OplAdsrParams& params) {
    // サステインレベル (SL) の計算
    if (this->sl == 15) {
        this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float slDb = this->sl * 3.0f;
        this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }

    updateIncrementsWithKeyScaleLinear(m_noteNumber);
}

float OplAdsr::noteOnLinear(float velocity, int noteNumber) {
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

    return velocity * tlGain * kslAttenuation;
}

void OplAdsr::updateIncrementsWithKeyScaleLinear(int noteNumber)
{
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
            timeInSeconds = std::min(timeInSeconds, 15.0f) * 1.5f;
        }

        return 1.0f / (timeInSeconds * (float)sampleRate);
        };

    // 各レートの計算
    this->attackInc = calcRegRate(this->ar, this->arMax, false, true);
    this->decayDec = calcRegRate(this->dr, this->drMax, false, false);
    this->sustainRateDec = 0.0f;
    this->releaseDec = calcRegRate(this->sus ? 5 : this->rr, this->rrMax, true, false);

    // 3. サステインレベル (SL) の計算
    if (sl == 15) {
        this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float slDb = this->sl * 3.0f;
        this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }
}

float OplAdsr::updateEnvelopeStateLinear(float currentLevel)
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
        if (this->egType) { // EG-TYPE = 1 (持続音／サステイン・タイプ)
            // 何もしない
        }
        else { // EG-TYPE = 0 (減衰音／パーカッシブ・タイプ)
            currentLevel -= this->releaseDec;
            if (currentLevel <= 0.0f) { currentLevel = 0.0f; this->state = State::Idle; }
        }
    }
    else if (this->state == State::Release) {
        currentLevel -= this->releaseDec;
        if (currentLevel <= 0.0f) { currentLevel = 0.0f; this->state = State::Idle; }
    }

    return currentLevel;
}

void OplAdsr::setParametersCurve(const OplAdsrParams& params) {
    auto calcLevel = [this](int prmIdx, int value, float maxValue) -> float {
        float normRate = (float)value / maxValue;

        return m_curveCore->process(positionIndex, (int)CurveParams::Target::RegValue, prmIdx, normRate);

        };

    this->totalLevel = calcLevel((int)CurveParams::TargetRegValue::Tl, this->tl, 63.0f);

    // サステインレベル (SL) の計算
    if (this->sl == 15) {
        this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float baseSustainLevel = calcLevel((int)CurveParams::TargetRegValue::Sl, this->sl, 15.0f);
        float slDb = (baseSustainLevel * 15.0f) * 3.0f;
        this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }

    updateIncrementsWithKeyScaleCurve(m_noteNumber);
}

float OplAdsr::noteOnCurve(float velocity, int noteNumber) {
    // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = (this->totalLevel * 63.0f) * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    float kslAttenuation = 1.0f;
    if (this->ksl > 0)
    {
        float octaveDiff = (float)(noteNumber - 48) / 12.0f;

        if (octaveDiff < 0) octaveDiff = 0;

        float totalDb = dbPerOcts[std::clamp(this->ksl, 0, 3)] * octaveDiff;

        kslAttenuation = std::pow(10.0f, -totalDb / 20.0f);
    }

    return velocity * tlGain * kslAttenuation;
}

void OplAdsr::updateIncrementsWithKeyScaleCurve(int noteNumber)
{
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
    attackInc = calcRegRate(this->ar, this->arMax, (int)CurveParams::TargetRegValue::Ar, false, true);
    decayDec = calcRegRate(this->dr, this->drMax, (int)CurveParams::TargetRegValue::Dr, false, false);
    sustainRateDec = 0.0f;
    releaseDec = calcRegRate(this->sus ? 5 : this->rr, this->rrMax, (int)CurveParams::TargetRegValue::Rr, true, false);

    // 3. サステインレベル (SL) の計算
    if (sl == 15) {
        this->m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        auto calcLevel = [this](int prmIdx, int value, float maxValue) -> float {
            float normRate = (float)value / maxValue;

            return m_curveCore->process(positionIndex, (int)CurveParams::Target::RegValue, prmIdx, normRate);

            };

        float baseSustainLevel = calcLevel((int)CurveParams::TargetRegValue::Sl, this->sl, 15.0f);
        float slDb = (baseSustainLevel * 15.0f) * 3.0f;
        this->m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }
}

float OplAdsr::updateEnvelopeStateCurve(float currentLevel)
{
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
        // ====================================================================
        // パーカッシブモード(EG-TYP=OFF)の判定
        // ====================================================================
        if (this->egType || m_sustain <= 0.0001f) { // EG-TYPE = 1 (持続音／サステイン・タイプ)
            // 何もしない

            return currentLevel;
        }
        else { // EG-TYPE = 0 (減衰音／パーカッシブ・タイプ)
            float startLevel = this->m_sustain;

            if (startLevel <= 0.0f) {
                this->state = State::Idle;

                return 0.0f;
            }

            // 1. 時間を進める
            float deltaX = this->releaseDec / startLevel;
            this->m_phaseProgress += deltaX;

            if (this->m_phaseProgress >= 1.0f) {
                this->m_phaseProgress = 1.0f;
                this->state = State::Idle;
                return 0.0f;
            }

            int prmIdx = this->sus ? (int)CurveParams::TargetRegValue::Sr : (int)CurveParams::TargetRegValue::Rr;
            float y = this->m_curveCore->process(posIdx, targetIdx, prmIdx, this->m_phaseProgress);

            float newLevel = startLevel - (y * startLevel);

            if (newLevel <= 0.0f) {
                newLevel = 0.0f;
                this->state = State::Idle;
            }

            return newLevel;
        }
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
        float deltaX = this->releaseDec / this->m_releaseStartLevel;
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

void OplAdsr::setParamMax(int ar, int dr, int sl, int rr, int tl) {
    arMax = ar;
    drMax = dr;
    slMax = sl;
    rrMax = rr;
    tlMax = tl;
}
