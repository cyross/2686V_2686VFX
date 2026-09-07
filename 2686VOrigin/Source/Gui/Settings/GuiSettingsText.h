// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace SettingsGuiText
{
	namespace Group
	{
		static inline const juce::String settingEnv = juce::String("") + "環境設定";
	}

	// 作り置きした波形プレビューの全削除
	static inline const juce::String clearWavePreviews = juce::String("") + "プレビュー波形の全削除";
	static inline const juce::String clearWavePreviewsTitle = juce::String("") + "プレビュー波形をすべて消しますか";
	static inline const juce::String clearWavePreviewsCount = juce::String("") + "%d 件あります。消しても、各画面の生成ボタンで作り直せます。";
	static inline const juce::String clearWavePreviewsOk = juce::String("") + "すべて消す";
	static inline const juce::String clearWavePreviewsCancel = juce::String("") + "取り消し";
}
