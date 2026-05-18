#pragma once

#include <cmath>

#include "../../../Generator/Noise/Lfsr/Core.h"

struct N88LfoValues {
	float am = 0.0f;
	float pm = 0.0f;
};

class N88LfoCore {
	double m_sampleRate = 44100.0; // DAW Host Sample Rate

	float m_amFreq = 0.0f;
	float m_pmFreq = 0.0f;

	int m_sdParam = 0;
	float m_sd = 0.0f;

	int m_pmWaveIndex = 0;
	int m_amWaveIndex = 0;

	bool m_isOneshotPm = false;
	bool m_isOneshotAm = false;
	
	double m_pmPhase = 0.0f;
	double m_amPhase = 0.0f;

	double m_pmPhaseDelta = 0.0f;
	double m_amPhaseDelta = 0.0f;

	float m_sdCounter = 0.0f;
	uint32_t  m_pmCycleCount = 0;
	uint32_t  m_amCycleCount = 0;

	float m_amSmoothRate = 1.0f;

	float m_currentNoiseSample = 0.0f;

	LfsrNoiseGen m_noiseGen;

	using N88LfoCalculator = float(*)(double phase, float noise);

	static const std::array<N88LfoCalculator, 6> pmStrategies;
	static const std::array<N88LfoCalculator, 6> amStrategies;
	static const std::array<float, 8> freqs;
public:
	bool amEnable = false;
	bool pmEnable = false;

	float pms = 0.0f;
	float pmd = 0.0f;
	float ams = 0.0f;
	float amd = 0.0f;

	float amSmooth = 0.01f;

	float signDb = 0.0f;
	float depthDb = 0.0f;
	float depthNorm = 0.0f;

	N88LfoValues value;

	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(int syncDelay, bool pm, bool am, int pmFreqIndex, int amFreqIndex, int pmIndex, int amIndex, float pms, float pmd, float amd, float amSmoothRate);
	void noteOn();
	void getSample();
	inline void updatePhaseDelta();
};
