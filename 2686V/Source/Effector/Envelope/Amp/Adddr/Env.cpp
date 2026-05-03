#include "./Env.h"

AmpAdddrEnv::AmpAdddrEnv(float ar, float d1r, float d1l, float d2r, float rr, float tl)
{
	this->ar = ar;
	this->d1r = d1r;
	this->d1l = d1l;
	this->d2r = d2r;
	this->rr = rr;
	this->tl = tl;
}

void AmpAdddrEnv::setParameters(const AmpAdddrEnvParams& params)
{
	this->ar = params.ar;
	this->d1r = params.d1r;
	this->d1l = params.d1l;
	this->d2r = params.d2r;
	this->rr = params.rr;
	this->tl = params.tl;
}
