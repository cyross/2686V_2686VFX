#pragma once

#include <map>
#include <vector>

#include <JuceHeader.h>

#include "Shared/Core/Synth/StepTable.h"

// ============================================================================
// 3.6.3 までの SSG SW AMP / PITCH ENV[11] を [256] へ写す
// ============================================================================
// 4.0.0 で [11] (10 段 + リリース) は [256] に置き換わった。[256] は段の値を
// パラメータでなく表 (StepTable) に持つので、古い state / プリセットに
// 書かれた [11] のパラメータを、読み込む前にここで写しておく。
//
//   STEPS / LOOP / LOOPTO / LOOPCNT / STL / ENDL / ENDL_EN / KEEP
//   ENABLE / BYPASS                      → 同じ名前の [256] のパラメータ
//   R1〜R10 / L1〜L10                    → 表の 1〜10 段
//   R11 / L11 (リリース)                 → RR / LR
//
// [256] の段の数は 1〜256 で、[11] の STEPS (1〜10) はそのまま使える。
// 写したあとの [256] は、[11] と同じように鳴る。
namespace StepEnvLegacy
{
	// APVTS の state の名前 (Io::StateKey と同じ)。この見出しは音の側のテストからも
	// 使うので、ParamFile.h (yaml / JSON の読み書き) には頼らない。
	inline const juce::String paramTag = "PARAM";
	inline const juce::String idAttr = "id";
	inline const juce::String valueAttr = "value";

	struct Names
	{
		juce::String oldBase;   // "_SSGSW11"
		juce::String newBase;   // "_SSGSW256"
	};

	inline const Names ssgSwEnv{ "_SSGSW11", "_SSGSW256" };
	inline const Names ssgSwPEnv{ "_SSGSWP11", "_SSGSWP256" };

	// APVTS の state (PARAM の並び) を書き換え、段の値を表へ入れる。
	// 古いパラメータは残しておく (読み込みで使われずに捨てられる)。
	// 写したものがあれば true。
	inline bool convertState(juce::XmlElement& state, StepTableStore& tables, const Names& names)
	{
		const auto oldMark = names.oldBase + "_";

		// 先に [256] のパラメータがどれだけあるかを見ておく。書いてあるものは上書きしない。
		std::map<juce::String, juce::XmlElement*> byId;

		for (auto* p : state.getChildWithTagNameIterator(paramTag))
			byId[p->getStringAttribute(idAttr)] = p;

		bool converted = false;

		auto setParam = [&](const juce::String& id, const juce::String& value) {
			if (byId.count(id) > 0) return;

			auto* p = state.createNewChildElement(paramTag);

			p->setAttribute(idAttr, id);
			p->setAttribute(valueAttr, value);

			byId[id] = p;
		};

		std::vector<std::pair<juce::String, juce::String>> found;

		for (const auto& [id, p] : byId) {
			const int at = id.indexOf(oldMark);

			if (at < 0) continue;

			found.emplace_back(id, p->getStringAttribute(valueAttr));
		}

		for (const auto& [id, value] : found) {
			const int at = id.indexOf(oldMark);
			const auto prefix = id.substring(0, at);
			const auto field = id.substring(at + oldMark.length());
			const auto newPrefix = prefix + names.newBase + "_";

			auto* table = tables.find(prefix + names.newBase);

			// R1〜R11 / L1〜L11
			if ((field.startsWithChar('R') || field.startsWithChar('L')) && field.substring(1).containsOnly("0123456789") && field.length() > 1) {
				const int step = field.substring(1).getIntValue();
				const bool rate = field.startsWithChar('R');

				if (step == 11) {
					setParam(newPrefix + (rate ? "RR" : "LR"), value);
				}
				else if (table != nullptr && step >= 1 && step <= 10) {
					if (rate) table->setRate(step, value.getFloatValue());
					else table->setLevel(step, value.getFloatValue());
				}

				converted = true;
				continue;
			}

			setParam(newPrefix + field, value);
			converted = true;
		}

		return converted;
	}
}
