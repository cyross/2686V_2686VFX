// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace WtGuiText
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
		static inline const juce::String visibleLfo = u8"[■]--- LFO ---";
		static inline const juce::String invisibleLfo = u8"[□]--- LFO ---";
		static inline const juce::String visibleAdsr = u8"[■]--- AMP ENV ---";
		static inline const juce::String invisibleAdsr = u8"[□]--- AMP ENV ---";
		static inline const juce::String visiblePitchAdsr = u8"[■]--- PITCH ENV ---";
		static inline const juce::String invisiblePitchAdsr = u8"[□]--- PITCH ENV ---";
		static inline const juce::String visibleMvol = u8"[■]--- MASTER VOLUME ---";
		static inline const juce::String invisibleMvol = u8"[□]--- MASTER VOLUME ---";
		static inline const juce::String shape = u8"--- SHAPE ---";
		static inline const juce::String visibleMod = u8"[■]--- MODULATION ---";
		static inline const juce::String invisibileMod = u8"[□]--- MODULATION ---";
		static inline const juce::String visibleMonoMode = u8"[■]--- MONO/POLY ---";
		static inline const juce::String invisibleMonoMode = u8"[□]--- MONO/POLY ---";
		static inline const juce::String visibleWaveFile = u8"[■]--- WAVE FILE ---";
		static inline const juce::String invisibleWaveFile = u8"[□]--- WAVE FILE ---";
		static inline const juce::String visibleDetune = u8"[■]--- DETUNE ---";
		static inline const juce::String invisibleDetune = u8"[□]--- DETUNE ---";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = u8"Main";
		static inline const juce::String wtCustom = u8"Custom Wave";
	}

	namespace Adsr
	{
		static inline const juce::String bypass = u8"Bypass";
		static inline const juce::String ar = u8" Ar";
		static inline const juce::String dr = u8" Dr";
		static inline const juce::String sl = u8" Sl";
		static inline const juce::String rr = u8" Rr";
		static inline const juce::String stl = u8" Stl";
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

	namespace Lfo
	{
		static inline const juce::String pmSpeed = u8"P.Speed";
		static inline const juce::String amSpeed = u8"A.Speed";
		static inline const juce::String pmShape = u8"P.Shape";
		static inline const juce::String amShape = u8"A.Shape";
		static inline const juce::String syncDelay = u8"Delay";
		static inline const juce::String amEn = u8"AM Enable";
		static inline const juce::String pmEn = u8"PM Enable";
		static inline const juce::String ams = u8"Ams";
		static inline const juce::String pms = u8"Pms";
		static inline const juce::String amd = u8"Amd";
		static inline const juce::String pmd = u8"Pmd";
		static inline const juce::String amSmoothRatio = u8"As Ratio";
	}

	namespace Wt
	{
		static inline const juce::String level = u8"Level";
		static inline const juce::String form = u8"Form";
		static inline const juce::String size = u8"Size";
		static inline const juce::String sampleSize = u8"Sample Size";
		static inline const juce::String steps = u8"Steps";
		static inline const juce::String waveform = u8"Waveform";
		static inline const juce::String fileImport = u8"Import (.wt)";
		static inline const juce::String fileExport = u8"Export (.wt)";

		namespace Mod
		{
			static inline const juce::String enable = u8"Enable";
			static inline const juce::String depth = u8"Depth";
			static inline const juce::String speed = u8"Speed";
		}

		namespace Detune
		{
			static inline const juce::String Dt1 = u8"Dt1";
			static inline const juce::String Dt2 = u8"Dt2";
		}

		namespace Custom
		{
			static inline const juce::String to0 = u8"-> 0.0";
			static inline const juce::String to1 = u8"-> 1.0";
			static inline const juce::String toM1 = u8"-> -1.0";
			static inline const juce::String smooth = u8"Smooth Waveform";
		}
	}
}
