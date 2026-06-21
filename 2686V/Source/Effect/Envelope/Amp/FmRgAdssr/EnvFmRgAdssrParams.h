#pragma once

#include "../../../KeyScale/Opn/KSOpnParams.h"

struct FmRgAdssrParams
{
	int ar = 0;
	int dr = 0;
	int sr = 0;
	int sl = 15;
	int rr = 0;
	int tl = 0;

	KSOpnParams ksOPN;

	bool xof = false;
	bool kor = false;
	bool bypass = false;
};