#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <vector>

// ============================================================================
// 段のループ (Rich Mode / TAP SEQUENCER の LOOP)
// ============================================================================
// 段の並び (1〜STEPS) に、START・END・COUNT の組を最大 256 個かける。
//
//   END の段を終えたら START の段へ戻る。戻るのは COUNT 回 (0 は無限)。
//   つまり START〜END の段は COUNT + 1 回通る (LOOP.CNT と同じ数え方)。
//
// 組は START・END の昇順に並べて持つ。並びの前にあるものから順に効く。
//
//   入れ子   : 内側 (START が後、または END が前) を先に回し、回し終えたら外側が
//              戻る。外側が戻るたびに、内側は数え直す。
//   跨ぎ     : 先に始まったループ (A) の中で始まり、A の END より後で終わる
//              ループ (B) があるとき。A を回し終えたら B の START へ進み、そこ
//              から B を回す。A はもう回さない。
//
// 入れ子の内側や、跨がれている側を無限 (COUNT 0) にすると、外側や跨いでいる
// 側へ進めなくなる。そう鳴るだけで止まりはしないが、画面では印を付けて知らせる
// (isTrapped)。
//
// 戻った回数などの進み具合は、使う側が State を持つ (エンベロープはボイスごと、
// TAP SEQUENCER はチャンネルで 1 つ)。
namespace StepLoop
{
	inline constexpr int maxEntries = 256;

	// LOOP MODE の値 (パラメータは 0 から)
	enum Mode { Original = 0, Rich = 1 };

	inline constexpr int modeMin = Original;
	inline constexpr int modeMax = Rich;
	inline constexpr int modeInitial = Original;

	// COUNT の範囲。LOOP.CNT と同じ (0 は無限)
	inline constexpr int countMin = 0;
	inline constexpr int countMax = 200;
	inline constexpr int countInitial = 1;

	struct Entry
	{
		int start = 1;
		int end = 1;
		int count = countInitial;

		bool operator==(const Entry& o) const { return start == o.start && end == o.end && count == o.count; }
	};

	// 並べる順。START が先、同じなら END が先
	inline bool before(const Entry& a, const Entry& b)
	{
		return a.start != b.start ? a.start < b.start : a.end < b.end;
	}

	inline void sort(std::vector<Entry>& entries)
	{
		std::stable_sort(entries.begin(), entries.end(), before);
	}

	// 範囲へ収める。START が END より後なら入れ替える
	inline Entry normalize(Entry e, int stepsMax)
	{
		e.start = std::clamp(e.start, 1, stepsMax);
		e.end = std::clamp(e.end, 1, stepsMax);
		e.count = std::clamp(e.count, countMin, countMax);

		if (e.start > e.end) std::swap(e.start, e.end);

		return e;
	}

	// 段の数が steps のときに効くか。STEPS より後ろの段にかかるものは効かない
	inline bool isValid(const Entry& e, int steps)
	{
		return e.start >= 1 && e.start <= e.end && e.end <= steps;
	}

	// 1 つの数へ詰める (表に原子で持つため)。START・END は 9 ビット、COUNT は 14 ビット
	inline std::uint32_t pack(const Entry& e)
	{
		return (std::uint32_t)(e.start & 0x1FF) | ((std::uint32_t)(e.end & 0x1FF) << 9) | ((std::uint32_t)(e.count & 0x3FFF) << 18);
	}

	inline Entry unpack(std::uint32_t v)
	{
		return { (int)(v & 0x1FF), (int)((v >> 9) & 0x1FF), (int)((v >> 18) & 0x3FFF) };
	}

	// j が i の内側か。i が戻るときに j を数え直す。
	// まったく同じ組どうしは、並びの後ろを内側とする (後ろを先に回すため)。
	template <typename Entries>
	bool isInner(const Entries& e, int j, int i)
	{
		if (j == i) return false;

		const Entry a = e[i];
		const Entry b = e[j];

		if (!(a.start <= b.start && b.end <= a.end)) return false;

		return b.start > a.start || b.end < a.end || j > i;
	}

	// k が i を跨ぐか (i の中で始まり、i の END より後で終わる)
	template <typename Entries>
	bool crosses(const Entries& e, int k, int i)
	{
		const Entry a = e[i];
		const Entry b = e[k];

		return a.start < b.start && b.start <= a.end && b.end > a.end;
	}

	// 無限のまま、外側や跨いでいる側へ進めなくしているか。画面の印に使う
	template <typename Entries>
	bool isTrapped(const Entries& e, int num, int steps, int i)
	{
		const Entry a = e[i];

		if (a.count != 0 || !isValid(a, steps)) return false;

		for (int j = 0; j < num; ++j) {
			if (j == i || !isValid(e[j], steps)) continue;
			if (isInner(e, i, j) || crosses(e, j, i)) return true;
		}

		return false;
	}

	// ループごとの進み具合。下の 15 ビットが戻った回数、上の 1 ビットが回し終えた印
	inline constexpr std::uint16_t finishedBit = 0x8000;
	inline constexpr std::uint16_t doneMask = 0x7FFF;

	struct State
	{
		std::array<std::uint16_t, maxEntries> v{};

		std::uint16_t get(int i) const { return v[(size_t)i]; }
		void set(int i, std::uint16_t x) { v[(size_t)i] = x; }
		void clear() { v.fill(0); }
	};

	// TAP SEQUENCER 用。音の側と画面のボタンの両方から進めるので原子で持つ
	struct AtomicState
	{
		std::array<std::atomic<std::uint16_t>, maxEntries> v{};

		std::uint16_t get(int i) const { return v[(size_t)i].load(std::memory_order_relaxed); }
		void set(int i, std::uint16_t x) { v[(size_t)i].store(x, std::memory_order_relaxed); }
		void clear() { for (auto& x : v) x.store(0, std::memory_order_relaxed); }
	};

	// 段 s を終えたあと、次に進む段を返す。STEPS を越えたら steps + 1。
	// e[0..num) は並べてあること。
	template <typename Entries, typename S>
	int next(const Entries& e, int num, int steps, S& st, int s)
	{
		num = std::clamp(num, 0, maxEntries);

		// END が s のものを、内側 (並びの後ろ) から見る
		for (int i = num - 1; i >= 0; --i) {
			const Entry a = e[i];

			if (a.end != s || !isValid(a, steps)) continue;

			const std::uint16_t x = st.get(i);

			if (x & finishedBit) continue;

			const int done = x & doneMask;

			if (a.count == 0 || done < a.count) {
				if (a.count > 0) st.set(i, (std::uint16_t)(done + 1));

				// 戻るたびに、内側を数え直す
				for (int j = 0; j < num; ++j) {
					if (isInner(e, j, i)) st.set(j, 0);
				}

				return a.start;
			}

			// 回し終えた。跨いでいるループがあれば、その START へ進む
			st.set(i, (std::uint16_t)(x | finishedBit));

			for (int k = 0; k < num; ++k) {
				if (!isValid(e[k], steps) || (st.get(k) & finishedBit)) continue;
				if (crosses(e, k, i)) return e[k].start;
			}
		}

		return s + 1;
	}
}
