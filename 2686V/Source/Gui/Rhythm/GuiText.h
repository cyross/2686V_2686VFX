// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace RhythmGuiText
{
	static inline const juce::String mode = u8"Mode";

	static inline const juce::String bit = u8"Bit";
	static inline const juce::String rate = u8"Rate";

	static inline const juce::String monoPoly = u8"MonoPhonic";

	namespace MasterVol
	{
		static inline const juce::String title = u8"M.Vol";
		static inline const juce::String unit = u8" dB";
	}

	namespace Category
	{
		static inline const juce::String visibleQuality = u8"[■]--- QUALITY ---";
		static inline const juce::String invisibleQuality = u8"[□]--- QUALITY ---";
		static inline const juce::String visibleAdsr = u8"[■]--- ADSR ---";
		static inline const juce::String invisibleAdsr = u8"[□]--- ADSR ---";
		static inline const juce::String visibleMvol = u8"[■]--- MASTER VOLUME ---";
		static inline const juce::String invisibleMvol = u8"[□]--- MASTER VOLUME ---";
		static inline const juce::String visiblePan = u8"[■]--- PAN ---";
		static inline const juce::String invisiblePan = u8"[□]--- PAN ---";
	}

	namespace File
	{
		static inline const juce::String load = u8"Load";
		static inline const juce::String Pcm = u8"PCM";
		static inline const juce::String clear = u8"X";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = u8"Main";
		static inline const juce::String padPrefix = u8"Pad";
	}

	namespace Rhythm
	{
		static inline const juce::String vol = u8"Level";

		namespace Pad
		{
			static inline const juce::String key = u8"Pad";
			static inline const juce::String vol = u8"Level";
			static inline const juce::String note = u8"Note";
			static inline const juce::String quality = u8"Bit";
			static inline const juce::String rate = u8"Rate";
			static inline const juce::String oneShot = u8"One Shot";
			static inline const juce::String pan = u8"Pan";
			static inline const juce::String rr = u8"Rr";
			static inline const juce::String pcmOffset = u8"Offset";
			static inline const juce::String pcmRatio = u8"Ratio";

			namespace Pan
			{
				static inline const juce::String l = u8"L";
				static inline const juce::String c = u8"C";
				static inline const juce::String r = u8"R";
			}
		}
	}
}
