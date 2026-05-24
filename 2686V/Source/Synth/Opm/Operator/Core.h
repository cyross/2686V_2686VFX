#pragma once

#include "./Params.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Detune/Opm/Core.h"
#include "../../../Generator/Fm/Fix/Core.h"
#include "../../../Effect/Lfo/opm/Core.h"
#include "../../../Effect/Envelope/Amp/FmRgAdddr/Core.h"
#include "../../../Effect/Envelope/Pitch/Adsr/Core.h"
#include "../../../Effect/Envelope/Amp/SsgSw/Core.h"

class OpmOperator : public FmOperator
{
public:
	OpmOperator() : FmOperator() {}

	OpmOpParams m_params;

	void prepare(int opIndex, double sampleRate);
	void setParameters(const OpmOpParams& params, float feedback);
	void setSampleRate(double sampleRate) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void noteOff() override;
	bool isPlaying() const override { return m_ampAdsr.isPlaying() || m_ssgSwEnv.isPlaying(); }
	void getSample(float& output, float modulator, const OpmLfoCore& hwLfo, float modWheel = 0.0f);
	void setCurveCore(CurveCore* p_curveCore);
private:
	OpmDetune m_detune;
	FixMode m_fixMode;
	FmRgAdddr m_ampAdsr;
	PitchAdsrEnv m_pitchAdsr;
	SsgSwEnv m_ssgSwEnv;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)
};
