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

	namespace KeyAssign
	{
		// 0: シングルキーアサイン (どの鍵盤でも全部が動く)
		// 1: キーアサインのカスタマイズ (対象ごとに決めた鍵盤で動く)
		//
		// 初期はシングル。これまでのデータを開いたときに動きが変わらないように。
		inline constexpr int single = 0;
		inline constexpr int custom = 1;
		inline constexpr int modeInitial = single;

		// 鍵盤の番号。初期は C3 (60)。
		inline constexpr int keyMin = 0;
		inline constexpr int keyMax = 127;
		inline constexpr int keyInitial = 60;
	}
}
