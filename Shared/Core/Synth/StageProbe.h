#pragma once

#include <algorithm>
#include <array>
#include <vector>


// ============================================================================
// 生成中の波形の窓 (4.4.0) のための、音源の途中の音を拾う口
// ============================================================================
// 窓は「音源の中で音がどう作られていくか」を段ごとに見せる。出来上がった音
// だけでは途中が見えないので、コアの中の決まった位置で値を写し取る。
//
// 拾うのは計算用の音源 (GuiStageRig) だけ。鳴らしている音源のコアは
// m_probe が nullptr のままなので、写す手間は掛からない。
//
//   raw  : 音源の中の加工を通す前の音 (FM はオペレーターを足しただけの音)
//   core : 音源の中の加工を通したあとの音 (コアが返す 1 ボイスぶん)
//   ops  : FM はオペレーターごとの音。RHYTHM はパッドごとの音
//
// ユニゾンで重なったボイスは 1 本目だけを拾う (ユニゾンの前の音を見せるため)。
// 重なって鳴った別の音 (打鍵が重なったとき) は足し合わせる。
struct StageProbe
{
	static constexpr int maxOps = 8;

	enum class Kind { other = 0, fm = 1, rhythm = 2 };

	// ボイスが「このブロックはここから書く」と入れる。コアは 1 サンプルごとに 1 つ進める
	int cursor = 0;

	std::vector<float> raw;
	std::vector<float> core;
	std::array<std::vector<float>, maxOps> ops;

	// ユニゾンが何本で鳴ったか (1 なら使っていない)
	int unisonSeen = 1;

	void resize(int samples)
	{
		raw.assign((size_t)samples, 0.0f);
		core.assign((size_t)samples, 0.0f);

		for (auto& op : ops) op.assign((size_t)samples, 0.0f);

		cursor = 0;
	}

	void clear()
	{
		std::fill(raw.begin(), raw.end(), 0.0f);
		std::fill(core.begin(), core.end(), 0.0f);

		for (auto& op : ops) std::fill(op.begin(), op.end(), 0.0f);

		cursor = 0;
	}

	// コアから 1 サンプルぶん書く。opValues は opCount 個
	void write(float rawValue, float coreValue, const float* opValues, int opCount)
	{
		const int at = cursor++;

		if (at < 0 || at >= (int)raw.size()) return;

		raw[(size_t)at] += rawValue;
		core[(size_t)at] += coreValue;

		for (int i = 0; i < std::min(opCount, maxOps); ++i) ops[(size_t)i][(size_t)at] += opValues[i];
	}
};

// ============================================================================
// 作るときだけ切る、エンベロープ・LFO などのまとまり
// ============================================================================
// パッチそのものは変えない。計算用の音源へ渡すパラメータの写しを書き換える。
// まとまりは、音源の値の名前で見分ける (どの音源も同じ名前で持っている)。
namespace StageSwitch
{
	enum Module
	{
		ampEnv = 0,  // AMP ENV (チャンネル・パッドの ADSR)
		opEnv,       // FM のオペレーターの EG
		pitchEnv,    // PITCH ENV
		lfo,         // LFO (PM / AM)
		ssgHwEnv,    // SSG HW ENV (音量・音程)
		ssgSwEnv,    // SSG SW ENV[256] (音量・音程)
		wtMod,       // WT MOD / WT AMP MOD
		stepEnv,     // ALG ENV・TL ENV・DUTY ENV・PEAK ENV
		paramEnv,    // PARAM ENV[256]
		count
	};

	using Mask = unsigned;

	inline constexpr Mask bit(int m) { return 1u << (unsigned)m; }

	inline constexpr Mask all = (1u << (unsigned)count) - 1u;
}
