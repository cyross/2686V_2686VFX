#include "./Env.h"

OpmEnv::OpmEnv(int ar, int d1r, int d1l, int d2r, int rr, int tl)
{
	this->ar = ar;
	this->d1r = d1r;
	this->d1l = d1l;
	this->d2r = d2r;
	this->rr = rr;
	this->tl = tl;
}

void OpmEnv::setParameters(const OpmEnvParams& params)
{
	this->ar = params.ar;
	this->d1r = params.d1r;
	this->d1l = params.d1l;
	this->d2r = params.d2r;
	this->rr = params.rr;
	this->tl = params.tl;
}
