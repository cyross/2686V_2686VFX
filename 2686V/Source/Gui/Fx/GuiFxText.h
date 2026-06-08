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
	}

	namespace Fx
	{
		static inline const juce::String masterBypass = juce::String("") + "バイパス";
		static inline const juce::String bypass = juce::String("") + "バイパス";
		static inline const juce::String reset = juce::String("") + "パラメータリセット";
		static inline const juce::String mix = u8"Mix";
		
		namespace Mix
		{
			static inline const juce::String dry = u8"Dry";
			static inline const juce::String mix = u8"50%";
			static inline const juce::String wet = u8"Wet";
		}

		namespace Tremolo
		{
			static inline const juce::String rate = u8"Rate";
			static inline const juce::String depth = u8"Depth";
		}

		namespace Vibrate
		{
			static inline const juce::String rate = u8"Rate";
			static inline const juce::String depth = u8"Depth";
		}

		namespace Mbc
		{
			static inline const juce::String bit = u8"Bit";
			static inline const juce::String rate = u8"Rate";
		}

		namespace Delay
		{
			static inline const juce::String time = u8"Time";
			static inline const juce::String fb = u8"Fb";
		}

		namespace Reverb
		{
			static inline const juce::String size = u8"Size";
			static inline const juce::String damp = u8"Damp";
		}

		namespace Filter
		{
			static inline const juce::String type = u8"Type";
			static inline const juce::String freq = u8"Freq";
			static inline const juce::String q = u8"Q";
		}

		namespace Eq3b
		{
			static inline const juce::String lowGainDb = u8"L-Gn";
			static inline const juce::String midFreq = u8"M-Fq";
			static inline const juce::String midGainDb = u8"M-Gn";
			static inline const juce::String highGainDb = u8"H-Gn";
		}
	}
}
