// UI で使用するデータを管理

#pragma once

namespace OpmGuiValue
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

	namespace MainGroup
	{
		static inline constexpr int width = 260;

		namespace Separator
		{
			static inline constexpr int height = 20;
		}
		// チップ全体のエンベロープを見るグラフ
		namespace Graph
		{
			static inline constexpr int height = 120;
			static inline constexpr int ButtonHeight = 16;
		}
	}

	namespace ParamGroup
	{
		namespace Graph
		{
			static inline constexpr int height = 120;
			static inline constexpr int ButtonHeight = 16;

			// どのエンベロープを映すかを選ぶ札 1 枚ぶんの幅
			static inline constexpr int ModeButtonWidth = 46;
		}

		namespace Separator
		{
			static inline constexpr int height = 16;
		}
	}

	namespace Fm
	{
		namespace Op
		{
			static inline constexpr int width = 256;
			static inline constexpr int height = 640;

			// 区分を横へ並べるときの、1 列ぶんの幅と隙間
			namespace Col
			{
				static inline constexpr int width = 280;
				static inline constexpr int gap = 10;

				// TARGET の右へ出す「オペレーター1」などの名前と、その右の余白
				static inline constexpr int nameWidth = 96;
				static inline constexpr int nameGap = 8;
			}

			namespace Padding
			{
				static inline constexpr int width = 12;
				static inline constexpr int height = 8;
			}
		}
	}
}
