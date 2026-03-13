#pragma once

#include <random>
#include <array>
#include <cmath>
#include <algorithm>

#include "../fm/FmCore.h"
#include "Opzx3Operator.h"

// ==========================================================
// OPZX3 Core
// Base: OPM (YM2151) / OPZ (YM2414)
// Extension: OPX (YMF271) Algorithms & MA-5 Waveforms
// ==========================================================
class Opzx3Core : public FmCore
{
public:
    Opzx3Core() : FmCore() {}

    void prepare(double sampleRate) override;
    void setParameters(const SynthParams& params);
    void noteOn(float freq, float velocity, int midiNote) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    float getSample() override;
    void setPcmBuffer(int opIndex, const std::vector<float>* pcmData);
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
private:
    std::array<Opzx3Operator, 4> m_operators;
    std::array<bool, 4> m_opMask{ false, false, false, false };

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
    int m_pms = 0; int m_ams = 0; int m_lfoWave = 2;
    bool m_pm = false;
    bool m_am = false;
    int m_pmd = 0;
    int m_amd = 0;

    // Noise LFSR Variables
    unsigned int m_lfsr = 0x1FFFF;
    float m_noisePhase = 0.0f;
    float m_noiseDelta = 0.0f;
    float m_currentNoiseSample = 0.0f;
    float m_targetNoiseFreq = 12000.0f;
    float m_amSmooth = 0.0f;
    float m_amSmoothRate = 0.005f;

    float m_modWheel = 0.0f;

    struct AlgRouting {
        float in2_1;             // OP2への入力 (1からの割合)
        bool  fb2_1;             // OP2からOP1へのフィードバック有無
        float in3_1, in3_2;      // OP3への入力 (1, 2からの割合)
        float in4_1, in4_2, in4_3; // OP4への入力 (1, 2, 3からの割合)
        float out_1, out_2, out_3, out_4; // 最終出力へのミックス割合
    };

    static const std::array<AlgRouting, 36> routings; // 36個のアルゴリズム定義

    void updateNoiseDelta(double targetRate);
};
