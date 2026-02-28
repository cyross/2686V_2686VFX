// UI で使用するデータを管理

#pragma once

#include <JuceHeader.h>

namespace GuiValue
{
	namespace Window
	{
		static inline const int width = 1280;
		static inline const int height = 660;
	}

	namespace About
	{
		namespace Logo
		{
			static const juce::String fontFamily = "Times New Roman";
			static inline const float fontSize = 128.0f;
		}
	}

#if defined(BUILD_AS_FX_PLUGIN)
	static inline const int TabNumber = 3;
#else
	static inline const int TabNumber = 14;

	// 鍵盤UIの高さ
	const inline int KeyboardHeight = 60;
#endif

	namespace Preview
	{
		static inline const int extraWidth = 288;
		static inline const int drawSize = 280;
	}

	namespace Tab
	{
		namespace Padding
		{
			static inline const int width = 20;
			static inline const int height = 20;
		}
	}

	namespace Group
	{
		static inline const int TitlePaddingTop = 10;

		namespace Padding
		{
			static inline const int width = 10;
			static inline const int height = 10;
		}
	}

	namespace MainGroup
	{
		static inline const int width = 280;

		namespace Row
		{
			static inline const int height = 20;

			namespace Padding
			{
				static inline const int top = 2;
				static inline const int bottom = 2;
				static inline const int right = 0;
			}

			namespace MainVol
			{
				static inline const int height = 28;
			}
		}

		namespace LastRow
		{
			namespace Padding
			{
				static inline const int bottom = 0;
			}
		}

		namespace Label
		{
			static inline const int width = 50;
		}

		namespace Value
		{
			static inline const int width = 190;
		}

		namespace Button
		{
			static inline const int width = 240;

			namespace Freq
			{
				static inline const int width = 120;
			}

			namespace Mix
			{
				static inline const int width = 64;
			}

			namespace Pan
			{
				static inline const int width = 64;
				static inline const int paddingHeight = 2;
			}
		}
	}

	namespace Label
	{
		static inline const int width = 40;
		static inline const int height = 20;
	}

	namespace Slider
	{
		static inline const int width = 50;
		static inline const int height = 20;

		namespace Value
		{
			static inline const int width = 50;
			static inline const int height = 20;
		}
	}

	namespace ComboBox
	{
		static inline const int width = 50;
		static inline const int height = 20;

	}

	namespace ToggleButton
	{
		static inline const int width = 80;
		static inline const int height = 20;

	}

	namespace TextButton
	{
		static inline const int width = 80;
		static inline const int height = 20;
	}

	namespace Fm
	{
		namespace Op
		{
			static inline const int width = 250;
			static inline const int height = 640;

			namespace Padding
			{
				static const int width = 12;
				static const int height = 8;
			}

			namespace Row
			{
				static inline const int height = 20;

				namespace Label
				{
					static const int width = 50;
				}
				
				namespace Value
				{
					static const int width = 190;
				}

				namespace Button
				{
					static const int wdth = 240;

					namespace Freq
					{
						static const int width = 120;
						static const int opzx3Width = 45;
					}

					namespace Pan
					{
						static const int width = 64;

						namespace Padding
						{
							static const int height = 2;
						}
					}

					namespace Mix
					{
						static const int width = 64;

						namespace Padding
						{
							static const int height = 2;
						}
					}

					namespace TriPeak
					{
						static const int width = 80;
					}

					namespace PcmFile
					{
						static const int width = 155;
					}

					namespace ClearPcm
					{
						static const int width = 20;
					}
				}

				namespace Padding
				{
					static const int bottom = 2;
					static const int right = 0;
				}
			}

			namespace LastRow
			{
				namespace Padding
				{
					static const int bottom = 0;
				}
			}

			static const int LabelWidth = 50;
			static const int LabelHeight = 15;
			static const int SliderWidth = 50;
			static const int SliderHeight = 15;
			static const int SliderValueWidth = 50;
			static const int SliderValueHeight = 15;
			static const int ComboBoxWidth = 80;
			static const int ComboBoxHeight = 15;
			static const int ToggleButtonWidth = 80;
			static const int ToggleButtonHeight = 15;
			static const int TextButtonWidth = 80;
			static const int TextButtonHeight = 15;
		}
	}

	namespace Ssg
	{
		static const int LeftWidth = 240;
		static const int RightWidth = 800;
		static const int Height = 640;
	}

	namespace Wt
	{
		static const int samples32 = 32;
		static const int samples64 = 64;

		static const int Height = 640;

		static const int RightWidth = 1000;
		static const int RightHeight = 560;

		namespace Custom
		{
			namespace Slider
			{
				static const int Width = 30;
				static const int Height = 480;
			}

			namespace ResetBtn
			{
				static const int Height = 36;
				static const int To0Width = 120;
				static const int To1Width = 120;
				static const int ToM1Width = 120;

				namespace Padding
				{
					static const int Top = 8;
					static const int Right = 4;
				}
			}
		}
	}

	namespace Rhythm
	{
		namespace Pad
		{
			static const int width = 250;
			static const int height = 240;
		}
	}

	namespace Adpcm
	{
		static inline const int width = 640;
		static inline const int height = 240;

		namespace File
		{
			static inline const int height = 80;
		}
	}

	namespace Fx
	{
		static const int Width = 1000;
		static const int Height = 720;
		static const int AreaHeight = 160;
		static const int GlobalBypassHeight = 120;
		static const int BypassWidth = 120;
		static const int GroupHeight = 200;
		static const int KnobAreaWidth = 200;
		static const int KnobWidth = 120;
		static const int KnobHeight = 60;
		static const int ButtonWidth = 240;
	}

	namespace Preset
	{
		static const int FileLabelHeight = 24;

		namespace Search
		{
			namespace Row
			{
				static const int Height = 30;

				namespace Padding
				{
					static const int Right = 5;
				}

				namespace Button
				{
					namespace Search
					{
						static const int Width = 60;
					}

					namespace Clear
					{
						static const int Width = 30;
					}
				}
			}

			namespace Padding
			{
				static const int Botton = 5;
			}
		}

		namespace Table
		{
			static const float WidthRate = 0.6f;
			static const int PaddingWidth = 4;
			static const int PaddingHeight = 4;

			namespace ColWidth
			{
				static const int FileName = 150;
				static const int Mode = 80;
				static const int PresetName = 150;
				static const int Author = 100;
				static const int Version = 50;
			}
		}

		namespace Meta
		{
			static const int AreaHeight = 320;
			static const int RowHeight = 20;
			static const int LabelWidth = 50;
			static const int PaddingWidth = 4;
			static const int PaddingHeight = 4;
		}

		namespace Button
		{
			static const int PaddingTop = 10;
			static const int PaddingHeight = 4;
			static const int Height = 20;
		}
	}

	namespace Settings
	{
		static const int PaddingHeight = 4;
		static const int RowHeight = 30;
		static const int LabelWidth = 80;
		static const int LongLabelWidth = 140;
		static const int ModeSelectorWidth = 320;
		static const int BrowseButtonWidth = 80;
		static const int ClearButtonWidth = 60;
		static const int HeadroomGainSliderWidth = 200;
		static const int ToggleWidth = 400;
		static const int ButtonWidth = 200;
		static const int ButtonPaddingRight = 4;
	}

	static const int TopParamWidth = 160;
	static const int QualityGroupHeight = 60;
	static const int QualityParamWidth = 160;
}
