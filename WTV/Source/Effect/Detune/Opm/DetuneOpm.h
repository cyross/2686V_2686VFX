#pragma once

#include <array>

#include "./DetuneOpmParams.h"

class OpmDetune
{
public:
	// 表は FmToOpzx7Tables.h の写しと突き合わせるテストでも読む
	static const std::array<float, 8> dtScales;
	static const std::array<float, 4> dt2Scales;
private:
	std::array<float, 17> mulScales;

	int detune;
	float realDetune;
	int detune2;
	float realDetune2;
	int multiple;
	float realMultiple;
public:
	OpmDetune();

	void setParameters(const OpmDetuneParams& params);
	float noteOn(float baseFreq) const;
};
