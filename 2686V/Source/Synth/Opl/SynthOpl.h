#pragma once

#include "../../Core/Fm/FmCore.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Processor/Opl/ProcessorOplValues.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"

#include "./Operator/SynthOplOp.h"

// ==========================================================
// OPL (YM3526/3812) Core
// Features: 2 Operators, 2 Algorithms (FM/AM), Wave Select
// ==========================================================
class OplCore : public FmCore
{
public:
    OplCore() : FmCore() {}

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
        std::array<float, OplPrValue::ops> out;                // 最終出力へのミックス割合
        std::array<std::array<float, OplPrValue::ops>, OplPrValue::ops> mod; // mod[dest][src]: srcからdestへの通常の変調割合
        std::array<std::array<float, OplPrValue::ops>, OplPrValue::ops> fbMod; // fbMod[dest][src]: srcからdestへのフィードバック変調割合
    };

    struct ModConnection {
        int srcOp;
        float amount;
        bool isForward;
    };

    struct OpRoutingConfig {
        std::array<ModConnection, OplPrValue::ops> mods;
        int modCount = 0;

        std::array<ModConnection, OplPrValue::ops> fbMods;
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

    static const std::array<AlgRouting, OplPrValue::algorithms> routings;
    std::array<OpRoutingConfig, OplPrValue::ops> m_activeRoutings;
private:
    std::array<OplOperator, OplPrValue::ops> m_operators;
    std::array<bool, OplPrValue::ops> m_opMask{ false };
    std::array<float, OplPrValue::ops> m_history1 = { 0.0f };
    std::array<float, OplPrValue::ops> m_history2 = { 0.0f };

    // チップ全体へ掛かる AMP ENV。オペレータごとのエンベロープとは別に、
    // 出力段でもう一段掛ける。level は次のサンプルへ持ち越す。
    AmpAdsrEnv m_ampEnvG;
    float m_ampEnvGLevel = 0.0f;

    SsgHwEnv m_ssgHwEnv;
    SsgSwEnv11 m_ssgSwEnv11g;

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

    float m_modWheel = 0.0f;

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
};
