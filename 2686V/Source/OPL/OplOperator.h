#pragma once

#include "../fm/FmOperator.h"

class OplOperator : public FmOperator
{
public:
	OplOperator() : FmOperator() {}

	void setParameters(const FmOpParams& params, float feedback) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void noteOff();
	void getSample(float& output, float modulator, float lfoAmp, float lfoPitch) override;
	float calcWaveform(double phase, int wave) override;
};
