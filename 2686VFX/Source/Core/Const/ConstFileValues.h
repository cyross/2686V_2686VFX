// ファイルに関するデータを管理

#pragma once

#include <JuceHeader.h>

namespace Io
{
	static inline const juce::String empty = "(Empty)";

	// 表示用の「無し」をファイル名として扱わないための判定。
	//
	// juce::File は絶対パスでない文字列を渡されると止まる。"(Empty)" の
	// ような表示文字をそのまま読み込みへ回してはいけない。
	static inline bool isFileName(const juce::String& text)
	{
		return text.isNotEmpty() && text != empty;
	}

	// 書き出すときは表示用の「無し」を残さない
	static inline juce::String toStoredFileName(const juce::String& text)
	{
		return isFileName(text) ? text : juce::String();
	}

	// パラメータファイルに書かれた音声ファイルの場所を、実際に開ける形へ直す。
	//
	// 配布するパラメータファイルは、置き場所が人それぞれなので絶対パスを
	// 書けない。Samples フォルダからの相対で書いておき、読むときにここで
	// 組み立てる。絶対パスで書かれたものはそのまま返す。
	static inline juce::String resolveSamplePath(const juce::String& text, const juce::String& sampleDir)
	{
		if (!isFileName(text)) return text;
		if (juce::File::isAbsolutePath(text)) return text;
		if (sampleDir.isEmpty() || !juce::File::isAbsolutePath(sampleDir)) return text;

		return juce::File(sampleDir).getChildFile(text).getFullPathName();
	}

	// juce::File を作ってよい文字列かどうか。
	//
	// 画面のラベルはファイル名だけを出しているので、そこから File を
	// 作ってはいけない。場所はプロセッサが持っているものを使うこと。
	static inline bool isFilePath(const juce::String& text)
	{
		return isFileName(text) && juce::File::isAbsolutePath(text);
	}

	namespace Extension
	{
		static inline const juce::String fxOrder = "fxo";
		static inline const juce::String fxParam = "2fx";
		static inline const juce::String opnaParam = "param.opna";
		static inline const juce::String opnParam = "param.opn";
		static inline const juce::String ssgParam = "param.ssg";
		static inline const juce::String rhythmParam = "param.rhythm";
		static inline const juce::String adpcmParam = "param.adpcm";
		static inline const juce::String opnaOpParam = "param.opna.op";
		static inline const juce::String opnOpParam = "param.opn.op";
		static inline const juce::String rhythmPadParam = "param.rhythm.pad";
		// オペレータごとの HW LFO。音源全体の N88 LFO とは別物なので、
		// 名前でも見分けられるようにしてある。
		static inline const juce::String OpnaHwLfoParam = "opnaHwLfo";
		static inline const juce::String N88LfoParam = "lfoN88";
		static inline const juce::String Opzx7LfoParam = "lfoOpzx7";
		static inline const juce::String AmpEnvParam = "ampEnv";
		static inline const juce::String SsgHwEnvParam = "ssgHwEnv";
		static inline const juce::String SsgHwPEnvParam = "ssgHwPEnv";
		static inline const juce::String WtModParam = "wtmod";
		static inline const juce::String WtAmpModParam = "wtampmod";
		static inline const juce::String SsgSwEnvParam = "ssgSwEnv";
		static inline const juce::String SsgSwEnvParam11 = "ssgSwEnv11";
		static inline const juce::String SsgSwPEnvParam11 = "ssgSwPEnv11";
		static inline const juce::String PitchEnvParam = "pitchEnv";
		static inline const juce::String DetuneParam = "detune";
		static inline const juce::String UnisonParam = "unison";
		static inline const juce::String QualityParam = "quality";
		static inline const juce::String PcmQualityParam = "pcmQuality";
		static inline const juce::String PcmPlayParam = "pcmPlay";
		static inline const juce::String ToneNoiseParam = "toneNoise";

		static inline const juce::String ColorSetting = "ccolor";
	};

	namespace ExtensionGlob
	{
		static inline const juce::String fxOrder = "*.fxo.json;*.fxo.yaml;*.fxo";
		static inline const juce::String fxParam = "*.2fx.json;*.2fx.yaml;*.2fx";
		static inline const juce::String opnaParam = "*.param.opna.json;*.param.opna.yaml;*.param.opna";
		static inline const juce::String opnParam = "*.param.opn.json;*.param.opn.yaml;*.param.opn";
		static inline const juce::String ssgParam = "*.param.ssg.json;*.param.ssg.yaml;*.param.ssg";
		static inline const juce::String rhythmParam = "*.param.rhythm.json;*.param.rhythm.yaml;*.param.rhythm";
		static inline const juce::String adpcmParam = "*.param.adpcm.json;*.param.adpcm.yaml;*.param.adpcm";
		static inline const juce::String opnaOpParam = "*.param.opna.op.json;*.param.opna.op.yaml;*.param.opna.op";
		static inline const juce::String rhythmPadParam = "*.param.rhythm.pad.json;*.param.rhythm.pad.yaml;*.param.rhythm.pad";
		static inline const juce::String OpnaHwLfoParam = "*.opnaHwLfo.json;*.opnaHwLfo.yaml;*.opnaHwLfo";
		static inline const juce::String opnOpParam = "*.param.opn.op.json;*.param.opn.op.yaml;*.param.opn.op";
		static inline const juce::String N88LfoParam = "*.lfoN88.json;*.lfoN88.yaml;*.lfoN88";
		static inline const juce::String Opzx7LfoParam = "*.lfoOpzx7.json;*.lfoOpzx7.yaml;*.lfoOpzx7";
		static inline const juce::String AmpEnvParam = "*.ampEnv.json;*.ampEnv.yaml;*.ampEnv";
		static inline const juce::String SsgHwEnvParam = "*.ssgHwEnv.json;*.ssgHwEnv.yaml;*.ssgHwEnv";
		static inline const juce::String SsgHwPEnvParam = "*.ssgHwPEnv.json;*.ssgHwPEnv.yaml;*.ssgHwPEnv";
		static inline const juce::String WtModParam = "*.wtmod.json;*.wtmod.yaml;*.wtmod";
		static inline const juce::String WtAmpModParam = "*.wtampmod.json;*.wtampmod.yaml;*.wtampmod";
		static inline const juce::String SsgSwEnvParam = "*.ssgSwEnv.json;*.ssgSwEnv.yaml;*.ssgSwEnv";
		static inline const juce::String PitchEnvParam = "*.pitchEnv.json;*.pitchEnv.yaml;*.pitchEnv";
		static inline const juce::String SsgSwEnvParam11 = "*.ssgSwEnv11.json;*.ssgSwEnv11.yaml;*.ssgSwEnv11";
		static inline const juce::String SsgSwPEnvParam11 = "*.ssgSwPEnv11.json;*.ssgSwPEnv11.yaml;*.ssgSwPEnv11";
		static inline const juce::String DetuneParam = "*.detune.json;*.detune.yaml;*.detune";
		static inline const juce::String UnisonParam = "*.unison.json;*.unison.yaml;*.unison";
		static inline const juce::String QualityParam = "*.quality.json;*.quality.yaml;*.quality";
		static inline const juce::String PcmQualityParam = "*.pcmQuality.json;*.pcmQuality.yaml;*.pcmQuality";
		static inline const juce::String PcmPlayParam = "*.pcmPlay.json;*.pcmPlay.yaml;*.pcmPlay";
		static inline const juce::String ToneNoiseParam = "*.toneNoise.json;*.toneNoise.yaml;*.toneNoise";
		static inline const juce::String ColorSetting = "*.ccolor.json;*.ccolor.yaml;*.ccolor";
	};

	namespace Folder
	{
		static inline const juce::String asset = "2686V";
		static inline const juce::String preset = "2686VOPresets";
		static inline const juce::String resource = "Resources";
		static inline const juce::String sample = "Samples";
		static inline const juce::String fxOrder = "FxOrders";
		static inline const juce::String fxParam = "FxParams";
		static inline const juce::String channelParam = "ChannelParams";
		static inline const juce::String lfoParam = "LfoParams";
		static inline const juce::String ampEnvParam = "AmpEnvParams";
		static inline const juce::String pitchEnvParam = "PitchEnvParams";
		static inline const juce::String ssgHwEnvParam = "SsgHwEnvParams";
		static inline const juce::String ssgSwEnvParam = "SsgSwEnvParams";
		static inline const juce::String detuneParam = "DetuneParams";
		static inline const juce::String unisonParam = "UnisonParams";
		static inline const juce::String qualityParam = "QualityParams";
		static inline const juce::String pcmPlayParam = "PcmPlayParams";
		static inline const juce::String toneNoiseParam = "ToneNoiseParams";
		static inline const juce::String colorSetting = "CustomizeColorSettings";
	};

	namespace Dialog
	{
		namespace Title
		{
			static inline const juce::String openAudioFile = "Select an Audio file";
			static inline const juce::String importFxOrderFile = "Import FX Order";
			static inline const juce::String exportFxOrderFile = "Export FX Order";
			static inline const juce::String importFxParamFile = "Import FX Param";
			static inline const juce::String exportFxParamFile = "Export FX Param";
			static inline const juce::String importChannelParamFile = "Import Channel Param";
			static inline const juce::String exportChannelParamFile = "Export Channel Param";
			static inline const juce::String importLfoParamFile = "Import LFO Param";
			static inline const juce::String exportLfoParamFile = "Export LFO Param";
			static inline const juce::String importAmpEnvParamFile = "Import Amp Env Param";
			static inline const juce::String exportAmpEnvParamFile = "Export Amp Env Param";
			static inline const juce::String importPitchEnvParamFile = "Import Pitch Env Param";
			static inline const juce::String exportPitchEnvParamFile = "Export Pitch Env Param";
			static inline const juce::String importSsgHwEnvParamFile = "Import SSG HW Env Param";
			static inline const juce::String importWtModParamFile = "Import Modulation Param";
			static inline const juce::String exportSsgHwEnvParamFile = "Export SSG HW Env Param";
			static inline const juce::String importSsgHwPEnvParamFile = "Import SSG HW Pitch Env Param";
			static inline const juce::String exportSsgHwPEnvParamFile = "Export SSG HW Pitch Env Param";
			static inline const juce::String exportWtModParamFile = "Export Modulation Param";
			static inline const juce::String importWtAmpModParamFile = "Import Amp Modulation Param";
			static inline const juce::String exportWtAmpModParamFile = "Export Amp Modulation Param";
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
