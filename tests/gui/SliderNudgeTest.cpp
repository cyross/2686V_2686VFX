// ============================================================================
// つまみの値の動かし方: ラベルのドラッグと、N ボタンで開くナッジの並び
// ============================================================================
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Gui/Components/StepSnap/StepSnap.h"

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
	CHECK(slider.valueForLabelDrag(0.25, 50, false) == doctest::Approx(0.5));
	CHECK(slider.valueForLabelDrag(0.25, 20, false) == doctest::Approx(0.25));
	CHECK(slider.valueForLabelDrag(0.25, -50, false) == doctest::Approx(0.0));
	CHECK(slider.valueForLabelDrag(0.25, 1000, false) == doctest::Approx(1.0));

	// 自由へ戻すと、刻みは使わない
	steps.selector().setSelectedItemIndex(0, juce::sendNotificationSync);
	CHECK(slider.getSnapSteps() == 0);
}

TEST_CASE("ラベルのドラッグ: 上へ動かすと増え、Shift で細かくなる")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	GuiContext ctx(*processor, *dynamic_cast<GuiEditorHost*>(editor.get()), processor->apvts);

	juce::Component parent;
	GuiSlider slider(ctx);

	slider.setup({ .parent = parent, .title = "VALUE" });
	slider.setRange(0.0, 100.0, 0.0);

	// 200px で端から端まで
	CHECK(slider.valueForLabelDrag(50.0, 20, false) == doctest::Approx(60.0));
	CHECK(slider.valueForLabelDrag(50.0, -20, false) == doctest::Approx(40.0));
	CHECK(slider.valueForLabelDrag(50.0, 20, true) == doctest::Approx(51.0));

	// 範囲の外へは出ない
	CHECK(slider.valueForLabelDrag(50.0, 1000, false) == doctest::Approx(100.0));
	CHECK(slider.valueForLabelDrag(50.0, -1000, false) == doctest::Approx(0.0));
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
