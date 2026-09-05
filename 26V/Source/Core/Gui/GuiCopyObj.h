#pragma once

#include <JuceHeader.h>
#include <array>

#include "../Const/ConstGlobal.h"

struct CopyBase {
	float level;
};

struct CopyFmBase {
	float level;
	int algorithm;
	int feedback;
};

struct CopyKSOpn {
	int ks;
};

struct CopyEnvAmpAdsr {
	float stl;
	float ar;
	float dr;
	float sl;
	float rr;

	bool kor;
	bool bypass;
};

struct CopyEnvRgAdssr {
	int ar;
	int dr;
	int sr;
	int sl;
	int rr;
	int tl;

	int ks;

	bool xof;
	bool kor;
	bool bypass;
};

struct CopyEnvSsgHw {
	bool enable;
	int shape;
	float period;
	float min;
	float max;
	bool smooth;
};

// SSG HW PITCH ENV。min / max はセント値なので整数で持つ。
struct CopyPEnvSsgHw {
	bool enable;
	int shape;
	float period;
	int min;
	int max;
	bool smooth;
};

struct CopyEnvSsgSw {
	bool flag;

	int steps;
	bool loop;
	int loopTo;
	int loopCount;
	float stl;
	std::array<float, 6> r = { 0.0f };
	std::array<float, 6> l = { 0.0f };
};

struct CopyEnvSsgSw11 {
	bool flag;

	int steps;
	bool loop;
	int loopTo;
	int loopCount;
	float stl;
	std::array<float, 11> r = { 0.0f };
	std::array<float, 11> l = { 0.0f };
};

struct CopyPEnvSsgSw11 {
	bool flag;

	int steps;
	bool loop;
	int loopTo;
	int loopCount;
	int stl;
	std::array<float, 11> r = { 0.0f };
	std::array<int, 11> l = { 0 };
};

struct CopyEnvPitchAdsr {
	bool flag;

	float ar;
	float dr;
	float sl;
	float rr;
	int stl;
	int atl;
	int ssl;
	int rll;
};

struct CopyDetuneOpn {
	int dt;
	int mul;
};

struct CopyDetuneOpzx7 {
	int dt;
	int dt2;
	int dt3;
	int mul;
	float mulRatio;
};

struct CopyLfoN88 {
	int syncDelay;
	bool pmEnable;
	bool amEnable;
	float freq;
	int wave;
	float pms;
	float pmd;
	float amd;
	float amSmRt;
};

struct CopyLfoN88Op {
	float ams;
};

struct CopyLfoOpzx7 {
	int pmSyncDelay;
	bool pmEnable;
	float pmFreq;
	int pmIndex;
	float pms;
	float pmd;
	int amSyncDelay;
	bool amEnable;
	float amFreq;
	int amIndex;
	float ams;
	float amd;
	float amSmoothRate;
};

struct CopyUnison {
	int voices;
	int detune;
	float spread;
	bool arpEnable;
	int arpFreq;
	bool arpSmooth;
	std::array<float, Global::unisonParaVoices> paraDistance;
	std::array<int, Global::unisonParaVoices> paraDetune;
};

struct CopyQuality {
	int depth;
	int rate;
};

struct CopyMask {
	bool mask;
};

struct CopyFix {
	bool fixedMode;
	float fixedFreq;
};

struct CopySsgEg {
	int ssgEg = 0;
	float fmSsgEgFreq;
};

struct CopyOpn {
	CopyFmBase fmBase;
	CopyQuality quality;
	CopyLfoN88 n88Lfo;
	CopyUnison unison;
};

struct CopyOpnOp {
	CopyDetuneOpn detune;
	CopyEnvRgAdssr aAdsr;
	CopyFix fix;
	CopyLfoN88Op n88Lfo;
	CopyEnvPitchAdsr pAdsr;
	CopyEnvSsgSw aSsgSw;
	CopyEnvSsgSw11 aSsgSw11;
	CopyPEnvSsgSw11 pEnvSsgSw11;
	CopyMask mask;
};
