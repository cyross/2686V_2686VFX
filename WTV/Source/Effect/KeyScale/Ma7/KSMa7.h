#pragma once

#include <array>

#include "./KSMa7Params.h"

class KSMa7 {
	bool m_ksr = false;
	int m_ksl = 0;
public:
	void setParameters(const KSMa7Params& params);
	int calcKeyScaleRate(const int noteNumber) const;
	float calcLevelScalingDb(const int noteNumber) const;
};
