// ============================================================================
// つまみの値の動かし方: ラベルのドラッグと、N ボタンで開くナッジの並び
// ============================================================================
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "Shared/Gui/Components/StepSnap/StepSnap.h"
#include "./EditorTestAccess.h"

namespace
{
	juce::StringArray titlesOf(const std::vector<GuiSlider::NudgeSpec>& specs)
	{
		juce::StringArray out;

		for (const auto& s : specs) out.add(s.title);

		return out;
	}

	const GuiSlider::NudgeSpec* findSpec(const std::vector<GuiSlider::NudgeSpec>& specs, const juce::String& title)
	{
		for (const auto& s : specs)
			if (s.title == title) return &s;

		return nullptr;
	}
}

TEST_CASE("ナッジ: 並びは範囲と間隔から決まり、押すと値が動く")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	GuiContext ctx(*processor, *dynamic_cast<GuiEditorHost*>(editor.get()), processor->apvts);

	juce::Component parent;
	GuiSlider slider(ctx);

	slider.setup({ .parent = parent, .title = "VALUE" });

	SUBCASE("整数 0〜127")
	{
		slider.setRange(0.0, 127.0, 1.0);

		const auto specs = slider.nudgeSpecs();

		CHECK(titlesOf(specs).joinIntoString(" ") == "MIN -10 -1 0 +1 +10 MAX");

		slider.setValue(64.0, juce::sendNotificationSync);
		slider.applyNudge(*findSpec(specs, "+10"));
		CHECK(slider.getValue() == doctest::Approx(74.0));

		slider.applyNudge(*findSpec(specs, "-1"));
		CHECK(slider.getValue() == doctest::Approx(73.0));

		slider.applyNudge(*findSpec(specs, "MAX"));
		CHECK(slider.getValue() == doctest::Approx(127.0));

		// 範囲の外へは出ない
		slider.applyNudge(*findSpec(specs, "+10"));
		CHECK(slider.getValue() == doctest::Approx(127.0));

		slider.applyNudge(*findSpec(specs, "MIN"));
		CHECK(slider.getValue() == doctest::Approx(0.0));
	}

	SUBCASE("連続値 0.0〜1.0")
	{
		slider.setRange(0.0, 1.0, 0.0);

		const auto specs = slider.nudgeSpecs();

		CHECK(titlesOf(specs).joinIntoString(" ") == "MIN -0.1 -0.01 -0.001 0 +0.001 +0.01 +0.1 MAX");

		slider.setValue(0.5, juce::sendNotificationSync);
		slider.applyNudge(*findSpec(specs, "+0.01"));
		CHECK(slider.getValue() == doctest::Approx(0.51));
	}

	SUBCASE("0〜7 のように狭い整数は 1 だけ")
	{
		slider.setRange(0.0, 7.0, 1.0);

		CHECK(titlesOf(slider.nudgeSpecs()).joinIntoString(" ") == "MIN -1 0 +1 MAX");
	}
}

TEST_CASE("ナッジ: STEPS で丸めているあいだは、刻みの倍数で動かす")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	GuiContext ctx(*processor, *dynamic_cast<GuiEditorHost*>(editor.get()), processor->apvts);

	juce::Component parent;
	GuiSlider slider(ctx);
	GuiComponentStepSnap steps(ctx);
	int tabOrder = 0;

	slider.setup({ .parent = parent, .title = "SL" });
	slider.setRange(0.0, 1.0, 0.0);
	steps.setupComponent(parent, { &slider }, tabOrder);

	CHECK(slider.getSnapSteps() == 0);

	// 1/4 刻み
	steps.selector().setSelectedItemIndex(2, juce::sendNotificationSync);
	CHECK(slider.getSnapSteps() == 4);

	const auto specs = slider.nudgeSpecs();

	CHECK(titlesOf(specs).joinIntoString(" ") == "MIN -1/4 0 +1/4 MAX");

	slider.setValue(0.25, juce::sendNotificationSync);
	slider.applyNudge(*findSpec(specs, "+1/4"));
	CHECK(slider.getValue() == doctest::Approx(0.5));

	// 1/100 刻みなら 10 刻みぶんも出す
	steps.selector().setSelectedItemIndex(13, juce::sendNotificationSync);
	CHECK(titlesOf(slider.nudgeSpecs()).joinIntoString(" ") == "MIN -10/100 -1/100 0 +1/100 +10/100 MAX");

	// ラベルのドラッグも刻みで動く。1/4 刻みは 1 刻みに 50px
	steps.selector().setSelectedItemIndex(2, juce::sendNotificationSync);
	CHECK(slider.valueForLabelDrag(0.25, 50) == doctest::Approx(0.5));
	CHECK(slider.valueForLabelDrag(0.25, 20) == doctest::Approx(0.25));
	CHECK(slider.valueForLabelDrag(0.25, -50) == doctest::Approx(0.0));
	CHECK(slider.valueForLabelDrag(0.25, 1000) == doctest::Approx(1.0));

	// STEPS のあいだは、Shift / Ctrl でそろえる刻みは使わない
	CHECK(slider.labelDragAlignStep(juce::ModifierKeys(juce::ModifierKeys::shiftModifier)) == 0.0);
	CHECK(slider.labelDragAlignStep(juce::ModifierKeys(juce::ModifierKeys::ctrlModifier)) == 0.0);

	// 自由へ戻すと、刻みは使わない
	steps.selector().setSelectedItemIndex(0, juce::sendNotificationSync);
	CHECK(slider.getSnapSteps() == 0);
}

TEST_CASE("ラベルのドラッグ: 上へ動かすと増え、Shift / Ctrl で刻みへそろう")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	GuiContext ctx(*processor, *dynamic_cast<GuiEditorHost*>(editor.get()), processor->apvts);

	juce::Component parent;
	GuiSlider slider(ctx);

	slider.setup({ .parent = parent, .title = "VALUE" });
	slider.setRange(0.0, 100.0, 0.0);

	// 200px で端から端まで
	CHECK(slider.valueForLabelDrag(50.0, 20) == doctest::Approx(60.0));
	CHECK(slider.valueForLabelDrag(50.0, -20) == doctest::Approx(40.0));

	// 範囲の外へは出ない
	CHECK(slider.valueForLabelDrag(50.0, 1000) == doctest::Approx(100.0));
	CHECK(slider.valueForLabelDrag(50.0, -1000) == doctest::Approx(0.0));

	using MK = juce::ModifierKeys;

	// 押しているキーとそろえる刻み
	CHECK(slider.labelDragAlignStep(MK()) == 0.0);
	CHECK(slider.labelDragAlignStep(MK(MK::shiftModifier)) == 0.1);
	CHECK(slider.labelDragAlignStep(MK(MK::ctrlModifier)) == 1.0);
	CHECK(slider.labelDragAlignStep(MK(MK::ctrlModifier | MK::shiftModifier)) == 10.0);

	// 刻みへそろえてから、刻みの数で動かす。0〜100 の 1 刻みは 2px (下限)、10 刻みは 20px
	CHECK(slider.valueForLabelDrag(50.37, 0, 0.1) == doctest::Approx(50.4));
	CHECK(slider.valueForLabelDrag(50.37, 4, 1.0) == doctest::Approx(52.0));
	CHECK(slider.valueForLabelDrag(53.0, 0, 10.0) == doctest::Approx(50.0));
	CHECK(slider.valueForLabelDrag(53.0, 20, 10.0) == doctest::Approx(60.0));
	CHECK(slider.valueForLabelDrag(53.0, -20, 10.0) == doctest::Approx(40.0));

	// 整数のつまみでは、Shift の 0.1 刻みは効かない
	slider.setRange(0.0, 127.0, 1.0);
	CHECK(slider.labelDragAlignStep(MK(MK::shiftModifier)) == 0.0);
	CHECK(slider.labelDragAlignStep(MK(MK::ctrlModifier)) == 1.0);
}

TEST_CASE("ナッジ: 値を直に置くボタンを足せる (カーブ編集の K の「1」)")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	GuiContext ctx(*processor, *dynamic_cast<GuiEditorHost*>(editor.get()), processor->apvts);

	juce::Component parent;
	GuiSlider slider(ctx);

	// K と同じく、パラメータに束ねず、範囲は JUCE の既定 (0〜10)
	slider.setup({ .parent = parent, .title = "K" });
	slider.addNudgeValue("1", 1.0);

	const auto specs = slider.nudgeSpecs();

	CHECK(titlesOf(specs).joinIntoString(" ") == "MIN -1 -0.1 -0.01 0 1 +0.01 +0.1 +1 MAX");

	int starts = 0, ends = 0;
	slider.onDragStart = [&] { ++starts; };
	slider.onDragEnd = [&] { ++ends; };

	slider.setValue(3.5, juce::sendNotificationSync);
	slider.applyNudge(*findSpec(specs, "1"));
	CHECK(slider.getValue() == doctest::Approx(1.0));

	// 帯のドラッグと同じく、始まりと終わりを知らせる (カーブ編集の表の作り直し)
	CHECK(starts == 1);
	CHECK(ends == 1);
}

TEST_CASE("ナッジ: つまみの右端に N ボタンがあり、押すと並びを編集画面へ重ねて出す")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	GuiContext ctx(*processor, *dynamic_cast<GuiEditorHost*>(editor.get()), processor->apvts);

	juce::Component parent;
	GuiSlider slider(ctx);

	editor->addAndMakeVisible(parent);
	parent.setBounds(0, 0, 300, 40);

	slider.setup({ .parent = parent, .title = "VALUE" });
	slider.setRange(0.0, 127.0, 1.0);
	slider.setBounds(80, 10, 200, 20);

	CHECK(slider.isNudgeButtonShown());

	slider.showNudgePopup();
	REQUIRE(slider.isNudgePopupShown());
	CHECK(slider.getNudgePopup()->getParentComponent() == editor.get());

	slider.hideNudgePopup();
	CHECK_FALSE(slider.isNudgePopupShown());

	// 帯が残らないほど狭いときは出さない
	slider.setBounds(80, 10, 60, 20);
	CHECK_FALSE(slider.isNudgeButtonShown());

	// 自前の N ボタンを持つ部品は外す
	slider.setBounds(80, 10, 200, 20);
	slider.setNudgeButtonWanted(false);
	CHECK_FALSE(slider.isNudgeButtonShown());

	editor->removeChildComponent(&parent);
}

TEST_CASE("ALG ENV: ALG の N ボタンで出す、値を動かすボタンがある")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	int checked = 0;

	std::function<void(juce::Component&)> walk = [&](juce::Component& c) {
		for (int i = 0; i < c.getNumChildComponents(); ++i)
		{
			auto* child = c.getChildComponent(i);

			// 値のつまみ (ALG) の直後に N ボタン、その後に値を動かすボタンが並ぶ
			if (auto* s = dynamic_cast<GuiSlider*>(child); s != nullptr && s->label.getText() == "ALG" && i + 2 < c.getNumChildComponents())
			{
				auto* n = dynamic_cast<GuiTextButton*>(c.getChildComponent(i + 1));

				if (n != nullptr && n->getButtonText() == "N")
				{
					auto* first = dynamic_cast<GuiTextButton*>(c.getChildComponent(i + 2));

					CHECK(first != nullptr);
					if (first != nullptr) CHECK((first->getButtonText() == "-10" || first->getButtonText() == "-1"));

					++checked;
				}
			}

			walk(*child);
		}

		if (auto* tabs = dynamic_cast<juce::TabbedComponent*>(&c))
		{
			for (int i = 0; i < tabs->getNumTabs(); ++i)
				if (auto* content = tabs->getTabContentComponent(i); content != nullptr && content->getParentComponent() != &c) walk(*content);
		}
	};

	walk(*editor);

	MESSAGE("ALG ENV を確かめた数: " << checked);

#if defined(GUI_TEST_REQUIRE_ALL)
	CHECK(checked > 0);
#endif
}

TEST_CASE("グラフの拡大率: 帯のドラッグも Shift / Ctrl で刻みへそろう")
{
	using MK = juce::ModifierKeys;

	GuiZoomSlider slider;
	slider.setRange(GuiEnvelopeGraph::zoomMin, GuiEnvelopeGraph::zoomMax, 0.01);
	slider.setSkewFactorFromMidPoint(4.0);

	const auto saved = MK::currentModifiers;

	auto snapWith = [&](int mods, double v) {
		MK::currentModifiers = MK(mods);
		return slider.snapValue(v, juce::Slider::absoluteDrag);
	};

	CHECK(snapWith(0, 3.37) == doctest::Approx(3.37));
	CHECK(snapWith(MK::shiftModifier, 3.37) == doctest::Approx(3.4));
	CHECK(snapWith(MK::ctrlModifier, 3.37) == doctest::Approx(3.0));
	CHECK(snapWith(MK::ctrlModifier | MK::shiftModifier, 13.7) == doctest::Approx(10.0));

	// 範囲 (0.1〜32) へ収める
	CHECK(snapWith(MK::ctrlModifier | MK::shiftModifier, 2.0) == doctest::Approx(GuiEnvelopeGraph::zoomMin));

	// ドラッグでないとき (数字の打ち込みなど) は、そろえない
	MK::currentModifiers = MK(MK::ctrlModifier);
	CHECK(slider.snapValue(3.37, juce::Slider::notDragging) == doctest::Approx(3.37));

	MK::currentModifiers = saved;
}

TEST_CASE("グラフの拡大率: x0.1〜x32 を選択肢から選べ、初期値は x1 (4.4.1)")
{
	CHECK(GuiEnvelopeGraph::zoomMin == doctest::Approx(0.1f));
	CHECK(GuiEnvelopeGraph::zoomMax == doctest::Approx(32.0f));

	for (int width : { 270, 400, 900 })
	{
		INFO("幅 " << width);

		GuiEnvelopeGraph graph;
		graph.setZoomable(true);
		graph.setBounds(0, 0, width, 200);

		// ボタンの列は無く、選択肢の欄が 1 つ
		juce::ComboBox* box = nullptr;
		int buttons = 0;

		for (auto* child : graph.getChildren())
		{
			if (auto* c = dynamic_cast<juce::ComboBox*>(child)) box = c;
			if (auto* b = dynamic_cast<juce::TextButton*>(child); b != nullptr && b->getButtonText().startsWith("x")) ++buttons;
		}

		CHECK(buttons == 0);
		REQUIRE(box != nullptr);

		CHECK(box->getNumItems() == (int)GuiZoom::choices.size());
		CHECK(box->getItemText(0) == "x0.1");
		CHECK(box->getItemText(1) == "x0.125");
		CHECK(box->getItemText(2) == "x0.167");
		CHECK(box->getItemText(box->getNumItems() - 1) == "x32");
		CHECK(box->getText() == "x1");
		CHECK(graph.getZoom() == doctest::Approx(1.0f));

		// 欄の中に収まり、枠からはみ出さない
		CHECK(box->getRight() <= graph.getWidth());
		CHECK(box->getWidth() >= 50);

		// 選ぶと倍率が変わる。x1 未満も選べる
		box->setSelectedItemIndex(0, juce::sendNotificationSync);
		CHECK(graph.getZoom() == doctest::Approx(0.1f));

		box->setSelectedItemIndex(box->getNumItems() - 1, juce::sendNotificationSync);
		CHECK(graph.getZoom() == doctest::Approx(32.0f));

		// 選択肢に無い値 (つまみで決めた値) は、その値を字で出す
		graph.setZoom(1.5f);
		CHECK(box->getText() == "x1.5");

		graph.setZoom(0.05f);
		CHECK(graph.getZoom() == doctest::Approx(0.1f));

		// 見た目を確かめたいときは、置き場を環境変数で渡すと PNG に書き出す
		if (const auto dir = juce::SystemStats::getEnvironmentVariable("STAGE_WAVE_SNAPSHOT_DIR", {}); dir.isNotEmpty())
		{
			graph.setZoom(1.0f);

			juce::Image image(juce::Image::ARGB, graph.getWidth(), graph.getHeight(), true);
			{
				juce::Graphics g(image);
				g.fillAll(juce::Colour(0xff202020));
				graph.paintEntireComponent(g, true);
			}

			auto file = juce::File(dir).getChildFile("graph_zoom_" + juce::String(width) + ".png");

			file.deleteFile();

			juce::FileOutputStream out(file);
			juce::PNGImageFormat().writeImageToStream(image, out);
		}
	}
}

TEST_CASE("表示倍率の選択肢: 寄せ・送り・字 (4.4.1)")
{
	CHECK(GuiZoom::choices.size() == 30);
	CHECK(GuiZoom::choices.front() == doctest::Approx(0.1f));
	CHECK(GuiZoom::choices.back() == doctest::Approx(32.0f));

	CHECK(GuiZoom::snap(1.0f) == doctest::Approx(1.0f));
	CHECK(GuiZoom::snap(0.12f) == doctest::Approx(0.125f));
	CHECK(GuiZoom::snap(0.92f) == doctest::Approx(0.9f));
	CHECK(GuiZoom::snap(0.96f) == doctest::Approx(1.0f));
	CHECK(GuiZoom::snap(100.0f) == doctest::Approx(32.0f));
	CHECK(GuiZoom::snap(0.0f) == doctest::Approx(1.0f));

	CHECK(GuiZoom::step(1.0f, -1) == doctest::Approx(0.9f));
	CHECK(GuiZoom::step(0.9f, 1) == doctest::Approx(1.0f));
	CHECK(GuiZoom::step(0.1f, -1) == doctest::Approx(0.1f));
	CHECK(GuiZoom::step(32.0f, 1) == doctest::Approx(32.0f));

	CHECK(GuiZoom::label(0.1f) == "x0.1");
	CHECK(GuiZoom::label(0.333f) == "x0.333");
	CHECK(GuiZoom::label(0.5f) == "x0.5");
	CHECK(GuiZoom::label(1.0f) == "x1");
	CHECK(GuiZoom::label(24.0f) == "x24");
}
