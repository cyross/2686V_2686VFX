#pragma once

#include "../../../Core/Fm/FmOperator.h"
#include "../../../Core/Fm/FmOpParams.h"
#include "../../../Effect/Envelope/Pitch/Adsr/Core.h"
#include "../../../Generator/Noise/Lfsr/Core.h"
#include "../../../Generator/Fm/Fix/Core.h"
#include "../../../Effect/Detune/Opm/Core.h"

class Opzx3Operator : public FmOperator
{
public:
	Opzx3Operator() : FmOperator() {}

	void setParameters(const FmOpParams& params, float feedback) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void getSample(float& output, float modulator, PitchAdsrEnv& pitchAdsr, float amLfoVal, float pmLfoVal, bool globalPm, bool globalAm, float globalPms, float globalAms, float globalPmd = -1.0f, float globalAmd = -1.0f, float modWheel = 0.0f);
	float calcWaveform(double phase, int wave) override;
private:
	OpmDetune m_detune;
	LfsrNoiseGen m_noiseGen;
	FixMode m_fixMode;
	void updateIncrementsWithKeyScale() override;
};
