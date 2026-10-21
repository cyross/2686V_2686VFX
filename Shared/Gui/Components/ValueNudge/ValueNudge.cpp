#include "./ValueNudge.h"

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiValues.h"

namespace
{
	// 色は LEVEL のナッジに合わせる
	juce::Colour stepColour() { return juce::Colours::lightblue.brighter(0.5f); }
	juce::Colour baseColour() { return juce::Colours::yellow.brighter(0.5f); }
	juce::Colour upColour() { return juce::Colours::lightgreen.brighter(0.5f); }
	juce::Colour downColour() { return juce::Colours::lightsalmon.brighter(0.5f); }

	using Spec = GuiComponentValueNudge::Spec;
	using Action = GuiComponentValueNudge::Action;

	Spec add(int v) { return { (v > 0 ? "+" : "") + juce::String(v), Action::Add, (double)v, stepColour() }; }
	Spec set(int v, juce::Colour c) { return { juce::String(v), Action::Set, (double)v, c }; }

	// 6 個以上並ぶ行は、文字を小さくして収める
	constexpr int smallFontFrom = 6;
	constexpr float smallFontHeight = 10.0f;
	constexpr int inner = 1;
}

GuiComponentValueNudge::Rows GuiComponentValueNudge::toneValueRows()
{
	return {
		{ add(-1), set(47, baseColour()), add(1) },
		// 上へ: 2 オクターブ・1 オクターブ・5 度・4 度。下へ: 5 度・1 オクターブ・2 オクターブ
		{ set(12, upColour()), set(24, upColour()), set(31, upColour()), set(35, upColour()),
		  set(70, downColour()), set(94, downColour()), set(188, downColour()) },
		{ add(-100), add(-10), add(10), add(100) },
	};
}

GuiComponentValueNudge::Rows GuiComponentValueNudge::noiseValueRows()
{
	return {
		{ add(-1), set(2, baseColour()), add(1) },
		{ set(1, upColour()), set(4, upColour()), set(8, upColour()),
		  set(12, downColour()), set(16, downColour()), set(31, downColour()) },
		{ add(-10), add(-5), add(5), add(10) },
	};
}

GuiComponentValueNudge::Rows GuiComponentValueNudge::envValueRows()
{
	return {
		{ add(-1), set(1303, baseColour()), add(1) },
		// 0.25 / 0.5 / 1 / 2 / 4 / 8 Hz
		{ set(5211, downColour()), set(2606, downColour()), set(1303, baseColour()),
		  set(651, upColour()), set(326, upColour()), set(163, upColour()) },
		{ add(-1000), add(-100), add(-10), add(10), add(100), add(1000) },
	};
}

GuiComponentValueNudge::GuiComponentValueNudge(const GuiContext& context) :
	GuiBase(context)
{
}

void GuiComponentValueNudge::setupComponent(juce::Component& parent, GuiSlider& slider, const Rows& r, int& tabOrder)
{
	rows = r;
	buttons.clear();

	for (const auto& row : rows)
	{
		const bool small = (int)row.size() >= smallFontFrom;

		for (const auto& s : row)
		{
			auto b = std::make_unique<GuiTextButton>(ctx);

			b->setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = s.title,
				.font = small ? std::optional<juce::Font>(juce::Font(juce::FontOptions(smallFontHeight))) : std::nullopt,
				.bgColor = s.colour, .isReset = false, .darkenWhenPressed = true });
			b->setWantsKeyboardFocus(true);
			b->setExplicitFocusOrder(++tabOrder);

			b->onClick = [&slider, s]() {
				const double next = (s.action == Action::Add) ? slider.getValue() + s.value : s.value;

				// 範囲の外はつまみが丸める
				slider.setValue(next, juce::sendNotification);
				};

			buttons.push_back(std::move(b));
		}
	}
}

void GuiComponentValueNudge::layoutRows(juce::Rectangle<int>& rect, int rowHeight, int paddingTop, int paddingBottom)
{
	size_t index = 0;

	for (const auto& row : rows)
	{
		auto area = rect.removeFromTop(rowHeight);
		rect.removeFromTop(paddingTop);

		const int n = (int)row.size();
		const int cellW = (area.getWidth() - inner * (n - 1)) / juce::jmax(1, n);

		for (int k = 0; k < n; ++k)
		{
			// 端数は最後のボタンへ寄せ、行の右端を揃える
			auto cell = (k == n - 1) ? area : area.removeFromLeft(cellW);

			buttons[index++]->setBounds(cell);
			area.removeFromLeft(inner);
		}

		rect.removeFromTop(paddingBottom);
	}
}

void GuiComponentValueNudge::layoutComponent(juce::Rectangle<int>& rect)
{
	layoutRows(rect, CoreGuiValue::MainGroup::Row::height,
		CoreGuiValue::MainGroup::Row::paddingTop, CoreGuiValue::MainGroup::Row::paddingBottom);
}

void GuiComponentValueNudge::layoutComponentRow(juce::Rectangle<int>& rect)
{
	layoutRows(rect, CoreGuiValue::ParamGroup::Row::height,
		CoreGuiValue::ParamGroup::Row::paddingTop, CoreGuiValue::ParamGroup::Row::paddingBottom);
}

void GuiComponentValueNudge::setVisibles(bool visible)
{
	for (auto& b : buttons) b->setVisible(visible);
}

void GuiComponentValueNudge::setEnables(bool enabled)
{
	for (auto& b : buttons) b->setEnabled(enabled);
}
