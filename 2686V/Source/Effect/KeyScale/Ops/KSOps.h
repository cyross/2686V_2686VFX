#pragma once

#include <array>

#include "./KSOpsParams.h"

class KSOps {
	std::array<int, 128> keyRates;
	std::array<float, 256> xs;
	std::array<float, 256> xpows;

	int m_ksBp = 60;
	int m_ksLc = 0;
	int m_ksRc = 0;
	float m_ksLd = 0.0f;
	float m_ksRd = 0.0f;
	int m_ksRs = 0;
public:
	KSOps();

	void setParameters(const KSOpsParams& params);
	int calcKeyScaleRate(const int noteNumber) const;
	float calcLevelScalingDb(const int noteNumber) const;
};
