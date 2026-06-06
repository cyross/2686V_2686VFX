#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorValues.h"

namespace FxPrValue
{
	inline constexpr float boolThread = CorePrValue::boolThread;

	namespace MBypass
	{
		inline constexpr float initial = false; // 初期値
	}

	namespace Bypass
	{
		inline constexpr float initial = false; // 初期値
	}

	namespace Mix
	{
		inline constexpr float min = 0.0f;
		inline constexpr float max = 1.0f;
		inline constexpr float initial = 0.0f;
	}

	namespace Tremolo
	{
		namespace Rate
		{
			inline constexpr float min = 0.1f; // 最小値
			inline constexpr float max = 20.0f;  // 最大値
			inline constexpr float initial = 5.0f; // 初期値
		}

		namespace Depth
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.5f; // 初期値
		}
	}

	namespace Vibrato
	{
		namespace Rate
		{
			inline constexpr float min = 0.1f; // 最小値
			inline constexpr float max = 10.0f;  // 最大値
			inline constexpr float initial = 2.0f; // 初期値
		}

		namespace Dept
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.5f; // 初期値
		}
	}

	namespace Mbc
	{
		namespace Rate
		{
			inline constexpr float min = 1.0f; // 最小値
			inline constexpr float max = 50.0f;  // 最大値
			inline constexpr float initial = 1.0f; // 初期値
		}

		namespace Bit
		{
			inline constexpr float min = 2.0f; // 最小値
			inline constexpr float max = 24.0f;  // 最大値
			inline constexpr float initial = 24.0f; // 初期値
		}
	}

	namespace Delay
	{
		namespace Time
		{
			inline constexpr float min = 10.0f; // 最小値
			inline constexpr float max = 1000.0f;  // 最大値
			inline constexpr float initial = 375.0f; // 初期値
		}

		namespace Fb
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 0.95f;  // 最大値
			inline constexpr float initial = 0.4f; // 初期値
		}
	}

	namespace Reverb
	{
		namespace Size
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.5f; // 初期値
		}

		namespace Damp
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.5f; // 初期値
		}
	}

	namespace Filter
	{
		namespace Type
		{
			inline constexpr int min = 1;
			inline constexpr int max = 3;
			inline constexpr int initial = 1; // 1:LPF, 2:HPF, 3:BPF
		}

		namespace Freq
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 20000.0f;
			inline constexpr float initial = 20000.0f;
		}

		namespace Q
		{
			inline constexpr float min = 0.1f;
			inline constexpr float max = 10.0f;
			inline constexpr float initial = 0.707f;
		}
	}

	namespace Eq3b
	{
		namespace LowGainDb
		{
			inline constexpr float min = -15.0f;
			inline constexpr float max = 15.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace MidFreq
		{
			inline constexpr float min = 500.0f;
			inline constexpr float max = 3000.0f;
			inline constexpr float initial = 2000.0f;
		}

		namespace MidGainDb
		{
			inline constexpr float min = -15.0f;
			inline constexpr float max = 15.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace HighGainDb
		{
			inline constexpr float min = -15.0f;
			inline constexpr float max = 15.0f;
			inline constexpr float initial = 0.0f;
		}
	}
}
