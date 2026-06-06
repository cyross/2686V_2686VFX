// ファイルに関するデータを管理

#pragma once

#include <JuceHeader.h>

namespace Io
{
	static inline const juce::String empty = "(Empty)";

	namespace Extension
	{
		static inline const juce::String wavetable = "wt";
	};

	namespace ExtensionGlob
	{
		static inline const juce::String wavetable = "*.wt";
	};

	namespace Folder
	{
		static inline const juce::String asset = "2686V";
		static inline const juce::String preset = "Presets";
		static inline const juce::String resource = "Resources";
		static inline const juce::String sample = "Samples";
		static inline const juce::String wavetable = "Wavetables";
	};

	namespace Dialog
	{
		namespace Title
		{
			static inline const juce::String openAudioFile = "Select an Audio file";
			static inline const juce::String importWavetableFile = "Import Wavetable";
			static inline const juce::String exportWavetableFile = "Export Wavetable";
		}
	}
};
