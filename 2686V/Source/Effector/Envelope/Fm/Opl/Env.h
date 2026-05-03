#pragma once

#include "./Params.h"

class OplEnv
{
	int ar;
	int dr;
	int sl;
	int rr;
	int tl;
public:
	OplEnv(int ar, int dr, int sl, int rr, int tl);
	void setParameters(const OplEnvParams& params);
};
