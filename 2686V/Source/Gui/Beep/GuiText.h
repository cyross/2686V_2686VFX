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
		static inline const juce::String visibleAdsr = u8"[■]--- AMP ENV ---";
		static inline const juce::String invisibleAdsr = u8"[□]--- AMP ENV ---";
		static inline const juce::String visiblePitchAdsr = u8"[■]--- PITCH ENV ---";
		static inline const juce::String invisiblePitchAdsr = u8"[□]--- PITCH ENV ---";
		static inline const juce::String visibleFix = u8"[■]--- FIX ---";
		static inline const juce::String invisibleFix = u8"[□]--- FIX ---";
		static inline const juce::String visibleSwEnv = u8"[■]--- SSG SW ENV ---";
		static inline const juce::String invisibleSwEnv = u8"[□]--- SSG SW ENV ---";
		static inline const juce::String visibleMvol = u8"[■]--- MASTER VOLUME ---";
		static inline const juce::String invisibleMvol = u8"[□]--- MASTER VOLUME ---";
		static inline const juce::String visibleMonoMode = u8"[■]--- MONO/POLY ---";
		static inline const juce::String invisibleMonoMode = u8"[□]--- MONO/POLY ---";
		static inline const juce::String visibleDetune = u8"[■]--- DETUNE ---";
		static inline const juce::String invisibleDetune = u8"[□]--- DETUNE ---";
		static inline const juce::String visibleUnison = u8"[■]--- UNISON/HARMONY ---";
		static inline const juce::String invisibleUnison = u8"[□]--- UNISON/HARMONY ---";
	}

	namespace Unison
	{
		static inline const juce::String voices = "Voices";
		static inline const juce::String detune = "Detune";
		static inline const juce::String spread = "Spread";
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
			static inline const juce::String Stl = u8"Stl";
			static inline const juce::String kor = u8"Kor";
		}
	}

	namespace PitchAdsr
	{
		static inline const juce::String enable = u8"Enable";
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String ar = u8"Ar";
		static inline const juce::String dr = u8"Dr";
		static inline const juce::String rr = u8"Rr";
		static inline const juce::String stl = u8"Stl";
		static inline const juce::String atl = u8"Atl";
		static inline const juce::String ssl = u8"Ssl";
		static inline const juce::String rll = u8"Rll";
	}

	namespace SsgSwEnv
	{
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String steps = u8"Steps";
		static inline const juce::String loop = u8"Loop";
		static inline const juce::String loopTo = u8"Loop To";
		static inline const juce::String loopCount = u8"Loop Cnt";
		static inline const juce::String stl = u8"Stl";
		static inline const juce::String r1 = u8"R1";
		static inline const juce::String l1 = u8"L1";
		static inline const juce::String r2 = u8"R2";
		static inline const juce::String l2 = u8"L2";
		static inline const juce::String r3 = u8"R3";
		static inline const juce::String l3 = u8"L3";
		static inline const juce::String r4 = u8"R4";
		static inline const juce::String l4 = u8"L4";
		static inline const juce::String r5 = u8"R5";
		static inline const juce::String l5 = u8"L5";
		static inline const juce::String r6 = u8"R6";
		static inline const juce::String l6 = u8"L6";
	}

	namespace Detune
	{
		static inline const juce::String Mul = u8"Mul";
		static inline const juce::String MulRatio = u8"M.Ratio";
		static inline const juce::String Dt1 = u8"Dt1";
		static inline const juce::String Dt2 = u8"Dt2";
	}
}
