#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Values.h"

namespace AdpcmPrValue
{
	inline constexpr float boolThread = CorePrValue::boolThread;

	namespace Bit
	{
		inline constexpr int min = CorePrValue::Pcm::Bit::min;
		inline constexpr int max = CorePrValue::Pcm::Bit::max;
		inline constexpr int initial = CorePrValue::Pcm::Bit::initial;
	}

	namespace Rate
	{
		// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
		inline constexpr int min = CorePrValue::Pcm::Rate::min;
		inline constexpr int max = CorePrValue::Pcm::Rate::max;
		inline constexpr int initial = CorePrValue::Pcm::Rate::initial;
	}

	namespace Level
	{
		inline constexpr float min = CorePrValue::Level::min;
		inline constexpr float max = CorePrValue::Level::max;
		inline constexpr float initial = CorePrValue::Level::initial;
	}

	namespace Loop
	{
		inline constexpr float initial = true;
	}

	namespace Pan
	{
		inline constexpr float min = 0.0f;
		inline constexpr float max = 1.0f;
		inline constexpr float initial = 0.5f;
	}

	namespace Offset
	{
		inline constexpr float min = CorePrValue::Pcm::Offset::min;
		inline constexpr float max = CorePrValue::Pcm::Offset::max;
		inline constexpr float initial = CorePrValue::Pcm::Offset::initial;
	}

	namespace Ratio
	{
		inline constexpr float min = CorePrValue::Pcm::Ratio::min;
		inline constexpr float max = CorePrValue::Pcm::Ratio::max;
		inline constexpr float initial = CorePrValue::Pcm::Ratio::initial;
	}

	namespace Adsr
	{
		namespace Bypass
		{
			inline constexpr float initial = CorePrValue::Adsr::Bypass::initial;
		}

		namespace Ar
		{
			inline constexpr float min = CorePrValue::Adsr::Ar::min;
			inline constexpr float max = CorePrValue::Adsr::Ar::max;
			inline constexpr float initial = CorePrValue::Adsr::Ar::initial;
		}

		namespace Dr
		{
			inline constexpr float min = CorePrValue::Adsr::Dr::min;
			inline constexpr float max = CorePrValue::Adsr::Dr::max;
			inline constexpr float initial = CorePrValue::Adsr::Dr::initial;
		}

		namespace Sl
		{
			inline constexpr float min = CorePrValue::Adsr::Sl::min;
			inline constexpr float max = CorePrValue::Adsr::Sl::max;
			inline constexpr float initial = CorePrValue::Adsr::Sl::initial;
		}

		namespace Rr
		{
			inline constexpr float min = CorePrValue::Adsr::Rr::min;
			inline constexpr float max = CorePrValue::Adsr::Rr::max;
			inline constexpr float initial = CorePrValue::Adsr::Rr::initial;
		}

		namespace Stl
		{
			inline constexpr float min = CorePrValue::Adsr::Stl::min;
			inline constexpr float max = CorePrValue::Adsr::Stl::max;
			inline constexpr float initial = CorePrValue::Adsr::Stl::initial;
		}
	}

	namespace PitchAdsr
	{
		namespace Bypass
		{
			inline constexpr float initial = false;
		}
	}

	namespace Dt1
	{
		inline constexpr int min = 0;
		inline constexpr int max = 7;
		inline constexpr int initial = 0;
	}

	namespace Dt2
	{
		inline constexpr int min = 0;
		inline constexpr int max = 3;
		inline constexpr int initial = 0;
	}

	namespace SsgSwEnv
	{
		namespace Bypass
		{
			inline constexpr float initial = true; // 初期値
		}

		namespace Steps
		{
			inline constexpr int min = 1;
			inline constexpr int max = 5;
			inline constexpr int initial = 5;
		}

		namespace Loop
		{
			inline constexpr float initial = false;
		}

		namespace LoopTo
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace LoopCount
		{
			inline constexpr int min = 0;
			inline constexpr int max = 200;
			inline constexpr int initial = 0;
		}

		namespace R1
		{
			inline constexpr float min = 0.0f; // 最小値（秒）
			inline constexpr float max = 5.0f;  // 最大値（秒）
			inline constexpr float initial = 0.0f; // 初期値（秒）
		}

		namespace R2
		{
			inline constexpr float min = 0.00f; // 最小値（秒）
			inline constexpr float max = 5.0f;  // 最大値（秒）
			inline constexpr float initial = 0.0f; // 初期値（秒）
		}

		namespace R3
		{
			inline constexpr float min = 0.00f; // 最小値（秒）
			inline constexpr float max = 5.0f;  // 最大値（秒）
			inline constexpr float initial = 0.0f; // 初期値（秒）
		}

		namespace R4
		{
			inline constexpr float min = 0.00f; // 最小値（秒）
			inline constexpr float max = 5.0f;  // 最大値（秒）
			inline constexpr float initial = 0.0f; // 初期値（秒）
		}

		namespace R5
		{
			inline constexpr float min = 0.00f; // 最小値（秒）
			inline constexpr float max = 5.0f;  // 最大値（秒）
			inline constexpr float initial = 0.0f; // 初期値（秒）
		}

		namespace R6
		{
			inline constexpr float min = 0.00f; // 最小値（秒）
			inline constexpr float max = 5.0f;  // 最大値（秒）
			inline constexpr float initial = 0.0f; // 初期値（秒）
		}

		namespace Stl
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace L1
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace L2
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace L3
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace L4
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace L5
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace L6
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}
	}
}
