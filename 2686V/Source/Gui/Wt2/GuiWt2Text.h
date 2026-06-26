// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace Wt2GuiText
{
	static inline const juce::String bit = u8"BIT";
	static inline const juce::String rate = u8"RATE";

	namespace Category
	{
		static inline const juce::String visibleForm = u8"[■]--- FORM ---";
		static inline const juce::String invisibleForm = u8"[□]--- FORM ---";
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
		static inline const juce::String pmSpeed = u8"P.SP";
		static inline const juce::String amSpeed = u8"A.SP";
		static inline const juce::String pmShape = u8"P.SP";
		static inline const juce::String amShape = u8"A.SP";
		static inline const juce::String syncDelay = u8"SDLY";
		static inline const juce::String amEn = u8"AM Enable";
		static inline const juce::String pmEn = u8"PM Enable";
		static inline const juce::String ams = u8"AMS";
		static inline const juce::String pms = u8"PMS";
		static inline const juce::String amd = u8"AMD";
		static inline const juce::String pmd = u8"PMD";
		static inline const juce::String amSmoothRatio = u8"AMSR";
	}

	namespace Wt
	{
		static inline const juce::String level = u8"LV";
		static inline const juce::String form = u8"FORM";
		static inline const juce::String size = u8"SIZE";
		static inline const juce::String sampleSize = u8"SMSZ";
		static inline const juce::String resolution = u8"RESO";
		static inline const juce::String waveform = u8"WFRM";
		static inline const juce::String fileImport = u8"Import (.wt2)";
		static inline const juce::String fileExport = u8"Export (.wt2)";

		namespace Mod
		{
			static inline const juce::String enable = u8"Enable";
			static inline const juce::String depth = u8"DPTH";
			static inline const juce::String speed = u8"SPED";
		}

		namespace Custom
		{
			static inline const juce::String toCenter = u8"-> Center";
			static inline const juce::String toMax = u8"-> Max";
			static inline const juce::String to0 = u8"-> 0";
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
