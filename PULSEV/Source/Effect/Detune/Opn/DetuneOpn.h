#pragma once

#include <array>

#include "./DetuneOpnParams.h"

class OpnDetune
{
	static const std::array<float, 8> dtScales;
	static const std::array<float, 16> mulScales;

	int detune;
	float realDetune;
	int multiple;
	float realMultiple;
public:
	void setParameters(const OpnDetuneParams& params);
	float noteOn(float baseFreq) const;
};
