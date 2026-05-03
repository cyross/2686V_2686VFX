#pragma once

#include "./Params.h"

class AmpAdddrEnv
{
	float ar;
	float d1r;
	float d1l;
	float d2r;
	float rr;
	float tl;

public:
	AmpAdddrEnv() : AmpAdddrEnv(0.03f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f) {}
	AmpAdddrEnv(float ar, float d1r, float d1l, float d2r, float rr, float tl);
	void setParameters(const AmpAdddrEnvParams& params);
};
