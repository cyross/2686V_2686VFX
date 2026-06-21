#pragma once

#include "../../../KeyScale/Ma7/KSMa7Params.h"
#include "../../../KeyScale/Opz/KSOpzParams.h"
#include "../../../KeyScale/Ops/KSOpsParams.h"

enum class Opzx7AdddrKeyScaleMode {
	MA7 = 0,
	OPZ = 1,
	OPS = 2
};

struct Opzx7RealAdssrParams
{
	float ar = 0.0f;
	float d1r = 0.0f;
	float d2r = 0.0f;
	float d1l = 0.0f;
	float rr = 0.0f;
	float tl = 0.0f;
};

struct Opzx7RgAdssrParams
{
	int ar = 0;
	int d1r = 0;
	int d2r = 0;
	int d1l = 0;
	int rr = 0;
	int tl = 0;
};

struct Opzx7AdddrParams
{
	bool rgEnable = false;
	Opzx7RealAdssrParams real;
	Opzx7RgAdssrParams rg;

	bool ksEn = false;

	Opzx7AdddrKeyScaleMode ksMode = Opzx7AdddrKeyScaleMode::MA7;
	KSMa7Params ksMA7;
	KSOpzParams ksOPZ;
	KSOpsParams ksOPS;

	bool sus = false;
	bool xof = false;
	bool kor = false;
	bool bypass = false;
};