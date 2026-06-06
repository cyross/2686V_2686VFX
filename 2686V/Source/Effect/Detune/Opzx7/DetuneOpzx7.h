#pragma once

#include <array>

class Opzx7Detune
{
	static const std::array<float, 16> dtScales;
	static const std::array<float, 4> dt2Scales;
	std::array<float, 22> mulScales;
	std::array<float, 10000> dt3Scales{};

	int detune;
	float realDetune;
	int detune2;
	float realDetune2;
	int detune3;
	float realDetune3;
	int multiple;
	float realMultiple;
public:
	Opzx7Detune();
	void setParameters(int dt, int dt2, int dt3, int mul, float mulRatio);
	float noteOn(float baseFreq) const;
};
