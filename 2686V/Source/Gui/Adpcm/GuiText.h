// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace AdpcmGuiText
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
		static inline const juce::String visibleAdsr = u8"[■]--- AMP ENV ---";
		static inline const juce::String invisibleAdsr = u8"[□]--- AMP ENV ---";
		static inline const juce::String visiblePitchAdsr = u8"[■]--- PITCH ENV ---";
		static inline const juce::String invisiblePitchAdsr = u8"[□]--- PITCH ENV ---";
		static inline const juce::String visibleMvol = u8"[■]--- MASTER VOLUME ---";
		static inline const juce::String invisibleMvol = u8"[□]--- MASTER VOLUME ---";
		static inline const juce::String visiblePan = u8"[■]--- PAN ---";
		static inline const juce::String invisiblePan = u8"[□]--- PAN ---";
		static inline const juce::String visibleDetune = u8"[■]--- DETUNE ---";
		static inline const juce::String invisibleDetune = u8"[□]--- DETUNE ---";
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
	}

	namespace Adsr
	{
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String ar = u8" Ar";
		static inline const juce::String dr = u8" Dr";
		static inline const juce::String sl = u8" Sl";
		static inline const juce::String rr = u8" Rr";
	}

	namespace PitchAdsr
	{
		static inline const juce::String enable = u8"Enable";
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String ar = u8" Ar";
		static inline const juce::String dr = u8" Dr";
		static inline const juce::String rr = u8" Rr";
		static inline const juce::String stl = u8" Stl";
		static inline const juce::String atl = u8" Atl";
		static inline const juce::String ssl = u8" Ssl";
		static inline const juce::String rll = u8" Rll";
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

		namespace Adsr
		{
			static inline const juce::String ar = u8"Ar";
			static inline const juce::String dr = u8"Dr";
			static inline const juce::String sl = u8"Sl";
			static inline const juce::String rr = u8"Rr";
			static inline const juce::String stl = u8"Stl";
		}
	}

	namespace Detune
	{
		static inline const juce::String Dt1 = u8"Dt1";
		static inline const juce::String Dt2 = u8"Dt2";
	}
}
