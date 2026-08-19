#pragma once

#include <array>

#include "./KSOpzParams.h"

class KSOpz {
	static const std::array<float, 8> m_dbPerOcts; // KSLの減衰カーブ定義

	std::array<int, 128> keyRates;
	std::array<float, 128> octaveDiffs;

	int m_ksr;
	int m_ksl;
	float m_depth;
	int m_shift;

	// KSL=99の時の1オクターブあたりの最大減衰量
	// TX81Zは最大でかなり急激に減衰するため 24.0dB / oct 程度で設定
	static inline const float m_maxDbPerOct = 24.0f;
public:
	KSOpz();

	void setParameters(const KSOpzParams& params);
	int calcKeyScaleRate(const int noteNumber) const;
	float calcLevelScalingDb(const int noteNumber) const;
};
