// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace WtGuiText
{
	static inline const juce::String bit = u8"Bit";
	static inline const juce::String rate = u8"Rate";

	namespace Category
	{
		static inline const juce::String visibleQuality = u8"[■]--- QUALITY ---";
		static inline const juce::String invisibleQuality = u8"[□]--- QUALITY ---";
		static inline const juce::String visibleLfo = u8"[■]--- LFO ---";
		static inline const juce::String invisibleLfo = u8"[□]--- LFO ---";
		static inline const juce::String shape = u8"--- SHAPE ---";
		static inline const juce::String visibleMod = u8"[■]--- MODULATION ---";
		static inline const juce::String invisibileMod = u8"[□]--- MODULATION ---";
		static inline const juce::String visibleWaveFile = u8"[■]--- WAVE FILE ---";
		static inline const juce::String invisibleWaveFile = u8"[□]--- WAVE FILE ---";
		static inline const juce::String visibleUtil = u8"[■]--- UTILITY ---";
		static inline const juce::String invisibleUtil = u8"[□]--- UTILITY ---";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = juce::String("") + "波形メモリチャンネル設定";
		static inline const juce::String wtCustom = juce::String("") + "波形メモリ編集";
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

		namespace Custom
		{
			static inline const juce::String to0 = u8"-> 0.0";
			static inline const juce::String to1 = u8"-> 1.0";
			static inline const juce::String toM1 = u8"-> -1.0";
			static inline const juce::String smooth = juce::String("") + "波形を滑らかにする";
		}
	}

	namespace Utility
	{
		static inline const juce::String bcLevel = u8"Level -> All Ch";
	}
}
