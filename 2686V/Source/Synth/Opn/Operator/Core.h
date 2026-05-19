#pragma once

#include "./Params.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Detune/Opn/Core.h"
#include "../../../Generator/Fm/Fix/Core.h"
#include "../../../Effect/Lfo/Opna/Core.h"
#include "../../../Effect/Lfo/N88/Core.h"

class OpnOperator : public FmOperator
{
public:
	OpnOperator() : FmOperator() {}

	OpnOpParams m_params;

	void setParameters(const OpnOpParams& params, float feedback);
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void getSample(float& output, float modulator, const N88LfoCore& n88Lfo, float modWheel = 0.0f);
	void updateIncrementsWithKeyScale() override;
private:
	float maxAmDepthDb = 11.8f;
	float m_ams = 1.0f;
	OpnDetune m_detune;
	FixMode m_fixMode;
	OpnaLfoCore m_lfo;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	void updateEnvelopeState() override;
};
