#pragma once

#include <array>

class Opzx7Detune
{
	static const std::array<float, 16> dtScales;
	static const std::array<float, 4> dt2Scales;

	int detune;
	float realDetune;
	int detune2;
	float realDetune2;
	int multiple;
	float realMultiple;
public:
	void setParameters(int dt, int dt2, int mul);
	float noteOn(float baseFreq) const;
};
