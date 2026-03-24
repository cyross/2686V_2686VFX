#pragma once

#include "../fm/FmOperator.h"
#include "../fm/FmOpParams.h"

class Opzx3Operator : public FmOperator
{
	float m_currentNoiseSample = 0.0f;
	float m_targetNoiseFreq = 12000.0f;
	unsigned int m_lfsr = 0x1FFFF;
public:
	Opzx3Operator() : FmOperator() {}

	void setParameters(const FmOpParams& params, float feedback) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void getSample(float& output, float modulator, float amLfoVal, float pmLfoVal, bool globalPm, bool globalAm, float globalPms, float globalAms, float globalPmd = -1.0f, float globalAmd = -1.0f, float modWheel = 0.0f) override;
	float calcWaveform(double phase, int wave) override;
};
