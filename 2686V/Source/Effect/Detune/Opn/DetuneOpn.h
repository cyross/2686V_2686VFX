#pragma once

#include <array>

class OpnDetune
{
	static const std::array<float, 8> dtScales;
	static const std::array<float, 16> mulScales;

	int detune;
	float realDetune;
	int multiple;
	float realMultiple;
public:
	void setParameters(int dt, int mul);
	float noteOn(float baseFreq) const;
};
