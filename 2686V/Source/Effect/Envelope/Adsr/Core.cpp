#include "./Core.h"

void AdsrAmpEnv::setParameters(const AdsrParams& params) {
	this->ar = params.ar;
	this->dr = params.dr;
	this->sl = params.sl;
	this->rr = params.rr;
	this->bypass = params.bypass;
}
