#pragma once

#include <JuceHeader.h>

namespace CorePrValue
{
	inline constexpr float boolThread = 0.5f; // 0.5以上をON、未満をOFFとみなす閾値

	namespace MasterVol
	{
		inline constexpr float min = -60.0f; // 最小値（dB）
		inline constexpr float max = 6.0f;   // 最大値（dB）
		inline constexpr float interval = 0.1f; // ステップ値（dB）
		inline constexpr float skew = 1.0f; // スキュー（1.0は線形）
		inline constexpr float initial = -3.0f; // デフォルト値（dB）
	}

	namespace MonoMode
	{
		inline constexpr bool initial = true;
	}

	namespace Level
	{
		inline constexpr float min = 0.0f; // 最小値
		inline constexpr float max = 1.0f;  // 最大値
		inline constexpr float initial = 1.0f; // 初期値
	}

	namespace Adsr
	{
		namespace Bypass
		{
			inline constexpr float initial = false; // 初期値
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

		namespace Sl
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 1.0f; // 初期値
		}

		namespace Sr
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

		namespace D1r
		{
			inline constexpr float min = 0.0f; // 最小値（秒）
			inline constexpr float max = 5.0f; // 最大値（秒）
			inline constexpr float initial = 0.0f; // 初期値（秒）
		}

		namespace D1l
		{
			inline constexpr float min = 0.0f; // 最小値（レベル）
			inline constexpr float max = 1.0f; // 最大値（レベル）
			inline constexpr float initial = 1.0f; // 初期値（レベル）
		}

		namespace D2r
		{
			inline constexpr float min = 0.0f; // 最小値（秒）
			inline constexpr float max = 5.0f; // 最大値（秒）
			inline constexpr float initial = 0.0f; // 初期値（秒）
		}

		namespace Stl
		{
			inline constexpr float min = 0.0f; // 最小値（レベル）
			inline constexpr float max = 1.0f; // 最大値（レベル）
			inline constexpr float initial = 0.0f; // 初期値（レベル）
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

		namespace Steps
		{
			inline constexpr int min = 0;
			inline constexpr int max = 4;
			inline constexpr int initial = 4;
		}

		namespace R0
		{
			inline constexpr float min = 0.00f; // 最小値（秒）
			inline constexpr float max = 5.0f;  // 最大値（秒）
			inline constexpr float initial = 0.0f; // 初期値（秒）
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

		namespace Stl
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace L0
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
	}

	namespace Quality
	{
		namespace Bit
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 6; // 最大値
			inline constexpr int initial = 5; // 初期値
		}

		namespace Rate
		{
			inline constexpr int min = 1; // 96kHz
			inline constexpr int max = 13; // 2kHz
			inline constexpr int initial = 2; // 55.5kHz
		}
	}

	namespace Pcm
	{
		namespace Bit
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 7; // 最大値
			inline constexpr int initial = 7; // 初期値
		}

		namespace Rate
		{
			inline constexpr int min = 1; // 2kHz
			inline constexpr int max = 13; // 96kHz
			inline constexpr int initial = 7; // 16kHz
		}

		namespace Offset
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 2000.0f;
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace Ratio
		{
			inline constexpr float min = 0.05f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f; // 初期値
		}
	}

	namespace Fm
	{
		namespace Fb
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 7; // 最大値
			inline constexpr int initial = 0; // 初期値
		}
	}

	namespace Op
	{
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
}
