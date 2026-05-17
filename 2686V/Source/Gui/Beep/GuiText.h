// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace BeepGuiText
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
		static inline const juce::String visibleAdsr = u8"[■]--- ADSR ---";
		static inline const juce::String invisibleAdsr = u8"[□]--- ADSR ---";
		static inline const juce::String visibleFix = u8"[■]--- FIX ---";
		static inline const juce::String invisibleFix = u8"[□]--- FIX ---";
		static inline const juce::String visibleMvol = u8"[■]--- MASTER VOLUME ---";
		static inline const juce::String invisibleMvol = u8"[□]--- MASTER VOLUME ---";
		static inline const juce::String visibleMonoMode = u8"[■]--- MONO/POLY ---";
		static inline const juce::String invisibleMonoMode = u8"[□]--- MONO/POLY ---";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = u8"Main";
	}

	namespace Beep
	{
		static inline const juce::String Level = u8"Level";
		static inline const juce::String Fix = u8"Enable";
		static inline const juce::String FFreq = u8"Freq";
		static inline const juce::String To2k = u8"-> 2k";

		namespace Adsr
		{
			static inline const juce::String Bypass = u8"Bypass";
			static inline const juce::String Ar = u8"Ar";
			static inline const juce::String Dr = u8"Dr";
			static inline const juce::String Sl = u8"Sl";
			static inline const juce::String Rr = u8"Rr";
		}
	}
}
