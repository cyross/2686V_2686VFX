#pragma once

class AmpAdsrEnvParams
{
public:
    AmpAdsrEnvParams(float ar = 0.03f, float dr = 0.0f, float sl = 0.0f, float rr = 0.0f, float tl = 0.0f)
	{
		this->ar = ar;
		this->dr = dr;
		this->sl = sl;
		this->rr = rr;
		this->tl = tl;
	}

	float ar;
	float dr;
	float sl;
	float rr;
	float tl;
};
