// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorKeys.h"

namespace Opzx7PrKey
{
	static inline const juce::String prefix = "OPZX7";

	static inline const juce::String level = "_LEVEL";

	static inline const juce::String op = CorePrKey::op;
	static inline const juce::String pitchAdsr = CorePrKey::pitchAdsr;
	static inline const juce::String ssgSwEnv = CorePrKey::ssgSwEnv;
	static inline const juce::String bypass = CorePrKey::bypass;

	static inline const juce::String alg = CorePrKey::Fm::alg;
	static inline const juce::String fb = CorePrKey::Fm::fb;
	static inline const juce::String bit = CorePrKey::Fm::bit;
	static inline const juce::String rate = CorePrKey::Fm::rate;
	static inline const juce::String panpot = "_PANPOT";
	static inline const juce::String panpot_en = "_PANPOT_EN";

	static inline const juce::String mul = CorePrKey::Post::Fm::Op::mul;
	static inline const juce::String mulRatio = "_MUL_RATIO";
	static inline const juce::String dt = CorePrKey::Post::Fm::Op::dt;
	static inline const juce::String dt2 = CorePrKey::Post::Fm::Op::dt2;
	static inline const juce::String dt3 = CorePrKey::Post::Fm::Op::dt3;
	static inline const juce::String tl = CorePrKey::Post::Fm::Op::tl;
	static inline const juce::String ar = CorePrKey::Post::Fm::Op::ar;
	static inline const juce::String rr = CorePrKey::Post::Fm::Op::rr;
	static inline const juce::String d1r = CorePrKey::Post::Fm::Op::d1r;
	static inline const juce::String d1l = CorePrKey::Post::Fm::Op::d1l;
	static inline const juce::String d2r = CorePrKey::Post::Fm::Op::d2r;
	static inline const juce::String ksEn = "_KS_EN";
	static inline const juce::String ksMode = "_KS_MODE";
	static inline const juce::String ksrMA7 = CorePrKey::Post::Fm::Op::ksr;
	static inline const juce::String kslMA7 = CorePrKey::Post::Fm::Op::ksl;
	static inline const juce::String ksrOPZ = "_KSR_OPZ";
	static inline const juce::String kslOPZ = "_KSL_OPZ";
	static inline const juce::String ksBp = "_KS_BP";
	static inline const juce::String ksLc = "_KS_LC";
	static inline const juce::String ksRc = "_KS_RC";
	static inline const juce::String ksLd = "_KS_LD";
	static inline const juce::String ksRd = "_KS_RD";
	static inline const juce::String ksRs = "_KS_RS";
	static inline const juce::String rgEn = CorePrKey::Post::Fm::Op::rgEn;
	static inline const juce::String rgAr = CorePrKey::Post::Fm::Op::rgAr;
	static inline const juce::String rgRr = CorePrKey::Post::Fm::Op::rgRr;
	static inline const juce::String rgTl = CorePrKey::Post::Fm::Op::rgTl;
	static inline const juce::String rgD1r = CorePrKey::Post::Fm::Op::rgD1r;
	static inline const juce::String rgD1l = CorePrKey::Post::Fm::Op::rgD1l;
	static inline const juce::String rgD2r = CorePrKey::Post::Fm::Op::rgD2r;

	static inline const juce::String fix = CorePrKey::Post::Fm::Op::fix;
	static inline const juce::String fixFreq = CorePrKey::Post::Fm::Op::fixFreq;
	static inline const juce::String sus = "_SUS";
	static inline const juce::String xof = "_XOF";
	static inline const juce::String kor = "_KOR";
	static inline const juce::String ampBypass = "_AMP_BYPASS";
	static inline const juce::String pcmOffset = "_PCM_OFFSET";
	static inline const juce::String pcmRatio = "_PCM_RATIO";
	static inline const juce::String loopPointEnable = "_LOOPPOINT_EN";
	static inline const juce::String loopPointStart = "_LOOPPOINT_ST";
	static inline const juce::String loopPointEnd = "_LOOPPOINT_END";

	static inline const juce::String se = CorePrKey::Post::Fm::Op::se;
	static inline const juce::String seFreq = CorePrKey::Post::Fm::Op::seFreq;

	static inline const juce::String pmFreq = "_LFO_PM_FREQ";
	static inline const juce::String amFreq = "_LFO_AM_FREQ";
	static inline const juce::String pgShape = CorePrKey::Post::Fm::Op::pgShape;
	static inline const juce::String egShape = CorePrKey::Post::Fm::Op::egShape;
	static inline const juce::String am = CorePrKey::Post::Fm::Op::am;
	static inline const juce::String vib = CorePrKey::Post::Fm::Op::vib;
	static inline const juce::String pms = CorePrKey::Post::Fm::Op::pms;
	static inline const juce::String ams = CorePrKey::Post::Fm::Op::ams;
	static inline const juce::String pmd = CorePrKey::Post::Fm::Op::pmd;
	static inline const juce::String amd = CorePrKey::Post::Fm::Op::amd;
	static inline const juce::String amSmoothRatio = "_LFO_ASMRT";
	static inline const juce::String pmSyncDelay = "_LFO_PM_SYNC_DELAY";
	static inline const juce::String amSyncDelay = "_LFO_AM_SYNC_DELAY";

	static inline const juce::String ws = CorePrKey::Post::Fm::Op::ws;

	static inline const juce::String mask = CorePrKey::Post::Fm::Op::mask;

	// ユニゾン・ハーモニー用
	namespace Unison
	{
		static inline const juce::String voices = CorePrKey::Unison::voices;
		static inline const juce::String detune = CorePrKey::Unison::detune;
		static inline const juce::String spread = CorePrKey::Unison::spread;
	}

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

	namespace GlLfo
	{
		static inline const juce::String pmFreq = "_LFO_PM_FREQ";
		static inline const juce::String amFreq = "_LFO_AM_FREQ";
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
		static inline const juce::String pmSyncDelay = "_LFO_PM_SYNC_DELAY";
		static inline const juce::String amSyncDelay = "_LFO_AM_SYNC_DELAY";
	};
};

