#pragma once

#include <cmath>

struct OplLfoValues {
	float am = 0.0f;
	float pm = 0.0f;
};

class OplLfoCore {
	double m_sampleRate = 44100.0; // DAW Host Sample Rate

	double m_pmPhase = 0.0f;
	double m_amPhase = 0.0f;

	double m_pmPhaseDelta = 0.0f;
	double m_amPhaseDelta = 0.0f;

	float m_modWheel = 0.0f;
public:
	bool amEnable = false;
	bool pmEnable = false;

	float pms = 0.0f;
	float pmd = 0.0f;
	float ams = 0.0f;
	float amd = 0.0f;

	float depthDb = 0.0f;
	float depthCent = 0.0f;

	OplLfoValues value;

	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(bool pm, bool am, float pms, float pmd, float ams, float amd);
	void getSample();
	inline void updatePhaseDelta();
	void setModWheel(float newWheel);
};
