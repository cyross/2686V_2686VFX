#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "Shared/Effect/Envelope/Step/EnvStep.h"

// ============================================================================
// PARAM ENV[256] と TAP PARAM SEQUENCER (音の側, 4.3.0)
// ============================================================================
// どちらも「音源のパラメータを、ボイスごとに別の値へ置き換える」もの。
//
//   PARAM ENV[256]      : SSG SW AMP ENV[256] と同じ 256 段のエンベロープで、
//                         選んだパラメータ (TARGET) を動かす
//   TAP PARAM SEQUENCER : 鍵を押すたびに段を 1 つ進め、その段に入れた
//                         (パラメータ, 値) の組でパラメータを置き換える
//
// 置き換える先は APVTS のパラメータそのもの。値はそのパラメータの 0〜1
// (NormalisableRange で決まる割合) で持ち、プロセッサが元の値へ直す。
// 指し先を変えても、値は「範囲のどのあたりか」として残る。
//
// 置き換えはボイスごとに行う。コアは毎ブロック受け取る音源の値 (OpnaParams など)
// を写し、プロセッサの適用口 (Applier) に置き換えた値で読み直してもらってから使う。
// 置き換えるものが無いときは写さない (これまでと同じ道を通る)。
namespace ParamMod
{
	// scope は -1 がチャンネル、0 からがオペレーター。index はその scope の
	// 指し先の一覧 (プロセッサが持つ) の番号。value は 0〜1。
	struct Override
	{
		int scope = -1;
		int index = 0;
		float value = 0.0f;
	};

	// TAP PARAM SEQUENCER の 1 段に持てる組の数
	inline constexpr int maxPairs = StepTable::maxPairs;

	// オペレーター (パッド) の数の上限 (OPZX7 の 8)
	inline constexpr int maxOps = 8;

	// PARAM ENV の本数。チャンネル 1 + オペレーター 8 (OPZX7)
	inline constexpr int maxEnvs = 1 + maxOps;

	// TAP PARAM SEQUENCER はチャンネルと、オペレーター (パッド) ごとに 1 つずつ持つ
	inline constexpr int maxTapPairs = maxPairs * (1 + maxOps);

	inline constexpr int maxOverrides = maxTapPairs + maxEnvs;

	// 打鍵 1 回ぶんの TAP PARAM SEQUENCER の組 (チャンネルとオペレーターのぶんを合わせたもの)
	struct TapStep
	{
		std::array<Override, maxTapPairs> pairs{};
		int count = 0;
	};

	// PARAM ENV 1 本ぶんの値。プロセッサが入れる
	struct EnvParams
	{
		bool enable = false;
		int scope = -1;
		int index = 0;

		// 指し先が整数や選択肢。段のあいだは値を斜めに繋がない
		bool stepped = false;

		// 指し先が押したときに決まる値 (MUL・DT・FIX など)。鳴っている最中も読み直す
		bool live = false;

		StepEnvParams env;
	};

	// 置き換えた値で音源の値を読み直す口。プロセッサが持つ。
	// chip は音源ごとの値 (OpnaParams など) を指す。
	struct Applier
	{
		virtual ~Applier() = default;
		virtual void apply(void* chip, const Override* list, int count) = 0;
	};

	// 音源ごとの値 (OpnaParams など) に持たせる束
	struct ChipParams
	{
		Applier* applier = nullptr;
		std::array<EnvParams, maxEnvs> envs{};
		int envCount = 0;

		// ユニゾンの ARP FREQ (チャンネルの一覧の番号と範囲)。アルペジオはボイスが
		// 刻むので、コアではなくボイスがこの値を読む。持たなければ -1
		int arpIndex = -1;
		int arpMin = 0;
		int arpMax = 0;
	};

	// ボイス (コア) が持つ、置き換えの進み具合
	class Runtime
	{
	public:
		void prepare(double sampleRate)
		{
			for (auto& e : envs) e.prepare(0, -1, sampleRate);
		}

		void updateSampleRate(double sampleRate)
		{
			for (auto& e : envs) e.updateTargetSampleRate(sampleRate);
		}

		// 毎ブロック。エンベロープの値を受け取る。指し先が変わったら走らせ直すまで使わない
		void configure(const ChipParams& p)
		{
			count = std::clamp(p.envCount, 0, maxEnvs);
			arpIndex = p.arpIndex;
			arpMin = p.arpMin;
			arpMax = p.arpMax;

			for (int i = 0; i < count; ++i) {
				const auto& src = p.envs[(size_t)i];
				auto& c = config[(size_t)i];

				// 走っていたものは止める (グラフの縦線も消える)
				if (src.enable != c.enable || src.scope != c.scope || src.index != c.index) stopEnv(i);

				c.enable = src.enable;
				c.scope = src.scope;
				c.index = src.index;
				c.stepped = src.stepped;
				c.live = src.live;

				if (c.enable) envs[(size_t)i].setParameters(src.env);
			}

			for (int i = count; i < maxEnvs; ++i) {
				config[(size_t)i].enable = false;
				stopEnv(i);
			}
		}

		void setTap(const TapStep& step) { tap = step; }

		bool hasEnvs() const
		{
			for (int i = 0; i < count; ++i) if (config[(size_t)i].enable) return true;

			return false;
		}

		bool active() const { return tap.count > 0 || hasEnvs(); }

		// 押したときに決まる値 (MUL・DT・FIX など) を、鳴らしている最中に動かしているか。
		// ほかの指し先は毎ブロック置き換えるので、周波数を読み直さなくてよい
		bool envRunning() const
		{
			for (int i = 0; i < count; ++i) {
				const auto& c = config[(size_t)i];

				if (c.enable && c.live && started[(size_t)i]) return true;
			}

			return false;
		}

		// 押したとき。レガートでは走らせ直さない (ほかの 256 段のエンベロープと同じ)
		void noteOn(bool isLegato)
		{
			for (int i = 0; i < count; ++i) {
				if (!config[(size_t)i].enable) continue;
				if (isLegato && started[(size_t)i]) continue;

				envs[(size_t)i].noteOn();
				started[(size_t)i] = true;
				last[(size_t)i] = envs[(size_t)i].value();
				sinceApply[(size_t)i] = 0;
			}
		}

		void noteOff()
		{
			for (int i = 0; i < count; ++i) {
				if (config[(size_t)i].enable && started[(size_t)i]) envs[(size_t)i].noteOff();
			}
		}

		// n サンプル進める。読み直しが要るほど値が変わったら true。
		// 段で値を保つ指し先は変わるたびに、斜めに動く指し先は continuousInterval
		// サンプルに 1 回まで読み直す (読み直しは安くないため)。大きく跳んだときは待たない。
		bool advance(int n)
		{
			bool changed = false;

			for (int i = 0; i < count; ++i) {
				const auto& c = config[(size_t)i];

				if (!c.enable || !started[(size_t)i]) continue;

				auto& env = envs[(size_t)i];
				float v = env.value();

				for (int k = 0; k < n; ++k) v = env.process();

				auto& since = sinceApply[(size_t)i];

				since += n;

				if (v == last[(size_t)i]) continue;

				if (!c.stepped && since < continuousInterval && std::abs(v - last[(size_t)i]) < bigJump) continue;

				last[(size_t)i] = v;
				since = 0;
				changed = true;
			}

			return changed;
		}

		// いま置き換える値を並べる。TAP が先、エンベロープが後 (同じ指し先なら後が勝つ)。
		int collect(std::array<Override, maxOverrides>& out) const
		{
			int n = 0;

			for (int i = 0; i < tap.count && n < maxOverrides; ++i) out[(size_t)n++] = tap.pairs[(size_t)i];

			for (int i = 0; i < count && n < maxOverrides; ++i) {
				const auto& c = config[(size_t)i];

				if (!c.enable || !started[(size_t)i]) continue;

				out[(size_t)n++] = { c.scope, c.index, std::clamp(last[(size_t)i], 0.0f, 1.0f) };
			}

			return n;
		}

		// ARP FREQ を置き換えていれば、その値 (Hz) を返す
		bool arpOverride(int& freq) const
		{
			if (arpIndex < 0) return false;

			std::array<Override, maxOverrides> list;

			const int n = collect(list);

			for (int i = n - 1; i >= 0; --i) {
				const auto& o = list[(size_t)i];

				if (o.scope != -1 || o.index != arpIndex) continue;

				freq = (int)std::lround((float)arpMin + o.value * (float)(arpMax - arpMin));

				return true;
			}

			return false;
		}

	private:
		struct Config
		{
			bool enable = false;
			int scope = -1;
			int index = 0;
			bool stepped = false;
			bool live = false;
		};

		void stopEnv(int i)
		{
			if (started[(size_t)i]) envs[(size_t)i].stop();

			started[(size_t)i] = false;
		}

		// 斜めに動く値を読み直す間隔 (サンプル) と、待たずに読み直す跳び幅
		static constexpr int continuousInterval = 64;
		static constexpr float bigJump = 0.05f;

		std::array<StepEnv, maxEnvs> envs;
		std::array<Config, maxEnvs> config{};
		std::array<bool, maxEnvs> started{};
		std::array<float, maxEnvs> last{};
		std::array<int, maxEnvs> sinceApply{};
		int count = 0;

		int arpIndex = -1;
		int arpMin = 0;
		int arpMax = 0;

		TapStep tap;
	};
}
