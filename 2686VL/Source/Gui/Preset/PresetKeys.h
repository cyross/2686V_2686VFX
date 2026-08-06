// プリセットの名前やラベルに関する文字列を管理

#pragma once

#include <JuceHeader.h>

namespace PresetKey
{
	namespace Search
	{
		static inline const juce::String title = juce::String("") + "検索";
		static inline const juce::String clear = "X";
	}

	namespace Table
	{
		static inline const juce::String title = juce::String("") + "プリセット一覧";

		namespace ColName
		{
			static inline const juce::String genre = juce::String("") + "ジャンル";
			static inline const juce::String fileName = juce::String("") + "ファイル名";
			static inline const juce::String mode = juce::String("") + "チャンネル";
			static inline const juce::String name = juce::String("") + "プリセット名";
			static inline const juce::String author = juce::String("") + "作者名";
			static inline const juce::String version = juce::String("") + "バージョン";
			static inline const juce::String lastModified = juce::String("") + "最終更新日時";
		}
	}

	namespace MetaData
	{
		static inline const juce::String title = juce::String("") + "プリセットメタデータ";

		namespace Label
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

	namespace Button
	{
		static inline const juce::String initPreset = juce::String("") + "プリセット初期化";
		static inline const juce::String loadPreset = juce::String("") + "プリセット読み込み";
		static inline const juce::String savePreset = juce::String("") + "プリセット保存";
		static inline const juce::String savePresetAs = juce::String("") + "ファイル名を指定してプリセットを保存";
		static inline const juce::String deletePreset = juce::String("") + "プリセット削除";
		static inline const juce::String refleshPresetList = juce::String("") + "プリセットリストの更新";
		static inline const juce::String reflectPresetInfo = juce::String("") + "選択メタデータを反映";
		static inline const juce::String copyPresetInfoToClipboard = juce::String("") + "クリップボードにコピー";
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
	static inline const juce::String opzx7PathPrefix = "opzx7PcmPath";
	static inline const juce::String opzx7WtPathPrefix = "opzx7WtPath";
	static inline const juce::String opzx7Wt2PathPrefix = "opzx7Wt2Path";
};
