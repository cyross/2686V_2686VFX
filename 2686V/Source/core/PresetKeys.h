// プリセットの名前やラベルに関する文字列を管理

#pragma once

#include <JuceHeader.h>

namespace PresetKey
{
	namespace Search
	{
		static inline const juce::String title = "Search";
		static inline const juce::String clear = "X";
	}

	namespace Table
	{
		static inline const juce::String title = "Presets";

		namespace ColName
		{
			static inline const juce::String genre = "Genre";
			static inline const juce::String fileName = "File Name";
			static inline const juce::String mode = "Mode";
			static inline const juce::String name = "Preset Name";
			static inline const juce::String author = "Author";
			static inline const juce::String version = "Version";
			static inline const juce::String lastModified = "Last Modified";
		}
	}

	namespace MetaData
	{
		static inline const juce::String title = "Preset Info";

		namespace Label
		{
			static inline const juce::String name = "Name: ";
			static inline const juce::String auther = "Author: ";
			static inline const juce::String version = "Ver: ";
			static inline const juce::String comment = "Comment: ";
			static inline const juce::String mode = "Mode: ";
		};
	}

	namespace Button
	{
		static inline const juce::String initPreset = "Init Preset";
		static inline const juce::String loadPreset = "Load Preset";
		static inline const juce::String savePreset = "Save Preset";
		static inline const juce::String deletePreset = "Delete Preset";
		static inline const juce::String refleshPresetList = "Refresh Preset List";
		static inline const juce::String reflectPresetInfo = "Reflect Preset Info";
		static inline const juce::String copyPresetInfoToClipboard = "Copy Preset Info to Clipboard";
	}

	static inline const juce::String name = "presetName";
	static inline const juce::String author = "presetAuthor";
	static inline const juce::String version = "presetVersion";
	static inline const juce::String comment = "presetComment";
	static inline const juce::String genre = "presetGenre";
	static inline const juce::String mode = "activeModeName";
	static inline const juce::String puginVersion = "pluginVersion";
	static inline const juce::String adpcmPath = "adpcmPath";
	static inline const juce::String rhythmPathPrefix = "rhythmPath";
	static inline const juce::String opzx3PathPrefix = "opzx3PcmPath";
};
