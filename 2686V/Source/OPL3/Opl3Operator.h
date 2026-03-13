#pragma once

#include "../fm/FmOperator.h"

class Opl3Operator : public FmOperator
{
public:
	Opl3Operator() : FmOperator() {}

	static const std::array<float, 4> dbPerOcts;

	void setParameters(const FmOpParams& params, float feedback) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void getSample(float& output, float modulator, float lfoAmp, float lfoPitch) override;
	float calcWaveform(double phase, int wave) override;
	void updateIncrementsWithKeyScale() override;
};
