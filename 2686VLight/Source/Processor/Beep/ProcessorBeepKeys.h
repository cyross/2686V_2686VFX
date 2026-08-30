#pragma once

#include <JuceHeader.h>

namespace BeepPrKey
{
	static inline const juce::String prefix = "BEEP";
	// 帯域制限によるエイリアスノイズ抑制の ON/OFF
	static inline const juce::String antiAlias = "_ANTIALIAS";
	// タイマの基準クロック (整数分周による音程の粗さを再現する)
	static inline const juce::String timerClock = "_TIMERCLOCK";
};
