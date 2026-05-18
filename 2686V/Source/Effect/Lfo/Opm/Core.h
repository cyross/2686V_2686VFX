#pragma once

#include <cmath>

#include "../../../Generator/Noise/Lfsr/Core.h"

struct OpmLfoValues {
	float am = 0.0f;
	float pm = 0.0f;
};

class OpmLfoCore {
	double m_sampleRate = 44100.0; // DAW Host Sample Rate

	float m_amFreq = 0.0f;
	float m_pmFreq = 0.0f;

	int m_sdParam = 0;
	float m_sd = 0.0f;

	int m_pmWaveIndex = 0;
	int m_amWaveIndex = 0;

	int m_pmsIndex = 0;
	int m_amsIndex = 0;

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

	using OpmLfoCalculator = float(*)(double phase, float noise);

	// OPZ7/PG-LFO波形の計算アルゴリズム配列
	static const std::array<OpmLfoCalculator, 4> lfoPmStrategies;

	// OPZ7/EG-LFO波形の計算アルゴリズム配列
	static const std::array<OpmLfoCalculator, 4> lfoAmStrategies;

	static const std::array<float, 4> amsDepths;

	static const std::array<float, 8> pmsDepths;
public:
	bool amEnable = false;
	bool pmEnable = false;

	float pms = 0.0f;
	float pmd = 0.0f;
	float ams = 0.0f;
	float amd = 0.0f;

	float amSmooth = 0.01f;

	float depthDb = 0.0f;
	float depthCent = 0.0f;

	OpmLfoValues value;

	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(int syncDelay, bool pm, bool am, float pmFreq, float amFreq, int pgIndex, int egIndex, int pmsIndex, int pmd, int amsIndex, int amd, float amSmoothRate);
	void noteOn();
	void getSample();
	inline void updatePhaseDelta();
};
