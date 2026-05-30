#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace OpmPrName
{
	static inline const juce::String bit = " Bit";
	static inline const juce::String rate = " Rate";
	static inline const juce::String alg = " Algorithm";
	static inline const juce::String fb = " Feedback";
	static inline const juce::String pan = " Pan";

	static inline const juce::String op = CorePrName::Fm::Op::key;

	static inline const juce::String mul = CorePrName::Fm::Op::mul;
	static inline const juce::String dt1 = CorePrName::Fm::Op::dt1;
	static inline const juce::String dt2 = CorePrName::Fm::Op::dt2;
	static inline const juce::String seFreq = CorePrName::Fm::Op::seFreq;
	static inline const juce::String ks = CorePrName::Fm::Op::ks;
	static inline const juce::String fix = CorePrName::Fm::Op::fix;
	static inline const juce::String fixFreq = CorePrName::Fm::Op::fixFreq;
	static inline const juce::String amsEn = CorePrName::Fm::Op::amsEn;
	static inline const juce::String eg = CorePrName::Fm::Op::eg;
	static inline const juce::String mask = CorePrName::Fm::Op::mask;
	static inline const juce::String rgEn = CorePrName::Fm::Op::rgEn;
	static inline const juce::String rgAr = CorePrName::Fm::Op::rgAr;
	static inline const juce::String rgD1r = CorePrName::Fm::Op::rgD1r;
	static inline const juce::String rgD1l = CorePrName::Fm::Op::rgD1l;
	static inline const juce::String rgD2r = CorePrName::Fm::Op::rgD2r;
	static inline const juce::String rgRr = CorePrName::Fm::Op::rgRr;
	static inline const juce::String rgTl = CorePrName::Fm::Op::rgTl;
	static inline const juce::String xof = " XOF";
	static inline const juce::String ampBypass = " AMP Bypass";

	namespace Lfo
	{
		static inline const juce::String freq = " LFO Speed";
		static inline const juce::String shape = " LFO Shape";
		static inline const juce::String pgShape = " LFO PM Shape";
		static inline const juce::String egShape = " LFO AM Shape";
		static inline const juce::String am = " LFO AM Enable";
		static inline const juce::String pm = " LFO PM Enable";
		static inline const juce::String ams = " LFO AMS";
		static inline const juce::String pms = " LFO PMS";
		static inline const juce::String pmd = " LFO PMD";
		static inline const juce::String amd = " LFO AMD";
		static inline const juce::String amSmoothRatio = " LFO AM Smooth Rate";
		static inline const juce::String syncDelay = " LFO Sync Delay";
	}

	namespace PitchAdsr
	{
		static inline const juce::String enable = " PitchEnv Enable";
		static inline const juce::String bypass = " PitchEnv Bypass";
		static inline const juce::String ar = " PitchEnv Ar";
		static inline const juce::String dr = " PitchEnv Dr";
		static inline const juce::String rr = " PitchEnv Rr";
		static inline const juce::String stl = " PitchEnv Stl";
		static inline const juce::String atl = " PitchEnv Atl";
		static inline const juce::String ssl = " PitchEnv Ssl";
		static inline const juce::String rll = " PitchEnv Rll";
	};

	namespace SsgSwEnv
	{
		static inline const juce::String enable = " SSG SwEnv Enable";
		static inline const juce::String bypass = " SSG SwEnv Bypass";
		static inline const juce::String steps = " SSG SwEnv Steps";
		static inline const juce::String loop = " SSG SwEnv Loop";
		static inline const juce::String loopTo = " SSG SwEnv Loop To";
		static inline const juce::String loopCount = " SSG SwEnv Loop Count";
		static inline const juce::String stl = " SSG SwEnv STL";
		static inline const juce::String r1 = " SSG SwEnv R1";
		static inline const juce::String l1 = " SSG SwEnv L1";
		static inline const juce::String r2 = " SSG SwEnv R2";
		static inline const juce::String l2 = " SSG SwEnv L2";
		static inline const juce::String r3 = " SSG SwEnv R3";
		static inline const juce::String l3 = " SSG SwEnv L3";
		static inline const juce::String r4 = " SSG SwEnv R4";
		static inline const juce::String l4 = " SSG SwEnv L4";
		static inline const juce::String r5 = " SSG SwEnv R5";
		static inline const juce::String l5 = " SSG SwEnv L5";
		static inline const juce::String r6 = " SSG SwEnv R6";
		static inline const juce::String l6 = " SSG SwEnv L6";
	}
}
