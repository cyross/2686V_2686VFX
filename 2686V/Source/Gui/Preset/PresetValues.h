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
			static inline const juce::String name = "";
			static inline const juce::String author = "";
			static inline const juce::String version = "1.0.0";
			static inline const juce::String comment = "";
			static inline const juce::String mode = "-";
			static inline const juce::String genre = "";
		}

		// メタデータの最大長
		namespace Length
		{
			static inline const int name = 512;
			static inline const int author = 128;
			static inline const int version = 64;
			static inline const int comment = 4096;
			static inline const int genre = 128;
		}

		// メタデータをクリップボードに貼り付ける際の前置詞
		namespace ClipBoardPrefix
		{
			static inline const juce::String name = juce::String("") + "プリセット名: ";
			static inline const juce::String auther = juce::String("") + "作者名: ";
			static inline const juce::String version = juce::String("") + "バージョン: ";
			static inline const juce::String comment = juce::String("") + "コメント: ";
			static inline const juce::String mode = juce::String("") + "チャンネル: ";
			static inline const juce::String genre = juce::String("") + "ジャンル: ";
			static inline const juce::String filePath = juce::String("") + "ファイルパス: ";
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
			static inline const juce::String invalidXmlNotice = juce::String("") + "XMLの書式が不正です！";
		}
	}
};
