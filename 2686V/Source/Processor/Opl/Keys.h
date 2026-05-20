// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Keys.h"

namespace OplPrKey
{
	static inline const juce::String prefix = "OPL";

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;
	static inline const juce::String pitchAdsr = CorePrKey::pitchAdsr;
	static inline const juce::String ssgSwEnv = CorePrKey::ssgSwEnv;
	static inline const juce::String bypass = CorePrKey::bypass;

	static inline const juce::String alg = CorePrKey::Fm::alg;
	static inline const juce::String fb = CorePrKey::Fm::fb;
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
	static inline const juce::String sus = "_SUS";

	static inline const juce::String am = CorePrKey::Post::Fm::Op::am;
	static inline const juce::String vib = CorePrKey::Post::Fm::Op::vib;
	static inline const juce::String eg = CorePrKey::Post::Fm::Op::eg;
	static inline const juce::String egType = CorePrKey::Post::Fm::Op::egType;
	static inline const juce::String pms = CorePrKey::Post::Fm::Op::pms;
	static inline const juce::String ams = CorePrKey::Post::Fm::Op::ams;
	static inline const juce::String pmd = CorePrKey::Post::Fm::Op::pmd;
	static inline const juce::String amd = CorePrKey::Post::Fm::Op::amd;

	static inline const juce::String mask = CorePrKey::Post::Fm::Op::mask;

	namespace PitchAdsr
	{
		static inline const juce::String enable = CorePrKey::PitchAdsr::enable;
		static inline const juce::String ar = CorePrKey::PitchAdsr::ar;
		static inline const juce::String dr = CorePrKey::PitchAdsr::dr;
		static inline const juce::String rr = CorePrKey::PitchAdsr::rr;
		static inline const juce::String stl = CorePrKey::PitchAdsr::stl;
		static inline const juce::String atl = CorePrKey::PitchAdsr::atl;
		static inline const juce::String ssl = CorePrKey::PitchAdsr::ssl;
		static inline const juce::String rll = CorePrKey::PitchAdsr::rll;
	};

	namespace SsgSwEnv
	{
		static inline const juce::String enable = "_SSGSW_ENABLE";
		static inline const juce::String bypass = "_SSGSW_BYPASS";
		static inline const juce::String steps = "_SSGSW_STEPS";
		static inline const juce::String stl = "_SSGSW_STL";
		static inline const juce::String loop = "_SSGSW_LOOP";
		static inline const juce::String loopTo = "_SSGSW_LOOPTO";
		static inline const juce::String loopCount = "_SSGSW_LOOPCNT";
		static inline const juce::String r1 = "_SSGSW_R1";
		static inline const juce::String l1 = "_SSGSW_L1";
		static inline const juce::String r2 = "_SSGSW_R2";
		static inline const juce::String l2 = "_SSGSW_L2";
		static inline const juce::String r3 = "_SSGSW_R3";
		static inline const juce::String l3 = "_SSGSW_L3";
		static inline const juce::String r4 = "_SSGSW_R4";
		static inline const juce::String l4 = "_SSGSW_L4";
		static inline const juce::String r5 = "_SSGSW_R5";
		static inline const juce::String l5 = "_SSGSW_L5";
		static inline const juce::String r6 = "_SSGSW_R6";
		static inline const juce::String l6 = "_SSGSW_L6";
	};
};

