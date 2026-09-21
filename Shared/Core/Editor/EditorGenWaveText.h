#pragma once

#include <JuceHeader.h>

#include "../Gui/GuiI18n.h"

// ============================================================================
// 生成波形 (GenWave) の文言
// ============================================================================
// 生成波形の部品とパラメータのブラウザーは 12 本で共有するので、文言も共有する。
namespace EditorGuiText
{
	namespace GenWave
	{
		static inline const I18n::Text title{ u8"生成波形", u8"Generated waveform" };
		static inline const I18n::Text generate{ u8"生成", u8"Generate" };
		static inline const I18n::Text regenerate{ u8"再生成", u8"Regenerate" };
		static inline const I18n::Text remove{ u8"削除", u8"Delete" };
		static inline const I18n::Text cycles{ u8"周期", u8"Cycles" };

		// 3 段の見出し。リアルタイムのオシロと同じ並び。
		static inline const juce::String channelL = "L";
		static inline const juce::String channelM = "M";
		static inline const juce::String channelR = "R";
		static inline const I18n::Text working{ u8"波形を作っています…", u8"Building the waveform…" };
		static inline const I18n::Text empty{ u8"「生成」で作ります", u8"Press Generate to build it" };
		static inline const I18n::Text generateTooltip{ u8"今の設定で 10 秒ぶんの波形を作ります。値を変えたら押し直してください。", u8"Builds 10 seconds of waveform from the current settings. Press it again after changing a value." };
		static inline const I18n::Text removeTooltip{ u8"作った波形を捨てます。", u8"Throws away the waveform that was built." };
		// 縦の拡大率の頭に付ける印 (x1〜x8)
		static inline const juce::String zoomPrefix = "x";

		// シークバーを右クリックしたときの、コマの番号を打ち込む欄の見出し
		static inline const I18n::Text frame{ u8"フレーム", u8"Frame" };
	}
}
