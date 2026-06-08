#pragma once

#include <JuceHeader.h>
#include <cmath>
#include <cstdlib>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Synth/SynthCore.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Generator/Noise/Ssg/GenNoiseSsg.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "../../Generator/Fm/Fix/FmFix.h"
#include "../../Advanced/Curve/AdvancedCurve.h"

class SsgCore : public SynthCore
{
public:
    SsgCore();

    static const std::array<float, 9> dutyPresets;

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
    double m_sampleRate = 44100.0;

    float m_level = 1.0f;
    float m_noiseLevel = 0.0f;
    float m_mix = 0.5;
    int m_waveform = 0; // 0:Pulse, 1:Triangle

    AmpAdsrEnv m_adsr;
    PitchAdsrEnv m_pitchAdsr;
	SsgSwEnv m_ssgSwEnv;
    Opzx7Detune m_detune;
    Opzx7LfoCore m_lfo;
    FixMode m_fixMode;

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
    bool m_dutyFc = false;
    float m_dutyFcFluc = 0.2f;

	// Triangle Params
    bool m_triKeyTrack = true;
    float m_triPeak = 0.5f; // Peak Position
    float m_triFreq = 440.0f;

    // Noise LFSR Params
    SsgNoiseGen m_noiseGen;

    // Rate / Quality Params
    int m_rateIndex = 1; // Default 55.5k
    double m_targetRate = 44100.0;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_prevSample = 0.0f;
    float m_quantizeSteps = 15.0f; // Default 4bit
    float m_currentFrequency = 440.0f;

    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_currentLevel = 0.0f;
    float m_baseLevel = 0.0f;

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    void updatePhaseDelta();

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
    int m_unisonIndex = 0;
    int m_unisonTotal = 1;
    float m_unisonDetuneAmt = 0.0f;
    float m_unisonSpreadAmt = 0.0f;
    float m_unisonPhaseOffset = 0.0f;
};
