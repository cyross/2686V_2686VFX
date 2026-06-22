#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddrParams.h"
#include "../../Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7AdddrParams.h"

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

struct CopyEnvRgAdddr {
	int ar;
	int d1r;
	int d2r;
	int d1l;
	int rr;
	int tl;

	FmRgAdddrKeyScaleMode mode;
	int ks;
	int ksrOPP;
	int kslOPP;

	bool xof;
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

struct CopyOpl {
	CopyFmBase fmBase;
	CopyQuality quality;
	CopyUnison unison;
};

struct CopyOplOp {
	CopyDetuneOpl detune;
	CopyEnvOplAdsr aAdsr;
	CopyLfoOpl lfo;
	CopyEnvPitchAdsr pAdsr;
	CopyEnvSsgSw aSsgSw;
	CopyMask mask;

	int waveSelect;
};

struct CopyOpl3 {
	CopyFmBase fmBase;
	CopyQuality quality;
	CopyUnison unison;
};

struct CopyOpl3Op {
	CopyDetuneOpl detune;
	CopyEnvOplAdsr aAdsr;
	CopyLfoOpl lfo;
	CopyEnvPitchAdsr pAdsr;
	CopyEnvSsgSw aSsgSw;
	CopyMask mask;

	int waveSelect;
};

struct CopyOpm {
	CopyFmBase fmBase;
	CopyIntPan pan;
	CopyLfoOpm lfo;
	CopyQuality quality;
	CopyUnison unison;
};

struct CopyOpmOp {
	CopyDetuneOpm detune;
	CopyEnvRgAdddr aAdsr;
	CopySsgEg ssgEg;
	CopyFix fix;
	CopyLfoOpmOp lfo;
	CopyEnvPitchAdsr pAdsr;
	CopyEnvSsgSw aSsgSw;
	CopyMask mask;

	int waveSelect;
};

struct CopyOpn {
	CopyFmBase fmBase;
	CopyQuality quality;
	CopyLfoN88 n88Lfo;
	CopyUnison unison;
};

struct CopyOpnOpm {
	CopyFmBase fmBase;
	CopyQuality quality;
	CopyUnison unison;
};

struct CopyOpnOp {
	CopyDetuneOpn detune;
	CopyEnvRgAdssr aAdsr;
	CopyFix fix;
	CopyLfoN88Op n88Lfo;
	CopyEnvPitchAdsr pAdsr;
	CopyEnvSsgSw aSsgSw;
	CopyMask mask;
};

struct CopyOpnOpmOp {
	CopyDetuneOpn detune;
	CopyEnvRgAdssr aAdsr;
	CopyFix fix;
	CopyEnvPitchAdsr pAdsr;
	CopyEnvSsgSw aSsgSw;
	CopyMask mask;
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
	CopyMask mask;

	int waveSelect;
};

struct CopyOpnaOpnOpm {
	CopyFmBase fmBase;
	CopyQuality quality;
	CopyUnison unison;
};

struct CopyOpnaOpnOpmOp {
	CopyDetuneOpn detune;
	CopyEnvRgAdssr aAdsr;
	CopySsgEg ssgEg;
	CopyFix fix;
	CopyLfoOpna opnaLfo;
	CopyLfoN88Op n88Lfo;
	CopyEnvPitchAdsr pAdsr;
	CopyEnvSsgSw aSsgSw;
	CopyMask mask;

	int waveSelect;
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

	int noteNumber;
	bool isOneShot;
	float toneLevel;
	float noiseLevel;
	float noiseFreq;
	float mix;
};
