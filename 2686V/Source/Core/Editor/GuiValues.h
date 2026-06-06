// UI で使用するデータを管理

#pragma once

#include <JuceHeader.h>

namespace EditorGuiValue
{
	namespace Window
	{
		static inline constexpr int width = 1580;
		static inline constexpr int height = 920;
	}

	namespace WaterMarkLogo
	{
		static inline const juce::String fontFamily = "Times New Roman";
		static inline constexpr float fontSize = 96.0f;
		static inline constexpr float fontAlpha = 0.6f;
	}

	static inline constexpr int TabNumber = 14;

	// 鍵盤UIの高さ
	static inline constexpr int KeyboardHeight = 60;

	namespace Preview
	{
		static inline constexpr int extraWidth = 288;
		static inline constexpr int drawSize = 280;
	}

	namespace Group
	{
		static inline constexpr int TitlePaddingTop = 10;

		namespace Padding
		{
			static inline constexpr int width = 10;
			static inline constexpr int height = 10;
		}
	}
}
