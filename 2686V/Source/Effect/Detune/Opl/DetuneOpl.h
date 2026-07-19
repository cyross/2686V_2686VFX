#pragma once

#include <array>

#include "./DetuneOplParams.h"

class OplDetune
{
	static const std::array<float, 16> mulScales;

	int multiple;
	float realMultiple;
public:
	void setParameters(const OplDetuneParams& params);
	float noteOn(float baseFreq) const;
};
