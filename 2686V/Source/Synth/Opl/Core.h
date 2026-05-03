#pragma once

#include "../../Core/Synth/SynthCore.h"
#include "../../Core/Synth/SynthParams.h"
#include "../../Generator/Fm/Alg/Opl/Alg.h"
#include "./Operator/Core.h"

// ==========================================================
// OPL (YM3526/3812) Core
// Features: 2 Operators, 2 Algorithms (FM/AM), Wave Select
// ==========================================================
class OplCore : public SynthCore
{
public:
    OplCore(): SynthCore() {}

    void prepare(double sampleRate) override;
    void setSampleRate(double sampleRate) override {};
    void setParameters(const SynthParams& params) override;
    void noteOn(float freq, float velocity, int midiNote) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;

    // LFO Sync Delay とカウンター
    float m_lfoSyncDelay = 0.0f;
    float m_lfoDelayCounter = 0.0f;
    uint32_t m_lfoCycleCount = 0;
    unsigned int m_lfsr = 0x1FFFF;
    float m_currentNoiseSample = 0.0f;
    double m_lfoPhase = 0.0;
    float  m_amSmooth = 0.0f;

    struct BaseAlgRouting {
        float in2_1;             // OP2への入力 (1からの割合)
        float in3_1, in3_2;      // OP3への入力 (1, 2からの割合)
        float in4_1, in4_2, in4_3; // OP4への入力 (1, 2, 3からの割合)
        float out_1, out_2, out_3, out_4; // 最終出力へのミックス割合
    };

    static const std::array<BaseAlgRouting, 8> baseRoutings;

    // =======================================================
    // LFO ストラテジー定義
    // =======================================================
    // PMとAMの2つの値を返すための構造体
    struct LfoResult {
        float pm;
        float am;
    };

    // LFO計算関数の型（位相とノイズを受け取り、LfoResultを返す関数ポインタ）
    using LfoCalculator = LfoResult(*)(double phase, float noise);

    // 5つのLFO波形の計算アルゴリズム配列
    static const std::array<LfoCalculator, 5> lfoStrategies;

    // 4つのN88ソフトウェアLFO波形の計算アルゴリズム配列
    static const std::array<LfoCalculator, 4> lfoN88Strategies;

    // 6つのN88ソフトウェアLFO波形の計算アルゴリズム配列
    static const std::array<LfoCalculator, 6> lfoN8886Strategies;
private:
    OplAlg m_alg;
    std::array<OplAlg::OpGetSampleFunc, 2> m_opGetSamples;
    std::array<OplOperator, 2> m_operators;
    std::array<bool, 2> m_opMask{ false, false };

    double m_hostSampleRate = 44100.0;
    int m_rateIndex = 1;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_prevSample = 0.0f;
    float m_quantizeSteps = 0.0f;
    double m_amPhase = 0.0;
    double m_vibPhase = 0.0;

    float m_modWheel = 0.0f;
    double m_lfoPhase = 0.0;
    float m_lfoFreq = 5.0f;

    std::array<double, 2> m_amPhases{ 0.0, 0.0 };
    std::array<double, 2> m_vibPhases{ 0.0, 0.0 };
};
