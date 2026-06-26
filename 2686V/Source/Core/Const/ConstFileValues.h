// ファイルに関するデータを管理

#pragma once

#include <JuceHeader.h>

namespace Io
{
	static inline const juce::String empty = "(Empty)";

	namespace Extension
	{
		static inline const juce::String wavetable = "wt";
		static inline const juce::String wt2 = "wt2";
		static inline const juce::String fxOrder = "fxo";
		static inline const juce::String fxParam = "2fx";
		static inline const juce::String OpnaLfoParam = "lfoOpna";
		static inline const juce::String N88LfoParam = "lfoN88";
		static inline const juce::String OpmLfoParam = "lfoOpm";
		static inline const juce::String OplLfoParam = "lfoOpl";
		static inline const juce::String Opzx7LfoParam = "lfoOpzx7";
		static inline const juce::String AmpEnvParam = "ampEnv";
		static inline const juce::String SsgSwEnvParam = "ssgSwEnv";
		static inline const juce::String PitchEnvParam = "pitchEnv";
		static inline const juce::String DetuneParam = "detune";
		static inline const juce::String UnisonParam = "unison";
		static inline const juce::String QualityParam = "quality";
		static inline const juce::String PcmQualityParam = "pcmQuality";
		static inline const juce::String PcmPlayParam = "pcmPlay";
		static inline const juce::String ToneNoiseParam = "toneNoise";
	};

	namespace ExtensionGlob
	{
		static inline const juce::String wavetable = "*.wt";
		static inline const juce::String wt2 = "*.wt2";
		static inline const juce::String fxOrder = "*.fxo";
		static inline const juce::String fxParam = "*.2fx";
		static inline const juce::String OpnaLfoParam = "*.lfoOpna";
		static inline const juce::String N88LfoParam = "*.lfoN88";
		static inline const juce::String OpmLfoParam = "*.lfoOpm";
		static inline const juce::String OplLfoParam = "*.lfoOpl";
		static inline const juce::String Opzx7LfoParam = "*.lfoOpzx7";
		static inline const juce::String AmpEnvParam = "*.ampEnv";
		static inline const juce::String SsgSwEnvParam = "*.ssgSwEnv";
		static inline const juce::String PitchEnvParam = "*.pitchEnv";
		static inline const juce::String DetuneParam = "*.detune";
		static inline const juce::String UnisonParam = "*.unison";
		static inline const juce::String QualityParam = "*.quality";
		static inline const juce::String PcmQualityParam = "*.pcmQuality";
		static inline const juce::String PcmPlayParam = "*.pcmPlay";
		static inline const juce::String ToneNoiseParam = "*.toneNoise";
	};

	namespace Folder
	{
		static inline const juce::String asset = "2686V";
		static inline const juce::String preset = "Presets";
		static inline const juce::String resource = "Resources";
		static inline const juce::String sample = "Samples";
		static inline const juce::String wavetable = "Wavetables";
		static inline const juce::String fxOrder = "FxOrders";
		static inline const juce::String fxParam = "FxParams";
		static inline const juce::String lfoParam = "LfoParams";
		static inline const juce::String ampEnvParam = "AmpEnvParams";
		static inline const juce::String pitchEnvParam = "PitchEnvParams";
		static inline const juce::String ssgSwEnvParam = "SsgSwEnvParams";
		static inline const juce::String detuneParam = "DetuneParams";
		static inline const juce::String unisonParam = "UnisonParams";
		static inline const juce::String qualityParam = "QualityParams";
		static inline const juce::String pcmPlayParam = "PcmPlayParams";
		static inline const juce::String toneNoiseParam = "ToneNoiseParams";
	};

	namespace Dialog
	{
		namespace Title
		{
			static inline const juce::String openAudioFile = "Select an Audio file";
			static inline const juce::String importWavetableFile = "Import Wavetable";
			static inline const juce::String exportWavetableFile = "Export Wavetable";
			static inline const juce::String importFxOrderFile = "Import FX Order";
			static inline const juce::String exportFxOrderFile = "Export FX Order";
			static inline const juce::String importFxParamFile = "Import FX Param";
			static inline const juce::String exportFxParamFile = "Export FX Param";
			static inline const juce::String importLfoParamFile = "Import LFO Param";
			static inline const juce::String exportLfoParamFile = "Export LFO Param";
			static inline const juce::String importAmpEnvParamFile = "Import Amp Env Param";
			static inline const juce::String exportAmpEnvParamFile = "Export Amp Env Param";
			static inline const juce::String importPitchEnvParamFile = "Import Pitch Env Param";
			static inline const juce::String exportPitchEnvParamFile = "Export Pitch Env Param";
			static inline const juce::String importSsgSwEnvParamFile = "Import SSG SW Env Param";
			static inline const juce::String exportSsgSwEnvParamFile = "Export SSG SW Env Param";
			static inline const juce::String importDetuneParamFile = "Import Detune Param";
			static inline const juce::String exportDetuneParamFile = "Export Detune Param";
			static inline const juce::String importUnisonParamFile = "Import Unison Param";
			static inline const juce::String exportUnisonParamFile = "Export Unison Param";
			static inline const juce::String importQualityParamFile = "Import Quality Param";
			static inline const juce::String exportQualityParamFile = "Export Quality Param";
			static inline const juce::String importPcmPlayParamFile = "Import Pcm Play Param";
			static inline const juce::String exportPcmPlayParamFile = "Export Pcm Play Param";
			static inline const juce::String importToneNoiseParamFile = "Import Tone / Noise Param";
			static inline const juce::String exportToneNoiseParamFile = "Export Tone / Noise Param";
		}
	}
};
