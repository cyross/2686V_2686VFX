#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Values.h"

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
