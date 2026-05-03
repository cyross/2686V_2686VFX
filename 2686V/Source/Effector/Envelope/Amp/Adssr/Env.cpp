#include "./Env.h"

AmpAdssrEnv::AmpAdssrEnv(float at, float dc, float ss, float sr, float rl, float tl)
{
	this->ar = at;
	this->dr = dc;
	this->sl = ss;
	this->sr = sr;
	this->rr = rl;
	this->tl = tl;
}

void AmpAdssrEnv::setParameters(const AmpAdssrEnvParams& params)
{
	this->ar = params.ar;
	this->dr = params.dr;
	this->sl = params.sl;
	this->sr = params.sr;
	this->rr = params.rr;
	this->tl = params.tl;
}
