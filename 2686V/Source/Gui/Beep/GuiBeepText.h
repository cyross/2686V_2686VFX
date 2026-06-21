// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace BeepGuiText
{
	namespace Category
	{
		static inline const juce::String visibleUtil = u8"[■]--- UTILITY ---";
		static inline const juce::String invisibleUtil = u8"[□]--- UTILITY ---";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = juce::String("") + "ビープ音チャンネル設定";
	}

	namespace Beep
	{
		static inline const juce::String Level = u8"Level";

		namespace Adsr
		{
			static inline const juce::String Bypass = u8"Bypass";
		}
	}

	namespace PitchAdsr
	{
		static inline const juce::String bypass = u8"Bypass";
	}

	namespace SsgSwEnv
	{
		static inline const juce::String bypass = u8"Bypass";
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
	}
}
