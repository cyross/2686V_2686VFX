#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace Opzx7PrName
{
	static inline const juce::String bit = " BIT";
	static inline const juce::String rate = " RATE";
	static inline const juce::String alg = " ALG";
	static inline const juce::String fb0 = " FB1";
	static inline const juce::String fb2 = " FB3";
	static inline const juce::String panpot = " PANPOT";
	static inline const juce::String panpot_en = " PANPOT Eable";

	static inline const juce::String op = CorePrName::Fm::Op::key;

	static inline const juce::String ar = CorePrName::Fm::Op::ar;
	static inline const juce::String rr = CorePrName::Fm::Op::rr;
	static inline const juce::String d1r = CorePrName::Fm::Op::d1r;
	static inline const juce::String d1l = CorePrName::Fm::Op::d1l;
	static inline const juce::String d2r = CorePrName::Fm::Op::d2r;
	static inline const juce::String mul = CorePrName::Fm::Op::mul;
	static inline const juce::String tl = CorePrName::Fm::Op::tl;
	static inline const juce::String dt = CorePrName::Fm::Op::dt;
	static inline const juce::String dt1 = CorePrName::Fm::Op::dt1;
	static inline const juce::String dt2 = CorePrName::Fm::Op::dt2;
	static inline const juce::String se = CorePrName::Fm::Op::sr;
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

	static inline const juce::String phaseOffset = "PHASE OFFSET";

	namespace PitchAdsr
	{
		static inline const juce::String enable = " PITCH-ENV Enable";
		static inline const juce::String ar = " PITCH-ENV AR";
		static inline const juce::String dr = " PITCH-ENV DR";
		static inline const juce::String rr = " PITCH-ENV RR";
		static inline const juce::String stl = " PITCH-ENV STL";
		static inline const juce::String atl = " PITCH-ENV ATL";
		static inline const juce::String ssl = " PITCH-ENV SSL";
		static inline const juce::String rll = " PITCH-ENV RLL";
	};

	namespace GlLfo
	{
		static inline const juce::String freq = " L.SPEED";
		static inline const juce::String shape = " L.SHAPE";
		static inline const juce::String pgShape = " L.P.SHAPE";
		static inline const juce::String egShape = " L.A.SHAPE";
		static inline const juce::String am = " LFO.AM";
		static inline const juce::String pm = " LFO.PM";
		static inline const juce::String ams = " L.AMS";
		static inline const juce::String pms = " L.PMS";
		static inline const juce::String pmd = " L.PMD";
		static inline const juce::String amd = " L.AMD";
		static inline const juce::String amSmoothRatio = " L.ASMRT";
		static inline const juce::String syncDelay = " L.S.Delay";
	}
}
