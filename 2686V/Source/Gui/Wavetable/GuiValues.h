// UI で使用するデータを管理

#pragma once

namespace WtGuiValue
{
	namespace MonoMode
	{
		static inline constexpr int paddingTop = 8;
	}

	namespace PresetName
	{
		static inline constexpr int paddingBottom = 8;
	}

	namespace Category
	{
		static inline constexpr int paddingTop = 4;
		static inline constexpr int paddingBottom = 3;
	}

	namespace PaddingTop
	{
		static inline constexpr int mask = 16;
		static inline constexpr int mmlBtn = 20;
	}

	namespace PaddingBottom
	{
		static inline constexpr int block = 6;
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

		namespace Row
		{
			static inline constexpr int height = 15;
			static inline constexpr int paddingTop = 3;
			static inline constexpr int paddingBottom = 0;
			static inline constexpr int paddingRight = 0;

			namespace Padding
			{
				static inline constexpr int top = 2;
				static inline constexpr int bottom = 2;
				static inline constexpr int right = 0;
			}

			namespace MainVol
			{
				static inline constexpr int height = 28;
			}
		}

		namespace LastRow
		{
			namespace Padding
			{
				static inline constexpr int bottom = 0;
			}
		}

		namespace Label
		{
			static inline constexpr int width = 60;
		}

		namespace LabelOnly
		{
			static inline constexpr int width = 280;
		}

		namespace Value
		{
			static inline constexpr int width = 220;
		}

		namespace Comps
		{
			namespace One
			{
				static inline constexpr int width = 260;
				static inline constexpr int paddingRight = 0;
			}

			namespace Two
			{
				static inline constexpr int width = 128;
				static inline constexpr int paddingRight = 4;
			}

			namespace Three
			{
				static inline constexpr int width = 85;
				static inline constexpr int paddingRight = 2;
			}
		}
	}

	namespace Label
	{
		static inline constexpr int width = 40;
		static inline constexpr int height = 20;
	}

	namespace Slider
	{
		static inline constexpr int width = 50;
		static inline constexpr int height = 20;

		namespace Value
		{
			static inline constexpr int width = 50;
			static inline constexpr int height = 20;
		}
	}

	namespace ComboBox
	{
		static inline constexpr int width = 50;
		static inline constexpr int height = 20;

	}

	namespace ToggleButton
	{
		static inline constexpr int width = 80;
		static inline constexpr int height = 20;

	}

	namespace TextButton
	{
		static inline constexpr int width = 80;
		static inline constexpr int height = 20;
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

		namespace Comps
		{
			namespace One
			{
				static inline constexpr int width = 226;
				static inline constexpr int paddingRight = 0;
			}

			namespace Two
			{
				static inline constexpr int width = 111;
				static inline constexpr int paddingRight = 4;
			}

			namespace Three
			{
				static inline constexpr int width = 74;
				static inline constexpr int paddingRight = 2;
			}

			namespace Four
			{
				static inline constexpr int width = 54;
				static inline constexpr int paddingRight = 3;
			}

			namespace Five
			{
				static inline constexpr int width = 42;
				static inline constexpr int paddingRight = 4;
			}
		}

		namespace Label
		{
			static inline constexpr int width = 50;
		}

		namespace LabelOnly
		{
			static inline constexpr int width = 240;
		}

		namespace Value
		{
			static inline constexpr int width = 190;
		}
	}

	static constexpr int samples32 = 32;
	static constexpr int samples64 = 64;

	static constexpr int Height = 680;

	static constexpr int RightWidth = 1000;
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
				static constexpr int Right = 4;
			}
		}
	}
}
