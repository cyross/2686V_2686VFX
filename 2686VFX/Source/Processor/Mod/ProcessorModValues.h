// パラメータの初期値と範囲を管理

#pragma once

#include <JuceHeader.h>

namespace ModPrValue
{
	namespace Env
	{
		// 初期はバイパス。鍵盤を触らない使い方でも素通しになるようにしておく。
		inline constexpr bool bypassInitial = true;
	}

	namespace Lfo
	{
		inline constexpr bool bypassInitial = true;
	}

	namespace Pitch
	{
		inline constexpr bool bypassInitial = true;
	}

	namespace WtMod
	{
		// 実機の搬送波にあたる周波数。低いほどゆっくり揺れる。
		// 初期は基準音のラにしてある。
		inline constexpr float baseFreqMin = 1.0f;
		inline constexpr float baseFreqMax = 2000.0f;
		inline constexpr float baseFreqInitial = 440.0f;
	}

	namespace Shift
	{
		inline constexpr bool bypassInitial = true;
	}
}
