#include <algorithm>

#include "./Core.h"

AmpAdsrEnv::AmpAdsrEnv() {
    this->noteOnFunctions = std::array<std::function<float()>, 2>{
		[this](){ 
            this->state = State::Attack;

            return this->stl;
        },
		[this](){
            this->state = State::Attack;
            this->m_phaseProgress = 0.0f;

            return this->stl;
        }
	};
	this->noteOffFunctions = std::array<std::function<void()>, 2>{
		[this](){
            this->state = State::Release;
        },
		[this](){
            this->state = State::Release;
            this->m_phaseProgress = 0.0f;
        }
	};
	this->processFunctions = std::array<std::array<std::function<float(float)>, 5>, 2>{
        std::array<std::function<float(float)>, 5>{
            [this](float newLevel) { // Idle
                return newLevel;
            },
            [this](float newLevel) { // Attack
                newLevel += this->attackInc;

                if (newLevel >= 1.0f) {
                    newLevel = 1.0f;
                    this->state = State::Decay;
                }

                return newLevel;
            },
            [this](float newLevel) { // Decay
                // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
                if (this->dr <= 0.0f)
                {
                    newLevel = 1.0f;
                    this->state = State::Sustain;
                }
                else if (newLevel > this->sl) {
                    newLevel -= this->decayDec;

                    if (newLevel <= (this->sl + 0.001f)) {
                        newLevel = this->sl;
                        this->state = State::Sustain;
                    }
                }
                else {
                    newLevel = this->sl;
                    this->state = State::Sustain;
                }

                return newLevel;
            },
            [this](float newLevel) { // Sustain
                return newLevel;
            },
            [this](float newLevel) { // Release
                newLevel -= this->releaseDec;

                if (newLevel <= 0.001f) {
                    newLevel = 0.0f;
                    this->state = State::Idle;
                }

                return newLevel;
            }
        },
        std::array<std::function<float(float)>, 5>{
            [this](float newLevel) { // Idle
                return newLevel;
            },
            [this](float newLevel) { // Attack
                // 1. 時間(x)を進める
                // attackIncは「1サンプルあたりに進む量」としてそのまま流用できる
                float y = 0.0f;

                // 開始の瞬間は確実に 0.0f を保証し、カーブ計算の誤差ジャンプを防ぐ
                if (this->m_phaseProgress > 0.0f) {
                    y = this->m_curveCore->process(
                        (int)CurveParams::Position::Common,
                        (int)CurveParams::Target::AmpEnv,
                        (int)CurveParams::TargetAmpEnv::Ar,
                        this->m_phaseProgress);
                }

                float outLevel = this->stl + y * (1.0f - this->stl);

                this->m_phaseProgress += this->attackInc;

                if (this->m_phaseProgress >= 1.0f) {
                    this->m_phaseProgress = 1.0f;
                    this->state = State::Decay;
                    outLevel = 1.0f;
                }

                return outLevel;
            },
            [this](float newLevel) { // Decay
                if (this->dr <= 0.0f) {
                    this->state = State::Sustain;
                    return 1.0f;
                }

                // 1. 時間(x)を進める
                // decayDecから「1サンプルあたりに進む割合(0〜1)」を逆算する
                // decayDecは「1.0からslへ向かう絶対値」になっているため、全体幅(1.0 - sl)で割る
                float totalDecayRange = 1.0f - this->sl;
                if (totalDecayRange <= 0.0f) {
                    this->state = State::Sustain;
                    return this->sl;
                }

                float deltaX = this->decayDec;
                this->m_phaseProgress += deltaX;

                if (this->m_phaseProgress >= 1.0f) {
                    this->m_phaseProgress = 0.0f; // Sustainに向けて確実に0にリセット！
                    this->state = State::Sustain;

                    return this->sl; // 最後のサンプルはターゲットレベルをきっちり返す
                }

                // 2. カーブコアからYを取得
                float y = m_curveCore->process(
                    (int)CurveParams::Position::Common,
                    (int)CurveParams::Target::AmpEnv,
                    (int)CurveParams::TargetAmpEnv::Dr,
                    this->m_phaseProgress);

                // Decayは 1.0 から SL へ向かう (減少なので、1.0から引いていく)
                return 1.0f - (y * totalDecayRange);
            },
            [this](float newLevel) { // Sustain
                return this->sl; // Sustainは変化しないのでそのまま
            },
            [this](float newLevel) { // Release
                // noteOffされた最初のフレームで、現在のレベルを始点として記録する
                if (this->m_phaseProgress == 0.0f) {
                    this->m_releaseStartLevel = newLevel;
                }

                // 0.0f ではなく、0.001f (1000分の1の音量) 以下なら即座に消す。
                // 人間の耳には聞こえないレベルであり、かつゼロ除算(Infinity爆発)を完全に防ぐための必須の措置です。
                if (this->m_releaseStartLevel <= 0.001f) {
                    this->state = State::Idle;
                    this->m_phaseProgress = 0.0f;
                    return 0.0f;
                }

                // 1. 時間(x)を進める
                float deltaX = this->releaseDec;
                this->m_phaseProgress += deltaX;

                if (this->m_phaseProgress >= 1.0f) {
                    this->m_phaseProgress = 0.0f;
                    this->state = State::Idle;
                    return 0.0f;
                }

                // 2. カーブコアからYを取得
                float y = m_curveCore->process(
                    (int)CurveParams::Position::Common,
                    (int)CurveParams::Target::AmpEnv,
                    (int)CurveParams::TargetAmpEnv::Rr,
                    this->m_phaseProgress);

                // Releaseは 開始レベル から 0.0 へ向かう
                return this->m_releaseStartLevel - (y * this->m_releaseStartLevel);
            }
        }
	};
	this->bypassedReleasedProcessFunctions = std::array<std::function<float()>, 2>{
		[this](){
           this->state = State::Idle;

           return 0.0f;
        },
		[this](){
           this->state = State::Idle;

           return 0.0f;
        }
	};
	this->bypassedProcessFunctions = std::array<std::function<float()>, 2>{
		[this](){
            return 1.0f;
        },
		[this](){
            return 1.0f;
        }
	};
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
	this->bypass = params.bypass;
    this->updateIncrements();
}

void AmpAdsrEnv::updateIncrements()
{
    if (this->sampleRate <= 0.0) return;

    this->attackInc = (1.0f - this->stl) / (float)(std::max(0.00001f, this->ar) * this->sampleRate);
    this->decayDec = (this->stl - this->sl) / (float)(std::max(0.00001f, this->dr) * this->sampleRate);
    this->releaseDec = this->sl / (float)(std::max(0.00001f, this->rr) * this->sampleRate);
}

void AmpAdsrEnv::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
    this->updateIncrements();
}

float AmpAdsrEnv::noteOn() {
    if (this->bypass) {
        return 1.0f;
    }

    return this->noteOnFunctions[this->m_curveCore == nullptr ? 0 : this->m_curveCore->index]();
}

void AmpAdsrEnv::noteOff() {
    if (this->bypass) {
        return;
    }

    this->noteOffFunctions[this->m_curveCore == nullptr ? 0 : this->m_curveCore->index]();
}

float AmpAdsrEnv::bypassedReleasedProcess() {
    return this->bypassedReleasedProcessFunctions[this->m_curveCore == nullptr ? 0 : this->m_curveCore->index]();
}

float AmpAdsrEnv::bypassedProcess() {
    return this->bypassedProcessFunctions[this->m_curveCore == nullptr ? 0 : this->m_curveCore->index]();
}

float AmpAdsrEnv::process(float currentLevel) {
    if (this->bypass) {
        return 1.0f;
    }

    return this->processFunctions[this->m_curveCore == nullptr ? 0 : this->m_curveCore->index][(int)this->state](currentLevel);
}
