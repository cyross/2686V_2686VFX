#include "./Midi.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentMidi::setupComponent(juce::Component& parent, int &tabOrder)
{
    cat.setupOtherCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- MIDI ---",
        .invisibleTitle = juce::String("") + "[□]--- MIDI ---",
        .enableChangeDetailVisible = true
        });

    monoMode.setup({ .parent = parent, .id = CPK::Midi::monoMode, .title = "MonoPhonic", .isReset = true });
    monoMode.setWantsKeyboardFocus(true);
    monoMode.setExplicitFocusOrder(++tabOrder);

	separator1.setupComponent(parent);

    useVelocity.setup({ .parent = parent, .id = CPK::Midi::useVelocity, .title = "Use Velocity", .isReset = true });
    useVelocity.setWantsKeyboardFocus(true);
    useVelocity.setExplicitFocusOrder(++tabOrder);

    fixedVelocity.setup({ .parent = parent, .id = CPK::Midi::fixedVelocity, .title = "Fixed Velocity", .isReset = true });
    fixedVelocity.setWantsKeyboardFocus(true);
    fixedVelocity.setExplicitFocusOrder(++tabOrder);

	separator2.setupComponent(parent);

    pitchResetOnLegato.setup({ .parent = parent, .id = CPK::Midi::pitchResetOnLegato, .title = "PitchEnv Reset On Legato", .isReset = true });
    pitchResetOnLegato.setWantsKeyboardFocus(true);
    pitchResetOnLegato.setExplicitFocusOrder(++tabOrder);

	resetSeparator.setupComponent(parent);

    monoButton.setup(GuiTextButton::Config{
        .parent = parent,
        .title = "-> Monophonic",
        .textColor = juce::Colours::black,
        .bgColor = juce::Colours::yellow.darker(0.5f),
        .borderColor = juce::Colours::yellow,
        .isReset = false
        });
    monoButton.setWantsKeyboardFocus(true);
    monoButton.setExplicitFocusOrder(++tabOrder);
    monoButton.onClick = [this] {
        monoMode.setToggleState(true, juce::NotificationType::sendNotification);
        useVelocity.setToggleState(true, juce::NotificationType::sendNotification);
        fixedVelocity.setValue(1.0f, juce::NotificationType::sendNotification);
        pitchResetOnLegato.setToggleState(true, juce::NotificationType::sendNotification);
        };

    polyButton.setup(GuiTextButton::Config{
        .parent = parent,
        .title = "-> Polyphonic",
        .textColor = juce::Colours::black,
        .bgColor = juce::Colours::orange.darker(0.5f),
        .borderColor = juce::Colours::orange,
        .isReset = false
        });
    polyButton.setWantsKeyboardFocus(true);
    polyButton.setExplicitFocusOrder(++tabOrder);
    polyButton.onClick = [this] {
        monoMode.setToggleState(false, juce::NotificationType::sendNotification);
        useVelocity.setToggleState(false, juce::NotificationType::sendNotification);
        fixedVelocity.setValue(0.5f, juce::NotificationType::sendNotification);
        pitchResetOnLegato.setToggleState(false, juce::NotificationType::sendNotification);
        };
}

void GuiComponentMidi::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

	monoMode.setVisible(visible);
	separator1.setVisible(visible);
    useVelocity.setVisible(visible);
    fixedVelocity.setVisibleWithLabel(visible);
	separator2.setVisible(visible);
    pitchResetOnLegato.setVisible(visible);
    resetSeparator.setVisible(visible);
    monoButton.setVisible(visible);
    polyButton.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoMode });
        
		separator1.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &useVelocity });
        layoutMain({ .mainRect = rect, .label = &fixedVelocity.label, .component = &fixedVelocity });

		separator2.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &pitchResetOnLegato });

		resetSeparator.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &monoButton });
        layoutMain({ .mainRect = rect, .component = &polyButton });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentMidi::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    monoMode.setVisible(visible);
    useVelocity.setVisible(visible);
    fixedVelocity.setVisibleWithLabel(visible);
    pitchResetOnLegato.setVisible(visible);
    resetSeparator.setVisible(visible);
    monoButton.setVisible(visible);
    polyButton.setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &monoMode });
        layoutRow({ .rowRect = rect, .component = &useVelocity });
        layoutRow({ .rowRect = rect, .label = &fixedVelocity.label, .component = &fixedVelocity });
        layoutRow({ .rowRect = rect, .component = &pitchResetOnLegato });

        resetSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &monoButton });
        layoutRow({ .rowRect = rect, .component = &polyButton });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentMidi::setEnables(bool enabled)
{
    monoMode.setEnabled(enabled);
    useVelocity.setEnabled(enabled);
    fixedVelocity.setEnabled(enabled);
    fixedVelocity.label.setEnabled(enabled);
    pitchResetOnLegato.setEnabled(enabled);
    monoButton.setEnabled(enabled);
    polyButton.setEnabled(enabled);
}
