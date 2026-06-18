#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorValues.h"

namespace RhythmPrValue
{
	inline constexpr float boolThread = CorePrValue::boolThread;

	inline constexpr int pads = 8;

	namespace Level
	{
		inline constexpr float min = CorePrValue::Level::min; // 最小値
		inline constexpr float max = CorePrValue::Level::max;  // 最大値
		inline constexpr float initial = CorePrValue::Level::initial; // 初期値
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
			// 1:32bit, 2:24bit, 3:20bit, 4:16bit, 5:12bit, 6:10bit, 7:9bit, 8:8bit, 9:7bit, 10:6bit, 11:5bit, 12:4bit PCM, 13: 4bit ADPCM, 14: 1bit DPCM
			inline constexpr int min = CorePrValue::Pcm::Bit::min;
			inline constexpr int max = CorePrValue::Pcm::Bit::max;
			inline constexpr int initial = CorePrValue::Pcm::Bit::initial;
		}

		namespace Rate
		{
			// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
			inline constexpr int min = CorePrValue::Pcm::Rate::min;
			inline constexpr int max = CorePrValue::Pcm::Rate::max;
			inline constexpr int initial = CorePrValue::Pcm::Rate::initial;
		}

		namespace Interp
		{
			inline constexpr int min = 0; // Nearest
			inline constexpr int max = 2; // Gaussian
			inline constexpr int initial = 1; // Linear
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
	}
}
