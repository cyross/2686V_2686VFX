#pragma once

#include "../../Core/Fm/FmCore.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"
#include "../../Generator/WtMod/GenWtModulator.h"
#include "../../Generator/WtMod/GenWtAmpModulator.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"

#include "./Operator/SynthOpl3Op.h"

// ==========================================================
// OPL3 (YMF262) Core
// Features: 4 Operators, Wave Select (8 types), 4-Op algorithms
// ==========================================================
class Opl3Core : public FmCore
{
public:
    Opl3Core() : FmCore() {}

    struct AlgRouting {
        std::array<float, Opl3PrValue::ops> out;                // 最終出力へのミックス割合
        std::array<std::array<float, Opl3PrValue::ops>, Opl3PrValue::ops> mod; // mod[dest][src]: srcからdestへの通常の変調割合
        std::array<std::array<float, Opl3PrValue::ops>, Opl3PrValue::ops> fbMod; // fbMod[dest][src]: srcからdestへのフィードバック変調割合
    };

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

    struct ModConnection {
        int srcOp;
        float amount;
        bool isForward;
    };

    struct OpRoutingConfig {
        std::array<ModConnection, Opl3PrValue::ops> mods;
        int modCount = 0;

        std::array<ModConnection, Opl3PrValue::ops> fbMods;
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

    static const std::array<AlgRouting, Opl3PrValue::algorithms> routings;
    std::array<OpRoutingConfig, Opl3PrValue::ops> m_activeRoutings;
private:
    std::array<Opl3Operator, Opl3PrValue::ops> m_operators;
    std::array<bool, Opl3PrValue::ops> m_opMask{ false };
    std::array<float, Opl3PrValue::ops> m_history1 = { 0.0f };

    // チップ全体へ掛かる AMP ENV。オペレータごとのエンベロープとは別に、
    // 出力段でもう一段掛ける。level は次のサンプルへ持ち越す。
    AmpAdsrEnv m_ampEnvG;
    float m_ampEnvGLevel = 0.0f;

    SsgHwEnv m_ssgHwEnv;

    // 音量側と同じ形をピッチへ当てるもの。チップ全体のピッチ倍率へ掛ける。
    SsgHwPEnv m_ssgHwPEnv;
    SsgSwEnv11 m_ssgSwEnv11g;

    // チップ全体へ掛かるピッチ側。オペレータは m_globalPitchRatio を
    // 参照しているので、ここを毎サンプル更新すれば全オペレータへ届く。
    SsgSwPEnv11 m_ssgSwPEnv11g;
    float m_globalPitchRatio = 1.0f;

    // チップ全体へ掛かる MODULATION。変調速度は搬送波との比なので、
    // 発音中のノートの位相増分を渡す。
    WtModulator m_wtMod;

    // WT PITCH MOD と同じ変調波形を、チップ全体の音量へ当てるもの。
    WtAmpModulator m_wtAmpMod;
    float m_noteFreq = 440.0f;

    // チップ全体のピッチ倍率を 1 サンプルぶん進める
    inline void updateGlobalPitchRatio(float notePhaseDelta) {
        float ratio = m_wtMod.process(notePhaseDelta) * m_ssgHwPEnv.process(1.0f);

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

    // 目標レートは 1 サンプルごとに要るので、変わったときだけ求めて持っておく
    double m_targetRate = 96000.0; // m_rateIndex = 1 (96kHz) に合わせた初期値
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_prevSample = 0.0f;
    float m_quantizeSteps = 0.0f;

    float m_modWheel = 0.0f;

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
};
