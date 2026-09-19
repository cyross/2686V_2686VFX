#pragma once

#include <array>

#include "./KSOpnParams.h"

class KSOpn {
	int m_ks = 0;
	int m_shift = 3;
public:
	void setParameters(const KSOpnParams& params);
	int calcKeyScaleRate(const int noteNumber) const;
	float calcLevelScalingDb(const int noteNumber) const;
};
