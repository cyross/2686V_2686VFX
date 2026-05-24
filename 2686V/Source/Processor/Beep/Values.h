#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Values.h"

namespace BeepPrValue
{
	inline constexpr float boolThread = CorePrValue::boolThread;

	namespace Level
	{
		inline constexpr float min = CorePrValue::Level::min;
		inline constexpr float max = CorePrValue::Level::max;
		inline constexpr float initial = CorePrValue::Level::initial;
	}

	namespace Fix
	{
		inline constexpr bool initial = false; // 初期値 (Fixed Frequency Switch)
	}

	namespace FixFreq
	{
		inline constexpr float min = 0.0f;
		inline constexpr float max = 8000.0f;
		inline constexpr float initial = 440.0f; // 初期値
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
	}

	namespace PitchAdsr
	{
		namespace Bypass
		{
			inline constexpr float initial = false;
		}
	}

	namespace Mul
	{
		inline constexpr int min = 0;
		inline constexpr int max = 21;
		inline constexpr int initial = 2; // x 1.0
	}

	namespace MulRatio
	{
		inline constexpr float min = 0.5f;
		inline constexpr float max = 27.57f;
		inline constexpr float initial = 1.0f;
	}

	namespace Dt1
	{
		inline constexpr int min = 0;
		inline constexpr int max = 15;
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
