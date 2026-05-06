#pragma once

#include <array>

class OpmDetune
{
	static const std::array<float, 8> dtScales;
	static const std::array<float, 4> dt2Scales;

	int detune;
	int detune2;
public:
	void setParameters(int dt, int dt2);
	float noteOn(float baseFreq, int multiple) const;
};
