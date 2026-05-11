#pragma once

#include <random>
#include <array>
#include <cmath>
#include <algorithm>

#include "../../Core/Fm/FmCore.h"
#include "../../Generator/Noise/Lfsr/Core.h"
#include "../../Generator/Fm/Alg/Opzx7/Core.h"

#include "./Operator/Core.h"

// ==========================================================
// OPZX7 Core
// Base: OPZ (YM2414)
// Extension: OPX (YMF271) Algorithms & MA-7 Waveforms
// ==========================================================
class Opzx7Core : public FmCore
{
public:
    Opzx7Core() : FmCore() {}

    using Opzx7LfoCalculator = float(*)(double phase, float noise);

    // OPM/PG-LFO波形の計算アルゴリズム配列
    static const std::array<Opzx7LfoCalculator, 8> lfoPgStrategies;

    // OPM/EG-LFO波形の計算アルゴリズム配列
    static const std::array<Opzx7LfoCalculator, 8> lfoEgStrategies;

    void prepare(double sampleRate) override;
    void setParameters(const SynthParams& params);
    void noteOn(float freq, float velocity, int midiNote) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    float getSample() override;
    void setPcmBuffer(int opIndex, const std::vector<float>* pcmData);
    void setWtBuffer(int opIndex, const std::vector<float>* wtData);
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
    void setSampleRate(double sampleRate) override ;
private:
    std::array<Opzx7Operator, 4> m_operators;
    std::array<bool, 4> m_opMask{ false, false, false, false };
    std::array<Opzx7Alg::OpGetSampleFunc, 4> opGetSamples;
    Opzx7Alg alg;
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
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;
    bool m_pm = false;
    bool m_am = false;
    int m_lfoPgWave = 0;
    int m_lfoEgWave = 0;
    float m_pms = 0.0f;
    float m_ams = 0.0f;
    float m_pmd = 0.0f;
    float m_amd = 0.0f;

    // Noise LFSR Variables
    unsigned int m_lfsr = 0x1FFFF;
    float m_noisePhase = 0.0f;
    float m_noiseDelta = 0.0f;
    float m_currentNoiseSample = 0.0f;
    float m_targetNoiseFreq = 12000.0f;
    float m_amSmooth = 0.0f;
    float m_amSmoothRate = 0.005f;

    float m_modWheel = 0.0f;

    // LFO Sync Delay とカウンター
    float m_lfoSyncDelay = 0.0f;
    float m_lfoDelayCounter = 0.0f;

    struct AlgRouting {
        float in2_1;             // OP2への入力 (1からの割合)
        bool  fb2_1;             // OP2からOP1へのフィードバック有無
        float in3_1, in3_2;      // OP3への入力 (1, 2からの割合)
        float in4_1, in4_2, in4_3; // OP4への入力 (1, 2, 3からの割合)
        float out_1, out_2, out_3, out_4; // 最終出力へのミックス割合
    };

    static const std::array<AlgRouting, 36> routings; // 36個のアルゴリズム定義

    int m_panpot = 0;
    bool m_panpot_enable = false;
    float m_panpot_l_rate = 1.0f;
    float m_panpot_r_rate = 1.0f;
};
