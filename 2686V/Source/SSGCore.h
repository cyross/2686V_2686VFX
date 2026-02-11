#pragma once
#include <JuceHeader.h>
#include <cmath>
#include <cstdlib>
#include "Mode.h"

class SsgCore
{
public:
    SsgCore() {
        m_lfsr = 0x1FFFF;
    }

    void prepare(double sampleRate) {
        if (sampleRate > 0.0) m_sampleRate = sampleRate;

        updateIncrements();
        updatePhaseDelta();
    }
    void setSampleRate(double sampleRate) { m_sampleRate = sampleRate; updateIncrements(); }

    void setParameters(const SynthParams& params)
    {
        m_level = params.ssgLevel;
        m_noiseLevel = params.ssgNoiseLevel;
        m_mix = params.ssgMix;
        m_targetNoiseFreq = params.ssgNoiseFreq;

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
            updatePhaseDelta();
        }

        // Bit Depth
        switch (params.ssgBitDepth) {
        case 0: m_quantizeSteps = 15.0f; break; // 4bit (Real SSG Volume steps)
        case 1: m_quantizeSteps = 31.0f; break; // 5bit
        case 2: m_quantizeSteps = 63.0f; break; // 6bit
        case 3: m_quantizeSteps = 255.0f; break; // 8bit
        case 4: m_quantizeSteps = 0.0f; break;   // Raw
        default: m_quantizeSteps = 255.0f; break;
        }

        updateIncrements();
    }

    void noteOn(float frequency)
    {
        m_currentFrequency = frequency; // Save for recalculation
        m_phase = 0.0f;

        updatePhaseDelta();

        m_hwEnvPhase = 0.0f;
        m_currentLevel = 0.0f;
        m_state = State::Attack;

        // Reset Rate Logic
        m_rateAccumulator = 1.0f;
        m_lastSample = 0.0f;
    }

    void noteOff()
    {
        m_state = State::Release;
    }

    bool isPlaying() const { return m_state != State::Idle; }

    float getSample()
    {
        if (m_state == State::Idle) return 0.0f;

        // --- ADSR / Gate Logic (Host Rate for smooth envelope) ---
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
        double targetRate = getTargetRate();
        double step = targetRate / m_sampleRate;
        m_rateAccumulator += step;

        // Update core logic only when virtual clock ticks
        if (m_rateAccumulator >= 1.0)
        {
            m_rateAccumulator -= 1.0;

            // ==========================================
            // 1. Hardware Envelope Update (Target Rate)
            // ==========================================
            // HW Env Freq is typically low, so delta is small
            float hwEnvDelta = m_envFreq / (float)targetRate;
            m_hwEnvPhase += hwEnvDelta;

            float hwEnvGain = 1.0f;

            if (m_useHwEnv)
            {
                double p = m_hwEnvPhase;
                bool isEvenCycle = ((int)p % 2 == 0);
                float phaseNorm = (float)(p - std::floor(p));

                switch (m_envShape)
                {
                case 0: hwEnvGain = 1.0f - phaseNorm; break; // \___
                case 1: hwEnvGain = (p < 1.0) ? (1.0f - phaseNorm) : 0.0f; break; // \___ (One shot)
                case 2: hwEnvGain = isEvenCycle ? (1.0f - phaseNorm) : phaseNorm; break; // \/\/
                case 3: hwEnvGain = (p < 1.0) ? (1.0f - phaseNorm) : 1.0f; break; // \"""
                case 4: hwEnvGain = phaseNorm; break; // /|/|
                case 5: hwEnvGain = (p < 1.0) ? phaseNorm : 1.0f; break; // /"""
                case 6: hwEnvGain = isEvenCycle ? phaseNorm : (1.0f - phaseNorm); break; // /\/\ (Triangle)
                case 7: hwEnvGain = (p < 1.0) ? phaseNorm : 0.0f; break; // /___
                }
            }

            // ==========================================
            // 2. Waveform Generation (Target Rate)
            // ==========================================
            float toneSample = 0.0f;

            if (m_waveform == 0) // Pulse
            {
                float currentDuty = 0.5f;
                if (m_dutyMode == 0) { // Preset
                    switch (m_dutyPreset) {
                    case 0: currentDuty = 0.5f; break;     // 1:1
                    case 1: currentDuty = 0.4375f; break;  // 7:9
                    case 2: currentDuty = 0.375f; break;   // 3:5
                    case 3: currentDuty = 0.3125f; break;  // 5:11
                    case 4: currentDuty = 0.25f; break;    // 1:3
                    case 5: currentDuty = 0.20f; break;    // 1:4
                    case 6: currentDuty = 0.1875f; break;  // 3:13
                    case 7: currentDuty = 0.125f; break;   // 1:7
                    case 8: currentDuty = 0.0625f; break;  // 1:15
                    default: currentDuty = 0.5f; break;
                    }
                }
                else {
                    currentDuty = m_dutyVar;
                }
                if (m_dutyInvert) currentDuty = 1.0f - currentDuty;

                toneSample = (m_phase < currentDuty) ? 1.0f : -1.0f;
            }
            else // Triangle
            {
                // Triangle Phase is driven either by note freq or fixed freq
                // Already handled in Phase Update below, here just shape it
                float phaseNorm = m_phase;
                float k = m_triPeak;
                if (k < 0.001f) k = 0.001f;
                if (k > 0.999f) k = 0.999f;

                if (phaseNorm < k) toneSample = -1.0f + 2.0f * (phaseNorm / k);
                else               toneSample = 1.0f - 2.0f * ((phaseNorm - k) / (1.0f - k));
            }

            // Phase Update
            float phaseInc = 0.0f;
            if (m_waveform == 1 && !m_triKeyTrack) {
                phaseInc = m_triFreq / (float)targetRate;
            }
            else {
                phaseInc = m_phaseDelta; // Note Freq / TargetRate
            }
            m_phase += phaseInc;
            if (m_phase >= 1.0f) m_phase -= 1.0f;

            // ==========================================
            // 3. Noise Generator (Target Rate)
            // ==========================================
            m_noisePhase += m_noiseDelta; // TargetNoiseFreq / TargetRate
            if (m_noisePhase >= 1.0f)
            {
                m_noisePhase -= 1.0f;
                // LFSR Step
                unsigned int bit0 = m_lfsr & 1;
                unsigned int bit3 = (m_lfsr >> 3) & 1;
                unsigned int nextBit = bit0 ^ bit3;
                m_lfsr >>= 1;
                if (nextBit) m_lfsr |= (1 << 16);

                m_currentNoiseSample = (m_lfsr & 1) ? 1.0f : -1.0f;
            }

            // ==========================================
            // 4. Mixing
            // ==========================================
            // Mix: 0.0=Tone, 1.0=Noise
            // SSG mixer logic: Tone ON/OFF, Noise ON/OFF usually.
            // Here using linear crossfade for flexibility.
            float toneGain = 1.0f - m_mix;
            float noiseGain = m_mix;

            float rawMixed = (toneSample * m_level * toneGain) + (m_currentNoiseSample * m_noiseLevel * noiseGain);

            // Apply HW Env
            rawMixed *= hwEnvGain;

            // ==========================================
            // 5. Quantize (Bit Depth)
            // ==========================================
            if (m_quantizeSteps > 0.0f) {
                // Normalize to 0..1 for stepping
                // Range is roughly -1 to 1 (ignoring overshoots from mix)
                // Clamp to be safe
                if (rawMixed > 1.0f) rawMixed = 1.0f;
                if (rawMixed < -1.0f) rawMixed = -1.0f;

                float norm = (rawMixed + 1.0f) * 0.5f;
                float quantized = std::floor(norm * m_quantizeSteps) / m_quantizeSteps;
                m_lastSample = (quantized * 2.0f) - 1.0f;
            }
            else {
                m_lastSample = rawMixed;
            }
        }

        // Output Sample (Sample & Hold) * ADSR Gain
        return m_lastSample * m_currentLevel * 0.5f;
    }

private:
    void updateIncrements() {
        if (m_sampleRate <= 0.0) return;
        m_attackInc = 1.0f / (float)(std::max(0.001f, m_adsr.a) * m_sampleRate);
        m_decayDec = 1.0f / (float)(std::max(0.001f, m_adsr.d) * m_sampleRate);
        m_releaseDec = 1.0f / (float)(std::max(0.001f, m_adsr.r) * m_sampleRate);
        m_noiseDelta = m_targetNoiseFreq / m_sampleRate;
    }

    // New Rate Logic
    double getTargetRate() const {
        switch (m_rateIndex) {
        case 0: return 96000.0;
        case 1: return 55500.0; // Typical OPNA
        case 2: return 48000.0;
        case 3: return 44100.0;
        case 4: return 22050.0;
        case 5: return 16000.0;
        case 6: return 8000.0;
        default: return 55500.0;
        }
    }

    void updatePhaseDelta() {
        double targetRate = getTargetRate();
        if (targetRate > 0.0) {
            m_phaseDelta = m_currentFrequency / targetRate;
            m_noiseDelta = m_targetNoiseFreq / targetRate;
        }
    }

    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0;

    float m_level = 1.0f;
    float m_noiseLevel = 0.0f;
    float m_mix = 0.5;
    int m_waveform = 0; // 0:Pulse, 1:Triangle

    SimpleAdsr m_adsr;
    bool m_adsrBypass = false;

    // HW Env Params
    bool m_useHwEnv = false;
    int m_envShape = 0;
    float m_envFreq = 1.0f;
    double m_hwEnvPhase = 0.0;

    // Duty Params
    int m_dutyMode = 0;
    int m_dutyPreset = 0;
    float m_dutyVar = 0.5f;
    bool m_dutyInvert = false;

	// Triangle Params
    bool m_triKeyTrack = true;
    float m_triPeak = 0.5f; // Peak Position
    float m_triFreq = 440.0f;

    // Noise LFSR Params
    unsigned int m_lfsr = 0x1FFFF; // 17-bit Shift Register (Seed must be non-zero)
    float m_noisePhase = 0.0f;
    float m_noiseDelta = 0.0f;
    float m_currentNoiseSample = 0.0f;
    float m_targetNoiseFreq = 12000.0f;

    // Rate / Quality Params
    int m_rateIndex = 1; // Default 55.5k
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_quantizeSteps = 15.0f; // Default 4bit
    float m_currentFrequency = 440.0f;

    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_currentLevel = 0.0f;
    float m_attackInc = 0.0f; float m_decayDec = 0.0f; float m_releaseDec = 0.0f;
};
