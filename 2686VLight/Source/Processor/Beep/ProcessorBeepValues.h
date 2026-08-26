#pragma once

#include <JuceHeader.h>

namespace BeepPrValue
{
	namespace AntiAlias
	{
		// 既定は OFF。既存のパッチの音を変えないため。
		inline constexpr bool initial = false;
	}

	namespace TimerClock
	{
		// 1: Free (分周なし) / 2: IBM PC / 3: PC-9801 8MHz系 / 4: PC-9801 5MHz系
		inline constexpr int min = 1;
		inline constexpr int max = 4;
		// 既定は Free。既存のパッチの音程を変えないため。
		inline constexpr int initial = 1;
	}
}
