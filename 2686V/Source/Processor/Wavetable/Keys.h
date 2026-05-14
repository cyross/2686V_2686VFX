// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Keys.h"

namespace WtPrKey
{
	static inline const juce::String prefix = "WT";

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;

	static inline const juce::String level = "_LEVEL";
	static inline const juce::String bit = "_BIT";
	static inline const juce::String rate = "_RATE";
	static inline const juce::String wave = "_WAVE";
	static inline const juce::String sampleSize = "_SIZE";
	static inline const juce::String steps = "_STEPS";

	namespace Mod {
		static inline const juce::String enable = "_MOD_ENABLE";
		static inline const juce::String depth = "_MOD_DEPTH";
		static inline const juce::String speed = "_MOD_SPEED";
	};

	static inline const juce::String bypass = CorePrKey::bypass;
	static inline const juce::String adsr = CorePrKey::adsr;
	static inline const juce::String pitchAdsr = CorePrKey::pitchAdsr;

	static inline const juce::String custom32 = "_CUSTOM32_";
	static inline const juce::String custom64 = "_CUSTOM64_";
	static inline const juce::String custom128 = "_CUSTOM128_";
	static inline const juce::String custom256 = "_CUSTOM256_";

	namespace Adsr
	{
		static inline const juce::String ar = CorePrKey::Adsr::ar;
		static inline const juce::String dr = CorePrKey::Adsr::dr;
		static inline const juce::String sl = CorePrKey::Adsr::sl;
		static inline const juce::String rr = CorePrKey::Adsr::rr;
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

	static inline const juce::String dt = CorePrKey::Post::Fm::Op::dt;
	static inline const juce::String dt2 = CorePrKey::Post::Fm::Op::dt2;
};

