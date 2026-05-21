// UI で使用するデータを管理

#pragma once

namespace OpnGuiValue
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

	namespace Fm
	{
		namespace Op
		{
			static inline constexpr int width = 256;
			static inline constexpr int height = 640;

			namespace Padding
			{
				static inline constexpr int width = 12;
				static inline constexpr int height = 8;
			}

			namespace Row
			{
				static inline constexpr int height = 15;

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

				namespace Button
				{
					static inline constexpr int wdth = 240;

					namespace Two
					{
						static constexpr int width = 120;

						namespace Padding
						{
							static constexpr int height = 2;
						}
					}

					namespace Three
					{
						static constexpr int width = 64;

						namespace Padding
						{
							static constexpr int height = 2;
						}
					}

					namespace Freq
					{
						static constexpr int width = 120;
						static constexpr int opzx7Width = 45;
					}
				}

				namespace Padding
				{
					static constexpr int bottom = 2;
					static constexpr int right = 0;
				}
			}

			namespace LastRow
			{
				namespace Padding
				{
					static constexpr int bottom = 0;
				}
			}

			static constexpr int LabelWidth = 50;
			static constexpr int LabelHeight = 15;
			static constexpr int SliderWidth = 50;
			static constexpr int SliderHeight = 15;
			static constexpr int SliderValueWidth = 50;
			static constexpr int SliderValueHeight = 15;
			static constexpr int ComboBoxWidth = 80;
			static constexpr int ComboBoxHeight = 15;
			static constexpr int ToggleButtonWidth = 80;
			static constexpr int ToggleButtonHeight = 15;
			static constexpr int TextButtonWidth = 80;
			static constexpr int TextButtonHeight = 15;
		}
	}

	static inline constexpr int TopParamWidth = 160;
	static inline constexpr int QualityGroupHeight = 60;
	static inline constexpr int QualityParamWidth = 160;
}
