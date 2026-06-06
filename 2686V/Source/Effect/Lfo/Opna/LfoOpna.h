#pragma once

#include <cmath>
#include <array>
#include <functional>

#include "../../../Generator/Noise/Lfsr/GenNoiseLfsr.h"

struct OpnaLfoValues {
	float am = 0.0f;
	float pm = 0.0f;
};

class OpnaLfoCore {
	std::array<std::function<void()>, 3> m_noteOnFunctions;

	double m_sampleRate = 44100.0; // DAW Host Sample Rate

	float m_amFreq = 0.0f;
	float m_pmFreq = 0.0f;

	int m_sdParam = 0;
	int m_sdIndex = 0;
	float m_sd = 0.0f;

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

	using OpnaLfoCalculator = float(*)(double phase, float noise);

	// OPZ7/PG-LFO波形の計算アルゴリズム配列
	static const std::array<OpnaLfoCalculator, 5> lfoPmStrategies;

	// OPZ7/EG-LFO波形の計算アルゴリズム配列
	static const std::array<OpnaLfoCalculator, 5> lfoAmStrategies;

	static const std::array<float, 8> freqs;
	static const std::array<float, 8> pmsDepths;
	static const std::array<float, 4> amsDepths;
public:
	OpnaLfoCore();

	bool amEnable = false;
	bool pmEnable = false;

	float pms = 0.0f;
	float pmd = 0.0f;
	float ams = 0.0f;
	float amd = 0.0f;

	float amSmooth = 0.01f;

	float depthDb = 0.0f;
	float depthCent = 0.0f;

	OpnaLfoValues value;

	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(int syncDelay, bool pm, bool am, int pmFreqIndex, int amFreqIndex, float pms, float ams, float amSmoothRate);
	void noteOn();
	void getSample();
	inline void updatePhaseDelta();
};
