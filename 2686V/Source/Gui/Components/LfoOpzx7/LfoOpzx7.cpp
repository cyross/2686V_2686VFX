#include "./LfoOpzx7.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Processor/ProcessorKeys.h"

static std::vector<SelectItem> lfoShapeItems = {
    {.name = "0: Sine",                .value = 1 },
    {.name = "1: Saw Up",              .value = 2 },
    {.name = "2: Saw Down",            .value = 3 },
    {.name = "3: Square",              .value = 4 },
    {.name = "4: Triangle",            .value = 5 },
    {.name = "5: Sample & Hold",       .value = 6 },
    {.name = "6: Saw Down & One Shot", .value = 7 },
    {.name = "7: Triangle & One Shot", .value = 8 },
};

void GuiComponentLfoOpzx7::setupComponent(
    juce::Component& parent,
    const juce::String& code,
    int& tabOrder
)
{
    cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- LFO ---",
        .invisibleTitle = juce::String("") + "[□]--- LFO ---",
        .enableChangeDetailVisible = true
        });

    pmLabel.setup({ .parent = parent, .title = "[PM]" });

    pmEnable.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::pm, .title = "Enable", .isReset = true });
    pmEnable.setWantsKeyboardFocus(true);
    pmEnable.setExplicitFocusOrder(++tabOrder);

    pmFreq.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::pmFreq, .title = "Freq", .isReset = true });
    pmFreq.setWantsKeyboardFocus(true);
    pmFreq.setExplicitFocusOrder(++tabOrder);

    pmSyncDelay.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::pmSyncDelay, .title = "S.Delay", .isReset = true });
    pmSyncDelay.setWantsKeyboardFocus(true);
    pmSyncDelay.setExplicitFocusOrder(++tabOrder);

    pmSDToZero.setup(GuiTextButton::Config{ .parent = parent, .title = "Async", .isReset = false, .isResized = false });
    pmSDToZero.setWantsKeyboardFocus(true);
    pmSDToZero.setExplicitFocusOrder(++tabOrder);
    pmSDToZero.onClick = [this] {
        pmSyncDelay.setValue(0.0f);
        };

    pmSDToOne.setup(GuiTextButton::Config{ .parent = parent, .title = "Sync", .isReset = false, .isResized = false });
    pmSDToOne.setWantsKeyboardFocus(true);
    pmSDToOne.setExplicitFocusOrder(++tabOrder);
    pmSDToOne.onClick = [this] {
        pmSyncDelay.setValue(1.0f);
        };

    pgShape.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::pgShape, .title = "Shape", .items = lfoShapeItems, .isReset = true });
    pgShape.setWantsKeyboardFocus(true);
    pgShape.setExplicitFocusOrder(++tabOrder);

    pms.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::pms, .title = "PMS", .isReset = true });
    pms.setWantsKeyboardFocus(true);
    pms.setExplicitFocusOrder(++tabOrder);

    pmd.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::pmd, .title = "PMD", .isReset = true });
    pmd.setWantsKeyboardFocus(true);
    pmd.setExplicitFocusOrder(++tabOrder);

    parent.addAndMakeVisible(pmAmSeparator);
    pmAmSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    amLabel.setup({ .parent = parent, .title = "[AM]" });

    amEnable.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::am, .title = "Enable", .isReset = true });
    amEnable.setWantsKeyboardFocus(true);
    amEnable.setExplicitFocusOrder(++tabOrder);

    amFreq.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::amFreq, .title = "Freq", .isReset = true });
    amFreq.setWantsKeyboardFocus(true);
    amFreq.setExplicitFocusOrder(++tabOrder);

    amSyncDelay.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::amSyncDelay, .title = "S.Delay", .isReset = true });
    amSyncDelay.setWantsKeyboardFocus(true);
    amSyncDelay.setExplicitFocusOrder(++tabOrder);

    amSDToZero.setup(GuiTextButton::Config{ .parent = parent, .title = "Async", .isReset = false, .isResized = false });
    amSDToZero.setWantsKeyboardFocus(true);
    amSDToZero.setExplicitFocusOrder(++tabOrder);
    amSDToZero.onClick = [this] {
        amSyncDelay.setValue(0.0f);
        };

    amSDToOne.setup(GuiTextButton::Config{ .parent = parent, .title = "Sync", .isReset = false, .isResized = false });
    amSDToOne.setWantsKeyboardFocus(true);
    amSDToOne.setExplicitFocusOrder(++tabOrder);
    amSDToOne.onClick = [this] {
        amSyncDelay.setValue(1.0f);
        };

    egShape.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::egShape, .title = "Shape", .items = lfoShapeItems, .isReset = true });
    egShape.setWantsKeyboardFocus(true);
    egShape.setExplicitFocusOrder(++tabOrder);

    amSmRt.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::amSmoothRatio, .title = "Sm.Rate", .isReset = true });
    amSmRt.setWantsKeyboardFocus(true);
    amSmRt.setExplicitFocusOrder(++tabOrder);

    ams.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::ams, .title = "AMS", .isReset = true });
    ams.setWantsKeyboardFocus(true);
    ams.setExplicitFocusOrder(++tabOrder);

    amd.setup({ .parent = parent, .id = code + CorePrKey::Post::Lfo::amd, .title = "AMD", .isReset = true });
    amd.setWantsKeyboardFocus(true);
    amd.setExplicitFocusOrder(++tabOrder);
}

void GuiComponentLfoOpzx7::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    pmLabel.setVisible(visible);
    pmEnable.setVisible(visible);
    pmFreq.setVisibleWithLabel(visible);
    pmSyncDelay.setVisibleWithLabel(visible);
    pmSDToZero.setVisible(visible);
    pmSDToOne.setVisible(visible);
    pgShape.setVisibleWithLabel(visible);
    pms.setVisibleWithLabel(visible);
    pmd.setVisibleWithLabel(visible);
    pmAmSeparator.setVisible(visible);
    amLabel.setVisible(visible);
    amEnable.setVisible(visible);
    amFreq.setVisibleWithLabel(visible);
    amSyncDelay.setVisibleWithLabel(visible);
    amSDToZero.setVisible(visible);
    amSDToOne.setVisible(visible);
    egShape.setVisibleWithLabel(visible);
    amSmRt.setVisibleWithLabel(visible);
    ams.setVisibleWithLabel(visible);
    amd.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &pmEnable, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .component = &pmLabel, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &pmFreq.label, .component = &pmFreq, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &pmSyncDelay.label, .component = &pmSyncDelay, .rowHeight = 12 });
        layoutMainTwoComps({ .rect = rect, .comp1 = &pmSDToZero, .comp2 = &pmSDToOne, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &pgShape.label, .component = &pgShape, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &pms.label, .component = &pms, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &pmd.label, .component = &pmd, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .component = &pmAmSeparator, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .component = &amLabel, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .component = &amEnable, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &amFreq.label, .component = &amFreq, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &amSyncDelay.label, .component = &amSyncDelay, .rowHeight = 12 });
        layoutMainTwoComps({ .rect = rect, .comp1 = &amSDToZero, .comp2 = &amSDToOne, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &egShape.label, .component = &egShape, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &amSmRt.label, .component = &amSmRt, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &ams.label, .component = &ams, .rowHeight = 12 });
        layoutMain({ .mainRect = rect, .label = &amd.label, .component = &amd, .rowHeight = 12 });
    }
}

void GuiComponentLfoOpzx7::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    pmLabel.setVisible(visible);
    pmEnable.setVisible(visible);
    pmFreq.setVisibleWithLabel(visible);
    pmSyncDelay.setVisibleWithLabel(visible);
    pmSDToZero.setVisible(visible);
    pmSDToOne.setVisible(visible);
    pgShape.setVisibleWithLabel(visible);
    pms.setVisibleWithLabel(visible);
    pmd.setVisibleWithLabel(visible);
    pmAmSeparator.setVisible(visible);
    amLabel.setVisible(visible);
    amEnable.setVisible(visible);
    amFreq.setVisibleWithLabel(visible);
    amSyncDelay.setVisibleWithLabel(visible);
    amSDToZero.setVisible(visible);
    amSDToOne.setVisible(visible);
    egShape.setVisibleWithLabel(visible);
    amSmRt.setVisibleWithLabel(visible);
    ams.setVisibleWithLabel(visible);
    amd.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &pmLabel, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .component = &pmEnable, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &pmFreq.label, .component = &pmFreq, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &pmSyncDelay.label, .component = &pmSyncDelay, .rowHeight = 12 });
        layoutRowTwoComps({ .rect = rect, .comp1 = &pmSDToZero, .comp2 = &pmSDToOne, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &pgShape.label, .component = &pgShape, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &pms.label, .component = &pms, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &pmd.label, .component = &pmd, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .component = &pmAmSeparator, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .component = &amLabel, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .component = &amEnable, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &amFreq.label, .component = &amFreq, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &amSyncDelay.label, .component = &amSyncDelay, .rowHeight = 12 });
        layoutRowTwoComps({ .rect = rect, .comp1 = &amSDToZero, .comp2 = &amSDToOne, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &egShape.label, .component = &egShape, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &amSmRt.label, .component = &amSmRt, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &ams.label, .component = &ams, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &amd.label, .component = &amd, .rowHeight = 12 });
    }
}

void GuiComponentLfoOpzx7::setEnabled(bool enabled) {
    cat.setEnabled(enabled);
    pmLabel.setEnabled(enabled);
    pmEnable.setEnabled(enabled);
    pmFreq.setEnabled(enabled);
    pmSyncDelay.setEnabled(enabled);
    pmSDToZero.setEnabled(enabled);
    pmSDToOne.setEnabled(enabled);
    pgShape.setEnabled(enabled);
    pms.setEnabled(enabled);
    pmd.setEnabled(enabled);
    pmAmSeparator.setEnabled(enabled);
    amLabel.setEnabled(enabled);
    amEnable.setEnabled(enabled);
    amFreq.setEnabled(enabled);
    amSyncDelay.setEnabled(enabled);
    amSDToZero.setEnabled(enabled);
    amSDToOne.setEnabled(enabled);
    egShape.setEnabled(enabled);
    amSmRt.setEnabled(enabled);
    ams.setEnabled(enabled);
    amd.setEnabled(enabled);
}

void GuiComponentLfoOpzx7::copyParams(CopyLfoOpzx7& copyObj) {
    copyObj.pmEnable = pmEnable.getToggleState();
    copyObj.pmFreq = pmFreq.getValue();
    copyObj.pmSyncDelay = pmSyncDelay.getValue();
    copyObj.pmIndex = pgShape.getSelectedId();
    copyObj.pms = pms.getValue();
    copyObj.pmd = pmd.getValue();
    copyObj.amEnable = amEnable.getToggleState();
    copyObj.amFreq = amFreq.getValue();
    copyObj.amSyncDelay = amSyncDelay.getValue();
    copyObj.amIndex = egShape.getSelectedId();
    copyObj.amSmoothRate = amSmRt.getValue();
    copyObj.ams = ams.getValue();
    copyObj.amd = amd.getValue();
}

void GuiComponentLfoOpzx7::pasteParams(CopyLfoOpzx7& copyObj) {
    pmEnable.setToggleState(copyObj.pmEnable, juce::sendNotification);
    pmFreq.setValue(copyObj.pmFreq, juce::sendNotification);
    pmSyncDelay.setValue(copyObj.pmSyncDelay, juce::sendNotification);
    pgShape.setSelectedId(copyObj.pmIndex, juce::sendNotification);
    pms.setValue(copyObj.pms, juce::sendNotification);
    pmd.setValue(copyObj.pmd, juce::sendNotification);
    amEnable.setToggleState(copyObj.amEnable, juce::sendNotification);
    amFreq.setValue(copyObj.amFreq, juce::sendNotification);
    amSyncDelay.setValue(copyObj.amSyncDelay, juce::sendNotification);
    egShape.setSelectedId(copyObj.amIndex, juce::sendNotification);
    amSmRt.setValue(copyObj.amSmoothRate, juce::sendNotification);
    ams.setValue(copyObj.ams, juce::sendNotification);
    amd.setValue(copyObj.amd, juce::sendNotification);
}
