// プリセットの名前やラベルに関する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Gui/GuiI18n.h"

namespace PresetKey
{
	namespace Search
	{
		static inline const I18n::Text title{ u8"検索", u8"Search" };
		static inline const juce::String clear = "X";
	}

	// 一覧の見方
	// 一覧に出すファイル形式の絞り込み。
	// 読み込みはどの形式でもできるので、これは探しやすさのためのもの。
	namespace Format
	{
		static inline const I18n::Text title{ u8"形式", u8"Format" };
		static inline const I18n::Text all{ u8"すべて", u8"All" };
		static inline const juce::String xml = "XML";
		static inline const juce::String json = "JSON";
		static inline const juce::String yaml = "YAML";
	}
	namespace View
	{
		static inline const I18n::Text title{ u8"表示", u8"Show" };
		static inline const I18n::Text all{ u8"すべて", u8"All" };
		static inline const I18n::Text favourites{ u8"お気に入り", u8"Favourites" };
		static inline const I18n::Text history{ u8"履歴", u8"History" };

		static inline const I18n::Text clearHistory{ u8"履歴を消す", u8"Clear the history" };

		namespace Mark
		{
			// お気に入りの入り切り。押せる印なので文字で出す。
			static inline const juce::String on = juce::String("") + "★";
			static inline const juce::String off = juce::String("") + "☆";
		}

		namespace Dialog
		{
			static inline const I18n::Text clearTitle{ u8"履歴を消す", u8"Clear the history" };
			static inline const I18n::Text clearMessage{ u8"読み込んだプリセットの履歴をすべて消します。よろしいですか？", u8"This clears the whole history of loaded presets. Continue?" };
			static inline const I18n::Text clearOk{ u8"消す", u8"Clear" };
			static inline const I18n::Text clearCancel{ u8"キャンセル", u8"Cancel" };
		}
	}

	// チャンネルでの絞り込み。
	//
	// プリセットは音源をまるごと差し替えるので、目当ての音源のものだけを
	// 見たいことが多い。入っているものだけを出し、ひとつも入っていない
	// ときは絞らない。
	namespace Channel
	{
		static inline const I18n::Text title{ u8"チャンネル", u8"Channel" };
		static inline const I18n::Text use{ u8"対象", u8"Use" };
		static inline const I18n::Text allOn{ u8"すべてON", u8"All on" };
		static inline const I18n::Text allOff{ u8"すべてOFF", u8"All off" };

		// いま対象にしているものの並び。ひとつも無ければ「すべて」。
		static inline const I18n::Text summary{ u8"対象: ", u8"Use: " };
		static inline const I18n::Text summaryNone{ u8"すべて", u8"All" };
	}

	namespace Table
	{
		static inline const I18n::Text title{ u8"プリセット一覧", u8"Presets" };

		namespace ColName
		{
			static inline const juce::String favourite = juce::String("") + "★";
			static inline const I18n::Text genre{ u8"ジャンル", u8"Genre" };
			static inline const I18n::Text format{ u8"形式", u8"Format" };
			static inline const I18n::Text fileName{ u8"ファイル名", u8"File name" };
			static inline const I18n::Text mode{ u8"チャンネル", u8"Channel" };
			static inline const I18n::Text name{ u8"プリセット名", u8"Preset name" };
			static inline const I18n::Text author{ u8"作者名", u8"Author" };
			static inline const I18n::Text version{ u8"バージョン", u8"Version" };
			static inline const I18n::Text lastModified{ u8"最終更新日時", u8"Last modified" };
		}
	}

	namespace MetaData
	{
		static inline const I18n::Text title{ u8"プリセットメタデータ", u8"Preset metadata" };

		namespace Label
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

	namespace Button
	{
		static inline const I18n::Text initPreset{ u8"プリセット初期化", u8"Initialise preset" };
		static inline const I18n::Text loadPreset{ u8"プリセット読み込み", u8"Load preset" };
		static inline const I18n::Text savePreset{ u8"プリセット保存", u8"Save preset" };
		static inline const I18n::Text savePresetAs{ u8"ファイル名を指定してプリセットを保存", u8"Save preset as…" };
		static inline const I18n::Text deletePreset{ u8"プリセット削除", u8"Delete preset" };
		static inline const I18n::Text refleshPresetList{ u8"プリセットリストの更新", u8"Refresh the preset list" };
		static inline const I18n::Text reflectPresetInfo{ u8"選択メタデータを反映", u8"Apply the selected metadata" };
		static inline const I18n::Text copyPresetInfoToClipboard{ u8"クリップボードにコピー", u8"Copy to clipboard" };
	}

	static inline const juce::String name = "presetName";
	static inline const juce::String author = "presetAuthor";
	static inline const juce::String version = "presetVersion";
	static inline const juce::String comment = "presetComment";
	static inline const juce::String genre = "presetGenre";
	static inline const juce::String mode = "activeModeName";
	static inline const juce::String puginVersion = "pluginVersion";

	// どのプラグインで書かれたものか。6 製品はプリセットの印 ("preset") が
	// 同じで、拡張子を変えるだけでは読み違えを止められなかった。
	// 3.1.0 より前のファイルはこれを持たないので、そのときは読ませる。
	static inline const juce::String plugin = "pluginName";
	static inline const juce::String adpcmPath = "adpcmPath";
	static inline const juce::String rhythmPathPrefix = "rhythmPath";
	static inline const juce::String opzx7PathPrefix = "opzx7PcmPath";
	static inline const juce::String opzx7WtPathPrefix = "opzx7WtPath";
	static inline const juce::String opzx7Wt2PathPrefix = "opzx7Wt2Path";
	// チャンネルごとの MODULATION 変調波形パス。キーは APVTS のプレフィックス。
	static inline const juce::String modWavePathPrefix = "modWavePath_";
};
