#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/ProcessorNames.h"

// UI上に表示するタイトル等を管理
namespace OplPrName
{
	static inline const juce::String bit = " Bit";
	static inline const juce::String rate = " Rate";
	static inline const juce::String alg = " Algorithm";
	static inline const juce::String fb = " Feedback";

	static inline const juce::String op = CorePrName::Fm::Op::key;

	static inline const juce::String ar = CorePrName::Fm::Op::ar;
	static inline const juce::String dr = CorePrName::Fm::Op::dr;
	static inline const juce::String sl = CorePrName::Fm::Op::sl;
	static inline const juce::String rr = CorePrName::Fm::Op::rr;
	static inline const juce::String mul = CorePrName::Fm::Op::mul;
	static inline const juce::String tl = CorePrName::Fm::Op::tl;
	static inline const juce::String ksr = CorePrName::Fm::Op::ksr;
	static inline const juce::String ksl = CorePrName::Fm::Op::ksl;
	static inline const juce::String am = CorePrName::Fm::Op::am;
	static inline const juce::String vib = CorePrName::Fm::Op::vib;
	static inline const juce::String egType = CorePrName::Fm::Op::egType;
	static inline const juce::String pms = CorePrName::Fm::Op::pms;
	static inline const juce::String ams = CorePrName::Fm::Op::ams;
	static inline const juce::String pmd = CorePrName::Fm::Op::pmd;
	static inline const juce::String amd = CorePrName::Fm::Op::amd;
	static inline const juce::String eg = CorePrName::Fm::Op::eg;
	static inline const juce::String sus = CorePrName::Fm::Op::sus;
	static inline const juce::String mask = CorePrName::Fm::Op::mask;
	static inline const juce::String xof = " XOF";
	static inline const juce::String kor = " KOR";
	static inline const juce::String ampBypass = " AMP Bypass";

	static inline const juce::String rgAr = CorePrName::Fm::Op::rgAr;
	static inline const juce::String rgDr = CorePrName::Fm::Op::rgDr;
	static inline const juce::String rgSl = CorePrName::Fm::Op::rgSl;
	static inline const juce::String rgRr = CorePrName::Fm::Op::rgRr;
	static inline const juce::String rgTl = CorePrName::Fm::Op::rgTl;

	// ユニゾン・ハーモニー用
	namespace Unison
	{
		static inline const juce::String voices = CorePrName::Unison::voices;
		static inline const juce::String detune = CorePrName::Unison::detune;
		static inline const juce::String spread = CorePrName::Unison::spread;
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
