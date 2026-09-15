#pragma once

#include <array>

#include "./DetuneOpnParams.h"

class OpnDetune
{
public:
	// 表は FmToOpzx7Tables.h の写しと突き合わせるテストでも読む
	static const std::array<float, 8> dtScales;
	static const std::array<float, 16> mulScales;
private:

	int detune;
	float realDetune;
	int multiple;
	float realMultiple;
public:
	void setParameters(const OpnDetuneParams& params);
	float noteOn(float baseFreq) const;
};
