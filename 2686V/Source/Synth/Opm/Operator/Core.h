#pragma once

#include "./Params.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Detune/Opm/Core.h"
#include "../../../Generator/Fm/Fix/Core.h"
#include "../../../Effect/Lfo/opm/Core.h"

class OpmOperator : public FmOperator
{
public:
	OpmOperator() : FmOperator() {}

	OpmOpParams m_params;

	void setParameters(const OpmOpParams& params, float feedback);
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void getSample(float& output, float modulator, const OpmLfoCore& hwLfo, float modWheel = 0.0f);
	void updateIncrementsWithKeyScale() override;
private:
	OpmDetune m_detune;
	FixMode m_fixMode;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	void updateEnvelopeState() override;
};
