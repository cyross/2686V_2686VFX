// UI で使用するデータを管理

#pragma once

#include <JuceHeader.h>

namespace SettingsGuiValue
{
	namespace Group
	{
		static inline constexpr int TitlePaddingTop = 10;

		namespace Padding
		{
			static inline constexpr int width = 10;
			static inline constexpr int height = 10;
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

	namespace Settings
	{
		static inline constexpr int PaddingHeight = 4;
		static inline constexpr int RowHeight = 30;
		static inline constexpr int LabelWidth = 180;
		static inline constexpr int LongLabelWidth = 220;
		static inline constexpr int ModeSelectorWidth = 320;
		static inline constexpr int BrowseButtonWidth = 80;
		static inline constexpr int ClearButtonWidth = 60;
		static inline constexpr int HeadroomGainSliderWidth = 200;
		static inline constexpr int ToggleWidth = 400;
		static inline constexpr int ButtonWidth = 200;
		static inline constexpr int ButtonPaddingRight = 4;
	}
}
