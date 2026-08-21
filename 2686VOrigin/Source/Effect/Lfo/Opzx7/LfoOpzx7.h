#pragma once

#include <cmath>
#include <array>
#include <functional>

#include "./LfoOpzx7Params.h"
#include "../../../Generator/Noise/Lfsr/GenNoiseLfsr.h"
#include "./LfoOpzx7Unit.h"

struct Opzx7LfoValues {
	float am = 0.0f;
	float pm = 0.0f;
};

class Opzx7LfoCore {
	double m_sampleRate = 44100.0; // DAW Host Sample Rate
public:
	Opzx7LfoCore();

	Opzx7LfoCoreUnit pm;
	Opzx7LfoCoreUnit am;

	Opzx7LfoValues value;

	void prepare(double sampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const LfoOpzx7Params& params);
	void noteOn();
	void getSample();
	inline void updatePhaseDelta();
};
