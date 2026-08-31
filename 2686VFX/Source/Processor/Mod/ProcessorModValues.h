// パラメータの初期値と範囲を管理

#pragma once

#include <JuceHeader.h>

namespace ModPrValue
{
	namespace AmpEnv
	{
		// 初期は切。鍵盤を触らない使い方でも素通しになるようにしておく。
		inline constexpr bool enableInitial = false;
	}
}
