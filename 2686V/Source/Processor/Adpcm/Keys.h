// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Keys.h"

namespace AdpcmPrKey
{
	static inline const juce::String prefix = "ADPCM";

	static inline const juce::String bypass = CorePrKey::bypass;
	static inline const juce::String adsr = CorePrKey::adsr;
	static inline const juce::String pitchAdsr = CorePrKey::pitchAdsr;
	static inline const juce::String ssgSwEnv = CorePrKey::ssgSwEnv;

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;

	static inline const juce::String level = "_LEVEL";
	static inline const juce::String pan = "_PAN";
	static inline const juce::String mode = "_MODE";
	static inline const juce::String rate = "_RATE";
	static inline const juce::String loop = "_LOOP";
	static inline const juce::String pcmOffset = "_PCM_OFFSET";
	static inline const juce::String pcmRatio = "_PCM_RATIO";

	// ユニゾン・ハーモニー用
	namespace Unison
	{
		static inline const juce::String voices = CorePrKey::Unison::voices;
		static inline const juce::String detune = CorePrKey::Unison::detune;
		static inline const juce::String spread = CorePrKey::Unison::spread;
	}

	namespace Adsr
	{
		static inline const juce::String ar = CorePrKey::Adsr::ar;
		static inline const juce::String dr = CorePrKey::Adsr::dr;
		static inline const juce::String sl = CorePrKey::Adsr::sl;
		static inline const juce::String rr = CorePrKey::Adsr::rr;
		static inline const juce::String stl = CorePrKey::Adsr::stl;
	};

	namespace PitchAdsr
	{
		static inline const juce::String ar = CorePrKey::PitchAdsr::ar;
		static inline const juce::String dr = CorePrKey::PitchAdsr::dr;
		static inline const juce::String rr = CorePrKey::PitchAdsr::rr;
		static inline const juce::String stl = CorePrKey::PitchAdsr::stl;
		static inline const juce::String atl = CorePrKey::PitchAdsr::atl;
		static inline const juce::String ssl = CorePrKey::PitchAdsr::ssl;
		static inline const juce::String rll = CorePrKey::PitchAdsr::rll;
	};

	static inline const juce::String mul = CorePrKey::Post::Fm::Op::mul;
	static inline const juce::String mulRatio = "_MUL_RATIO";
	static inline const juce::String dt = CorePrKey::Post::Fm::Op::dt;
	static inline const juce::String dt2 = CorePrKey::Post::Fm::Op::dt2;

	namespace SsgSwEnv
	{
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

