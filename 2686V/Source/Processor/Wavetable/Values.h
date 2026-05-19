#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Values.h"

namespace WtPrValue
{
	inline constexpr float boolThread = CorePrValue::boolThread;

	namespace Level
	{
		inline constexpr float min = CorePrValue::Level::min; // 最小値
		inline constexpr float max = CorePrValue::Level::max;  // 最大値
		inline constexpr float initial = CorePrValue::Level::initial; // 初期値
	}

	namespace Bit
	{
		inline constexpr int min = CorePrValue::Quality::Bit::min;
		inline constexpr int max = CorePrValue::Quality::Bit::max;
		inline constexpr int initial = CorePrValue::Quality::Bit::initial;
	}

	namespace Rate
	{
		// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
		inline constexpr int min = CorePrValue::Quality::Rate::min;
		inline constexpr int max = CorePrValue::Quality::Rate::max;
		inline constexpr int initial = 3; // 49.7kHz
	}

	inline constexpr int customSize1 = 32;
	inline constexpr int customSize2 = 64;
	inline constexpr int customSize3 = 128;
	inline constexpr int customSize4 = 256;

	namespace SammpleSize
	{
		inline constexpr int min = 0; // 最小値
		inline constexpr int max = 3; // 最大値
		inline constexpr int initial = 0; // 最大値
	}

	namespace Steps
	{
		// 0, +16, +32, +64, +128, +256, -16, -32, -64, -128, -256
		inline constexpr int min = 0; // 最小値
		inline constexpr int max = 10; // 最大値
		inline constexpr int initial = 0; // 最大値
	}

	namespace WaveForm
	{
		inline constexpr int min = 0; // 最小値
		inline constexpr int max = 8; // 最大値
		inline constexpr int initial = 0; // 最大値
	}

	namespace Custom
	{
		inline constexpr float min = -1.0f; // 最小値
		inline constexpr float max = 1.0f;  // 最大値
		inline constexpr float initial = 0.0f; // 初期値
	}

	namespace Mod
	{
		namespace Enable
		{
			inline constexpr float initial = false; // 初期値
		}

		namespace Depth
		{
			inline constexpr float min = 0.1f; // 最小値
			inline constexpr float max = 1.0f; // 最大値
			inline constexpr float initial = 0.2f; // 初期値
		}

		namespace Speed
		{
			inline constexpr float min = 0.1f; // 最小値
			inline constexpr float max = 10.0f; // 最大値
			inline constexpr float initial = 1.0f; // 初期値
		}
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
			inline constexpr int min = 0;
			inline constexpr int max = 4;
			inline constexpr int initial = 4;
		}

		namespace R0
		{
			inline constexpr float min = 0.00f; // 最小値（秒）
			inline constexpr float max = 5.0f;  // 最大値（秒）
			inline constexpr float initial = 0.0f; // 初期値（秒）
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

		namespace Stl
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace L0
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
	}

	namespace Lfo
	{
		namespace AmFreq
		{
			inline constexpr float min = 0.1f;
			inline constexpr float max = 50.0f;
			inline constexpr float initial = 5.0f;
		}

		namespace PmFreq
		{
			inline constexpr float min = 0.1f;
			inline constexpr float max = 50.0f;
			inline constexpr float initial = 5.0f;
		}

		namespace AmSmRt
		{
			inline constexpr float min = 0.005f;
			inline constexpr float max = 0.5f;
			inline constexpr float initial = 0.005f;
		}

		namespace Am
		{
			inline constexpr bool initial = false;
		}

		namespace Ams
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Amd
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Pm
		{
			inline constexpr bool initial = false;
		}

		namespace Pms
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Pmd
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace SyncDelay
		{
			inline constexpr int min = 0;
			inline constexpr int max = 255;
			inline constexpr int initial = 0;
		}

		namespace PmShape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
		}

		namespace AmShape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
		}
	}
}
