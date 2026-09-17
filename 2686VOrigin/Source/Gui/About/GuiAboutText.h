// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

#include "../../Core/Gui/GuiI18n.h"

namespace AboutGuiText
{
	// マニュアルへの行き先。言語に合わせて日本語版と英語版を出し分ける。
	namespace Manual
	{
		static inline const I18n::Text navigate{ u8"ユーザーズマニュアル", u8"User's manual" };
		static inline const I18n::Text linkUrl{
			u8"https://cyross.github.io/2686V_2686VFX/",
			u8"https://cyross.github.io/2686V_2686VFX/en/"
		};
	}
}
