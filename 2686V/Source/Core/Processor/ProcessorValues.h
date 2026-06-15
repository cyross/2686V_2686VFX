#pragma once

#include <JuceHeader.h>

#include "../Const/ConstGlobal.h"

namespace CorePrValue
{
	inline constexpr float boolThread = 0.5f; // 0.5以上をON、未満をOFFとみなす閾値
	inline constexpr float rateMin = 0.001f;
	inline constexpr float rateMax = Global::RateMaxSeconds::real;
	inline constexpr float levelMin = 0.0;
	inline constexpr float levelMax = 1.0f;
	inline constexpr int pitchLevelMin = -4800;
	inline constexpr int pitchLevelMax = 4800;
	inline constexpr int pitchLevelIni = 0;

	namespace MonoMode
	{
		inline constexpr bool initial = true;
	}

	namespace UseVelocity
	{
		inline constexpr bool initial = true;
	}

	namespace PitchResetOnLegato
	{
		inline constexpr bool initial = true;
	}

	namespace FixedVelocity
	{
		inline constexpr float min = 0.0f;
		inline constexpr float max = 1.0f;
		inline constexpr float initial = 1.0f;
	}

	namespace Level
	{
		inline constexpr float min = CorePrValue::levelMin;
		inline constexpr float max = CorePrValue::levelMax;
		inline constexpr float initial = CorePrValue::levelMax;
	}

	namespace Unison
	{
		namespace Voices
		{
			inline constexpr int min = 1;
			inline constexpr int max = Global::unisonVoices;
			inline constexpr int initial = 1;
		}

		namespace Detune
		{
			inline constexpr int min = -4800;
			inline constexpr int max = 4800;
			inline constexpr int initial = 0;
		}

		namespace Spread
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.5f;
		}
	}

	namespace Adsr
	{
		namespace Bypass
		{
			inline constexpr float initial = false;
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

		namespace Sl
		{
			inline constexpr float min = CorePrValue::levelMin;
			inline constexpr float max = CorePrValue::levelMax;
			inline constexpr float initial = CorePrValue::levelMax;
		}

		namespace Sr
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

		namespace D1r
		{
			inline constexpr float min = CorePrValue::rateMin;
			inline constexpr float max = CorePrValue::rateMax;
			inline constexpr float initial = CorePrValue::rateMin;
		}

		namespace D1l
		{
			inline constexpr float min = CorePrValue::levelMin;
			inline constexpr float max = CorePrValue::levelMax;
			inline constexpr float initial = CorePrValue::levelMax;
		}

		namespace D2r
		{
			inline constexpr float min = CorePrValue::rateMin;
			inline constexpr float max = CorePrValue::rateMax;
			inline constexpr float initial = CorePrValue::rateMin;
		}

		namespace Stl
		{
			inline constexpr float min = CorePrValue::levelMin;
			inline constexpr float max = CorePrValue::levelMax;
			inline constexpr float initial = CorePrValue::levelMin;
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
			inline constexpr float initial = false;
		}

		namespace Enable
		{
			inline constexpr float initial = true;
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
		namespace Bypass
		{
			inline constexpr float initial = true;
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
			inline constexpr float min = CorePrValue::rateMin;
			inline constexpr float max = CorePrValue::rateMax;
			inline constexpr float initial = CorePrValue::rateMin;
		}

		namespace R2
		{
			inline constexpr float min = CorePrValue::rateMin;
			inline constexpr float max = CorePrValue::rateMax;
			inline constexpr float initial = CorePrValue::rateMin;
		}

		namespace R3
		{
			inline constexpr float min = CorePrValue::rateMin;
			inline constexpr float max = CorePrValue::rateMax;
			inline constexpr float initial = CorePrValue::rateMin;
		}

		namespace R4
		{
			inline constexpr float min = CorePrValue::rateMin;
			inline constexpr float max = CorePrValue::rateMax;
			inline constexpr float initial = CorePrValue::rateMin;
		}

		namespace R5
		{
			inline constexpr float min = CorePrValue::rateMin;
			inline constexpr float max = CorePrValue::rateMax;
			inline constexpr float initial = CorePrValue::rateMin;
		}

		namespace R6
		{
			inline constexpr float min = CorePrValue::rateMin;
			inline constexpr float max = CorePrValue::rateMax;
			inline constexpr float initial = CorePrValue::rateMin;
		}

		namespace Stl
		{
			inline constexpr float min = CorePrValue::levelMin;
			inline constexpr float max = CorePrValue::levelMax;
			inline constexpr float initial = CorePrValue::levelMax;
		}

		namespace L1
		{
			inline constexpr float min = CorePrValue::levelMin;
			inline constexpr float max = CorePrValue::levelMax;
			inline constexpr float initial = CorePrValue::levelMax;
		}

		namespace L2
		{
			inline constexpr float min = CorePrValue::levelMin;
			inline constexpr float max = CorePrValue::levelMax;
			inline constexpr float initial = CorePrValue::levelMax;
		}

		namespace L3
		{
			inline constexpr float min = CorePrValue::levelMin;
			inline constexpr float max = CorePrValue::levelMax;
			inline constexpr float initial = CorePrValue::levelMax;
		}

		namespace L4
		{
			inline constexpr float min = CorePrValue::levelMin;
			inline constexpr float max = CorePrValue::levelMax;
			inline constexpr float initial = CorePrValue::levelMax;
		}

		namespace L5
		{
			inline constexpr float min = CorePrValue::levelMin;
			inline constexpr float max = CorePrValue::levelMax;
			inline constexpr float initial = CorePrValue::levelMax;
		}

		namespace L6
		{
			inline constexpr float min = CorePrValue::levelMin;
			inline constexpr float max = CorePrValue::levelMax;
			inline constexpr float initial = CorePrValue::levelMin;
		}
	}

	namespace Quality
	{
		namespace Bit
		{
			// 1:4bit, 2:5bit, 3:6bit, 4:7bit, 5:8bit, 6:9bit, 7:10bit, 8:12bit, 9:16bit, 10:20bit, 11:24bit, 12:raw(32bit)
			inline constexpr int min = 1; // 4-bit (16 steps)
			inline constexpr int max = 12; // Raw
			inline constexpr int initial = 9; // 16-bit
		}

		namespace Rate
		{
			// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
			inline constexpr int min = 1; // 96kHz
			inline constexpr int max = 15; // 2kHz
			inline constexpr int initial = 2; // 55.5kHz
		}
	}

	namespace Pcm
	{
		namespace Bit
		{
			// 1:32bit, 2:24bit, 3:20bit, 4:16bit, 5:12bit, 6:10bit, 7:9bit, 8:8bit, 9:7bit, 10:6bit, 11:5bit, 12:4bit PCM, 13: 4bit ADPCM, 14: 1bit DPCM
			inline constexpr int min = 1; // Raw (32bit)
			inline constexpr int max = 14; // 1-bit DPCM
			inline constexpr int initial = 13; // 4-bit ADPCM
		}

		namespace Rate
		{
			// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
			inline constexpr int min = 1; // 96kHz
			inline constexpr int max = 15; // 2kHz
			inline constexpr int initial = 9; // 16kHz
		}

		namespace Offset
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 2000.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Ratio
		{
			inline constexpr float min = 0.05f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f;
		}
	}

	namespace Fm
	{
		namespace Fb
		{
			inline constexpr int min = 0;
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
		}
	}

	namespace Op
	{
		namespace Mul
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 1;
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
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f;
		}

		namespace EgType
		{
			inline constexpr bool initial = true;
		}

		namespace Ks
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace Ksr
		{
			inline constexpr bool initial = false;
		}

		namespace Ksl
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace Se
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
		}

		namespace SeFreq
		{
			inline constexpr float min = 0.1f;
			inline constexpr float max = 30.0f;
			inline constexpr float initial = 1.0f;
		}

		namespace Mask
		{
			inline constexpr bool initial = false;
		}
	}

	namespace FmRg
	{
		namespace En
		{
			inline constexpr bool initial = true;
		}

		namespace Ar
		{
			inline constexpr int min = 0;
			inline constexpr int max = 31;
			inline constexpr int initial = 31;
		}

		namespace Dr
		{
			inline constexpr int min = 0;
			inline constexpr int max = 31;
			inline constexpr int initial = 0;
		}

		namespace Sl
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 15;
		}

		namespace Sr
		{
			inline constexpr int min = 0;
			inline constexpr int max = 31;
			inline constexpr int initial = 0;
		}

		namespace Rr
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 15;
		}

		namespace Tl
		{
			inline constexpr int min = 0;
			inline constexpr int max = 127;
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

	namespace OplRg
	{
		namespace En
		{
			inline constexpr bool initial = true;
		}

		namespace Ar
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 15;
		}

		namespace Dr
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
		}

		namespace Sl
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 15;
		}

		namespace Rr
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 15;
		}

		namespace Tl
		{
			inline constexpr int min = 0;
			inline constexpr int max = 63;
			inline constexpr int initial = 0;
		}
	}

	namespace Opzx7Detune {
		namespace Mul
		{
			inline constexpr int min = 0; // 0.5x
			inline constexpr int max = 21; // Ratio
			inline constexpr int initial = 2; // 1.0x
		}

		namespace MulRatio
		{
			inline constexpr float min = 0.01f;
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

		namespace Dt3
		{
			inline constexpr int min = -4800;
			inline constexpr int max = 4800;
			inline constexpr int initial = 0;
		}
	}

	namespace FixMode {
		namespace Enable
		{
			inline constexpr bool initial = false;
		}

		namespace Freq
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 8000.0f;
			inline constexpr float initial = 440.0f;
		}
	}

	namespace Opzx7Lfo
	{
		namespace AmFreq
		{
			inline constexpr float min = 0.1f;
			inline constexpr float max = 50.0f;
			inline constexpr float initial = 5.0f;
		}

		namespace PmFreq
		{
			inline constexpr float min = 0.1f;
			inline constexpr float max = 50.0f;
			inline constexpr float initial = 5.0f;
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
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Amd
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Pm
		{
			inline constexpr bool initial = false;
		}

		namespace Pms
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Pmd
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace SyncDelay
		{
			inline constexpr int min = 0;
			inline constexpr int max = 255;
			inline constexpr int initial = 0;
		}

		namespace PmShape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
		}

		namespace AmShape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
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
