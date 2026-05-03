#include "./Env.h"

OplEnv::OplEnv(int ar, int dr, int sl, int rr, int tl)
{
	this->ar = ar;
	this->dr = dr;
	this->sl = sl;
	this->rr = rr;
	this->tl = tl;
}

void OplEnv::setParameters(const OplEnvParams& params)
{
	this->ar = params.ar;
	this->dr = params.dr;
	this->sl = params.sl;
	this->rr = params.rr;
	this->tl = params.tl;
}
