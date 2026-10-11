#pragma once

#include <algorithm>
#include <cmath>

// ============================================================================
// SSG (YM2203) の設定値と周波数の換算
// ============================================================================
// 実機は周波数を直に持たず、分周の設定値 (整数) を持つ。設定値から周波数は
// 次の式で決まる (クロックは 667 kHz とする)。
//
//   トーン       Fnote = 667 * 1000 / (32  * To)   To: 0〜4095
//   ノイズ       Fnois = 667 * 1000 / (32  * Tn)   Tn: 0〜31
//   エンベロープ Fenv  = 667 * 1000 / (512 * Te)   Te: 0〜65535
//
// 設定値 0 は、実機と同じく 1 として扱う (式のままでは 0 で割る)。
//
// SSG HW AMP ENV / SSG HW PITCH ENV の VALUE と、FORM の NOISE の VALUE は、
// ここで Hz へ直してから、これまでの PERIOD / N.FREQ と同じ道を通す。
// FORM の TONE の VALUE は、鳴らす周波数を To の整数の格子へ寄せる。
namespace SsgRegister
{
	inline constexpr double clock = 667.0 * 1000.0;

	inline constexpr int toneMax = 4095;
	inline constexpr int noiseMax = 31;
	inline constexpr int envMax = 65535;

	inline float envHz(int te) { return (float)(clock / (512.0 * std::max(1, te))); }
	inline float noiseHz(int tn) { return (float)(clock / (32.0 * std::max(1, tn))); }
	inline float toneHz(int to) { return (float)(clock / (32.0 * std::max(1, to))); }

	// その周波数にいちばん近い To (1〜4095)
	inline int toneValue(float hz)
	{
		if (!(hz > 0.0f)) return toneMax;

		const double to = std::round(clock / (32.0 * (double)hz));

		return (int)std::clamp(to, 1.0, (double)toneMax);
	}

	// To の格子へ寄せた周波数
	inline float quantizeTone(float hz)
	{
		if (!(hz > 0.0f)) return hz;

		return toneHz(toneValue(hz));
	}

	// 寄せた周波数 ÷ 元の周波数。再生の進み幅へ掛けて使う (PCM 系)
	inline float quantizeToneRatio(float hz)
	{
		if (!(hz > 0.0f)) return 1.0f;

		return quantizeTone(hz) / hz;
	}
}
