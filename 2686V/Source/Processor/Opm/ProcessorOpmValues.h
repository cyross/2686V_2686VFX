#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorValues.h"

namespace OpmPrValue
{
	inline constexpr float boolThread = CorePrValue::boolThread;

	inline constexpr int ops = 4;

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
		inline constexpr int initial = CorePrValue::Quality::Rate::initial;
	}

	namespace Alg
	{
		inline constexpr int min = 0;
		inline constexpr int max = 7;
		inline constexpr int initial = 7;
	}

	namespace Fb
	{
		inline constexpr int min = CorePrValue::Fm::Fb::min;
		inline constexpr int max = CorePrValue::Fm::Fb::max;
		inline constexpr int initial = CorePrValue::Fm::Fb::initial;
	}

	namespace Pan
	{
		inline constexpr int min = -1;
		inline constexpr int max = 1;
		inline constexpr int initial = 0;
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

	namespace Op
	{
		namespace RgAdsr
		{
			namespace Ar
			{
				inline constexpr int min = CorePrValue::FmRg::Ar::min;
				inline constexpr int max = CorePrValue::FmRg::Ar::max;
				inline constexpr int initial = CorePrValue::FmRg::Ar::initial;
			}

			namespace Rr
			{
				inline constexpr int min = CorePrValue::FmRg::Rr::min;
				inline constexpr int max = CorePrValue::FmRg::Rr::max;
				inline constexpr int initial = CorePrValue::FmRg::Rr::initial;
			}

			namespace Tl
			{
				inline constexpr int min = CorePrValue::FmRg::Tl::min;
				inline constexpr int max = CorePrValue::FmRg::Tl::max;
				inline constexpr int initial = CorePrValue::FmRg::Tl::initial;
			}

			namespace D1r
			{
				inline constexpr int min = CorePrValue::FmRg::D1r::min;
				inline constexpr int max = CorePrValue::FmRg::D1r::max;
				inline constexpr int initial = CorePrValue::FmRg::D1r::initial;
			}

			namespace D1l
			{
				inline constexpr int min = CorePrValue::FmRg::D1l::min;
				inline constexpr int max = CorePrValue::FmRg::D1l::max;
				inline constexpr int initial = CorePrValue::FmRg::D1l::initial;
			}

			namespace D2r
			{
				inline constexpr int min = CorePrValue::FmRg::D2r::min;
				inline constexpr int max = CorePrValue::FmRg::D2r::max;
				inline constexpr int initial = CorePrValue::FmRg::D2r::initial;
			}
		}
		namespace Mul
		{
			inline constexpr int min = 0; // 最小値（倍率）
			inline constexpr int max = 15; // 最大値（倍率）
			inline constexpr int initial = 1; // 初期値（倍率）
		}

		namespace Dt
		{
			inline constexpr int min = 0;
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
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

		namespace Tl
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace EgType
		{
			inline constexpr bool initial = true; // 初期値
		}

		namespace Ks
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 3; // 最大値
			inline constexpr int initial = 0; // 最大値
		}

		namespace SeFreq
		{
			inline constexpr float min = 0.1f; // 最小値
			inline constexpr float max = 30.0f; // 最大値
			inline constexpr float initial = 1.0f; // 初期値
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

		namespace Xof
		{
			inline constexpr bool initial = false;
		}

		namespace Kor
		{
			inline constexpr bool initial = false;
		}

		namespace AmpBypass
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
			namespace Freqs
			{
				inline constexpr int min = 0;
				inline constexpr int max = 7;
				inline constexpr int initial = 0;
			}

			namespace Am
			{
				inline constexpr bool initial = false; // 初期値
			}

			namespace Ams
			{
				inline constexpr int min = 0;
				inline constexpr int max = 3;
				inline constexpr int initial = 0;
			}

			namespace AmsEn
			{
				inline constexpr bool initial = true; // 初期値
			}

			namespace Vib
			{
				inline constexpr bool initial = false; // 初期値
			}

			namespace Pms
			{
				inline constexpr int min = 0;
				inline constexpr int max = 7;
				inline constexpr int initial = 0;
			}

			namespace SyncDelay
			{
				inline constexpr int min = 0;
				inline constexpr int max = 255;
				inline constexpr int initial = 0;
			}
		}
	}

	namespace Lfo
	{
		namespace Freq
		{
			inline constexpr int min = 0;
			inline constexpr int max = 255;
			inline constexpr int initial = 0;
		}

		namespace N88Freq
		{
			inline constexpr int min = 0;
			inline constexpr int max = 16383;
			inline constexpr int initial = 0;
		}

		namespace Shape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 4;
			inline constexpr int initial = 1;
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
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace Amd
		{
			inline constexpr int min = 0;
			inline constexpr int max = 127;
			inline constexpr int initial = 0;
		}

		namespace AmsFloat
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace AmdFloat
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace N88Amd
		{
			inline constexpr int min = -127;
			inline constexpr int max = 127;
			inline constexpr int initial = 0;
		}

		namespace N88Ams
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
		}

		namespace Pm
		{
			inline constexpr bool initial = false;
		}

		namespace Pms
		{
			inline constexpr int min = 0;
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
		}

		namespace Pmd
		{
			inline constexpr int min = 0;
			inline constexpr int max = 127;
			inline constexpr int initial = 0;
		}

		namespace PmsFloat
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace PmdFloat
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace N88Pmd
		{
			inline constexpr int min = -127;
			inline constexpr int max = 127;
			inline constexpr int initial = 0;
		}

		namespace N88Pms
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
		}

		namespace SyncDelay
		{
			inline constexpr int min = 0;
			inline constexpr int max = 255;
			inline constexpr int initial = 0;
		}

		namespace SyncDelayOpm
		{
			inline constexpr int min = 0;
			inline constexpr int max = 127;
			inline constexpr int initial = 0;
		}

		namespace PgShape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace EgShape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}
	}
}
