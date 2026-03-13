#pragma once

#include "../fm/FmOperator.h"

class OpnaOperator : public FmOperator
{
public:
	OpnaOperator() : FmOperator() {}

	void setParameters(const FmOpParams& params, float feedback) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void getSample(float& output, float modulator, float amLfoVal, float pmLfoVal, bool globalPm, bool globalAm, int globalPms, int globalAms, float globalPmd = -1.0f, float globalAmd = -1.0f, float modWheel = 0.0f) override;
};
