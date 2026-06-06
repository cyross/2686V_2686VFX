#pragma once

#include <array>

class OplDetune
{
	static const std::array<float, 16> mulScales;

	int multiple;
	float realMultiple;
public:
	void setParameters(int mul);
	float noteOn(float baseFreq) const;
};
