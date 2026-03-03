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
	namespace Label {
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour CategoryText = juce::Colours::yellow;
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
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour Bg = defaultBgColor;
	};
	namespace ToggleButton {
		static inline const juce::Colour Fg = defaultFgColor;
	};
	namespace TextButton {
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour Bg = defaultBgColor;
		static inline const juce::Colour Border = defaultFgColor;
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
			static inline const juce::Colour To1 = juce::Colours::red.withAlpha(0.7f);
			static inline const juce::Colour To05 = juce::Colours::yellow.withAlpha(0.7f);
			static inline const juce::Colour To0 = juce::Colours::green.withAlpha(0.7f);
			static inline const juce::Colour ToM05 = juce::Colours::cyan.withAlpha(0.7f);
			static inline const juce::Colour ToM1 = juce::Colours::blue.withAlpha(0.7f);
		}
	};
	namespace Settings {
		static inline const juce::Colour SaveAsDefaultBtnBg = juce::Colours::green.withAlpha(0.5f);
	};
};
