#pragma once

#include <JuceHeader.h>

#include "../Gui/GuiI18n.h"

// ============================================================================
// 生成波形 (GenWave) の文言
// ============================================================================
// パラメータのブラウザーと生成波形の再生の操作 (GenWavePlayer) の文言。12 本で共有する。
// 生成中の波形の窓 (4.4.0) の文言は EditorStageWaveText.h
namespace EditorGuiText
{
	namespace GenWave
	{
		static inline const I18n::Text generate{ u8"生成", u8"Generate" };
		static inline const I18n::Text regenerate{ u8"再生成", u8"Regenerate" };

		// シークバーを右クリックしたときの、コマの番号を打ち込む欄の見出し
		static inline const I18n::Text frame{ u8"フレーム", u8"Frame" };
	}
}
