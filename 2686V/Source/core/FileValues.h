// ファイルに関するデータを管理

#pragma once

#include <JuceHeader.h>

namespace Io
{
	static inline const juce::String empty = "(Empty)";

	namespace Folder
	{
		static inline const juce::String asset = "2686V";
		static inline const juce::String preset = "Presets";
		static inline const juce::String sample = "Samples";
	};

	namespace Dialog
	{
		namespace Title
		{
			static inline const juce::String openAudioFile = "Select an Audio file";
		}
	}
};
