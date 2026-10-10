// ============================================================================
// EDIT 区分・TARGET の複写と消去・コピー履歴 (CPHIST)
// ============================================================================
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <functional>
#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiParamEdit.h"
#include "Shared/Gui/Components/Edit/TargetsDialog.h"
#include "Shared/Gui/Components/Edit/CopyHistory.h"
#include "./EditorTestAccess.h"

namespace
{
#if defined(GUI_TEST_REQUIRE_ALL)
	constexpr bool kRequireAll = true;
#else
	constexpr bool kRequireAll = false;
#endif

	struct Env
	{
		std::unique_ptr<AudioPlugin2686V> processor;
		std::unique_ptr<juce::AudioProcessorEditor> editor;

		Env()
		{
			processor = std::make_unique<AudioPlugin2686V>();
			editor.reset(processor->createEditor());

			EditorTestAccess::materializeAll(ed());
		}

		~Env()
		{
			editor.reset();
			processor.reset();
		}

		AudioPlugin2686VEditor& ed() { return *static_cast<AudioPlugin2686VEditor*>(editor.get()); }
		juce::TabbedComponent& tabs() { return EditorTestAccess::tabs(ed()); }

		juce::Component* openTab(int i)
		{
			tabs().setCurrentTabIndex(i);
			ed().resized();

			return tabs().getTabContentComponent(i);
		}
	};

	void collect(juce::Component* root, std::vector<juce::Component*>& out)
	{
		for (auto* c : root->getChildren())
		{
			out.push_back(c);
			collect(c, out);
		}
	}

	template <typename T>
	std::vector<T*> findAll(juce::Component* root, std::function<bool(T&)> pred = nullptr)
	{
		std::vector<juce::Component*> all;
		std::vector<T*> out;

		collect(root, all);

		for (auto* c : all)
		{
			if (auto* t = dynamic_cast<T*>(c)) {
				if (pred == nullptr || pred(*t)) out.push_back(t);
			}
		}

		return out;
	}

	// 画面に出ているか。テストでは窓を出さないので isShowing は使えない
	bool shown(juce::Component& c)
	{
		for (auto* p = &c; p != nullptr; p = p->getParentComponent())
		{
			// 画面そのものは、テストでは出していない
			if (dynamic_cast<juce::AudioProcessorEditor*>(p) != nullptr) return true;
			if (!p->isVisible()) return false;
		}

		return false;
	}

	GuiTextButton* findButton(juce::Component* root, const juce::String& text, bool visibleOnly = true)
	{
		auto found = findAll<GuiTextButton>(root, [&](GuiTextButton& b) {
			return b.getButtonText() == text && (!visibleOnly || shown(b));
		});

		return found.empty() ? nullptr : found.front();
	}

	// 押す。triggerClick は次にメッセージが回ったときに届くので、テストでは直に呼ぶ
	void click(juce::Button& b)
	{
		if (b.onClick) b.onClick();
	}

	// 区分を開いて並べ直す
	void openCategories(Env& env, juce::Component* root, const juce::String& title)
	{
		for (auto* cat : findAll<GuiCategoryLabel>(root, [&](GuiCategoryLabel& c) { return c.getText() == title; }))
			cat->setDetailVisible(true);

		env.ed().resized();
	}

	// 部品が、見出しの板 (区分の中身) に入っているか
	bool insideCategory(GuiCategoryLabel& cat, juce::Component& c)
	{
		if (c.getParentComponent() != cat.getParentComponent()) return false;

		return cat.backdropBounds().contains(c.getBounds().getCentre());
	}
}

TEST_CASE("EDIT: チャンネルのタブには EDIT 区分があり、CUT / COPY / PASTE / CLEAR と Level -> All Ch が入っている")
{
	Env env;

	int checked = 0;

	for (int i = 0; i < env.tabs().getNumTabs(); ++i)
	{
		auto* root = env.openTab(i);

		if (root == nullptr) continue;

		openCategories(env, root, "EDIT");

		auto level = findButton(root, "Level -> All Ch", false);

		// Level -> All Ch を持つのはチャンネルのタブだけ
		if (level == nullptr) continue;

		INFO(env.tabs().getTabNames()[i].toStdString());

		auto cats = findAll<GuiCategoryLabel>(root, [&](GuiCategoryLabel& c) {
			return c.getText() == "EDIT" && c.getParentComponent() == level->getParentComponent();
		});

		REQUIRE(cats.size() == 1);

		auto* cat = cats.front();

		for (auto* name : { "CUT", "COPY", "PASTE", "CLEAR" })
		{
			auto* b = findButton(cat->getParentComponent(), name);

			INFO(name);
			REQUIRE(b != nullptr);
			CHECK(b->getWidth() > 0);
			CHECK(insideCategory(*cat, *b));
		}

		// UTILITY から移したもの
		CHECK(shown(*level));
		CHECK(insideCategory(*cat, *level));

		if (auto* dup = findButton(root, "Duplicate Op Params", false))
		{
			CHECK(shown(*dup));
			CHECK(insideCategory(*cat, *dup));
		}

		// 古い名前は残っていない
		CHECK(findButton(root, "Copy Op Params", false) == nullptr);

		++checked;
	}

	if (kRequireAll) CHECK(checked >= 14);
	MESSAGE("EDIT: " << checked << " タブ");
}

TEST_CASE("EDIT: オペレーター / パッドの EDIT 区分が、TARGET の単位を写す")
{
	Env env;

	int checked = 0;

	for (int i = 0; i < env.tabs().getNumTabs(); ++i)
	{
		auto* root = env.openTab(i);

		if (root == nullptr) continue;

		openCategories(env, root, "EDIT");

		// オペレーター / パッドの EDIT は、グローバルの EDIT と別の板にある
		auto level = findButton(root, "Level -> All Ch", false);

		if (level == nullptr) continue;

		auto cats = findAll<GuiCategoryLabel>(root, [&](GuiCategoryLabel& c) {
			return c.getText() == "EDIT" && c.getParentComponent() != level->getParentComponent();
		});

		if (cats.empty()) continue;

		INFO(env.tabs().getTabNames()[i].toStdString());

		auto* cat = cats.front();
		auto* copy = findButton(cat->getParentComponent(), "COPY");

		REQUIRE(copy != nullptr);
		CHECK(insideCategory(*cat, *copy));

		const int before = (int)ParamEdit::Clipboard::get().history().size();

		click(*copy);

		const auto& h = ParamEdit::Clipboard::get().history();

		REQUIRE((int)h.size() > before);
		CHECK(h.front().scope == ParamEdit::Scope::Unit);

		const bool unitName = h.front().unit.startsWith("OP") || h.front().unit.startsWith("PAD");

		CHECK(unitName);
		CHECK_FALSE(h.front().values.empty());

		++checked;
	}

	// OPNA / OPN / OPL / OPL3 / OPM / OPZX7 / RHYTHM
	if (kRequireAll) CHECK(checked == 7);
	MESSAGE("オペレーター / パッドの EDIT: " << checked << " タブ");
}

TEST_CASE("Duplicate Targets: ダイアログは WAVE MEMORY / FORM の枠を隠さず、選んだ TARGET へ写す")
{
	Env env;

	int checked = 0;

	for (int i = 0; i < env.tabs().getNumTabs(); ++i)
	{
		auto* root = env.openTab(i);

		if (root == nullptr) continue;

		openCategories(env, root, "EDIT");

		auto* open = findButton(root, "Duplicate Targets");

		if (open == nullptr) continue;

		INFO(env.tabs().getTabNames()[i].toStdString());

		click(*open);

		auto dialogs = findAll<GuiTargetsDialog>(root);

		REQUIRE(dialogs.size() == 1);

		auto* dialog = dialogs.front();

		// 波形や音声の枠 (WAVE MEMORY / FORM を持つ枠) と重ならない。
		// メインの枠 (EDIT がある枠) は重なってもよい。
		for (auto* group : findAll<GuiScrollGroup>(root))
		{
			if (group->getParentComponent() != dialog->getParentComponent()) continue;
			if (group->isParentOf(open)) continue;

			INFO(group->getText().toStdString() << " " << group->getBounds().toString().toStdString()
				<< " / dialog " << dialog->getBounds().toString().toStdString());
			CHECK_FALSE(group->getBounds().intersects(dialog->getBounds()));
		}

		CHECK(dialog->getParentComponent()->getLocalBounds().contains(dialog->getBounds()));
		CHECK(dialog->getHeight() >= dialog->preferredHeight(dialog->getWidth()));

		dialog->fromSlider().setValue(0.0, juce::sendNotificationSync);
		click(dialog->allOffButton());
		dialog->targetToggle(3).setToggleState(true, juce::dontSendNotification);
		dialog->targetToggle(5).setToggleState(true, juce::dontSendNotification);

		// 写し元と同じ番号は選べない
		CHECK_FALSE(dialog->targetToggle(0).isEnabled());

		click(dialog->runButton());

		// 閉じている
		CHECK(findAll<GuiTargetsDialog>(root).empty());

		++checked;
	}

	// WT+ / ADPCM+
	if (kRequireAll) CHECK(checked == 2);
	MESSAGE("Duplicate Targets: " << checked << " タブ");
}

TEST_CASE("Duplicate Targets / Clear Targets: 値を写し、消す")
{
	Env env;

	for (const auto& [channel, key] : { std::pair<juce::String, juce::String>{ "WTPLUS", "SPEED" }, { "ADPCMP", "SPEED" } })
	{
		auto* p0 = env.processor->apvts.getParameter(channel + "_SLOT0_" + key);

		if (p0 == nullptr) continue;

		INFO(channel.toStdString());

		auto* p3 = env.processor->apvts.getParameter(channel + "_SLOT3_" + key);
		auto* p5 = env.processor->apvts.getParameter(channel + "_SLOT5_" + key);

		REQUIRE(p3 != nullptr);
		REQUIRE(p5 != nullptr);

		p0->setValueNotifyingHost(0.8f);

		// どれかのタブの部品から窓口をもらう
		auto cats = findAll<GuiCategoryLabel>(env.editor.get());
		REQUIRE_FALSE(cats.empty());

		const auto& ctx = cats.front()->context();

		// パラメータを写さないときは変わらない
		const float before = p3->getValue();

		ParamEdit::duplicateUnitTo(ctx, channel, "SLOT", 0, { 3, 5 }, false);
		CHECK(p3->getValue() == doctest::Approx(before));

		ParamEdit::duplicateUnitTo(ctx, channel, "SLOT", 0, { 3, 5 }, true);
		CHECK(p3->getValue() == doctest::Approx(0.8f).epsilon(0.01));
		CHECK(p5->getValue() == doctest::Approx(0.8f).epsilon(0.01));

		ParamEdit::clearUnits(ctx, channel, "SLOT", { 3 });
		CHECK(p3->getValue() == doctest::Approx(p3->getDefaultValue()));
		CHECK(p5->getValue() == doctest::Approx(0.8f).epsilon(0.01));
	}
}

TEST_CASE("CPHIST: 写したものが一覧に出て、絞り込みと再コピーができる")
{
	Env env;

	auto& clip = ParamEdit::Clipboard::get();

	ParamEdit::Snapshot a;
	a.scope = ParamEdit::Scope::Channel;
	a.tab = "TAB-A";
	a.values["LEVEL"] = 0.5f;

	ParamEdit::Snapshot b;
	b.scope = ParamEdit::Scope::Category;
	b.tab = "TAB-B";
	b.unit = "OP2";
	b.category = "AMP ENV";
	b.values["R_AR"] = 3.0f;

	clip.push(a);
	clip.push(b);

	auto btn = findAll<juce::TextButton>(env.editor.get(), [](juce::TextButton& t) { return t.getButtonText() == "CPHIST"; });

	REQUIRE(btn.size() == 1);

	click(*btn.front());

	auto lists = findAll<GuiCopyHistory>(env.editor.get());

	REQUIRE(lists.size() == 1);

	auto* hist = lists.front();

	CHECK(hist->getBounds() == env.editor->getLocalBounds());
	CHECK((int)hist->rows().size() >= 2);
	CHECK(hist->rows().front().tab == "TAB-B");

	// タブで絞る
	auto& tabs = hist->tabFilter();

	for (int i = 0; i < tabs.getNumItems(); ++i)
	{
		if (tabs.getItemText(i) == "TAB-A") tabs.setSelectedItemIndex(i, juce::sendNotificationSync);
	}

	REQUIRE(hist->rows().size() == 1);
	CHECK(hist->rows().front().tab == "TAB-A");

	// 再コピー
	hist->recopy(0);

	REQUIRE(clip.current().has_value());
	CHECK(clip.current()->tab == "TAB-A");

	// 区分で絞る
	tabs.setSelectedItemIndex(0, juce::sendNotificationSync);

	auto& cats = hist->categoryFilter();

	for (int i = 0; i < cats.getNumItems(); ++i)
	{
		if (cats.getItemText(i) == "AMP ENV") cats.setSelectedItemIndex(i, juce::sendNotificationSync);
	}

	REQUIRE_FALSE(hist->rows().empty());

	for (const auto& r : hist->rows()) CHECK(r.category == "AMP ENV");

	// 単位で絞る (オペレーター)
	cats.setSelectedItemIndex(0, juce::sendNotificationSync);

	auto& units = hist->unitFilter();

	units.setSelectedItemIndex(2, juce::sendNotificationSync);

	for (const auto& r : hist->rows()) CHECK(r.unit.startsWith("OP"));

	hist->close();
}
