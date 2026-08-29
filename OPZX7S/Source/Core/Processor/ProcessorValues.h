#pragma once

#include <JuceHeader.h>

#include "../Const/ConstGlobal.h"

namespace CPV
{
	inline constexpr float boolThread = 0.5f; // 0.5以上をON、未満をOFFとみなす閾値
	inline constexpr float rateMin = 0.001f;
	inline constexpr float rateMax = Global::RateMaxSeconds::real;
	inline constexpr float levelMin = 0.0;
	inline constexpr float levelMax = 10.0f;
	inline constexpr float levelIni = 1.0f;
	inline constexpr int pitchLevelMin = -4800;
	inline constexpr int pitchLevelMax = 4800;
	inline constexpr int pitchLevelIni = 0;

	namespace Midi
	{
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
	}

	namespace Level
	{
		inline constexpr float min = CPV::levelMin;
		inline constexpr float max = CPV::levelMax;
		inline constexpr float initial = CPV::levelIni;
	}

	namespace Vol
	{
		inline constexpr float min = CPV::levelMin;
		inline constexpr float max = CPV::levelMax;
		inline constexpr float initial = CPV::levelIni;
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

	namespace Loop
	{
		inline constexpr float initial = true;
	}

	namespace SsgWaveForm
	{
		inline constexpr int min = 0; // 最小値
		inline constexpr int max = 1; // 最大値
		inline constexpr int initial = 0; // 最大値
	}

	namespace Tn
	{
		namespace Tone
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

		namespace Freq
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 30000.0f;
			inline constexpr float initial = 12000.0f; // 初期値
		}

		namespace NON
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

	namespace Wt {
		namespace SammpleSize
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace Steps
		{
			// 0, +16, +32, +64, +128, +256, -16, -32, -64, -128, -256
			inline constexpr int min = 0;
			inline constexpr int max = 10;
			inline constexpr int initial = 0;
		}

		namespace WaveForm
		{
			inline constexpr int min = 0;
			inline constexpr int max = 8;
			inline constexpr int initial = 8;
		}

		namespace CustomValue
		{
			inline constexpr float min = -1.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		inline constexpr int CustomSize32 = 32;
		inline constexpr int CustomSize64 = 64;
		inline constexpr int CustomSize128 = 128;
		inline constexpr int CustomSize256 = 256;
	}

	namespace Wt2 {
		namespace WaveForm
		{
			inline constexpr int min = 0;
			inline constexpr int max = 8;
			inline constexpr int initial = 8;
		}

		namespace SammpleSize
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace Resolution
		{
			// 16, 32, 64, 128, 256
			inline constexpr int min = 0;
			inline constexpr int max = 4;
			inline constexpr int initial = 0;
		}

		namespace CustomValue
		{
			inline constexpr int min = 0;
			inline constexpr int max = 255;
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

	namespace PcmPan
	{
		inline constexpr float min = 0.0f;
		inline constexpr float max = 1.0f;
		inline constexpr float initial = 0.5f;
	}

	namespace OpmPan {
		inline constexpr int min = -1;
		inline constexpr int max = 1;
		inline constexpr int initial = 0;
	}

	namespace OpnaPan
	{
		inline constexpr int min = -1;
		inline constexpr int max = 1;
		inline constexpr int initial = 0;
	}

	namespace Panpot {
		namespace Enable
		{
			inline constexpr bool initial = false;
		}

		namespace Panpot
		{
			inline constexpr int min = 0;
			inline constexpr int max = 31;
			inline constexpr int initial = 15;
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

	namespace QualityPcm
	{
		namespace Bit
		{
			// 1:32bit, 2:24bit, 3:20bit, 4:16bit, 5:12bit, 6:10bit, 7:9bit, 8:8bit, 9:7bit, 10:6bit, 11:5bit, 12:4bit PCM, 13: 4bit ADPCM, 14: 1bit DPCM
			inline constexpr int min = 1; // Raw (32bit)
			inline constexpr int max = 21; // K054539
			inline constexpr int initial = 13; // 4-bit ADPCM
		}

		namespace Rate
		{
			// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
			inline constexpr int min = 1; // 96kHz
			inline constexpr int max = 15; // 2kHz
			inline constexpr int initial = 9; // 16kHz
		}

		namespace Interp
		{
			// 0:Nearest 1:Linear 2:Gaussian 3:Zero-Order Hold 4:Cosine 5:B-Spline 6:Lagrange
			inline constexpr int min = 0;
			inline constexpr int max = 6;
			inline constexpr int initial = 1;
		}
	}

	namespace Pcm
	{
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

	namespace Lp
	{
		namespace Enable
		{
			inline constexpr float initial = false;
		}

		namespace Start
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 0.999999f;
			inline constexpr float initial = 0.0f;
		}

		namespace End
		{
			inline constexpr float min = 0.000001f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f;
		}

	}

	namespace Alg
	{
		inline constexpr int min = 0;
	}

	namespace Fb
	{
		inline constexpr int min = 0;
		inline constexpr int max = 7;
		inline constexpr int initial = 0;
	}

	namespace Opzx7Fb
	{
		inline constexpr float min = -8.0f;
		inline constexpr float max = 8.0f;
		inline constexpr float initial = 0.0f;
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

		namespace ArpEnable
		{
			inline constexpr bool initial = false;
		}

		// 疑似高速アルペジオの切り替え速度 (Hz)
		namespace ArpFreq
		{
			inline constexpr int min = 1;
			inline constexpr int max = 4000;
			inline constexpr int initial = 60;
		}

		// 低速時のクリック対策ランプ (OFF ならハードゲート)
		namespace ArpSmooth
		{
			inline constexpr bool initial = true;
		}

		// ボイス単位の定位オフセット (Distance に加算)
		namespace ParaDistance
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		// ボイス単位のデチューン (従来のデチューンに加算)
		namespace ParaDetune
		{
			inline constexpr int min = -4800;
			inline constexpr int max = 4800;
			inline constexpr int initial = 0;
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
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace Dr
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace Sl
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f;
		}

		namespace Sr
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace Rr
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace D1r
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace D1l
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f;
		}

		namespace D2r
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace Stl
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
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

		namespace R
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace L
		{
			inline constexpr int min = CPV::pitchLevelMin;
			inline constexpr int max = CPV::pitchLevelMax;
			inline constexpr int initial = CPV::pitchLevelIni;
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

		namespace R
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace L
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f;
		}

		namespace SRL
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}
	}

	namespace SsgSwEnv11
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
			inline constexpr int max = 10;
			inline constexpr int initial = 10;
		}

		namespace Loop
		{
			inline constexpr float initial = false;
		}

		namespace LoopTo
		{
			inline constexpr int min = 0;
			inline constexpr int max = 8;
			inline constexpr int initial = 0;
		}

		namespace LoopCount
		{
			inline constexpr int min = 0;
			inline constexpr int max = 200;
			inline constexpr int initial = 0;
		}

		namespace R
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace L
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f;
		}

		namespace SRL
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}
	}

	namespace SsgSwPEnv11
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
			inline constexpr int max = 10;
			inline constexpr int initial = 10;
		}

		namespace Loop
		{
			inline constexpr float initial = false;
		}

		namespace LoopTo
		{
			inline constexpr int min = 0;
			inline constexpr int max = 8;
			inline constexpr int initial = 0;
		}

		namespace LoopCount
		{
			inline constexpr int min = 0;
			inline constexpr int max = 200;
			inline constexpr int initial = 0;
		}

		namespace R
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace L
		{
			inline constexpr int min = CPV::pitchLevelMin;
			inline constexpr int max = CPV::pitchLevelMax;
			inline constexpr int initial = CPV::pitchLevelIni;
		}
	}

	namespace OpSsgEnv {
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
	}

	namespace OpMask
	{
		inline constexpr bool initial = false;
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
			inline constexpr int initial = 15;
		}

		namespace Sl
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
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
			inline constexpr int max = 63;
			inline constexpr int initial = 0;
		}

		namespace EgType
		{
			inline constexpr bool initial = true;
		}

		namespace Sus
		{
			inline constexpr bool initial = false;
		}

		namespace Xof
		{
			inline constexpr bool initial = false;
		}

		namespace Kor
		{
			inline constexpr bool initial = false;
		}

		namespace Bypass
		{
			inline constexpr bool initial = false;
		}
	}

	namespace OpmRg
	{
		namespace Ar
		{
			inline constexpr int min = 0;
			inline constexpr int max = 31;
			inline constexpr int initial = 31;
		}

		namespace D1r
		{
			inline constexpr int min = 0;
			inline constexpr int max = 31;
			inline constexpr int initial = 31;
		}

		namespace D1l
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
		}

		namespace D2r
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

		namespace Xof
		{
			inline constexpr bool initial = false;
		}

		namespace Kor
		{
			inline constexpr bool initial = false;
		}

		namespace Bypass
		{
			inline constexpr bool initial = false;
		}
	}

	namespace OpnRg
	{
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
			inline constexpr int initial = 31;
		}

		namespace Sl
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
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

		namespace Sus
		{
			inline constexpr bool initial = false;
		}

		namespace Xof
		{
			inline constexpr bool initial = false;
		}

		namespace Kor
		{
			inline constexpr bool initial = false;
		}

		namespace Bypass
		{
			inline constexpr bool initial = false;
		}
	}

	namespace Opzx7Rg {
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
			inline constexpr int initial = 31;
		}

		namespace D1l
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
		}

		namespace D2r
		{
			inline constexpr int min = 0;
			inline constexpr int max = 31;
			inline constexpr int initial = 0;
		}

		namespace Sus
		{
			inline constexpr bool initial = false;
		}

		namespace Xof
		{
			inline constexpr bool initial = false;
		}

		namespace Kor
		{
			inline constexpr bool initial = false;
		}

		namespace Bypass
		{
			inline constexpr bool initial = false;
		}
	}

	namespace Opzx7Real {
		namespace Ar
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace D1r
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace D1l
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f;
		}

		namespace D2r
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace Rr
		{
			inline constexpr float min = CPV::rateMin;
			inline constexpr float max = CPV::rateMax;
			inline constexpr float initial = CPV::rateMin;
		}

		namespace Tl
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f;
		}

		namespace Sus
		{
			inline constexpr bool initial = false;
		}

		namespace Xof
		{
			inline constexpr bool initial = false;
		}

		namespace Kor
		{
			inline constexpr bool initial = false;
		}

		namespace Bypass
		{
			inline constexpr bool initial = false;
		}
	}

	namespace OplKs {
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
	}

	namespace OpmKs {
		namespace KsMode
		{
			inline constexpr int min = 0;
			inline constexpr int max = 1;
			inline constexpr int initial = 0;
		}

		namespace Ks
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace KsrOPP
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace KslOPP
		{
			inline constexpr int min = 0;
			inline constexpr int max = 99;
			inline constexpr int initial = 0;
		}
	}

	namespace OpnKs {
		namespace Ks
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}
	}

	namespace Opzx7Ks {
		namespace KsEn
		{
			inline constexpr bool initial = false;
		}

		namespace KsMode
		{
			inline constexpr int min = 0;
			inline constexpr int max = 2;
			inline constexpr int initial = 0;
		}

		namespace KsMA7
		{
			inline constexpr bool initial = false;
		}

		namespace KslMA7
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace KsrOPZ
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace KslOPZ
		{
			inline constexpr int min = 0;
			inline constexpr int max = 99;
			inline constexpr int initial = 0;
		}

		namespace KsBp
		{
			inline constexpr int min = 0;
			inline constexpr int max = 127;
			inline constexpr int initial = 60;
		}

		namespace KsLc
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace KsRc
		{
			inline constexpr int min = 0;
			inline constexpr int max = 3;
			inline constexpr int initial = 0;
		}

		namespace KsLd
		{
			inline constexpr float min = 0.0;
			inline constexpr float max = 1.0;
			inline constexpr float initial = 0.0;
		}

		namespace KsRd
		{
			inline constexpr float min = 0.0;
			inline constexpr float max = 1.0;
			inline constexpr float initial = 0.0;
		}

		namespace KsRs
		{
			inline constexpr int min = 0;
			inline constexpr int max = 7;
			inline constexpr int initial = 0;
		}
	}

	namespace OplDetune {
		namespace Mul
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 1;
		}
	}

	namespace OpmDetune {
		namespace Mul
		{
			inline constexpr int min = 0;
			inline constexpr int max = 16;
			inline constexpr int initial = 1;
		}

		namespace MulRatio
		{
			inline constexpr float min = 0.01f;
			inline constexpr float max = 100.0f;
			inline constexpr float initial = 1.0f;
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
	}

	namespace OpnDetune {
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
	}

	namespace Opzx7Detune {
		namespace Mul
		{
			inline constexpr int min = 0;
			inline constexpr int max = 21;
			inline constexpr int initial = 2;
		}

		namespace MulRatio
		{
			inline constexpr float min = 0.01f;
			inline constexpr float max = 100.0f;
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

	namespace OplLfo
	{
		namespace Am
		{
			inline constexpr bool initial = false;
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
			inline constexpr bool initial = false;
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

	namespace OpmLfo
	{
		namespace Freq
		{
			inline constexpr int min = 0;
			inline constexpr int max = 255;
			inline constexpr int initial = 0;
		}

		namespace AmSmRt
		{
			inline constexpr float min = 0.005f;
			inline constexpr float max = 0.5f;
			inline constexpr float initial = 0.005f;
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

		namespace SyncDelay
		{
			inline constexpr int min = 0;
			inline constexpr int max = 127;
			inline constexpr int initial = 0;
		}

		namespace AmsEn
		{
			inline constexpr bool initial = true;
		}
	}

	namespace OpnaLfo
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

		namespace AmSmRt
		{
			inline constexpr float min = 0.005f;
			inline constexpr float max = 0.5f;
			inline constexpr float initial = 0.005f;
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
			inline constexpr int max = 12;
			inline constexpr int initial = 0;
		}

		namespace AmShape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 12;
			inline constexpr int initial = 0;
		}
	}

	namespace N88Lfo
	{
		namespace Freq
		{
			inline constexpr int min = 0;
			inline constexpr int max = 16383;
			inline constexpr int initial = 0;
		}

		namespace Shape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 5;
			inline constexpr int initial = 0;
		}

		namespace SyncDelay
		{
			inline constexpr int min = 0;
			inline constexpr int max = 255;
			inline constexpr int initial = 0;
		}

		namespace AmSmRt
		{
			inline constexpr float min = 0.005f;
			inline constexpr float max = 0.5f;
			inline constexpr float initial = 0.005f;
		}

		namespace Pm
		{
			inline constexpr bool initial = false;
		}

		namespace Pmd
		{
			inline constexpr int min = -127;
			inline constexpr int max = 127;
			inline constexpr int initial = 0;
		}

		namespace Pms
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
		}

		namespace Am
		{
			inline constexpr bool initial = false;
		}

		namespace Amd
		{
			inline constexpr int min = -127;
			inline constexpr int max = 127;
			inline constexpr int initial = 0;
		}

		namespace Ams
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
		}
	}

	namespace WtMod
	{
		namespace Enable
		{
			inline constexpr float initial = false;
		}

		namespace Depth
		{
			inline constexpr float min = 0.1f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.2f;
		}

		namespace Speed
		{
			inline constexpr float min = 0.1f;
			inline constexpr float max = 10.0f;
			inline constexpr float initial = 1.0f;
		}

		// 変調方式
		//   0    : 正弦波 (FDS の簡易版)
		//   1〜4 : FDS(2C33) の 32ステップ階段テーブル
		//          (1:Triangle / 2:Saw / 3:Reset / 4:Pulse)
		//   5, 6 : WonderSwan ch3 のハードウェアスイープ (上昇 / 下降)
		//   7    : PC Engine HuC6280 の LFO (32サンプルの波形メモリで変調)
		//   8    : FDS(2C33) の 32ステップ階段テーブル (ユーザー編集)
		namespace Shape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 8;
			inline constexpr int initial = 0;
		}

		// FDS(2C33) の変調テーブル。32 エントリ・各 3bit の増減値で、
		// 0 = +0 / 1 = +1 / 2 = +2 / 3 = +4 / 4 = カウンタをリセット /
		// 5 = -4 / 6 = -2 / 7 = -1 を表す。初期値は対称三角のテーブル。
		// 使う変調波形スロットの番号
		namespace WaveSlot
		{
			inline constexpr int min = 0;
			inline constexpr int max = Global::WtMod::slots - 1;
			inline constexpr int initial = 0;
		}

		namespace FdsTable
		{
			inline constexpr int size = 32;
			inline constexpr int min = 0;
			inline constexpr int max = 7;
		}

		// HuC6280 の波形メモリは 32 サンプル。変調用の波形もそれに合わせる。
		namespace WaveSize
		{
			inline constexpr int size = 32;
		}

		// 64/128/256 点の波形を 32 点へ落とすときの方法。
		// ON  : 区間平均してから元のピークへ正規化する (折り返しが減る)
		// OFF : 単純間引き (元波形の値をそのまま拾う)
		namespace WaveSmooth
		{
			inline constexpr bool initial = true;
		}
	}

	namespace SsgDuty
	{
		namespace Mode
		{
			inline constexpr int min = 0;
			inline constexpr int max = 1;
			inline constexpr int initial = 0;
		}

		namespace Preset
		{
			inline constexpr int min = 0;
			inline constexpr int max = 8;
			inline constexpr int initial = 0;
		}

		namespace Var
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 0.5f;
			inline constexpr float initial = 0.5f;
		}

		namespace Inv
		{
			inline constexpr float initial = false;
		}

		namespace Fc
		{
			inline constexpr float initial = false;
		}

		namespace FcFluc
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.2f;
		}
	}

	namespace SsgTri
	{
		namespace KeyTrack
		{
			inline constexpr float initial = true;
		}

		namespace Peak
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.5f;
		}

		namespace ManualFreq
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 8000.0f;
			inline constexpr float initial = 440.0f;
		}
	}

	namespace SsgHwEnv
	{
		namespace Enable
		{
			inline constexpr bool initial = false;
		}

		// 0〜7 は実機 AY-3-8910 の shape 8〜15、8 以降はオリジナル波形。
		// 実体は SsgHwShape (EnvSsgHwParams.h) を参照。
		namespace Shape
		{
			inline constexpr int min = 0;
			inline constexpr int max = 43;
			inline constexpr int initial = 0;
		}

		namespace Period
		{
			inline constexpr float min = 0.1f;
			inline constexpr float max = 200.0f;
			inline constexpr float initial = 1.0f;
		}

		namespace Min
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 0.0f;
		}

		namespace Max
		{
			inline constexpr float min = 0.0f;
			inline constexpr float max = 1.0f;
			inline constexpr float initial = 1.0f;
		}

		namespace Smooth
		{
			inline constexpr bool initial = false;
		}
	}

	namespace OplEg {
		inline constexpr int min = 0;
		inline constexpr int max = 3;
		inline constexpr int initial = 0;
	}

	namespace Opl3Eg {
		inline constexpr int min = 0;
		inline constexpr int max = 9;
		inline constexpr int initial = 0;
	}

	namespace OpmSeFreq
	{
		inline constexpr float min = 0.1f;
		inline constexpr float max = 30.0f;
		inline constexpr float initial = 1.0f;
	}

	namespace OpnaSe
	{
		namespace Se
		{
			inline constexpr int min = 0;
			inline constexpr int max = 15;
			inline constexpr int initial = 0;
		}

		namespace Freq
		{
			inline constexpr float min = 0.1f;
			inline constexpr float max = 30.0f;
			inline constexpr float initial = 1.0f;
		}
	}

	namespace Opzx7Ws
	{
		inline constexpr int min = 0;
		inline constexpr int max = 72;
		inline constexpr int initial = 0;
	}
}
