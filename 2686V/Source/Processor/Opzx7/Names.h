#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace Opzx7PrName
{
	static inline const juce::String bit = " Bit";
	static inline const juce::String rate = " Rate";
	static inline const juce::String alg = " Algorithm";
	static inline const juce::String fb = " Feedback";
	static inline const juce::String panpot = " Panpot";
	static inline const juce::String panpot_en = " Panpot Eable";

	static inline const juce::String op = CorePrName::Fm::Op::key;

	static inline const juce::String ar = CorePrName::Fm::Op::ar;
	static inline const juce::String rr = CorePrName::Fm::Op::rr;
	static inline const juce::String d1r = CorePrName::Fm::Op::d1r;
	static inline const juce::String d1l = CorePrName::Fm::Op::d1l;
	static inline const juce::String d2r = CorePrName::Fm::Op::d2r;
	static inline const juce::String mul = CorePrName::Fm::Op::mul;
	static inline const juce::String mulRatio = " MUL Ratio";
	static inline const juce::String tl = CorePrName::Fm::Op::tl;
	static inline const juce::String dt = CorePrName::Fm::Op::dt;
	static inline const juce::String dt1 = CorePrName::Fm::Op::dt1;
	static inline const juce::String dt2 = CorePrName::Fm::Op::dt2;
	static inline const juce::String se = CorePrName::Fm::Op::se;
	static inline const juce::String seFreq = CorePrName::Fm::Op::seFreq;
	static inline const juce::String ks = CorePrName::Fm::Op::ks;
	static inline const juce::String ksr = CorePrName::Fm::Op::ksr;
	static inline const juce::String ksl = CorePrName::Fm::Op::ksl;
	static inline const juce::String fix = CorePrName::Fm::Op::fix;
	static inline const juce::String fixFreq = CorePrName::Fm::Op::fixFreq;
	static inline const juce::String ws = CorePrName::Fm::Op::ws;

	static inline const juce::String am = CorePrName::Fm::Op::am;
	static inline const juce::String vib = CorePrName::Fm::Op::vib;
	static inline const juce::String amsEn = CorePrName::Fm::Op::amsEn;

	static inline const juce::String lfoFreq = CorePrName::Fm::Op::lfoFreq;
	static inline const juce::String freqs = CorePrName::Fm::Op::freqs;

	static inline const juce::String pgShape = CorePrName::Fm::Op::pgShape;
	static inline const juce::String egShape = CorePrName::Fm::Op::egShape;

	static inline const juce::String pms = CorePrName::Fm::Op::pms;
	static inline const juce::String ams = CorePrName::Fm::Op::ams;
	static inline const juce::String pmd = CorePrName::Fm::Op::pmd;
	static inline const juce::String amd = CorePrName::Fm::Op::amd;

	static inline const juce::String n88Amd = CorePrName::Fm::Op::n88Amd;
	static inline const juce::String n88Ams = CorePrName::Fm::Op::n88Ams;
	static inline const juce::String n88Pmd = CorePrName::Fm::Op::n88Pmd;
	static inline const juce::String n88Pms = CorePrName::Fm::Op::n88Pms;

	static inline const juce::String syncDelay = CorePrName::Fm::Op::syncDelay;
	static inline const juce::String egType = CorePrName::Fm::Op::egType;
	static inline const juce::String eg = CorePrName::Fm::Op::eg;
	static inline const juce::String pcmOffset = " PCM Offset";
	static inline const juce::String pcmRatio = " PCM Ratio";

	static inline const juce::String mask = CorePrName::Fm::Op::mask;

	static inline const juce::String rgEn = CorePrName::Fm::Op::rgEn;
	static inline const juce::String rgAr = CorePrName::Fm::Op::rgAr;
	static inline const juce::String rgRr = CorePrName::Fm::Op::rgRr;
	static inline const juce::String rgTl = CorePrName::Fm::Op::rgTl;
	static inline const juce::String rgD1r = CorePrName::Fm::Op::rgD1r;
	static inline const juce::String rgD1l = CorePrName::Fm::Op::rgD1l;
	static inline const juce::String rgD2r = CorePrName::Fm::Op::rgD2r;

	static inline const juce::String sus = " SUS";
	static inline const juce::String xof = " XOF";
	static inline const juce::String ampBypass = " AMP Bypass";

	static inline const juce::String phaseOffset = "Phase Offset";

	namespace PitchAdsr
	{
		static inline const juce::String enable = " PitchEnv Enable";
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

	namespace GlLfo
	{
		static inline const juce::String freq = " Global LFO Speed";
		static inline const juce::String shape = " Global LFO Shape";
		static inline const juce::String pgShape = " Global LFO PM Shape";
		static inline const juce::String egShape = " Global LFO AM Shape";
		static inline const juce::String am = " Global LFO Am Enable";
		static inline const juce::String pm = " Global LFO Pm Enable";
		static inline const juce::String ams = " Global LFO AMS";
		static inline const juce::String pms = " Global LFO PMS";
		static inline const juce::String pmd = " Global LFO PMD";
		static inline const juce::String amd = " Global LFO AMD";
		static inline const juce::String amSmoothRatio = " Global LFO AM Smooth Rate";
		static inline const juce::String syncDelay = " Global LFO Sync Delay";
	}
}
