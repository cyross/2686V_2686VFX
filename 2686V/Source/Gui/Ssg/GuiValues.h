// UI で使用するデータを管理

#pragma once

namespace SsgGuiValue
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
			static inline constexpr int ButtonHeight = 16;
		}

		namespace Separator
		{
			static inline constexpr int height = 20;
		}
	}

	namespace ParamGroup
	{
		static inline constexpr int width = 250;
	}
}
