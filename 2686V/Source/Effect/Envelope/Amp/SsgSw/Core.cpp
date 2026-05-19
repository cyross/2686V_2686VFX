#include <algorithm>

#include "./Core.h"

void SsgSwEnv::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void SsgSwEnv::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = sampleRate;
    this->updateIncrements();
}

void SsgSwEnv::setParameters(const SsgSwEnvParams& params) {
	this->bypass = params.bypass;
	this->steps = params.steps;
	this->r0 = params.r0;
	this->l0 = params.l0;
	this->r1 = params.r1;
	this->l1 = params.l1;
	this->r2 = params.r2;
	this->l2 = params.l2;
	this->r3 = params.r3;
	this->l3 = params.l3;
	this->r4 = params.r4;
	this->l4 = params.l4;
	this->r5 = params.r5;
	this->stl = params.stl;
    this->updateIncrements();
}

float SsgSwEnv::noteOn() {
    this->state = State::R0;

    this->m_currentRate = this->stl;

    return this->stl;
}

void SsgSwEnv::noteOff() {
    this->state = State::R5;

    // キーが離された瞬間のレベル(m_currentRate)から、0.0に向けて減衰する傾きを計算
    float timeInSeconds = std::max(0.001f, this->r5);
    this->r5Inc = (0.0f - this->m_currentRate) / (timeInSeconds * (float)this->sampleRate);
}

float SsgSwEnv::bypassedReleasedProcess() {
   this->state = State::Idle;

   return 0.0f;
}

float SsgSwEnv::bypassedProcess() {
    return 1.0f;
}

float SsgSwEnv::process(float currentLevel) {
    if (this->bypass) {
        return currentLevel;
    }

	float newRate = this->m_currentRate;

    // --- ADSR Logic ---
    if (this->state == State::R0) {
        newRate += this->r0Inc;

        if (this->r0Inc < 0.0f ? newRate <= this->l0 : newRate >= this->l0) {
            newRate = this->l0;
            if (this->steps > 0)
            {
				this->state = State::R1;
            }
        }
    }
    else if (this->state == State::R1) {
        newRate += this->r1Inc;

        if (this->r1Inc < 0.0f ? newRate <= this->l1 : newRate >= this->l1) {
            newRate = this->l1;
            if (this->steps > 1)
            {
                this->state = State::R2;
            }
        }
    }
    else if (this->state == State::R2) {
        newRate += this->r2Inc;

        if (this->r2Inc < 0.0f ? newRate <= this->l2 : newRate >= this->l2) {
            newRate = this->l2;
            if (this->steps > 2)
            {
                this->state = State::R3;
            }
        }
    }
    else if (this->state == State::R3) {
        newRate += this->r3Inc;

        if (this->r3Inc < 0.0f ? newRate <= this->l3 : newRate >= this->l3) {
            newRate = this->l3;
            if (this->steps > 3)
            {
                this->state = State::R4;
            }
        }
    }
    else if (this->state == State::R4) {
        newRate += this->r4Inc;

        if (this->r4Inc < 0.0f ? newRate <= this->l4 : newRate >= this->l4) {
            newRate = this->l4;
        }
    }
    else if (this->state == State::R5) {
        newRate += this->r5Inc;

        if (newRate <= 0.0f) {
            newRate = 0.0f;
            this->state = State::Idle;
        }
    }

	this->m_currentRate = newRate;

    return currentLevel * newRate;
}

void SsgSwEnv::updateIncrements()
{
    if (this->sampleRate <= 0.0) return;

    this->r0Inc = (this->l0 - this->stl) / (float)(std::max(0.001f, this->r0) * this->sampleRate);
    this->r1Inc = (this->l1 - this->l0) / (float)(std::max(0.001f, this->r1) * this->sampleRate);
    this->r2Inc = (this->l2 - this->l1) / (float)(std::max(0.001f, this->r2) * this->sampleRate);
    this->r3Inc = (this->l3 - this->l2) / (float)(std::max(0.001f, this->r3) * this->sampleRate);
    this->r4Inc = (this->l4 - this->l3) / (float)(std::max(0.001f, this->r4) * this->sampleRate);
	// r5IncはnoteOff()で動的に計算されるため、ここでは初期化のみ行う
    this->r5Inc = 0.0f;
}

void SsgSwEnv::updateSampleRate(double newSampleRate) {
	this->sampleRate = newSampleRate;
    this->updateIncrements();
}
