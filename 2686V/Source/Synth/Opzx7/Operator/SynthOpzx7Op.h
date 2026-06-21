#pragma once

#include "./SynthOpzx7OpParams.h"
#include "../../../Core/Fm/FmOperator.h"
#include "../../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../../Generator/Noise/Lfsr/GenNoiseLfsr.h"
#include "../../../Generator/Fm/Fix/FmFix.h"
#include "../../../Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "../../../Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "../../../Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7Adddr.h"
#include "../../../Effect/Feedback/Feedback.h"
#include "../../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"

class Opzx7Operator : public FmOperator
{
public:
	Opzx7Operator() : FmOperator(), m_detune() {
		Feedback::setFeedbackVector(fVector, 5.0f);
	}

	Opzx7OpParams m_params;

	void prepare(int opIndex, double sampleRate);
	void setSampleRate(double sampleRate) override;
	void setParameters(const Opzx7OpParams& params, int feedback);
	void noteOn(float frequency, float velocity, int noteNumber, bool isLegato = false) override;
	void noteOff() override;
	bool isPlaying() const override { return m_ampAdsr.isPlaying() || m_ssgSwEnv.isPlaying(); }
	void getSample(float& output, float modulator, float feedbackModulator, Opzx7LfoCore &glLfo, float modWheel = 0.0f);
	float calcWaveform(double phase, int wave) override;
	void setCurveCore(CurveCore* p_curveCore);
	// PCMデータ用
	void setPcmBuffer(std::vector<float>* pcmData) { m_pcmBuffer = pcmData; }
	void clearPcmBuffer() { m_pcmBuffer = nullptr; }
	// 波形メモリ用
	void setWtBuffer(std::vector<float>* wtData) { m_wtBuffer = wtData; }
	void setWt2Buffer(std::vector<float>* wtData) { m_wt2Buffer = wtData; }
	void clearWtBuffer() { m_wtBuffer = nullptr; }
	void clearWt2Buffer() { m_wt2Buffer = nullptr; }

	// ユニゾン・ハーモニー用
	// ユニゾン時の位相オフセットを受け取る関数
	void setUnisonPhaseOffset(float offset) { m_unisonPhaseOffset = offset; }
	void setMonoMode(bool isMono) { m_isMonoMode = isMono; }
private:
	Opzx7Detune m_detune;
	LfsrNoiseGen m_noiseGen;
	FixMode m_fixMode;
	PitchAdsrEnv m_pitchAdsr;
	Opzx7LfoCore m_lfo;
	Opzx7Adddr m_ampAdsr;
	SsgSwEnv m_ssgSwEnv;

	std::array<float, 8> fVector = { 0.0f };

	// OPZX7 の外部 PCM データ用
	std::vector<float>* m_pcmBuffer = nullptr;
	// OPZX7 の波形データ用
	std::vector<float>* m_wtBuffer = nullptr;
	std::vector<float>* m_wt2Buffer = nullptr;

	bool m_zeroDecay = false;
	float m_sustain = 1.0f;  // SL (Sustain Level)

	// ユニゾン・ハーモニー用
	bool m_isMonoMode = false;
	float m_unisonPhaseOffset = 0.0f;
};
