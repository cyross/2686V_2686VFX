#pragma once

#include <cstdint>
#include <random>
#include <vector>

namespace GenPcmHelper
{
	static std::random_device pcmRd;
	static std::mt19937 pcmGen(pcmRd());
	static std::uniform_real_distribution<float> pcmDis(0.0f, 1.0f);

	void lowPassFilter(std::vector<int16_t>& buffer);
	float bitReduction(float input, int qIndex);
}
