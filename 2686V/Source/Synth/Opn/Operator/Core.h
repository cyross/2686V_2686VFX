#pragma once

#include "./Params.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Detune/Opn/Core.h"
#include "../../../Generator/Fm/Fix/Core.h"
#include "../../../Effect/Lfo/Opna/Core.h"
#include "../../../Effect/Lfo/N88/Core.h"
#include "../../../Effect/Envelope/Amp/FmRgAdssr/Core.h"
#include "../../../Effect/Envelope/Pitch/Adsr/Core.h"
#include "../../../Effect/Envelope/Amp/SsgSw/Core.h"

class OpnOperator : public FmOperator
{
public:
	OpnOperator() : FmOperator(), m_detune() {}

	OpnOpParams m_params;

	void prepare(int opIndex, double sampleRate);
	void setParameters(const OpnOpParams& params, float feedback);
	void setSampleRate(double sampleRate) override;
	void noteOn(float frequency, float velocity, int noteNumber, bool isLegato = false) override;
	void noteOff() override;
	bool isPlaying() const override { return m_ampAdsr.isPlaying() || m_ssgSwEnv.isPlaying(); }
	void getSample(float& output, float modulator, const N88LfoCore& n88Lfo, float modWheel = 0.0f);
	void setCurveCore(CurveCore* p_curveCore);

	// ユニゾン・ハーモニー用
	// ユニゾン時の位相オフセットを受け取る関数
	void setUnisonPhaseOffset(float offset) { m_unisonPhaseOffset = offset; }
	void setMonoMode(bool isMono) { m_isMonoMode = isMono; }
private:
	float maxAmDepthDb = 11.8f;
	float m_ams = 1.0f;
	OpnDetune m_detune;
	FixMode m_fixMode;
	OpnaLfoCore m_lfo;
	FmRgAdssr m_ampAdsr;
	PitchAdsrEnv m_pitchAdsr;
	SsgSwEnv m_ssgSwEnv;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	// ユニゾン・ハーモニー用
	bool m_isMonoMode = false;
	float m_unisonPhaseOffset = 0.0f;
};
