// UI で使用するデータを管理

#pragma once

namespace WtPlusGuiValue
{
	namespace Category
	{
		static inline constexpr int paddingTop = 4;
		static inline constexpr int paddingBottom = 3;
	}

	namespace Group
	{
		static inline constexpr int TitlePaddingTop = 10;

		namespace Padding
		{
			static inline constexpr int width = 10;
			static inline constexpr int height = 10;
		}
	}

	// 波形メモリ設定グループの幅。32 行ぶんのロード UI と、
	// その右に並ぶ波形プレビューが入る。
	static inline constexpr int WaveGroupWidth = 600;

	// 波形メモリスロットの 1 行
	namespace Slots
	{
		// ロード UI の横幅。グループを広げても行の見た目は変えない。
		static inline constexpr int rowWidth = 280;

		// 行と波形のあいだ / 行と行のあいだ
		static inline constexpr int gap = 6;
	}

	namespace MainGroup
	{
		static inline constexpr int width = 260;

		namespace Graph
		{
			static inline constexpr int height = 120;
			static inline constexpr int ButtonHeight = 16;
		}

		namespace Separator
		{
			static inline constexpr int height = 20;
		}
	}
}
