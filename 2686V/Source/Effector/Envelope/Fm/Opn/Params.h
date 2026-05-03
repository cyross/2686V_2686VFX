#pragma once

class OpnEnvParams
{
public:
    OpnEnvParams(int ar = 31, int dr = 0, int sl = 0, int sr = 0, int rr = 15, int tl = 0)
    {
		this->ar = ar;
		this->dr = dr;
		this->sl = sl;
		this->sr = sr;
		this->rr = rr;
		this->tl = tl;
		this->enable = true;
    }

	int ar;
	int dr;
	int sl;
	int sr;
	int rr;
	int tl;
	bool enable; // RG-EN
};
