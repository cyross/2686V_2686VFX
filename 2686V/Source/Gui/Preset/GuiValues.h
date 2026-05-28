// UI で使用するデータを管理

#pragma once

#include <JuceHeader.h>

namespace PresetGuiValue
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
			static inline constexpr int FileName = 200;
			static inline constexpr int Mode = 80;
			static inline constexpr int PresetName = 300;
			static inline constexpr int Author = 100;
			static inline constexpr int Version = 50;
			static inline constexpr int LastModified = 120;
		}
	}

	namespace Meta
	{
		static inline constexpr int AreaHeight = 420;
		static inline constexpr int RowHeight = 20;
		static inline constexpr int LabelWidth = 80;
		static inline constexpr int PaddingWidth = 4;
		static inline constexpr int PaddingHeight = 4;
		static inline constexpr int PaddingHeight2 = 12;
	}

	namespace Button
	{
		static inline constexpr int PaddingTop = 10;
		static inline constexpr int PaddingHeight = 4;
		static inline constexpr int Height = 32;
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
}
