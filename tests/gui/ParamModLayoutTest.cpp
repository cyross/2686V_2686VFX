// ============================================================================
// PARAM ENV[256] と TAP PARAM SEQUENCER の区分 (4.3.0)
// ============================================================================
//   ・TARGET の並びは、束ねたパラメータの範囲と同じ数でなければならない
//     (選んだ番号と値の対応は、並びの数で割って決まるため)
//   ・TAP PARAM SEQUENCER の ADD PAIR は見えて押せ、押すと段へ組が入る
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <functional>
#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"
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
