// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Keys.h"

namespace Opl3PrKey
{
	static inline const juce::String prefix = "OPL3";

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;

	static inline const juce::String alg = CorePrKey::Fm::alg;
	static inline const juce::String fb0 = CorePrKey::Fm::fb0;
	static inline const juce::String fb2 = CorePrKey::Fm::fb2;
	static inline const juce::String bit = CorePrKey::Fm::bit;
	static inline const juce::String rate = CorePrKey::Fm::rate;

	static inline const juce::String op = CorePrKey::op;

	static inline const juce::String mul = CorePrKey::Post::Fm::Op::mul;
	static inline const juce::String ksr = CorePrKey::Post::Fm::Op::ksr;
	static inline const juce::String ksl = CorePrKey::Post::Fm::Op::ksl;
	static inline const juce::String rgAr = CorePrKey::Post::Fm::Op::rgAr;
	static inline const juce::String rgDr = CorePrKey::Post::Fm::Op::rgDr;
	static inline const juce::String rgSl = CorePrKey::Post::Fm::Op::rgSl;
	static inline const juce::String rgSr = CorePrKey::Post::Fm::Op::rgSr;
	static inline const juce::String rgRr = CorePrKey::Post::Fm::Op::rgRr;
	static inline const juce::String rgTl = CorePrKey::Post::Fm::Op::rgTl;
	static inline const juce::String rgD1r = CorePrKey::Post::Fm::Op::rgD1r;
	static inline const juce::String rgD1l = CorePrKey::Post::Fm::Op::rgD1l;
	static inline const juce::String rgD2r = CorePrKey::Post::Fm::Op::rgD2r;

	static inline const juce::String am = CorePrKey::Post::Fm::Op::am;
	static inline const juce::String vib = CorePrKey::Post::Fm::Op::vib;
	static inline const juce::String eg = CorePrKey::Post::Fm::Op::eg;
	static inline const juce::String egType = CorePrKey::Post::Fm::Op::egType;
	static inline const juce::String pms = CorePrKey::Post::Fm::Op::pms;
	static inline const juce::String ams = CorePrKey::Post::Fm::Op::ams;
	static inline const juce::String pmd = CorePrKey::Post::Fm::Op::pmd;
	static inline const juce::String amd = CorePrKey::Post::Fm::Op::amd;

	static inline const juce::String mask = CorePrKey::Post::Fm::Op::mask;
};

