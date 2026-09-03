#pragma once

#include <random>
#include <array>
#include <cmath>
#include <algorithm>

#include "../../Core/Fm/FmCore.h"
#include "../../Generator/Noise/Lfsr/GenNoiseLfsr.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"
#include "../../Generator/WtMod/GenWtModulator.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"

#include "./Operator/SynthOpzx7Op.h"

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
    void setSampleRate(double sampleRate) override;
    void setParameters(const SynthParams& params);
    void noteOn(float freq, float velocity, int midiNote, bool isLegato = false) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    float getSample() override;
    void setPcmBuffer(int opIndex, std::vector<float>* pcmData);
    void setWtBuffer(int opIndex, std::vector<float>* wtData);
    void setWt2Buffer(int opIndex, std::vector<float>* wtData);
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
    void setCurveCore(CurveCore* p_curveCore);
    void clearPcmBuffer(int opIndex);
    void clearWtBuffer(int opIndex);
    void clearWt2Buffer(int opIndex);

    // ユニゾン・ハーモニー用
    // ユニゾン・ハーモニーは SynthCore::m_unison に集約

    struct AlgRouting {
        std::array<float, Opzx7PrValue::ops> out;                // 最終出力へのミックス割合
        std::array<std::array<float, Opzx7PrValue::ops>, Opzx7PrValue::ops> mod; // mod[dest][src]: srcからdestへの通常の変調割合
        std::array<std::array<float, Opzx7PrValue::ops>, Opzx7PrValue::ops> fbMod; // fbMod[dest][src]: srcからdestへのフィードバック変調割合
    };

    struct ModConnection {
        int srcOp;
        float amount;
        bool isForward;
    };

    struct OpRoutingConfig {
        std::array<ModConnection, Opzx7PrValue::ops> mods;
        int modCount = 0;

        std::array<ModConnection, Opzx7PrValue::ops> fbMods;
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
    static const std::array<AlgRouting, Opzx7PrValue::algorithms> routings;
    std::array<OpRoutingConfig, Opzx7PrValue::ops> m_activeRoutings;
private:
    std::array<Opzx7Operator, Opzx7PrValue::ops> m_operators;
    std::array<bool, Opzx7PrValue::ops> m_opMask{ false };
    std::array<float, Opzx7PrValue::ops> m_history1 = { 0.0f };
    std::array<float, Opzx7PrValue::ops> m_history2 = { 0.0f };

    Opzx7LfoCore m_lfo;

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
    int m_cachedAlgMode = -1;
    void updateRoutingCache();
    void applyRoutingToCache(const AlgRouting& r);

    double m_hostSampleRate = 44100.0;
    int m_algorithm = 0;
    int m_algorithmCodeBase = 0;
    int m_algorithmCodeShift = 4; // x16
    AlgMatrixParams m_algMatrix;

    float m_level = 1.0f;

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

    int m_panpot = 0;
    bool m_panpot_enable = false;
    float m_panpot_l_rate = 1.0f;
    float m_panpot_r_rate = 1.0f;

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
};
