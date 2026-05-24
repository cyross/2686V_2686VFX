#pragma once

#include "./Params.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Detune/Opn/Core.h"
#include "../../../Effect/Lfo/Opna/Core.h"
#include "../../../Generator/Fm/Fix/Core.h"
#include "../../../Effect/Lfo/N88/Core.h"
#include "../../../Effect/Envelope/Amp/FmRgAdssr/Core.h"
#include "../../../Effect/Envelope/Pitch/Adsr/Core.h"
#include "../../../Effect/Envelope/Amp/SsgSw/Core.h"
#include "../Params.h"

class OpnaOperator : public FmOperator
{
public:
	OpnaOperator() : FmOperator() {}

	OpnaOpParams m_params;

	void prepare(int opIndex, double sampleRate);
	void setSampleRate(double sampleRate) override;
	void setParameters(const OpnaOpParams& params, float feedback, float amSmoothRate);
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void noteOff() override;
	bool isPlaying() const override { return m_ampAdsr.isPlaying() || m_ssgSwEnv.isPlaying(); }
	void processLfo();
	void getSample(float& output, float modulator, const N88LfoCore& n88Lfo, float modWheel = 0.0f);
	void setCurveCore(CurveCore* p_curveCore);
private:
	OpnDetune m_detune;
	FixMode m_fixMode;
	OpnaLfoCore m_hwLfo;
	FmRgAdssr m_ampAdsr;
	PitchAdsrEnv m_pitchAdsr;
	SsgSwEnv m_ssgSwEnv;
	float maxAmDepthDb = 11.8f;
	float m_ams = 1.0f;
	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	using SsgWaveCalculator = float(*)(double p);
	static const std::array<SsgWaveCalculator, 16> ssgWaveStrategies;
};
