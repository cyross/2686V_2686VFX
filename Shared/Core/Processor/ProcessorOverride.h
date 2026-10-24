#pragma once

#include <atomic>

// ============================================================================
// パラメータの値を、読むあいだだけ別の値に見せる (PARAM ENV / TAP PARAM SEQUENCER)
// ============================================================================
// PrHelper::getBool / getInt / getFloat はパラメータの値をここ経由で読む。
// Scope を作っているあいだは、並べたパラメータだけ置き換えた値が返る。
//
// ボイスが音源の値を読み直すとき (ParamMod::Applier) に使う。プロセッサの
// 「パラメータから音源の値を作る」道具をそのまま使えるので、指し先ごとに
// 直し方を書かずに済む。置き換えはスレッドごと (音のスレッドと、画面のスレッドで
// 作る波形プレビューが混ざらない)。並べていないときは素通しする。
namespace PrOverride
{
	struct Entry
	{
		std::atomic<float>* ptr = nullptr;
		float value = 0.0f;
	};

	inline thread_local const Entry* list = nullptr;
	inline thread_local int count = 0;

	// 同じパラメータが 2 つあれば後ろが勝つ
	inline float load(std::atomic<float>* ptr)
	{
		for (int i = count - 1; i >= 0; --i) {
			if (list[i].ptr == ptr) return list[i].value;
		}

		return ptr->load(std::memory_order_relaxed);
	}

	class Scope
	{
	public:
		Scope(const Entry* entries, int n) : prevList(list), prevCount(count)
		{
			list = entries;
			count = n;
		}

		~Scope()
		{
			list = prevList;
			count = prevCount;
		}

		Scope(const Scope&) = delete;
		Scope& operator=(const Scope&) = delete;

	private:
		const Entry* prevList;
		int prevCount;
	};
}
