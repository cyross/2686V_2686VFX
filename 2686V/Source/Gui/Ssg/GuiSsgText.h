// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace SsgGuiText
{
	static inline const juce::String bit = u8"Bit";
	static inline const juce::String rate = u8"Rate";

	namespace Category
	{
		static inline const juce::String visibleQuality = u8"[■]--- QUALITY ---";
		static inline const juce::String invisibleQuality = u8"[□]--- QUALITY ---";
		static inline const juce::String visibleLfo = u8"[■]--- LFO ---";
		static inline const juce::String invisibleLfo = u8"[□]--- LFO ---";
		static inline const juce::String shape = u8"--- ENVELOPE ---";
		static inline const juce::String visibleHwEnv = u8"[■]--- HW ENV ---";
		static inline const juce::String invisibleHwEnv = u8"[□]--- HW ENV ---";
		static inline const juce::String mix = u8"--- MIX ---";
		static inline const juce::String ssgTone = u8"--- TONE ---";
		static inline const juce::String ssgNoise = u8"--- NOISE ---";
		static inline const juce::String invert = u8"--- INVERT ---";
		static inline const juce::String peak = u8"--- PEAK ---";
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
		static inline const juce::String mainGroup = juce::String("") + "SSGチャンネル設定";
		static inline const juce::String ssgDuty = u8"矩形波(デューティ比)設定";
		static inline const juce::String ssgTri = juce::String("") + "三角波設定";
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

	namespace Ssg
	{
		static inline const juce::String level = u8"Level";

		namespace Voice
		{
			static inline const juce::String form = u8"Form";
			static inline const juce::String tone = u8"Tone";
			static inline const juce::String noise = u8"Noise";
			static inline const juce::String noiseFreq = u8"Freq";
			static inline const juce::String noiseOnNote = u8"Noise On Note";
			static inline const juce::String mix = u8"Mix";
		}

		namespace Duty
		{
			static inline const juce::String mode = u8"Mode";
			static inline const juce::String preset = u8"Preset";
			static inline const juce::String var = u8"Ratio";
			static inline const juce::String invert = u8"Invert Phrase";
			static inline const juce::String fc = u8"FC Mode";
			static inline const juce::String fcFluc = u8"FC Fluc";
		}

		namespace Tri
		{
			static inline const juce::String keyTrack = u8"Key Track (Pitch)";
			static inline const juce::String peak = u8"Peak";
			static inline const juce::String manualFreq = u8"Freq";
			static inline const juce::String peakTo00 = u8"0.0 (Down)";
			static inline const juce::String peakTo05 = u8"0.5 (Tri)";
			static inline const juce::String peakTo10 = u8"1.0 (Up)";

		}

		namespace HwEnv
		{
			static inline const juce::String enable = u8"Enable";
			static inline const juce::String shape = u8"Shape";
			static inline const juce::String speed = u8"Period";
		}
	}

	namespace Utility
	{
		static inline const juce::String bcLevel = u8"Level -> All Ch";
	}
}
