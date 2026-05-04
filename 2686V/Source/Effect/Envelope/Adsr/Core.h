#pragma once

#include "./Params.h"

class AdsrAmpEnv {
public:
	void setParameters(const AdsrParams& params);

	float ar = 0.01f;
	float dr = 0.0f;
	float sl = 1.0f;
	float rr = 0.2f;
	bool bypass = false;
};
