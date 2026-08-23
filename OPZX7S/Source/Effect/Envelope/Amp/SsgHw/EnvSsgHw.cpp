#include <cmath>
#include <algorithm>

#include "./EnvSsgHw.h"

SsgHwEnv::SsgHwEnv() {
}

void SsgHwEnv::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void SsgHwEnv::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = sampleRate;
}

void SsgHwEnv::setParameters(const SsgHwEnvParams& params) {
    this->m_useHwEnv = params.enable;
    this->m_envShape = params.shape;
    this->m_envFreq = params.period;
    this->m_min = params.min;
    this->m_max = params.max;
}

void SsgHwEnv::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
}

void SsgHwEnv::noteOn() {
    this->m_hwEnvPhase = 0.0f;
}

void SsgHwEnv::noteOff() {
    // 何もしない
}

float SsgHwEnv::process() {
    // ==========================================
    // 1. Hardware Envelope Update
    // ==========================================
    float hwEnvDelta = m_envFreq / (float)sampleRate;
    m_hwEnvPhase += hwEnvDelta;

    // 位相が無限増大して小数の精度が落ちるのを防ぐラップアラウンド
    if (m_hwEnvPhase >= 2.0) {
        if (m_envShape % 2 == 0) m_hwEnvPhase -= 2.0;
        else m_hwEnvPhase = 2.0;
    }

    float hwEnvGain = this->m_max;

    if (m_useHwEnv)
    {
        double p = m_hwEnvPhase;
        bool isEvenCycle = ((int)p % 2 == 0);
        float phaseNorm = (float)(p - std::floor(p));

        switch (m_envShape) {
        case 0:
            hwEnvGain = this->m_max - phaseNorm;
            break;
        case 1:
            hwEnvGain = (p < this->m_max) ? (this->m_max - phaseNorm) : this->m_min;
            break;
        case 2:
            hwEnvGain = isEvenCycle ? (this->m_max - phaseNorm) : phaseNorm;
            break;
        case 3:
            hwEnvGain = (p < this->m_max) ? (this->m_max - phaseNorm) : this->m_min;
            break;
        case 4:
            hwEnvGain = phaseNorm;
            break;
        case 5:
            hwEnvGain = (p < this->m_max) ? phaseNorm : this->m_max;
            break;
        case 6:
            hwEnvGain = (p < this->m_max) ? phaseNorm : this->m_max;
            break;
        case 7:
            hwEnvGain = isEvenCycle ? phaseNorm : (this->m_max - phaseNorm);
            break;
        default:
            hwEnvGain = phaseNorm;
        }

        if (hwEnvGain < this->m_min) {
            hwEnvGain = this->m_min;
        }
    }

    return hwEnvGain;
}
