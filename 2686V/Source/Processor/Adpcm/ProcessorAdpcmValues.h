#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorValues.h"

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
		// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k 7: 22.05k, 8: 16k, 9: 12k, 10: 11k 11: 8k 12: 5.5k 13: 4k 14: 2k
		inline constexpr int min = CorePrValue::Pcm::Rate::min;
		inline constexpr int max = CorePrValue::Pcm::Rate::max;
		inline constexpr int initial = CorePrValue::Pcm::Rate::initial;
	}

	namespace Fix
	{
		inline constexpr bool initial = CorePrValue::FixMode::Enable::initial;
	}

	namespace FixFreq
	{
		inline constexpr float min = CorePrValue::FixMode::Freq::min;
		inline constexpr float max = CorePrValue::FixMode::Freq::max;
		inline constexpr float initial = CorePrValue::FixMode::Freq::initial;
	}

	// ユニゾン・ハーモニー用
	namespace Unison
	{
		namespace Voices
		{
			inline constexpr int min = CorePrValue::Unison::Voices::min;
			inline constexpr int max = CorePrValue::Unison::Voices::max;
			inline constexpr int initial = CorePrValue::Unison::Voices::initial;
		}

		namespace Detune
		{
			inline constexpr int min = CorePrValue::Unison::Detune::min;
			inline constexpr int max = CorePrValue::Unison::Detune::max;
			inline constexpr int initial = CorePrValue::Unison::Detune::initial;
		}

		namespace Spread
		{
			inline constexpr float min = CorePrValue::Unison::Spread::min;
			inline constexpr float max = CorePrValue::Unison::Spread::max;
			inline constexpr float initial = CorePrValue::Unison::Spread::initial;
		}
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

		namespace Kor
		{
			inline constexpr bool initial = false;
		}
	}

	namespace PitchAdsr
	{
		namespace Bypass
		{
			inline constexpr float initial = CorePrValue::PitchAdsr::Bypass::initial;
		}
	}

	namespace Mul
	{
		inline constexpr int min = CorePrValue::Opzx7Detune::Mul::min;
		inline constexpr int max = CorePrValue::Opzx7Detune::Mul::max;
		inline constexpr int initial = CorePrValue::Opzx7Detune::Mul::initial;
	}

	namespace MulRatio
	{
		inline constexpr float min = CorePrValue::Opzx7Detune::MulRatio::min;
		inline constexpr float max = CorePrValue::Opzx7Detune::MulRatio::max;
		inline constexpr float initial = CorePrValue::Opzx7Detune::MulRatio::initial;
	}

	namespace Dt1
	{
		inline constexpr int min = CorePrValue::Opzx7Detune::Dt1::min;
		inline constexpr int max = CorePrValue::Opzx7Detune::Dt1::max;
		inline constexpr int initial = CorePrValue::Opzx7Detune::Dt1::initial;
	}

	namespace Dt2
	{
		inline constexpr int min = CorePrValue::Opzx7Detune::Dt2::min;
		inline constexpr int max = CorePrValue::Opzx7Detune::Dt2::max;
		inline constexpr int initial = CorePrValue::Opzx7Detune::Dt2::initial;
	}

	namespace Dt3
	{
		inline constexpr int min = CorePrValue::Opzx7Detune::Dt3::min;
		inline constexpr int max = CorePrValue::Opzx7Detune::Dt3::max;
		inline constexpr int initial = CorePrValue::Opzx7Detune::Dt3::initial;
	}

	namespace SsgSwEnv
	{
		namespace Bypass
		{
			inline constexpr float initial = CorePrValue::SsgSwEnv::Bypass::initial;
		}

		namespace Steps
		{
			inline constexpr int min = CorePrValue::SsgSwEnv::Steps::min;
			inline constexpr int max = CorePrValue::SsgSwEnv::Steps::max;
			inline constexpr int initial = CorePrValue::SsgSwEnv::Steps::initial;
		}

		namespace Loop
		{
			inline constexpr float initial = CorePrValue::SsgSwEnv::Loop::initial;
		}

		namespace LoopTo
		{
			inline constexpr int min = CorePrValue::SsgSwEnv::LoopTo::min;
			inline constexpr int max = CorePrValue::SsgSwEnv::LoopTo::max;
			inline constexpr int initial = CorePrValue::SsgSwEnv::LoopTo::initial;
		}

		namespace LoopCount
		{
			inline constexpr int min = CorePrValue::SsgSwEnv::LoopCount::min;
			inline constexpr int max = CorePrValue::SsgSwEnv::LoopCount::max;
			inline constexpr int initial = CorePrValue::SsgSwEnv::LoopCount::initial;
		}

		namespace R1
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::R1::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::R1::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::R1::initial;
		}

		namespace R2
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::R2::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::R2::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::R2::initial;
		}

		namespace R3
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::R3::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::R3::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::R3::initial;
		}

		namespace R4
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::R4::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::R4::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::R4::initial;
		}

		namespace R5
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::R5::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::R5::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::R5::initial;
		}

		namespace R6
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::R6::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::R6::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::R6::initial;
		}

		namespace Stl
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::Stl::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::Stl::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::Stl::initial;
		}

		namespace L1
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::L1::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::L1::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::L1::initial;
		}

		namespace L2
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::L2::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::L2::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::L2::initial;
		}

		namespace L3
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::L3::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::L3::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::L3::initial;
		}

		namespace L4
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::L4::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::L4::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::L4::initial;
		}

		namespace L5
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::L5::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::L5::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::L5::initial;
		}

		namespace L6
		{
			inline constexpr float min = CorePrValue::SsgSwEnv::L6::min;
			inline constexpr float max = CorePrValue::SsgSwEnv::L6::max;
			inline constexpr float initial = CorePrValue::SsgSwEnv::L6::initial;
		}
	}
}
