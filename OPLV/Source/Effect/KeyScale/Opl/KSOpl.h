#pragma once

#include <array>

#include "./KSOplParams.h"

class KSOpl {
	bool m_ksr = false;
	int m_ksl = 0;
public:
	void setParameters(const KSOplParams& params);
	int calcKeyScaleRate(const int noteNumber) const;
	// 他の KeyScale クラスは dB を返すが、このクラスだけは
	// テーブルにゲイン(倍率)を持っているのでゲインを返す。
	float calcLevelScalingGain(const int noteNumber) const;
};
