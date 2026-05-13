// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Keys.h"

namespace PrKey
{
	static inline const juce::String prefix = "OPM";

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;

	static inline const juce::String alg = CorePrKey::Fm::alg;
	static inline const juce::String fb0 = CorePrKey::Fm::fb0;
	static inline const juce::String fb2 = CorePrKey::Fm::fb2;
	static inline const juce::String bit = CorePrKey::Fm::bit;
	static inline const juce::String rate = CorePrKey::Fm::rate;
	static inline const juce::String pan = "_PAN";

	static inline const juce::String op = CorePrKey::op;

	static inline const juce::String mul = CorePrKey::Post::Fm::Op::mul;
	static inline const juce::String dt = CorePrKey::Post::Fm::Op::dt;
	static inline const juce::String dt2 = CorePrKey::Post::Fm::Op::dt2;
	static inline const juce::String ks = CorePrKey::Post::Fm::Op::ks;
	static inline const juce::String rgAr = CorePrKey::Post::Fm::Op::rgAr;
	static inline const juce::String rgRr = CorePrKey::Post::Fm::Op::rgRr;
	static inline const juce::String rgTl = CorePrKey::Post::Fm::Op::rgTl;
	static inline const juce::String rgD1r = CorePrKey::Post::Fm::Op::rgD1r;
	static inline const juce::String rgD1l = CorePrKey::Post::Fm::Op::rgD1l;
	static inline const juce::String rgD2r = CorePrKey::Post::Fm::Op::rgD2r;

	static inline const juce::String fix = CorePrKey::Post::Fm::Op::fix;
	static inline const juce::String fixFreq = CorePrKey::Post::Fm::Op::fixFreq;
	static inline const juce::String seFreq = CorePrKey::Post::Fm::Op::seFreq;
	static inline const juce::String amsEn = CorePrKey::Post::Fm::Op::amsEn;

	static inline const juce::String mask = CorePrKey::Post::Fm::Op::mask;

	namespace Lfo
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

