#pragma once

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>

// ============================================================================
// 鳴っている位置 (グラフの縦線。4.3.0)
// ============================================================================
// エンベロープがいまどの区間のどのあたりにいるかを、音の側が書き、画面が読んで
// グラフの上へ縦線を引く。いちばん新しく押した音だけが書く (owner)。
//
// 区間 (phase) は、段のエンベロープなら段の番号 (1〜)、ADSR ならアタック・
// ディケイ。サスティンとリリースはどちらも決まった値で表す。進み (frac) は
// 区間の中の 0〜1。
struct LivePosition
{
	static constexpr int idle = 0;       // 鳴っていない
	static constexpr int release = -1;   // 離したあと (リリース)
	static constexpr int sustain = -2;   // 鍵を押しているあいだ保つ (サスティン)

	// ADSR の区間
	static constexpr int attack = 1;
	static constexpr int decay = 2;

	std::atomic<std::uint32_t> owner{ 0 };
	std::atomic<int> phase{ idle };
	std::atomic<float> frac{ 0.0f };

	// 押した音ごとに振る番号。0 は使わない
	static std::uint32_t nextId()
	{
		static std::atomic<std::uint32_t> counter{ 0 };

		std::uint32_t id = counter.fetch_add(1, std::memory_order_relaxed) + 1;

		if (id == 0) id = counter.fetch_add(1, std::memory_order_relaxed) + 1;

		return id;
	}

	// 持ち主になる。返した番号で書く
	std::uint32_t claim()
	{
		const std::uint32_t id = nextId();

		owner.store(id, std::memory_order_relaxed);

		return id;
	}

	// このスレッドでは書かない。波形プレビューの計算は本物の音源を回すので、
	// そのままだと計算のたびにグラフの縦線が動いてしまう
	static bool& mutedOnThisThread()
	{
		thread_local bool muted = false;

		return muted;
	}

	struct Mute
	{
		const bool before = mutedOnThisThread();

		Mute() { mutedOnThisThread() = true; }
		~Mute() { mutedOnThisThread() = before; }
	};

	// 持ち主のときだけ書く。ほかの音に代わっていたら false
	bool write(std::uint32_t id, int p, float f)
	{
		if (owner.load(std::memory_order_relaxed) != id) return false;

		frac.store(f, std::memory_order_relaxed);
		phase.store(p, std::memory_order_relaxed);

		return true;
	}
};

// ----------------------------------------------------------------------------
// 書く側の手助け。押したときに start、進めるたびに write
// ----------------------------------------------------------------------------
class LiveWriter
{
public:
	void start(LivePosition* target)
	{
		pos = target;
		id = target != nullptr && !LivePosition::mutedOnThisThread() ? target->claim() : 0;
		current = LivePosition::idle;
	}

	bool active() const { return id != 0; }

	// 区間と進みを書く。鳴り終えたら (idle) 書いて手を離す。ほかの音が持ち主になっても離す
	void write(int phase, float frac)
	{
		if (id == 0) return;

		current = phase;

		if (!pos->write(id, phase, std::clamp(frac, 0.0f, 1.0f)) || phase == LivePosition::idle) id = 0;
	}

protected:
	LivePosition* pos = nullptr;
	std::uint32_t id = 0;
	int current = LivePosition::idle;
};

// 音量のエンベロープ (AMP ENV・FM 音源の EG) の位置を書く。どれも 0〜1 の音量で、
// アタックは 1 へ、ディケイは SL へ、サスティンは (SR で下がるなら) 0 へ、
// リリースは 0 へ向かう。区間の中の進みは、区間に入ったときの音量から行き先まで
// のどこにいるかで決める (グラフの線のうえで、いまの音量のところ)。
// 行き先が負なら動かない区間で、真ん中に置く。サスティンは動いていなければ真ん中
class LiveLevelWriter : public LiveWriter
{
public:
	void write(int phase, float level, float to)
	{
		if (id == 0) return;

		if (phase != current) from = level;

		float f = 0.5f;

		const bool moving = std::abs(level - from) > 1.0e-4f;
		const bool still = phase == LivePosition::sustain && !moving;

		if (to >= 0.0f && !still && std::abs(to - from) > 1.0e-5f) f = (level - from) / (to - from);

		LiveWriter::write(phase, f);
	}

private:
	float from = 0.0f;
};
