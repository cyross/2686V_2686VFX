#pragma once

#include "./Params.h"
#include "../../../Effect/Lfo/Opl/Core.h"
#include "../../../Effect/Envelope/Amp/OplAdsr/Core.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Envelope/Pitch/Adsr/Core.h"
#include "../../../Effect/Envelope/Amp/SsgSw/Core.h"
#include "../../../Effect/Detune/Opl/Core.h"

class Opl3Operator : public FmOperator
{
public:
	Opl3Operator() : FmOperator() {}

	Opl3OpParams m_params;

	static const std::array<float, 4> dbPerOcts;

	void prepare(int opIndex, double sampleRate);
	void setParameters(const Opl3OpParams& params, float feedback);
	void setSampleRate(double sampleRate) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void noteOff() override;
	bool isPlaying() const override { return m_ampAdsr.isPlaying() || m_ssgSwEnv.isPlaying(); }
	void getSample(float& output, float modulator);
	void processLfo();
	float calcWaveform(double phase, int wave) override;
	void setModWheel(float modWheel) { this->m_lfo.setModWheel(modWheel); };
	void setCurveCore(CurveCore* p_curveCore);

	// ユニゾン・ハーモニー用
	// ユニゾン時の位相オフセットを受け取る関数
	void setUnisonPhaseOffset(float offset) { m_unisonPhaseOffset = offset; }
	void setMonoMode(bool isMono) { m_isMonoMode = isMono; }
private:
	OplLfoCore m_lfo;
	OplAdsr m_ampAdsr;
	OplDetune m_detune;
	PitchAdsrEnv m_pitchAdsr;
	SsgSwEnv m_ssgSwEnv;

	bool m_zeroDecay = true;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	// ユニゾン・ハーモニー用
	bool m_isMonoMode = false;
	float m_unisonPhaseOffset = 0.0f;
};
