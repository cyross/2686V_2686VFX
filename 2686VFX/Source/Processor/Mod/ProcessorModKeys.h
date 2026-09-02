// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorKeys.h"

// 出力へ掛ける変調のパラメータ。
//
// 音源のプラグインでは、これらはチャンネルごとに持っていた。エフェクトには
// チャンネルが無いので、出力に対して 1 組だけ持つ。
namespace ModPrKey
{
	static inline const juce::String prefix = "MOD";

	// エンベロープは MIDI の押し離しで動く。鍵盤を押している間だけ
	// 音量が変わるので、押さなければ素通しになる。
	namespace Env
	{
		static inline const juce::String bypass = "_ENV_BYPASS";
	}

	// LFO は鍵盤を押さなくても回り続ける。入り切りだけで使える。
	namespace Lfo
	{
		static inline const juce::String bypass = "_LFO_BYPASS";
	}

	// 音程側。入ってきた音を溜めてから読み出す速さを変えるので、
	// 音量側とは別に入り切りできるようにしてある。
	namespace Pitch
	{
		static inline const juce::String bypass = "_PITCH_BYPASS";
	}

	// WT PITCH MOD の速さは、実機では搬送波の周波数に対する比で決まる。
	// エフェクトには搬送波が無いので、その代わりの周波数を持つ。
	namespace WtMod
	{
		static inline const juce::String baseFreq = "_WTMOD_BASEFREQ";
	}

	// 音程を一定量ずらすもの。鍵盤を押さなくても掛かるので、
	// 押し離しで動くエンベロープとは別に入り切りできるようにしてある。
	namespace Shift
	{
		static inline const juce::String bypass = "_SHIFT_BYPASS";
	}
}
