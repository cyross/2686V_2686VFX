#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <cmath>

#include "../../Core/Synth/SynthCore.h"
#include "../../Core/Synth/SynthParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "../../Generator/Fm/Fix/FmFix.h"
#include "../../Advanced/Curve/AdvancedCurve.h"

class Wt2Core : public SynthCore
{
public:
    Wt2Core();

    void prepare(double sampleRate) override;
	void setSampleRate(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    void noteOn(float freq, float velocity, int midiNote, bool isLegato = false) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    void setPitchBendRatio(float ratio) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
    void setCurveCore(CurveCore* p_curveCore);

    // ユニゾン・ハーモニー用
    void setUnisonParams(int index, int total, float detune, float spread) {
        m_unisonIndex = index;
        m_unisonTotal = total;
        m_unisonDetuneAmt = detune;
        m_unisonSpreadAmt = spread;

        // ユニゾンのインデックスに応じて位相を均等にずらす (0.0 〜 1.0)
        // (例: 3ボイスなら 0.0, 0.33, 0.66)
        m_unisonPhaseOffset = (total > 1) ? ((float)index / (float)total) : 0.0f;
    }
private:
    void generateWaveform(int type);
    void updatePhaseDelta();

    double m_sampleRate = 44100.0;

    AmpAdsrEnv m_adsr;
    PitchAdsrEnv m_pitchAdsr;
    SsgSwEnv m_ssgSwEnv;
    Opzx7Detune m_detune;
    Opzx7LfoCore m_lfo;
    FixMode m_fixMode;

    float m_level = 1.0f;

    // Wave Data
    std::vector<float> m_sourceWave; // Internal High-Res (Length 64)
    int m_tableSizeIndex = 0;
    int m_tableSize = 32;            // Playback Size (32 or 64)
    float m_quantizeSteps = 15.0f;   // 4bit=15
    int m_waveform = -1; // for initialize
    int m_prevTableSize = -1; // サイズ変更検知用
    int m_customWaveResolution = 0; // 現在の解像度(0〜4)

    std::array<int, 32> m_customWaveCache32; // for data storage
    std::array<int, 64> m_customWaveCache64; // for data storage
    std::array<int, 128> m_customWaveCache128; // for data storage
    std::array<int, 256> m_customWaveCache256; // for data storage
    std::array<int, 4> m_tableSizes;
    std::array<int*, 4> m_customWaves;

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
    float m_baseLevel = 0.0f;

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
    int m_unisonIndex = 0;
    int m_unisonTotal = 1;
    float m_unisonDetuneAmt = 0.0f;
    float m_unisonSpreadAmt = 0.0f;
    float m_unisonPhaseOffset = 0.0f;
};
