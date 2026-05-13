#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Values.h"

namespace PrValue
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
			namespace Rr
			{
				inline constexpr float min = CorePrValue::Adsr::Rr::min;
				inline constexpr float max = CorePrValue::Adsr::Rr::max;
				inline constexpr float initial = CorePrValue::Adsr::Rr::initial;
			}
		}
	}
}
