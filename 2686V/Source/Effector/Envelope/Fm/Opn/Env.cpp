#include "./Env.h"

OpnEnv::OpnEnv(int ar, int dr, int sl, int sr, int rr, int tl)
{
	this->ar = ar;
	this->dr = dr;
	this->sl = sl;
	this->sr = sr;
	this->rr = rr;
	this->tl = tl;
}

void OpnEnv::setParameters(const OpnEnvParams& params)
{
	this->ar = params.ar;
	this->dr = params.dr;
	this->sl = params.sl;
	this->sr = params.sr;
	this->rr = params.rr;
	this->tl = params.tl;
}
