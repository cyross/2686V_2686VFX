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
	// id に mark を含む PARAM を外す
	inline void removeParams(juce::XmlElement& state, const juce::String& mark)
	{
		std::vector<juce::XmlElement*> old;

		for (auto* p : state.getChildWithTagNameIterator("PARAM"))
			if (p->getStringAttribute("id").contains(mark)) old.push_back(p);

		for (auto* p : old) state.removeChildElement(p, true);
	}

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
	// 写したあと、古いパラメータは state から外す。APVTS は知らない PARAM を
	// 捨てずに持ち続けるので、残すと次に読み込むたびに写し直して、[256] で
	// 直した表を古い値で上書きしてしまう。
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

		removeParams(state, oldMark);

		return converted;
	}

	// ========================================================================
	// 3.6.3 までの 6 段の SSG SW AMP ENV (_SSGSW_) を [256] へ写す
	// ========================================================================
	// 6 段は 5 段 + リリース。[11] (いまの [256]) と掛け合わせて鳴っていたが、
	// [256] 1 つではその掛け合わせを表せない。そこで
	//
	//   6 段が切れている         → 写さない (鳴りは変わらない)
	//   6 段だけが使われている   → [256] へ写す (同じように鳴る)
	//   両方が使われている       → [256] を残し、6 段は捨てる
	//
	// とする。写すときは
	//
	//   ENABLE / BYPASS / STEPS / LOOP / LOOPTO / LOOPCNT / STL → 同じ名前の [256]
	//   R1〜R5 / L1〜L5                                       → 表の 1〜5 段
	//   R6 / L6 (リリース)                                    → RR / LR
	//
	// KEEP と ENDL は 6 段に無かったので切る。[11] の写しより後に呼ぶこと。
	inline const juce::String sixStepBase = "_SSGSW";

	// 6 段を捨てたところ (両方が使われていた) の頭の名前を返す。
	inline juce::StringArray convertSixStep(juce::XmlElement& state, StepTableStore& tables, const juce::String& newBase = ssgSwEnv.newBase)
	{
		const auto oldMark = sixStepBase + "_";
		const auto newMark = newBase + "_";

		std::map<juce::String, juce::XmlElement*> byId;

		for (auto* p : state.getChildWithTagNameIterator(paramTag))
			byId[p->getStringAttribute(idAttr)] = p;

		// 頭 (OPNA_OP1 など) ごとに、6 段の値を集める
		std::map<juce::String, std::map<juce::String, juce::String>> sixByPrefix;

		for (const auto& [id, p] : byId) {
			const int at = id.indexOf(oldMark);

			if (at < 0) continue;

			sixByPrefix[id.substring(0, at)][id.substring(at + oldMark.length())] = p->getStringAttribute(valueAttr);
		}

		auto valueOf = [&](const juce::String& id, const juce::String& fallback) {
			auto it = byId.find(id);

			return it != byId.end() ? it->second->getStringAttribute(valueAttr) : fallback;
		};

		auto setParam = [&](const juce::String& id, const juce::String& value) {
			auto it = byId.find(id);
			auto* p = it != byId.end() ? it->second : state.createNewChildElement(paramTag);

			p->setAttribute(idAttr, id);
			p->setAttribute(valueAttr, value);

			byId[id] = p;
		};

		auto isOn = [](const juce::String& v) { return v.getFloatValue() >= 0.5f; };

		juce::StringArray dropped;

		for (const auto& [prefix, six] : sixByPrefix) {
			auto field = [&](const juce::String& name, const juce::String& fallback) {
				auto it = six.find(name);

				return it != six.end() ? it->second : fallback;
			};

			// 入切だけで中身の無い頭は写すものが無い
			if (six.count("STEPS") == 0) continue;

			// オペレータは ENABLE、ほかは BYPASS で入切する
			const bool hasEnable = six.count("ENABLE") > 0;
			const bool sixOn = hasEnable ? isOn(field("ENABLE", "0")) : !isOn(field("BYPASS", "1"));

			if (!sixOn) continue;

			const auto newPrefix = prefix + newMark;
			const bool newOn = hasEnable ? isOn(valueOf(newPrefix + "ENABLE", "0")) : !isOn(valueOf(newPrefix + "BYPASS", "1"));

			if (newOn) {
				dropped.add(prefix);
				continue;
			}

			if (hasEnable) setParam(newPrefix + "ENABLE", "1");
			else setParam(newPrefix + "BYPASS", "0");

			for (const auto* name : { "STEPS", "LOOP", "LOOPTO", "LOOPCNT", "STL" }) {
				if (six.count(name) > 0) setParam(newPrefix + name, field(name, "0"));
			}

			if (six.count("R6") > 0) setParam(newPrefix + "RR", field("R6", "0"));
			if (six.count("L6") > 0) setParam(newPrefix + "LR", field("L6", "0"));

			setParam(newPrefix + "KEEP", "0");
			setParam(newPrefix + "ENDL_EN", "0");

			if (auto* table = tables.find(prefix + newBase)) {
				table->reset();

				for (int s = 1; s <= 5; ++s) {
					const juce::String no(s);

					if (six.count("R" + no) > 0) table->setRate(s, field("R" + no, "0").getFloatValue());
					if (six.count("L" + no) > 0) table->setLevel(s, field("L" + no, "0").getFloatValue());
				}
			}
		}

		removeParams(state, oldMark);

		return dropped;
	}
}
