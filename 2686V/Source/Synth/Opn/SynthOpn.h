#pragma once

#include "../../Core/Fm/FmCore.h"
#include "../../Generator/Noise/Lfsr/GenNoiseLfsr.h"
#include "../../Effect/Lfo/N88/LfoN88.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"

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
    // ユニゾン・ハーモニーは SynthCore::m_unison に集約

    struct AlgRouting {
        std::array<float, OpnPrValue::ops> out;                // 最終出力へのミックス割合
        std::array<std::array<float, OpnPrValue::ops>, OpnPrValue::ops> mod; // mod[dest][src]: srcからdestへの通常の変調割合
        std::array<std::array<float, OpnPrValue::ops>, OpnPrValue::ops> fbMod; // fbMod[dest][src]: srcからdestへのフィードバック変調割合
    };

    struct ModConnection {
        int srcOp;
        float amount;
        bool isForward;
    };

    struct OpRoutingConfig {
        std::array<ModConnection, OpnPrValue::ops> mods;
        int modCount = 0;

        std::array<ModConnection, OpnPrValue::ops> fbMods;
        int fbModCount = 0;

        float outLevel = 0.0f;
    };

    // =========================================================================
    // テンプレートによる強制ループ展開 (Loop Unrolling)
    // =========================================================================
    template<size_t I>
    inline void processSingleOperator(float* currentOut, float& finalOut);

    template<size_t... Is>
    inline void processAllOperators(std::index_sequence<Is...>, float* currentOut, float& finalOut) {
        // Fold Expression を用いて関数呼び出しをベタ書き展開する
        (processSingleOperator<Is>(currentOut, finalOut), ...);
    }

    static const std::array<AlgRouting, OpnPrValue::algorithms> routings;
    std::array<OpRoutingConfig, OpnPrValue::ops> m_activeRoutings;
private:
    std::array<OpnOperator, OpnPrValue::ops> m_operators;
    std::array<bool, OpnPrValue::ops> m_opMask{ false };
    std::array<float, OpnPrValue::ops> m_history1 = { 0.0f };
    std::array<float, OpnPrValue::ops> m_history2 = { 0.0f };

    LfsrNoiseGen m_noiseGen;
    N88LfoCore m_n88Lfo;

    // チップ全体へ掛かる AMP ENV。オペレータごとのエンベロープとは別に、
    // 出力段でもう一段掛ける。level は次のサンプルへ持ち越す。
    AmpAdsrEnv m_ampEnvG;
    float m_ampEnvGLevel = 0.0f;

    SsgHwEnv m_ssgHwEnv;
    SsgSwEnv11 m_ssgSwEnv11g;

    // チップ全体へ掛かるピッチ側。オペレータは m_globalPitchRatio を
    // 参照しているので、ここを毎サンプル更新すれば全オペレータへ届く。
    SsgSwPEnv11 m_ssgSwPEnv11g;
    float m_globalPitchRatio = 1.0f;

    // チップ全体のピッチ倍率を 1 サンプルぶん進める
    inline void updateGlobalPitchRatio() {
        float ratio = 1.0f;

        if (!m_ssgSwPEnv11g.isBypass()) {
            ratio *= m_ssgSwPEnv11g.process(1.0f);
        }
        else {
            if (m_ssgSwPEnv11g.isRelease()) m_ssgSwPEnv11g.bypassedReleasedProcess();
        }

        m_globalPitchRatio = ratio;
    }

    int m_cachedAlgorithm = -1;
    void updateRoutingCache();

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
};
