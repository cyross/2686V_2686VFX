#pragma once

#include "./SynthOpnaOpParams.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Detune/Opn/DetuneOpn.h"
#include "../../../Effect/Lfo/Opna/LfoOpna.h"
#include "../../../Generator/Fm/Fix/FmFix.h"
#include "../../../Effect/Lfo/N88/LfoN88.h"
#include "../../../Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssr.h"
#include "../../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"
#include "../../../Effect/Envelope/Pitch/SsgHw/EnvSsgHw.h"
#include "../../../Generator/WtMod/GenWtAmpModulator.h"
#include "../../../Effect/Feedback/Feedback.h"
#include "../SynthOpnaParams.h"

class OpnaOperator : public FmOperator
{
public:
	OpnaOperator() : FmOperator(), m_detune() {
		Feedback::setFeedbackVector(fVector);
	}

	OpnaOpParams m_params;

	void prepare(int opIndex, double sampleRate);
	void setSampleRate(double sampleRate) override;
	void setParameters(const OpnaOpParams& params, int feedback);
	void noteOn(float frequency, float velocity, int noteNumber, bool isLegato = false) override;
	void noteOff() override;

	// 全アンプエンベロープがバイパスされているか
	bool isAllAmpBypassed() const {
		return m_ampAdsr.isBypass() &&
			(!m_params.ssgEnvEnable || m_ssgSwEnv.isBypass()) &&
			(!m_params.ssgEnv11Enable || m_ssgSwEnv11.isBypass());
	}

	// アンプエンベロープのどれかが現在鳴っているか
	bool isPlaying() const {
		return m_ampAdsr.isPlaying() ||
			(m_params.ssgEnvEnable && m_ssgSwEnv.isPlaying()) ||
			(m_params.ssgEnv11Enable && m_ssgSwEnv11.isPlaying());
	}

	void processLfo();
	void getSample(float& output, float modulator, float feedbackModulator, const N88LfoCore& n88Lfo, float modWheel = 0.0f);
	void setCurveCore(CurveCore* p_curveCore);

	float getFeedbackAverage() const {
		return (m_fb1 + m_fb2) * 0.5f;
	}

	// ユニゾン・ハーモニー用
	// ユニゾン時の位相オフセットを受け取る関数
	void setUnisonPhaseOffset(float offset) { m_unisonPhaseOffset = offset; }
	void setMonoMode(bool isMono) { m_isMonoMode = isMono; }
private:
	OpnDetune m_detune;
	FixMode m_fixMode;
	OpnaLfoCore m_hwLfo;
	FmRgAdssr m_ampAdsr;
	PitchAdsrEnv m_pitchAdsr;
	SsgSwEnv m_ssgSwEnv;
	SsgSwEnv11 m_ssgSwEnv11;
	SsgSwPEnv11 m_ssgSwPenv11;

	// SSG HW PITCH ENV。オペレーターごとに 1 つ持つ。
	SsgHwPEnv m_ssgHwPEnv;

	// WT AMP MOD。オペレーターごとに 1 つ持つ。
	WtAmpModulator m_wtAmpMod;

	float maxAmDepthDb = Global::Lfo::maxAmDepthDb;
	float m_ams = 1.0f;
	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	std::array<float, 8> fVector = { 0.0f };

	using SsgWaveCalculator = float(*)(double p);
	static const std::array<SsgWaveCalculator, 16> ssgWaveStrategies;

	// ユニゾン・ハーモニー用
	bool m_isMonoMode = false;
	float m_unisonPhaseOffset = 0.0f;
};
