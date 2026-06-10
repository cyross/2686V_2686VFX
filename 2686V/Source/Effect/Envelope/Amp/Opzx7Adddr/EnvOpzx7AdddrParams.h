#pragma once

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
	bool ksr = false;
	int ksl = 0;
	bool ksEn = false;
	bool sus = false;
	bool xof = false;
	bool kor = false;
	bool bypass = false;
};