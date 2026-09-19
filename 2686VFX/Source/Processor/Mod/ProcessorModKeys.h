// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include <array>

#include "Shared/Core/Processor/ProcessorKeys.h"

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
	// ただしキーアサインをカスタマイズしたときは、割り当てた鍵盤を
	// 押している間だけ掛かる。
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
	// キーアサインをカスタマイズしたときは、LFO と同じく割り当てた
	// 鍵盤を押している間だけ掛かる。
	namespace Shift
	{
		static inline const juce::String bypass = "_SHIFT_BYPASS";
	}

	// どの鍵盤で動かすか。
	//
	// 「シングルキーアサイン」はこれまでどおり、どの鍵盤でも全部が動く。
	// 「キーアサインのカスタマイズ」では、対象ごとに決めた鍵盤でだけ動く。
	// 同じ鍵盤を複数の対象へ割り当ててよい。
	namespace KeyAssign
	{
		static inline const juce::String mode = "_KEYASSIGN_MODE";

		// 割り当てる対象。並びは画面の一覧の並び (変調の列と同じ)。
		// 鍵盤を押したときに処理する順番は、これとは別に ModProcessor が
		// これまでどおりの順で持つ。
		enum Target
		{
			AmpEnv,
			SsgHwEnv,
			WtAmpMod,
			SsgSwEnv11,
			PitchEnv,
			SsgHwPEnv,
			SsgSwPEnv11,
			WtMod,
			LfoAm,
			LfoPm,
			MulDet,
			Unison,
			Arpeggio,
			NumTargets
		};

		struct TargetInfo
		{
			// パラメータ ID の尻尾
			juce::String key;

			// パラメータファイルでの名前
			juce::String fileKey;

			// 画面と DAW に出す名前。実機の用語なので訳さない。
			juce::String name;
		};

		static inline const std::array<TargetInfo, NumTargets> targets = { {
			{ "_KEY_AMPENV",      "ampEnv",      "AMP ENV" },
			{ "_KEY_SSGHWENV",    "ssgHwEnv",    "SSG HW AMP ENV" },
			{ "_KEY_WTAMPMOD",    "wtAmpMod",    "WT AMP MOD" },
			{ "_KEY_SSGSWENV11",  "ssgSwEnv11",  "SSG SW AMP ENV[11]" },
			{ "_KEY_PITCHENV",    "pitchEnv",    "PITCH ENV" },
			{ "_KEY_SSGHWPENV",   "ssgHwPEnv",   "SSG HW PITCH ENV" },
			{ "_KEY_SSGSWPENV11", "ssgSwPEnv11", "SSG SW PITCH ENV[11]" },
			{ "_KEY_WTMOD",       "wtMod",       "WT PITCH MOD" },
			{ "_KEY_LFOAM",       "lfoAm",       "LFO AM" },
			{ "_KEY_LFOPM",       "lfoPm",       "LFO PM" },
			{ "_KEY_MULDET",      "mulDet",      "MUL/DET" },
			{ "_KEY_UNISON",      "unison",      "UNISON/HARMONY" },
			{ "_KEY_ARP",         "arpeggio",    "ARPEGGIO" },
		} };
	}
}
