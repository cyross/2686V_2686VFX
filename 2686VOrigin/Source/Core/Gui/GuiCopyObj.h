#pragma once

#include <JuceHeader.h>
#include <array>

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

struct CopyEnvOplAdsr {
	int ar;
	int dr;
	int sl;
	int rr;
	int tl;

	bool ksr;
	int ksl;

	bool sus;
	bool egType;

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

struct CopyDetuneOpl {
	int mul;
};

struct CopyDetuneOpm {
	int dt;
	int dt2;
	int mul;
	float mulRatio;
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

struct CopyLfoOpl {
	bool pm;
	bool am;
	float pms;
	float pmd;
	float ams;
	float amd;
};

struct CopyLfoOpm {
	float freq;
	int syncDelay;
	bool pm;
	bool am;
	int pgIndex;
	int egIndex;
	int pmsIndex;
	int pmd;
	int amsIndex;
	int amd;
	float amSmoothRate;
};

struct CopyLfoOpmOp {
	bool amsEn;
};

struct CopyLfoOpna {
	int syncDelay;
	int freqsIndex;
	bool pm;
	bool am;
	float pms;
	float ams;
	float amSmoothRate;
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

struct CopyPcmQuality {
	int mode;
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

struct CopyOpna {
	CopyFmBase fmBase;
	CopyIntPan pan;
	CopyLfoN88 n88Lfo;
	CopyQuality quality;
	CopyUnison unison;
};

struct CopyOpnaOp {
	CopyDetuneOpn detune;
	CopyEnvRgAdssr aAdsr;
	CopySsgEg ssgEg;
	CopyFix fix;
	CopyLfoOpna opnaLfo;
	CopyLfoN88Op n88Lfo;
	CopyEnvPitchAdsr pAdsr;
	CopyEnvSsgSw aSsgSw;
	CopyEnvSsgSw11 aSsgSw11;
	CopyPEnvSsgSw11 pEnvSsgSw11;
	CopyMask mask;

	int waveSelect;
};

struct CopyRhythmPad {
	CopyBase base;
	CopyFloatPan pan;
	CopyPcm pcm;
	CopyPcmQuality quality;
	CopyEnvAmpAdsr aAdsr;
	CopyEnvPitchAdsr pAdsr;
	CopyEnvSsgSw11 aSsgSw11;
	CopyPEnvSsgSw11 pEnvSsgSw11;

	int noteNumber;
	bool isOneShot;
	float toneLevel;
	float noiseLevel;
	float noiseFreq;
	float mix;
};
