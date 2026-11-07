#include "./GuiScopeZoom.h"

#include "./GuiComponents.h"
#include "./GuiHost.h"
#include "./GuiI18n.h"

namespace
{
	const I18n::Text zoomLabel{ u8"縦の倍率", u8"V zoom" };

	constexpr int labelWidth = 64;
	constexpr int textBoxWidth = 44;
}

GuiScopeZoom::GuiScopeZoom(GuiProcessorHost& h) : host(h)
{
	label.setText(zoomLabel, juce::dontSendNotification);
	label.setFont(juce::Font(juce::FontOptions(12.0f)));
	label.setColour(juce::Label::textColourId, juce::Colours::white);
	label.setJustificationType(juce::Justification::centredLeft);
	addAndMakeVisible(label);

	slider.setRange(zoomMin, zoomMax, 0.01);
	slider.setSkewFactorFromMidPoint(1.0);
	slider.setNumDecimalPlacesToDisplay(2);
	slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, textBoxWidth, 16);
	slider.setDoubleClickReturnValue(true, 1.0);
	slider.setValue(currentZoom(), juce::dontSendNotification);
	slider.onValueChange = [this] { setZoom((float)slider.getValue()); };
	addAndMakeVisible(slider);
}

float GuiScopeZoom::currentZoom() const
{
	return clampZoom(host.scopeZoom);
}

void GuiScopeZoom::attach(std::initializer_list<GuiWaveformPreview*> list)
{
	scopes.assign(list.begin(), list.end());

	for (auto* scope : scopes) {
		scope->setVerticalZoom(currentZoom());
		scope->onVerticalZoomWheel = [this](float zoom) { setZoom(zoom); };
	}
}

void GuiScopeZoom::setZoom(float zoom)
{
	zoom = clampZoom(zoom);

	host.scopeZoom = zoom;

	slider.setValue(zoom, juce::dontSendNotification);

	for (auto* scope : scopes) scope->setVerticalZoom(zoom);
}

void GuiScopeZoom::resized()
{
	auto r = getLocalBounds();

	label.setBounds(r.removeFromLeft(labelWidth));
	slider.setBounds(r);
}
