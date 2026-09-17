#pragma once

#include <JuceHeader.h>

#include "../../Core/Gui/GuiI18n.h"

#include "../../Core/Io/ParamFile.h"
#include "../../Core/Const/ConstFileValues.h"

namespace ColorsGuiText
{
	static inline const I18n::Text title{ u8"色の設定", u8"Colour settings" };

	namespace Table
	{
		static inline const I18n::Text swatch{ u8"色", u8"Colour" };
		static inline const I18n::Text item{ u8"項目", u8"Item" };
		static inline const I18n::Text value{ u8"設定値", u8"Value" };
		static inline const I18n::Text reset{ u8"既定", u8"Default" };
	}

	static inline const I18n::Text hint{ u8"行をダブルクリックすると色を変更できます", u8"Double-click a row to change its colour" };
	static inline const I18n::Text resetAll{ u8"すべて既定へ戻す", u8"Reset all to defaults" };
	static inline const I18n::Text resetRow{ u8"戻す", u8"Reset" };
	static inline const I18n::Text load{ u8"読み込み", u8"Load" };
	static inline const I18n::Text save{ u8"書き出し", u8"Save" };

	namespace File
	{
		static inline const I18n::Text openTitle{ u8"色の設定を読み込み", u8"Load colour settings" };
		static inline const I18n::Text saveTitle{ u8"色の設定を書き出し", u8"Save colour settings" };
		// 拡張子は書き出す形で決まるので、その都度組み立てる
		static inline juce::String defaultName()
		{
			return Io::defaultFileName(Io::Extension::ColorSetting);
		}

		// ファイルの中身を見分けるための印。4 つのプラグインで共通に
		// してあるので、書き出した設定はどれでも読める。
		static inline const juce::String format = "2686V color settings";

		static inline constexpr int version = 1;

		namespace Key
		{
			static inline const juce::String format = "format";
			static inline const juce::String version = "version";
			static inline const juce::String colors = "colors";
		}
	}

	namespace Dialog
	{
		static inline const I18n::Text title{ u8"色の変更", u8"Change colour" };
		static inline const I18n::Text nameList{ u8"色名から選ぶ", u8"Pick by colour name" };
		static inline const I18n::Text palette{ u8"実機のパレットから選ぶ", u8"Pick from a hardware palette" };

		// 色名リストの先頭に置く、何も選んでいない状態
		static inline const I18n::Text nameNone{ u8"(選択なし)", u8"(none)" };

		static inline const I18n::Text ownNames{ u8"このプラグインの色名", u8"This plugin's colour names" };
		static inline const I18n::Text juceNames{ u8"JUCE の色名", u8"JUCE colour names" };
		static inline const I18n::Text apply{ u8"決定", u8"OK" };
		static inline const I18n::Text cancel{ u8"キャンセル", u8"Cancel" };
		static inline const I18n::Text reset{ u8"既定へ戻す", u8"Reset to default" };

		// 独自に付けた色名についての断り書き。
		// 実機由来の色は実測値だが、キャラクター由来のものは作者の主観なので、
		// 選択肢の末尾に必ずこれを出す。
		static inline const I18n::Text notice{
			u8"※独自に追加した色名は、作者の個人的なイメージです。"
			u8"公式の設定とは全く関係ございません。関係各所へのお問い合わせはご遠慮ください。",
			u8"Note: the colour names added here are the author's own impressions. "
			u8"They have nothing to do with any official setting, so please do not ask the parties involved about them."
		};
	}
}
