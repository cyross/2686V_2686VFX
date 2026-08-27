// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace WtPlusGuiText
{
	namespace Category
	{
		static inline const juce::String visibleWave = u8"[■]--- WAVE ---";
		static inline const juce::String invisibleWave = u8"[□]--- WAVE ---";
		static inline const juce::String visibleSlots = u8"[■]--- WAVE MEMORY ---";
		static inline const juce::String invisibleSlots = u8"[□]--- WAVE MEMORY ---";
		static inline const juce::String visibleUtil = u8"[■]--- UTILITY ---";
		static inline const juce::String invisibleUtil = u8"[□]--- UTILITY ---";
	}

	namespace Group
	{
		static inline const juce::String mainGroup = juce::String("") + "拡張ウェーブテーブルチャンネル設定";
		static inline const juce::String waveGroup = juce::String("") + "波形メモリ設定";
	}

	namespace Wt
	{
		static inline const juce::String slot = u8"SLOT";
		static inline const juce::String interpolate = u8"Interpolate";
		static inline const juce::String steps = u8"STEP";

		namespace Slots
		{
			static inline const juce::String wt = u8"WT";
			static inline const juce::String wt2 = u8"W2";
			static inline const juce::String clear = u8"Clear";
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

	namespace SsgSwEnv11
	{
		static inline const juce::String bypass = u8"Bypass";
	}

	namespace SsgSwPEnv11
	{
		static inline const juce::String bypass = u8"Bypass";
	}

	namespace Utility
	{
		static inline const juce::String bcLevel = u8"Broadcast Level";
	}
}
