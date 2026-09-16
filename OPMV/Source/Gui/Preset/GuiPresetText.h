// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

#include "../../Core/Gui/GuiI18n.h"

namespace PresetGuiText
{
	namespace Preset
	{
		namespace Dialog
		{
			static inline const I18n::Text initPreset{ u8"プリセット初期化", u8"Initialise preset" };
			static inline const I18n::Text initPresetNotice{ u8"全ての音源とアドバンスドモードの設定値を初期化します。よろしいですか？", u8"This resets every chip and the advanced mode to their defaults. Continue?" };
			static inline const I18n::Text initPresetSuccedBtn{ u8"初期化", u8"Initialise" };
			static inline const I18n::Text initPresetCancelBtn{ u8"キャンセル", u8"Cancel" };

			static inline const I18n::Text deletePreset{ u8"プリセット削除", u8"Delete preset" };
			static inline const I18n::Text deletePresetNotice{ u8"プリセットファイルを削除します。よろしいですか？", u8"This deletes the preset file. Continue?" };
			static inline const I18n::Text deletePresetSuccedBtn{ u8"削除", u8"Delete" };
			static inline const I18n::Text deletePresetCancelBtn{ u8"キャンセル", u8"Cancel" };

			static inline const I18n::Text reflectPresetToolTipMessage{ u8"プリセットのメタデータをクリップボードにコピーします。", u8"Copies the preset's metadata to the clipboard." };
		}
	}
}
