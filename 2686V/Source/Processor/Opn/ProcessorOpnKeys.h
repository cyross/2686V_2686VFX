// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorKeys.h"

namespace OpnPrKey
{
	static inline const juce::String prefix = "OPN";

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
	static inline const juce::String dt = CorePrKey::Post::Fm::Op::dt;
	static inline const juce::String ks = CorePrKey::Post::Fm::Op::ks;
	static inline const juce::String rgAr = CorePrKey::Post::Fm::Op::rgAr;
	static inline const juce::String rgDr = CorePrKey::Post::Fm::Op::rgDr;
	static inline const juce::String rgSl = CorePrKey::Post::Fm::Op::rgSl;
	static inline const juce::String rgSr = CorePrKey::Post::Fm::Op::rgSr;
	static inline const juce::String rgRr = CorePrKey::Post::Fm::Op::rgRr;
	static inline const juce::String rgTl = CorePrKey::Post::Fm::Op::rgTl;
	static inline const juce::String xof = "_XOF";
	static inline const juce::String kor = "_KOR";
	static inline const juce::String ampBypass = "_AMP_BYPASS";

	static inline const juce::String fix = CorePrKey::Post::Fm::Op::fix;
	static inline const juce::String fixFreq = CorePrKey::Post::Fm::Op::fixFreq;

	static inline const juce::String se = CorePrKey::Post::Fm::Op::se;
	static inline const juce::String seFreq = CorePrKey::Post::Fm::Op::seFreq;
	static inline const juce::String n88Ams = CorePrKey::Post::Fm::Op::n88Ams;

	static inline const juce::String mask = CorePrKey::Post::Fm::Op::mask;

	// ユニゾン・ハーモニー用
	namespace Unison
	{
		static inline const juce::String voices = CorePrKey::Unison::voices;
		static inline const juce::String detune = CorePrKey::Unison::detune;
		static inline const juce::String spread = CorePrKey::Unison::spread;
	}

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
