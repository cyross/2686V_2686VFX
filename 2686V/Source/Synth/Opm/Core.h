#pragma once

#include <random>

#include "../../Core/Fm/FmCore.h"
#include "../../Generator/Noise/Lfsr/Core.h"
#include "../../Generator/Fm/Alg/Basic/Core.h"

#include "./Operator/Core.h"

class OpmCore : public FmCore
{
public:
    OpmCore() : FmCore() {}
    void prepare(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    void noteOn(float freq, float velocity, int midiNote) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
private:
    std::array<OpmOperator, 4> m_operators;
    std::array<bool, 4> m_opMask{ false, false, false, false };
    std::array<BasicAlg::OpGetSampleFunc, 4> opGetSamples;
    BasicAlg alg;
    LfsrNoiseGen m_noiseGen;

    double m_hostSampleRate = 44100.0;
    int m_algorithm = 0;

    // Rate & Quality
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_prevSample = 0.0f;
    float m_quantizeSteps = 0.0f;

    // OPM LFO
    float m_lfoFreq = 5.0f;
    int m_pms = 0;
    int m_ams = 0;
    int m_lfoPgWave = 2;
    int m_lfoEgWave = 2;
    bool m_pm = false;
    bool m_am = false;
    int m_pmd = 0;
    int m_amd = 0;

    float m_amSmoothRate = 0.005f;

    float m_modWheel = 0.0f;

    using OpmLfoCalculator = float(*)(double phase, float noise);

    // OPM/PG-LFO波形の計算アルゴリズム配列
    static const std::array<OpmLfoCalculator, 4> lfoHwPgStrategies;

    // OPM/EG-LFO波形の計算アルゴリズム配列
    static const std::array<OpmLfoCalculator, 4> lfoHwEgStrategies;
};