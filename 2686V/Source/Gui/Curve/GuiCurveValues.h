// UI で使用するデータを管理

#pragma once

namespace CurveGuiValue
{
	namespace CurveGroup
	{
		namespace Padding
		{
			static inline constexpr int width = 10;
			static inline constexpr int height = 10;
		}

		namespace Separator
		{
			static inline constexpr int height = 20;
		}

		namespace Row
		{
			static inline constexpr int height = 24;
			static inline constexpr int paddingY = 8;
			static inline constexpr int valuePaddingY = 2;

			static inline constexpr int titleWidth = 80;

			static inline constexpr int labelWidth = 120;

			namespace Padding
			{
				static inline constexpr int x = 4;
				static inline constexpr int y = 1;
			}
		}
	}
}
