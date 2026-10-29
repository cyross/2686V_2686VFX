#pragma once

#include <JuceHeader.h>

#include <array>
#include <vector>

#include "./ProcessorHelper.h"
#include "./ProcessorOverride.h"
#include "./ProcessorTapSeq.h"
#include "Shared/Core/Synth/ParamMod.h"
#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Processor/Beep/ProcessorBeepKeys.h"

// ============================================================================
// PARAM ENV[256] と TAP PARAM SEQUENCER (パラメータの側, 4.3.0)
// ============================================================================
// 動かせるパラメータ (指し先) の一覧と、2 つの機能のパラメータ、
// 置き換えた値を元のパラメータの値へ直す道具 (Resolver) をまとめる。
//
// 指し先は APVTS のパラメータそのもの。一覧は鍵の尻尾と画面の名前だけを持ち、
// 範囲や刻み、値の見せ方はパラメータ自身から引く。
//
//   ch : チャンネルの指し先。鍵は 頭 + 尻尾                (OPNA_ALG)
//   op : オペレーターの指し先。鍵は 頭 + _OP + 番号 + 尻尾 (OPNA_OP0_R_AR)
//        RHYTHM ではパッド                                  (RHYTHM_PAD0_FREQ)
//
// 一覧の並びの番号は、PARAM ENV の TARGET (パラメータ) と TAP PARAM SEQUENCER の
// 組 (state) に書かれる。足すときは後ろへ足し、並べ替えたり消したりしないこと。
namespace ParamModCatalog
{
	struct Entry
	{
		juce::String suffix;
		juce::String label;
	};

	struct Catalog
	{
		std::vector<Entry> ch;
		std::vector<Entry> op;

		// オペレーター (パッド) の鍵の印と、画面に出す名前
		juce::String opKey = CPK::op;
		juce::String opLabel = "OP";

		// チャンネルの PARAM ENV に、オペレーター (パッド) の指し先も並べる (RHYTHM)。
		// RHYTHM は 4.3.0 の途中までパッドごとの PARAM ENV を持たなかった。
		// パッドごとのものを足したあとも、保存した TARGET の番号を変えないよう残す。
		bool flattenOps = false;

		// TL (op の一覧の番号)。PARAM ENV の TARGET の初期値で、TL ENV を写す先。無ければ -1
		int tlIndex = -1;

		// ARP FREQ (ch の一覧の番号)。アルペジオはボイスが刻むので、ボイスが読む
		int arpIndex = -1;
	};

	namespace detail
	{
		// WT PITCH MOD / WT AMP MOD / SSG HW ENV / SSG HW PITCH ENV の速さと波形。
		// チャンネルにも、FM のオペレーターにもある
		inline void addMods(std::vector<Entry>& v)
		{
			v.push_back({ CPK::WtMod::speed, "WT PITCH MOD SPEED" });
			v.push_back({ CPK::WtMod::waveSlot, "WT PITCH MOD WAVE" });
			v.push_back({ CPK::WtAmpMod::speed, "WT AMP MOD SPEED" });
			v.push_back({ CPK::WtAmpMod::waveSlot, "WT AMP MOD WAVE" });
			v.push_back({ CPK::SsgHwEnv::period, "SSG HW ENV PERIOD" });
			v.push_back({ CPK::SsgHwPEnv::period, "SSG HW PITCH ENV PERIOD" });
		}

		// OPZX7 と同じ形の LFO (FM 以外のチャンネル)
		inline void addOpzx7LfoFreqs(std::vector<Entry>& v)
		{
			v.push_back({ CPK::Opzx7Lfo::pmFreq, "LFO PM FREQ" });
			v.push_back({ CPK::Opzx7Lfo::amFreq, "LFO AM FREQ" });
		}

		inline void addArp(Catalog& c)
		{
			c.arpIndex = (int)c.ch.size();
			c.ch.push_back({ CPK::Unison::arpFreq, "ARP FREQ" });
		}

		// FM 以外のチャンネルに共通する後ろの並び
		inline void addNonFmTail(Catalog& c)
		{
			c.ch.push_back({ CPK::fixFreq, "FIX FREQ" });
			addOpzx7LfoFreqs(c.ch);
			addMods(c.ch);
			addArp(c);
		}
	}

	inline const Catalog& opna()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = { { CPK::Fm::alg, "ALG" }, { CPK::Fm::fb, "FB" }, { CPK::N88Lfo::freq, "N88 LFO FREQ" } };
			detail::addMods(x.ch);
			detail::addArp(x);

			x.tlIndex = 0;
			x.op = {
				{ CPK::Fm::rgTl, "TL" }, { CPK::Fm::rgAr, "AR" }, { CPK::Fm::rgDr, "DR" }, { CPK::Fm::rgSr, "SR" },
				{ CPK::Fm::rgSl, "SL" }, { CPK::Fm::rgRr, "RR" }, { CPK::Fm::ks, "KS" }, { CPK::mul, "MUL" },
				{ CPK::dt, "DT" }, { CPK::fixFreq, "FIX FREQ" }, { CPK::Fm::se, "SSG-EG" }, { CPK::Fm::seFreq, "SSG-EG FREQ" },
				{ CPK::OpnaLfo::freqs, "HW LFO FREQ" }, { CPK::OpnaLfo::pms, "HW LFO PMS" }, { CPK::OpnaLfo::ams, "HW LFO AMS" },
				{ CPK::N88Lfo::ams, "N88 LFO AMS" },
			};
			detail::addMods(x.op);

			return x;
		}();

		return c;
	}

	inline const Catalog& opn()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = { { CPK::Fm::alg, "ALG" }, { CPK::Fm::fb, "FB" }, { CPK::N88Lfo::freq, "N88 LFO FREQ" } };
			detail::addMods(x.ch);
			detail::addArp(x);

			x.tlIndex = 0;
			x.op = {
				{ CPK::Fm::rgTl, "TL" }, { CPK::Fm::rgAr, "AR" }, { CPK::Fm::rgDr, "DR" }, { CPK::Fm::rgSr, "SR" },
				{ CPK::Fm::rgSl, "SL" }, { CPK::Fm::rgRr, "RR" }, { CPK::Fm::ks, "KS" }, { CPK::mul, "MUL" },
				{ CPK::dt, "DT" }, { CPK::fixFreq, "FIX FREQ" }, { CPK::N88Lfo::ams, "N88 LFO AMS" },
			};
			detail::addMods(x.op);

			return x;
		}();

		return c;
	}

	inline const Catalog& opm()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = { { CPK::Fm::alg, "ALG" }, { CPK::Fm::fb, "FB" }, { CPK::OpmLfo::freq, "HW LFO FREQ" } };
			detail::addMods(x.ch);
			detail::addArp(x);

			x.tlIndex = 0;
			x.op = {
				{ CPK::Fm::rgTl, "TL" }, { CPK::Fm::rgAr, "AR" }, { CPK::Fm::rgD1r, "D1R" }, { CPK::Fm::rgD1l, "D1L" },
				{ CPK::Fm::rgD2r, "D2R" }, { CPK::Fm::rgRr, "RR" }, { CPK::Fm::ksMode, "KS MODE" }, { CPK::Fm::ks, "KS" },
				{ CPK::Fm::ksrOPP, "KSR (OPP)" }, { CPK::Fm::kslOPP, "KSL (OPP)" }, { CPK::mul, "MUL" }, { CPK::mulRatio, "MUL RATIO" },
				{ CPK::dt, "DT1" }, { CPK::dt2, "DT2" }, { CPK::fixFreq, "FIX FREQ" }, { CPK::Fm::seFreq, "SSG-EG FREQ" },
			};
			detail::addMods(x.op);

			return x;
		}();

		return c;
	}

	// OPL と OPL3 は同じ並び
	inline const Catalog& opl()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = { { CPK::Fm::alg, "ALG" }, { CPK::Fm::fb, "FB" } };
			detail::addMods(x.ch);
			detail::addArp(x);

			x.tlIndex = 0;
			x.op = {
				{ CPK::Fm::rgTl, "TL" }, { CPK::Fm::rgAr, "AR" }, { CPK::Fm::rgDr, "DR" }, { CPK::Fm::rgSl, "SL" },
				{ CPK::Fm::rgRr, "RR" }, { CPK::Fm::ksl, "KSL" }, { CPK::mul, "MUL" }, { CPK::Fm::eg, "EG" },
				{ CPK::OplLfo::ams, "LFO AM SPEED" }, { CPK::OplLfo::amd, "LFO AM DEPTH" },
				{ CPK::OplLfo::pms, "LFO PM SPEED" }, { CPK::OplLfo::pmd, "LFO PM DEPTH" },
			};
			detail::addMods(x.op);

			return x;
		}();

		return c;
	}

	inline const Catalog& opl3() { return opl(); }

	inline const Catalog& opzx7()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = {
				{ CPK::Fm::alg, "ALG" },
				{ CPK::Fm::fb1, "FB OP1" }, { CPK::Fm::fb2, "FB OP2" }, { CPK::Fm::fb3, "FB OP3" }, { CPK::Fm::fb4, "FB OP4" },
				{ CPK::Fm::fb5, "FB OP5" }, { CPK::Fm::fb6, "FB OP6" }, { CPK::Fm::fb7, "FB OP7" }, { CPK::Fm::fb8, "FB OP8" },
			};
			detail::addOpzx7LfoFreqs(x.ch);
			detail::addMods(x.ch);
			detail::addArp(x);

			// RG-EN が入っているときはレジスタ値 (TL など)、切れているときは実数の値
			// (TL (F) など) が効く
			x.tlIndex = 0;
			x.op = {
				{ CPK::Fm::rgTl, "TL" }, { CPK::Fm::rgAr, "AR" }, { CPK::Fm::rgD1r, "D1R" }, { CPK::Fm::rgD1l, "D1L" },
				{ CPK::Fm::rgD2r, "D2R" }, { CPK::Fm::rgRr, "RR" },
				{ CPK::Fm::tl, "TL (F)" }, { CPK::Fm::ar, "AR (F)" }, { CPK::Fm::d1r, "D1R (F)" }, { CPK::Fm::d1l, "D1L (F)" },
				{ CPK::Fm::d2r, "D2R (F)" }, { CPK::Fm::rr, "RR (F)" },
				{ CPK::Fm::ksMode, "KS MODE" }, { CPK::Fm::kslMA7, "KSL" }, { CPK::Fm::ksrOPZ, "KSR (OPZ)" }, { CPK::Fm::kslOPZ, "KSL (OPZ)" },
				{ CPK::Fm::ksBp, "KS BP" }, { CPK::Fm::ksLc, "KS LC" }, { CPK::Fm::ksRc, "KS RC" }, { CPK::Fm::ksLd, "KS LD" },
				{ CPK::Fm::ksRd, "KS RD" }, { CPK::Fm::ksRs, "KS RS" },
				{ CPK::mul, "MUL" }, { CPK::mulRatio, "MUL RATIO" }, { CPK::dt, "DT1" }, { CPK::dt2, "DT2" }, { CPK::dt3, "DT3" },
				{ CPK::Fm::ws, "WS" }, { CPK::fixFreq, "FIX FREQ" }, { CPK::Fm::se, "SSG-EG" }, { CPK::Fm::seFreq, "SSG-EG FREQ" },
				{ CPK::speed, "PCM SPEED" },
				{ CPK::Opzx7Lfo::pmFreq, "LFO PM FREQ" }, { CPK::Opzx7Lfo::amFreq, "LFO AM FREQ" },
				{ CPK::Opzx7Lfo::pms, "LFO PMS" }, { CPK::Opzx7Lfo::ams, "LFO AMS" },
				{ CPK::Opzx7Lfo::pmd, "LFO PMD" }, { CPK::Opzx7Lfo::amd, "LFO AMD" },
			};
			detail::addMods(x.op);

			return x;
		}();

		return c;
	}

	inline const Catalog& ssg()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = {
				{ CPK::SsgDuty::preset, "DUTY PRESET" }, { CPK::SsgDuty::var, "DUTY" },
				{ CPK::SsgTri::peak, "TRI PEAK" }, { CPK::SsgTri::freq, "TRI FREQ" },
				{ CPK::Tn::freq, "NOISE FREQ" }, { CPK::Tn::toneValue, "TONE VALUE" }, { CPK::Tn::noiseValue, "NOISE VALUE" },
				{ CPK::speed, "SPEED" },
			};
			detail::addNonFmTail(x);

			return x;
		}();

		return c;
	}

	inline const Catalog& wt()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = { { CPK::Wt::wave, "WAVE" }, { CPK::speed, "SPEED" } };
			detail::addNonFmTail(x);

			return x;
		}();

		return c;
	}

	inline const Catalog& wt2()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = { { CPK::Wt2::wave, "WAVE" }, { CPK::speed, "SPEED" } };
			detail::addNonFmTail(x);

			return x;
		}();

		return c;
	}

	// WT+ の波形は波形メモリのスロット
	inline const Catalog& wtPlus()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = { { CPK::Wt::slot, "WAVE" } };
			detail::addNonFmTail(x);

			return x;
		}();

		return c;
	}

	inline const Catalog& adpcm()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = { { CPK::speed, "SPEED" }, { CPK::Tn::freq, "NOISE FREQ" } };
			detail::addNonFmTail(x);

			return x;
		}();

		return c;
	}

	// ADPCM+ の音声は PCM のスロット
	inline const Catalog& adpcmPlus()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = { { CPK::AdpcmPlus::slot, "VOICE" } };
			detail::addNonFmTail(x);

			return x;
		}();

		return c;
	}

	inline const Catalog& beep()
	{
		static const Catalog c = [] {
			Catalog x;

			x.ch = { { BeepPrKey::timerClock, "TIMER CLOCK" } };
			detail::addNonFmTail(x);

			return x;
		}();

		return c;
	}

	// RHYTHM はパッドごとに値を持つので、指し先はパッド (op の一覧) に並べる
	inline const Catalog& rhythm()
	{
		static const Catalog c = [] {
			Catalog x;

			detail::addArp(x);

			x.opKey = "_PAD";
			x.opLabel = "PAD";
			x.flattenOps = true;
			x.op = { { CPK::speed, "SPEED" }, { CPK::Tn::freq, "NOISE FREQ" }, { CPK::fixFreq, "FIX FREQ" } };
			detail::addOpzx7LfoFreqs(x.op);
			detail::addMods(x.op);

			return x;
		}();

		return c;
	}

	// 鍵 (APVTS のパラメータ ID)。op は 0 から
	inline juce::String paramId(const juce::String& prefix, const Catalog& c, int scope, int index)
	{
		if (scope < 0) return index >= 0 && index < (int)c.ch.size() ? prefix + c.ch[(size_t)index].suffix : juce::String();

		return index >= 0 && index < (int)c.op.size() ? prefix + c.opKey + juce::String(scope) + c.op[(size_t)index].suffix : juce::String();
	}

	// 画面に出す名前。オペレーター (パッド) の指し先は番号を付ける (OP1 AR)
	inline juce::String label(const Catalog& c, int scope, int index)
	{
		if (scope < 0) return index >= 0 && index < (int)c.ch.size() ? c.ch[(size_t)index].label : juce::String("?");

		return c.opLabel + juce::String(scope + 1) + " " + (index >= 0 && index < (int)c.op.size() ? c.op[(size_t)index].label : juce::String("?"));
	}

	// チャンネルの PARAM ENV の TARGET に並べる数。flattenOps ならパッドごとの指し先が続く
	inline int channelTargetCount(const Catalog& c, int ops)
	{
		return (int)c.ch.size() + (c.flattenOps ? ops * (int)c.op.size() : 0);
	}

	// チャンネルの PARAM ENV の TARGET の番号を (scope, index) へ
	inline void decodeChannelTarget(const Catalog& c, int target, int& scope, int& index)
	{
		const int chCount = (int)c.ch.size();

		if (target < chCount || !c.flattenOps || c.op.empty()) {
			scope = -1;
			index = target;
			return;
		}

		const int k = target - chCount;

		scope = k / (int)c.op.size();
		index = k % (int)c.op.size();
	}

	// チャンネルの PARAM ENV の TARGET に並べる名前
	inline juce::StringArray channelTargetLabels(const Catalog& c, int ops)
	{
		juce::StringArray names;

		const int n = channelTargetCount(c, ops);

		for (int t = 0; t < n; ++t) {
			int scope = -1, index = 0;

			decodeChannelTarget(c, t, scope, index);
			names.add(label(c, scope, index));
		}

		return names;
	}

	// 指し先のパラメータの鍵。チャンネルの PARAM ENV の TARGET の並び (flattenOps を含む)
	inline std::vector<juce::String> channelTargetIds(const juce::String& prefix, const Catalog& c, int ops)
	{
		std::vector<juce::String> ids;

		const int n = channelTargetCount(c, ops);

		for (int t = 0; t < n; ++t) {
			int scope = -1, index = 0;

			decodeChannelTarget(c, t, scope, index);
			ids.push_back(paramId(prefix, c, scope, index));
		}

		return ids;
	}

	// オペレーター (パッド) の頭 (OPNA_OP0) に尻尾を足した鍵。オペレーターの PARAM ENV の TARGET の並び
	inline std::vector<juce::String> opTargetIds(const juce::String& opPrefix, const Catalog& c)
	{
		std::vector<juce::String> ids;

		for (const auto& e : c.op) ids.push_back(opPrefix + e.suffix);

		return ids;
	}

	// パラメータの初期値 (0〜1)。PARAM ENV の値の組を、指し先の初期値から始めるのに使う
	inline std::vector<float> defaultsOf(juce::AudioProcessorValueTreeState& apvts, const std::vector<juce::String>& ids)
	{
		std::vector<float> defaults;

		for (const auto& id : ids) {
			auto* param = apvts.getParameter(id);

			defaults.push_back(param != nullptr ? param->getDefaultValue() : 0.0f);
		}

		return defaults;
	}

	inline juce::StringArray opTargetLabels(const Catalog& c)
	{
		juce::StringArray names;

		for (const auto& e : c.op) names.add(e.label);

		return names;
	}
}

// ============================================================================
// PARAM ENV[256] のパラメータ
// ============================================================================
// SSG SW AMP ENV[256] と同じ頭に、指し先 (TARGET) を足したもの。値 (STL・段の L・LR・
// ENDL) は指し先のパラメータの 0〜1 で、指し先ごとに表の値の組 (StepTable::sets) へ持つ。
// 指し先を変えると、その指し先の値で鳴る。R・STEP・ループは指し先によらず 1 つ。
//
//   <base>_ENABLE / _TARGET / _STEPS / _LOOP / _LOOPTO / _LOOPCNT / _LOOPMODE /
//   _RR / _ENDL_EN / _KEEP
//
// 4.3.0 の最初の形は STL / LR / ENDL をパラメータに持ち、指し先で分けていなかった。
// その state は IoParamEnvLegacy.h が、保存したときの指し先の組へ写す。
//
// チャンネルに 1 本 (頭 + base)、FM はオペレーターごとにも 1 本 (頭 + _OPn + base)、
// RHYTHM はパッドごとにも 1 本 (頭 + _PADn + base)。
// オペレーターの PARAM ENV は 4.0.0 の TL ENV の代わりで、TARGET の初期値は TL。
namespace ParamEnv
{
	inline const juce::String base = "_PRMENV";

	struct Keys
	{
		juce::String base;
		juce::String enable, target, steps, loop, loopTo, loopCount, loopMode, rr, endlEnable, keep;

		explicit Keys(const juce::String& b) :
			base(b),
			enable(b + "_ENABLE"),
			target(b + "_TARGET"),
			steps(b + "_STEPS"),
			loop(b + "_LOOP"),
			loopTo(b + "_LOOPTO"),
			loopCount(b + "_LOOPCNT"),
			loopMode(b + "_LOOPMODE"),
			rr(b + "_RR"),
			endlEnable(b + "_ENDL_EN"),
			keep(b + "_KEEP")
		{
		}
	};

	inline const Keys& keys()
	{
		static const Keys k(base);
		return k;
	}

	// 段の R の範囲は SSG SW AMP ENV[256] と同じ
	inline constexpr float rateInitial = CPV::SsgSwEnv256::R::initial;

	struct Ptrs : PrPtrsStepEnv
	{
		std::atomic<float>* enable = nullptr;
		std::atomic<float>* target = nullptr;
	};

	// targets は TARGET に並べる数
	inline void addParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName, int targets, int targetInitial)
	{
		const auto& k = keys();
		const auto n = prefixName + " ParamEnv";

		PrHelper::addBool(layout, prefix + k.enable, n + " Enable", false);
		PrHelper::addInt(layout, prefix + k.target, n + " Target", 0, juce::jmax(1, targets) - 1, targetInitial);
		PrHelper::addInt(layout, prefix + k.steps, n + " Steps",
			CPV::SsgSwEnv256::Steps::min, CPV::SsgSwEnv256::Steps::max, CPV::SsgSwEnv256::Steps::initial);
		PrHelper::addBool(layout, prefix + k.loop, n + " Loop", false);
		PrHelper::addInt(layout, prefix + k.loopTo, n + " Loop To",
			CPV::SsgSwEnv256::LoopTo::min, CPV::SsgSwEnv256::LoopTo::max, CPV::SsgSwEnv256::LoopTo::initial);
		PrHelper::addInt(layout, prefix + k.loopCount, n + " Loop Count",
			CPV::SsgSwEnv256::LoopCount::min, CPV::SsgSwEnv256::LoopCount::max, CPV::SsgSwEnv256::LoopCount::initial);
		PrHelper::addInt(layout, prefix + k.loopMode, n + " Loop Mode", StepLoop::modeMin, StepLoop::modeMax, StepLoop::modeInitial);
		PrHelper::addFloat(layout, prefix + k.rr, n + " RR", CPV::SsgSwEnv256::RR::min, CPV::SsgSwEnv256::RR::max, CPV::SsgSwEnv256::RR::initial);
		PrHelper::addBool(layout, prefix + k.endlEnable, n + " EndL Enable", false);
		PrHelper::addBool(layout, prefix + k.keep, n + " Keep", false);
	}

	// defaults は指し先ごとの値の組の初期値 (TARGET の並びの順)
	inline void setupPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, Ptrs& p, StepTableStore& tables, const std::vector<float>& defaults)
	{
		const auto& k = keys();

		p.enable = apvts.getRawParameterValue(prefix + k.enable);
		p.target = apvts.getRawParameterValue(prefix + k.target);
		p.steps = apvts.getRawParameterValue(prefix + k.steps);
		p.loop = apvts.getRawParameterValue(prefix + k.loop);
		p.loopTo = apvts.getRawParameterValue(prefix + k.loopTo);
		p.loopCount = apvts.getRawParameterValue(prefix + k.loopCount);
		p.loopMode = apvts.getRawParameterValue(prefix + k.loopMode);
		p.rr = apvts.getRawParameterValue(prefix + k.rr);
		p.endlEnable = apvts.getRawParameterValue(prefix + k.endlEnable);
		p.keep = apvts.getRawParameterValue(prefix + k.keep);

		// 段の R と、指し先ごとの値の組はプロセッサが持つ表から引く
		p.table = &tables.getWithLevelSets(prefix + k.base, rateInitial, defaults);
	}

	inline int target(const Ptrs& p) { return PrHelper::getInt(p.target); }

	// stepped は指し先が整数や選択肢か。そのときは常に KEEP にする。
	// set は値の組 (TARGET の値)。STL・L・LR・ENDL は音の側 (StepEnv) が組から取る
	inline void applyEnv(const Ptrs& p, StepEnvParams& params, bool stepped, int set)
	{
		params.bypass = false;
		params.steps = PrHelper::getInt(p.steps);
		params.loop = PrHelper::getBool(p.loop);
		params.loopTo = PrHelper::getInt(p.loopTo);
		params.loopCount = PrHelper::getInt(p.loopCount);
		params.loopMode = PrHelper::getInt(p.loopMode);
		params.stl = 0.0f;
		params.lr = 0.0f;
		params.rr = PrHelper::getFloat(p.rr);
		params.endl = 0.0f;
		params.endlEnable = PrHelper::getBool(p.endlEnable);
		params.keep = stepped || PrHelper::getBool(p.keep);
		params.table = p.table;
		params.levelSet = set;
	}
}

// ============================================================================
// TAP PARAM SEQUENCER のパラメータ
// ============================================================================
// 頭 (Enable・STEPS・LOOP と、段を動かす 3 つの札) は TAP AMP / PITCH SEQUENCER と
// 同じ。段の値の代わりに、段ごとの (指し先, 値) の組を表の pairs に持つ。
namespace TapParam
{
	inline const TapSeq::Spec& spec()
	{
		static const TapSeq::Spec s{ TapSeq::Keys("_TAPPARAM"), " TapParam", 0.0f, 1.0f, 0.0f, false };
		return s;
	}

	inline void addParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName)
	{
		TapSeq::addParameters(layout, prefix, prefixName, spec());
	}

	inline void setupPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, TapSeq::Ptrs& p, StepTableStore& tables)
	{
		const auto& k = spec().keys;

		p.enable = apvts.getRawParameterValue(prefix + k.enable);
		p.steps = apvts.getRawParameterValue(prefix + k.steps);
		p.loop = apvts.getRawParameterValue(prefix + k.loop);
		p.inc = apvts.getRawParameterValue(prefix + k.inc);
		p.dec = apvts.getRawParameterValue(prefix + k.dec);
		p.reset = apvts.getRawParameterValue(prefix + k.reset);

		// 組の置き場を持つ表
		p.table = &tables.getWithPairs(prefix + k.base);
	}
}

// ============================================================================
// 置き換えた値を、元のパラメータの値へ直す
// ============================================================================
// プロセッサが 1 つ持つ。init で指し先のパラメータを引いておき、ボイスから
// 頼まれたら (ParamMod::Applier) 0〜1 の値をパラメータの値へ直して PrOverride に並べる。
class ParamModResolver
{
public:
	// ops はオペレーター (パッド) の数
	void init(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, const ParamModCatalog::Catalog& c, int ops)
	{
		catalog = &c;
		opCount = juce::jlimit(0, ParamMod::maxEnvs - 1, ops);

		auto slotOf = [&apvts](const juce::String& id) {
			Slot s;

			s.ptr = apvts.getRawParameterValue(id);
			s.param = apvts.getParameter(id);

			// 一覧に書いた鍵がパラメータに無い。一覧を直すこと
			jassert(s.ptr != nullptr && s.param != nullptr);

			s.stepped = s.param != nullptr && (dynamic_cast<juce::AudioParameterInt*>(s.param) != nullptr
				|| dynamic_cast<juce::AudioParameterChoice*>(s.param) != nullptr
				|| dynamic_cast<juce::AudioParameterBool*>(s.param) != nullptr);

			return s;
		};

		ch.clear();
		op.clear();

		for (int i = 0; i < (int)c.ch.size(); ++i) ch.push_back(slotOf(ParamModCatalog::paramId(prefix, c, -1, i)));

		op.resize((size_t)opCount);

		for (int o = 0; o < opCount; ++o) {
			for (int i = 0; i < (int)c.op.size(); ++i) op[(size_t)o].push_back(slotOf(ParamModCatalog::paramId(prefix, c, o, i)));
		}
	}

	const ParamModCatalog::Catalog& getCatalog() const { return *catalog; }
	int getOpCount() const { return opCount; }

	bool isStepped(int scope, int index) const
	{
		const auto* s = slot(scope, index);

		return s != nullptr && s->stepped;
	}

	// TL の最大値 (TL ENV の値を TL のレジスタ値へ戻すのに使う)
	float maxOf(int scope, int index) const
	{
		const auto* s = slot(scope, index);

		return s != nullptr && s->param != nullptr ? s->param->convertFrom0to1(1.0f) : 0.0f;
	}

	struct Dirty
	{
		bool channel = false;
		std::array<bool, ParamMod::maxEnvs> ops{};
	};

	// 置き換える値を PrOverride の並びにする。読み直すべき区分を dirty に付ける。
	// 返すのは並べた数
	int resolve(const ParamMod::Override* list, int n, std::array<PrOverride::Entry, ParamMod::maxOverrides>& out, Dirty& dirty) const
	{
		int m = 0;

		for (int i = 0; i < n && m < ParamMod::maxOverrides; ++i) {
			const auto& o = list[i];
			const auto* s = slot(o.scope, o.index);

			if (s == nullptr || s->ptr == nullptr || s->param == nullptr) continue;

			float raw = s->param->convertFrom0to1(juce::jlimit(0.0f, 1.0f, o.value));

			if (s->stepped) raw = std::round(raw);

			out[(size_t)m++] = { s->ptr, raw };

			if (o.scope < 0) dirty.channel = true;
			else dirty.ops[(size_t)o.scope] = true;
		}

		return m;
	}

	// チャンネルの PARAM ENV を音の側の値にする
	void applyChannelEnv(const ParamEnv::Ptrs& p, ParamMod::EnvParams& out) const
	{
		int scope = -1, index = 0;

		const int target = ParamEnv::target(p);

		ParamModCatalog::decodeChannelTarget(*catalog, target, scope, index);

		out.enable = PrHelper::getBool(p.enable) && slot(scope, index) != nullptr;
		out.scope = scope;
		out.index = index;
		out.stepped = isStepped(scope, index);

		ParamEnv::applyEnv(p, out.env, out.stepped, target);
	}

	// オペレーターの PARAM ENV を音の側の値にする。TL を指しているときは、
	// オペレーターの TL ENV (4.0.0 から持つ、TL の道) へ渡す。音は 4.0.0 の TL ENV と同じ。
	void applyOpEnv(const ParamEnv::Ptrs& p, int opIndex, ParamMod::EnvParams& out, bool& tlEnable, StepEnvParams& tlEnv, float& tlMax) const
	{
		const int index = ParamEnv::target(p);
		const bool enable = PrHelper::getBool(p.enable) && slot(opIndex, index) != nullptr;
		const bool stepped = isStepped(opIndex, index);

		out.scope = opIndex;
		out.index = index;
		out.stepped = stepped;

		ParamEnv::applyEnv(p, out.env, stepped, index);

		if (index == catalog->tlIndex) {
			out.enable = false;
			tlEnable = enable;
			tlEnv = out.env;
			tlMax = maxOf(opIndex, index);
		}
		else {
			out.enable = enable;
			tlEnable = false;
		}
	}

	// TL の道を持たないもの (RHYTHM のパッド) の PARAM ENV を音の側の値にする
	void applyOpEnv(const ParamEnv::Ptrs& p, int opIndex, ParamMod::EnvParams& out) const
	{
		const int index = ParamEnv::target(p);

		out.enable = PrHelper::getBool(p.enable) && slot(opIndex, index) != nullptr;
		out.scope = opIndex;
		out.index = index;
		out.stepped = isStepped(opIndex, index);

		ParamEnv::applyEnv(p, out.env, out.stepped, index);
	}

	// 音源の値に、ARP FREQ の在りかと範囲を入れる
	void fillArp(ParamMod::ChipParams& p) const
	{
		p.arpIndex = -1;

		const int i = catalog->arpIndex;
		const auto* s = slot(-1, i);

		if (s == nullptr || s->param == nullptr) return;

		p.arpIndex = i;
		p.arpMin = (int)std::lround(s->param->convertFrom0to1(0.0f));
		p.arpMax = (int)std::lround(s->param->convertFrom0to1(1.0f));
	}

private:
	struct Slot
	{
		std::atomic<float>* ptr = nullptr;
		juce::RangedAudioParameter* param = nullptr;
		bool stepped = false;
	};

	const Slot* slot(int scope, int index) const
	{
		if (scope < 0) return index >= 0 && index < (int)ch.size() ? &ch[(size_t)index] : nullptr;
		if (scope >= opCount) return nullptr;

		const auto& v = op[(size_t)scope];

		return index >= 0 && index < (int)v.size() ? &v[(size_t)index] : nullptr;
	}

	const ParamModCatalog::Catalog* catalog = nullptr;
	int opCount = 0;

	std::vector<Slot> ch;
	std::vector<std::vector<Slot>> op;
};
