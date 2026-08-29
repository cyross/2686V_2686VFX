#pragma once

#include <JuceHeader.h>

namespace ColorsGuiText
{
	static inline const juce::String title = u8"色の設定";

	namespace Table
	{
		static inline const juce::String swatch = u8"色";
		static inline const juce::String item = u8"項目";
		static inline const juce::String value = u8"設定値";
	}

	static inline const juce::String hint = u8"行をダブルクリックすると色を変更できます";
	static inline const juce::String resetAll = u8"すべて既定へ戻す";

	namespace Dialog
	{
		static inline const juce::String title = u8"色の変更";
		static inline const juce::String nameList = u8"色名から選ぶ";
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
