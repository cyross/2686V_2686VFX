#include <JuceHeader.h>
#include <algorithm>

#include "./Core.h"

inline void OplLfoCore::updatePhaseDelta()
{
    this->m_pmPhaseDelta = (double)this->pms / this->m_sampleRate;
    this->m_amPhaseDelta = (double)this->ams / this->m_sampleRate;
}

void OplLfoCore::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void OplLfoCore::updateTargetSampleRate(double newSampleRate) {
    this->m_sampleRate = newSampleRate;

    updatePhaseDelta();
}

void OplLfoCore::setParameters(bool pm, bool am, float pms, float pmd, float ams, float amd)
{
    this->pmEnable = pm;
    this->pms = pms;
    this->pmd = pmd;

    this->amEnable = am;
    this->ams = ams;
    this->amd = amd;

    this->depthDb = 1.0f - std::pow(10.0f, -this->amd / 20.0f);
    this->depthCent = std::pow(2.0f, this->pmd / 1200.0f) - 1.0f + this->m_modWheel;

    updatePhaseDelta();
}

void OplLfoCore::setModWheel(float newWheel)
{
    this->m_modWheel = newWheel * 0.03f;

    this->depthCent = std::pow(2.0f, this->pmd / 1200.0f) - 1.0f + this->m_modWheel;
}

void OplLfoCore::getSample()
{
    float ampMod = 1.0f;
    float pitchMod = 1.0f;

    // --- AM (Tremolo) ---
    if (this->amEnable) {
        this->m_amPhase += this->m_amPhaseDelta;
        if (this->m_amPhase >= 1.0) this->m_amPhase -= 1.0;

        float amVal = 0.0f;
        if (this->m_amPhase < 0.25)      amVal = (float)(this->m_amPhase * 4.0);
        else if (this->m_amPhase < 0.75) amVal = (float)(1.0 - (this->m_amPhase - 0.25) * 4.0);
        else                             amVal = (float)(-1.0 + (this->m_amPhase - 0.75) * 4.0);

        ampMod = 1.0f - (amVal + 1.0f) * 0.5f * this->depthDb;
    }

    // --- VIB (Vibrato) ---
    if (this->pmEnable) {
        this->m_pmPhase += this->m_pmPhaseDelta;
        if (this->m_pmPhase >= 1.0) m_pmPhase -= 1.0;

        float pmVal = 0.0f;
        if (this->m_pmPhase < 0.25)      pmVal = (float)(this->m_pmPhase * 4.0);
        else if (this->m_pmPhase < 0.75) pmVal = (float)(1.0 - (this->m_pmPhase - 0.25) * 4.0);
        else                             pmVal = (float)(-1.0 + (this->m_pmPhase - 0.75) * 4.0);

        pitchMod = 1.0f + pmVal * this->depthCent;
    }

    this->value.am = ampMod;
    this->value.pm = pitchMod;
}