// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace EditorGuiText
{
	namespace Tab
	{
		static inline const juce::String opna = u8"OPNA";
		static inline const juce::String opn = u8"OPN";
		static inline const juce::String opl = u8"OPL";
		static inline const juce::String opl3 = u8"OPL3";
		static inline const juce::String opm = u8"OPM";
		static inline const juce::String opzx7 = u8"OPZX7";
		static inline const juce::String ssg = u8"SSG";
		static inline const juce::String wt = u8"WT";
		static inline const juce::String rhythm = u8"RHYTHM";
		static inline const juce::String adpcm = u8"ADPCM";
		static inline const juce::String beep = u8"BEEP";
		static inline const juce::String fx = u8"FX";
		static inline const juce::String preset = u8"PRESET";
		static inline const juce::String settings = u8"SETTINGS";
		static inline const juce::String about = u8"ABOUT";
		static inline const juce::String advanced = u8"ADVANCED";
	}

	namespace Preview
	{
		static inline const juce::String show = u8">>";
		static inline const juce::String hide = u8"<<";
		static inline const juce::String tooltipShow = u8"Hide Preview";
		static inline const juce::String tooltipHide = u8"Show Preview";
	}

	namespace Panic
	{
		static inline const juce::String title = u8"!";
		static inline const juce::String tooltip = u8"Reset Audio Engine";
	}

	namespace Undo
	{
		static inline const juce::String title = u8"Un";
		static inline const juce::String tooltip = u8"Undo";
		static inline const juce::String tooltipNone = u8"Nothing to undo";
	}

	namespace Redo
	{
		static inline const juce::String title = u8"Re";
		static inline const juce::String tooltip = u8"Redo";
		static inline const juce::String tooltipNone = u8"Nothing to Redo";
	}

	namespace Copy
	{
		static inline const juce::String title = u8"C";
		static inline const juce::String tooltip = u8"Copy FM Parameters";
	}

	namespace Paste
	{
		static inline const juce::String title = u8"P";
		static inline const juce::String tooltip = u8"Paste FM Parameters";
	}

	namespace Reset
	{
		static inline const juce::String title = u8"Reset";
		static inline const juce::String tooltip = u8"Reset Parameters";
	}
}
