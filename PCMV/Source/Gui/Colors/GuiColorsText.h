#pragma once

#include <JuceHeader.h>

#include "../../Core/Io/ParamFile.h"
#include "../../Core/Const/ConstFileValues.h"

namespace ColorsGuiText
{
	static inline const juce::String title = u8"色の設定";

	namespace Table
	{
		static inline const juce::String swatch = u8"色";
		static inline const juce::String item = u8"項目";
		static inline const juce::String value = u8"設定値";
		static inline const juce::String reset = u8"既定";
	}

	static inline const juce::String hint = u8"行をダブルクリックすると色を変更できます";
	static inline const juce::String resetAll = u8"すべて既定へ戻す";
	static inline const juce::String resetRow = u8"戻す";
	static inline const juce::String load = u8"読み込み";
	static inline const juce::String save = u8"書き出し";

	namespace File
	{
		static inline const juce::String openTitle = u8"色の設定を読み込み";
		static inline const juce::String saveTitle = u8"色の設定を書き出し";
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
		static inline const juce::String title = u8"色の変更";
		static inline const juce::String nameList = u8"色名から選ぶ";
		static inline const juce::String palette = u8"実機のパレットから選ぶ";

		// 色名リストの先頭に置く、何も選んでいない状態
		static inline const juce::String nameNone = u8"(選択なし)";

		static inline const juce::String ownNames = u8"このプラグインの色名";
		static inline const juce::String juceNames = u8"JUCE の色名";
		static inline const juce::String apply = u8"決定";
		static inline const juce::String cancel = u8"キャンセル";
		static inline const juce::String reset = u8"既定へ戻す";

		// 独自に付けた色名についての断り書き。
		// 実機由来の色は実測値だが、キャラクター由来のものは作者の主観なので、
		// 選択肢の末尾に必ずこれを出す。
		static inline const juce::String notice =
			u8"※独自に追加した色名は、作者の個人的なイメージです。"
			u8"公式の設定とは全く関係ございません。関係各所へのお問い合わせはご遠慮ください。";
	}
}
