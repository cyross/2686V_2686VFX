#pragma once

#include <JuceHeader.h>

namespace BeepPrKey
{
	static inline const juce::String prefix = "BEEP";
	// 帯域制限によるエイリアスノイズ抑制の ON/OFF
	static inline const juce::String antiAlias = "_ANTIALIAS";
};
