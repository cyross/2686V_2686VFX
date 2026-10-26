// ============================================================================
// STEPS: LEVEL のほかの 0.0〜1.0 のつまみ (AMP ENV の SL、SSG HW / SW AMP ENV、
// WT AMP MOD / WT PITCH MOD の DEPTH) を決まった刻みへ丸める
// DUTY / PEAK RATIO ENVELOPE は範囲に対する 1/10・1/4・1/5・1/8・1/16 (4.3.0)
// ============================================================================
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <functional>
#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiGraphValues.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Shared/Gui/Components/StepSnap/StepSnap.h"
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

	// STEPS の選択肢か (自由・1/2・… の 14 個)
	bool isStepSnap(GuiComboBox& box)
	{
		return box.getNumItems() == 14 && box.getItemText(1).contains("1 / 2") && box.getItemText(13).contains("1 / 100");
	}
}

TEST_CASE("STEPS: 選択肢は LEVEL から「1 刻み」を除いたもので、刻みへ丸めて範囲へ収める")
{
	CHECK(GuiComponentStepSnap::stepsOf(0) == 0);    // 自由
	CHECK(GuiComponentStepSnap::stepsOf(1) == 2);
	CHECK(GuiComponentStepSnap::stepsOf(2) == 4);
	CHECK(GuiComponentStepSnap::stepsOf(8) == 256);
	CHECK(GuiComponentStepSnap::stepsOf(9) == 3);
	CHECK(GuiComponentStepSnap::stepsOf(13) == 100);
	CHECK(GuiComponentStepSnap::stepsOf(14) == 0);

	CHECK(GuiComponentStepSnap::snap(0.3, 0, 0.0, 1.0) == doctest::Approx(0.3));
	CHECK(GuiComponentStepSnap::snap(0.3, 4, 0.0, 1.0) == doctest::Approx(0.25));
	CHECK(GuiComponentStepSnap::snap(0.4, 3, 0.0, 1.0) == doctest::Approx(1.0 / 3.0));
	CHECK(GuiComponentStepSnap::snap(0.99, 2, 0.0, 1.0) == doctest::Approx(1.0));
	CHECK(GuiComponentStepSnap::snap(1.2, 2, 0.0, 1.0) == doctest::Approx(1.0));
}

TEST_CASE("STEPS: SL / SSG HW・SW AMP ENV / WT AMP MOD の DEPTH の下にあり、選ぶと値を丸める")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	// 上限が 1.0 の、刻みのないつまみ (FM の SL のようなレジスタ値は外す)
	auto unitSlider = [&](const juce::String& suffix) -> GuiSlider* {
		auto found = findAll<GuiSlider>(*editor, [&](GuiSlider& s) {
			return s.getComponentID().endsWith(suffix) && s.getMinimum() >= 0.0 && s.getMaximum() == 1.0 && s.getInterval() < 0.01;
		});

		return found.empty() ? nullptr : found.front();
	};

	// つまみの後に置いた STEPS (同じ親の中で、つまみより後に足した最初のもの)。
	// 区分が閉じていると場所が決まらないので、足した順で探す
	auto stepsBelow = [&](GuiSlider& slider) -> GuiComboBox* {
		auto* parent = slider.getParentComponent();

		if (parent == nullptr) return nullptr;

		const int from = parent->getIndexOfChildComponent(&slider);

		for (int i = from + 1; i < parent->getNumChildComponents(); ++i)
		{
			if (auto* box = dynamic_cast<GuiComboBox*>(parent->getChildComponent(i)); box != nullptr && isStepSnap(*box)) return box;
		}

		return nullptr;
	};

	struct Target { const char* name; juce::String suffix; };

	const std::vector<Target> targets = {
		{ "AMP ENV SL", CPK::Adsr::sl },
		{ "SSG HW AMP ENV MIN", CPK::SsgHwEnv::min },
		{ "SSG HW AMP ENV MAX", CPK::SsgHwEnv::max },
		// L.TARGET が 0 のとき、LEVEL のつまみは STL に繋がっている
		{ "SSG SW AMP ENV[256] LEVEL", CPK::SsgSwEnv256::stl },
		{ "SSG SW AMP ENV[256] LR", CPK::SsgSwEnv256::lr },
		{ "SSG SW AMP ENV[256] ENDL", CPK::SsgSwEnv256::endl },
		{ "WT AMP MOD DEPTH", CPK::WtAmpMod::depth },
		{ "WT PITCH MOD DEPTH", CPK::WtMod::depth },
	};

	int checked = 0;

	for (const auto& t : targets)
	{
		INFO(t.name);

		auto* slider = unitSlider(t.suffix);

		// その音源を持たないプラグインは飛ばす
		if (slider == nullptr) continue;

		auto* steps = stepsBelow(*slider);

		REQUIRE(steps != nullptr);

		// 効くように、つまみを触れる状態にする
		const bool wasEnabled = slider->isEnabled();
		slider->setEnabled(true);

		// 1/4 刻みを選ぶと、いまの値が丸まる
		slider->setValue(0.3, juce::sendNotificationSync);
		steps->setSelectedItemIndex(2, juce::sendNotificationSync);
		CHECK(slider->getValue() == doctest::Approx(0.25));

		// 動かしても刻みへ寄る
		slider->setValue(0.6, juce::sendNotificationSync);
		CHECK(slider->getValue() == doctest::Approx(0.5));

		// 下限より下の刻みは下限に寄る (WT PITCH MOD の DEPTH は 0.1 から)
		slider->setValue(0.05, juce::sendNotificationSync);
		CHECK(slider->getValue() == doctest::Approx(juce::jmax(0.0, slider->getMinimum())));

		// 自由へ戻せば丸めない
		steps->setSelectedItemIndex(0, juce::sendNotificationSync);
		slider->setValue(0.3, juce::sendNotificationSync);
		CHECK(slider->getValue() == doctest::Approx(0.3));

		slider->setEnabled(wasEnabled);

		++checked;
	}

	MESSAGE("STEPS を確かめたつまみ: " << checked);
	CHECK(checked > 0);

	editor.reset();
}

TEST_CASE("STEPS: TARGET の切り替え (束縛し直し) では、切り替えた先の値を書き換えない")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	auto& apvts = processor->apvts;

	// 0.0〜1.0 の連続値のパラメータを 2 つ探す
	juce::StringArray ids;

	for (auto* p : processor->getParameters())
	{
		auto* f = dynamic_cast<juce::AudioParameterFloat*>(p);

		if (f == nullptr || f->range.start != 0.0f || f->range.end != 1.0f || f->range.interval > 0.0f) continue;

		if (f->paramID.endsWith(CPK::Adsr::sl)) ids.add(f->paramID);

		if (ids.size() == 2) break;
	}

	if (ids.size() < 2) return;

	GuiContext ctx(*processor, *dynamic_cast<GuiEditorHost*>(editor.get()), apvts);

	juce::Component parent;
	GuiSlider slider(ctx);
	GuiComponentStepSnap steps(ctx);
	int tabOrder = 0;

	slider.setup({ .parent = parent, .id = ids[0], .title = "SL", .isReset = false });
	steps.setupComponent(parent, { &slider }, tabOrder);

	// 切り替え先は刻みに乗っていない値にしておく
	auto* other = apvts.getParameter(ids[1]);
	other->setValueNotifyingHost(0.3f);

	steps.selector().setSelectedItemIndex(2, juce::sendNotificationSync); // 1/4

	slider.setValue(0.6, juce::sendNotificationSync);
	CHECK(slider.getValue() == doctest::Approx(0.5));

	// 切り替えただけでは丸めない
	slider.rebind(ids[1]);
	CHECK(slider.getValue() == doctest::Approx(0.3).epsilon(0.001));
	CHECK(other->getValue() == doctest::Approx(0.3f).epsilon(0.001));

	// 切り替えた後に動かせば丸める
	slider.setValue(0.4, juce::sendNotificationSync);
	CHECK(slider.getValue() == doctest::Approx(0.5));

	editor.reset();
}

namespace
{
	// DUTY / PEAK RATIO ENVELOPE の STEPS の選択肢か (自由・1/10・1/4・1/5・1/8・1/16 の 6 個)
	bool isRatioSnap(GuiComboBox& box)
	{
		return box.getNumItems() == 6 && box.getItemText(1).contains("1 / 10") && box.getItemText(5).contains("1 / 16");
	}
}

TEST_CASE("STEPS (DUTY / PEAK RATIO ENVELOPE): 1/10・1/4・1/5・1/8・1/16 で、範囲の幅に対して丸める")
{
	using Snap = GuiComponentStepSnap;

	CHECK(Snap::stepsOf(0, Snap::Choices::Ratio) == 0);    // 自由
	CHECK(Snap::stepsOf(1, Snap::Choices::Ratio) == 10);
	CHECK(Snap::stepsOf(2, Snap::Choices::Ratio) == 4);
	CHECK(Snap::stepsOf(3, Snap::Choices::Ratio) == 5);
	CHECK(Snap::stepsOf(4, Snap::Choices::Ratio) == 8);
	CHECK(Snap::stepsOf(5, Snap::Choices::Ratio) == 16);
	CHECK(Snap::stepsOf(6, Snap::Choices::Ratio) == 0);

	// PEAK (0〜1): 0.1 / 0.25 / 0.2 / 0.125 / 0.0625 単位
	CHECK(Snap::snap(0.33, 10, 0.0, 1.0, 1.0) == doctest::Approx(0.3));
	CHECK(Snap::snap(0.33, 5, 0.0, 1.0, 1.0) == doctest::Approx(0.4));
	CHECK(Snap::snap(0.33, 16, 0.0, 1.0, 1.0) == doctest::Approx(0.3125));

	// DUTY (0〜100%): 10% / 25% / 20% / 12.5% / 6.25% 単位
	CHECK(Snap::snap(37.0, 10, 0.0, 100.0, 100.0) == doctest::Approx(40.0));
	CHECK(Snap::snap(37.0, 4, 0.0, 100.0, 100.0) == doctest::Approx(25.0));
	CHECK(Snap::snap(37.0, 8, 0.0, 100.0, 100.0) == doctest::Approx(37.5));
	CHECK(Snap::snap(37.0, 16, 0.0, 100.0, 100.0) == doctest::Approx(37.5));
}

TEST_CASE("STEPS (DUTY / PEAK RATIO ENVELOPE): 区分の値のつまみの下にあり、選ぶと値を丸める")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	auto boxes = findAll<GuiComboBox>(*editor, [](GuiComboBox& b) { return isRatioSnap(b); });

	// SSG を持たないプラグインには無い
	if (boxes.empty()) return;

	int checked = 0;

	for (auto* box : boxes)
	{
		// 同じ区分の値のつまみ (名前が DUTY / PEAK)。三角波の PEAK (パラメータ) は除く
		GuiSlider* value = nullptr;

		for (auto* s : findAll<GuiSlider>(*box->getParentComponent(), [](GuiSlider&) { return true; }))
		{
			if (s->getParentComponent() != box->getParentComponent()) continue;
			if (s->getComponentID().endsWith(CPK::SsgTri::peak)) continue;

			const auto name = s->label.getText();

			if (name == "DUTY" || name == "PEAK") value = s;
		}

		if (value == nullptr) continue;

		const double span = value->getMaximum() - value->getMinimum();

		CAPTURE(span);

		value->setValue(0.33 * span, juce::sendNotificationSync);

		// 1/10 刻み
		box->setSelectedItemIndex(1, juce::sendNotificationSync);

		CHECK(value->getValue() == doctest::Approx(0.3 * span));
		CHECK(value->getSnapSteps() == 10);
		CHECK(value->getSnapScale() == doctest::Approx(span));

		// 自由へ戻す
		box->setSelectedItemIndex(0, juce::sendNotificationSync);

		++checked;
	}

	MESSAGE("DUTY / PEAK の STEPS を確かめた数: " << checked);
	CHECK(checked > 0);

	editor.reset();
}
