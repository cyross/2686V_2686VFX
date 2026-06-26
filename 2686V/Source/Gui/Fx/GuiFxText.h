// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace FxGuiText
{
	namespace Group
	{
		static inline const juce::String mainGroup = juce::String("") + "エフェクター";
		static inline const juce::String fxTremolo = juce::String("") + "トレモロ";
		static inline const juce::String fxVibrato = juce::String("") + "ビブラート";
		static inline const juce::String fxMbc = juce::String("") + "ビットクラッシャー";
		static inline const juce::String fxDelay = juce::String("") + "ディレイ";
		static inline const juce::String fxReverb = juce::String("") + "リバーブ";
		static inline const juce::String fxFilter = juce::String("") + "フィルター";
		static inline const juce::String fxEq3B = juce::String("") + "3バンドイコライザー";
		static inline const juce::String sfcEcho = juce::String("") + "SFCエコー";
	}

	namespace Fx
	{
		static inline const juce::String masterBypass = juce::String("") + "バイパス";
		static inline const juce::String bypass = juce::String("") + "バイパス";
		static inline const juce::String reset = juce::String("") + "パラメータリセット";
		static inline const juce::String mix = u8"MIX";
		static inline const juce::String orderFileImport = u8"[IM]Order";
		static inline const juce::String orderFileExport = u8"[EX]Order";
		static inline const juce::String paramFileImport = u8"[IM]Param";
		static inline const juce::String paramFileExport = u8"[EX]Param";

		namespace Mix
		{
			static inline const juce::String dry = u8"Dry";
			static inline const juce::String mix = u8"50%";
			static inline const juce::String wet = u8"Wet";
		}

		namespace Tremolo
		{
			static inline const juce::String rate = u8"RATE";
			static inline const juce::String depth = u8"DPTH";
		}

		namespace Vibrate
		{
			static inline const juce::String rate = u8"RATE";
			static inline const juce::String depth = u8"DPTH";
		}

		namespace Mbc
		{
			static inline const juce::String bit = u8"BIT";
			static inline const juce::String rate = u8"RATE";
		}

		namespace Delay
		{
			static inline const juce::String time = u8"TIME";
			static inline const juce::String fb = u8"FB";
		}

		namespace Reverb
		{
			static inline const juce::String size = u8"SIZE";
			static inline const juce::String damp = u8"DAMP";
		}

		namespace Filter
		{
			static inline const juce::String type = u8"TYPE";
			static inline const juce::String freq = u8"FQ";
			static inline const juce::String q = u8"Q";
		}

		namespace Eq3b
		{
			static inline const juce::String lowGainDb = u8"LGn";
			static inline const juce::String midFreq = u8"MFq";
			static inline const juce::String midGainDb = u8"MGn";
			static inline const juce::String highGainDb = u8"HGn";
		}

		namespace SfcEcho
		{
			static inline const juce::String time = u8"TIME";
			static inline const juce::String fb = u8"FB";
			static inline const juce::String firCoef0 = u8"FC0";
			static inline const juce::String firCoef1 = u8"FC1";
			static inline const juce::String firCoef2 = u8"FC2";
			static inline const juce::String firCoef3 = u8"FC3";
			static inline const juce::String firCoef4 = u8"FC4";
			static inline const juce::String firCoef5 = u8"FC5";
			static inline const juce::String firCoef6 = u8"FC6";
			static inline const juce::String firCoef7 = u8"FC7";
		}
	}
}
