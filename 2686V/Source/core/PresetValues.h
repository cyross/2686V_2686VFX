// プリセットに関する値を管理

#pragma once

#include <JuceHeader.h>

namespace PresetValue
{
	namespace MetaData
	{
		// メタデータの初期値
		namespace Initial
		{
			static inline const juce::String name = "Init Preset";
			static inline const juce::String author = "Anonymous User";
			static inline const juce::String version = "1.0.0";
			static inline const juce::String comment = "";
		} 

		// メタデータの最大長
		namespace Length
		{
			static inline const int name = 512;
			static inline const int author = 128;
			static inline const int version = 64;
			static inline const int comment = 2048;
		}

		// メタデータをクリップボードに貼り付ける際の前置詞
		namespace ClipBoardPrefix
		{
			static inline const juce::String name = "Name: ";
			static inline const juce::String auther = "Author: ";
			static inline const juce::String version = "Ver: ";
			static inline const juce::String comment = "Comment: ";
			static inline const juce::String mode = "Mode: ";
		};
	}

	// ファイルに関する値
	namespace File
	{
		static inline const juce::String glob = "*.xml";
		static inline const juce::String ext = ".xml";
		static inline const juce::String def = "Untitled";

		namespace Name
		{
			static inline const juce::String initial = "init_preset.xml";
		}

		namespace Message
		{
			static inline const juce::String invalidXmlNotice = "Invalid XML";
		}
	}
};
