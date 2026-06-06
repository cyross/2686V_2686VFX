// UI で使用するデータを管理

#pragma once

namespace WtGuiValue
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

	namespace ParamGroup
	{
		static inline constexpr int width = 250;
		static inline constexpr int height = 640;

		namespace Row
		{
			static inline constexpr int width = 226;
			static inline constexpr int height = 12;
			static inline constexpr int paddingTop = 3;
			static inline constexpr int paddingBottom = 0;
			static inline constexpr int paddingRight = 0;
		}
	}

	static constexpr int Height = 680;

	static constexpr int RightWidth = 1024;
	static constexpr int RightHeight = 700;

	namespace Custom
	{
		namespace Slider
		{
			static constexpr int Width = 30;
			static constexpr int Height = 600;
		}

		namespace SetBtn
		{
			static constexpr int height = 10;
			static constexpr int paddingBottom = 1;
		}

		namespace ResetBtn
		{
			static constexpr int height = 36;
			static constexpr int to0Width = 120;
			static constexpr int to1Width = 120;
			static constexpr int toM1Width = 120;

			namespace Padding
			{
				static constexpr int Top = 8;
				static constexpr int Bottom = 4;
				static constexpr int Right = 8;
			}
		}
	}
}
