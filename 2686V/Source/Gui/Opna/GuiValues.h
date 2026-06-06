// UI で使用するデータを管理

#pragma once

namespace OpnaGuiValue
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
		namespace Graph
		{
			static inline constexpr int height = 120;
			static inline constexpr int ButtonHeight = 16;
		}

		namespace Separator
		{
			static inline constexpr int height = 16;
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
		}
	}
}
