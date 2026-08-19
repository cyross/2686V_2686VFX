#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7AdddrParams.h"

struct CopyBase {
	float level;
};

struct CopyFmBase {
	float level;
	int algorithm;
	int feedback;
};

struct CopyKSOpl {
	bool ksr;
	int ksl;
};

struct CopyKSOpp {
	int ksr;
	int ksl;
};

struct CopyKSOps {
	int ksBp;
	int ksLc;
	int ksRc;
	float ksLd;
	float ksRd;
	int ksRs;
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

struct CopyEnvOpzx7Adddr {
	bool rgEnable;

	float ar;
	float d1r;
	float d2r;
	float d1l;
	float rr;
	float tl;

	int rgAr;
	int rgD1r;
	int rgG2r;
	int rgD1l;
	int rgRr;
	int rgTl;

	bool ksEn;
	Opzx7AdddrKeyScaleMode mode;
	bool ksrMA7;
	int kslMA7;
	int ksrOPZ;
	int kslOPZ;
	int ksBp;
	int ksLc;
	int ksRc;
	float ksLd;
	float ksRd;
	int ksRs;

	bool sus;
	bool xof;
	bool kor;
	bool bypass;
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

struct CopyDetuneOpzx7 {
	int dt;
	int dt2;
	int dt3;
	int mul;
	float mulRatio;
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

struct CopyIntPan {
	int pan;
};

struct CopyFloatPan {
	float pan;
};

struct CopyPanpot {
	bool enable;
	int panpot;
};

struct CopyPcm {
	juce::String finename;
	float pcmOffset;
	float pcmRatio;
};

struct CopyWt {
	juce::String filename;
};

struct CopySsgEg {
	int ssgEg = 0;
	float fmSsgEgFreq;
};

struct CopyOpzx7 {
	CopyFmBase fmBase;
	CopyPanpot panpot;
	CopyLfoOpzx7 lfo;
	CopyQuality quality;
	CopyUnison unison;
};

struct CopyOpzx7Op {
	CopyDetuneOpzx7 detune;
	CopyEnvOpzx7Adddr aAdsr;
	CopyFix fix;
	CopySsgEg ssgEg;
	CopyPcm pcm;
	CopyWt wt;
	CopyWt wt2;
	CopyLfoOpzx7 lfo;
	CopyEnvPitchAdsr pAdsr;
	CopyEnvSsgSw aSsgSw;
	CopyEnvSsgSw11 aSsgSw11;
	CopyPEnvSsgSw11 pEnvSsgSw11;
	CopyMask mask;

	int waveSelect;
};
