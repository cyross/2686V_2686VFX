#pragma once

#include "./Params.h"
#include "../../../Effect/Lfo/Opl/Core.h"
#include "../../../Effect/Envelope/Amp/OplAdsr/Core.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Envelope/Pitch/Adsr/Core.h"
#include "../../../Effect/Envelope/Amp/SsgSw/Core.h"

class Opl3Operator : public FmOperator
{
public:
	Opl3Operator() : FmOperator() {}

	Opl3OpParams m_params;

	static const std::array<float, 4> dbPerOcts;

	void setParameters(const Opl3OpParams& params, float feedback);
	void updateTargetSampleRate(double newSampleRate);
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void noteOff() override;
	bool isPlaying() const override { return m_ampAdsr.isPlaying() || m_ssgSwEnv.isPlaying(); }
	void getSample(float& output, float modulator);
	void processLfo();
	float calcWaveform(double phase, int wave) override;
	void setModWheel(float modWheel) { this->m_lfo.setModWheel(modWheel); };
private:
	OplLfoCore m_lfo;
	OplAdsr m_ampAdsr;
	PitchAdsrEnv m_pitchAdsr;
	SsgSwEnv m_ssgSwEnv;

	bool m_zeroDecay = true;
	float m_sustain = 1.0f;  // SL (Sustain Level)
};
