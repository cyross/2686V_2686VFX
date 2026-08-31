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
		static inline const juce::String enable = "_ENV_ENABLE";
	}

	// LFO は鍵盤を押さなくても回り続ける。入り切りだけで使える。
	namespace Lfo
	{
		static inline const juce::String enable = "_LFO_ENABLE";
	}
}
