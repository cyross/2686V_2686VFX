#pragma once

#include <array>

class Opzx7Detune
{
	static const std::array<float, 16> dtScales;
	static const std::array<float, 4> dt2Scales;
	std::array<float, 22> mulScales;

	int detune;
	float realDetune;
	int detune2;
	float realDetune2;
	int multiple;
	float realMultiple;
public:
	Opzx7Detune();
	void setParameters(int dt, int dt2, int mul, float mulRatio);
	float noteOn(float baseFreq) const;
};
