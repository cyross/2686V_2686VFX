#pragma once

#include "../../../Core/Fm/FmOperator.h"
#include "../../../Core/Fm/FmOpParams.h"
#include "../../../Effect/Detune/Opn/Core.h"
#include "../../../Generator/Fm/Fix/Core.h"
#include "../Params.h"

class OpnaOperator : public FmOperator
{
public:
	OpnaOperator() : FmOperator() {}

	void setParameters(const FmOpParams& params, float feedback) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void getSample(float& output, float modulator, float amLfoVal, float pmLfoVal, bool globalPm, bool globalAm, float globalPms, float globalAms, float globalPmd = -1.0f, float globalAmd = -1.0f, float modWheel = 0.0f) override;
private:
	OpnDetune m_detune;
	FixMode m_fixMode;
};
