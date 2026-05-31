#pragma once

#include <array>

#include "./Params.h"
#include "../../../Effect/Lfo/Opl/Core.h"
#include "../../../Effect/Envelope/Amp/OplAdsr/Core.h"
#include "../../../Effect/Envelope/Pitch/Adsr/Core.h"
#include "../../../Effect/Envelope/Amp/SsgSw/Core.h"
#include "../../../Core/Fm/FmOperator.h"

class OplOperator : public FmOperator
{
public:
	OplOperator() : FmOperator() {}

	OplOpParams m_params;

	static const std::array<float, 4> dbPerOcts;

	void prepare(int opIndex, double sampleRate);
	void setParameters(const OplOpParams& params, float feedback);
	void setSampleRate(double sampleRate) override;
	void noteOn(float frequency, float velocity, int noteNumber) override;
	void noteOff() override;
	void getSample(float& output, float modulator);
	bool isPlaying() const override { return m_ampAdsr.isPlaying() || m_ssgSwEnv.isPlaying(); }
	void processLfo();
	float calcWaveform(double phase, int wave) override;
	void setModWheel(float modWheel){ this->m_lfo.setModWheel(modWheel); };
	void setCurveCore(CurveCore* p_curveCore);

	// ユニゾン・ハーモニー用
	// ユニゾン時の位相オフセットを受け取る関数
	void setUnisonPhaseOffset(float offset) { m_unisonPhaseOffset = offset; }
	void setMonoMode(bool isMono) { m_isMonoMode = isMono; }
private:
	OplLfoCore m_lfo;
	OplAdsr m_ampAdsr;
	PitchAdsrEnv m_pitchAdsr;
	SsgSwEnv m_ssgSwEnv;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	// ユニゾン・ハーモニー用
	bool m_isMonoMode = false;
	float m_unisonPhaseOffset = 0.0f;
};
