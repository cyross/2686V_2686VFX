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
		static inline const juce::String pan = u8"Pan";
		static inline const juce::String quality = u8"Bit";
		static inline const juce::String interp = u8"Interp";
		static inline const juce::String tone = u8"T.LV";
		static inline const juce::String noise = u8"N.LV";
		static inline const juce::String noiseFreq = u8"N.FQ";
		static inline const juce::String mix = u8"Mix";
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
		static inline const juce::String lfoFileImport = u8"Import LFO Param (.lfoOpzx7)";
		static inline const juce::String lfoFileExport = u8"Export LFO Param (.lfoOpzx7)";
		static inline const juce::String ampEnvFileImport = u8"Import Amp Env Param (.ampEnv)";
		static inline const juce::String ampEnvFileExport = u8"Export Amp Env Param (.ampEnv)";
		static inline const juce::String pitchEnvFileImport = u8"Import Pitch Env Param (.pitchEnv)";
		static inline const juce::String pitchEnvFileExport = u8"Export Pitch Env Param (.pitchEnv)";
		static inline const juce::String ssgSwEnvFileImport = u8"Import SSG SW Env Param (.ssgSwEnv)";
		static inline const juce::String ssgSwEnvFileExport = u8"Export SSG SW Env Param (.ssgSwEnv)";
		static inline const juce::String detuneFileImport = u8"Import Detune Param (.detune)";
		static inline const juce::String detuneFileExport = u8"Export Detune Param (.detune)";
		static inline const juce::String unisonFileImport = u8"Import Unison Param (.unison)";
		static inline const juce::String unisonFileExport = u8"Export Unison Param (.unison)";
		static inline const juce::String qualityFileImport = u8"Import Quality Param (.pcmQuality)";
		static inline const juce::String qualityFileExport = u8"Export Quality Param (.pcmQuality)";
	}
}
