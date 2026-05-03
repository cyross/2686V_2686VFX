#pragma once

#include "./Params.h"

class OpmEnv {
	int ar;
	int d1r;
	int d1l;
	int d2r;
	int rr;
	int tl;
public:
	OpmEnv(int ar, int d1r, int d1l, int d2r, int rr, int tl);
	void setParameters(const OpmEnvParams& params);
};
