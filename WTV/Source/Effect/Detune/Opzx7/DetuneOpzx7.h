#pragma once

#include <array>

#include "./DetuneOpzx7Params.h"

class Opzx7Detune
{
	static const std::array<float, 16> dtScales;
	static const std::array<float, 4> dt2Scales;
	std::array<float, 22> mulScales;
	// 参照するのは clamp(detune3, -4800, 4800) + 4800 の範囲だけなので 9601 個で足りる。
	// 中身はパラメータに依らない定数なので、実体ごとに持たず全体で 1 つを分け合う。
	// 以前は 1 実体につき 40KB を抱えていて、2686V では 1782 実体で約 71MB あった。
	static const std::array<float, 9601> dt3Scales;

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
	void setParameters(const Opzx7DetuneParams& params);
	float noteOn(float baseFreq) const;
};
