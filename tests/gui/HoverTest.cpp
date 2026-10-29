// ============================================================================
// マウスが乗ったときの色 (4.3.0)
// ============================================================================
// 部品の地と枠は、決まった色へ変えずに元の色から作る。暗い色は明るく、
// 明るい色は少し暗く、透けている色は少し濃くする。
// 区分の中身の板は、乗っているいちばん内側の 1 枚だけを少し明るくする。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Shared/Core/Gui/GuiColor.h"
#include "Shared/Core/Gui/GuiComponents.h"

TEST_CASE("Hover: 部品の地と枠は元の色に合わせて変える")
{
	SUBCASE("暗い色は明るくなる")
	{
		const auto dark = juce::Colour::fromRGB(0x20, 0x20, 0x60);

		CHECK(GuiColor::Hover::face(dark).getPerceivedBrightness() > dark.getPerceivedBrightness());
		CHECK(GuiColor::Hover::border(dark).getPerceivedBrightness() > GuiColor::Hover::face(dark).getPerceivedBrightness());
	}

	SUBCASE("明るい色は少し暗くなる")
	{
		const juce::Colour light = GuiColor::Palette::OffWhite;

		CHECK(GuiColor::Hover::face(light).getPerceivedBrightness() < light.getPerceivedBrightness());
		CHECK(GuiColor::Hover::border(light).getPerceivedBrightness() < GuiColor::Hover::face(light).getPerceivedBrightness());
	}

	SUBCASE("透けている色は濃くなる。透けていない色は透けない")
	{
		const auto clear = juce::Colours::black.withAlpha(0.35f);

		CHECK(GuiColor::Hover::face(clear).getFloatAlpha() > clear.getFloatAlpha());
		CHECK(GuiColor::Hover::face(juce::Colours::darkblue).getFloatAlpha() == doctest::Approx(1.0f));
	}
}

TEST_CASE("Hover: 区分の板は、乗っているいちばん内側の 1 枚だけを明るくする")
{
	juce::Component parent;
	parent.setBounds(0, 0, 200, 200);
	parent.setVisible(true);

	// 大区分の板の中に小区分の板がある形
	GuiCategoryBackdrop major;
	GuiCategoryBackdrop minor;

	parent.addAndMakeVisible(major);
	parent.addAndMakeVisible(minor);

	major.setBounds(0, 0, 200, 200);
	minor.setBounds(10, 10, 50, 50);

	GuiCategoryBackdrop::updateHover({ 20.0f, 20.0f });

	CHECK(minor.isHovered());
	CHECK_FALSE(major.isHovered());

	GuiCategoryBackdrop::updateHover({ 150.0f, 150.0f });

	CHECK_FALSE(minor.isHovered());
	CHECK(major.isHovered());

	// 外へ出たら戻す
	GuiCategoryBackdrop::updateHover({ 500.0f, 500.0f });

	CHECK_FALSE(minor.isHovered());
	CHECK_FALSE(major.isHovered());

	// 隠れている板は明るくしない
	minor.setVisible(false);

	GuiCategoryBackdrop::updateHover({ 20.0f, 20.0f });

	CHECK_FALSE(minor.isHovered());
	CHECK(major.isHovered());

	GuiCategoryBackdrop::updateHover({ 500.0f, 500.0f });
}
