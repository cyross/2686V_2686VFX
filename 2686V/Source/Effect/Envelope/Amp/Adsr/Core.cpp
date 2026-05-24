#include <algorithm>

#include "./Core.h"

AmpAdsrEnv::AmpAdsrEnv() {
	this->noteOnFunctions = std::array<std::function<float()>, 2>{
		[this](){ return this->noteOnLinear(); },
		[this](){ return this->noteOnCurve(); }
	};
	this->noteOffFunctions = std::array<std::function<void()>, 2>{
		[this](){ this->noteOffLinear(); },
		[this](){ this->noteOffCurve(); }
	};
	this->processFunctions = std::array<std::function<float(float)>, 2>{
		[this](float currentLevel){ return this->processLinear(currentLevel); },
		[this](float currentLevel){ return this->processCurve(currentLevel); }
	};
	this->bypassedReleasedProcessFunctions = std::array<std::function<float()>, 2>{
		[this](){ return this->bypassedReleasedProcessLinear(); },
		[this](){ return this->bypassedReleasedProcessCurve(); }
	};
	this->bypassedProcessFunctions = std::array<std::function<float()>, 2>{
		[this](){ return this->bypassedProcessLinear(); },
		[this](){ return this->bypassedProcessCurve(); }
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

    this->attackInc = (1.0f - this->stl) / (float)(std::max(0.001f, this->ar) * this->sampleRate);
    this->decayDec = (this->stl - this->sl) / (float)(std::max(0.001f, this->dr) * this->sampleRate);
    this->releaseDec = this->sl / (float)(std::max(0.001f, this->rr) * this->sampleRate);
}

void AmpAdsrEnv::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
    this->updateIncrements();
}

float AmpAdsrEnv::noteOn() {
	if (this->m_curveCore == nullptr) {
		return this->noteOnLinear();
	}

    return this->noteOnFunctions[this->m_curveCore->index]();
}

void AmpAdsrEnv::noteOff() {
    if (this->m_curveCore == nullptr) {
        this->noteOffLinear();

        return;
    }

    this->noteOffFunctions[this->m_curveCore->index]();
}

float AmpAdsrEnv::bypassedReleasedProcess() {
    if (this->m_curveCore == nullptr) {
        return this->bypassedReleasedProcessLinear();
    }

    return this->bypassedReleasedProcessFunctions[this->m_curveCore->index]();
}

float AmpAdsrEnv::bypassedProcess() {
    if (this->m_curveCore == nullptr) {
        return this->bypassedProcessLinear();
    }

    return this->bypassedProcessFunctions[this->m_curveCore->index]();
}

float AmpAdsrEnv::process(float currentLevel) {
    if (this->m_curveCore == nullptr) {
        return this->processLinear(currentLevel);
    }

    return this->processFunctions[this->m_curveCore->index](currentLevel);
}

float AmpAdsrEnv::noteOnLinear() {
    this->state = State::Attack;

    return this->stl;
}

void AmpAdsrEnv::noteOffLinear() {
    this->state = State::Release;
}

float AmpAdsrEnv::bypassedReleasedProcessLinear() {
   this->state = State::Idle;

   return 0.0f;
}

float AmpAdsrEnv::bypassedProcessLinear() {
    return 1.0f;
}

float AmpAdsrEnv::processLinear(float currentLevel) {
    if (this->bypass) {
        return currentLevel;
    }

	float newLevel = currentLevel;

    // --- ADSR Logic ---
    if (this->state == State::Attack) {
        newLevel += this->attackInc;

        if (newLevel >= 1.0f) {
            newLevel = 1.0f;
            this->state = State::Decay;
        }
    }
    else if (this->state == State::Decay) {
        // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
        if (this->dr <= 0.0f)
        {
            newLevel = 1.0f;
            this->state = State::Sustain;
        }
        else if (newLevel > this->sl) {
            newLevel -= this->decayDec;

            if (newLevel <= this->sl) {
                newLevel = this->sl;
                this->state = State::Sustain;
            }
        }
        else {
            newLevel = this->sl;
            this->state = State::Sustain;
        }
    }
    else if (this->state == State::Release) {
        newLevel -= this->releaseDec;

        if (newLevel <= 0.0f) {
            newLevel = 0.0f;
            this->state = State::Idle;
        }
    }

    return newLevel;
}

float AmpAdsrEnv::noteOnCurve() {
    this->state = State::Attack;
    this->m_phaseProgress = 0.0f;

    return this->stl;
}

void AmpAdsrEnv::noteOffCurve() {
    this->state = State::Release;
    this->m_phaseProgress = 0.0f;
}

float AmpAdsrEnv::bypassedReleasedProcessCurve() {
   this->state = State::Idle;

   return 0.0f;
}

float AmpAdsrEnv::bypassedProcessCurve() {
    return 1.0f;
}

float AmpAdsrEnv::processCurve(float currentLevel) {
    if (this->bypass) {
        return currentLevel;
    }

    if (this->m_curveCore == nullptr) {
        // CurveCoreがセットされていなければ安全のため線形処理にフォールバック
        return processLinear(currentLevel);
    }

    // --- カーブモードの ADSR Logic ---

    int posIdx = (int)CurveParams::Position::Common;
    int targetIdx = (int)CurveParams::Target::AmpEnv;

    if (this->state == State::Attack) {
        // 1. 時間(x)を進める
        // attackIncは「1サンプルあたりに進む量」としてそのまま流用できる
        this->m_phaseProgress += this->attackInc;

        if (this->m_phaseProgress >= 1.0f) {
            this->m_phaseProgress = 1.0f;
            this->state = State::Decay;
        }

        // 2. カーブコアに問い合わせてY(レベル)を取得
        int prmIdx = (int)CurveParams::TargetAmpEnv::Ar;

        // x(0~1) を渡して y(0~1) をもらう
        float y = m_curveCore->process(posIdx, targetIdx, prmIdx, this->m_phaseProgress);

        // Attackは STL から 1.0 へ向かう
        return this->stl + y * (1.0f - this->stl);
    }
    else if (this->state == State::Decay) {
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

        float deltaX = this->decayDec / totalDecayRange;
        this->m_phaseProgress += deltaX;

        if (this->m_phaseProgress >= 1.0f) {
            this->m_phaseProgress = 1.0f;
            this->state = State::Sustain;
        }

        // 2. カーブコアからYを取得
        int prmIdx = (int)CurveParams::TargetAmpEnv::Dr;

        float y = m_curveCore->process(posIdx, targetIdx, prmIdx, this->m_phaseProgress);

        // Decayは 1.0 から SL へ向かう (減少なので、1.0から引いていく)
        return 1.0f - (y * totalDecayRange);
    }
    else if (this->state == State::Sustain) {
        return this->sl; // Sustainは変化しないのでそのまま
    }
    else if (this->state == State::Release) {
        // noteOffされた最初のフレームで、現在のレベルを始点として記録する
        if (this->m_phaseProgress == 0.0f) {
            this->m_releaseStartLevel = currentLevel;
        }

        if (this->m_releaseStartLevel <= 0.0f) {
            this->state = State::Idle;
            return 0.0f;
        }

        // 1. 時間(x)を進める
        float deltaX = this->releaseDec / this->m_releaseStartLevel;
        this->m_phaseProgress += deltaX;

        if (this->m_phaseProgress >= 1.0f) {
            this->m_phaseProgress = 1.0f;
            this->state = State::Idle;
            return 0.0f;
        }

        // 2. カーブコアからYを取得
        int prmIdx = (int)CurveParams::TargetAmpEnv::Rr;

        float y = m_curveCore->process(posIdx, targetIdx, prmIdx, this->m_phaseProgress);

        // Releaseは 開始レベル から 0.0 へ向かう
        return this->m_releaseStartLevel - (y * this->m_releaseStartLevel);
    }

    return currentLevel;
}
