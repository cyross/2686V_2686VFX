#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorValues.h"

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
		// 1:4bit, 2:5bit, 3:6bit, 4:7bit, 5:8bit, 6:9bit, 7:10bit, 8:12bit, 9:16bit, 10:20bit, 11:24bit, 12:raw(32bit)
		inline constexpr int min = CorePrValue::Quality::Bit::min;
		inline constexpr int max = CorePrValue::Quality::Bit::max;
		inline constexpr int initial = CorePrValue::Quality::Bit::initial;
	}

	namespace Rate
	{
		// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
		inline constexpr int min = CorePrValue::Quality::Rate::min;
		inline constexpr int max = CorePrValue::Quality::Rate::max;
		inline constexpr int initial = 3; // 49.7kHz
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
		inline constexpr int initial = 8; // 最大値
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

	namespace Lfo
	{
		namespace AmFreq
		{
			inline constexpr float min = CorePrValue::Opzx7Lfo::AmFreq::min;
			inline constexpr float max = CorePrValue::Opzx7Lfo::AmFreq::max;
			inline constexpr float initial = CorePrValue::Opzx7Lfo::AmFreq::initial;
		}

		namespace PmFreq
		{
			inline constexpr float min = CorePrValue::Opzx7Lfo::PmFreq::min;
			inline constexpr float max = CorePrValue::Opzx7Lfo::PmFreq::max;
			inline constexpr float initial = CorePrValue::Opzx7Lfo::PmFreq::initial;
		}

		namespace AmSmRt
		{
			inline constexpr float min = CorePrValue::Opzx7Lfo::AmSmRt::min;
			inline constexpr float max = CorePrValue::Opzx7Lfo::AmSmRt::max;
			inline constexpr float initial = CorePrValue::Opzx7Lfo::AmSmRt::initial;
		}

		namespace Am
		{
			inline constexpr bool initial = CorePrValue::Opzx7Lfo::Am::initial;
		}

		namespace Ams
		{
			inline constexpr float min = CorePrValue::Opzx7Lfo::Ams::min;
			inline constexpr float max = CorePrValue::Opzx7Lfo::Ams::max;
			inline constexpr float initial = CorePrValue::Opzx7Lfo::Ams::initial;
		}

		namespace Amd
		{
			inline constexpr float min = CorePrValue::Opzx7Lfo::Amd::min;
			inline constexpr float max = CorePrValue::Opzx7Lfo::Amd::max;
			inline constexpr float initial = CorePrValue::Opzx7Lfo::Amd::initial;
		}

		namespace Pm
		{
			inline constexpr bool initial = CorePrValue::Opzx7Lfo::Pm::initial;
		}

		namespace Pms
		{
			inline constexpr float min = CorePrValue::Opzx7Lfo::Pms::min;
			inline constexpr float max = CorePrValue::Opzx7Lfo::Pms::max;
			inline constexpr float initial = CorePrValue::Opzx7Lfo::Pms::initial;
		}

		namespace Pmd
		{
			inline constexpr float min = CorePrValue::Opzx7Lfo::Pmd::min;
			inline constexpr float max = CorePrValue::Opzx7Lfo::Pmd::max;
			inline constexpr float initial = CorePrValue::Opzx7Lfo::Pmd::initial;
		}

		namespace SyncDelay
		{
			inline constexpr int min = CorePrValue::Opzx7Lfo::SyncDelay::min;
			inline constexpr int max = CorePrValue::Opzx7Lfo::SyncDelay::max;
			inline constexpr int initial = CorePrValue::Opzx7Lfo::SyncDelay::initial;
		}

		namespace PmShape
		{
			inline constexpr int min = CorePrValue::Opzx7Lfo::PmShape::min;
			inline constexpr int max = CorePrValue::Opzx7Lfo::PmShape::max;
			inline constexpr int initial = CorePrValue::Opzx7Lfo::PmShape::initial;
		}

		namespace AmShape
		{
			inline constexpr int min = CorePrValue::Opzx7Lfo::AmShape::min;
			inline constexpr int max = CorePrValue::Opzx7Lfo::AmShape::max;
			inline constexpr int initial = CorePrValue::Opzx7Lfo::AmShape::initial;
		}
	}
}
