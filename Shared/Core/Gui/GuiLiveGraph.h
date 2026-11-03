#pragma once

#include <JuceHeader.h>

// ============================================================================
// グラフの、鳴っている位置を動かして見せる (4.3.0)
// ============================================================================
// TAP SEQUENCER (いまの段) と PARAM ENV[256] (エンベロープの位置) を、鍵を
// 弾くのに合わせて動かす。どちらも SETTINGS で入り切りと速さ (FPS) を選ぶ。
// 見た目だけの話で、音には影響しない。
//
// 動かさないときも、値の書き換え (表の版) だけは idleHz で見に行く。
namespace LiveGraph
{
	// 選べる速さ (FPS)
	inline constexpr int fpsChoices[] = { 15, 30, 60, 120 };
	inline constexpr int fpsInitial = 30;

	// 動かさないときに、値の書き換えを見に行く速さ
	inline constexpr int idleHz = 10;

	// 選べる速さのどれかへ寄せる (設定ファイルに書かれた値は選択肢と限らない)
	inline int clampFps(int fps)
	{
		for (int f : fpsChoices) {
			if (fps <= f) return f;
		}

		return fpsChoices[std::size(fpsChoices) - 1];
	}

	inline int hzOf(bool live, int fps) { return live ? clampFps(fps) : idleHz; }

	// 速さが変わっていれば刻み直す。毎回の timerCallback から呼んでよい
	inline void pace(juce::Timer& timer, int hz)
	{
		const int ms = juce::jmax(1, 1000 / juce::jmax(1, hz));

		if (timer.getTimerInterval() != ms) timer.startTimer(ms);
	}
}
