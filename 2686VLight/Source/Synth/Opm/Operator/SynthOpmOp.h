#pragma once

#include "./SynthOpmOpParams.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Detune/Opm/DetuneOpm.h"
#include "../../../Generator/Fm/Fix/FmFix.h"
#include "../../../Effect/Lfo/Opm/LfoOpm.h"
#include "../../../Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddr.h"
#include "../../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"
#include "../../../Effect/Envelope/Pitch/SsgHw/EnvSsgHw.h"
#include "../../../Generator/WtMod/GenWtAmpModulator.h"
#include "../../../Effect/Feedback/Feedback.h"
#include "../../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"

class OpmOperator : public FmOperator
{
public:
	OpmOperator() : FmOperator(), m_detune() {
		Feedback::setFeedbackVector(fVector);
	}

	OpmOpParams m_params;

	void prepare(int opIndex, double sampleRate);
	void setParameters(const OpmOpParams& params, int feedback);
	void setSampleRate(double sampleRate) override;
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

	void getSample(float& output, float modulator, float feedbackModulator, const OpmLfoCore& hwLfo, float modWheel = 0.0f);

	float getFeedbackAverage() const {
		return (m_fb1 + m_fb2) * 0.5f;
	}

	// ユニゾン・ハーモニー用
	// ユニゾン時の位相オフセットを受け取る関数
	void setUnisonPhaseOffset(float offset) { m_unisonPhaseOffset = offset; }
	void setMonoMode(bool isMono) { m_isMonoMode = isMono; }
private:
	OpmDetune m_detune;
	FixMode m_fixMode;
	FmRgAdddr m_ampAdsr;
	PitchAdsrEnv m_pitchAdsr;
	SsgSwEnv m_ssgSwEnv;
	SsgSwEnv11 m_ssgSwEnv11;
	SsgSwPEnv11 m_ssgSwPenv11;

	// SSG HW PITCH ENV。オペレーターごとに 1 つ持つ。
	SsgHwPEnv m_ssgHwPEnv;

	// WT AMP MOD。オペレーターごとに 1 つ持つ。
	WtAmpModulator m_wtAmpMod;

	std::array<float, 8> fVector = { 0.0f };

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	// ユニゾン・ハーモニー用
	bool m_isMonoMode = false;
	float m_unisonPhaseOffset = 0.0f;
};
