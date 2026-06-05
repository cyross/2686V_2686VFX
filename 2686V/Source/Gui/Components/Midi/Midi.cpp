#include "./Midi.h"

#include "../../../Core/Processor/PluginProcessor.h"
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

    monoMode.setup({ .parent = parent, .id = "MONO_MODE", .title = "MonoPhonic", .isReset = true });
    monoMode.setWantsKeyboardFocus(true);
    monoMode.setExplicitFocusOrder(++tabOrder);

    useVelocity.setup({ .parent = parent, .id = "USE_VELICITY", .title = "Use Velocity", .isReset = true });
    useVelocity.setWantsKeyboardFocus(true);
    useVelocity.setExplicitFocusOrder(++tabOrder);

    fixedVelocity.setup({ .parent = parent, .id = "FIXED_VELICITY", .title = "Fixed Velocity", .isReset = true });
    fixedVelocity.setWantsKeyboardFocus(true);
    fixedVelocity.setExplicitFocusOrder(++tabOrder);

    pitchResetOnLegato.setup({ .parent = parent, .id = "PITCH_RESET_LEGATO", .title = "PitchEnv Reset On Legato", .isReset = true });
    pitchResetOnLegato.setWantsKeyboardFocus(true);
    pitchResetOnLegato.setExplicitFocusOrder(++tabOrder);
}

void GuiComponentMidi::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

	monoMode.setVisible(visible);
    useVelocity.setVisible(visible);
    fixedVelocity.setVisibleWithLabel(visible);
    pitchResetOnLegato.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoMode });
        layoutMain({ .mainRect = rect, .component = &useVelocity });
        layoutMain({ .mainRect = rect, .label = &fixedVelocity.label, .component = &fixedVelocity });
        layoutMain({ .mainRect = rect, .component = &pitchResetOnLegato });
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

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &monoMode });
        layoutRow({ .rowRect = rect, .component = &useVelocity });
        layoutRow({ .rowRect = rect, .label = &fixedVelocity.label, .component = &fixedVelocity });
        layoutRow({ .rowRect = rect, .component = &pitchResetOnLegato });
    }
}

void GuiComponentMidi::setEnables(bool enabled)
{
    monoMode.setEnabled(enabled);
    useVelocity.setEnabled(enabled);
    fixedVelocity.setEnabled(enabled);
    fixedVelocity.label.setEnabled(enabled);
    pitchResetOnLegato.setEnabled(enabled);
}
