#pragma once

class OpmEnvParams
{
public:
    OpmEnvParams(int ar = 31, int dr = 0, int sl = 0, int sr = 0, int rr = 15, int tl = 0)
    {
		this->ar = ar;
		this->d1r = dr;
		this->d1l = sl;
		this->d2r = sr;
		this->rr = rr;
		this->tl = tl;
		this->enable = true;
    }

	int ar;
	int d1r;
	int d1l;
	int d2r;
	int rr;
	int tl;
	bool enable; // RG-EN
};
