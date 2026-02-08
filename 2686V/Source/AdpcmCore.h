#pragma once
#include <JuceHeader.h>
#include <vector>
#include <cmath>
#include "Mode.h"

class AdpcmCore
{
public:
    AdpcmCore() {}

    void prepare(double sampleRate)
    {
        m_sampleRate = sampleRate;
        updateIncrements();
        if (m_waveTable.empty()) generateWaveform();
    }

    void setParameters(const SynthParams& params)
    {
        m_level = params.adpcmLevel;
        m_isLooping = params.adpcmLoop;
        m_adsr = params.adpcmAdsr;
        updateIncrements();
    }

    void noteOn(float frequency)
    {
        m_position = 0.0;

        float rootFreq = 130.81f;
        m_pitchRatio = frequency / rootFreq;

        m_currentLevel = 0.0f;
        m_state = State::Attack;

        m_hasFinished = false;
    }

    void noteOff()
    {
        m_state = State::Release;
    }

    bool isPlaying() const { return m_state != State::Idle; }

    float getSample()
    {
        if (m_state == State::Idle) return 0.0f;

        if (m_hasFinished) return 0.0f;

        // --- ADSR Logic ---
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

        // --- PCM Playback (Linear Interpolation) ---
        if (m_position >= m_waveTable.size())
        {
            if (m_isLooping)
            {
                m_position -= m_waveTable.size();
            }
            else
            {
                m_hasFinished = true;
                return 0.0f;
            }
        }

        int index0 = (int)m_position;
        int index1 = index0 + 1;
        if (index1 >= m_waveTable.size())
        {
            index1 = m_isLooping ? 0 : index0;
        }

        float frac = (float)(m_position - index0);
        float sample = m_waveTable[index0] * (1.0f - frac) + m_waveTable[index1] * frac;

        m_position += m_pitchRatio;

        return sample * m_level * m_currentLevel * 0.5f;
    }

private:
    void updateIncrements() {
        if (m_sampleRate <= 0.0) return;
        m_attackInc = 1.0f / (float)(std::max(0.001f, m_adsr.a) * m_sampleRate);
        m_decayDec = 1.0f / (float)(std::max(0.001f, m_adsr.d) * m_sampleRate);
        m_releaseDec = 1.0f / (float)(std::max(0.001f, m_adsr.r) * m_sampleRate);
    }

    void generateWaveform()
    {
        double targetFreq = 130.81;
        int numCycles = 130;
        double samplesPerCycle = m_sampleRate / targetFreq;
        int length = (int)std::round(samplesPerCycle * numCycles);

        m_waveTable.resize(length);

        double exactFreq = (double)numCycles * m_sampleRate / (double)length;

        for (int i = 0; i < length; ++i)
        {
            double t = (double)i / m_sampleRate;

            float s1 = std::sin(t * exactFreq * 2.0 * juce::MathConstants<double>::pi);
            float s2 = std::sin(t * exactFreq * 2.0 * 2.0 * juce::MathConstants<double>::pi) * 0.5f;
            float s3 = std::sin(t * exactFreq * 3.0 * 2.0 * juce::MathConstants<double>::pi) * 0.25f;

            m_waveTable[i] = (s1 + s2 + s3) * 0.5f;
        }
    }

    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0;

    // PCM Data
    std::vector<float> m_waveTable;
    double m_position = 0.0;
    float m_pitchRatio = 1.0f;

    // Params
    float m_level = 1.0f;
    SimpleAdsr m_adsr;
    float m_currentLevel = 0.0f;
    float m_attackInc = 0.0f; float m_decayDec = 0.0f; float m_releaseDec = 0.0f;

    bool m_isLooping = true;
    bool m_hasFinished = false;
};
