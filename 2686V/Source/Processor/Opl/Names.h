#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace OplPrName
{
	static inline const juce::String bit = " BIT";
	static inline const juce::String rate = " RATE";
	static inline const juce::String alg = " ALG";
	static inline const juce::String fb0 = " FB1";

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

	static inline const juce::String rgAr = CorePrName::Fm::Op::rgAr;
	static inline const juce::String rgDr = CorePrName::Fm::Op::rgDr;
	static inline const juce::String rgSl = CorePrName::Fm::Op::rgSl;
	static inline const juce::String rgRr = CorePrName::Fm::Op::rgRr;
	static inline const juce::String rgTl = CorePrName::Fm::Op::rgTl;
}
