#pragma once

#include <JuceHeader.h>

#include "../Gui/GuiI18n.h"

// ============================================================================
// 読み込み中の表示 (GuiLoading) の文言
// ============================================================================
// 読み込み中の表示は 12 本で共有する部品なので、文言も共有する。
namespace EditorGuiText
{
	namespace Loading
	{
		static inline const I18n::Text cancel{ u8"中止", u8"Cancel" };
		static inline const I18n::Text cancelling{ u8"中止しています…", u8"Cancelling…" };
	}
}
