#include <JuceHeader.h>

#include "BeepCore.h"

void BeepCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_sampleRate = sampleRate;
}

void BeepCore::setParameters(const SynthParams& params) {
    m_level = params.beepLevel;
    m_bypassAdsr = params.beepAdsrBypass;
    m_adsr = params.beepAdsr;

    auto calcInc = [&](float time) { return time <= 0.001f ? 1.0f : 1.0f / (time * (float)m_sampleRate); };
    m_attackInc = calcInc(m_adsr.a);
    m_decayDec = calcInc(m_adsr.d);
    m_releaseDec = calcInc(m_adsr.r);
}

void BeepCore::noteOn(float freq, float velocity) {
    m_baseFreq = freq;
    m_phase = 0.0f;
    m_phaseDelta = m_baseFreq / (float)m_sampleRate;

    m_targetLevel = velocity * m_level;

    if (m_bypassAdsr) {
        m_currentLevel = m_targetLevel;
        m_state = State::Sustain;
    }
    else {
        m_currentLevel = 0.0f;
        m_state = State::Attack;
    }
}

void BeepCore::noteOff() {
    if (m_bypassAdsr) {
        m_currentLevel = 0.0f;
        m_state = State::Idle;
    }
    else {
        m_state = State::Release;
    }
}

bool BeepCore::isPlaying() const { return m_state != State::Idle; }

void BeepCore::setPitchBend(int pitchWheelValue) {
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;
    m_pitchBendRatio = std::pow(2.0f, (norm * 2.0f) / 12.0f);
    m_phaseDelta = (m_baseFreq * m_pitchBendRatio) / (float)m_sampleRate;
}

void BeepCore::updateEnvelope() {
    if (m_state == State::Attack) {
        m_currentLevel += m_attackInc;
        if (m_currentLevel >= m_targetLevel) { m_currentLevel = m_targetLevel; m_state = State::Decay; }
    }
    else if (m_state == State::Decay) {
        float sl = m_targetLevel * m_adsr.s;
        m_currentLevel -= m_decayDec;
        if (m_currentLevel <= sl) { m_currentLevel = sl; m_state = State::Sustain; }
    }
    else if (m_state == State::Release) {
        m_currentLevel -= m_releaseDec;
        if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
    }
}

float BeepCore::getSample() {
    if (m_state == State::Idle) return 0.0f;
    if (!m_bypassAdsr) updateEnvelope();

    // 究極にシンプルな1-bit矩形波（50% Duty）
    float output = (m_phase < 0.5f) ? 1.0f : -1.0f;

    m_phase += m_phaseDelta;
    if (m_phase >= 1.0f) m_phase -= 1.0f;

    // 音量に変換 (0.2f はフルボリューム時のクリッピング防止用ゲイン)
    return output * m_currentLevel * 0.2f;
}

// モジュレーションホイール (0 - 127)
void BeepCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

void BeepCore::setPitchBendRatio(float ratio)
{
    m_pitchBendRatio = ratio;
}