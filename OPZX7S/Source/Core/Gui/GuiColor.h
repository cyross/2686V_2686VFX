#pragma once

#include <JuceHeader.h>

namespace GuiColor {
	static inline const juce::Colour defaultFgColor = juce::Colours::white.withAlpha(0.8f);
	static inline const juce::Colour defaultBgColor = juce::Colours::transparentBlack;
	namespace Editor {
		static inline const juce::Colour defaultBg = juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0f);
		static inline const juce::Colour wallpaperBg = juce::Colours::black.withAlpha(0.2f);
		static inline const juce::Colour blurWallpaperBg = juce::Colours::black.withAlpha(0.5f);
	};
	namespace Group {
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour Bg = juce::Colours::black.withAlpha(0.4f);
		static inline const juce::Colour Border = defaultFgColor;
	};
	namespace ScrollBar {
		static inline const juce::Colour Thumb = juce::Colours::darkgrey;
	};
	namespace Label {
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour CategoryText = juce::Colours::yellow;
	};
	namespace Category {
		// 80〜90 年代を意識したレトロモダン。文字は黒、背景はカテゴリ色。
		static inline const juce::Colour Text = juce::Colours::black;

		// 黒い文字を載せるので、素の色より明るくして陰にならないようにする。
		static inline const juce::Colour HwBg = juce::Colours::yellow.brighter(0.5f);
		static inline const juce::Colour SwBg = juce::Colours::aqua.brighter(0.5f);
		static inline const juce::Colour OtherBg = juce::Colours::lime.brighter(0.5f);

		// カテゴリを開いたとき、中身の背後へ敷く色。
		static inline const juce::Colour ContentBg = juce::Colours::black.withAlpha(0.5f);

		// 開閉マーカー。閉じているときは黒、開いているときはピンク。
		// ピンクは初音ミクの髪留めやイヤホンの色を参照した差し色。
		static inline const juce::Colour MarkerClosed = juce::Colours::black;
		static inline const juce::Colour MarkerOpen = juce::Colour::fromRGB(0xFF, 0x33, 0x99);

		// マーカーの縁。背景が黄でも水色でも見えるよう常に暗い色を敷く。
		static inline const juce::Colour MarkerBorder = juce::Colours::black.withAlpha(0.6f);
	};
	namespace Slider {
		static inline const juce::Colour Track = defaultFgColor;
		static inline const juce::Colour Thumb = juce::Colour::fromRGB(0x8, 0x8, 0xB3);
		namespace Value {
			static inline const juce::Colour Text = defaultFgColor;
			static inline const juce::Colour Highlight = juce::Colour::fromFloatRGBA(0.7f, 0.7f, 0.03f, 0.2f);
			static inline const juce::Colour Border = defaultFgColor;
		};
	};
	namespace ComboBox {
		static inline const juce::Colour Text = juce::Colours::black.brighter(0.2f);
		static inline const juce::Colour Bg = juce::Colours::white.darker(0.2f);
	};
	namespace ToggleButton {
		static inline const juce::Colour Fg = defaultFgColor;
	};
	namespace TextButton {
		static inline const juce::Colour Text = juce::Colours::black.brighter(0.2f);
		static inline const juce::Colour TextOn = juce::Colours::black;
		static inline const juce::Colour Bg = juce::Colours::white.darker(0.2f);;
		static inline const juce::Colour Border = juce::Colours::black.brighter(0.2f);;
	};
	namespace HyperLink {
		static inline const juce::Colour Fg = juce::Colours::lightblue;
	};
	namespace TableList {
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour SelectedBg = juce::Colours::lightblue.withAlpha(0.5f);
		static inline const juce::Colour RowBg1 = defaultBgColor;
		static inline const juce::Colour RowBg2 = juce::Colours::white.withAlpha(0.1f);
	};
	namespace TextEditor {
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour Bg = defaultBgColor;
		static inline const juce::Colour Border = defaultFgColor;
	};
	namespace WaveformContainer {
		static inline const juce::Colour Track = juce::Colours::white.withAlpha(0.5f);
		static inline const juce::Colour Thumb = juce::Colour::fromRGB(0x8, 0x8, 0xB3);
		namespace ResetBtn {
			static inline const juce::Colour P01 = juce::Colours::white.withAlpha(0.7f);
			static inline const juce::Colour P001 = juce::Colours::grey.withAlpha(0.7f);
			static inline const juce::Colour To1 = juce::Colours::red.withAlpha(0.7f);
			static inline const juce::Colour To05 = juce::Colours::yellow.withAlpha(0.7f);
			static inline const juce::Colour To0 = juce::Colours::green.withAlpha(0.7f);
			static inline const juce::Colour ToM05 = juce::Colours::cyan.withAlpha(0.7f);
			static inline const juce::Colour ToM1 = juce::Colours::blue.withAlpha(0.7f);
			static inline const juce::Colour M001 = juce::Colours::grey.withAlpha(0.7f);
			static inline const juce::Colour M01 = juce::Colours::black.withAlpha(0.7f);
		}
	};
	namespace Waveform2Container {
		static inline const juce::Colour Track = juce::Colours::white.withAlpha(0.5f);
		static inline const juce::Colour Thumb = juce::Colour::fromRGB(0x8, 0x8, 0xB3);
		namespace ResetBtn {
			static inline const juce::Colour ToCenter = juce::Colours::green.withAlpha(0.7f);
			static inline const juce::Colour ToMax = juce::Colours::red.withAlpha(0.7f);
			static inline const juce::Colour To0 = juce::Colours::blue.withAlpha(0.7f);
		}
	};
	namespace Settings {
		static inline const juce::Colour SaveAsDefaultBtnBg = juce::Colours::green.withAlpha(0.5f);
	};
};
