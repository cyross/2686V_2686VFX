// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace AdpcmGuiText
{
	namespace Category
	{
		static inline const juce::String visibleOptional = u8"[■]--- OPTIONAL ---";
		static inline const juce::String invisibleOptional = u8"[□]--- OPTIONAL ---";
		static inline const juce::String visibleQuality = u8"[■]--- QUALITY ---";
		static inline const juce::String invisibleQuality = u8"[□]--- QUALITY ---";
		static inline const juce::String visiblePan = u8"[■]--- PAN ---";
		static inline const juce::String invisiblePan = u8"[□]--- PAN ---";
		static inline const juce::String visibleUtil = u8"[■]--- UTILITY ---";
		static inline const juce::String invisibleUtil = u8"[□]--- UTILITY ---";
	}

	namespace File
	{
		static inline const juce::String load = u8"Load";
		static inline const juce::String Pcm = u8"PCM";
		static inline const juce::String clear = u8"X";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = juce::String("") + "ADPCMチャンネル設定";
	}

	namespace Adsr
	{
		static inline const juce::String bypass = u8"Bypass";
	}

	namespace PitchAdsr
	{
		static inline const juce::String bypass = u8"Bypass";
	}

	namespace SsgSwEnv
	{
		static inline const juce::String bypass = u8"Bypass";
	}

	namespace Adpcm
	{
		static inline const juce::String level = u8"Level";
		static inline const juce::String pan = u8"Pan";
		static inline const juce::String quality = u8"Bit";
		static inline const juce::String rate = u8"Rate";
		static inline const juce::String loop = u8"Loop";
		static inline const juce::String pcmOffset = u8"Offset";
		static inline const juce::String pcmRatio = u8"Ratio";

		namespace Pan
		{
			static inline const juce::String l = u8"L";
			static inline const juce::String c = u8"C";
			static inline const juce::String r = u8"R";
		}
	}

	namespace Utility
	{
		static inline const juce::String bcLevel = u8"Level -> All Ch";
	}
}
