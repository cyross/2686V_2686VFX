// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace AdpcmGuiText
{
	namespace Category
	{
		static inline const juce::String visibleForm = u8"[■]--- FORM ---";
		static inline const juce::String invisibleForm = u8"[□]--- FORM ---";
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
		static inline const juce::String level = u8"LV";
		static inline const juce::String pan = u8"PAN";
		static inline const juce::String quality = u8"BIT";
		static inline const juce::String rate = u8"RATE";
		static inline const juce::String interp = u8"INTP";
		static inline const juce::String tone = u8"T.LV";
		static inline const juce::String noise = u8"N.LV";
		static inline const juce::String noiseFreq = u8"N.FQ";
		static inline const juce::String toTone = u8"Tone";
		static inline const juce::String toNoise = u8"Noise";
		static inline const juce::String mix = u8"MIX";
		static inline const juce::String loop = u8"LOOP";
		static inline const juce::String loopPointEnable = u8"Loop Point Enable";
		static inline const juce::String loopPointStart = u8"LPST";
		static inline const juce::String loopPointEnd = u8"LPED";
		static inline const juce::String pcmOffset = u8"P.OF";
		static inline const juce::String pcmRatio = u8"P.RT";

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
