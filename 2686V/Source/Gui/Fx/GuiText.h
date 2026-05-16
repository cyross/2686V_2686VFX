// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace FxGuiText
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

	namespace Tab
	{
		static inline const juce::String opna = u8"OPNA";
		static inline const juce::String opn = u8"OPN";
		static inline const juce::String opl = u8"OPL";
		static inline const juce::String opl3 = u8"OPL3";
		static inline const juce::String opm = u8"OPM";
		static inline const juce::String opzx7 = u8"OPZX7";
		static inline const juce::String ssg = u8"SSG";
		static inline const juce::String wt = u8"WT";
		static inline const juce::String rhythm = u8"RHYTHM";
		static inline const juce::String adpcm = u8"ADPCM";
		static inline const juce::String beep = u8"BEEP";
		static inline const juce::String fx = u8"FX";
		static inline const juce::String preset = u8"PRESET";
		static inline const juce::String settings = u8"SETTINGS";
		static inline const juce::String about = u8"ABOUT";
	}

	namespace Category
	{
		static inline const juce::String m = u8"--- BASIC ---";
		static inline const juce::String mix = u8"--- MIX ---";
	}

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
