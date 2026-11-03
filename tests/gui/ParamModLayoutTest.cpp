// ============================================================================
// PARAM ENV[256] と TAP PARAM SEQUENCER の区分 (4.3.0)
// ============================================================================
//   ・TARGET の並びは、束ねたパラメータの範囲と同じ数でなければならない
//     (選んだ番号と値の対応は、並びの数で割って決まるため)
//   ・TAP PARAM SEQUENCER の ADD PAIR は見えて押せ、押すと段へ組が入る
//   ・SETTINGS で、グラフの鳴っている位置の表示を入り切りでき、速さ (FPS) を選べる
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <functional>
#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "./EditorTestAccess.h"

namespace
{
	template <typename T>
	std::vector<T*> findAll(juce::Component& root, std::function<bool(T&)> pred)
	{
		std::vector<T*> out;

		std::function<void(juce::Component&)> walk = [&](juce::Component& c) {
			for (auto* child : c.getChildren())
			{
				if (auto* t = dynamic_cast<T*>(child); t != nullptr && pred(*t)) out.push_back(t);

				walk(*child);
			}

			if (auto* tabs = dynamic_cast<juce::TabbedComponent*>(&c))
			{
				for (int i = 0; i < tabs->getNumTabs(); ++i)
					if (auto* content = tabs->getTabContentComponent(i); content != nullptr && content->getParentComponent() != &c) walk(*content);
			}
		};

		walk(root);

		return out;
	}

	void relayout(juce::Component& c)
	{
		c.resized();

		for (auto* child : c.getChildren()) relayout(*child);
	}

	struct Editor
	{
		std::unique_ptr<AudioPlugin2686V> processor{ std::make_unique<AudioPlugin2686V>() };
		std::unique_ptr<juce::AudioProcessorEditor> editor;

		// title を含む区分だけを開いて並べ直す
		explicit Editor(const juce::String& title)
		{
			editor.reset(processor->createEditor());

			EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

			for (auto* cat : findAll<GuiCategoryLabel>(*editor, [](GuiCategoryLabel&) { return true; }))
				cat->setDetailVisible(cat->getText().contains(title) || cat->getText().startsWith("[["));

			editor->resized();
			relayout(*editor);
		}
	};
}

TEST_CASE("PARAM ENV[256]: TARGET の並びは、束ねたパラメータの範囲と同じ数")
{
	Editor e("PARAM ENV");

	const auto boxes = findAll<GuiComboBox>(*e.editor, [](GuiComboBox& b) { return b.getComponentID().endsWith("_PRMENV_TARGET"); });

#if GUI_TEST_IS_FX
	// 2686VFX は音源を持たない
	CHECK(boxes.empty());
#else
	REQUIRE(!boxes.empty());

	for (auto* box : boxes)
	{
		CAPTURE(box->getComponentID());

		auto* param = dynamic_cast<juce::AudioParameterInt*>(e.processor->apvts.getParameter(box->getComponentID()));

		REQUIRE(param != nullptr);

		const int count = param->getRange().getEnd() - param->getRange().getStart() + 1;

		CHECK(box->getNumItems() == count);

		// 並びの名前はどれも空でない
		for (int i = 0; i < box->getNumItems(); ++i) CHECK(box->getItemText(i).isNotEmpty());
	}
#endif
}

TEST_CASE("TAP PARAM SEQUENCER: ADD PAIR は見えて押せ、押すと段へ組が入る")
{
	Editor e("TAP PARAM SEQUENCER");

	auto adds = findAll<juce::TextButton>(*e.editor, [](juce::TextButton& b) { return b.getButtonText() == "ADD PAIR"; });

#if GUI_TEST_IS_FX
	// 2686VFX は音源を持たない
	CHECK(adds.empty());
#else
	REQUIRE(!adds.empty());

	int pressed = 0;

	for (auto* add : adds)
	{
		// 窓に収まらず高さの無い区分は見ない
		if (!add->isVisible() || add->getHeight() <= 0) continue;

		auto* parent = add->getParentComponent();
		auto* hit = parent->getComponentAt(add->getBounds().getCentre());

		CAPTURE(add->getBounds().toString());
		CHECK((hit == add || (hit != nullptr && add->isParentOf(hit))));

		REQUIRE(add->onClick != nullptr);

		add->onClick();

		++pressed;
	}

	CHECK(pressed > 0);

	// 押した数だけ、どこかの表の 1 段目に組が入っている
	int withPairs = 0;

	e.processor->stepTables.forEach([&withPairs](const juce::String& key, StepTable& table) {
		if (key.endsWith("_TAPPARAM") && table.pairCount(1) > 0) ++withPairs;
	});

	CHECK(withPairs == pressed);
#endif
}

TEST_CASE("SETTINGS: グラフの鳴っている位置の表示は入り切りでき、切ると速さは選べない")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	auto selectors = findAll<GuiComboBox>(*editor, [](GuiComboBox& b) {
		return b.getNumItems() == 4 && b.getItemText(0) == "15 FPS" && b.getItemText(3) == "120 FPS";
	});

	REQUIRE(!selectors.empty());

	// SETTINGS は裏のタブなので、表に出るまで並べられない。ここで並べてもらう
	auto* page = selectors[0]->getParentComponent();
	auto* settings = dynamic_cast<GuiBase*>(page);

	REQUIRE(settings != nullptr);

	settings->layout(page->getLocalBounds());

	// 入り切りは、速さと同じ行の左に置いてある
	std::vector<GuiToggleButton*> toggles;

	for (auto* selector : selectors)
	{
		auto row = findAll<GuiToggleButton>(*page, [selector](GuiToggleButton& t) {
			return t.isVisible() && !t.getBounds().isEmpty()
				&& t.getParentComponent() == selector->getParentComponent() && t.getY() == selector->getY() && t.getRight() <= selector->getX();
		});

		REQUIRE(row.size() == 1);

		// 頁からはみ出さない
		CHECK(selector->getBottom() <= page->getHeight());

		toggles.push_back(row[0]);
	}

#if GUI_TEST_IS_FX
	// 2686VFX は TAP SEQUENCER だけ
	const size_t expected = 1;
#else
	// TAP SEQUENCER・PARAM ENV[256]・エンベロープ
	const size_t expected = 3;
#endif

	REQUIRE(toggles.size() == expected);
	REQUIRE(selectors.size() == expected);

	// 初期値は入、30 FPS
	CHECK(processor->tapLiveGraph);
	CHECK(processor->tapLiveFps == 30);

	for (size_t i = 0; i < expected; ++i)
	{
		CHECK(toggles[i]->getToggleState());
		CHECK(selectors[i]->isEnabled());
		CHECK(selectors[i]->getSelectedId() == 30);
	}

	// 速さを選ぶと、プロセッサの値が変わる
	selectors[0]->setSelectedId(60, juce::sendNotificationSync);

	CHECK(processor->tapLiveFps == 60);

	// 切ると速さは選べない
	toggles[0]->setToggleState(false, juce::dontSendNotification);

	REQUIRE(toggles[0]->onClick != nullptr);

	toggles[0]->onClick();

	CHECK_FALSE(processor->tapLiveGraph);
	CHECK_FALSE(selectors[0]->isEnabled());

	editor.reset();
}
