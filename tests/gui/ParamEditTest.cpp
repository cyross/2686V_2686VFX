// ============================================================================
// パラメータの切り取り・コピー・貼り付け・初期化 (ParamEdit)
// ============================================================================
// 値はパラメータ ID からチャンネルとオペレーター・パッド・枠の番号を外した
// 名前で持ち、貼り先では同じ名前のものだけを入れる。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiParamEdit.h"
#include "./EditorTestAccess.h"

namespace
{
	struct Env
	{
		std::unique_ptr<AudioPlugin2686V> processor;
		std::unique_ptr<juce::AudioProcessorEditor> editor;

		Env()
		{
			processor = std::make_unique<AudioPlugin2686V>();
			editor.reset(processor->createEditor());

			EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));
		}

		~Env()
		{
			editor.reset();
			processor.reset();
		}

		juce::TabbedComponent* tabs()
		{
			std::function<juce::TabbedComponent* (juce::Component*)> find = [&](juce::Component* c) -> juce::TabbedComponent* {
				if (auto* t = dynamic_cast<juce::TabbedComponent*>(c)) return t;

				for (auto* child : c->getChildren())
				{
					if (auto* t = find(child)) return t;
				}

				return nullptr;
			};

			return find(editor.get());
		}

		// 名前のタブを開いて、その中身を返す
		juce::Component* openTab(const juce::String& name)
		{
			auto* t = tabs();

			if (t == nullptr) return nullptr;

			for (int i = 0; i < t->getNumTabs(); ++i)
			{
				if (t->getTabNames()[i] != name) continue;

				t->setCurrentTabIndex(i);
				editor->resized();

				return t->getTabContentComponent(i);
			}

			return nullptr;
		}

		void set(const juce::String& id, float value)
		{
			auto* p = processor->apvts.getParameter(id);

			REQUIRE_MESSAGE(p != nullptr, id.toStdString());

			p->setValueNotifyingHost(p->convertTo0to1(value));
		}

		float get(const juce::String& id)
		{
			auto* p = processor->apvts.getParameter(id);

			REQUIRE_MESSAGE(p != nullptr, id.toStdString());

			return p->convertFrom0to1(p->getValue());
		}

		bool has(const juce::String& id) { return processor->apvts.getParameter(id) != nullptr; }
	};

	void collect(juce::Component* root, std::vector<juce::Component*>& out)
	{
		for (auto* c : root->getChildren())
		{
			out.push_back(c);
			collect(c, out);
		}
	}

	GuiCategoryLabel* findCategory(juce::Component* root, const juce::String& title)
	{
		std::vector<juce::Component*> all;

		collect(root, all);

		for (auto* c : all)
		{
			if (auto* cat = dynamic_cast<GuiCategoryLabel*>(c)) {
				if (cat->getText() == title) return cat;
			}
		}

		return nullptr;
	}
}

TEST_CASE("ParamEdit: ID からキーを作る")
{
	using ParamEdit::Scope;

	CHECK(ParamEdit::keyOf("OPM_LFO_FREQ", Scope::Channel) == "LFO_FREQ");
	CHECK(ParamEdit::keyOf("OPM_OP2_R_AR", Scope::Unit) == "R_AR");
	CHECK(ParamEdit::keyOf("OPM_OP2_R_AR", Scope::Category) == "R_AR");
	CHECK(ParamEdit::keyOf("WTPLUS_SLOT3_SPEED", Scope::Channel) == "SLOT3_SPEED");
	CHECK(ParamEdit::keyOf("WTPLUS_SLOT3_SPEED", Scope::Category) == "SPEED");

	// SSGSW11 のような番号付きの名前は単位ではない
	CHECK(ParamEdit::keyOf("OPM_SSGSW11_STEPS", Scope::Category) == "SSGSW11_STEPS");

	CHECK(ParamEdit::isUnitToken("OP0"));
	CHECK(ParamEdit::isUnitToken("PAD7"));
	CHECK(ParamEdit::isUnitToken("SLOT31"));
	CHECK_FALSE(ParamEdit::isUnitToken("OPZX7"));
	CHECK_FALSE(ParamEdit::isUnitToken("SSGSW11"));

	CHECK(ParamEdit::unitIndexOf("ADPCMP_SLOT3", "ADPCMP", "SLOT") == 3);
	CHECK(ParamEdit::unitIndexOf("ADPCMP", "ADPCMP", "SLOT") == -1);
	CHECK(ParamEdit::unitIndexOf("RHYTHM_PAD2", "ADPCMP", "SLOT") == -1);
}

TEST_CASE("ParamEdit: 履歴は新しいものが先頭で、選び直せる")
{
	auto& clip = ParamEdit::Clipboard::get();

	ParamEdit::Snapshot a;
	a.tab = "A";
	a.values["LEVEL"] = 0.25f;

	ParamEdit::Snapshot b;
	b.tab = "B";
	b.values["LEVEL"] = 0.75f;

	clip.push(a);
	clip.push(b);

	REQUIRE(clip.current().has_value());
	CHECK(clip.current()->tab == "B");
	CHECK(clip.history().front().tab == "B");

	const int serialA = clip.history()[1].serial;

	clip.select(serialA);

	CHECK(clip.current()->tab == "A");
}

TEST_CASE("ParamEdit: FX の効果の枠を右クリックで写す単位にできる")
{
	Env env;

	// FX はタブの中とは限らない (音源のプラグインでは画面の右に常に出ている)
	std::vector<juce::Component*> all;
	collect(env.editor.get(), all);

	const GuiContext ctx(*static_cast<GuiProcessorHost*>(env.processor.get()),
		*dynamic_cast<GuiEditorHost*>(env.editor.get()), env.processor->apvts);

	int found = 0;

	for (auto* c : all)
	{
		auto* group = dynamic_cast<GuiGroup*>(c);

		if (group == nullptr) continue;

		const auto target = ParamEdit::groupTarget(ctx, *group);

		const bool hasFilterType = std::find(target.ids.begin(), target.ids.end(), juce::String("FX_FIL_TYPE")) != target.ids.end();

		if (!hasFilterType) continue;

		// 隣の効果のパラメータは入らない
		for (const auto& id : target.ids) CHECK(id.startsWith("FX_FIL_"));

		CHECK(target.channel == "FX");
		CHECK(target.tab.isNotEmpty());

		++found;
	}

	CHECK(found == 1);
}

#if defined(GUI_TEST_REQUIRE_ALL)
TEST_CASE("ParamEdit: グローバル部分を、音源の違うチャンネルの同じ名前へ貼る")
{
	Env env;

	auto* opm = env.openTab("OPM");
	REQUIRE(opm != nullptr);

	env.set("OPM_LEVEL", 0.37f);
	env.set("OPM_OP1_R_AR", 7.0f);

	ParamEdit::Target from;
	from.scope = ParamEdit::Scope::Channel;
	from.channel = "OPM";

	auto* cat = findCategory(opm, "QUALITY");
	REQUIRE(cat != nullptr);

	const auto& ctx = cat->context();
	const auto snap = ParamEdit::capture(ctx, from);

	// グローバル部分にオペレーターは入らない
	CHECK(snap.values.count("LEVEL") == 1);
	CHECK(snap.values.count("OP1_R_AR") == 0);

	ParamEdit::Target to;
	to.scope = ParamEdit::Scope::Channel;
	to.channel = "OPN";

	const float before = env.get("OPN_OP1_R_AR");

	ParamEdit::paste(ctx, to, snap);

	CHECK(env.get("OPN_LEVEL") == doctest::Approx(0.37f).epsilon(0.001));

	// オペレーターには触らない
	CHECK(env.get("OPN_OP1_R_AR") == doctest::Approx(before));
}

TEST_CASE("ParamEdit: オペレーターのまとまりを写す (Duplicate Ops Params)")
{
	Env env;

	auto* opm = env.openTab("OPM");
	REQUIRE(opm != nullptr);

	env.set("OPM_OP0_R_AR", 3.0f);
	env.set("OPM_OP1_R_AR", 11.0f);
	env.set("OPM_OP0_MUL", 5.0f);

	auto* cat = findCategory(opm, "QUALITY");
	REQUIRE(cat != nullptr);

	ParamEdit::duplicateUnits(cat->context(), "OPM", "OP", 0, 2, 2);

	CHECK(env.get("OPM_OP2_R_AR") == doctest::Approx(3.0f));
	CHECK(env.get("OPM_OP3_R_AR") == doctest::Approx(11.0f));
	CHECK(env.get("OPM_OP2_MUL") == doctest::Approx(5.0f));

	// 写し元はそのまま
	CHECK(env.get("OPM_OP0_R_AR") == doctest::Approx(3.0f));
}

TEST_CASE("ParamEdit: CLEAR は初期値へ戻す")
{
	Env env;

	auto* opm = env.openTab("OPM");
	REQUIRE(opm != nullptr);

	auto* p = env.processor->apvts.getParameter("OPM_LEVEL");
	REQUIRE(p != nullptr);

	env.set("OPM_LEVEL", 0.2f);
	REQUIRE(p->getValue() != doctest::Approx(p->getDefaultValue()));

	auto* cat = findCategory(opm, "QUALITY");
	REQUIRE(cat != nullptr);

	ParamEdit::Target t;
	t.scope = ParamEdit::Scope::Channel;
	t.channel = "OPM";

	ParamEdit::clear(cat->context(), t);

	CHECK(p->getValue() == doctest::Approx(p->getDefaultValue()));
}

TEST_CASE("ParamEdit: 区分の中身は、閉じていても開いているときと同じに集まる")
{
	Env env;

	auto* opm = env.openTab("OPM");
	REQUIRE(opm != nullptr);

	auto* cat = findCategory(opm, "QUALITY");
	REQUIRE(cat != nullptr);

	cat->setDetailVisible(true);
	env.editor->resized();

	const auto open = ParamEdit::categoryTarget(*cat);

	cat->setDetailVisible(false);
	env.editor->resized();

	const auto closed = ParamEdit::categoryTarget(*cat);

	// 調べたあとは閉じたまま
	CHECK_FALSE(cat->isDetailVisible());

	CHECK_FALSE(open.ids.empty());
	CHECK(open.ids == closed.ids);
	CHECK(open.channel == "OPM");
	CHECK(open.tab == "OPM");

	const bool hasBit = std::find(open.ids.begin(), open.ids.end(), juce::String("OPM_BIT")) != open.ids.end();
	CHECK(hasBit);

	// ほかの区分のパラメータは入らない
	const bool hasLevel = std::find(open.ids.begin(), open.ids.end(), juce::String("OPM_LEVEL")) != open.ids.end();
	CHECK_FALSE(hasLevel);
}

TEST_CASE("ParamEdit: オペレーターの区分を、別のチャンネルのオペレーターへ貼る")
{
	Env env;

	auto* opm = env.openTab("OPM");
	REQUIRE(opm != nullptr);

	env.set("OPM_OP0_R_AR", 9.0f);

	ParamEdit::Target from;
	from.scope = ParamEdit::Scope::Unit;
	from.channel = "OPM";
	from.unit = "OP0";

	auto* cat = findCategory(opm, "QUALITY");
	REQUIRE(cat != nullptr);

	const auto snap = ParamEdit::capture(cat->context(), from);

	ParamEdit::Target to;
	to.scope = ParamEdit::Scope::Unit;
	to.channel = "OPM";
	to.unit = "OP3";

	ParamEdit::paste(cat->context(), to, snap);

	CHECK(env.get("OPM_OP3_R_AR") == doctest::Approx(9.0f));
}
#endif
