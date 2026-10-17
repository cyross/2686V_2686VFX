#pragma once

#include <JuceHeader.h>

#include "./ProcessorHelper.h"
#include "./ProcessorStructs.h"
#include "./ProcessorValues.h"
#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Effect/Envelope/Step/EnvStep.h"

// ============================================================================
// 値のエンベロープ (DUTY RATIO ENV / PEAK RATIO ENV など)
// ============================================================================
// SSG SW AMP ENV[256] と同じ 256 段のエンベロープで、音量ではなく別の値
// (矩形波のデューティ比・三角波の頂点など) を動かすもの。どれも「Enable」で
// 入切し、パラメータの名前は頭 (base) に決まった尻尾を付けて作る。
//
//   <base>_ENABLE / _STEPS / _LOOP / _LOOPTO / _LOOPCNT / _STL / _LR / _RR /
//   _ENDL / _ENDL_EN / _KEEP
//
// 段の R / L は SSG SW AMP ENV[256] と同じく表 (StepTable) に持つ。表の鍵は
// 頭 + base (例 "SSG_DUTYENV")。
namespace ValueEnv
{
	struct Keys
	{
		juce::String base;
		juce::String enable, steps, loop, loopTo, loopCount, stl, lr, rr, endl, endlEnable, keep;

		explicit Keys(const juce::String& b) :
			base(b),
			enable(b + "_ENABLE"),
			steps(b + "_STEPS"),
			loop(b + "_LOOP"),
			loopTo(b + "_LOOPTO"),
			loopCount(b + "_LOOPCNT"),
			stl(b + "_STL"),
			lr(b + "_LR"),
			rr(b + "_RR"),
			endl(b + "_ENDL"),
			endlEnable(b + "_ENDL_EN"),
			keep(b + "_KEEP")
		{
		}
	};

	struct Spec
	{
		Keys keys;

		// パラメータの表示名の頭 (" SSG DutyEnv" など)
		juce::String name;

		// 値 (STL / L / LR / ENDL) の範囲
		float levelMin = 0.0f;
		float levelMax = 1.0f;
		float levelInitial = 0.0f;
	};

	// 起動時の初期化の順に頼らないよう、関数の中で作る (CPK の文字列と同じ理由)
	inline const Spec& duty()
	{
		static const Spec s{ Keys("_DUTYENV"), " SSG DutyEnv", 0.0f, 100.0f, 50.0f };
		return s;
	}

	inline const Spec& peak()
	{
		static const Spec s{ Keys("_PEAKENV"), " SSG PeakEnv", 0.0f, 1.0f, 0.5f };
		return s;
	}

	struct Ptrs : PrPtrsStepEnv
	{
		std::atomic<float>* enable = nullptr;
	};

	// 段の R の範囲は SSG SW AMP ENV[256] と同じ
	inline constexpr float rateInitial = CPV::SsgSwEnv256::R::initial;

	inline void addParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix, const juce::String& prefixName, const Spec& s)
	{
		const auto& k = s.keys;
		const auto n = prefixName + s.name;

		PrHelper::addBool(layout, prefix + k.enable, n + " Enable", false);
		PrHelper::addInt(layout, prefix + k.steps, n + " Steps",
			CPV::SsgSwEnv256::Steps::min, CPV::SsgSwEnv256::Steps::max, CPV::SsgSwEnv256::Steps::initial);
		PrHelper::addBool(layout, prefix + k.loop, n + " Loop", false);
		PrHelper::addInt(layout, prefix + k.loopTo, n + " Loop To",
			CPV::SsgSwEnv256::LoopTo::min, CPV::SsgSwEnv256::LoopTo::max, CPV::SsgSwEnv256::LoopTo::initial);
		PrHelper::addInt(layout, prefix + k.loopCount, n + " Loop Count",
			CPV::SsgSwEnv256::LoopCount::min, CPV::SsgSwEnv256::LoopCount::max, CPV::SsgSwEnv256::LoopCount::initial);
		PrHelper::addFloat(layout, prefix + k.stl, n + " STL", s.levelMin, s.levelMax, s.levelInitial);
		PrHelper::addFloat(layout, prefix + k.lr, n + " LR", s.levelMin, s.levelMax, s.levelInitial);
		PrHelper::addFloat(layout, prefix + k.rr, n + " RR", CPV::SsgSwEnv256::RR::min, CPV::SsgSwEnv256::RR::max, CPV::SsgSwEnv256::RR::initial);
		PrHelper::addFloat(layout, prefix + k.endl, n + " EndL", s.levelMin, s.levelMax, s.levelInitial);
		PrHelper::addBool(layout, prefix + k.endlEnable, n + " EndL Enable", false);
		PrHelper::addBool(layout, prefix + k.keep, n + " Keep", false);
	}

	inline void setupPtrs(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix, const Spec& s, Ptrs& p, StepTableStore& tables)
	{
		const auto& k = s.keys;

		p.enable = apvts.getRawParameterValue(prefix + k.enable);
		p.steps = apvts.getRawParameterValue(prefix + k.steps);
		p.loop = apvts.getRawParameterValue(prefix + k.loop);
		p.loopTo = apvts.getRawParameterValue(prefix + k.loopTo);
		p.loopCount = apvts.getRawParameterValue(prefix + k.loopCount);
		p.stl = apvts.getRawParameterValue(prefix + k.stl);
		p.lr = apvts.getRawParameterValue(prefix + k.lr);
		p.rr = apvts.getRawParameterValue(prefix + k.rr);
		p.endl = apvts.getRawParameterValue(prefix + k.endl);
		p.endlEnable = apvts.getRawParameterValue(prefix + k.endlEnable);
		p.keep = apvts.getRawParameterValue(prefix + k.keep);

		// 段の R / L はプロセッサが持つ表から引く
		p.table = &tables.get(prefix + k.base, rateInitial, s.levelInitial);
	}

	inline void apply(const Ptrs& p, StepEnvParams& params, bool& enable)
	{
		enable = PrHelper::getBool(p.enable);
		params.bypass = false;
		params.steps = PrHelper::getInt(p.steps);
		params.loop = PrHelper::getBool(p.loop);
		params.loopTo = PrHelper::getInt(p.loopTo);
		params.loopCount = PrHelper::getInt(p.loopCount);
		params.stl = PrHelper::getFloat(p.stl);
		params.lr = PrHelper::getFloat(p.lr);
		params.rr = PrHelper::getFloat(p.rr);
		params.endl = PrHelper::getFloat(p.endl);
		params.endlEnable = PrHelper::getBool(p.endlEnable);
		params.keep = PrHelper::getBool(p.keep);
		params.table = p.table;
	}
}
