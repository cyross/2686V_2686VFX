#pragma once

#include "./EnvSsgHwParams.h"

class SsgHwEnv {
public:
	SsgHwEnv();

	// HW Env Params
	bool m_useHwEnv = false;
	int m_envShape = 0;
	float m_envFreq = 1.0f;
	double m_hwEnvPhase = 0.0;
	float m_min = 0.0f;
	float m_max = 1.0f;

	float m_currentRate = 0.0f;
	double sampleRate = 44100.0; // DAW Host Sample Rate
	float currentLevel = 0.0f;

	void prepare(double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const SsgHwEnvParams& params);
	void noteOn();
	void noteOff();
	float process();
};
