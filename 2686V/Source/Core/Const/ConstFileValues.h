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
		static inline const juce::String wavetable = "wt";
		static inline const juce::String wt2 = "wt2";
		static inline const juce::String fxOrder = "fxo";
		static inline const juce::String fxParam = "2fx";
		static inline const juce::String opnaParam = "param.opna";
		static inline const juce::String opnParam = "param.opn";
		static inline const juce::String oplParam = "param.opl";
		static inline const juce::String opl3Param = "param.opl3";
		static inline const juce::String opmParam = "param.opm";
		static inline const juce::String opzx7sParam = "param.opzx7s";
		static inline const juce::String ssgParam = "param.ssg";
		static inline const juce::String wtParam = "param.wt";
		static inline const juce::String wt2Param = "param.wt2";
		static inline const juce::String wtPlusParam = "param.wtplus";
		static inline const juce::String rhythmParam = "param.rhythm";
		static inline const juce::String adpcmParam = "param.adpcm";
		static inline const juce::String beepParam = "param.beep";
		static inline const juce::String opnaOpParam = "param.opna.op";
		static inline const juce::String opnOpParam = "param.opn.op";
		static inline const juce::String oplOpParam = "param.opl.op";
		static inline const juce::String opl3OpParam = "param.opl3.op";
		static inline const juce::String opmOpParam = "param.opm.op";
		static inline const juce::String opzx7sOpParam = "param.opzx7s.op";
		static inline const juce::String rhythmPadParam = "param.rhythm.pad";
		static inline const juce::String curveParam = "curve_";
		// オペレータごとの HW LFO。音源全体の N88 LFO とは別物なので、
		// 名前でも見分けられるようにしてある。
		static inline const juce::String OpnaHwLfoParam = "opnaHwLfo";
		static inline const juce::String N88LfoParam = "lfoN88";
		static inline const juce::String OpmLfoParam = "lfoOpm";
		static inline const juce::String OplLfoParam = "lfoOpl";
		static inline const juce::String Opzx7LfoParam = "lfoOpzx7";
		static inline const juce::String AmpEnvParam = "ampEnv";
		static inline const juce::String SsgHwEnvParam = "ssgHwEnv";
		static inline const juce::String WtModParam = "wtmod";
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
		static inline const juce::String wavetable = "*.wt";
		static inline const juce::String wt2 = "*.wt2";
		static inline const juce::String fxOrder = "*.fxo.json;*.fxo.yaml";
		static inline const juce::String fxParam = "*.2fx.json;*.2fx.yaml";
		static inline const juce::String opnaParam = "*.param.opna.json;*.param.opna.yaml";
		static inline const juce::String opnParam = "*.param.opn.json;*.param.opn.yaml";
		static inline const juce::String oplParam = "*.param.opl.json;*.param.opl.yaml";
		static inline const juce::String opl3Param = "*.param.opl3.json;*.param.opl3.yaml";
		static inline const juce::String opmParam = "*.param.opm.json;*.param.opm.yaml";
		static inline const juce::String opzx7sParam = "*.param.opzx7s.json;*.param.opzx7s.yaml";
		static inline const juce::String ssgParam = "*.param.ssg.json;*.param.ssg.yaml";
		static inline const juce::String wtParam = "*.param.wt.json;*.param.wt.yaml";
		static inline const juce::String wt2Param = "*.param.wt2.json;*.param.wt2.yaml";
		static inline const juce::String wtPlusParam = "*.param.wtplus.json;*.param.wtplus.yaml";
		static inline const juce::String rhythmParam = "*.param.rhythm.json;*.param.rhythm.yaml";
		static inline const juce::String adpcmParam = "*.param.adpcm.json;*.param.adpcm.yaml";
		static inline const juce::String beepParam = "*.param.beep.json;*.param.beep.yaml";
		static inline const juce::String opnaOpParam = "*.param.opna.op.json;*.param.opna.op.yaml";
		static inline const juce::String opnOpParam = "*.param.opn.op.json;*.param.opn.op.yaml";
		static inline const juce::String oplOpParam = "*.param.opl.op.json;*.param.opl.op.yaml";
		static inline const juce::String opl3OpParam = "*.param.opl3.op.json;*.param.opl3.op.yaml";
		static inline const juce::String opmOpParam = "*.param.opm.op.json;*.param.opm.op.yaml";
		static inline const juce::String opzx7sOpParam = "*.param.opzx7s.op.json;*.param.opzx7s.op.yaml";
		static inline const juce::String rhythmPadParam = "*.param.rhythm.pad.json;*.param.rhythm.pad.yaml";
		static inline const juce::String OpnaHwLfoParam = "*.opnaHwLfo.json;*.opnaHwLfo.yaml";
		static inline const juce::String N88LfoParam = "*.lfoN88.json;*.lfoN88.yaml";
		static inline const juce::String OpmLfoParam = "*.lfoOpm.json;*.lfoOpm.yaml";
		static inline const juce::String OplLfoParam = "*.lfoOpl.json;*.lfoOpl.yaml";
		static inline const juce::String Opzx7LfoParam = "*.lfoOpzx7.json;*.lfoOpzx7.yaml";
		static inline const juce::String AmpEnvParam = "*.ampEnv.json;*.ampEnv.yaml";
		static inline const juce::String SsgHwEnvParam = "*.ssgHwEnv.json;*.ssgHwEnv.yaml";
		static inline const juce::String WtModParam = "*.wtmod.json;*.wtmod.yaml";
		static inline const juce::String SsgSwEnvParam = "*.ssgSwEnv.json;*.ssgSwEnv.yaml";
		static inline const juce::String PitchEnvParam = "*.pitchEnv.json;*.pitchEnv.yaml";
		static inline const juce::String SsgSwEnvParam11 = "*.ssgSwEnv11.json;*.ssgSwEnv11.yaml";
		static inline const juce::String SsgSwPEnvParam11 = "*.ssgSwPEnv11.json;*.ssgSwPEnv11.yaml";
		static inline const juce::String DetuneParam = "*.detune.json;*.detune.yaml";
		static inline const juce::String UnisonParam = "*.unison.json;*.unison.yaml";
		static inline const juce::String QualityParam = "*.quality.json;*.quality.yaml";
		static inline const juce::String PcmQualityParam = "*.pcmQuality.json;*.pcmQuality.yaml";
		static inline const juce::String PcmPlayParam = "*.pcmPlay.json;*.pcmPlay.yaml";
		static inline const juce::String ToneNoiseParam = "*.toneNoise.json;*.toneNoise.yaml";
		static inline const juce::String ColorSetting = "*.ccolor.json;*.ccolor.yaml";
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
		static inline const juce::String channelParam = "ChannelParams";
		static inline const juce::String curveParam = "CurveParams";
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
			static inline const juce::String importWavetableFile = "Import Wavetable";
			static inline const juce::String exportWavetableFile = "Export Wavetable";
			static inline const juce::String importFxOrderFile = "Import FX Order";
			static inline const juce::String exportFxOrderFile = "Export FX Order";
			static inline const juce::String importFxParamFile = "Import FX Param";
			static inline const juce::String exportFxParamFile = "Export FX Param";
			static inline const juce::String importChannelParamFile = "Import Channel Param";
			static inline const juce::String exportChannelParamFile = "Export Channel Param";
			static inline const juce::String importCurveParamFile = "Import Curve Param";
			static inline const juce::String exportCurceParamFile = "Export Curve Param";
			static inline const juce::String importLfoParamFile = "Import LFO Param";
			static inline const juce::String exportLfoParamFile = "Export LFO Param";
			static inline const juce::String importAmpEnvParamFile = "Import Amp Env Param";
			static inline const juce::String exportAmpEnvParamFile = "Export Amp Env Param";
			static inline const juce::String importPitchEnvParamFile = "Import Pitch Env Param";
			static inline const juce::String exportPitchEnvParamFile = "Export Pitch Env Param";
			static inline const juce::String importSsgHwEnvParamFile = "Import SSG HW Env Param";
			static inline const juce::String importWtModParamFile = "Import Modulation Param";
			static inline const juce::String exportSsgHwEnvParamFile = "Export SSG HW Env Param";
			static inline const juce::String exportWtModParamFile = "Export Modulation Param";
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
