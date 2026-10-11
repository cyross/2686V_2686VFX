#include "./ToneNoiseValue.h"

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiStructs.h"
#include "Shared/Core/Processor/ProcessorKeys.h"

namespace
{
	namespace Text
	{
		inline const juce::String toneValue = "TONE: Use VALUE";
		inline const juce::String noiseValue = "NOISE: Use VALUE";
		inline const juce::String noiseValueKnob = "N.VALUE";
	}
}

GuiComponentToneNoiseValue::GuiComponentToneNoiseValue(const GuiContext& context) :
	GuiBase(context),
	toneValueToggle(context),
	noiseValueToggle(context),
	noiseValueSlider(context)
{
}

void GuiComponentToneNoiseValue::setupComponent(juce::Component& parent, const juce::String& prefix, int& tabOrder, GuiSlider* noiseFreq)
{
	noiseFreqSlider = noiseFreq;

	toneValueToggle.setup({ .parent = parent, .id = prefix + CPK::Tn::toneValueOn, .title = Text::toneValue, .isReset = true });
	toneValueToggle.setWantsKeyboardFocus(true);
	toneValueToggle.setExplicitFocusOrder(++tabOrder);

	noiseValueToggle.setup({ .parent = parent, .id = prefix + CPK::Tn::noiseValueOn, .title = Text::noiseValue, .isReset = true });
	noiseValueToggle.setWantsKeyboardFocus(true);
	noiseValueToggle.setExplicitFocusOrder(++tabOrder);

	noiseValueSlider.setup({ .parent = parent, .id = prefix + CPK::Tn::noiseValue, .title = Text::noiseValueKnob, .isReset = true });
	noiseValueSlider.setWantsKeyboardFocus(true);
	noiseValueSlider.setExplicitFocusOrder(++tabOrder);

	// TARGET の切り替えやプリセットの読み込みでも変わるので、状態の変化を受ける
	noiseValueToggle.watchToggle([this] { applyActive(); });

	applyActive();
}

void GuiComponentToneNoiseValue::rebind(const juce::String& prefix)
{
	toneValueToggle.rebind(prefix + CPK::Tn::toneValueOn);
	noiseValueToggle.rebind(prefix + CPK::Tn::noiseValueOn);
	noiseValueSlider.rebind(prefix + CPK::Tn::noiseValue);

	applyActive();
}

void GuiComponentToneNoiseValue::setVisibles(bool visible)
{
	toneValueToggle.setVisible(visible);
	noiseValueToggle.setVisible(visible);
	noiseValueSlider.setVisibleWithLabel(visible);
}

void GuiComponentToneNoiseValue::layoutComponent(juce::Rectangle<int>& rect)
{
	layoutMain({ .mainRect = rect, .component = &toneValueToggle });
	layoutMain({ .mainRect = rect, .component = &noiseValueToggle });
	layoutMain({ .mainRect = rect, .label = &noiseValueSlider.label, .component = &noiseValueSlider });
}

void GuiComponentToneNoiseValue::layoutComponentRow(juce::Rectangle<int>& rect)
{
	layoutRow({ .rowRect = rect, .component = &toneValueToggle });
	layoutRow({ .rowRect = rect, .component = &noiseValueToggle });
	layoutRow({ .rowRect = rect, .label = &noiseValueSlider.label, .component = &noiseValueSlider });
}

void GuiComponentToneNoiseValue::applyActive()
{
	const bool useValue = noiseValueToggle.getToggleState();

	noiseValueSlider.setEnabledWithLabel(useValue);

	if (noiseFreqSlider != nullptr) noiseFreqSlider->setEnabledWithLabel(!useValue);
}
