#pragma once

#include <array>

#include "./KSOplParams.h"

class KSOpl {
	static const std::array<float, 4> dbPerOcts;

	std::array<int, 128> keyRates;
	std::array<std::array<float, 4>, 128> kslAttenuations;

	bool m_ksr;
	int m_ksl;
public:
	KSOpl();

	void setParameters(const KSOplParams& params);
	int calcKeyScaleRate(const int noteNumber) const;
	// 他の KeyScale クラスは dB を返すが、このクラスだけは
	// テーブルにゲイン(倍率)を持っているのでゲインを返す。
	float calcLevelScalingGain(const int noteNumber) const;
};
