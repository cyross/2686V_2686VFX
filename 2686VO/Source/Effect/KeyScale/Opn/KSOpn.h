#pragma once

#include <array>

#include "./KSOpnParams.h"

class KSOpn {
	std::array<int, 128> keyRates;
	std::array<float, 128> octaveDiffs;

	int m_ks;
	int m_shift;
public:
	KSOpn();

	void setParameters(const KSOpnParams& params);
	int calcKeyScaleRate(const int noteNumber) const;
	float calcLevelScalingDb(const int noteNumber) const;
};
