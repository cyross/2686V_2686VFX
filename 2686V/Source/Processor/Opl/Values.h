#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Values.h"

namespace OplPrValue
{
	inline constexpr float boolThread = CorePrValue::boolThread;

	inline constexpr int ops = 2;

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

	namespace Alg
	{
		inline constexpr int min = 0;
		inline constexpr int max = 1;
		inline constexpr int initial = 0;
	}

	namespace Fb
	{
		inline constexpr int min = CorePrValue::Fm::Fb::min;
		inline constexpr int max = CorePrValue::Fm::Fb::max;
		inline constexpr int initial = CorePrValue::Fm::Fb::initial;
	}

	namespace Op
	{
		namespace RgAdsr
		{
			namespace Ar
			{
				inline constexpr int min = CorePrValue::OplRg::Ar::min;
				inline constexpr int max = CorePrValue::OplRg::Ar::max;
				inline constexpr int initial = CorePrValue::OplRg::Ar::initial;
			}

			namespace Dr
			{
				inline constexpr int min = CorePrValue::OplRg::Dr::min;
				inline constexpr int max = CorePrValue::OplRg::Dr::max;
				inline constexpr int initial = CorePrValue::OplRg::Dr::initial;
			}

			namespace Sl
			{
				inline constexpr int min = CorePrValue::OplRg::Sl::min;
				inline constexpr int max = CorePrValue::OplRg::Sl::max;
				inline constexpr int initial = CorePrValue::OplRg::Sl::initial;
			}

			namespace Rr
			{
				inline constexpr int min = CorePrValue::OplRg::Rr::min;
				inline constexpr int max = CorePrValue::OplRg::Rr::max;
				inline constexpr int initial = CorePrValue::OplRg::Rr::initial;
			}

			namespace Tl
			{
				inline constexpr int min = CorePrValue::OplRg::Tl::min;
				inline constexpr int max = CorePrValue::OplRg::Tl::max;
				inline constexpr int initial = CorePrValue::OplRg::Tl::initial;
			}
		}

		namespace Mul
		{
			inline constexpr int min = 0; // 最小値（倍率）
			inline constexpr int max = 15; // 最大値（倍率）
			inline constexpr int initial = 1; // 初期値（倍率）
		}

		namespace EgType
		{
			inline constexpr bool initial = true; // 初期値
		}

		namespace Ksr
		{
			inline constexpr bool initial = false; // 初期値
		}

		namespace Ksl
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 3; // 最大値
			inline constexpr int initial = 0; // 最大値
		}

		namespace Eg
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace Sus
		{
			inline constexpr bool initial = false;
		}

		namespace Mask
		{
			inline constexpr bool initial = CorePrValue::Op::Mask::initial;
		}

		namespace PitchAdsr
		{
			namespace Enable
			{
				inline constexpr float initial = CorePrValue::PitchAdsr::Enable::initial;
			}

			namespace Ar
			{
				inline constexpr float min = CorePrValue::rateMin;
				inline constexpr float max = CorePrValue::rateMax;
				inline constexpr float initial = CorePrValue::rateMin;
			}

			namespace Dr
			{
				inline constexpr float min = CorePrValue::rateMin;
				inline constexpr float max = CorePrValue::rateMax;
				inline constexpr float initial = CorePrValue::rateMin;
			}

			namespace Rr
			{
				inline constexpr float min = CorePrValue::rateMin;
				inline constexpr float max = CorePrValue::rateMax;
				inline constexpr float initial = CorePrValue::rateMin;
			}

			namespace Ssl
			{
				inline constexpr int min = CorePrValue::pitchLevelMin;
				inline constexpr int max = CorePrValue::pitchLevelMax;
				inline constexpr int initial = CorePrValue::pitchLevelIni;
			}

			namespace Atl
			{
				inline constexpr int min = CorePrValue::pitchLevelMin;
				inline constexpr int max = CorePrValue::pitchLevelMax;
				inline constexpr int initial = CorePrValue::pitchLevelIni;
			}

			namespace Stl
			{
				inline constexpr int min = CorePrValue::pitchLevelMin;
				inline constexpr int max = CorePrValue::pitchLevelMax;
				inline constexpr int initial = CorePrValue::pitchLevelIni;
			}

			namespace Rll
			{
				inline constexpr int min = CorePrValue::pitchLevelMin;
				inline constexpr int max = CorePrValue::pitchLevelMax;
				inline constexpr int initial = CorePrValue::pitchLevelIni;
			}
		}

		namespace SsgSwEnv
		{
			namespace Enable
			{
				inline constexpr float initial = CorePrValue::SsgSwEnv::Enable::initial;
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
			namespace Am
			{
				inline constexpr bool initial = false; // 初期値
			}

			namespace Ams
			{
				inline constexpr float min = 1.0f;
				inline constexpr float max = 20.0f;
				inline constexpr float initial = 3.7f;
			}

			namespace Amd
			{
				inline constexpr float min = 0.1f;
				inline constexpr float max = 10.0f;
				inline constexpr float initial = 1.0f;
			}

			namespace Vib
			{
				inline constexpr bool initial = false; // 初期値
			}

			namespace Pms
			{
				inline constexpr float min = 1.0f;
				inline constexpr float max = 20.0f;
				inline constexpr float initial = 6.4f;
			}

			namespace Pmd
			{
				inline constexpr float min = 1.0f;
				inline constexpr float max = 100.0f;
				inline constexpr float initial = 7.0f;
			}
		}
	}
}
