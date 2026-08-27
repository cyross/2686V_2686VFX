#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <cmath>

#include "../../Core/Synth/SynthCore.h"
#include "../../Core/Synth/WtPlusWave.h"
#include "../../Generator/Fds/GenFdsModTable.h"
#include "../../Core/Synth/SynthParams.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "../../Generator/Fm/Fix/FmFix.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"

class WtPlusCore : public SynthCore
{
public:
    WtPlusCore();

    void prepare(double sampleRate) override;
	void setSampleRate(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    void noteOn(float freq, float velocity, int midiNote, bool isLegato = false) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    void setPitchBendRatio(float ratio) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;

    // 波形メモリはプロセッサが所有する。コアは参照だけを受け取る。
    void setWaveSlots(const WtPlusWaveSlots* slots) { m_waveSlots = slots; }
    void setCurveCore(CurveCore* p_curveCore);

    // ユニゾン・ハーモニー用
    // ユニゾン・ハーモニーは SynthCore::m_unison に集約
private:
    void updatePhaseDelta();

    double m_sampleRate = 44100.0;

    AmpAdsrEnv m_adsr;
    PitchAdsrEnv m_pitchAdsr;
    SsgSwEnv m_ssgSwEnv;
    Opzx7Detune m_detune;
    Opzx7LfoCore m_lfo;
    FixMode m_fixMode;
    SsgSwEnv11 m_ssgSwEnv11;
    SsgSwPEnv11 m_ssgSwPenv11;
    SsgHwEnv m_ssgHwEnv;

    float m_level = 1.0f;

    // Wave Data
    // 波形の実体はプロセッサが持つので、ここは参照だけを持つ。
    // 32 スロットぶんをコアごとにコピーすると重いうえ、
    // 読み込み時に全ボイスへ配り直す手間も増える。
    const WtPlusWaveSlots* m_waveSlots = nullptr;

    // 選択中のスロット番号。オートメーションで振れる。
    int m_slot = 0;

    float m_quantizeSteps = 15.0f;   // 4bit=15

    // 波形テーブルの読み出しを線形補間するか
    bool m_interpolate = true;

    // Rate / Lo-Fi
    int m_rateIndex = 5; // Default ID 6 (16kHz) -> Index 5
    double m_targetRate = 44100.0;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_currentFrequency = 440.0f;

    // Modulation
    bool m_modEnable = false;
    float m_modDepth = 0.0f;
    float m_modSpeed = 1.0f;
    int m_modShape = 0; // WtModShape を参照
    std::array<float, 32> m_modWave = { 0.0f }; // HuC6280 モード用の変調波形

    // FdsUser モード用。レジスタ値のテーブルと、それを積算した階段波。
    // 積算はテーブルが変わったときだけ行う。
    std::array<int, 32> m_modFdsTable = { 0 };
    std::array<float, 32> m_modFdsSteps = { 0.0f };
    float m_modPhase = 0.0f;

    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_currentLevel = 0.0f;
    float m_baseLevel = 0.0f;

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
};
