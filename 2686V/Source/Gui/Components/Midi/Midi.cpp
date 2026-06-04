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
}

void GuiComponentMidi::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

	monoMode.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoMode });
    }
}

void GuiComponentMidi::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    monoMode.setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &monoMode });
    }
}

void GuiComponentMidi::setEnables(bool enabled)
{
    monoMode.setEnabled(enabled);
}
