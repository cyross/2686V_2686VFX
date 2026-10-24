#pragma once

#include <map>
#include <vector>

#include <JuceHeader.h>

#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Core/Synth/StepTableIo.h"
#include "./IoStepEnvLegacy.h"

// ============================================================================
// 4.0.0〜4.2.x の TL ENV を、4.3.0 の PARAM ENV[256] (オペレーター) へ写す
// ============================================================================
// 4.3.0 で TL ENV は PARAM ENV の指し先 (TARGET) の 1 つ「TL」にまとまった。
// PARAM ENV は値を指し先のパラメータの 0〜1 で持つので、TL のレジスタ値を
// TL の最大値 (OPNA / OPN / OPM は 127、OPL / OPL3 / OPZX7 は 63) で割って写す。
//
//   <頭>_TLENV_ENABLE / STEPS / LOOP / LOOPTO / LOOPCNT / LOOPMODE / RR / ENDL_EN
//                                  → 同じ名前の <頭>_PRMENV_*
//   <頭>_TLENV_STL / LR / ENDL     → <頭>_PRMENV_STL / LR / ENDL (÷ TL の最大値)
//   表 <頭>_TLENV                  → 表 <頭>_PRMENV (L ÷ TL の最大値。R・ループ・SKIP はそのまま)
//                                  → <頭>_PRMENV_TARGET は TL (0)、KEEP は切 (TL は常に段を保つ)
//
// 写したあと、TL ENV のパラメータと表は state から外す。写した PARAM ENV は TL ENV と同じに鳴る。
namespace ParamEnvLegacy
{
	inline const juce::String oldBase = "_TLENV";
	inline const juce::String newBase = "_PRMENV";

	// TL のレジスタ値の最大値。頭の音源の名前で決まる
	inline float tlMaxOf(const juce::String& prefix)
	{
		const auto chip = prefix.upToFirstOccurrenceOf("_OP", false, false);

		return (chip == "OPNA" || chip == "OPN" || chip == "OPM") ? 127.0f : 63.0f;
	}

	// TL の値 (文字) を 0〜1 にする
	inline juce::String normalised(const juce::String& value, float tlMax)
	{
		return juce::String(juce::jlimit(0.0f, 1.0f, value.getFloatValue() / tlMax), 6);
	}

	// 写したものがあれば true
	inline bool convertState(juce::XmlElement& state, StepTableStore& tables)
	{
		using StepEnvLegacy::paramTag;
		using StepEnvLegacy::idAttr;
		using StepEnvLegacy::valueAttr;

		const auto oldMark = oldBase + "_";

		std::map<juce::String, juce::XmlElement*> byId;

		for (auto* p : state.getChildWithTagNameIterator(paramTag))
			byId[p->getStringAttribute(idAttr)] = p;

		// 書いてあるもの (4.3.0 以降の state) は上書きしない
		auto setParam = [&](const juce::String& id, const juce::String& value) {
			if (byId.count(id) > 0) return;

			auto* p = state.createNewChildElement(paramTag);

			p->setAttribute(idAttr, id);
			p->setAttribute(valueAttr, value);

			byId[id] = p;
		};

		std::vector<std::pair<juce::String, juce::String>> found;

		for (const auto& [id, p] : byId) {
			if (id.indexOf(oldMark) >= 0) found.emplace_back(id, p->getStringAttribute(valueAttr));
		}

		bool converted = false;
		juce::StringArray prefixes;

		for (const auto& [id, value] : found) {
			const int at = id.indexOf(oldMark);
			const auto prefix = id.substring(0, at);
			const auto field = id.substring(at + oldMark.length());
			const auto newPrefix = prefix + newBase + "_";
			const float tlMax = tlMaxOf(prefix);

			const bool level = field == "STL" || field == "LR" || field == "ENDL";

			setParam(newPrefix + field, level ? normalised(value, tlMax) : value);
			prefixes.addIfNotAlreadyThere(prefix);
			converted = true;
		}

		for (const auto& prefix : prefixes) {
			setParam(prefix + newBase + "_TARGET", "0");
			setParam(prefix + newBase + "_KEEP", "0");
		}

		// 段の表。読み込みで表は作り直されているので、新しい表へ文字から写して L を割る
		if (auto* node = state.getChildByName(StepTableIo::xmlTag)) {
			std::vector<juce::XmlElement*> old;

			for (auto* item : node->getChildWithTagNameIterator(StepTableIo::xmlItem)) {
				const auto key = item->getStringAttribute("key");

				if (!key.endsWith(oldBase)) continue;

				old.push_back(item);

				const auto prefix = key.dropLastCharacters(oldBase.length());
				auto* table = tables.find(prefix + newBase);

				if (table == nullptr) continue;

				// 4.3.0 以降の state に新しい表が書いてあれば、それを優先する
				bool written = false;

				for (auto* other : node->getChildWithTagNameIterator(StepTableIo::xmlItem)) {
					if (other->getStringAttribute("key") == prefix + newBase) written = true;
				}

				if (written) continue;

				StepTableIo::fromText(*table, item->getStringAttribute("values"));

				const float tlMax = tlMaxOf(prefix);

				for (int s = 1; s <= StepTable::size; ++s) table->setLevel(s, juce::jlimit(0.0f, 1.0f, table->level(s) / tlMax));

				converted = true;
			}

			for (auto* item : old) node->removeChildElement(item, true);
		}

		StepEnvLegacy::removeParams(state, oldMark);

		return converted;
	}
}
