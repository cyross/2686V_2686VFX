// ============================================================================
// 初期化のあとの描き直し (4.3.0)
// ============================================================================
// FM 音源のオペレーター・RHYTHM のパッドの小さな絵は、TARGET が指している
// 1 枚しか値の変化を追わない。PRESET タブの初期化や RESET で値を戻したあとは、
// 並べ直して残りの絵も描き直す。描き直さないと、TARGET で指し直すまで
// 戻る前の絵が残り、初期化されていないように見えた。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <map>
#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
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

	// タブの名前ごとに、そのタブの絵と、いまの番号
	using Graphs = std::map<GuiEnvelopeGraph*, std::pair<juce::String, std::uint32_t>>;

	Graphs graphsOfAllTabs(juce::AudioProcessorEditor& editor, juce::TabbedComponent& tabs)
	{
		Graphs out;

		// 隠れたタブの中身は木から外れているので、1 枚ずつ前へ出して集める
		for (int i = 0; i < tabs.getNumTabs(); ++i)
		{
			tabs.setCurrentTabIndex(i);

			std::vector<juce::Component*> all;

			collect(&editor, all);

			for (auto* c : all) {
				if (auto* g = dynamic_cast<GuiEnvelopeGraph*>(c)) out[g] = { tabs.getTabNames()[i], g->revision() };
			}
		}

		return out;
	}

	void scramble(AudioPlugin2686V& processor)
	{
		juce::Random rnd(1);

		for (auto* p : processor.getParameters()) {
			if (auto* r = dynamic_cast<juce::RangedAudioParameter*>(p)) {
				if (r->paramID != CPK::mode) r->setValueNotifyingHost(rnd.nextFloat());
			}
		}
	}

	// 描き直されなかった絵の数をタブごとに数える。
	// TARGET の大きな枠は指している枠を写すだけで番号を進めないので、1 枚までは残ってよい
	void checkRedrawn(const Graphs& before)
	{
		std::map<juce::String, int> stale;

		for (auto& [g, v] : before) {
			if (g->revision() == v.second) ++stale[v.first];
		}

		for (auto& [tab, n] : stale) {
			INFO("tab " << tab);
			CHECK(n <= 1);
		}
	}

	struct Fixture
	{
		std::unique_ptr<AudioPlugin2686V> processor = std::make_unique<AudioPlugin2686V>();
		std::unique_ptr<juce::AudioProcessorEditor> editor;

		Fixture()
		{
			editor.reset(processor->createEditor());

			EditorTestAccess::materializeAll(ed());

			// どのタブも一度並べ、値を散らしてから絵を描かせておく
			scramble(*processor);

			for (int i = 0; i < tabs().getNumTabs(); ++i)
			{
				tabs().setCurrentTabIndex(i);
				ed().resized();
			}
		}

		AudioPlugin2686VEditor& ed() { return *static_cast<AudioPlugin2686VEditor*>(editor.get()); }
		juce::TabbedComponent& tabs() { return EditorTestAccess::tabs(ed()); }

		juce::TextButton* button(const juce::String& text)
		{
			std::vector<juce::Component*> all;

			collect(editor.get(), all);

			for (auto* c : all) {
				if (auto* b = dynamic_cast<juce::TextButton*>(c); b != nullptr && b->getButtonText() == text && b->onClick) return b;
			}

			return nullptr;
		}
	};
}

TEST_CASE("InitRedraw: PRESET タブの初期化のあと、どのタブの絵も描き直す")
{
	Fixture f;

	const int presetTab = f.tabs().getTabNames().indexOf("PRESET");

	if (presetTab < 0)
	{
		MESSAGE("PRESET タブが無いので飛ばす");

		return;
	}

	const auto before = graphsOfAllTabs(*f.editor, f.tabs());

	f.tabs().setCurrentTabIndex(presetTab);

	// 確かめのダイアログを越えたあとと同じことをする
	f.processor->initPreset();
	f.ed().resized();

	for (int i = 0; i < f.tabs().getNumTabs(); ++i) f.tabs().setCurrentTabIndex(i);

	checkRedrawn(before);
}

TEST_CASE("InitRedraw: RESET のあと、TARGET が指していないオペレーター・パッドの絵も描き直す")
{
	Fixture f;

	const auto before = graphsOfAllTabs(*f.editor, f.tabs());

	// 音源のタブを順に前へ出して RESET を押す
	for (int i = 0; i < f.tabs().getNumTabs(); ++i)
	{
		f.tabs().setCurrentTabIndex(i);

		if (auto* reset = f.button(EditorGuiText::Reset::title); reset != nullptr && reset->isEnabled()) reset->onClick();
	}

	for (int i = 0; i < f.tabs().getNumTabs(); ++i) f.tabs().setCurrentTabIndex(i);

	checkRedrawn(before);
}
