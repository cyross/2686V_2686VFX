#include <JuceHeader.h>
#include <algorithm>

#include "./LfoOpl.h"
#include "../../../Core/Const/ConstGlobal.h"

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

void OplLfoCore::setParameters(const LfoOplParams& params)
{
    this->pmEnable = params.pm;
    this->pms = params.pms;
    this->pmd = params.pmd;

    this->amEnable = params.am;
    this->ams = params.ams;
    this->amd = params.amd;

    // amd はそのまま「LFO の谷での最大減衰量(dB)」として扱う。
    // 上限は全音源共通の maxAmDepthDb に合わせる。
    this->depthDb = std::min(this->amd, Global::Lfo::maxAmDepthDb);
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

        // 実機はアッテネータ(dB)を動かすので、リニア補間ではなく dB で補間する
        float unipolar = (amVal + 1.0f) * 0.5f;

        ampMod = std::pow(10.0f, -(unipolar * this->depthDb) / 20.0f);
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