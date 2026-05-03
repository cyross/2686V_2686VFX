#pragma once

#include "../../Core/Synth/SynthCore.h"
#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Sample/Fm/Core.h"
#include "../../Generator/Noise/Lfsr/Noise.h"
#include "../../Effector/Lfo/Counter/Counter.h"
#include "../../Effector/Lfo/Opna/Lfo.h"
#include "../../Generator/Fm/Alg/Basic/Alg.h"
#include "./Operator/Core.h"

// ==========================================================
// OPNA (YM2608) Core
// Features: 4 Operators, 8 Algorithms, SSG-EG, Hardware LFO
// ==========================================================
class OpnaCore : public SynthCore
{
    OpnaLfo opnaLfo;
public:
    OpnaCore(): SynthCore() {}

    void prepare(double sampleRate) override;
    void setSampleRate(double sampleRate) override {};
    void setParameters(const SynthParams& params) override;
    void noteOn(float freq, float velocity, int midiNote) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setPitchBendRatio(float ratio) override {};
    void setModulationWheel(int wheelValue) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;

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
    FmSampleCore sampleCore;
	LfoCounter lfoCounter;
    BasicAlg alg;
    std::array<BasicAlg::OpGetSampleFunc, 4> opGetSamples;
    std::array<OpnaOperator, 4> operators;
    std::array<bool, 4> opMask{ false, false, false, false };

    double hostSampleRate = 44100.0;

    // Rate & Quality
    int rateIndex = 1;
    float lastSample = 0.0f;
    float prevSample = 0.0f;
    float qntSteps = 0.0f;

	LfsrNoiseGen m_noiseGen;

    float modWheel = 0.0f;
};
