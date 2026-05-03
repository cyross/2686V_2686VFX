#pragma once

#include "./Params.h"

class AmpAdssrEnv
{
	float ar;
	float dr;
	float sl;
	float sr;
	float rr;
	float tl;

public:
	AmpAdssrEnv() : AmpAdssrEnv(0.03f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f) {}
	AmpAdssrEnv(float ar, float dr, float sl, float sr, float rr, float tl);
	void setParameters(const AmpAdssrEnvParams& params);
};
