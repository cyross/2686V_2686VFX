#pragma once

#include <array>

#include "./Params.h"
#include "../../../Effect/Lfo/Opl/Core.h"
#include "../../../Core/Fm/FmOperator.h"

class OplOperator : public FmOperator
{
public:
	OplOperator() : FmOperator() {}

	OplOpParams m_params;

	static const std::array<float, 4> dbPerOcts;

	void setParameters(const OplOpParams& params, float feedback);
	void setSampleRate(double sampleRate) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void noteOff();
	void getSample(float& output, float modulator);
	void processLfo();
	float calcWaveform(double phase, int wave) override;
	void updateIncrementsWithKeyScale() override;
	void setModWheel(float modWheel){ this->m_lfo.setModWheel(modWheel); };
private:
	OplLfoCore m_lfo;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	void updateEnvelopeState() override;
};
