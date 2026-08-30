#include <algorithm>
#include <cmath>

#include "./EnvOplAdsr.h"

OplAdsr::OplAdsr()
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

void OplAdsr::prepare(int posIndex, double sampleRate) {
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
    this->tl = params.tl;

    this->m_ksOPL.setParameters(params.ksOPL);

    this->sus = params.sus;
    this->egType = params.egType;
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

float OplAdsr::noteOn(float velocity, int noteNumber) {
    m_noteNumber = noteNumber;

    this->m_phaseProgress = 0.0f;

    if (this->bypass) {
        this->m_targetLevel = velocity;
        return velocity;
    }

    this->state = State::Attack;

    // 目標レベルの計算
    float attenuationDb = tl * 0.75f;

    // KSL はゲイン(倍率)で返ってくるので、dB に直して TL と合算する
    float kslGain = m_ksOPL.calcLevelScalingGain(noteNumber);
    float kslDb = (kslGain > 0.0f) ? (-20.0f * std::log10(kslGain)) : 96.0f;

    // 実機のレベルスケーリングは TL(出力レベルレジスタ)から引く形で働くので、
    // TL レジスタの全域 (tlMax × 0.75dB) より深くは減衰しない。同じ上限で頭打ちにする。
    float maxAttenDb = (this->tlMax > 0) ? ((float)this->tlMax * 0.75f) : 47.25f;
    float totalDb = std::clamp(attenuationDb + kslDb, 0.0f, maxAttenDb);

    // 最終到達レベルを内部に保存する
    this->m_targetLevel = velocity * std::pow(10.0f, -totalDb / 20.0f);

    return this->m_targetLevel;
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

    // ====================================================================
    // 実機のアルゴリズムで増減量を計算
    // ====================================================================
    // 1. キースケールレート (KSR) の算出
    int ksrValue = m_ksOPL.calcKeyScaleRate(noteNumber);

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
    this->attackInc = calcRegRate(this->ar, this->arMax, false, true);
    this->decayDec = calcRegRate(this->dr, this->drMax, false, false);
    this->sustainRateDec = 0.0f;
    releaseTimeInc = calcRegRate(this->sus ? 5 : this->rr, this->rrMax, true, false);;
    releaseDec = kor ? 0.0f : releaseTimeInc;
}

float OplAdsr::updateEnvelopeState(float currentLevel)
{
    // 返すのは 0.0〜1.0 の包絡線。
    // 音量 (velocity / TL / KSL) はオペレータ側が m_targetLevel を掛けて反映するので、
    // ここで掛けると二重になる (FmRgAdssr / FmRgAdddr と同じ扱い)。
    if (this->bypass) {
        return 1.0f;
    }

    float limitLevel = 0.0f;

    switch (this->state) {
    case State::Idle:
        return currentLevel;
    case State::Attack:
        currentLevel += attackInc * 1.0f;

        if (currentLevel >= 1.0f) {
            currentLevel = 1.0f;
            state = State::Decay;
        }

        return currentLevel;
    case State::Decay:
        limitLevel = this->m_sustain * 1.0f;

        // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
        if (this->m_zeroDecay) {
            currentLevel = 1.0f;
            this->state = State::Sustain;
        }
        else if (currentLevel > limitLevel) {
            if (this->decayDec > 0.0f) {
                // 減衰量も targetLevel 基準にする
                currentLevel -= this->decayDec * 1.0f;
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
        return currentLevel;
    case State::Sustain:
        // ====================================================================
        // パーカッシブモード(EG-TYP=OFF)の判定
        // ====================================================================
        if (this->egType) { // EG-TYPE = 1 (持続音／サステイン・タイプ)
            // 何もしない
        }
        else { // EG-TYPE = 0 (減衰音／パーカッシブ・タイプ)
            currentLevel -= this->releaseDec * 1.0f;
            if (currentLevel <= 0.001f) {
                currentLevel = 0.0f;
                this->state = State::Idle;
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

        currentLevel -= this->releaseDec * 1.0f;

        if (currentLevel <= 0.001f) {
            currentLevel = 0.0f;
            state = State::Idle;
        }

        return currentLevel;
    }

    return currentLevel; // 念のため、どこにも当てはまらない場合は現在のレベルを維持
}

void OplAdsr::setParamMax(int ar, int dr, int sl, int rr, int tl) {
    arMax = ar;
    drMax = dr;
    slMax = sl;
    rrMax = rr;
    tlMax = tl;
}

void OplAdsr::bypassedReleasedProcess() {
    this->state = State::Idle;
}
