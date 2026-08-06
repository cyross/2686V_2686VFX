#pragma once

#include <array>

#include "./KSMa7Params.h"

class KSMa7 {
	static const std::array<float, 4> m_dbPerOcts; // KSLの減衰カーブ定義

	std::array<int, 128> keyRates;
	std::array<std::array<float, 4>, 128> totalDbs;

	bool m_ksr;
	int m_ksl;
public:
	KSMa7();

	void setParameters(const KSMa7Params& params);
	int calcKeyScaleRate(const int noteNumber) const;
	float calcLevelScalingDb(const int noteNumber) const;
};
