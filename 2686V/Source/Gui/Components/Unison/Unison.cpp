#include "./Unison.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentUnison::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder)
{
    cat.setupOtherCategory({
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

    detuneMinus1200.setup({ .parent = parent, .id = "", .title = "-1200", .isReset = false });
    detuneMinus1200.setWantsKeyboardFocus(true);
    detuneMinus1200.setExplicitFocusOrder(++tabOrder);
	detuneMinus1200.onClick = [this] {
		detune.setValue(detune.getValue() - 1200);
		};

    detuneMinus100.setup({ .parent = parent, .id = "", .title = "-100", .isReset = false });
    detuneMinus100.setWantsKeyboardFocus(true);
    detuneMinus100.setExplicitFocusOrder(++tabOrder);
    detuneMinus100.onClick = [this] {
        detune.setValue(detune.getValue() - 100);
        };

    detune0.setup({ .parent = parent, .id = "", .title = "0", .isReset = false });
    detune0.setWantsKeyboardFocus(true);
    detune0.setExplicitFocusOrder(++tabOrder);
    detune0.onClick = [this] {
        detune.setValue(0);
        };

    detunePlus100.setup({ .parent = parent, .id = "", .title = "+100", .isReset = false });
    detunePlus100.setWantsKeyboardFocus(true);
    detunePlus100.setExplicitFocusOrder(++tabOrder);
    detunePlus100.onClick = [this] {
        detune.setValue(detune.getValue() + 100);
        };

    detunePlus120.setup({ .parent = parent, .id = "", .title = "+120", .isReset = false });
    detunePlus120.setWantsKeyboardFocus(true);
    detunePlus120.setExplicitFocusOrder(++tabOrder);
    detunePlus120.onClick = [this] {
        detune.setValue(detune.getValue() + 120);
        };

    spread.setup({ .parent = parent, .id = code + "_UNI_SPREAD", .title = "Spread", .isReset = true });
    spread.setWantsKeyboardFocus(true);
    spread.setExplicitFocusOrder(++tabOrder);
}

void GuiComponentUnison::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visibleUnison = cat.isDetailVisible();

    voices.setVisibleWithLabel(visibleUnison);
    detune.setVisibleWithLabel(visibleUnison);
	detuneMinus1200.setVisible(visibleUnison);
	detuneMinus100.setVisible(visibleUnison);
	detune0.setVisible(visibleUnison);
	detunePlus100.setVisible(visibleUnison);
	detunePlus120.setVisible(visibleUnison);
    spread.setVisibleWithLabel(visibleUnison);

    if (visibleUnison)
    {
        layoutMain({ .mainRect = rect, .label = &voices.label, .component = &voices });
        layoutMain({ .mainRect = rect, .label = &detune.label, .component = &detune });
        layoutMainFiveComps({
            .rect = rect,
            .comp1 = &detuneMinus1200,
            .comp2 = &detuneMinus100,
            .comp3 = &detune0,
            .comp4 = &detunePlus100,
            .comp5 = &detunePlus120
            });
        layoutMain({ .mainRect = rect, .label = &spread.label, .component = &spread });
    }
}
