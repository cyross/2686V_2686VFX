// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Keys.h"

namespace RhythmPrKey
{
	static inline const juce::String prefix = "RHYTHM";

	static inline const juce::String masterVol = CorePrKey::masterVol;
	static inline const juce::String monoMode = CorePrKey::monoMode;

	static inline const juce::String level = "_LEVEL";
	
	static inline const juce::String pad = "_PAD";

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
	};
};

