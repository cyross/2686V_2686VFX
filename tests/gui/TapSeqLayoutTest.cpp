// ============================================================================
// TAP AMP / PITCH SEQUENCER の SKIP ボタンは、開いた区分の中で見えて押せる
// ============================================================================
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

	juce::TextButton* sibling(juce::Component& of, const juce::String& text)
	{
		auto* parent = of.getParentComponent();

		if (parent == nullptr) return nullptr;

		for (auto* child : parent->getChildren())
			if (auto* b = dynamic_cast<juce::TextButton*>(child); b != nullptr && b->getButtonText() == text) return b;

		return nullptr;
	}
}

TEST_CASE("TAP SEQUENCER: SKIP ボタンは CLEAR STEP と同じく見えて、ほかの部品に覆われない")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	// TAP SEQUENCER の区分だけを開いて並べ直す。ほかも開くと、決まった高さの欄から溢れる
	for (auto* cat : findAll<GuiCategoryLabel>(*editor, [](GuiCategoryLabel&) { return true; }))
		cat->setDetailVisible(cat->getText().contains("SEQUENCER"));

	editor->resized();

	std::function<void(juce::Component&)> relayout = [&](juce::Component& c) {
		c.resized();

		for (auto* child : c.getChildren()) relayout(*child);
	};

	relayout(*editor);

	// SKIP の行のボタン (ラベル「SKIP」の右。文字は OFF / ON)
	const auto labels = findAll<GuiLabel>(*editor, [](GuiLabel& l) { return l.getText() == "SKIP"; });

	std::vector<juce::TextButton*> skips;

	for (auto* label : labels)
	{
		for (auto* child : label->getParentComponent()->getChildren())
		{
			auto* b = dynamic_cast<juce::TextButton*>(child);

			if (b != nullptr && b->getButtonText() == "OFF" && b->getY() == label->getY() && b->getX() > label->getX()) skips.push_back(b);
		}
	}

	REQUIRE(!skips.empty());

	int checked = 0;

	for (auto* skip : skips)
	{
		auto* clear = sibling(*skip, "CLEAR STEP");

		REQUIRE(clear != nullptr);

		// 窓に収まらず高さの無い区分は見ない (CLEAR STEP も潰れている)
		if (!clear->isVisible() || clear->getHeight() <= 0) continue;

		CAPTURE(skip->getBounds().toString());
		CAPTURE(clear->getBounds().toString());

		CHECK(skip->isVisible());
		CHECK(skip->getWidth() > 0);
		CHECK(skip->getHeight() > 0);

		// 押せる (同じ親の中で、ほかの部品が上に重なっていない)
		auto* parent = skip->getParentComponent();
		auto* hit = parent->getComponentAt(skip->getBounds().getCentre());

		CHECK((hit == skip || (hit != nullptr && skip->isParentOf(hit))));

		++checked;
	}

	CHECK(checked > 0);
}
