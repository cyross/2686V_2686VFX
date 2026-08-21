#pragma once

#include <cmath>
#include <array>
#include <functional>

#include "../../../Generator/Noise/Lfsr/GenNoiseLfsr.h"

class Opzx7LfoCoreUnit {
	double m_sampleRate = 44100.0; // DAW Host Sample Rate

	float m_freq = 0.0f;

	int m_sdParam = 0;
	int m_sdIndex = 0;
	float m_sd = 0.0f;

	int m_waveIndex = 0;

	bool m_isOneshot = false;

	double m_phase = 0.0f;

	double m_phaseDelta = 0.0f;

	float m_sdCounter = 0.0f;
	uint32_t m_sdCycleCount = 0;

	float m_currentNoiseSample = 0.0f;

	LfsrNoiseGen m_noiseGen;

	float m_smooth = 0.0f;
	float m_smoothRate = 0.0f;
public:
	Opzx7LfoCoreUnit();

	bool enable = false;

	float ms = 0.0f;
	float md = 0.0f;

	float depthDb = 0.0f;
	float depthCent = 0.0f;

	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(int syncDelay, bool enable, float freq, int index, float ms, float md, float smoothRate);
	void noteOn();
	float getSample();
	float getSamplePm();
	float getSampleAm();
	inline void updatePhaseDelta();
};
