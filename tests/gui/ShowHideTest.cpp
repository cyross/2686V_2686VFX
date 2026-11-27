// ============================================================================
// 区分の中のまとまりの出し入れ (4.5.0) と、送りの範囲
// ============================================================================
// ・初めは隠していて「SHOW xxx」。押すと「HIDE xxx」になり、まとまりの部品が
//   ボタンの下に出る。上の区切り線は出したままなので、ボタンは動かない。もう一度押すと戻る
// ・区分とまとまりをすべて開いても、送る中身の丈が部品の下端まで届いている
//   (並べるときの仮の高さ 2000px で打ち切られて、下の区分へ送れなかった)
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <functional>
#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiColor.h"
#include "Shared/Gui/Components/ShowHide/ShowHide.h"
#include "./EditorTestAccess.h"

namespace
{
	void collect(juce::Component* root, std::vector<juce::Component*>& out)
	{
		for (auto* c : root->getChildren())
		{
			out.push_back(c);
			collect(c, out);
		}
	}

	template <typename T>
	std::vector<T*> findAll(juce::Component* root, std::function<bool(T&)> pred = [](T&) { return true; })
	{
		std::vector<juce::Component*> all;
		std::vector<T*> out;

		collect(root, all);

		for (auto* c : all)
		{
			if (auto* t = dynamic_cast<T*>(c)) {
				if (pred(*t)) out.push_back(t);
			}
		}

		return out;
	}

	// 区分をすべて開いて並べ直す
	void openAllCategories(juce::AudioProcessorEditor& editor)
	{
		for (auto* cat : findAll<GuiCategoryLabel>(&editor)) cat->setDetailVisible(true);

		editor.resized();
	}

	// 区分の中のまとまりもすべて出す
	void showAllParts(juce::AudioProcessorEditor& editor)
	{
		for (auto* b : findAll<GuiShowHideButton>(&editor)) b->setShown(true);

		editor.resized();
	}

	// 画面の上で本当に出ているか (親をたどって、どれも出ているか)
	bool reallyVisible(juce::Component* c, juce::Component* root)
	{
		for (; c != nullptr && c != root; c = c->getParentComponent())
			if (!c->isVisible()) return false;

		return c == root;
	}

	// 同じ親に載っていて、いま出ている部品の数
	int visibleSiblings(juce::Component& c)
	{
		int n = 0;

		for (auto* s : c.getParentComponent()->getChildren())
			if (s->isVisible()) ++n;

		return n;
	}
}

TEST_CASE("ShowHide: 初めは隠していて、押すと SHOW と HIDE が入れ替わり、まとまりがボタンの下に出る")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	EditorTestAccess::unfoldAllGroups(processor->apvts);

	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());
	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	auto& tabs = EditorTestAccess::tabs(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	bool tried = false;

	for (int i = 0; i < tabs.getNumTabs() && !tried; ++i)
	{
		tabs.setCurrentTabIndex(i);
		openAllCategories(*editor);

		auto* root = editor.get();
		auto buttons = findAll<GuiShowHideButton>(root, [root](GuiShowHideButton& b) {
			return reallyVisible(&b, root) && b.getPartsName() == "LOOP";
		});

		if (buttons.empty()) continue;

		tried = true;

		auto* button = buttons.front();

		// 初めは隠している
		CHECK_FALSE(button->isShown());
		CHECK(button->getButtonText() == "SHOW LOOP");
		CHECK(button->findColour(juce::TextButton::buttonColourId) == GuiColor::ShowHide::ShowBg.get());
		CHECK(button->findColour(juce::TextButton::textColourOffId) == GuiColor::ShowHide::ShowText.get());

		const int before = visibleSiblings(*button);
		const int y = button->getY();

		std::vector<juce::Component*> hiddenBefore;
		for (auto* s : button->getParentComponent()->getChildren())
			if (!s->isVisible()) hiddenBefore.push_back(s);

		button->onClick();

		CHECK(button->isShown());
		CHECK(button->getButtonText() == "HIDE LOOP");
		CHECK(button->findColour(juce::TextButton::buttonColourId) == GuiColor::ShowHide::HideBg.get());

		// まとまりの部品が出る。ボタンは動かず、出た部品はすべてボタンの下に並ぶ
		CHECK(visibleSiblings(*button) > before);
		CHECK(button->getY() == y);

		int appeared = 0;
		int below = 0;
		for (auto* s : hiddenBefore)
		{
			if (!s->isVisible()) continue;
			++appeared;
			if (s->getY() >= button->getBottom()) ++below;
		}
		CHECK(appeared > 0);
		CHECK(below == appeared);

		button->onClick();

		CHECK_FALSE(button->isShown());
		CHECK(visibleSiblings(*button) == before);
		CHECK(button->getY() == y);
	}

	CHECK(tried);
}

TEST_CASE("ShowHide: 区分とまとまりをすべて開いても、送る中身の丈が部品の下端まで届く")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	EditorTestAccess::unfoldAllGroups(processor->apvts);

	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());
	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	auto& tabs = EditorTestAccess::tabs(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	int checked = 0;

	for (int i = 0; i < tabs.getNumTabs(); ++i)
	{
		tabs.setCurrentTabIndex(i);
		openAllCategories(*editor);
		showAllParts(*editor);

		for (auto* group : findAll<GuiScrollGroup>(editor.get(), [](GuiScrollGroup& g) { return g.isVisible(); }))
		{
			int bottom = 0;

			for (auto* child : group->contentCanvas.getChildren())
				if (child->isVisible()) bottom = juce::jmax(bottom, child->getBottom());

			if (bottom == 0) continue;

			++checked;

			INFO(tabs.getTabNames()[i].toStdString() << " / " << group->getText().toStdString());
			CHECK(group->contentCanvas.getHeight() >= bottom);
		}
	}

	CHECK(checked > 0);
}
