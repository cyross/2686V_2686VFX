// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Keys.h"

namespace OpnPrKey
{
	static inline const juce::String prefix = "OPN";

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;

	static inline const juce::String alg = CorePrKey::Fm::alg;
	static inline const juce::String fb0 = CorePrKey::Fm::fb0;
	static inline const juce::String fb2 = CorePrKey::Fm::fb2;
	static inline const juce::String bit = CorePrKey::Fm::bit;
	static inline const juce::String rate = CorePrKey::Fm::rate;

	static inline const juce::String op = CorePrKey::op;

	static inline const juce::String mul = CorePrKey::Post::Fm::Op::mul;
	static inline const juce::String dt = CorePrKey::Post::Fm::Op::dt;
	static inline const juce::String ks = CorePrKey::Post::Fm::Op::ks;
	static inline const juce::String rgAr = CorePrKey::Post::Fm::Op::rgAr;
	static inline const juce::String rgDr = CorePrKey::Post::Fm::Op::rgDr;
	static inline const juce::String rgSl = CorePrKey::Post::Fm::Op::rgSl;
	static inline const juce::String rgSr = CorePrKey::Post::Fm::Op::rgSr;
	static inline const juce::String rgRr = CorePrKey::Post::Fm::Op::rgRr;
	static inline const juce::String rgTl = CorePrKey::Post::Fm::Op::rgTl;

	static inline const juce::String fix = CorePrKey::Post::Fm::Op::fix;
	static inline const juce::String fixFreq = CorePrKey::Post::Fm::Op::fixFreq;

	static inline const juce::String se = CorePrKey::Post::Fm::Op::se;
	static inline const juce::String seFreq = CorePrKey::Post::Fm::Op::seFreq;
	static inline const juce::String n88Ams = CorePrKey::Post::Fm::Op::n88Ams;

	static inline const juce::String mask = CorePrKey::Post::Fm::Op::mask;

	namespace N88Lfo
	{
		static inline const juce::String freq = "_LFO_FREQ";
		static inline const juce::String shape = "_LFO_SHAPE";
		static inline const juce::String pgShape = "_LFO_PG_SHAPE";
		static inline const juce::String egShape = "_LFO_EG_SHAPE";
		static inline const juce::String am = "_LFO_AM";
		static inline const juce::String pm = "_LFO_PM";
		static inline const juce::String pms = "_LFO_PMS";
		static inline const juce::String ams = "_LFO_AMS";
		static inline const juce::String pmd = "_LFO_PMD";
		static inline const juce::String amd = "_LFO_AMD";
		static inline const juce::String amSmoothRatio = "_LFO_ASMRT";
		static inline const juce::String syncDelay = "_LFO_SYNC_DELAY";
	};

};
