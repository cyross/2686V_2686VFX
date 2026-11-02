#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

// ============================================================================
// 変化量で進む段 (RATE の段, 4.3.0)
// ============================================================================
// 段のエンベロープ (StepEnv) の段は、ふつう (TIME) は「段の頭の値から L へ R 秒で」
// 進む。始まる値が違うと、同じ R でも動く速さが変わる。
//
// RATE の段は、決まった変化量 (D) で L へ向かい、L に届くか R 秒たつかの早いほうで
// 終える。どこから始まっても同じ速さで動く。
//
//   T が 0       : 線形。1 秒あたり D ずつ動く
//   T が 0 より大 : 刻み。段に入ってから T 秒待つたびに D ずつ動く
//                   (入った瞬間は動かない。MML のソフトウェアエンベロープと同じ)
//   R が最小     : 時間では打ち切らない (L に届くまで進む)
//   D が 0       : 動かない。R 秒たったら終える (打ち切らないなら、離すまでその段にいる)
//
// 向きは L で決まる (D は大きさだけ)。L に届いたら止め、行き過ぎない。
//
// T は秒で持つほか、T SYNC を入れたときはホストのテンポに合わせた音符の長さで持つ
// (tickNote)。音の側 (StepEnv) と絵 (GuiEnvelopeGraph) が同じ式で長さを出すよう、
// 計算はここにまとめる。
namespace StepRate
{
	// R がこれ以下なら時間で打ち切らない (R の下限 CPV::rateMin と同じ)
	inline constexpr float noLimitRate = 0.001f;

	// 段 1 つぶんの設定
	struct Step
	{
		bool rate = false;     // RATE の段か (切っていれば TIME)
		float delta = 0.0f;    // 変化量 D (大きさ)
		float tickSec = 0.0f;  // 1 刻みの秒。0 は線形
		int tickNote = 0;      // T SYNC のときの 1 刻み (notes の番号)。0 は線形

		bool operator==(const Step& o) const
		{
			return rate == o.rate && delta == o.delta && tickSec == o.tickSec && tickNote == o.tickNote;
		}

		bool operator!=(const Step& o) const { return !(*this == o); }
	};

	// T SYNC で選べる 1 刻みの長さ。beats は 4 分音符を 1 としたもの。[0] は線形
	struct Note
	{
		const char* label;
		double beats;
	};

	inline constexpr std::array<Note, 20> notes = { {
		{ "LINEAR", 0.0 },
		{ "1/1", 4.0 }, { "1/2", 2.0 }, { "1/2.", 3.0 }, { "1/2T", 4.0 / 3.0 },
		{ "1/4", 1.0 }, { "1/4.", 1.5 }, { "1/4T", 2.0 / 3.0 },
		{ "1/8", 0.5 }, { "1/8.", 0.75 }, { "1/8T", 1.0 / 3.0 },
		{ "1/16", 0.25 }, { "1/16.", 0.375 }, { "1/16T", 1.0 / 6.0 },
		{ "1/32", 0.125 }, { "1/32.", 0.1875 }, { "1/32T", 1.0 / 12.0 },
		{ "1/64", 0.0625 }, { "1/64.", 0.09375 }, { "1/64T", 1.0 / 24.0 },
	} };

	inline constexpr int noteCount = (int)notes.size();

	// ホストがテンポをくれないとき (Standalone など)
	inline constexpr double defaultBpm = 120.0;

	inline bool limited(float r) { return r > noLimitRate + 1.0e-6f; }

	// 1 刻みの秒。0 は線形
	inline double tickSeconds(const Step& s, bool sync, double bpm)
	{
		if (!sync) return std::max(0.0, (double)s.tickSec);

		const int n = std::clamp(s.tickNote, 0, noteCount - 1);

		if (n == 0) return 0.0;

		return notes[(size_t)n].beats * 60.0 / (bpm > 0.0 ? bpm : defaultBpm);
	}

	// 段に入ってから t 秒の値。start から target へ、delta ずつ (tick 秒ごと。0 なら線形) 進む
	inline float valueAt(float start, float target, float delta, double tick, double t)
	{
		const float dist = std::abs(target - start);
		const float dir = target >= start ? 1.0f : -1.0f;

		if (dist <= 0.0f || delta <= 0.0f || t <= 0.0) return start;

		double moved = 0.0;

		if (tick > 0.0) moved = std::floor(t / tick + 1.0e-9) * (double)delta;
		else moved = (double)delta * t;

		return start + dir * (float)std::min((double)dist, moved);
	}

	// 段の長さと、終わったときの値
	struct Segment
	{
		double seconds = 0.0;  // 長さ。endless なら意味を持たない
		float end = 0.0f;      // 終わったときの値
		bool endless = false;  // 終わらない (D が 0 で、時間でも打ち切らない)
	};

	// limit は R (秒)。noLimitRate 以下なら時間で打ち切らない
	inline Segment segment(float start, float target, float delta, double tick, float limit)
	{
		const float dist = std::abs(target - start);
		const bool hasLimit = limited(limit);

		// すでに L にいる。待たずに終える
		if (dist <= 0.0f) return { 0.0, target, false };

		double reach = std::numeric_limits<double>::infinity();

		if (delta > 0.0f) {
			if (tick > 0.0) reach = std::ceil((double)dist / (double)delta - 1.0e-9) * tick;
			else reach = (double)dist / (double)delta;
		}

		if (!hasLimit) {
			if (!std::isfinite(reach)) return { 0.0, start, true };

			return { reach, target, false };
		}

		if (reach <= (double)limit) return { reach, target, false };

		return { (double)limit, valueAt(start, target, delta, tick, (double)limit), false };
	}
}
