// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace SsgGuiText
{
	static inline const juce::String bit = u8"Bit";
	static inline const juce::String rate = u8"Rate";

	namespace Category
	{
		static inline const juce::String visibleForm = u8"[■]--- FORM ---";
		static inline const juce::String invisibleForm = u8"[□]--- FORM ---";
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
		static inline const juce::String pmSpeed = u8"P.SP";
		static inline const juce::String amSpeed = u8"A.SP";
		static inline const juce::String pmShape = u8"P.SH";
		static inline const juce::String amShape = u8"A.SH";
		static inline const juce::String syncDelay = u8"SDLY";
		static inline const juce::String amEn = u8"AM Enable";
		static inline const juce::String pmEn = u8"PM Enable";
		static inline const juce::String ams = u8"AMS";
		static inline const juce::String pms = u8"PMS";
		static inline const juce::String amd = u8"AMD";
		static inline const juce::String pmd = u8"PMD";
		static inline const juce::String amSmoothRatio = u8"AMSR";
	}

	namespace Ssg
	{
		static inline const juce::String level = u8"LV";

		namespace Voice
		{
			static inline const juce::String form = u8"FORM";
			static inline const juce::String tone = u8"T.LV";
			static inline const juce::String noise = u8"N.LV";
			static inline const juce::String noiseFreq = u8"N.FQ";
			static inline const juce::String noiseOnNote = u8"Noise On Note";
			static inline const juce::String mix = u8"MIX";
		}

		namespace Duty
		{
			static inline const juce::String mode = u8"MODE";
			static inline const juce::String preset = u8"PRST";
			static inline const juce::String var = u8"RAT";
			static inline const juce::String invert = u8"Invert Phrase";
			static inline const juce::String fc = u8"FC Mode";
			static inline const juce::String fcFluc = u8"FCFL";
		}

		namespace Tri
		{
			static inline const juce::String keyTrack = u8"Key Track (Pitch)";
			static inline const juce::String peak = u8"PEAK";
			static inline const juce::String manualFreq = u8"FQ";
			static inline const juce::String peakTo00 = u8"0.0 (Down)";
			static inline const juce::String peakTo05 = u8"0.5 (Tri)";
			static inline const juce::String peakTo10 = u8"1.0 (Up)";

		}

		namespace HwEnv
		{
			static inline const juce::String enable = u8"Enable";
			static inline const juce::String shape = u8"SHPE";
			static inline const juce::String speed = u8"PROD";
		}
	}

	namespace Utility
	{
		static inline const juce::String bcLevel = u8"Level -> All Ch";
		static inline const juce::String lfoFileImport = u8"[IM]LFO";
		static inline const juce::String lfoFileExport = u8"[EX]LFO";
		static inline const juce::String ampEnvFileImport = u8"[IM]Amp Env";
		static inline const juce::String ampEnvFileExport = u8"[EX]Amp Env";
		static inline const juce::String pitchEnvFileImport = u8"[IM]Pitch Env";
		static inline const juce::String pitchEnvFileExport = u8"[EX]Pitch Env";
		static inline const juce::String ssgSwEnvFileImport = u8"[IM]SSG SW Env";
		static inline const juce::String ssgSwEnvFileExport = u8"[EX]SSG SW Env";
		static inline const juce::String detuneFileImport = u8"[IM]Detune";
		static inline const juce::String detuneFileExport = u8"[EX]Detune";
		static inline const juce::String unisonFileImport = u8"[IM]Unison";
		static inline const juce::String unisonFileExport = u8"[EX]Unison";
		static inline const juce::String qualityFileImport = u8"[IM]Quality";
		static inline const juce::String qualityFileExport = u8"[EX]Quality";
		static inline const juce::String pcmPlayFileImport = u8"[IM]PCM Play";
		static inline const juce::String pcmPlayFileExport = u8"[EX]PCM Play";
		static inline const juce::String toneNoiseFileImport = u8"[IM]Tone/Noise";
		static inline const juce::String toneNoiseFileExport = u8"[EX]Tone/Noise";
	}
}
