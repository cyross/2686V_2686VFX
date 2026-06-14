#pragma once

enum class KeyScaleMode {
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

	KeyScaleMode ksMode = KeyScaleMode::MA7;

	// MA7
	bool ksrMA7 = false;
	int kslMA7 = 0;

	// OPZ
	bool ksrOPZ = false;
	int kslOPZ = 0;

	// OPS
	int ksBp = 60;
	int ksLc = 0;
	int ksRc = 0;
	float ksLd = 0.0f;
	float ksRd = 0.0f;
	int ksRs = 0;

	bool sus = false;
	bool xof = false;
	bool kor = false;
	bool bypass = false;
};