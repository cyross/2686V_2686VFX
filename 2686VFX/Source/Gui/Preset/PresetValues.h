// プリセットのタブはこのプラグインには無い。
// DAW へ渡す状態の見出しに使うので、名前と初期値だけを残してある。

// プリセットに関する値を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Io/ParamFile.h"

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
		// 3.0.0 より前の XML も一覧に出す。読み込みだけは残してあるため。
		static inline const juce::String glob = "*.86v.json;*.86v.yaml;*.xml";

		// 書き出すときの拡張子。選ばれている形に合わせる。
		static inline juce::String extension()
		{
			return ".86v." + Io::fileFormatExtension();
		}
		static inline const juce::String def = "Untitled";

		namespace Name
		{
			static inline const juce::String initial = "init_preset_origin.xml";

			// お気に入りと履歴の置き場。プラグインごとに分ける。
			// プリセットの置き場も拡張子もプラグインで違うため。
			static inline const juce::String library = "2686VOrigin.library";
		}

		namespace Message
		{
			static inline const juce::String invalidXmlNotice = juce::String("") + "XMLの書式が不正です！";
		}
	}
};
