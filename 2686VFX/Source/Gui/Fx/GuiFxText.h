// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

#include "../../Core/Gui/GuiI18n.h"

namespace FxGuiText
{
	// 出力へ掛ける変調。SSG のソフトウェアエンベロープは、
	// どれを止めるかの札を外から渡す作りなので、ここで文言を持つ。
	namespace Mod
	{
		namespace SsgSwEnv
		{
			static inline const I18n::Text bypass{ u8"バイパス", u8"Bypass" };
		}

		namespace SsgSwEnv11
		{
			static inline const I18n::Text bypass{ u8"バイパス", u8"Bypass" };
		}

		namespace PitchEnv
		{
			static inline const I18n::Text bypass{ u8"バイパス", u8"Bypass" };
		}

		namespace SsgSwPEnv11
		{
			static inline const I18n::Text bypass{ u8"バイパス", u8"Bypass" };
		}

		// 実機の搬送波にあたる周波数。実機には無い、こちらで足したもの。
		namespace WtMod
		{
			static inline const I18n::Text baseFreq{ u8"基準周波数", u8"Base frequency" };
		}
	}

	namespace Fx
	{
		// 2686V PCM Bit Crusher。刻みは音源の QUALITY と同じもの。
		namespace Pcm
		{
			static inline const juce::String bit = "BIT";
			static inline const juce::String rate = "RATE";
			static inline const juce::String interp = "INTERP";
		}
	}

	namespace Group
	{
		static inline const I18n::Text mainGroup{ u8"エフェクター", u8"Effects" };
		static inline const I18n::Text fxTremolo{ u8"トレモロ", u8"Tremolo" };
		static inline const I18n::Text fxVibrato{ u8"ビブラート", u8"Vibrato" };
		static inline const I18n::Text fxMbc{ u8"ビットクラッシャー", u8"Bit crusher" };
		static inline const I18n::Text fxDelay{ u8"ディレイ", u8"Delay" };
		static inline const I18n::Text fxReverb{ u8"リバーブ", u8"Reverb" };
		static inline const I18n::Text fxFilter{ u8"フィルター", u8"Filter" };
		static inline const I18n::Text fxEq3B{ u8"3バンドイコライザー", u8"3-band EQ" };
		static inline const I18n::Text sfcEcho{ u8"SFCエコー", u8"SFC echo" };
		static inline const I18n::Text fxPcm{ u8"PCMビットクラッシャー", u8"PCM bit crusher" };
	}

	namespace Fx
	{
		static inline const I18n::Text masterBypass{ u8"バイパス", u8"Bypass" };
		static inline const I18n::Text bypass{ u8"バイパス", u8"Bypass" };
		static inline const I18n::Text reset{ u8"パラメータリセット", u8"Reset parameters" };
		static inline const I18n::Text showRoute{ u8"設定", u8"Order" };
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
			static inline const juce::String depth = u8"DEPTH";
		}

		namespace Vibrate
		{
			static inline const juce::String rate = u8"RATE";
			static inline const juce::String depth = u8"DEPTH";
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
			static inline const juce::String freq = u8"FREQ";
			static inline const juce::String q = u8"Q";
		}

		namespace Eq3b
		{
			static inline const juce::String lowGainDb = u8"LOW.GAIN";
			static inline const juce::String midFreq = u8"MID.FREQ";
			static inline const juce::String midGainDb = u8"MID.GAIN";
			static inline const juce::String highGainDb = u8"HI.GAIN";
		}

		namespace SfcEcho
		{
			static inline const juce::String time = u8"TIME";
			static inline const juce::String fb = u8"FB";
			static inline const juce::String firCoef0 = u8"COEF0";
			static inline const juce::String firCoef1 = u8"COEF1";
			static inline const juce::String firCoef2 = u8"COEF2";
			static inline const juce::String firCoef3 = u8"COEF3";
			static inline const juce::String firCoef4 = u8"COEF4";
			static inline const juce::String firCoef5 = u8"COEF5";
			static inline const juce::String firCoef6 = u8"COEF6";
			static inline const juce::String firCoef7 = u8"COEF7";
		}
	}
}
