#pragma once

#include "../../../Core/Fm/FmOperator.h"
#include "../../../Core/Fm/FmOpParams.h"
#include "../../../Effect/Envelope/Pitch/Adsr/Core.h"
#include "../../../Generator/Noise/Lfsr/Core.h"
#include "../../../Generator/Fm/Fix/Core.h"
#include "../../../Effect/Detune/Opzx7/Core.h"
#include "../../../Effect/Lfo/Opzx7/Core.h"

class Opzx7Operator : public FmOperator
{
public:
	Opzx7Operator() : FmOperator() {}

	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const FmOpParams& params, float feedback) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void noteOff() override;
	void getSample(float& output, float modulator, Opzx7LfoCore &glLfo, float modWheel = 0.0f);
	float calcWaveform(double phase, int wave) override;
private:
	Opzx7Detune m_detune;
	LfsrNoiseGen m_noiseGen;
	FixMode m_fixMode;
	PitchAdsrEnv m_pitchAdsr;
	Opzx7LfoCore m_lfo;
	void updateIncrementsWithKeyScale() override;
};
