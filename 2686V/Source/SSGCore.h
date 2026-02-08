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

    void prepare(double sampleRate) { m_sampleRate = sampleRate; updateIncrements(); }
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

        updateIncrements();
    }

    void noteOn(float frequency)
    {
        m_phase = 0.0;
        m_phaseDelta = (frequency * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

        m_hwEnvPhase = 0.0f;

        m_currentLevel = 0.0f;
        m_state = State::Attack;
    }

    void noteOff()
    {
        m_state = State::Release;
    }

    bool isPlaying() const { return m_state != State::Idle; }

    float getSample()
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
                float diff = m_adsr.s - m_currentLevel;
                if (std::abs(diff) > 0.0001f) m_currentLevel += diff * 0.05f;
                else m_currentLevel = m_adsr.s;
            }
            else if (m_state == State::Release) {
                m_currentLevel -= m_releaseDec;
                if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
            }
        }

        // ==========================================
        // 1. Hardware Envelope / Triangle Phase Update
        // ==========================================
        m_hwEnvPhase += (m_envFreq / m_sampleRate);

        float hwEnvGain = 1.0f; // Default Unity Gain

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

        if (m_waveform == 0) // Pulse (Standard)
        {
            // --- Duty Cycle Calculation ---
            float currentDuty = 0.5f;
            if (m_dutyMode == 0) {
                // 1:1(50%), 7:9(43.75), 3:5(37.5), 5:11(31.25),
                // 1:3(25%), 1:4(20%), 3:13(18.75), 1:7(12.5), 1:15(6.25)
                switch(m_dutyPreset) {
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
            } else {
                currentDuty = m_dutyVar;
            }
            if (m_dutyInvert) currentDuty = 1.0f - currentDuty;

            // Pulse Generation (Uses Note Frequency m_phase)
            toneSample = (m_phase < currentDuty) ? 1.0f : -1.0f;
            m_phase += m_phaseDelta;
            if (m_phase >= 1.0f) m_phase -= 1.0f;
        }
        else // Triangle (Env Speed Driven)
        {
            float phaseInc = 0.0f;

            if (m_triKeyTrack)
            {
                phaseInc = m_phaseDelta; // Note Frequency
            }
            else
            {
                phaseInc = m_triFreq / m_sampleRate;
            }

            m_phase += phaseInc;
            if (m_phase >= 1.0f) m_phase -= 1.0f;

            float phaseNorm = m_phase;

            // Peak Logic (Saw/Tri Transform)
            float k = m_triPeak;
            if (k < 0.001f) k = 0.001f;
            if (k > 0.999f) k = 0.999f;

            if (phaseNorm < k)
            {
                toneSample = -1.0f + 2.0f * (phaseNorm / k);
            }
            else
            {
                toneSample = 1.0f - 2.0f * ((phaseNorm - k) / (1.0f - k));
            }
        }

        // ==========================================
        // Noise Generator (LFSR 17-bit)
        // ==========================================
        // YM2149 / AY-3-8910 Algorithm:
        // Input bit = Bit0 XOR Bit3
        // Shift Right
        // Bit16 = Input bit

        m_noisePhase += m_noiseDelta;
        if (m_noisePhase >= 1.0f)
        {
            m_noisePhase -= 1.0f;

            // LFSR Step
            unsigned int bit0 = m_lfsr & 1;
            unsigned int bit3 = (m_lfsr >> 3) & 1;
            unsigned int nextBit = bit0 ^ bit3;

            m_lfsr >>= 1;

            if (nextBit)
            {
                m_lfsr |= (1 << 16);
            }

            m_currentNoiseSample = (m_lfsr & 1) ? 1.0f : -1.0f;
        }


        // ==========================================
        // 3. Mixing & Output
        // ==========================================

        // Mix = 0.0 -> Tone 100%, Noise 0%
        // Mix = 0.5 -> Tone 50%, Noise 50%
        // Mix = 1.0 -> Tone 0%, Noise 100%
        float toneGain = 1.0f - m_mix;
        float noiseGain = m_mix;

        float mixedSignal = (toneSample * m_level * toneGain) + (m_currentNoiseSample * m_noiseLevel * noiseGain);

        return mixedSignal * m_currentLevel * hwEnvGain * 0.5f;
    }

private:
    void updateIncrements() {
        if (m_sampleRate <= 0.0) return;
        m_attackInc = 1.0f / (float)(std::max(0.001f, m_adsr.a) * m_sampleRate);
        m_decayDec = 1.0f / (float)(std::max(0.001f, m_adsr.d) * m_sampleRate);
        m_releaseDec = 1.0f / (float)(std::max(0.001f, m_adsr.r) * m_sampleRate);
        m_noiseDelta = m_targetNoiseFreq / m_sampleRate;
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

    // Noise LFSR Variables
    unsigned int m_lfsr = 0x1FFFF; // 17-bit Shift Register (Seed must be non-zero)
    float m_noisePhase = 0.0f;
    float m_noiseDelta = 0.0f;
    float m_currentNoiseSample = 0.0f;
    float m_targetNoiseFreq = 12000.0f;

    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_currentLevel = 0.0f;
    float m_attackInc = 0.0f; float m_decayDec = 0.0f; float m_releaseDec = 0.0f;
};
