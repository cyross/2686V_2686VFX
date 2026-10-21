#include "./StepSnap.h"

#include <cmath>

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiValues.h"

namespace
{
	// LEVEL の STEPS から「1 刻み」を除いたもの。並びは LEVEL に合わせる
	const std::vector<SelectItem>& stepItems()
	{
		static const std::vector<SelectItem> items = {
			{ I18n::pick(u8" 1: 自由設定", u8" 1: Free"), 1 },
			{ I18n::pick(u8" 2: 1 / 2 刻み", u8" 2: by 1 / 2"), 2 },
			{ I18n::pick(u8" 3: 1 / 4 刻み", u8" 3: by 1 / 4"), 3 },
			{ I18n::pick(u8" 4: 1 / 8 刻み", u8" 4: by 1 / 8"), 4 },
			{ I18n::pick(u8" 5: 1 / 16 刻み", u8" 5: by 1 / 16"), 5 },
			{ I18n::pick(u8" 6: 1 / 32 刻み", u8" 6: by 1 / 32"), 6 },
			{ I18n::pick(u8" 7: 1 / 64 刻み", u8" 7: by 1 / 64"), 7 },
			{ I18n::pick(u8" 8: 1 / 128 刻み", u8" 8: by 1 / 128"), 8 },
			{ I18n::pick(u8" 9: 1 / 256 刻み", u8" 9: by 1 / 256"), 9 },
			{ I18n::pick(u8"10: 1 / 3 刻み", u8"10: by 1 / 3"), 10 },
			{ I18n::pick(u8"11: 1 / 6 刻み", u8"11: by 1 / 6"), 11 },
			{ I18n::pick(u8"12: 1 / 7 刻み", u8"12: by 1 / 7"), 12 },
			{ I18n::pick(u8"13: 1 / 10 刻み", u8"13: by 1 / 10"), 13 },
			{ I18n::pick(u8"14: 1 / 100 刻み", u8"14: by 1 / 100"), 14 },
		};

		return items;
	}
}

GuiComponentStepSnap::GuiComponentStepSnap(const GuiContext& context) :
	GuiBase(context),
	stepSelector(context)
{
}

GuiComponentStepSnap::~GuiComponentStepSnap()
{
	for (auto* s : m_sliders) s->removeListener(this);
}

int GuiComponentStepSnap::stepsOf(int index)
{
	static constexpr int table[] = { 0, 2, 4, 8, 16, 32, 64, 128, 256, 3, 6, 7, 10, 100 };

	if (index < 0 || index >= (int)std::size(table)) return 0;

	return table[index];
}

double GuiComponentStepSnap::snap(double value, int steps, double lo, double hi)
{
	if (steps <= 0) return value;

	return juce::jlimit(lo, hi, std::round(value * steps) / steps);
}

void GuiComponentStepSnap::setupComponent(juce::Component& parent, const std::vector<GuiSlider*>& sliders, int& tabOrder,
	const juce::String& title, std::optional<juce::Font> labelFont)
{
	for (auto* s : m_sliders) s->removeListener(this);

	m_sliders = sliders;

	for (auto* s : m_sliders) s->addListener(this);

	// 初めは自由
	for (auto* s : m_sliders) s->setSnapSteps(0);

	stepSelector.setup({ .parent = parent, .id = "", .title = title, .items = stepItems(), .isReset = false, .labelFont = labelFont });
	stepSelector.setSelectedItemIndex(0, juce::dontSendNotification); // 初めは自由
	stepSelector.setWantsKeyboardFocus(true);
	stepSelector.setExplicitFocusOrder(++tabOrder);

	// 切り替えた瞬間に、いまの値を丸める
	stepSelector.onChange = [this] { snapAll(); };
}

void GuiComponentStepSnap::snapOne(GuiSlider& slider)
{
	const int steps = stepsOf(stepSelector.getSelectedItemIndex());

	if (steps <= 0) return;

	const double now = slider.getValue();
	const double snapped = snap(now, steps, slider.getMinimum(), slider.getMaximum());

	// 既に丸まっているなら書き戻さない。書き戻すと知らせがもう一巡する
	if (std::abs(now - snapped) < 1.0e-6) return;

	// 知らせは止めない。止めると丸めた値がパラメータへ伝わらず、見た目と音がずれる
	slider.setValue(snapped, juce::sendNotification);
}

void GuiComponentStepSnap::snapAll()
{
	// ラベルのドラッグやナッジの並びが、選んだ刻みで動かせるよう知らせる
	const int steps = stepsOf(stepSelector.getSelectedItemIndex());

	for (auto* s : m_sliders) s->setSnapSteps(steps);

	for (auto* s : m_sliders) snapOne(*s);
}

void GuiComponentStepSnap::sliderValueChanged(juce::Slider* slider)
{
	for (auto* s : m_sliders)
	{
		if (s != slider) continue;

		// 束縛し直している途中 (TARGET の切り替え) は、その先の値をそのまま見せる。
		// ここで丸めると、切り替えただけで切り替えた先の値を書き換えてしまう
		if (s->isRebinding()) return;

		snapOne(*s);
		return;
	}
}

void GuiComponentStepSnap::layoutComponent(juce::Rectangle<int>& rect, int rowHeight)
{
	layoutMain({ .mainRect = rect, .label = &stepSelector.label, .component = &stepSelector,
		.rowHeight = rowHeight > 0 ? rowHeight : CoreGuiValue::MainGroup::Row::height });
}

void GuiComponentStepSnap::layoutComponentRow(juce::Rectangle<int>& rect, int rowHeight)
{
	layoutRow({ .rowRect = rect, .label = &stepSelector.label, .component = &stepSelector,
		.rowHeight = rowHeight > 0 ? rowHeight : CoreGuiValue::ParamGroup::Row::height });
}

void GuiComponentStepSnap::setVisibleWithLabel(bool visible)
{
	stepSelector.setVisibleWithLabel(visible);
}

void GuiComponentStepSnap::setEnabledWithLabel(bool enabled)
{
	stepSelector.setEnabledWithLabel(enabled);
}
