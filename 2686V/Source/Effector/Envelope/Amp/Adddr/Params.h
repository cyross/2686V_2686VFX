#pragma once

class AmpAdddrEnvParams
{
public:
    AmpAdddrEnvParams(float ar = 0.03f, float d1r = 0.0f, float d1l = 0.0f, float d2r = 0.03f, float rr = 0.0f, float tl = 0.0f)
	{
		this->ar = ar;
		this->d1r = d1r;
		this->d1l = d1l;
		this->d2r = d2r;
		this->rr = rr;
		this->tl = tl;
	}

	float ar;
	float d1r;
	float d1l;
	float d2r;
	float rr;
	float tl;
};
