// UI で使用するデータを管理

#pragma once

namespace RhythmGuiValue
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
	}

	namespace ParamGroup
	{
		namespace Row
		{
			static inline constexpr int width = 226;
			static inline constexpr int height = 12;
			static inline constexpr int paddingTop = 3;
			static inline constexpr int paddingBottom = 0;
			static inline constexpr int paddingRight = 0;
		}

		namespace RhythmPcm
		{
			namespace LoadBtn
			{
				static constexpr int width = 50;
			}

			namespace FileLabel
			{
				static constexpr int width = 155;
			}

			namespace ClearBtn
			{
				static constexpr int width = 35;
			}
		}

		namespace RhythmPan
		{
			static inline constexpr int width = 74;
			static inline constexpr int paddingRight = 4;
		}
	}

	namespace Pad
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


		static constexpr int width = 230;

		// 区分を横へ並べるときの、1 列ぶんの幅と隙間
		//
		// 幅は FORM の読み込み行で決まる。Load 50 + ファイル名 155 +
		// Clear 35 で 240 要るので、枠の余白 (左右 10) と縦の送り棒の
		// ぶんを足したものより狭くすると、Clear が見切れる。
		namespace Col
		{
			static inline constexpr int width = 280;
			static inline constexpr int gap = 10;

			// TARGET の右へ出す「オペレーター1」などの名前と、その右の余白
			static inline constexpr int nameWidth = 96;
			static inline constexpr int nameGap = 8;
		}
		static constexpr int height = 460;
	}
}
