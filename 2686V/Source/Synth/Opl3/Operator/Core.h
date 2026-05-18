#pragma once

#include "../../../Effect/Lfo/Opl/Core.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Core/Fm/FmOpParams.h"

class Opl3Operator : public FmOperator
{
public:
	Opl3Operator() : FmOperator() {}

	static const std::array<float, 4> dbPerOcts;

	void setParameters(const FmOpParams& params, float feedback) override;
	void setSampleRate(double sampleRate) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void getSample(float& output, float modulator);
	void processLfo();
	float calcWaveform(double phase, int wave) override;
	void updateIncrementsWithKeyScale() override;
	void setModWheel(float modWheel) { this->m_lfo.setModWheel(modWheel); };
private:
	OplLfoCore m_lfo;
};
