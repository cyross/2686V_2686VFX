#pragma once

#include <cmath>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Synth/SynthCore.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "../../Generator/Fm/Fix/FmFix.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "../../Generator/WtMod/GenWtModulator.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHw.h"

class BeepCore : public SynthCore
{
public:
    BeepCore() : SynthCore() {}

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
    void setCurveCore(CurveCore* p_curveCore);

    // ユニゾン・ハーモニー用
    // ユニゾン・ハーモニーは SynthCore::m_unison に集約
private:
    double m_sampleRate = 44100.0;
    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_baseFreq = 440.0f;
    float m_pitchBendRatio = 1.0f;
    // MODULATION (FDS / WonderSwan / HuC6280)
    WtModulator m_wtMod;

    float m_modWheel = 0.0f;

    float m_currentLevel = 0.0f;
    float m_baseLevel = 0.0f;
    float m_targetLevel = 0.0f;

    // Params
    float m_level = 1.0f;

    AmpAdsrEnv m_adsr;
    FixMode m_fixMode;
    PitchAdsrEnv m_pitchAdsr;
    SsgSwEnv m_ssgSwEnv;
    Opzx7Detune m_detune;
    Opzx7LfoCore m_lfo;
    SsgSwEnv11 m_ssgSwEnv11;
    SsgSwPEnv11 m_ssgSwPenv11;
    SsgHwEnv m_ssgHwEnv;

    // 音量側と同じ形をピッチへ当てるもの。両方を同時に掛けられる。
    SsgHwPEnv m_ssgHwPEnv;

    bool m_antiAlias = false;

    // タイマの基準クロック(Hz)。0.0 なら分周せず連続値のまま鳴らす
    double m_timerClock = 0.0;

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
};
