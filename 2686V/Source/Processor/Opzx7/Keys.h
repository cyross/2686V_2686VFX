// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Keys.h"

namespace Opzx7PrKey
{
	static inline const juce::String prefix = "OPZX7";

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;

	static inline const juce::String op = CorePrKey::op;
	static inline const juce::String pitchAdsr = CorePrKey::pitchAdsr;

	static inline const juce::String alg = CorePrKey::Fm::alg;
	static inline const juce::String fb0 = CorePrKey::Fm::fb0;
	static inline const juce::String fb2 = CorePrKey::Fm::fb2;
	static inline const juce::String bit = CorePrKey::Fm::bit;
	static inline const juce::String rate = CorePrKey::Fm::rate;
	static inline const juce::String panpot = "_PANPOT";
	static inline const juce::String panpot_en = "_PANPOT_EN";

	static inline const juce::String mul = CorePrKey::Post::Fm::Op::mul;
	static inline const juce::String dt = CorePrKey::Post::Fm::Op::dt;
	static inline const juce::String dt2 = CorePrKey::Post::Fm::Op::dt2;
	static inline const juce::String tl = CorePrKey::Post::Fm::Op::tl;
	static inline const juce::String ar = CorePrKey::Post::Fm::Op::ar;
	static inline const juce::String rr = CorePrKey::Post::Fm::Op::rr;
	static inline const juce::String d1r = CorePrKey::Post::Fm::Op::d1r;
	static inline const juce::String d1l = CorePrKey::Post::Fm::Op::d1l;
	static inline const juce::String d2r = CorePrKey::Post::Fm::Op::d2r;
	static inline const juce::String ks = CorePrKey::Post::Fm::Op::ks;
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
	static inline const juce::String phaseOffset = "_PHASE_OFFSET";
	static inline const juce::String pcmOffset = "_PCM_OFFSET";
	static inline const juce::String pcmRatio = "_PCM_RATIO";

	static inline const juce::String se = CorePrKey::Post::Fm::Op::se;
	static inline const juce::String seFreq = CorePrKey::Post::Fm::Op::seFreq;

	static inline const juce::String lfoFreq = CorePrKey::Post::Fm::Op::lfoFreq;

	static inline const juce::String am = CorePrKey::Post::Fm::Op::am;
	static inline const juce::String vib = CorePrKey::Post::Fm::Op::vib;

	static inline const juce::String pgShape = CorePrKey::Post::Fm::Op::pgShape;
	static inline const juce::String egShape = CorePrKey::Post::Fm::Op::egShape;

	static inline const juce::String pms = CorePrKey::Post::Fm::Op::pms;
	static inline const juce::String ams = CorePrKey::Post::Fm::Op::ams;
	static inline const juce::String pmd = CorePrKey::Post::Fm::Op::pmd;
	static inline const juce::String amd = CorePrKey::Post::Fm::Op::amd;

	static inline const juce::String syncDelay = CorePrKey::Post::Fm::Op::syncDelay;

	static inline const juce::String ws = CorePrKey::Post::Fm::Op::ws;

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

	namespace GlLfo
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

