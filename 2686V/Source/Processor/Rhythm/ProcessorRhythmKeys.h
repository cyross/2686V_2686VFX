// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorKeys.h"

namespace RhythmPrKey
{
	static inline const juce::String prefix = "RHYTHM";

	static inline const juce::String adsr = CorePrKey::adsr;
	static inline const juce::String bypass = CorePrKey::bypass;
	static inline const juce::String pitchAdsr = CorePrKey::pitchAdsr;
	static inline const juce::String ssgSwEnv = CorePrKey::ssgSwEnv;

	static inline const juce::String level = "_LEVEL";
	
	static inline const juce::String pad = "_PAD";

	// ユニゾン・ハーモニー用
	namespace Unison
	{
		static inline const juce::String voices = CorePrKey::Unison::voices;
		static inline const juce::String detune = CorePrKey::Unison::detune;
		static inline const juce::String spread = CorePrKey::Unison::spread;
	}

	namespace Pad
	{
		static inline const juce::String volume = "_VOL";
		static inline const juce::String mode = "_MODE";
		static inline const juce::String rate = "_RATE";
		static inline const juce::String pan = "_PAN";
		static inline const juce::String note = "_NOTE";
		static inline const juce::String oneShot = "_ONESHOT";
		static inline const juce::String rr = "_RR";
		static inline const juce::String pcmOffset = "_PCM_OFFSET";
		static inline const juce::String pcmRatio = "_PCM_RATIO";

		namespace Adsr
		{
			static inline const juce::String ar = CorePrKey::Adsr::ar;
			static inline const juce::String dr = CorePrKey::Adsr::dr;
			static inline const juce::String sl = CorePrKey::Adsr::sl;
			static inline const juce::String rr = CorePrKey::Adsr::rr;
			static inline const juce::String stl = CorePrKey::Adsr::stl;
			static inline const juce::String kor = "_KOR";
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
	};
};

