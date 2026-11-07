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
#include "Gui/Settings/GuiSettingsText.h"
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

	// 入り切りは、速さと同じ行の左に置いてある。レベルメーター (4.4.0) の行も速さを選ぶので、それは除く
	std::vector<GuiToggleButton*> toggles;
	std::vector<GuiComboBox*> liveSelectors;

	for (auto* selector : selectors)
	{
		auto row = findAll<GuiToggleButton>(*page, [selector](GuiToggleButton& t) {
			return t.isVisible() && !t.getBounds().isEmpty()
				&& t.getParentComponent() == selector->getParentComponent() && t.getY() == selector->getY() && t.getRight() <= selector->getX();
		});

		REQUIRE(row.size() == 1);

		// 頁からはみ出さない
		CHECK(selector->getBottom() <= page->getHeight());

		if (row[0]->getButtonText() == SettingsGuiText::levelMeter.get()) continue;

		toggles.push_back(row[0]);
		liveSelectors.push_back(selector);
	}

	selectors = liveSelectors;

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

TEST_CASE("SETTINGS: レベルメーターは入り切りでき、切るとほかの項目は選べない。メーターは子のいちばん奥で、タブの下 3/4 にある")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	auto toggles = findAll<GuiToggleButton>(*editor, [](GuiToggleButton& t) { return t.getButtonText() == SettingsGuiText::levelMeter.get(); });

	REQUIRE(toggles.size() == 1);

	auto* toggle = toggles[0];
	auto* page = toggle->getParentComponent();
	auto* settings = dynamic_cast<GuiBase*>(page);

	REQUIRE(settings != nullptr);

	settings->layout(page->getLocalBounds());

	// 同じ行に、速さ・描き方・帯の数・ピークホールド
	auto selectors = findAll<GuiComboBox>(*page, [toggle](GuiComboBox& b) { return b.isVisible() && b.getY() == toggle->getY() && b.getX() >= toggle->getRight(); });
	auto peakHolds = findAll<GuiToggleButton>(*page, [](GuiToggleButton& t) { return t.getButtonText() == SettingsGuiText::levelMeterPeakHold.get(); });

	REQUIRE(selectors.size() == 3);
	REQUIRE(peakHolds.size() == 1);

	CHECK(peakHolds[0]->getY() == toggle->getY());
	CHECK(peakHolds[0]->getBottom() <= page->getHeight());
	CHECK(peakHolds[0]->getRight() <= page->getWidth());

	// 初期値は入、15 FPS、LED、32 帯、ピークホールド入
	CHECK(processor->levelMeterShow);
	CHECK(processor->levelMeterFps == 15);
	CHECK(processor->levelMeterStyle == GuiLevelMeter::led);
	CHECK(processor->levelMeterBands == 32);
	CHECK(processor->levelMeterPeakHold);

	GuiComboBox* style = nullptr;
	GuiComboBox* bands = nullptr;

	for (auto* b : selectors) {
		if (b->getNumItems() == GuiLevelMeter::styleCount) style = b;
		if (b->getNumItems() == 2) bands = b;
	}

	REQUIRE(style != nullptr);
	REQUIRE(bands != nullptr);

	// 80s-90s 01〜04 も選べる
	CHECK(style->getItemText(GuiLevelMeter::retro01) == "80s-90s 01");
	CHECK(style->getItemText(GuiLevelMeter::retro04) == "80s-90s 04");

	// 選ぶとプロセッサの値が変わる
	style->setSelectedId(GuiLevelMeter::line + 1, juce::sendNotificationSync);
	bands->setSelectedId(16, juce::sendNotificationSync);

	CHECK(processor->levelMeterStyle == GuiLevelMeter::line);
	CHECK(processor->levelMeterBands == 16);

	// 切るとほかの項目は選べない
	toggle->setToggleState(false, juce::dontSendNotification);

	REQUIRE(toggle->onClick != nullptr);

	toggle->onClick();

	CHECK_FALSE(processor->levelMeterShow);

	for (auto* b : selectors) CHECK_FALSE(b->isEnabled());

	CHECK_FALSE(peakHolds[0]->isEnabled());

	// メーターは子のいちばん奥 (壁紙のすぐ上) にあり、タブの中身の下 3/4 に収まる
	auto meters = findAll<GuiLevelMeter>(*editor, [](GuiLevelMeter&) { return true; });

	REQUIRE(meters.size() == 1);

	auto* meter = meters[0];

	CHECK(meter->getParentComponent() == editor.get());
	CHECK(editor->getIndexOfChildComponent(meter) == 0);
	bool clicks = true, childClicks = true;
	meter->getInterceptsMouseClicks(clicks, childClicks);

	CHECK_FALSE(clicks);

	editor->resized();

	auto* tabs = findAll<juce::TabbedComponent>(*editor, [](juce::TabbedComponent&) { return true; }).front();
	const auto content = tabs->getBounds().withTrimmedTop(tabs->getTabBarDepth());

	CHECK(content.contains(meter->getBounds()));
	CHECK(meter->getBottom() >= content.getBottom() - 20);
	CHECK(meter->getHeight() >= content.getHeight() * 3 / 4 - 20);
	CHECK(meter->getHeight() <= content.getHeight() * 3 / 4);

	// どの描き方でも、広い枠・狭い枠ともに描ける。音を入れて、段が灯った状態で描く
	processor->levelMeterShow = true;
	processor->levelMeterPeakHold = true;

	meter->setVisible(true);
	meter->refresh();

	REQUIRE(processor->levelMeterFeed.active.load());

	{
		// 低いほうから高いほうへ、大きさを変えた正弦波をいくつか重ねる
		std::vector<float> l(4096), r(4096);
		const double freqs[] = { 60.0, 150.0, 400.0, 1000.0, 2400.0, 6000.0, 12000.0 };
		const float amps[] = { 0.5f, 0.3f, 0.2f, 0.12f, 0.08f, 0.04f, 0.02f };

		for (size_t i = 0; i < l.size(); ++i) {
			float v = 0.0f;

			for (size_t k = 0; k < std::size(freqs); ++k) v += amps[k] * (float)std::sin(juce::MathConstants<double>::twoPi * freqs[k] * (double)i / 48000.0);

			l[i] = v;
			r[i] = v * 0.6f;
		}

		processor->levelMeterFeed.push(l.data(), r.data(), (int)l.size(), 48000.0);
	}

	for (int st = 0; st < GuiLevelMeter::styleCount; ++st) {
		for (bool compact : { false, true }) {
			for (int bandCount : GuiLevelMeter::bandChoices) {
				INFO("style " << st << " compact " << compact << " bands " << bandCount);

				processor->levelMeterStyle = st;
				processor->levelMeterBands = bandCount;
				meter->setCompact(compact);

				meter->pull();

				const auto image = meter->createComponentSnapshot(meter->getLocalBounds());

				CHECK(image.isValid());

				// 見た目を確かめたいときは、置き場を環境変数で渡すと PNG に書き出す
				if (const auto dir = juce::SystemStats::getEnvironmentVariable("LEVEL_METER_SNAPSHOT_DIR", {}); dir.isNotEmpty()) {
					juce::Image framed(juce::Image::ARGB, image.getWidth(), image.getHeight(), true);
					{
						juce::Graphics fg(framed);
						fg.fillAll(juce::Colours::black);
						fg.drawImageAt(image, 0, 0);
					}

					juce::File file = juce::File(dir).getChildFile("meter_" + juce::String(st) + (compact ? "_compact" : "") + "_" + juce::String(bandCount) + ".png");
					file.deleteFile();

					juce::FileOutputStream out(file);
					juce::PNGImageFormat().writeImageToStream(framed, out);
				}
			}
		}
	}

	editor.reset();
}
