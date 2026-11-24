// ============================================================================
// エンベロープ・シーケンサーのパターン (4.5.0)
// ============================================================================
// 区分の設定を組ごとに持ち、PATTERN で音の側が読む値を切り替える。
//   ・PATTERNS が 1 (既定) のときは、つまみの値がそのまま音の側へ届く
//   ・PATTERN を変えると、音の側はその組の値を読み、APVTS にもその組の値が戻る
//   ・RESET TO CHANGE を入れていると、切り替えで始め直しの数が進む
//   ・組の値は state へ書いて読み直すと戻る
//   ・段の表は組ごとに別の表を持つ
// どの本にもある UNISON (VOICES) で確かめる。段の表は SSG SW AMP ENV[256] で
// (持っている本だけ)。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Core/Processor/PluginProcessor.h"
#include "Shared/Core/Processor/ProcessorPattern.h"

namespace
{
	// 先頭の UNISON の区分の頭 (OPNA_UNI など)
	juce::String findBase(AudioPlugin2686V& processor, const juce::String& token)
	{
		for (auto* p : processor.getParameters()) {
			auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(p);

			if (ranged == nullptr) continue;

			const auto id = ranged->getParameterID();

			if (id.endsWith(token + EnvPattern::patternsKey)) return id.dropLastCharacters(EnvPattern::patternsKey.length());
		}

		return {};
	}

	void set(AudioPlugin2686V& processor, const juce::String& id, float value)
	{
		auto* param = processor.apvts.getParameter(id);

		REQUIRE_MESSAGE(param != nullptr, id.toStdString());

		param->setValueNotifyingHost(param->convertTo0to1(value));
	}

	float get(AudioPlugin2686V& processor, const juce::String& id)
	{
		return processor.apvts.getRawParameterValue(id)->load();
	}
}

TEST_CASE("Pattern: PATTERN を変えると、音の側が読む値とつまみがその組の値になる")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& store = processor->patterns;

	const auto base = findBase(*processor, EnvPattern::unison);

	REQUIRE(base.isNotEmpty());

	const auto voices = base + "_VOICES";
	auto* live = PatternStore::raw(processor->apvts, voices);

	// 音の側はパラメータを直に読まない
	CHECK(live != processor->apvts.getRawParameterValue(voices));

	// 組が 1 つのときは、つまみの値がそのまま届く
	set(*processor, voices, 4.0f);
	store.process();
	CHECK(live->load() == doctest::Approx(4.0f));

	// 2 組にして 2 組目へ。2 組目はまだ初期値
	set(*processor, base + EnvPattern::patternsKey, 2.0f);
	set(*processor, base + EnvPattern::patternKey, 2.0f);
	store.process();

	const float initial = processor->apvts.getParameter(voices)->convertFrom0to1(processor->apvts.getParameter(voices)->getDefaultValue());

	CHECK(live->load() == doctest::Approx(initial));

	// つまみにも 2 組目の値が戻る
	store.update();
	CHECK(get(*processor, voices) == doctest::Approx(initial));
	CHECK(store.editingPattern(base) == 2);

	// 2 組目を変える
	set(*processor, voices, 6.0f);
	store.process();
	CHECK(live->load() == doctest::Approx(6.0f));

	// 1 組目へ戻すと、1 組目の値
	set(*processor, base + EnvPattern::patternKey, 1.0f);
	store.process();
	CHECK(live->load() == doctest::Approx(4.0f));

	store.update();
	CHECK(get(*processor, voices) == doctest::Approx(4.0f));
}

TEST_CASE("Pattern: PATTERN は PATTERNS で抑える")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& store = processor->patterns;

	const auto base = findBase(*processor, EnvPattern::unison);

	REQUIRE(base.isNotEmpty());

	const auto voices = base + "_VOICES";
	auto* live = PatternStore::raw(processor->apvts, voices);

	set(*processor, voices, 3.0f);
	store.process();

	// 組が 1 つのまま PATTERN だけ上げても、1 組目のまま
	set(*processor, base + EnvPattern::patternKey, 5.0f);
	store.process();

	CHECK(live->load() == doctest::Approx(3.0f));
	CHECK(EnvPattern::clampPattern(1.0f, 5.0f) == 1);
	CHECK(EnvPattern::clampPattern(3.0f, 5.0f) == 3);
}

TEST_CASE("Pattern: RESET TO CHANGE を入れていると、切り替えで始め直しの数が進む")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& store = processor->patterns;

	const auto base = findBase(*processor, EnvPattern::unison);

	REQUIRE(base.isNotEmpty());

	const auto* restart = PatternStore::restartOf(processor->apvts, base);

	REQUIRE(restart != nullptr);

	set(*processor, base + EnvPattern::patternsKey, 3.0f);
	store.process();

	const auto before = restart->load();

	// 切っていると進まない
	set(*processor, base + EnvPattern::patternKey, 2.0f);
	store.process();
	CHECK(restart->load() == before);

	// 入れていると進む
	set(*processor, base + EnvPattern::resetKey, 1.0f);
	set(*processor, base + EnvPattern::patternKey, 3.0f);
	store.process();
	CHECK(restart->load() == before + 1);

	// 切り替えなければ進まない
	store.process();
	CHECK(restart->load() == before + 1);
}

TEST_CASE("Pattern: 組の値は state へ書いて読み直すと戻る")
{
	juce::MemoryBlock block;
	juce::String base;

	{
		auto processor = std::make_unique<AudioPlugin2686V>();
		auto& store = processor->patterns;

		base = findBase(*processor, EnvPattern::unison);

		REQUIRE(base.isNotEmpty());

		const auto voices = base + "_VOICES";

		set(*processor, voices, 2.0f);
		set(*processor, base + EnvPattern::patternsKey, 3.0f);
		store.process();

		set(*processor, base + EnvPattern::patternKey, 3.0f);
		store.process();
		store.update();

		set(*processor, voices, 7.0f);
		store.process();

		processor->getStateInformation(block);
	}

	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& store = processor->patterns;

	processor->setStateInformation(block.getData(), (int)block.getSize());

	const auto voices = base + "_VOICES";
	auto* live = PatternStore::raw(processor->apvts, voices);

	// 選んでいた 3 組目から始まる
	store.process();
	CHECK(store.editingPattern(base) == 3);
	CHECK(get(*processor, voices) == doctest::Approx(7.0f));
	CHECK(live->load() == doctest::Approx(7.0f));

	// 1 組目も戻っている
	set(*processor, base + EnvPattern::patternKey, 1.0f);
	store.process();
	CHECK(live->load() == doctest::Approx(2.0f));
}

TEST_CASE("Pattern: 段の表は組ごとに別の表を持つ")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& store = processor->patterns;

	const auto base = findBase(*processor, EnvPattern::ssgSwEnv256);

	if (base.isEmpty()) {
		MESSAGE("SSG SW AMP ENV[256] を持たないので飛ばす");
		return;
	}

	// 表の鍵は区分の頭と同じ (code + CPK::ssgSwEnv256)
	auto* first = processor->stepTables.find(base);

	REQUIRE(first != nullptr);

	set(*processor, base + EnvPattern::patternsKey, 2.0f);
	store.process();
	store.update();

	auto* second = processor->stepTables.find(EnvPattern::tableKey(base, 2));

	REQUIRE(second != nullptr);
	CHECK(second != first);

	// 画面が引く表は、選んでいる組のもの
	CHECK(store.tableKeyFor(base) == base);

	set(*processor, base + EnvPattern::patternKey, 2.0f);
	store.process();
	store.update();

	CHECK(store.tableKeyFor(base) == EnvPattern::tableKey(base, 2));
}
