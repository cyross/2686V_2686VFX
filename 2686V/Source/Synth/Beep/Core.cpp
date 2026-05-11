#include <JuceHeader.h>

#include "./Core.h"

void BeepCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_sampleRate = sampleRate;

	m_adsr.prepare(m_sampleRate);
    m_fixMode.setParameters(false, 2000.0f);
}

void BeepCore::setParameters(const SynthParams& params) {
    m_level = params.beep.level;

    m_adsr.setParameters(params.beep.adsr);

	m_fixMode.setParameters(params.beep.fixedMode, params.beep.fixedFreq);
}

void BeepCore::noteOn(float freq, float velocity, int midiNote) {
    m_baseFreq = m_fixMode.noteOn(freq);
    m_phase = 0.0f;
    m_phaseDelta = m_baseFreq / (float)m_sampleRate;

    m_targetLevel = velocity * m_level;

    m_adsr.noteOn();
}

void BeepCore::noteOff() {
    m_adsr.noteOff();
}

bool BeepCore::isPlaying() const { return m_adsr.isPlaying(); }

void BeepCore::setPitchBend(int pitchWheelValue) {
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;
    m_pitchBendRatio = std::pow(2.0f, (norm * 2.0f) / 12.0f);
    m_phaseDelta = (m_baseFreq * m_pitchBendRatio) / (float)m_sampleRate;
}

float BeepCore::getSample() {
    if (m_adsr.isIdle()) {
        return 0.0f;
    }

    m_currentLevel = m_adsr.process(m_currentLevel);

    // 究極にシンプルな1-bit矩形波（50% Duty）
    float output = (m_phase < 0.5f) ? 1.0f : -1.0f;

    m_phase += m_phaseDelta;
    if (m_phase >= 1.0f) m_phase -= 1.0f;

    // 音量に変換
    return output * m_currentLevel;
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

void BeepCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    outL[startSample + sampleIdx] += sample * 0.5f; // モノラルなので、ステレオで言うCと同じ計算
    outR[startSample + sampleIdx] += sample * 0.5f; // モノラルなので、ステレオで言うCと同じ計算

    isActive = isPlaying();
}
