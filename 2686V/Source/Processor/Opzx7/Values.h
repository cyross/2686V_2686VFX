#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Values.h"

namespace Opzx7PrValue
{
	inline constexpr float boolThread = CorePrValue::boolThread;

	inline constexpr int ops = 4;
	inline constexpr int pcmIndex = 31;
	inline constexpr int wtIndex = 15;

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
		inline constexpr int max = 35;
		inline constexpr int initial = 15;
	}

	namespace Fb
	{
		inline constexpr int min = CorePrValue::Fm::Fb::min;
		inline constexpr int max = CorePrValue::Fm::Fb::max;
		inline constexpr int initial = CorePrValue::Fm::Fb::initial;
	}

	namespace Panpot
	{
		inline constexpr int min = 0; // 最小値
		inline constexpr int max = 31; // 最大値
		inline constexpr int initial = 15; // 初期値
	}

	namespace PanpotEnable
	{
		inline constexpr bool initial = false; // 初期値
	}

	namespace Op
	{
		namespace PhaseOffset
		{
			inline constexpr float min = 0.00;
			inline constexpr float max = 360.00;
			inline constexpr float initial = 0.00;
		}

		namespace Ws
		{
			inline constexpr int min = 0;
			inline constexpr int max = 72;
			inline constexpr int initial = 0;
		}

		namespace Mul
		{
			inline constexpr int min = 0;
			inline constexpr int max = 21;
			inline constexpr int initial = 2; // x 1.0
		}

		namespace MulRatio
		{
			inline constexpr float min = 0.5f;
			inline constexpr float max = 27.57f;
			inline constexpr float initial = 1.0f;
		}

		namespace Dt1
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
		}

		namespace Dt2
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace Ks
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 3; // 最大値
			inline constexpr int initial = 0; // 最大値
		}

		namespace Se
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 15; // 最大値
			inline constexpr int initial = 0; // 最大値
		}

		namespace SeFreq
		{
			inline constexpr float min = 0.1f; // 最小値
			inline constexpr float max = 30.0f; // 最大値
			inline constexpr float initial = 1.0f; // 初期値
		}

		namespace Sus
		{
			inline constexpr bool initial = false;
		}

		namespace Xof
		{
			inline constexpr bool initial = false;
		}

		namespace Adsr
		{
			namespace Ar
			{
				inline constexpr float min = CorePrValue::Adsr::Ar::min;
				inline constexpr float max = CorePrValue::Adsr::Ar::max;
				inline constexpr float initial = CorePrValue::Adsr::Ar::initial;
			}

			namespace Rr
			{
				inline constexpr float min = CorePrValue::Adsr::Rr::min;
				inline constexpr float max = CorePrValue::Adsr::Rr::max;
				inline constexpr float initial = CorePrValue::Adsr::Rr::initial;
			}

			namespace D1r
			{
				inline constexpr float min = CorePrValue::Adsr::D1r::min;
				inline constexpr float max = CorePrValue::Adsr::D1r::max;
				inline constexpr float initial = CorePrValue::Adsr::D1r::initial;
			}

			namespace D1l
			{
				inline constexpr float min = CorePrValue::Adsr::D1l::min;
				inline constexpr float max = CorePrValue::Adsr::D1l::max;
				inline constexpr float initial = CorePrValue::Adsr::D1l::initial;
			}

			namespace D2r
			{
				inline constexpr float min = CorePrValue::Adsr::D2r::min;
				inline constexpr float max = CorePrValue::Adsr::D2r::max;
				inline constexpr float initial = CorePrValue::Adsr::D2r::initial;
			}

			namespace Tl
			{
				inline constexpr float min = CorePrValue::Op::Tl::min;
				inline constexpr float max = CorePrValue::Op::Tl::max;
				inline constexpr float initial = CorePrValue::Op::Tl::initial;
			}
		}

		namespace RgAdsr {
			namespace Enable
			{
				inline constexpr bool initial = true;
			}

			namespace Ar
			{
				inline constexpr int min = 0;
				inline constexpr int max = 31;
				inline constexpr int initial = 31;
			}

			namespace Rr
			{
				inline constexpr int min = 0;
				inline constexpr int max = 31;
				inline constexpr int initial = 31;
			}

			namespace Tl
			{
				inline constexpr int min = 0;
				inline constexpr int max = 63;
				inline constexpr int initial = 0;
			}

			namespace D1r
			{
				inline constexpr int min = 0;
				inline constexpr int max = 31;
				inline constexpr int initial = 0;
			}

			namespace D1l
			{
				inline constexpr int min = 0;
				inline constexpr int max = 15;
				inline constexpr int initial = 15;
			}

			namespace D2r
			{
				inline constexpr int min = 0;
				inline constexpr int max = 31;
				inline constexpr int initial = 0;
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

		namespace SsgSwEnv
		{
			namespace Bypass
			{
				inline constexpr float initial = true; // 初期値
			}

			namespace Enable
			{
				inline constexpr float initial = false;
			}

			namespace Steps
			{
				inline constexpr int min = 1;
				inline constexpr int max = 5;
				inline constexpr int initial = 5;
			}

			namespace Loop
			{
				inline constexpr float initial = false;
			}

			namespace LoopTo
			{
				inline constexpr int min = 0;
				inline constexpr int max = 3;
				inline constexpr int initial = 0;
			}

			namespace LoopCount
			{
				inline constexpr int min = 0;
				inline constexpr int max = 200;
				inline constexpr int initial = 0;
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

			namespace R6
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

			namespace L5
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}

			namespace L6
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}
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

		namespace Mask
		{
			inline constexpr bool initial = CorePrValue::Op::Mask::initial;
		}

		namespace Lfo
		{
			namespace Freqs
			{
				inline constexpr int min = 0;
				inline constexpr int max = 7;
				inline constexpr int initial = 0;
			}

			namespace Shape
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
			inline constexpr float min = 0.1f;
			inline constexpr float max = 50.0f;
			inline constexpr float initial = 5.0f;
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
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
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
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
		}

		namespace EgShape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
		}
	}
}
