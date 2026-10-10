// ============================================================================
// 一部の FM パラメータ表示を、PC-9801 の N88-BASIC(86) 準拠にする
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
	void collectSliders(juce::Component& root, std::vector<GuiSlider*>& out)
	{
		for (auto* c : root.getChildren())
		{
			if (auto* s = dynamic_cast<GuiSlider*>(c)) out.push_back(s);

			collectSliders(*c, out);
		}
	}

	std::vector<GuiSlider*> slidersEndingWith(juce::Component& root, const juce::String& suffix)
	{
		std::vector<GuiSlider*> all, out;

		collectSliders(root, all);

		for (auto* s : all)
		{
			if (s->getComponentID().endsWith(suffix)) out.push_back(s);
		}

		return out;
	}
}

TEST_CASE("N88-BASIC(86) 準拠: 対象は FM のレジスタの AR / DR / SR / RR / SL (D1R / D2R / D1L) だけ")
{
	for (const char* id : { "OPNA_OP0_R_AR", "OPN_OP1_R_DR", "OPNA_OP2_R_SR", "OPL_OP0_R_RR", "OPM_OP3_R_SL",
		"OPM_OP0_R_D1R", "OPZX7_OP7_R_D2R", "OPZX7_OP0_R_D1L" })
		CHECK(GuiSlider::isN88Target(id));

	for (const char* id : { "OPNA_OP0_R_TL", "OPNA_OP0_AR", "OPM_OP0_R_KS", "SSG_AR" })
		CHECK_FALSE(GuiSlider::isN88Target(id));
}

TEST_CASE("N88-BASIC(86) 準拠: 入れると数字と向きが逆になり、内部の値は変わらない")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	auto ars = slidersEndingWith(*editor, "_R_AR");

	if (ars.empty()) return;

	auto* ar = ars.front();
	auto* param = processor->apvts.getParameter(ar->getComponentID());

	REQUIRE(param != nullptr);

	const double lo = ar->getMinimum();
	const double hi = ar->getMaximum();

	INFO(ar->getComponentID().toStdString());

	// 切っているときは、そのまま
	CHECK_FALSE(ar->isN88Displayed());
	CHECK(ar->getTextFromValue(lo) == juce::String(juce::roundToInt(lo)));

	processor->fmN88Display = true;
	GuiSlider::refreshN88Display(*editor);

	CHECK(ar->isN88Displayed());

	// 内部 0 は「31」、内部 31 は「0」と見える
	CHECK(ar->getTextFromValue(lo) == juce::String(juce::roundToInt(hi)));
	CHECK(ar->getTextFromValue(hi) == juce::String(juce::roundToInt(lo)));

	// 左端が最大 (内部の値として)
	CHECK(ar->proportionOfLengthToValue(0.0) == doctest::Approx(hi));
	CHECK(ar->proportionOfLengthToValue(1.0) == doctest::Approx(lo));

	// 画面の数字 (MML の入力) で 0 を入れると、内部では最大
	ar->setDisplayedValue(lo, juce::sendNotificationSync);
	CHECK(ar->getValue() == doctest::Approx(hi));
	CHECK(param->convertFrom0to1(param->getValue()) == doctest::Approx(hi));

	// 打ち込んだ数字も逆に読む
	CHECK(ar->getValueFromText(juce::String(juce::roundToInt(hi))) == doctest::Approx(lo));

	// 対象でないつまみは変わらない
	auto tls = slidersEndingWith(*editor, "_R_TL");

	if (!tls.empty()) CHECK_FALSE(tls.front()->isN88Displayed());

	// 切ると戻る。値はそのまま
	processor->fmN88Display = false;
	GuiSlider::refreshN88Display(*editor);

	CHECK_FALSE(ar->isN88Displayed());
	CHECK(ar->getValue() == doctest::Approx(hi));
	CHECK(ar->getTextFromValue(lo) == juce::String(juce::roundToInt(lo)));
	CHECK(ar->proportionOfLengthToValue(0.0) == doctest::Approx(lo));

	editor.reset();
}

TEST_CASE("N88-BASIC(86) 準拠: SETTINGS のトグルは MUL/DET・FIX の下にあり、画面からはみ出さない")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	std::vector<GuiToggleButton*> toggles;

	std::function<void(juce::Component&)> collect = [&](juce::Component& c) {
		for (auto* child : c.getChildren())
		{
			if (auto* t = dynamic_cast<GuiToggleButton*>(child)) toggles.push_back(t);

			collect(*child);
		}
	};

	collect(*editor);

	GuiToggleButton* n88 = nullptr;
	GuiToggleButton* live = nullptr;

	for (auto* t : toggles)
	{
		if (t->getButtonText().contains("N88-BASIC(86)")) n88 = t;
		if (t->getButtonText().contains("MUL/DET")) live = t;
	}

	// 2686VFX には無い
	if (n88 == nullptr) return;

	REQUIRE(live != nullptr);

	auto* page = n88->getParentComponent();

	// 開いているタブでなくても並べてもらう
	page->setBounds(page->getParentComponent() != nullptr ? page->getParentComponent()->getLocalBounds() : page->getBounds());
	page->resized();

	CHECK(n88->getY() > live->getY());
	CHECK_FALSE(n88->getToggleState());

	// 区分の枠からはみ出す部品が無い
	for (auto* c : page->getChildren())
	{
		if (!c->isVisible() || c->getBounds().isEmpty()) continue;

		INFO(c->getName().toStdString() << " " << c->getBounds().toString().toStdString()
			<< " / page " << page->getLocalBounds().toString().toStdString());
		CHECK(c->getBottom() <= page->getHeight());
	}

	editor.reset();
}
