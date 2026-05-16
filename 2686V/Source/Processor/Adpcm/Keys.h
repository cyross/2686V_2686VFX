// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Keys.h"

namespace AdpcmPrKey
{
	static inline const juce::String prefix = "ADPCM";

	static inline const juce::String bypass = CorePrKey::bypass;
	static inline const juce::String adsr = CorePrKey::adsr;

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;

	static inline const juce::String level = "_LEVEL";
	static inline const juce::String pan = "_PAN";
	static inline const juce::String mode = "_MODE";
	static inline const juce::String rate = "_RATE";
	static inline const juce::String loop = "_LOOP";
	static inline const juce::String pcmOffset = "_PCM_OFFSET";
	static inline const juce::String pcmRatio = "_PCM_RATIO";

	namespace Adsr
	{
		static inline const juce::String ar = CorePrKey::Adsr::ar;
		static inline const juce::String dr = CorePrKey::Adsr::dr;
		static inline const juce::String sl = CorePrKey::Adsr::sl;
		static inline const juce::String rr = CorePrKey::Adsr::rr;
	};
};

