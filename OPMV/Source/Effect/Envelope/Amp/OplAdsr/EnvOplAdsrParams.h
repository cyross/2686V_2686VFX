#pragma once

#include "../../../KeyScale/Opl/KSOplParams.h"

struct OplAdsrParams
{
	int ar = 0;
	int dr = 0;
	int sl = 15;
	int rr = 0;
	int tl = 0;

	KSOplParams ksOPL;

	bool egType = false;
	bool sus = false;
	bool xof = false;
	bool kor = false;
	bool bypass = false;
};