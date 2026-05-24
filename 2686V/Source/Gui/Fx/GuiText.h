// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace FxGuiText
{
	namespace Group
	{
		static inline const juce::String mainGroup = u8"FX";
		static inline const juce::String fxTremolo = u8"Tremolo";
		static inline const juce::String fxVibrato = u8"Vibrato";
		static inline const juce::String fxMbc = u8"Modern Bit Crusher";
		static inline const juce::String fxDelay = u8"Delay";
		static inline const juce::String fxReverb = u8"Reverb";
		static inline const juce::String fxFilter = u8"Filter";
	}

	namespace Fx
	{
		static inline const juce::String masterBypass = u8"Bypass";
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String reset = u8"Reset";
		static inline const juce::String mix = u8"Mix";
		
		namespace Mix
		{
			static inline const juce::String dry = u8"Dry";
			static inline const juce::String mix = u8"50%";
			static inline const juce::String wet = u8"Wet";
		}

		namespace Tremolo
		{
			static inline const juce::String rate = u8"Rate";
			static inline const juce::String depth = u8"Depth";
		}

		namespace Vibrate
		{
			static inline const juce::String rate = u8"Rate";
			static inline const juce::String depth = u8"Depth";
		}

		namespace Mbc
		{
			static inline const juce::String bit = u8"Bit";
			static inline const juce::String rate = u8"Rate";
		}

		namespace Delay
		{
			static inline const juce::String time = u8"Time";
			static inline const juce::String fb = u8"Fb";
		}

		namespace Reverb
		{
			static inline const juce::String size = u8"Size";
			static inline const juce::String damp = u8"Damp";
		}

		namespace Filter
		{
			static inline const juce::String type = u8"Type";
			static inline const juce::String freq = u8"Freq";
			static inline const juce::String q = u8" Q";
		}
	}
}
