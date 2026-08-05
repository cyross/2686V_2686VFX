#include <algorithm>

#include "./EnvAmpAdsr.h"

AmpAdsrEnv::AmpAdsrEnv() {
}

void AmpAdsrEnv::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void AmpAdsrEnv::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = sampleRate;
    this->updateIncrements();
}

void AmpAdsrEnv::setParameters(const AmpAdsrParams& params) {
	this->ar = params.ar;
	this->dr = params.dr;
	this->sl = params.sl;
	this->rr = params.rr;
	this->stl = params.stl;
    this->kor = params.kor;
	this->bypass = params.bypass;
    this->updateIncrements();
}

void AmpAdsrEnv::updateIncrements()
{
    if (this->sampleRate <= 0.0) return;

    // 1. 純粋な時間進行度 (1サンプルあたりに 0.0 から 1.0 に向かうステップ量)
    this->attackTimeInc = 1.0f / (float)(std::max(0.00001f, this->ar) * this->sampleRate);
    this->decayTimeInc = 1.0f / (float)(std::max(0.00001f, this->dr) * this->sampleRate);
    this->releaseTimeInc = 1.0f / (float)(std::max(0.00001f, this->rr) * this->sampleRate);

    // 2. リニアモード用のレベル増減量
    this->attackInc = (1.0f - this->stl) * this->attackTimeInc;
    this->decayDec = (1.0f - this->sl) * this->decayTimeInc;
    this->releaseDec = this->kor ? 0.0f : this->sl * this->releaseTimeInc;
}

void AmpAdsrEnv::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
    this->updateIncrements();
}

float AmpAdsrEnv::noteOn() {
    if (this->bypass) {
        return 1.0f;
    }

    if (this->m_curveCore == nullptr || this->m_curveCore->index == 0) {
        this->state = State::Attack;

        return this->stl;
    }
    else {
        this->state = State::Attack;
        this->m_phaseProgress = 0.0f;

        return this->stl;
    }
}

void AmpAdsrEnv::noteOff() {
    if (this->bypass) {
        return;
    }

    if (this->m_curveCore == nullptr || this->m_curveCore->index == 0) {
        this->state = State::Release;
        this->m_phaseProgress = 0.0f; // kor向け
    }
    else {
        this->state = State::Release;
        this->m_phaseProgress = 0.0f;
    }
}

float AmpAdsrEnv::bypassedReleasedProcess() {
    this->state = State::Idle;

    return 0.0f;
}

float AmpAdsrEnv::bypassedProcess() {
    return 1.0f;
}

float AmpAdsrEnv::process(float currentLevel) {
    if (this->bypass) {
        return 1.0f;
    }

    if (this->m_curveCore == nullptr || this->m_curveCore->index == 0) {
        float limitLevel = 0.0f;

        switch (this->state) {
        case State::Idle:
            return currentLevel;
        case State::Attack:
            currentLevel += this->attackInc;

            if (currentLevel >= 1.0f) {
                currentLevel = 1.0f;
                this->state = State::Decay;
            }

            return currentLevel;
        case State::Decay:
            // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
            if (this->dr <= 0.0f)
            {
                currentLevel = 1.0f;
                this->state = State::Sustain;
            }
            else if (currentLevel > this->sl) {
                currentLevel -= this->decayDec;

                if (currentLevel <= (this->sl + 0.001f)) {
                    currentLevel = this->sl;
                    this->state = State::Sustain;
                }
            }
            else {
                currentLevel = this->sl;
                this->state = State::Sustain;
            }

            return currentLevel;
        case State::Sustain:
            return currentLevel;
        case State::Release:
            // kor向けに時間を進める
            this->m_phaseProgress += this->releaseTimeInc;

            if (this->m_phaseProgress >= 1.0f) {
                this->m_phaseProgress = 0.0f;
                this->state = State::Idle;
                return 0.0f;
            }

            currentLevel -= this->releaseDec;

            if (currentLevel <= 0.001f) {
                currentLevel = 0.0f;
                this->state = State::Idle;
            }

            return currentLevel;
        }
    }
    else {
        float y = 0.0f;
        float outLevel = 0.0f;
        float totalDecayRange = 0.0f;

        switch (this->state) {
        case State::Idle:
            return currentLevel;
        case State::Attack:
            if (this->m_phaseProgress == 0.0f) {
                // stl(Start Level) ではなく、現在の音量(currentLevel)を記録する
                this->m_attackStartLevel = currentLevel;
            }

            // 1. 時間(x)を進める
            // attackIncは「1サンプルあたりに進む量」としてそのまま流用できる
            y = 0.0f;

            // 開始の瞬間は確実に 0.0f を保証し、カーブ計算の誤差ジャンプを防ぐ
            if (this->m_phaseProgress > 0.0f) {
                y = this->m_curveCore->process(
                    (int)CurveParams::Position::Common,
                    (int)CurveParams::Target::AmpEnv,
                    (int)CurveParams::TargetAmpEnv::Ar,
                    this->m_phaseProgress);
            }

            outLevel = this->m_attackStartLevel + (1.0f - this->m_attackStartLevel) * y;

            this->m_phaseProgress += this->attackTimeInc;

            if (this->m_phaseProgress >= 1.0f) {
                this->m_phaseProgress = 0.0f;
                this->state = State::Decay;
                outLevel = 1.0f;
            }

            return outLevel;
        case State::Decay:
            if (this->dr <= 0.0f) {
                this->state = State::Sustain;
                return currentLevel;
            }

            // Decayの開始レベルを記録
            if (this->m_phaseProgress == 0.0f) {
                this->m_releaseStartLevel = currentLevel; // releaseStartLevel を流用
            }

            // 1. 時間(x)を進める
            // decayDecから「1サンプルあたりに進む割合(0〜1)」を逆算する
            // decayDecは「this->m_releaseStartLevelからslへ向かう絶対値」になっているため、全体幅(this->m_releaseStartLevel - sl)で割る
            totalDecayRange = this->m_releaseStartLevel - this->sl;

            if (totalDecayRange <= 0.0f) {
                this->m_phaseProgress = 0.0f;
                this->state = State::Sustain;
                return this->sl;
            }

            this->m_phaseProgress += this->decayTimeInc;

            if (this->m_phaseProgress >= 1.0f) {
                this->m_phaseProgress = 0.0f; // Sustainに向けて確実に0にリセット！
                this->state = State::Sustain;

                return this->sl; // 最後のサンプルはターゲットレベルをきっちり返す
            }

            // 2. カーブコアからYを取得
            y = m_curveCore->process(
                (int)CurveParams::Position::Common,
                (int)CurveParams::Target::AmpEnv,
                (int)CurveParams::TargetAmpEnv::Dr,
                this->m_phaseProgress);

            // Decayは 1.0 から SL へ向かう (減少なので、1.0から引いていく)
            return this->m_releaseStartLevel - (y * totalDecayRange);
        case State::Sustain:
            return this->sl; // Sustainは変化しないのでそのまま
        case State::Release:
            // noteOffされた最初のフレームで、現在のレベルを始点として記録する
            if (this->m_phaseProgress == 0.0f) {
                this->m_releaseStartLevel = currentLevel;
            }

            // 0.0f ではなく、0.001f (1000分の1の音量) 以下なら即座に消す。
            // 人間の耳には聞こえないレベルであり、かつゼロ除算(Infinity爆発)を完全に防ぐための必須の措置です。
            if (this->m_releaseStartLevel <= 0.001f) {
                this->state = State::Idle;
                this->m_phaseProgress = 0.0f;
                return 0.0f;
            }

            // 1. 時間(x)を進める
            this->m_phaseProgress += this->releaseTimeInc;

            if (this->m_phaseProgress >= 1.0f) {
                this->m_phaseProgress = 0.0f;
                this->state = State::Idle;
                return 0.0f;
            }

            if (this->kor) {
                return this->m_releaseStartLevel;
            }

            // 2. カーブコアからYを取得
            y = m_curveCore->process(
                (int)CurveParams::Position::Common,
                (int)CurveParams::Target::AmpEnv,
                (int)CurveParams::TargetAmpEnv::Rr,
                this->m_phaseProgress);

            // Releaseは 開始レベル から 0.0 へ向かう
            return this->m_releaseStartLevel - (y * this->m_releaseStartLevel);
        }
    }

    return currentLevel; // 念のため、どこにも当てはまらない場合は現在のレベルを維持
}
