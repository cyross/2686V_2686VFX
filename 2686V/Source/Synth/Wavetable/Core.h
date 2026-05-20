#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <cmath>

#include "../../Core/Synth/SynthCore.h"
#include "../../Core/Synth/SynthParams.h"
#include "../../Effect/Envelope/Amp/Adsr/Core.h"
#include "../../Effect/Envelope/Pitch/Adsr/Core.h"
#include "../../Effect/Envelope/Amp/SsgSw/Core.h"
#include "../../Effect/Detune/Opm/Core.h"
#include "../../Effect/Lfo/Opzx7/Core.h"

class WtCore : public SynthCore
{
public:
    WtCore();

    void prepare(double sampleRate) override;
	void setSampleRate(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    void noteOn(float freq, float velocity, int midiNote) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    void setPitchBendRatio(float ratio) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
private:
    void generateWaveform(int type);
    void updatePhaseDelta();

    double m_sampleRate = 44100.0;

    AmpAdsrEnv m_adsr;
    PitchAdsrEnv m_pitchAdsr;
    SsgSwEnv m_ssgSwEnv;
    OpmDetune m_detune;
    Opzx7LfoCore m_lfo;

    float m_level = 1.0f;

    // Wave Data
    std::vector<float> m_sourceWave; // Internal High-Res (Length 64)
    int m_tableSizeIndex = 0;
    int m_tableSize = 32;            // Playback Size (32 or 64)
    float m_quantizeSteps = 15.0f;   // 4bit=15
    int m_waveform = -1; // for initialize
    int m_prevTableSize = -1; // サイズ変更検知用
    std::array<float, 32> m_customWaveCache32; // for data storage
    std::array<float, 64> m_customWaveCache64; // for data storage
    std::array<float, 128> m_customWaveCache128; // for data storage
    std::array<float, 256> m_customWaveCache256; // for data storage
    std::array<int, 4> m_tableSizes;
    std::array<float*, 4> m_customWaves;

    // Rate / Lo-Fi
    int m_rateIndex = 5; // Default ID 6 (16kHz) -> Index 5
    double m_targetRate = 44100.0;
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

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;
};
