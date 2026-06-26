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
		static inline const juce::String Level = u8"LV";

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
