// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace PresetGuiText
{
	namespace Preset
	{
		namespace Dialog
		{
			static inline const juce::String initPreset = juce::String("") + "プリセット初期化";
			static inline const juce::String initPresetNotice = juce::String("") + "全ての音源とアドバンスドモードの設定値を初期化します。よろしいですか？";
			static inline const juce::String initPresetSuccedBtn = juce::String("") + "初期化";
			static inline const juce::String initPresetCancelBtn = juce::String("") + "キャンセル";

			static inline const juce::String deletePreset = juce::String("") + "プリセット削除";
			static inline const juce::String deletePresetNotice = juce::String("") + "プリセットファイルを削除します。よろしいですか？";
			static inline const juce::String deletePresetSuccedBtn = juce::String("") + "削除";
			static inline const juce::String deletePresetCancelBtn = juce::String("") + "キャンセル";

			static inline const juce::String reflectPresetToolTipMessage = juce::String("") + "プリセットのメタデータをクリップボードにコピーします。";
		}
	}
}
