#pragma once

#include "./Params.h"

class OpnEnv {
	int ar;
	int dr;
	int sl;
	int sr;
	int rr;
	int tl;
public:
	OpnEnv(int ar, int dr, int sl, int sr, int rr, int tl);
	void setParameters(const OpnEnvParams& params);
};
