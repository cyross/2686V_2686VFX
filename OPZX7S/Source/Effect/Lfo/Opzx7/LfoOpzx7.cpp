#include <JuceHeader.h>
#include <algorithm>

#include "./LfoOpzx7.h"

Opzx7LfoCore::Opzx7LfoCore(): pm(), am() {
}

inline void Opzx7LfoCore::updatePhaseDelta()
{
    pm.updatePhaseDelta();
    am.updatePhaseDelta();
}

void Opzx7LfoCore::prepare(double sampleRate) {
    pm.prepare(sampleRate);
    am.prepare(sampleRate);
}

void Opzx7LfoCore::updateTargetSampleRate(double newSampleRate) {
    pm.updateTargetSampleRate(newSampleRate);
    am.updateTargetSampleRate(newSampleRate);
}

void Opzx7LfoCore::setParameters(const LfoOpzx7Params& params)
{
    pm.setParameters(params.pmSyncDelay, params.pmEnable, params.pmFreq, params.pgIndex, params.pms, params.pmd, 0.0f);
    am.setParameters(params.amSyncDelay, params.amEnable, params.amFreq, params.egIndex, params.ams, params.amd, params.amSmoothRate);
}

void Opzx7LfoCore::noteOn()
{
    pm.noteOn();
    am.noteOn();
}

void Opzx7LfoCore::getSample()
{
    this->value.am = am.getSampleAm();
    this->value.pm = pm.getSamplePm();
}