#pragma once

#include <JuceHeader.h>

#include "./ProcessorHelper.h"
#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Core/Synth/TapSequencer.h"

// ============================================================================
// TAP AMP / PITCH SEQUENCER (パラメータの側)
// ============================================================================
// チャンネルごとに 2 つ持つ。パラメータは頭 (Enable・STEPS・LOOP) と、段を動かす
// 3 つの札 (INCREMENT / DECREMENT / RESET) だけ。段の値とループの組は表に持つ。
//
//   <base>_ENABLE / _STEPS / _LOOP / _INC / _DEC / _RESET
//
// 札はオートメーションで押せるよう、0.5 を下から上へ越えたときに 1 回動く。
// 画面のボタンは表の cursor を直に動かす (札は経由しない)。
namespace TapSeq
{
	struct Keys
	{
		juce::String base, enable, steps, loop, inc, dec, reset;

		explicit Keys(const juce::String& b) :
			base(b),
			enable(b + "_ENABLE"),
			steps(b + "_STEPS"),
			loop(b + "_LOOP"),
			inc(b + "_INC"),
			dec(b + "_DEC"),
			reset(b + "_RESET")
		{
		}
	};

	struct Spec
	{
		Keys keys;

		// パラメータの表示名の頭
		juce::String name;

		// 段の値の範囲と初期値。初期値のときは鳴りが変わらない
		float valueMin = 0.0f;
		float valueMax = 1.0f;
		float valueInitial = 0.0f;

		// 値が整数 (セント)
		bool integer = false;
	};

	inline constexpr int stepsMin = 1;
	inline constexpr int stepsMax = StepTable::size;
	inline constexpr int stepsInitial = 1;

	// 起動時の初期化の順に頼らないよう、関数の中で作る
	inline const Spec& amp()
	{
		static const Spec s{ Keys("_TAPAMP"), " TapAmp", 0.0f, 4.0f, 1.0f, false };
		return s;
	}

	inline const Spec& pitch()
	{
		static const Spec s{ Keys("_TAPPITCH"), " TapPitch", -2400.0f, 2400.0f, 0.0f, true };
		return s;
	}

	struct Ptrs
	{
		std::atomic<float>* enable = nullptr;
		std::atomic<float>* steps = nullptr;
		std::atomic<float>* loop = nullptr;
		std::atomic<float>* inc = nullptr;
		std::atomic<float>* dec = nullptr;
		std::atomic<float>* reset = nullptr;
		StepTable* table = nullptr;

		// 札の前の値 (0.5 を越えたかを見る)
		bool lastInc = false;
		bool lastDec = false;
		bool lastReset = false;
	};

	inline void addParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName, const Spec& s)
	{
		const auto& k = s.keys;
		const auto n = prefixName + s.name;

		PrHelper::addBool(layout, prefix + k.enable, n + " Enable", false);
		PrHelper::addInt(layout, prefix + k.steps, n + " Steps", stepsMin, stepsMax, stepsInitial);
		PrHelper::addBool(layout, prefix + k.loop, n + " Loop", false);
		PrHelper::addBool(layout, prefix + k.inc, n + " Increment", false);
		PrHelper::addBool(layout, prefix + k.dec, n + " Decrement", false);
		PrHelper::addBool(layout, prefix + k.reset, n + " Reset", false);
	}

	inline void setupPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, const Spec& s, Ptrs& p, StepTableStore& tables)
	{
		const auto& k = s.keys;

		p.enable = apvts.getRawParameterValue(prefix + k.enable);
		p.steps = apvts.getRawParameterValue(prefix + k.steps);
		p.loop = apvts.getRawParameterValue(prefix + k.loop);
		p.inc = apvts.getRawParameterValue(prefix + k.inc);
		p.dec = apvts.getRawParameterValue(prefix + k.dec);
		p.reset = apvts.getRawParameterValue(prefix + k.reset);

		// 段の値は表の L に持つ (R は使わない)
		p.table = &tables.get(prefix + k.base, 0.0f, s.valueInitial);
	}

	// 札を見て段を動かし、音の側へ渡す値を作る
	inline void apply(Ptrs& p, TapSeqParams& params)
	{
		params.enable = PrHelper::getBool(p.enable);
		params.steps = std::clamp(PrHelper::getInt(p.steps), stepsMin, stepsMax);
		params.loop = PrHelper::getBool(p.loop);
		params.table = p.table;

		if (p.table == nullptr) return;

		auto rose = [](std::atomic<float>* ptr, bool& last) {
			const bool now = ptr != nullptr && ptr->load(std::memory_order_relaxed) >= 0.5f;
			const bool r = now && !last;

			last = now;

			return r;
		};

		const int steps = params.steps;
		int stored = p.table->cursor.load(std::memory_order_relaxed);
		int cursor = std::clamp(stored, 0, steps - 1);

		// INCREMENT は押したときと同じくループに沿って進む。DECREMENT は 1 段ずつ戻る
		if (rose(p.inc, p.lastInc)) cursor = tapSeqNext(params, cursor);
		if (rose(p.dec, p.lastDec)) cursor = (cursor + steps - 1) % steps;

		if (rose(p.reset, p.lastReset)) {
			tapSeqRewind(*p.table);
			return;
		}

		// 波形プレビューの組み立てで、画面のスレッドからも呼ばれる。変わったときだけ、
		// それも読んだあとに鍵で進んでいなければ書く (進めた段を古い値で潰さない)
		if (cursor != stored) p.table->cursor.compare_exchange_strong(stored, cursor, std::memory_order_relaxed);
	}
}
