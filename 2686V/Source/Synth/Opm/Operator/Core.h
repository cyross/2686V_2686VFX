#pragma once

#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Detune/Opm/Core.h"
#include "../../../Generator/Fm/Fix/Core.h"
#include "../../../Effect/Lfo/opm/Core.h"

class OpmOperator : public FmOperator
{
public:
	OpmOperator() : FmOperator() {}

	void setParameters(const FmOpParams& params, float feedback) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void getSample(float& output, float modulator, const OpmLfoCore& hwLfo, float modWheel = 0.0f);
private:
	OpmDetune m_detune;
	FixMode m_fixMode;
};
