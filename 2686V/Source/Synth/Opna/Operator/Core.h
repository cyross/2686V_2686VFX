#pragma once

#include "../../../Core/Fm/FmOperator.h"
#include "../../../Core/Fm/FmOpParams.h"
#include "../../../Effect/Detune/Opn/Core.h"
#include "../../../Effect/Lfo/Opna/Core.h"
#include "../../../Generator/Fm/Fix/Core.h"
#include "../../../Effect/Lfo/N88/Core.h"
#include "../Params.h"

class OpnaOperator : public FmOperator
{
public:
	OpnaOperator() : FmOperator() {}

	void prepare(double sampleRate);
	void setSampleRate(double sampleRate) override;
	void setParameters(const FmOpParams& params, float feedback, float amSmoothRate);
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void processLfo();
	void getSample(float& output, float modulator, const N88LfoCore& n88Lfo, float modWheel = 0.0f);
private:
	OpnDetune m_detune;
	FixMode m_fixMode;
	OpnaLfoCore m_hwLfo;
	float maxAmDepthDb = 11.8f;
	float m_ams = 1.0f;
};
