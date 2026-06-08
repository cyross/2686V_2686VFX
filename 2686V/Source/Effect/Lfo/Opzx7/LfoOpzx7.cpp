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

void Opzx7LfoCore::setParameters(
    int pmSyncDelay,
    int amSyncDelay,
    bool pmEnable,
    bool amEnable,
    float pmFreq,
    float amFreq,
    int pgIndex,
    int egIndex,
    float pms,
    float pmd,
    float ams,
    float amd,
    float amSmoothRate
)
{
    pm.setParameters(pmSyncDelay, pmEnable, pmFreq, pgIndex, pms, pmd, 0.0f);
    am.setParameters(amSyncDelay, amEnable, amFreq, egIndex, ams, amd, amSmoothRate);
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