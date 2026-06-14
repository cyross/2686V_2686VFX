#pragma once

#include "../../Core/Fm/FmCore.h"
#include "../../Generator/Noise/Lfsr/GenNoiseLfsr.h"
#include "../../Effect/Lfo/N88/LfoN88.h"
#include "../../Advanced/Curve/AdvancedCurve.h"

#include "./Operator/SynthOpnOp.h"

// ==========================================================
// OPN (YM2203) Core
// Features: 4 Operators, 8 Algorithms, No SSG-EG, No HW LFO
// ==========================================================
class OpnCore : public FmCore
{
public:
    OpnCore() : FmCore() {}

    void prepare(double sampleRate) override;
    void setSampleRate(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    void noteOn(float freq, float velocity, int midiNote, bool isLegato = false) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
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
    std::array<OpnOperator, 4> m_operators;
    std::array<bool, 4> m_opMask{ false, false, false, false };
    LfsrNoiseGen m_noiseGen;
    N88LfoCore m_n88Lfo;

    float m_level = 1.0f;

    int m_algorithm = 0;
    double m_hostSampleRate = 44100.0;
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_prevSample = 0.0f;
    float m_quantizeSteps = 0.0f;

    int m_lfoWave = 0; // 0:Sine, 1:Saw, 2:Square, 3:Tri, 4:Noise
    float m_amSmooth = 0.0f;
    float m_amSmoothRate = 0.005f;

    float m_modWheel = 0.0f;

    // Software LFO for Mod Wheel
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f; // Default ~5Hz

    // LFO Sync Delay とカウンター
    float m_lfoSyncDelay = 0.0f;
    float m_lfoDelayCounter = 0.0f;
    uint32_t m_lfoCycleCount = 0;

    bool m_pm = false;
    bool m_am = false;
    int m_pms = 0;
    int m_ams = 0;
    int m_pmd = 0;
    int m_amd = 0;
    double m_lfoTimerAcc = 0.0;
    float m_steppedPmLfoVal = 0.0f;
    float m_steppedAmLfoVal = 0.0f;

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
    int m_unisonIndex = 0;
    int m_unisonTotal = 1;
    float m_unisonDetuneAmt = 0.0f;
    float m_unisonSpreadAmt = 0.0f;
    float m_unisonPhaseOffset = 0.0f;
};
