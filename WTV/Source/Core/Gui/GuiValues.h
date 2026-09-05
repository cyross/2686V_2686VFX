// UI で使用するデータを管理

#pragma once

#include <JuceHeader.h>

namespace CoreGuiValue
{
	static inline constexpr int TabNumber = 7;

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
		static inline constexpr int paddingBottom = 8;
	}

	namespace Category
	{
		static inline constexpr int paddingTop = 4;
		static inline constexpr int paddingBottom = 3;
		static inline constexpr int gapBelow = 6;
	}

	namespace ParamNameRow
	{
		static inline constexpr int width = 226;
		static inline constexpr int height = 24;
		static inline constexpr int paddingTop = 0;
		static inline constexpr int paddingBottom = 0;
		static inline constexpr int paddingRight = 0;
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
		static inline constexpr int width = 280;

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

			namespace Pcm
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
			static inline constexpr int width = 50;
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
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Two
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Three
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Four
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Five
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Six
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Seven
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}
		}

		namespace WtFiles
		{
			namespace LoadBtn
			{
				static constexpr int width = 25;
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

		// 値を出す枠。
		//
		// 幅を決め打ちにすると、周波数のように桁の多い値が入らず
		// 省略されてしまう。実際に出る文字から決めるので、ここでは
		// 下限と上限だけを持つ。
		namespace ValueBox
		{
			static inline constexpr int minWidth = 42;
			static inline constexpr int maxWidth = 80;
			static inline constexpr int height = 20;

			// 文字の左右へ置く余白
			static inline constexpr int padding = 10;

			// 既定より少し小さくして、桁数を稼ぐ
			static inline constexpr float fontHeight = 12.0f;
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
			static inline constexpr int width = 246;
			static inline constexpr int height = 14;
			static inline constexpr int paddingTop = 3;
			static inline constexpr int paddingBottom = 0;
			static inline constexpr int paddingRight = 0;
		}

		namespace Comps
		{
			namespace One
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingRight = 0;
			}

			namespace Two
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Three
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Four
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Five
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Six
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}

			namespace Seven
			{
				static inline constexpr int width = 0;
				static inline constexpr int paddingLeft = 0;
				static inline constexpr int paddingInner = 1;
				static inline constexpr int paddingRight = 0;
			}
		}

		namespace Opzx7Pcm
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

		namespace Label
		{
			static inline constexpr int width = 40;
		}

		namespace LabelOnly
		{
			static inline constexpr int width = 240;
		}

		namespace Value
		{
			static inline constexpr int width = 190;
		}

		namespace WtFiles
		{
			namespace LoadBtn
			{
				static constexpr int width = 25;
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
	}
}
