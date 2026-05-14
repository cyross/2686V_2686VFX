#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/Values.h"

namespace Opl3PrValue
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
		inline constexpr int initial = 3; // 49.7kHz
	}

	namespace Alg
	{
		inline constexpr int min = 0;
		inline constexpr int max = 3;
		inline constexpr int initial = 3;
	}

	namespace Fb0
	{
		inline constexpr int min = CorePrValue::Fm::Fb0::min;
		inline constexpr int max = CorePrValue::Fm::Fb0::max;
		inline constexpr int initial = CorePrValue::Fm::Fb0::initial;
	}

	namespace Fb2
	{
		inline constexpr int min = CorePrValue::Fm::Fb2::min;
		inline constexpr int max = CorePrValue::Fm::Fb2::max;
		inline constexpr int initial = CorePrValue::Fm::Fb2::initial;
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
			inline constexpr int max = 9;
			inline constexpr int initial = 0;
		}

		namespace Mask
		{
			inline constexpr bool initial = CorePrValue::Op::Mask::initial;
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
