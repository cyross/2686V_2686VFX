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
// FORM の TONE の VALUE は、鍵盤の周波数へ To から決まる倍率を掛ける (toneRatio)。
namespace SsgRegister
{
	inline constexpr double clock = 667.0 * 1000.0;

	inline float envHz(int te) { return (float)(clock / (512.0 * std::max(1, te))); }
	inline float noiseHz(int tn) { return (float)(clock / (32.0 * std::max(1, tn))); }
	inline float toneHz(int to) { return (float)(clock / (32.0 * std::max(1, to))); }

	// FORM の TONE の VALUE。鍵盤の周波数へ Fnote(To) / Fnote(toneBase) を掛ける。
	// 基準は T.VALUE の初期値なので、初期値のままなら 1 倍。To を大きく
	// するほど低くなる
	inline constexpr int toneBase = 47;

	inline float toneRatio(int to) { return toneHz(to) / toneHz(toneBase); }
}
