#pragma once

#include <array>

#include "./KSOpzParams.h"

class KSOpz {
	int m_ksr = 0;
	int m_ksl = 0;
	float m_depth = 0.0f;
	int m_shift = 3;

	// KSL=99の時の1オクターブあたりの最大減衰量
	// TX81Zは最大でかなり急激に減衰するため 24.0dB / oct 程度で設定
	static inline const float m_maxDbPerOct = 24.0f;
public:
	void setParameters(const KSOpzParams& params);
	int calcKeyScaleRate(const int noteNumber) const;
	float calcLevelScalingDb(const int noteNumber) const;
};
