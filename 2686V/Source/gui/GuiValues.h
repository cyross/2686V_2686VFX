// UI で使用するデータを管理

#pragma once

#include <JuceHeader.h>

namespace GuiValue
{
	namespace Window
	{
		static inline constexpr int width = 1280;
		static inline constexpr int height = 720;
	}

	namespace WaterMarkLogo
	{
		static inline const juce::String fontFamily = "Times New Roman";
		static inline constexpr float fontSize = 96.0f;
		static inline constexpr float fontAlpha = 0.3f;
	}

	namespace About
	{
		namespace LogoImage
		{
			static inline constexpr int areaHeight = 280;
			static inline constexpr int imageSize = 256;
		}

		namespace PluginName
		{
			static inline constexpr int areaHeight = 60;
			static inline const juce::String fontFamily = "Times New Roman";
			static inline constexpr float fontSize = 32.0f;
		}

		namespace PluginVerion
		{
			static inline constexpr int areaHeight = 40;
			static inline constexpr float fontSize = 20.0f;
		}

		namespace PluginCioyright
		{
			static inline constexpr int areaHeight = 30;
			static inline constexpr float fontSize = 16.0f;
		}

		namespace VstLogo
		{

			static inline constexpr int areaHeight = 96;
			static inline constexpr int width = 150;
			static inline constexpr int height = 60;
		}

		namespace VstNotice
		{
			static inline constexpr int areaHeight = 20;
			static inline constexpr float fontSize = 14.0f;
		}

		namespace GplNotice
		{
			static inline constexpr int areaHeight = 40;
			static inline constexpr float fontSize = 14.0f;
		}

		namespace GplLink
		{
			static inline constexpr int areaHeight = 40;
			static inline constexpr float fontSize = 24.0f;
		}
	}

#if defined(BUILD_AS_FX_PLUGIN)
	static inline constexpr int TabNumber = 3;
#else
	static inline constexpr int TabNumber = 14;

	// 鍵盤UIの高さ
	static inline constexpr int KeyboardHeight = 60;
#endif

	namespace Preview
	{
		static inline constexpr int extraWidth = 288;
		static inline constexpr int drawSize = 280;
	}

	namespace MVol
	{
		static inline constexpr int paddingTop = 16;
	}

	namespace MonoMode
	{
		static inline constexpr int paddingTop = 20;
	}

	namespace PresetName
	{
		static inline constexpr int paddingBottom = 20;
	}

	namespace Category
	{
		static inline constexpr int paddingTop = 4;
		static inline constexpr int paddingBotton = 3;
	}

	namespace PaddingTop
	{
		static inline constexpr int mask = 16;
		static inline constexpr int mmlBtn = 20;
	}

	namespace PaddingBottom
	{
		static inline constexpr int block = 6;
		static inline constexpr int mainRow = 3;
		static inline constexpr int row = 3;
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
			static inline constexpr int height = 18;

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
			static inline constexpr int width = 240;

			namespace Freq
			{
				static inline constexpr int width = 120;
			}

			namespace Mix
			{
				static inline constexpr int width = 64;
			}

			namespace Pan
			{
				static inline constexpr int width = 64;
				static inline constexpr int paddingHeight = 2;
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
			static inline constexpr int paddingTop = 2;
		}
	}

	namespace Fm
	{
		namespace Op
		{
			static inline constexpr int width = 250;
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
						static constexpr int opzx3Width = 45;
					}

					namespace Pan
					{
						static constexpr int width = 64;

						namespace Padding
						{
							static constexpr int height = 2;
						}
					}

					namespace Mix
					{
						static constexpr int width = 64;

						namespace Padding
						{
							static constexpr int height = 2;
						}
					}

					namespace TriPeak
					{
						static constexpr int width = 80;
					}

					namespace LoadPcm
					{
						static constexpr int width = 50;
					}

					namespace PcmFile
					{
						static constexpr int width = 155;
					}

					namespace ClearPcm
					{
						static constexpr int width = 20;
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

	namespace Ssg
	{
		static constexpr int LeftWidth = 240;
		static constexpr int RightWidth = 800;
		static constexpr int Height = 640;
	}

	namespace Wt
	{
		static constexpr int samples32 = 32;
		static constexpr int samples64 = 64;

		static constexpr int Height = 680;

		static constexpr int RightWidth = 1000;
		static constexpr int RightHeight = 660;

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
				static constexpr int paddingBottom = 2;
			}

			namespace ResetBtn
			{
				static constexpr int Height = 36;
				static constexpr int To0Width = 120;
				static constexpr int To1Width = 120;
				static constexpr int ToM1Width = 120;

				namespace Padding
				{
					static constexpr int Top = 8;
					static constexpr int Right = 4;
				}
			}
		}
	}

	namespace Rhythm
	{
		namespace Pad
		{
			static constexpr int width = 250;
			static constexpr int height = 340;
		}
	}

	namespace Adpcm
	{
		static inline constexpr int width = 640;
		static inline constexpr int height = 240;

		namespace File
		{
			static inline constexpr int height = 80;
		}
	}

	namespace Fx
	{
		static inline constexpr int Width = 1000;
		static inline constexpr int Height = 720;
		static inline constexpr int AreaHeightBig = 310;
		static inline constexpr int AreaHeightMini = 150;
		static inline constexpr int AreaHeight = 220;
		static inline constexpr int GlobalBypassHeight = 120;
		static inline constexpr int BypassWidth = 120;
		static inline constexpr int GroupHeight = 200;
		static inline constexpr int KnobAreaWidth = 200;
		static inline constexpr int KnobWidth = 120;
		static inline constexpr int KnobHeight = 60;
		static inline constexpr int ButtonWidth = 240;
	}

	namespace Preset
	{
		static inline constexpr int FileLabelHeight = 24;

		namespace Search
		{
			namespace Row
			{
				static inline constexpr int Height = 30;

				namespace Padding
				{
					static inline constexpr int Right = 5;
				}

				namespace Button
				{
					namespace Search
					{
						static inline constexpr int Width = 60;
					}

					namespace Clear
					{
						static inline constexpr int Width = 30;
					}
				}
			}

			namespace Padding
			{
				static inline constexpr int Botton = 5;
			}
		}

		namespace Table
		{
			static inline constexpr float WidthRate = 0.6f;
			static inline constexpr int PaddingWidth = 4;
			static inline constexpr int PaddingHeight = 4;

			namespace ColWidth
			{
				static inline constexpr int Genre = 80;
				static inline constexpr int FileName = 150;
				static inline constexpr int Mode = 80;
				static inline constexpr int PresetName = 150;
				static inline constexpr int Author = 100;
				static inline constexpr int Version = 50;
				static inline constexpr int LastModified = 120;
			}
		}

		namespace Meta
		{
			static inline constexpr int AreaHeight = 320;
			static inline constexpr int RowHeight = 20;
			static inline constexpr int LabelWidth = 50;
			static inline constexpr int PaddingWidth = 4;
			static inline constexpr int PaddingHeight = 4;
			static inline constexpr int PaddingHeight2 = 12;
		}

		namespace Button
		{
			static inline constexpr int PaddingTop = 10;
			static inline constexpr int PaddingHeight = 4;
			static inline constexpr int Height = 20;
		}
	}

	namespace Settings
	{
		static inline constexpr int PaddingHeight = 4;
		static inline constexpr int RowHeight = 30;
		static inline constexpr int LabelWidth = 80;
		static inline constexpr int LongLabelWidth = 140;
		static inline constexpr int ModeSelectorWidth = 320;
		static inline constexpr int BrowseButtonWidth = 80;
		static inline constexpr int ClearButtonWidth = 60;
		static inline constexpr int HeadroomGainSliderWidth = 200;
		static inline constexpr int ToggleWidth = 400;
		static inline constexpr int ButtonWidth = 200;
		static inline constexpr int ButtonPaddingRight = 4;
	}

	static inline constexpr int TopParamWidth = 160;
	static inline constexpr int QualityGroupHeight = 60;
	static inline constexpr int QualityParamWidth = 160;
}
