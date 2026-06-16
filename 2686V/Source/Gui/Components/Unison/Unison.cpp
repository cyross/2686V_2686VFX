#include "./Unison.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentUnison::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder)
{
    cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- UNISON/HARMONY ---",
        .invisibleTitle = juce::String("") + "[□]--- UNISON/HARMONY ---",
        .enableChangeDetailVisible = true
        });

    voices.setup({ .parent = parent, .id = code + "_UNI_VOICES", .title = "Voices", .isReset = true });
    voices.setWantsKeyboardFocus(true);
    voices.setExplicitFocusOrder(++tabOrder);

    detune.setup({ .parent = parent, .id = code + "_UNI_DETUNE", .title = "Detune", .isReset = true });
    detune.setWantsKeyboardFocus(true);
    detune.setExplicitFocusOrder(++tabOrder);

    detuneButtons.setupComponent(parent, detune, tabOrder);

    spread.setup({ .parent = parent, .id = code + "_UNI_SPREAD", .title = "Spread", .isReset = true });
    spread.setWantsKeyboardFocus(true);
    spread.setExplicitFocusOrder(++tabOrder);
}

void GuiComponentUnison::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    voices.setVisibleWithLabel(visible);
    detune.setVisibleWithLabel(visible);
    detuneButtons.setVisibles(visible);
    spread.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &voices.label, .component = &voices });
        layoutMain({ .mainRect = rect, .label = &detune.label, .component = &detune });
        detuneButtons.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &spread.label, .component = &spread });
    }
}
void GuiComponentUnison::copyParams(CopyUnison& copyObj) {
    copyObj.voices = voices.getValue();
    copyObj.detune = detune.getValue();
    copyObj.spread = spread.getValue();
}

void GuiComponentUnison::pasteParams(CopyUnison& copyObj) {
    voices.setValue(copyObj.voices, juce::sendNotification);
    detune.setValue(copyObj.detune, juce::sendNotification);
    spread.setValue(copyObj.spread, juce::sendNotification);
}
