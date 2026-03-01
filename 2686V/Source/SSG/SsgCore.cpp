#include "SsgCore.h"
#include "../synth/SynthHelpers.h"

SsgCore::SsgCore() {
    m_lfsr = 0x1FFFF;
}

void SsgCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_sampleRate = sampleRate;

    updateIncrements();
    updatePhaseDelta();
}

void SsgCore::setSampleRate(double sampleRate) { m_sampleRate = sampleRate; updateIncrements(); }

void SsgCore::setParameters(const SynthParams& params)
{
    m_level = params.ssgLevel;
    m_noiseLevel = params.ssgNoiseLevel;
    m_mix = params.ssgMix;
    m_baseNoiseFreq = params.ssgNoiseFreq;
    m_noiseOnNote = params.ssgNoiseOnNote;

    m_adsrBypass = params.ssgAdsrBypass;
    m_adsr = params.ssgAdsr;

    m_waveform = params.ssgWaveform;

    m_useHwEnv = params.ssgUseHwEnv;
    m_envShape = params.ssgEnvShape;
    m_envFreq = params.ssgEnvPeriod;

    m_dutyMode = params.ssgDutyMode;
    m_dutyPreset = params.ssgDutyPreset;
    m_dutyVar = params.ssgDutyVar;
    m_dutyInvert = params.ssgDutyInvert;

    m_triKeyTrack = params.ssgTriKeyTrack;
    m_triPeak = params.ssgTriPeak;
    m_triFreq = params.ssgTriFreq;

    if (m_rateIndex != params.ssgRateIndex) {
        m_rateIndex = params.ssgRateIndex;
    }

    m_quantizeSteps = getTargetBitDepth(params.ssgBitDepth);

    updateIncrements();
    updateNoiseFrequency();
    updatePhaseDelta();
}

void SsgCore::noteOn(float frequency)
{
    m_currentFrequency = frequency; // Save for recalculation
    m_phase = 0.0f;

    updateNoiseFrequency();

    updatePhaseDelta();

    m_hwEnvPhase = 0.0f;
    m_currentLevel = 0.0f;
    m_state = State::Attack;

    // Reset Rate Logic
    m_rateAccumulator = 1.0f;
    m_lastSample = 0.0f;
}

void SsgCore::noteOff()
{
    m_state = State::Release;
}

bool SsgCore::isPlaying() const { return m_state != State::Idle; }

// ピッチベンド (0 - 16383, Center=8192)
void SsgCore::setPitchBend(int pitchWheelValue)
{
    // 範囲を -1.0 ～ 1.0 に正規化
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;

    // 半音単位のレンジ (例: +/- 2半音)
    float semitones = 2.0f;

    // 比率計算: 2^(semitones / 12)
    // norm * semitones で変化量を決定
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);

    setPitchBendRatio(ratio);
}

// モジュレーションホイール (0 - 127)
void SsgCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

void SsgCore::setPitchBendRatio(float ratio)
{ m_pitchBendRatio = ratio; }

// --- SsgCore.cpp : getSample() を丸ごと差し替え ---

float SsgCore::getSample()
{
    if (m_state == State::Idle) return 0.0f;

    // --- ADSR / Gate Logic ---
    if (m_adsrBypass)
    {
        if (m_state == State::Release) {
            m_currentLevel = 0.0f;
            m_state = State::Idle;
            return 0.0f;
        }
        m_currentLevel = 1.0f;
    }
    else
    {
        if (m_state == State::Attack) {
            m_currentLevel += m_attackInc;
            if (m_currentLevel >= 1.0f) { m_currentLevel = 1.0f; m_state = State::Decay; }
        }
        else if (m_state == State::Decay) {
            if (m_currentLevel > m_adsr.s) {
                m_currentLevel -= m_decayDec;
                if (m_currentLevel <= m_adsr.s) { m_currentLevel = m_adsr.s; m_state = State::Sustain; }
            }
            else { m_currentLevel = m_adsr.s; m_state = State::Sustain; }
        }
        else if (m_state == State::Sustain) {
            m_currentLevel = m_adsr.s;
        }
        else if (m_state == State::Release) {
            m_currentLevel -= m_releaseDec;
            if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
        }
    }

    // --- Sample Rate Emulation ---
    double targetRate = getTargetRate(m_rateIndex);
    double stepSize = targetRate / m_sampleRate;
    m_rateAccumulator += stepSize;

    // ★修正: SSG（矩形波）の場合は、線形補間ではなく「平均化(アベレージング)」が正解
    int steps = 0;
    float sumOut = 0.0f;

    // Update core logic only when virtual clock ticks
    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        // ==========================================
        // LFO Calculation (Software Vibrato)
        // ==========================================
        double lfoInc = m_lfoFreq / targetRate;
        m_lfoPhase += lfoInc;
        if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

        float lfoVal = 0.0f;
        if (m_lfoPhase < 0.25)      lfoVal = (float)(m_lfoPhase * 4.0);
        else if (m_lfoPhase < 0.75) lfoVal = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
        else                        lfoVal = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);

        float modDepth = m_modWheel * 0.03f;
        float lfoPitchMod = 1.0f + (lfoVal * modDepth);
        float freqMult = m_pitchBendRatio * lfoPitchMod;

        float phaseInc = 0.0f;
        if (m_waveform == 1 && !m_triKeyTrack) {
            phaseInc = (m_triFreq / (float)targetRate) * freqMult;
        }
        else {
            phaseInc = m_phaseDelta * freqMult;
        }

        // ==========================================
        // 1. Hardware Envelope Update
        // ==========================================
        float hwEnvDelta = m_envFreq / (float)targetRate;
        m_hwEnvPhase += hwEnvDelta;

        // ★修正: 位相が無限増大して小数の精度が落ちるのを防ぐラップアラウンド
        if (m_hwEnvPhase >= 2.0) {
            if (m_envShape % 2 == 0) m_hwEnvPhase -= 2.0;
            else m_hwEnvPhase = 2.0;
        }

        float hwEnvGain = 1.0f;
        if (m_useHwEnv)
        {
            double p = m_hwEnvPhase;
            bool isEvenCycle = ((int)p % 2 == 0);
            float phaseNorm = (float)(p - std::floor(p));

            switch (m_envShape)
            {
            case 0: hwEnvGain = 1.0f - phaseNorm; break;
            case 1: hwEnvGain = (p < 1.0) ? (1.0f - phaseNorm) : 0.0f; break;
            case 2: hwEnvGain = isEvenCycle ? (1.0f - phaseNorm) : phaseNorm; break;
            case 3: hwEnvGain = (p < 1.0) ? (1.0f - phaseNorm) : 1.0f; break;
            case 4: hwEnvGain = phaseNorm; break;
            case 5: hwEnvGain = (p < 1.0) ? phaseNorm : 1.0f; break;
            case 6: hwEnvGain = isEvenCycle ? phaseNorm : (1.0f - phaseNorm); break;
            case 7: hwEnvGain = (p < 1.0) ? phaseNorm : 0.0f; break;
            }
        }

        // ==========================================
        // 2. Waveform Generation
        // ==========================================
        float toneSample = 0.0f;

        if (m_waveform == 0) // Pulse
        {
            float currentDuty = 0.5f;
            if (m_dutyMode == 0) {
                switch (m_dutyPreset) {
                case 0: currentDuty = 0.5f; break;
                case 1: currentDuty = 0.4375f; break;
                case 2: currentDuty = 0.375f; break;
                case 3: currentDuty = 0.3125f; break;
                case 4: currentDuty = 0.25f; break;
                case 5: currentDuty = 0.20f; break;
                case 6: currentDuty = 0.1875f; break;
                case 7: currentDuty = 0.125f; break;
                case 8: currentDuty = 0.0625f; break;
                default: currentDuty = 0.5f; break;
                }
            }
            else {
                currentDuty = m_dutyVar;
            }
            if (m_dutyInvert) currentDuty = 1.0f - currentDuty;

            // 極端なデューティ比による波形消失を防ぐ最低保証
            float minDuty = phaseInc;
            if (currentDuty < minDuty) currentDuty = minDuty;
            if (currentDuty > 1.0f - minDuty) currentDuty = 1.0f - minDuty;

            toneSample = (m_phase < currentDuty) ? 1.0f : -1.0f;

        }
        else // Triangle
        {
            float phaseNorm = m_phase;
            float k = m_triPeak;
            if (k < 0.001f) k = 0.001f;
            if (k > 0.999f) k = 0.999f;

            if (phaseNorm < k) toneSample = -1.0f + 2.0f * (phaseNorm / k);
            else                toneSample = 1.0f - 2.0f * ((phaseNorm - k) / (1.0f - k));
        }

        m_phase += phaseInc;
        if (m_phase >= 1.0f) m_phase -= 1.0f;

        // ==========================================
        // 3. Noise Generator
        // ==========================================
        if (m_targetNoiseFreq > 0.001f)
        {
            m_noisePhase += m_noiseDelta;
            while (m_noisePhase >= 1.0f)
            {
                m_noisePhase -= 1.0f;
                unsigned int bit0 = m_lfsr & 1;
                unsigned int bit3 = (m_lfsr >> 3) & 1;
                unsigned int nextBit = bit0 ^ bit3;
                m_lfsr >>= 1;
                if (nextBit) m_lfsr |= (1 << 16);
                m_currentNoiseSample = (m_lfsr & 1) ? 1.0f : -1.0f;
            }
        }
        else {
            m_currentNoiseSample = 0.0f;
        }

        // ==========================================
        // 4. Mixing
        // ==========================================
        float toneGain = 1.0f - m_mix;
        float noiseGain = m_mix;
        float rawMixed = (toneSample * m_level * toneGain) + (m_currentNoiseSample * m_noiseLevel * noiseGain);

        rawMixed *= hwEnvGain;

        // ==========================================
        // 5. Quantize (Bit Depth)
        // ==========================================
        float finalOut = 0.0f;
        if (m_quantizeSteps > 0.0f) {
            if (rawMixed > 1.0f) rawMixed = 1.0f;
            if (rawMixed < -1.0f) rawMixed = -1.0f;

            // ★修正: ディザ(dither)を削除し、実機DACと同じ純粋な四捨五入にする(プレビューのゴミ解消)
            float norm = (rawMixed + 1.0f) * 0.5f;
            float quantized = std::round(norm * m_quantizeSteps) / m_quantizeSteps;
            finalOut = (quantized * 2.0f) - 1.0f;
        }
        else {
            finalOut = rawMixed;
        }

        sumOut += finalOut;
        steps++;
    }

    if (steps > 0) {
        m_lastSample = sumOut / (float)steps;
    }

    return m_lastSample * m_currentLevel * 0.5f;
}

void SsgCore::updateIncrements()
{
    if (m_sampleRate <= 0.0) return;
    m_attackInc = 1.0f / (float)(std::max(0.001f, m_adsr.a) * m_sampleRate);
    m_decayDec = 1.0f / (float)(std::max(0.001f, m_adsr.d) * m_sampleRate);
    m_releaseDec = 1.0f / (float)(std::max(0.001f, m_adsr.r) * m_sampleRate);
}

void SsgCore::updateNoiseFrequency()
{
    if (m_noiseOnNote) {
        // ONの時：キーボードの音程に合わせてノイズの周波数を上下させる
        m_targetNoiseFreq = m_baseNoiseFreq * (m_currentFrequency / 440.0);
    }
    else {
        // OFFの時：スライダーで設定したノイズ周波数に固定する
        m_targetNoiseFreq = m_baseNoiseFreq;
    }
}

void SsgCore::updatePhaseDelta() {
    double targetRate = getTargetRate(m_rateIndex);
    if (targetRate > 0.0) {
        m_phaseDelta = m_currentFrequency / targetRate;
        m_noiseDelta = m_targetNoiseFreq / targetRate;
    }
}
