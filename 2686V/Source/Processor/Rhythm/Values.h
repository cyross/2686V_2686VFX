#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Values.h"

namespace RhythmPrValue
{
	inline constexpr float boolThread = CorePrValue::boolThread;

	inline constexpr int pads = 8;

	namespace Level
	{
		inline constexpr float min = CorePrValue::Level::min; // 最小値
		inline constexpr float max = CorePrValue::Level::max;  // 最大値
		inline constexpr float initial = CorePrValue::Level::initial; // 初期値
	}

	namespace Pad
	{
		namespace Pan
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.5f;
		}

		namespace Note
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 127; // 最大値
			inline constexpr int initial = 60; // 初期値
		}

		namespace OneShot
		{
			inline constexpr float initial = true; // 初期値
		}

		namespace Bit
		{
			inline constexpr int min = CorePrValue::Pcm::Bit::min;
			inline constexpr int max = CorePrValue::Pcm::Bit::max;
			inline constexpr int initial = CorePrValue::Pcm::Bit::initial;
		}

		namespace Rate
		{
			// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 11k 9: 8k 10: 5.5k 11: 4k 12: 2k
			inline constexpr int min = CorePrValue::Pcm::Rate::min;
			inline constexpr int max = CorePrValue::Pcm::Rate::max;
			inline constexpr int initial = CorePrValue::Pcm::Rate::initial;
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
		}

		namespace PitchAdsr
		{
			namespace Bypass
			{
				inline constexpr float initial = false; // 初期値
			}

			namespace Enable
			{
				inline constexpr float initial = true; // 初期値
			}

			namespace Ar
			{
				inline constexpr float min = 0.00f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.0f; // 初期値（秒）
			}

			namespace Dr
			{
				inline constexpr float min = 0.0f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.0f; // 初期値（秒）
			}

			namespace Rr
			{
				inline constexpr float min = 0.00f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.0f; // 初期値（秒）
			}

			namespace Ssl
			{
				inline constexpr int min = -2400; // 最小値
				inline constexpr int max = 2400;  // 最大値
				inline constexpr int initial = 0; // 初期値
			}

			namespace Atl
			{
				inline constexpr int min = -2400; // 最小値
				inline constexpr int max = 2400;  // 最大値
				inline constexpr int initial = 0; // 初期値
			}

			namespace Stl
			{
				inline constexpr int min = -2400; // 最小値
				inline constexpr int max = 2400;  // 最大値
				inline constexpr int initial = 0; // 初期値
			}

			namespace Rll
			{
				inline constexpr int min = -2400; // 最小値
				inline constexpr int max = 2400;  // 最大値
				inline constexpr int initial = 0; // 初期値
			}
		}
	}
}
