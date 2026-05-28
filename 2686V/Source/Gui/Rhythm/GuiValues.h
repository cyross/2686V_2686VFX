// UI で使用するデータを管理

#pragma once

namespace RhythmGuiValue
{
	namespace MVol
	{
		static inline constexpr int paddingTop = 16;
	}

	namespace MonoMode
	{
		static inline constexpr int paddingTop = 8;
	}

	namespace PresetName
	{
		static inline constexpr int paddingBottom = 2;
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

	namespace Tab
	{
		namespace Padding
		{
			static inline constexpr int width = 20;
			static inline constexpr int height = 20;
		}
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
				static constexpr int width = 20;
			}
		}

		namespace RhythmPan
		{
			static inline constexpr int width = 74;
			static inline constexpr int paddingRight = 4;
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

	namespace Pad
	{
		namespace Graph
		{
			static inline constexpr int height = 120;
			static inline constexpr int ButtonHeight = 16;
		}

		namespace Separator
		{
			static inline constexpr int height = 16;
		}

		static constexpr int width = 230;
		static constexpr int height = 460;
	}

	static inline constexpr int TopParamWidth = 160;
	static inline constexpr int QualityGroupHeight = 60;
	static inline constexpr int QualityParamWidth = 160;
}
