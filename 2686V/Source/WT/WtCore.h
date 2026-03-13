#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <cmath>

#include "../synth/SynthCore.h"
#include "../synth/SynthParams.h"

class WtCore : SynthCore
{
public:
    WtCore();

    void prepare(double sampleRate);
    void setParameters(const SynthParams& params);
    void noteOn(float frequency);
    void noteOff();
    bool isPlaying() const;
    void setPitchBend(int pitchWheelValue);
    void setModulationWheel(int wheelValue);
    void setPitchBendRatio(float ratio);
    float getSample();
private:
    void generateWaveform(int type);
    void updateIncrements();
    void updatePhaseDelta();

    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0;

    SimpleAdsr m_adsr;
    float m_level = 1.0f;

    // Wave Data
    std::vector<float> m_sourceWave; // Internal High-Res (Length 64)
    int m_tableSize = 32;            // Playback Size (32 or 64)
    float m_quantizeSteps = 15.0f;   // 4bit=15
    int m_waveform = -1; // for initialize
    int m_prevTableSize = -1; // サイズ変更検知用
    std::array<float, 32> m_customWaveCache32; // for data storage
    std::array<float, 64> m_customWaveCache64; // for data storage

    // Rate / Lo-Fi
    int m_rateIndex = 5; // Default ID 6 (16kHz) -> Index 5
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_currentFrequency = 440.0f;

    // Modulation
    bool m_modEnable = false;
    float m_modDepth = 0.0f;
    float m_modSpeed = 1.0f;
    float m_modPhase = 0.0f;

    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_currentLevel = 0.0f;
    float m_attackInc = 0.0f;
    float m_decayDec = 0.0f;
    float m_releaseDec = 0.0f;

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;
};
