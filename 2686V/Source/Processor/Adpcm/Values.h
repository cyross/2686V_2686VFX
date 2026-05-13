#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Values.h"

namespace PrValue
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
	}
}
