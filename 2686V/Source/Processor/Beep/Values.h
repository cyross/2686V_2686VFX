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
}
