// UI で使用するデータを管理

#pragma once

namespace RhythmGuiValue
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

		namespace Separator
		{
			static inline constexpr int height = 20;
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
}
