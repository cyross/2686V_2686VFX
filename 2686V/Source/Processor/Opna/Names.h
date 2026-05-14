#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace OpnaPrName
{
	static inline const juce::String bit = " BIT";
	static inline const juce::String rate = " RATE";
	static inline const juce::String alg = " ALG";
	static inline const juce::String fb0 = " FB1";
	static inline const juce::String fb2 = " FB3";
	static inline const juce::String pan = " PAN";

	static inline const juce::String op = CorePrName::Fm::Op::key;

	static inline const juce::String mul = CorePrName::Fm::Op::mul;
	static inline const juce::String dt = CorePrName::Fm::Op::dt;
	static inline const juce::String se = CorePrName::Fm::Op::se;
	static inline const juce::String seFreq = CorePrName::Fm::Op::seFreq;
	static inline const juce::String ks = CorePrName::Fm::Op::ks;
	static inline const juce::String fix = CorePrName::Fm::Op::fix;
	static inline const juce::String fixFreq = CorePrName::Fm::Op::fixFreq;
	static inline const juce::String am = CorePrName::Fm::Op::am;
	static inline const juce::String vib = CorePrName::Fm::Op::vib;
	static inline const juce::String syncDelay = CorePrName::Fm::Op::syncDelay;
	static inline const juce::String freqs = CorePrName::Fm::Op::freqs;
	static inline const juce::String pms = CorePrName::Fm::Op::pms;
	static inline const juce::String ams = CorePrName::Fm::Op::ams;
	static inline const juce::String n88Ams = CorePrName::Fm::Op::n88Ams;
	static inline const juce::String rgAr = CorePrName::Fm::Op::rgAr;
	static inline const juce::String rgDr = CorePrName::Fm::Op::rgDr;
	static inline const juce::String rgSl = CorePrName::Fm::Op::rgSl;
	static inline const juce::String rgSr = CorePrName::Fm::Op::rgSr;
	static inline const juce::String rgRr = CorePrName::Fm::Op::rgRr;
	static inline const juce::String rgTl = CorePrName::Fm::Op::rgTl;
	static inline const juce::String mask = CorePrName::Fm::Op::mask;

	namespace N88Lfo
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
