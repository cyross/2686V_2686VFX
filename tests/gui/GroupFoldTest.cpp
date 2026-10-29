// ============================================================================
// 枠の幅の畳み (4.3.0)
// ============================================================================
// FM 音源のオペレーター・RHYTHM のパッド・2686VFX の枠は、見出しを押すと
// 幅が 30px になり、もう一度押すと戻る。全て開く「<>」・全て閉じる「><」もある。
// どれを畳んだかは state に残るので、読み直した画面でも畳んだまま出る。
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
#if defined(GUI_TEST_REQUIRE_ALL)
	constexpr bool kRequireAll = true;
#else
	constexpr bool kRequireAll = false;
#endif

	void collect(juce::Component* root, std::vector<juce::Component*>& out)
	{
		for (auto* c : root->getChildren())
		{
			out.push_back(c);
			collect(c, out);
		}
	}

	template <typename T>
	std::vector<T*> findAll(juce::Component* root, std::function<bool(T&)> pred)
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

	// 画面に出ているか。テストでは窓を出さないので isShowing は使えない
	bool shown(juce::Component& c)
	{
		for (auto* p = &c; p != nullptr; p = p->getParentComponent())
		{
			if (dynamic_cast<juce::AudioProcessorEditor*>(p) != nullptr) return true;
			if (!p->isVisible()) return false;
		}

		return false;
	}

	struct Editor
	{
		std::unique_ptr<juce::AudioProcessorEditor> editor;

		explicit Editor(AudioPlugin2686V& processor)
		{
			editor.reset(processor.createEditor());

			EditorTestAccess::materializeAll(ed());
		}

		AudioPlugin2686VEditor& ed() { return *static_cast<AudioPlugin2686VEditor*>(editor.get()); }

		// 畳める枠のうち、いま出ているもの
		std::vector<ColoredGroupComponent*> foldables(const juce::String& title = {})
		{
			return findAll<ColoredGroupComponent>(editor.get(), [&](ColoredGroupComponent& g) {
				return g.isFoldable() && shown(g) && (title.isEmpty() || g.getText() == title);
			});
		}

		// 畳める枠のあるタブを開く。無ければ false
		bool openFoldableTab()
		{
			auto& tabs = EditorTestAccess::tabs(ed());

			for (int i = 0; i < tabs.getNumTabs(); ++i)
			{
				tabs.setCurrentTabIndex(i);
				ed().resized();

				if (!foldables().empty()) return true;
			}

			return false;
		}

		juce::TextButton* button(const juce::String& text)
		{
			auto found = findAll<juce::TextButton>(editor.get(), [&](juce::TextButton& b) {
				return b.getButtonText() == text && shown(b);
			});

			return found.empty() ? nullptr : found.front();
		}
	};

	void click(juce::Button& b)
	{
		if (b.onClick) b.onClick();
	}

	// c の上の場所 at へマウスを動かした知らせ
	juce::MouseEvent mouseAt(juce::Component& c, juce::Point<float> at)
	{
		const auto now = juce::Time::getCurrentTime();

		return { juce::Desktop::getInstance().getMainMouseSource(), at, {},
			juce::MouseInputSource::defaultPressure, juce::MouseInputSource::defaultOrientation,
			juce::MouseInputSource::defaultRotation, juce::MouseInputSource::defaultTiltX, juce::MouseInputSource::defaultTiltY,
			&c, &c, now, at, now, 0, false };
	}
}

TEST_CASE("GroupFold: 見出しを押すと幅が 30px になり、もう一度押すと戻る。state に残り、開き直しても畳んだまま")
{
	auto processor = std::make_unique<AudioPlugin2686V>();

	juce::String title;
	int openWidth = 0;
	juce::MemoryBlock saved;

	{
		Editor e(*processor);

		if (!e.openFoldableTab())
		{
			// FM 音源も RHYTHM も持たない本 (WTV・PULSEV) には畳める枠が無い
			if (kRequireAll) FAIL("畳める枠が見つからない");

			MESSAGE("畳める枠が無いので飛ばす");

			return;
		}

		auto* group = e.foldables().front();

		title = group->getText();
		openWidth = group->getWidth();

		REQUIRE(openWidth > ColoredGroupComponent::foldedWidth);
		REQUIRE(group->onTitleClick != nullptr);

		// 畳む
		group->onTitleClick();
		e.ed().resized();

		CHECK(group->isFolded());
		CHECK(group->getWidth() == ColoredGroupComponent::foldedWidth);

		// 戻す
		group->onTitleClick();
		e.ed().resized();

		CHECK_FALSE(group->isFolded());
		CHECK(group->getWidth() == openWidth);

		// 畳んだまま保存する
		group->onTitleClick();
		e.ed().resized();

		processor->getStateInformation(saved);
	}

	// 何も畳んでいない state へ戻してから、保存したものを読む
	{
		juce::MemoryBlock empty;

		{
			Editor e(*processor);

			REQUIRE(e.openFoldableTab());

			click(*e.button("<>"));

			processor->getStateInformation(empty);
		}

		processor->setStateInformation(empty.getData(), (int)empty.getSize());

		Editor e(*processor);

		REQUIRE(e.openFoldableTab());

		for (auto* g : e.foldables()) CHECK_FALSE(g->isFolded());
	}

	processor->setStateInformation(saved.getData(), (int)saved.getSize());

	Editor e(*processor);

	REQUIRE(e.openFoldableTab());

	auto found = e.foldables(title);

	REQUIRE_FALSE(found.empty());
	CHECK(found.front()->isFolded());
	CHECK(found.front()->getWidth() == ColoredGroupComponent::foldedWidth);
}

TEST_CASE("GroupFold: 「><」で全て畳み、「<>」で全て開く")
{
	auto processor = std::make_unique<AudioPlugin2686V>();

	Editor e(*processor);

	if (!e.openFoldableTab())
	{
		if (kRequireAll) FAIL("畳める枠が見つからない");

		MESSAGE("畳める枠が無いので飛ばす");

		return;
	}

	auto* closeAll = e.button("><");
	auto* openAll = e.button("<>");

	REQUIRE(closeAll != nullptr);
	REQUIRE(openAll != nullptr);

	click(*closeAll);
	e.ed().resized();

	for (auto* g : e.foldables())
	{
		CHECK(g->isFolded());
		CHECK(g->getWidth() == ColoredGroupComponent::foldedWidth);
	}

	click(*openAll);
	e.ed().resized();

	for (auto* g : e.foldables())
	{
		CHECK_FALSE(g->isFolded());
		CHECK(g->getWidth() > ColoredGroupComponent::foldedWidth);
	}
}

TEST_CASE("GroupFold: 「<>」は濃い緑、「><」は濃い青の地にオフホワイトの文字")
{
	auto processor = std::make_unique<AudioPlugin2686V>();

	Editor e(*processor);

	if (!e.openFoldableTab())
	{
		if (kRequireAll) FAIL("畳める枠が見つからない");

		MESSAGE("畳める枠が無いので飛ばす");

		return;
	}

	auto* openAll = e.button("<>");
	auto* closeAll = e.button("><");

	REQUIRE(openAll != nullptr);
	REQUIRE(closeAll != nullptr);

	CHECK(openAll->findColour(juce::TextButton::buttonColourId) == GuiColor::GroupFoldBtn::OpenBg.get());
	CHECK(closeAll->findColour(juce::TextButton::buttonColourId) == GuiColor::GroupFoldBtn::CloseBg.get());

	for (auto* b : { openAll, closeAll }) CHECK(b->findColour(juce::TextButton::textColourOffId) == GuiColor::GroupFoldBtn::Text.get());
}

TEST_CASE("GroupFold: 見出しにマウスが乗ると色を変える。畳んだ見出しでも同じ")
{
	auto processor = std::make_unique<AudioPlugin2686V>();

	Editor e(*processor);

	if (!e.openFoldableTab())
	{
		if (kRequireAll) FAIL("畳める枠が見つからない");

		MESSAGE("畳める枠が無いので飛ばす");

		return;
	}

	auto* group = e.foldables().front();

	// 開いた見出しは左上に横向き
	const juce::Point<float> openTitle{ ColoredGroupComponent::titleInsetX + 2.0f, ColoredGroupComponent::titleHeight * 0.5f };

	group->mouseEnter(mouseAt(*group, openTitle));
	CHECK(group->isTitleHovered());

	group->mouseExit(mouseAt(*group, openTitle));
	CHECK_FALSE(group->isTitleHovered());

	// 見出しの外では変えない
	group->mouseMove(mouseAt(*group, group->getLocalBounds().toFloat().getBottomRight() - juce::Point<float>{ 4.0f, 4.0f }));
	CHECK_FALSE(group->isTitleHovered());

	// 畳んだ見出しは上から縦向き
	group->onTitleClick();
	e.ed().resized();

	REQUIRE(group->isFolded());

	const juce::Point<float> foldedTitle{ group->getWidth() * 0.5f, 6.0f };

	group->mouseMove(mouseAt(*group, foldedTitle));
	CHECK(group->isTitleHovered());

	group->mouseExit(mouseAt(*group, foldedTitle));
	CHECK_FALSE(group->isTitleHovered());
}
