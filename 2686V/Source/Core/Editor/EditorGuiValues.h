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
		static inline constexpr int paddingLeft = 24;
		static inline constexpr int paddingTop = 50;
		static inline constexpr int paddingInnerY = 20;
		static inline constexpr int paddingDrawSpaceY = 5;
		static inline constexpr int extraWidth = 248;
		static inline constexpr int drawWidth = 200;
		static inline constexpr int drawHeight = 100;
		static inline constexpr int labelHeight = 30;
	}

	namespace MiniPreview
	{
		static inline constexpr int paddingLeft = 5;
		static inline constexpr int paddingRight = 5;
		static inline constexpr int paddingTop = 5;
		static inline constexpr int paddingBottom = 5;
		static inline constexpr int paddingInnerX = 10;
		static inline constexpr int paddingInnerY = 5;
		static inline constexpr int paddingDrawSpaceY = 5;
		static inline constexpr int presetLabelWidth = 400;
		static inline constexpr int presetLabelHeight = 40;
		static inline constexpr int modeLabelWidth = 150;
		static inline constexpr int modeLabelHeight = 40;
		static inline constexpr int presetPaddingRight = 10;
		static inline constexpr int drawWidth = 200;
		static inline constexpr int drawHeight = 100;
		static inline constexpr int labelHeight = 20;
		static inline constexpr int logoWidth = 200;
		static inline constexpr int logoHeight = 48;
	}

	namespace MinimumPreview
	{
		static inline constexpr int paddingLeft = 5;
		static inline constexpr int paddingTop = 5;
		static inline constexpr int paddingRight = 5;
		static inline constexpr int paddingBottom = 5;
		static inline constexpr int paddingInnerX = 10;
		static inline constexpr int paddingInnerY = 20;
		static inline constexpr int presetLabelWidth = 400;
		static inline constexpr int presetLabelHeight = 40;
		static inline constexpr int modeLabelWidth = 150;
		static inline constexpr int modeLabelHeight = 40;
		static inline constexpr int presetPaddingRight = 10;
		static inline constexpr int labelHeight = 30;
		static inline constexpr int logoWidth = 200;
		static inline constexpr int logoHeight = 48;
	}

	namespace SystemBtns
	{
		static inline constexpr int paddingTop = 5;
		static inline constexpr int paddingRight = 5;
		static inline constexpr int paddingInnerX = 5;
		static inline constexpr int buttonWidth = 40;
		static inline constexpr int miniButtonWidth = 30;
		static inline constexpr int initButtonWidth = 60;
		static inline constexpr int buttonHeight = 20;
	}

	namespace StateBtns
	{
		static inline constexpr int paddingBottom = 10;
		static inline constexpr int paddingLeft = 10;
		static inline constexpr int paddingInnerX = 10;
		static inline constexpr int width = 30;
		static inline constexpr int height = 20;
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

	namespace Fx
	{
		static inline constexpr int width = 300;
	}
}
