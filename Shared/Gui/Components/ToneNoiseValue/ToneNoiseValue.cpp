#include "./ToneNoiseValue.h"

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiStructs.h"
#include "Shared/Core/Processor/ProcessorKeys.h"

namespace
{
	namespace Text
	{
		inline const juce::String toneValue = "TONE: Use VALUE";
		inline const juce::String toneValueKnob = "T.VALUE";
		inline const juce::String noiseValue = "NOISE: Use VALUE";
		inline const juce::String noiseValueKnob = "N.VALUE";
	}
}

GuiComponentToneNoiseValue::GuiComponentToneNoiseValue(const GuiContext& context) :
	GuiBase(context),
	toneValueToggle(context),
	toneValueSlider(context),
	toneValueNudge(context),
	noiseValueToggle(context),
	noiseValueSlider(context),
	noiseValueNudge(context)
{
}

void GuiComponentToneNoiseValue::setupComponent(juce::Component& parent, const juce::String& prefix, int& tabOrder, GuiSlider* noiseFreq)
{
	noiseFreqSlider = noiseFreq;

	toneValueToggle.setup({ .parent = parent, .id = prefix + CPK::Tn::toneValueOn, .title = Text::toneValue, .isReset = true });
	toneValueToggle.setWantsKeyboardFocus(true);
	toneValueToggle.setExplicitFocusOrder(++tabOrder);

	toneValueSlider.setupComponent(parent, prefix + CPK::Tn::toneValue, Text::toneValueKnob, tabOrder, std::nullopt);
	toneValueNudge.setupComponent(parent, toneValueSlider.getSlider(), GuiComponentValueNudge::toneValueRows(), tabOrder);

	noiseValueToggle.setup({ .parent = parent, .id = prefix + CPK::Tn::noiseValueOn, .title = Text::noiseValue, .isReset = true });
	noiseValueToggle.setWantsKeyboardFocus(true);
	noiseValueToggle.setExplicitFocusOrder(++tabOrder);

	noiseValueSlider.setupComponent(parent, prefix + CPK::Tn::noiseValue, Text::noiseValueKnob, tabOrder, std::nullopt);
	noiseValueNudge.setupComponent(parent, noiseValueSlider.getSlider(), GuiComponentValueNudge::noiseValueRows(), tabOrder);

	// TARGET の切り替えやプリセットの読み込みでも変わるので、状態の変化を受ける
	toneValueToggle.watchToggle([this] { applyActive(); });
	noiseValueToggle.watchToggle([this] { applyActive(); });

	applyActive();
}

void GuiComponentToneNoiseValue::rebind(const juce::String& prefix)
{
	toneValueToggle.rebind(prefix + CPK::Tn::toneValueOn);
	toneValueSlider.getSlider().rebind(prefix + CPK::Tn::toneValue);
	noiseValueToggle.rebind(prefix + CPK::Tn::noiseValueOn);
	noiseValueSlider.getSlider().rebind(prefix + CPK::Tn::noiseValue);

	applyActive();
}

void GuiComponentToneNoiseValue::setVisibles(bool visible)
{
	toneValueToggle.setVisible(visible);
	toneValueSlider.setVisibleWithLabel(visible);
	toneValueNudge.setVisibles(visible && toneValueSlider.isVisibleNudge());
	noiseValueToggle.setVisible(visible);
	noiseValueSlider.setVisibleWithLabel(visible);
	noiseValueNudge.setVisibles(visible && noiseValueSlider.isVisibleNudge());
}

void GuiComponentToneNoiseValue::layoutComponent(juce::Rectangle<int>& rect)
{
	layoutMain({ .mainRect = rect, .component = &toneValueToggle });
	toneValueSlider.layoutComponent(rect);
	if (toneValueSlider.isVisibleNudge()) toneValueNudge.layoutComponent(rect);
	layoutMain({ .mainRect = rect, .component = &noiseValueToggle });
	noiseValueSlider.layoutComponent(rect);
	if (noiseValueSlider.isVisibleNudge()) noiseValueNudge.layoutComponent(rect);
}

void GuiComponentToneNoiseValue::layoutComponentRow(juce::Rectangle<int>& rect)
{
	layoutRow({ .rowRect = rect, .component = &toneValueToggle });
	toneValueSlider.layoutComponentRow(rect);
	if (toneValueSlider.isVisibleNudge()) toneValueNudge.layoutComponentRow(rect);
	layoutRow({ .rowRect = rect, .component = &noiseValueToggle });
	noiseValueSlider.layoutComponentRow(rect);
	if (noiseValueSlider.isVisibleNudge()) noiseValueNudge.layoutComponentRow(rect);
}

void GuiComponentToneNoiseValue::applyActive()
{
	toneValueSlider.setEnabledWithLabel(toneValueToggle.getToggleState());
	toneValueNudge.setEnables(toneValueToggle.getToggleState());

	const bool useValue = noiseValueToggle.getToggleState();

	noiseValueSlider.setEnabledWithLabel(useValue);
	noiseValueNudge.setEnables(useValue);

	if (noiseFreqSlider != nullptr) noiseFreqSlider->setEnabledWithLabel(!useValue);
}
