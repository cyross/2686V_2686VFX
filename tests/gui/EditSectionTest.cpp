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
#include "Shared/Core/Gui/GuiColor.h"
#include "Shared/Gui/Components/Edit/Edit.h"
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

TEST_CASE("Duplicate Ops Params: SHIFT / UNSHIFT は 1 オペレーターずつずらし、はみ出すと反対の端へ回る")
{
	using G = GuiComponentEdit::OpGroup;

	const auto zx = GuiComponentEdit::opzx7Groups();
	const auto four = GuiComponentEdit::fourOpGroups();

	auto indexOf = [](const std::vector<G>& groups, int first, int size) {
		for (int i = 0; i < (int)groups.size(); ++i)
			if (groups[(size_t)i].first == first && groups[(size_t)i].size == size) return i;
		return -1;
	};

	auto shifted = [&](const std::vector<G>& groups, int first, int size, int dir) {
		return groups[(size_t)GuiComponentEdit::shiftedGroup(groups, indexOf(groups, first, size), dir)].name();
	};

	// 8 オペレーター: [6,7,8] → [1,2,3]、[1,2,3] → [6,7,8]
	CHECK(shifted(zx, 5, 3, +1) == "[1,2,3]");
	CHECK(shifted(zx, 0, 3, -1) == "[6,7,8]");
	CHECK(shifted(zx, 0, 3, +1) == "[2,3,4]");
	CHECK(shifted(zx, 2, 2, -1) == "[2,3]");
	CHECK(shifted(zx, 6, 2, +1) == "[1,2]");

	// 4 つは [1-4] と [5-8] しか無いので、その 2 つを行き来する
	CHECK(shifted(zx, 0, 4, +1) == "[5,6,7,8]");
	CHECK(shifted(zx, 4, 4, +1) == "[1,2,3,4]");
	CHECK(shifted(zx, 0, 4, -1) == "[5,6,7,8]");

	// 4 オペレーター音源は [1,2] と [3,4]
	CHECK(shifted(four, 0, 2, +1) == "[3,4]");
	CHECK(shifted(four, 2, 2, +1) == "[1,2]");
	CHECK(shifted(four, 0, 2, -1) == "[3,4]");
}

TEST_CASE("Duplicate Ops Params: SWAP で FROM と TO を入れ替え、SHIFT は FROM / TO を別々に動かす")
{
	Env env;

	int checked = 0;

	for (int i = 0; i < env.tabs().getNumTabs(); ++i)
	{
		auto* root = env.openTab(i);

		if (root == nullptr) continue;

		openCategories(env, root, "EDIT");

		auto* swap = findButton(root, "SWAP");

		if (swap == nullptr) continue;

		INFO(env.tabs().getTabNames()[i].toStdString());

		auto* parent = swap->getParentComponent();

		auto combos = findAll<GuiComboBox>(parent, [](GuiComboBox& c) {
			return c.getNumItems() > 0 && c.getItemText(0).startsWith("[");
		});

		REQUIRE(combos.size() >= 2);

		auto& from = *combos[0];
		auto& to = *combos[1];

		auto shifts = findAll<GuiTextButton>(parent, [](GuiTextButton& b) { return b.getButtonText() == "SHIFT" && shown(b); });
		auto unshifts = findAll<GuiTextButton>(parent, [](GuiTextButton& b) { return b.getButtonText() == "UNSHIFT" && shown(b); });

		REQUIRE(shifts.size() == 2);
		REQUIRE(unshifts.size() == 2);

		// SWAP は FROM / TO の下、SHIFT はその下に FROM・TO の順
		CHECK(swap->getY() > to.getY());
		CHECK(shifts[0]->getY() > swap->getY());
		CHECK(shifts[1]->getY() > shifts[0]->getY());

		const auto fromText = from.getText();
		const auto toText = to.getText();

		REQUIRE(fromText != toText);

		click(*swap);

		CHECK(from.getText() == toText);
		CHECK(to.getText() == fromText);

		// FROM だけが動く
		const auto toBefore = to.getText();
		const auto fromBefore = from.getText();

		click(*shifts[0]);
		CHECK(from.getText() != fromBefore);
		CHECK(to.getText() == toBefore);

		click(*unshifts[0]);
		CHECK(from.getText() == fromBefore);

		// TO だけが動く
		click(*shifts[1]);
		CHECK(to.getText() != toBefore);
		CHECK(from.getText() == fromBefore);

		click(*unshifts[1]);
		CHECK(to.getText() == toBefore);

		++checked;
	}

	// OPNA / OPN / OPL3 / OPM / OPZX7
	if (kRequireAll) CHECK(checked == 5);
	MESSAGE("SWAP / SHIFT: " << checked << " タブ");
}

TEST_CASE("Duplicate Targets / Clear Targets: もう一度押すと閉じ、ESC でも閉じる。クリアは赤")
{
	Env env;

	int checked = 0;

	for (int i = 0; i < env.tabs().getNumTabs(); ++i)
	{
		auto* root = env.openTab(i);

		if (root == nullptr) continue;

		openCategories(env, root, "EDIT");

		auto* dup = findButton(root, "Duplicate Targets");
		auto* clr = findButton(root, "Clear Targets");

		if (dup == nullptr || clr == nullptr) continue;

		INFO(env.tabs().getTabNames()[i].toStdString());

		auto openDialog = [&]() -> GuiTargetsDialog* {
			auto d = findAll<GuiTargetsDialog>(root);
			return d.empty() ? nullptr : d.front();
		};

		// 開いて選び、もう一度押すと閉じる
		click(*dup);
		REQUIRE(openDialog() != nullptr);
		openDialog()->targetToggle(3).setToggleState(true, juce::dontSendNotification);

		click(*dup);
		CHECK(openDialog() == nullptr);

		// 同じ場所へ開き直しても、トグルが並んでいる (大きさ 0 で消えない)
		for (int round = 0; round < 3; ++round)
		{
			click(*dup);
			REQUIRE(openDialog() != nullptr);

			for (int k = 0; k < 4; ++k)
				CHECK_FALSE(openDialog()->targetToggle(k).getBounds().isEmpty());

			click(*dup);
			CHECK(openDialog() == nullptr);
		}

		// 複写を開いているときにクリアを押すと、クリアに切り替わる
		click(*dup);
		click(*clr);
		REQUIRE(openDialog() != nullptr);
		CHECK(openDialog()->runButton().findColour(juce::TextButton::buttonColourId) == GuiColor::Edit::ClearBg.get());

		click(*clr);
		CHECK(openDialog() == nullptr);

		// ESC
		click(*clr);
		REQUIRE(openDialog() != nullptr);
		CHECK(openDialog()->keyPressed(juce::KeyPress(juce::KeyPress::escapeKey)));
		CHECK(openDialog() == nullptr);

		++checked;
	}

	if (kRequireAll) CHECK(checked == 2);
	MESSAGE("ダイアログの開け閉め: " << checked << " タブ");
}

TEST_CASE("RHYTHM: パッドの EDIT は OPTIONAL の下、Duplicate Pad Params はグローバルの EDIT にあり、選んだパッドへ写す")
{
	Env env;

	auto* p0 = env.processor->apvts.getParameter("RHYTHM_PAD0_SPEED");

	if (p0 == nullptr) return;

	int checked = 0;

	for (int i = 0; i < env.tabs().getNumTabs(); ++i)
	{
		auto* root = env.openTab(i);

		if (root == nullptr) continue;

		openCategories(env, root, "EDIT");
		openCategories(env, root, "OPTIONAL");

		auto* dup = findButton(root, "Duplicate Pad Params");

		if (dup == nullptr) continue;

		auto* parent = dup->getParentComponent();

		// Duplicate Pad Params はグローバルの EDIT (Level -> All Ch と同じ板) にある
		auto* level = findButton(root, "Level -> All Ch");

		REQUIRE(level != nullptr);
		CHECK(level->getParentComponent() == parent);

		// パッドの EDIT は、OPTIONAL と同じ列のその下
		auto optional = findAll<GuiCategoryLabel>(root, [](GuiCategoryLabel& c) { return c.getText() == "OPTIONAL"; });

		REQUIRE(optional.size() == 1);

		auto padEdit = findAll<GuiCategoryLabel>(optional.front()->getParentComponent(), [](GuiCategoryLabel& c) { return c.getText() == "EDIT"; });

		REQUIRE(padEdit.size() == 1);
		CHECK(padEdit.front()->getY() > optional.front()->getY());

		// UTILITY に古いものは残っていない
		CHECK(findAll<GuiTextButton>(root, [](GuiTextButton& b) { return b.getButtonText() == "Duplicate Pad Params"; }).size() == 1);

		auto toggles = findAll<GuiToggleButton>(parent, [](GuiToggleButton& t) { return t.getButtonText().containsOnly("0123456789") && shown(t); });
		auto withFile = findAll<GuiToggleButton>(parent, [](GuiToggleButton& t) { return t.getButtonText() == "WITH FILE"; });

		// パッドは 8 つ (86V は 6 つ)
		REQUIRE(toggles.size() >= 6);
		REQUIRE(withFile.size() == 1);
		CHECK_FALSE(withFile.front()->getToggleState());

		// FROM は 1 (PAD0)。写し元は選べず、写し先を選ぶまで押せない
		CHECK_FALSE(toggles[0]->isEnabled());
		CHECK_FALSE(dup->isEnabled());

		auto* p3 = env.processor->apvts.getParameter("RHYTHM_PAD3_SPEED");
		auto* p5 = env.processor->apvts.getParameter("RHYTHM_PAD5_SPEED");
		auto* p4 = env.processor->apvts.getParameter("RHYTHM_PAD4_SPEED");

		p0->setValueNotifyingHost(0.8f);
		const float before4 = p4->getValue();

		toggles[3]->setToggleState(true, juce::dontSendNotification);
		toggles[5]->setToggleState(true, juce::dontSendNotification);
		if (toggles[5]->onClick) toggles[5]->onClick();

		CHECK(dup->isEnabled());

		click(*dup);

		CHECK(p3->getValue() == doctest::Approx(0.8f).epsilon(0.01));
		CHECK(p5->getValue() == doctest::Approx(0.8f).epsilon(0.01));
		CHECK(p4->getValue() == doctest::Approx(before4));

		++checked;
	}

	if (kRequireAll) CHECK(checked == 1);
	MESSAGE("Duplicate Pad Params: " << checked << " タブ");
}

TEST_CASE("RHYTHM: With Note を切っているあいだは、PASTE / CUT / CLEAR で NOTE に触らない")
{
	Env env;

	auto* n0 = env.processor->apvts.getParameter("RHYTHM_PAD0_NOTE");

	if (n0 == nullptr) return;

	auto* n2 = env.processor->apvts.getParameter("RHYTHM_PAD2_NOTE");
	auto* s0 = env.processor->apvts.getParameter("RHYTHM_PAD0_SPEED");
	auto* s2 = env.processor->apvts.getParameter("RHYTHM_PAD2_SPEED");

	REQUIRE(n2 != nullptr);

	juce::ToggleButton* toggle = nullptr;

	for (int i = 0; i < env.tabs().getNumTabs() && toggle == nullptr; ++i)
	{
		auto* root = env.openTab(i);

		if (root == nullptr) continue;

		openCategories(env, root, "EDIT");

		auto found = findAll<GuiToggleButton>(root, [](GuiToggleButton& t) { return t.getButtonText() == "With Note"; });

		if (!found.empty()) toggle = found.front();
	}

	REQUIRE(toggle != nullptr);

	// 初めは切っている
	CHECK_FALSE(toggle->getToggleState());

	auto cats = findAll<GuiCategoryLabel>(env.editor.get());
	REQUIRE_FALSE(cats.empty());

	const auto& ctx = cats.front()->context();

	n0->setValueNotifyingHost(0.2f);
	s0->setValueNotifyingHost(0.7f);

	const float noteBefore = n2->getValue();

	// 複写 (Duplicate Pad Params と同じ道)
	ParamEdit::duplicateUnitTo(ctx, "RHYTHM", "PAD", 0, { 2 }, true, false);

	CHECK(s2->getValue() == doctest::Approx(0.7f).epsilon(0.01));
	CHECK(n2->getValue() == doctest::Approx(noteBefore));

	// パッド単位の COPY / PASTE
	ParamEdit::Target from, to;
	from.scope = to.scope = ParamEdit::Scope::Unit;
	from.channel = to.channel = "RHYTHM";
	from.unit = "PAD0";
	to.unit = "PAD2";

	ParamEdit::paste(ctx, to, ParamEdit::capture(ctx, from));
	CHECK(n2->getValue() == doctest::Approx(noteBefore));

	// CLEAR / CUT でも初期値へ戻さない (ほかは戻す)
	n2->setValueNotifyingHost(0.4f);

	ParamEdit::clear(ctx, to);
	CHECK(n2->getValue() == doctest::Approx(0.4f).epsilon(0.01));
	CHECK(s2->getValue() == doctest::Approx(s2->getDefaultValue()));

	ParamEdit::cut(ctx, to);
	CHECK(n2->getValue() == doctest::Approx(0.4f).epsilon(0.01));

	// 入れると貼る
	toggle->setToggleState(true, juce::dontSendNotification);
	if (toggle->onClick) toggle->onClick();

	ParamEdit::paste(ctx, to, ParamEdit::capture(ctx, from));
	CHECK(n2->getValue() == doctest::Approx(0.2f).epsilon(0.01));

	// 入れると消す
	ParamEdit::clear(ctx, to);
	CHECK(n2->getValue() == doctest::Approx(n2->getDefaultValue()));

	toggle->setToggleState(false, juce::dontSendNotification);
	if (toggle->onClick) toggle->onClick();
}
