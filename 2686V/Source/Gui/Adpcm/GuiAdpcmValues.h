// UI で使用するデータを管理

#pragma once

namespace AdpcmGuiValue
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
			static inline constexpr int ButtonHeight = 20;
		}

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
					static constexpr int width = 20;
				}
			}
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
}
