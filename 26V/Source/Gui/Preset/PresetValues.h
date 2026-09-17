// プリセットに関する値を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Gui/GuiI18n.h"

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
			static inline const I18n::Text name{ u8"プリセット名: ", u8"Preset name: " };
			static inline const I18n::Text auther{ u8"作者名: ", u8"Author: " };
			static inline const I18n::Text version{ u8"バージョン: ", u8"Version: " };
			static inline const I18n::Text comment{ u8"コメント: ", u8"Comment: " };
			static inline const I18n::Text mode{ u8"チャンネル: ", u8"Channel: " };
			static inline const I18n::Text genre{ u8"ジャンル: ", u8"Genre: " };
			static inline const I18n::Text filePath{ u8"ファイルパス: ", u8"File path: " };
		};
	}

	// ファイルに関する値
	namespace File
	{
		// 3.0.0 より前の XML も一覧に出す。読み込みだけは残してあるため。
		static inline const juce::String glob = "*.26v.json;*.26v.yaml;*.xml";

		// 書き出すときの拡張子。選ばれている形に合わせる。
		static inline juce::String extension()
		{
			return ".26v." + Io::fileFormatExtension();
		}
		static inline const juce::String def = "Untitled";

		namespace Name
		{
			static inline const juce::String initial = "init_preset_26v.xml";

			// お気に入りと履歴の置き場。プラグインごとに分ける。
			// プリセットの置き場も拡張子もプラグインで違うため。
			static inline const juce::String library = "26V.library";
		}

		namespace Message
		{
			static inline const I18n::Text invalidXmlNotice{ u8"XMLの書式が不正です！", u8"The XML is malformed." };
		}
	}
};
