// ============================================================================
// 項目の欠けた状態を読んで保存し、読み直す
// ============================================================================
// 古い版のプリセットや、項目を絞って配るプリセットは、すべてのパラメータを
// 持っていない。それを読むと JUCE は欠けたパラメータへ値の無い子を作り、
// 保存でそのまま値の空な項目として書かれていた。読み直すと 0 として入り、
// STEPS が 10 から 1 になるなど、初期値から外れていた。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <vector>

#include "Core/Processor/PluginProcessor.h"

namespace
{
	// 空の値 (= 0) で読むと初期値から外れるもの。不具合の見えるものだけを選ぶ。
	std::vector<juce::RangedAudioParameter*> pickParams(AudioPlugin2686V& processor, size_t count)
	{
		std::vector<juce::RangedAudioParameter*> picked;

		for (auto* param : processor.getParameters())
		{
			auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(param);

			if (ranged == nullptr) continue;

			if (std::abs(ranged->convertTo0to1(0.0f) - ranged->getDefaultValue()) < 1.0e-4f) continue;

			picked.push_back(ranged);

			if (picked.size() >= count) break;
		}

		return picked;
	}

	// 選んだパラメータを状態から外して読ませる
	void loadStateWithout(AudioPlugin2686V& processor, const std::vector<juce::RangedAudioParameter*>& missing)
	{
		auto xml = processor.apvts.copyState().createXml();

		REQUIRE(xml != nullptr);

		for (auto* param : missing)
		{
			auto* child = xml->getChildByAttribute("id", param->paramID);

			REQUIRE_MESSAGE(child != nullptr, param->paramID.toStdString());

			xml->removeChildElement(child, true);
		}

		juce::MemoryBlock block;
		juce::AudioProcessor::copyXmlToBinary(*xml, block);

		processor.setStateInformation(block.getData(), (int)block.getSize());
	}

	// 読み直す前に、不具合のときに入っていた値 (0) へずらしておく
	void moveToZero(const std::vector<juce::RangedAudioParameter*>& params)
	{
		for (auto* param : params) param->setValueNotifyingHost(param->convertTo0to1(0.0f));
	}

	void checkDefaults(const std::vector<juce::RangedAudioParameter*>& params)
	{
		for (auto* param : params)
		{
			INFO(param->paramID.toStdString());
			CHECK(param->getValue() == doctest::Approx(param->getDefaultValue()).epsilon(1.0e-4));
		}
	}
}

TEST_CASE("PresetState: a state missing params saves them and reloads at their defaults")
{
	auto processor = std::make_unique<AudioPlugin2686V>();

	auto missing = pickParams(*processor, 32);

	REQUIRE(!missing.empty());

	loadStateWithout(*processor, missing);

	SUBCASE("DAW state")
	{
		juce::MemoryBlock saved;
		processor->getStateInformation(saved);

		auto xml = juce::AudioProcessor::getXmlFromBinary(saved.getData(), (int)saved.getSize());

		REQUIRE(xml != nullptr);

		for (auto* param : missing)
		{
			auto* child = xml->getChildByAttribute("id", param->paramID);

			INFO(param->paramID.toStdString());
			REQUIRE(child != nullptr);
			CHECK(child->getStringAttribute("value").isNotEmpty());
		}

		moveToZero(missing);

		processor->setStateInformation(saved.getData(), (int)saved.getSize());

		checkDefaults(missing);
	}

	SUBCASE("Preset file")
	{
		juce::TemporaryFile temp(".preset");

		processor->savePreset(temp.getFile());

		REQUIRE(temp.getFile().existsAsFile());

		moveToZero(missing);

		processor->loadPreset(temp.getFile());

		checkDefaults(missing);
	}
}
