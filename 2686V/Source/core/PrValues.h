#pragma once

#include <JuceHeader.h>

namespace PrValue
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

	namespace Adsr
	{
		namespace Ar
		{
			inline constexpr float min = 0.03f; // 最小値（秒）
			inline constexpr float max = 5.0f;  // 最大値（秒）
			inline constexpr float initial = 0.03f; // 初期値（秒）
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

		namespace Rr
		{
			inline constexpr float min = 0.03f; // 最小値（秒）
			inline constexpr float max = 5.0f;  // 最大値（秒）
			inline constexpr float initial = 0.03f; // 初期値（秒）
		}
	}

	namespace Opna
	{
		inline constexpr int ops = 4;

		namespace Alg
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 7; // 最大値
			inline constexpr int initial = 7; // 初期値
		}

		namespace Fb0
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 7.0f; // 最大値
			inline constexpr float initial = 0.0f; // 初期値

		}

		namespace Fb2
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 7.0f; // 最大値
			inline constexpr float initial = 0.0f; // 初期値

		}

		namespace Bit
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 5; // 最大値
			inline constexpr int initial = 5; // 初期値

		}

		namespace Rate
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 7; // 最大値
			inline constexpr int initial = 2; // 初期値
		}

		namespace Lfo
		{
			namespace Freq
			{
				inline constexpr float min = 0.1f;
				inline constexpr float max = 200.0f;
				inline constexpr float initial = 5.0f;
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

			namespace Ar
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
			}

			namespace Dr
			{
				inline constexpr float min = 0.0f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.0f; // 初期値（秒）
			}

			namespace Sr
			{
				inline constexpr float min = 0.0f; // 最小値（秒）
				inline constexpr float max = 10.0f;  // 最大値（秒）
				inline constexpr float initial = 0.0f; // 初期値（秒）
			}

			namespace Rr
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
			}

			namespace Sl
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 1.0f; // 初期値
			}

			namespace Tl
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}

			namespace Ks
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 3; // 最大値
				inline constexpr int initial = 0; // 最大値
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

			namespace Se
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 15; // 最大値
				inline constexpr int initial = 0; // 最大値
			}

			namespace SeFreq
			{
				inline constexpr float min = 0.1f; // 最小値
				inline constexpr float max = 20.0f; // 最大値
				inline constexpr float initial = 1.0f; // 初期値
			}

			namespace Mask
			{
				inline constexpr bool initial = false;

			}
		}
	}

	namespace Opn
	{
		inline constexpr int ops = 4;

		namespace Alg
		{
			inline constexpr int min = 0;
			inline constexpr int max = 7;
			inline constexpr int initial = 7;
		}

		namespace Fb0
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 7.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Fb2
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 7.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Bit
		{
			inline constexpr int min = 1;
			inline constexpr int max = 5;
			inline constexpr int initial = 5;

		}

		namespace Rate
		{
			inline constexpr int min = 1;
			inline constexpr int max = 7;
			inline constexpr int initial = 2;
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

			namespace Ar
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
			}

			namespace Dr
			{
				inline constexpr float min = 0.0f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.0f; // 初期値（秒）
			}

			namespace Sr
			{
				inline constexpr float min = 0.0f; // 最小値（秒）
				inline constexpr float max = 10.0f;  // 最大値（秒）
				inline constexpr float initial = 0.0f; // 初期値（秒）
			}

			namespace Rr
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
			}

			namespace Sl
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 1.0f; // 初期値
			}

			namespace Tl
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}

			namespace Ks
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 3; // 最大値
				inline constexpr int initial = 0; // 最大値
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
				inline constexpr bool initial = false;

			}
		}

	}

	namespace Opl

	{
		inline constexpr int ops = 2;

		namespace Alg
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 1; // 最大値
			inline constexpr int initial = 0; // 初期値
		}

		namespace Fb0
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 7.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Bit
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 5; // 最大値
			inline constexpr int initial = 5; // 初期値

		}

		namespace Rate
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 7; // 最大値
			inline constexpr int initial = 2; // 初期値
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

			namespace Ar
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
			}

			namespace Dr
			{
				inline constexpr float min = 0.0f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.0f; // 初期値（秒）
			}

			namespace Rr
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
			}

			namespace Sl
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 1.0f; // 初期値
			}

			namespace Tl
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
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

			namespace Am
			{
				inline constexpr bool initial = false; // 初期値
			}

			namespace Vib
			{
				inline constexpr bool initial = false; // 初期値
			}

			namespace EgType
			{
				inline constexpr bool initial = true; // 初期値
			}

			namespace Eg
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 3; // 最大値
				inline constexpr int initial = 0; // 最大値
			}

			namespace Mask
			{
				inline constexpr bool initial = false;

			}
		}
	}

	namespace Opl3
	{
		inline constexpr int ops = 4;

		namespace Alg
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 3; // 最大値
			inline constexpr int initial = 3; // 初期値
		}

		namespace Fb0
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 7.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Fb2
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 7.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Bit
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 5; // 最大値
			inline constexpr int initial = 5; // 初期値

		}

		namespace Rate
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 7; // 最大値
			inline constexpr int initial = 2; // 初期値
		}

		namespace Op
		{
			namespace Mul
			{
				inline constexpr int min = 0; // 最小値（倍率）
				inline constexpr int max = 15; // 最大値（倍率）
				inline constexpr int initial = 1; // 初期値（倍率）
			}

			namespace Ar
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
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

			namespace Rr
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
			}

			namespace Tl
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
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

			namespace Am
			{
				inline constexpr bool initial = false; // 初期値
			}

			namespace Vib
			{
				inline constexpr bool initial = false; // 初期値
			}

			namespace EgType
			{
				inline constexpr bool initial = true; // 初期値
			}

			namespace Eg
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 7; // 最大値
				inline constexpr int initial = 0; // 最大値
			}

			namespace Mask
			{
				inline constexpr bool initial = false;

			}
		}
	}

	namespace Opm
	{
		inline constexpr int ops = 4;

		namespace Alg
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 7; // 最大値
			inline constexpr int initial = 7; // 初期値
		}

		namespace Fb0
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 7.0f; // 最大値
			inline constexpr float initial = 0.0f; // 初期値

		}

		namespace Bit
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 5; // 最大値
			inline constexpr int initial = 5; // 初期値

		}

		namespace Rate
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 7; // 最大値
			inline constexpr int initial = 2; // 初期値
		}

		namespace Lfo
		{
			namespace Freq
			{
				inline constexpr float min = 0.1f;
				inline constexpr float max = 200.0f;
				inline constexpr float initial = 5.0f;
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
		}

		namespace Op
		{
			namespace Mul
			{
				inline constexpr int min = 0; // 最小値（倍率）
				inline constexpr int max = 15; // 最大値（倍率）
				inline constexpr int initial = 1; // 初期値（倍率）
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

			namespace Ar
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
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
				inline constexpr float max = 10.0f; // 最大値（秒）
				inline constexpr float initial = 0.0f; // 初期値（秒）
			}

			namespace Rr
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
			}

			namespace Tl
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}

			namespace Ks
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 3; // 最大値
				inline constexpr int initial = 0; // 最大値
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

			namespace Se
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 15; // 最大値
				inline constexpr int initial = 0; // 最大値
			}

			namespace SeFreq
			{
				inline constexpr float min = 0.1f; // 最小値
				inline constexpr float max = 20.0f; // 最大値
				inline constexpr float initial = 1.0f; // 初期値
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

			namespace Mask
			{
				inline constexpr bool initial = false;

			}
		}
	}

	namespace Opzx3
	{
		inline constexpr int ops = 4;

		namespace Alg
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 27; // 最大値
			inline constexpr int initial = 15; // 初期値
		}

		namespace Fb0
		{

			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 7.0f; // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace Bit
		{
			inline constexpr int min = 1;
			inline constexpr int max = 5;
			inline constexpr int initial = 5;

		}

		namespace Rate
		{
			inline constexpr int min = 1;
			inline constexpr int max = 7;
			inline constexpr int initial = 2;
		}

		namespace Lfo
		{
			namespace Freq
			{
				inline constexpr float min = 0.1f;
				inline constexpr float max = 200.0f;
				inline constexpr float initial = 5.0f;
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
		}

		namespace Op
		{
			namespace Mul
			{
				inline constexpr int min = 0; // 最小値（倍率）
				inline constexpr int max = 15; // 最大値（倍率）
				inline constexpr int initial = 1; // 初期値（倍率）
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

			namespace Ar
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
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
				inline constexpr float max = 10.0f; // 最大値（秒）
				inline constexpr float initial = 0.0f; // 初期値（秒）
			}

			namespace Rr
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
			}

			namespace Tl
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}

			namespace Ks
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 3; // 最大値
				inline constexpr int initial = 0; // 最大値
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

			namespace Ws
			{
				inline constexpr int min = 0;
				inline constexpr int max = 29;
				inline constexpr int initial = 0;
			}

			namespace SeFreq
			{
				inline constexpr float min = 0.1f; // 最小値
				inline constexpr float max = 20.0f; // 最大値
				inline constexpr float initial = 1.0f; // 初期値
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

			namespace Mask
			{
				inline constexpr bool initial = false;

			}
		}
	}

	namespace Ssg
	{
		namespace Bit
		{
			inline constexpr int min = 1;
			inline constexpr int max = 5;
			inline constexpr int initial = 3; // 初期値

		}

		namespace Rate
		{
			inline constexpr int min = 1;
			inline constexpr int max = 7;
			inline constexpr int initial = 6; // 初期値
		}

		namespace Tome
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 1.0f; // 初期値

		}

		namespace Noise
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 1.0f; // 初期値

		}

		namespace NoiseFreq
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 30000.0f;
			inline constexpr float initial = 12000.0f; // 初期値
		}

		namespace NoiseMix
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace NoiseOnNote
		{
			inline constexpr float initial = false; // 初期値
		}

		namespace waveForm
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 1; // 最大値
			inline constexpr int initial = 0; // 最大値
		}

		namespace Adsr
		{
			namespace Bypass
			{
				inline constexpr float initial = false; // 初期値
			}
		}

		namespace Duty
		{
			namespace Mode
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 1; // 最大値
				inline constexpr int initial = 0; // 最大値
			}

			namespace Preset
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 8; // 最大値
				inline constexpr int initial = 0; // 最大値

			}

			namespace Var
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 0.5f;  // 最大値
				inline constexpr float initial = 0.5f; // 初期値

			}

			namespace Inv
			{
				inline constexpr float initial = false; // 初期値
			}

		}

		namespace Tri
		{
			namespace KeyTrack
			{
				inline constexpr float initial = true; // 初期値

			}

			namespace Peak
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.5f; // 初期値
			}

			namespace ManualFreq
			{
				inline constexpr float min = 0.0f;
				inline constexpr float max = 8000.0f;
				inline constexpr float initial = 440.0f; // 初期値
			}
		}

		namespace HwEnv
		{
			namespace Enable
			{
				inline constexpr bool initial = false; // 初期値
			}

			namespace Shape
			{
				inline constexpr int min = 0; // 最小値
				inline constexpr int max = 7; // 最大値
				inline constexpr int initial = 0; // 最大値
			}

			namespace Period
			{
				inline constexpr float min = 0.1f; // 最小値
				inline constexpr float max = 200.0f; // 最大値
				inline constexpr float initial = 1.0f; // 初期値
			}
		}
	}

	namespace Wt
	{
		inline constexpr int customSize1 = 32; // 最大値
		inline constexpr int customSize2 = 64; // 最大値

		namespace BIt
		{
			inline constexpr int min = 1;
			inline constexpr int max = 5;
			inline constexpr int initial = 3; // 初期値
		}

		namespace Rate
		{
			inline constexpr int min = 1;
			inline constexpr int max = 7;
			inline constexpr int initial = 6; // 初期値
		}

		namespace SammpleSize
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 1; // 最大値
			inline constexpr int initial = 0; // 最大値
		}

		namespace WaveForm
		{
			inline constexpr int min = 0; // 最小値
			inline constexpr int max = 8; // 最大値
			inline constexpr int initial = 0; // 最大値
		}

		namespace Custom
		{
			inline constexpr float min = -1.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.0f; // 初期値
		}

		namespace Level
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.5f; // 初期値
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
	}

	namespace Rhythm
	{
		inline constexpr int pads = 8;

		namespace Vol
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 1.0f; // 初期値
		}

		namespace Pad
		{
			namespace Vol
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 1.0f; // 初期値
			}
			
			namespace Pan
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.5f; // 初期値
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
				inline constexpr int min = 1; // 最小値
				inline constexpr int max = 7; // 最大値
				inline constexpr int initial = 7; // 初期値
			}

			namespace Rate
			{
				inline constexpr int min = 1; // 最小値
				inline constexpr int max = 7; // 最大値
				inline constexpr int initial = 6; // 初期値
			}

			namespace Rr
			{
				inline constexpr float min = 0.03f; // 最小値（秒）
				inline constexpr float max = 5.0f;  // 最大値（秒）
				inline constexpr float initial = 0.03f; // 初期値（秒）
			}
		}
	}

	namespace Adpcm
	{
		namespace Vol
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 1.0f; // 初期値
		}

		namespace Pan
		{
			inline constexpr float min = 0.0f; // 最小値
			inline constexpr float max = 1.0f;  // 最大値
			inline constexpr float initial = 0.5f; // 初期値

		}

		namespace Loop
		{
			inline constexpr float initial = true; // 初期値
		}

		// 0:Raw, 1:24bit, 2:16bit, 3:8bit, 4:5bit, 5:4bit PCM 6:ADPCM(4bit)
		namespace Bit
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 7; // 最大値
			inline constexpr int initial = 1; // 初期値
		}
		// 0: 96kHz 1: 55.5kHz 2: 48kHz 3: 44.1kHz 4: 22.5kHz 5: 16k 6: 8k

		namespace Rate
		{
			inline constexpr int min = 1; // 最小値
			inline constexpr int max = 7; // 最大値
			inline constexpr int initial = 4; // 初期値
		}
	}

	namespace Beep
	{
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
		}
	}

	namespace Fx
	{
		namespace MBypass
		{
			inline constexpr float initial = false; // 初期値
		}

		namespace Tremolo
		{
			namespace Bypass
			{
				inline constexpr float initial = false; // 初期値
			}

			namespace Rate
			{
				inline constexpr float min = 0.1f; // 最小値
				inline constexpr float max = 20.0f;  // 最大値
				inline constexpr float initial = 5.0f; // 初期値
			}

			namespace Depth
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.5f; // 初期値
			}

			namespace Mix
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}
		}

		namespace Vibrato
		{
			namespace Bypass
			{
				inline constexpr float initial = false; // 初期値
			}

			namespace Rate
			{
				inline constexpr float min = 0.1f; // 最小値
				inline constexpr float max = 10.0f;  // 最大値
				inline constexpr float initial = 2.0f; // 初期値
			}

			namespace Dept
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.5f; // 初期値
			}

			namespace Mix
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}
		}

		namespace Mbc
		{
			namespace Bypass
			{
				inline constexpr float initial = false; // 初期値
			}

			namespace Rate
			{
				inline constexpr float min = 1.0f; // 最小値
				inline constexpr float max = 50.0f;  // 最大値
				inline constexpr float initial = 1.0f; // 初期値
			}

			namespace Bit
			{
				inline constexpr float min = 2.0f; // 最小値
				inline constexpr float max = 24.0f;  // 最大値
				inline constexpr float initial = 24.0f; // 初期値
			}

			namespace Mix
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}
		}
		
		namespace Delay
		{
			namespace Bypass
			{
				inline constexpr float initial = false; // 初期値
			}

			namespace Time
			{
				inline constexpr float min = 10.0f; // 最小値
				inline constexpr float max = 1000.0f;  // 最大値
				inline constexpr float initial = 375.0f; // 初期値
			}

			namespace Fb
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 0.95f;  // 最大値
				inline constexpr float initial = 0.4f; // 初期値
			}

			namespace Mix
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}
		}

		namespace Reverb
		{
			namespace Bypass
			{
				inline constexpr float initial = false; // 初期値
			}

			namespace Size
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.5f; // 初期値
			}

			namespace Damp
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.5f; // 初期値
			}

			namespace Mix
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}
		}

		namespace Rbc
		{
			namespace Bypass
			{
				inline constexpr float initial = false; // 初期値
			}

			namespace Rate
			{
				// 1:Raw, 2:24bit, 3:16bit, 4:8bit, 5:5bit, 6:4bit PCM 7:ADPCM(4bit)
				inline constexpr int min = 1; // 最小値
				inline constexpr int max = 7;  // 最大値
				inline constexpr int initial = 6; // 初期値
			}

			namespace Bit
			{
				// 1: 96kHz 2: 55.5kHz 3: 48kHz 4: 44.1kHz 5: 22.5kHz 6: 16k 7: 8k
				inline constexpr int min = 1; // 最小値
				inline constexpr int max = 7;  // 最大値
				inline constexpr int initial = 7; // 初期値
			}

			namespace Mix
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}
		}

		namespace Filter
		{
			namespace Bypass
			{
				inline constexpr float initial = false; // 初期値
			}

			namespace Type
			{
				inline constexpr int min = 1;
				inline constexpr int max = 3;
				inline constexpr int initial = 1; // 1:LPF, 2:HPF, 3:BPF
			}

			namespace Freq
			{
				inline constexpr float min = 0.0f;
				inline constexpr float max = 20000.0f;
				inline constexpr float initial = 20000.0f;
			}

			namespace Q
			{
				inline constexpr float min = 0.1f;
				inline constexpr float max = 10.0f;
				inline constexpr float initial = 0.707f;
			}

			namespace Mix
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}
		}

		namespace SoftClipper
		{
			namespace Bypass
			{
				inline constexpr float initial = false; // 初期値
			}

			namespace Mix
			{
				inline constexpr float min = 0.0f; // 最小値
				inline constexpr float max = 1.0f;  // 最大値
				inline constexpr float initial = 0.0f; // 初期値
			}
		}
	}
}




