#include "./Fix.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

std::array<juce::String, 128> GuiComponentFix::noteLabelText{
    "C-2", "C#-2", "D-2", "D#-2", "E-2", "F-2", "F#-2", "G-2", "G#-2", "A-2", "A#-2", "B-2",
    "C-1", "C#-1", "D-1", "D#-1", "E-1", "F-1", "F#-1", "G-1", "G#-1", "A-1", "A#-1", "B-1",
    "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
    "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
    "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
    "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
    "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
    "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
    "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
    "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
    "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8"
};

void GuiComponentFix::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder, const juce::String& toLabel, int toValue)
{
    cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- FIX ---",
        .invisibleTitle = juce::String("") + "[□]--- FIX ---",
        .enableChangeDetailVisible = true
    });

    enable.setup({ .parent = parent, .id = code + "_FIX", .title = "Enable", .isReset = true });
    enable.setWantsKeyboardFocus(true);
    enable.setExplicitFocusOrder(++tabOrder);

    freq.setup({ .parent = parent, .id = code + "_FREQ", .title = "Freq", .isReset = true});
    freq.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    freq.setWantsKeyboardFocus(true);
    freq.setExplicitFocusOrder(++tabOrder);
    freq.setValue(toValue);

    parent.addAndMakeVisible(freqToSeparator);
    freqToSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    freqTo.setup({ .parent = parent, .title = toLabel, .isReset = false, .isResized = false });
    freqTo.setWantsKeyboardFocus(true);
    freqTo.setExplicitFocusOrder(++tabOrder);
    freqTo.onClick = [this, toValue] { freq.setValue(toValue, juce::sendNotification); };

    freqToZero.setup(GuiTextButton::Config{ .parent = parent, .title = "->0", .isReset = false, .isResized = false});
    freqToZero.setWantsKeyboardFocus(true);
    freqToZero.setExplicitFocusOrder(++tabOrder);
    freqToZero.onClick = [this] { freq.setValue(0, juce::sendNotification); };

    freqTo05.setup(GuiTextButton::Config{ .parent = parent, .title = "->0.5", .isReset = false, .isResized = false });
    freqTo05.setWantsKeyboardFocus(true);
    freqTo05.setExplicitFocusOrder(++tabOrder);
    freqTo05.onClick = [this] { freq.setValue(0.5, juce::sendNotification); };

    freqTo1.setup(GuiTextButton::Config{ .parent = parent, .title = "->1", .isReset = false, .isResized = false });
    freqTo1.setWantsKeyboardFocus(true);
    freqTo1.setExplicitFocusOrder(++tabOrder);
    freqTo1.onClick = [this] { freq.setValue(1, juce::sendNotification); };

    freqTo2.setup(GuiTextButton::Config{ .parent = parent, .title = "->2", .isReset = false, .isResized = false });
    freqTo2.setWantsKeyboardFocus(true);
    freqTo2.setExplicitFocusOrder(++tabOrder);
    freqTo2.onClick = [this] { freq.setValue(2, juce::sendNotification); };

    parent.addAndMakeVisible(freqAddSeparator);
    freqAddSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    freqM05.setup(GuiTextButton::Config{ .parent = parent, .title = "-0.5", .isReset = false, .isResized = false });
    freqM05.setWantsKeyboardFocus(true);
    freqM05.setExplicitFocusOrder(++tabOrder);
    freqM05.onClick = [this] { freq.setValue(freq.getValue() - 0.5f, juce::sendNotification); };

    freqM02.setup(GuiTextButton::Config{ .parent = parent, .title = "-0.2", .isReset = false, .isResized = false });
    freqM02.setWantsKeyboardFocus(true);
    freqM02.setExplicitFocusOrder(++tabOrder);
    freqM02.onClick = [this] { freq.setValue(freq.getValue() - 0.2f, juce::sendNotification); };

    freqM01.setup(GuiTextButton::Config{ .parent = parent, .title = "-0.1", .isReset = false, .isResized = false });
    freqM01.setWantsKeyboardFocus(true);
    freqM01.setExplicitFocusOrder(++tabOrder);
    freqM01.onClick = [this] { freq.setValue(freq.getValue() - 0.1f, juce::sendNotification); };

    freqP01.setup(GuiTextButton::Config{ .parent = parent, .title = "+0.1", .isReset = false, .isResized = false });
    freqP01.setWantsKeyboardFocus(true);
    freqP01.setExplicitFocusOrder(++tabOrder);
    freqP01.onClick = [this] { freq.setValue(freq.getValue() + 0.1f, juce::sendNotification); };

    freqP02.setup(GuiTextButton::Config{ .parent = parent, .title = "+0.2", .isReset = false, .isResized = false });
    freqP02.setWantsKeyboardFocus(true);
    freqP02.setExplicitFocusOrder(++tabOrder);
    freqP02.onClick = [this] { freq.setValue(freq.getValue() + 0.2f, juce::sendNotification); };

    freqP05.setup(GuiTextButton::Config{ .parent = parent, .title = "+0.5", .isReset = false, .isResized = false });
    freqP05.setWantsKeyboardFocus(true);
    freqP05.setExplicitFocusOrder(++tabOrder);
    freqP05.onClick = [this] { freq.setValue(freq.getValue() + 0.5f, juce::sendNotification); };

    freqM5.setup(GuiTextButton::Config{ .parent = parent, .title = "-5", .isReset = false, .isResized = false });
    freqM5.setWantsKeyboardFocus(true);
    freqM5.setExplicitFocusOrder(++tabOrder);
    freqM5.onClick = [this] { freq.setValue(freq.getValue() - 5.0f, juce::sendNotification); };

    freqM2.setup(GuiTextButton::Config{ .parent = parent, .title = "-2", .isReset = false, .isResized = false });
    freqM2.setWantsKeyboardFocus(true);
    freqM2.setExplicitFocusOrder(++tabOrder);
    freqM2.onClick = [this] { freq.setValue(freq.getValue() - 2.0f, juce::sendNotification); };

    freqM1.setup(GuiTextButton::Config{ .parent = parent, .title = "-1", .isReset = false, .isResized = false });
    freqM1.setWantsKeyboardFocus(true);
    freqM1.setExplicitFocusOrder(++tabOrder);
    freqM1.onClick = [this] { freq.setValue(freq.getValue() - 1.0f, juce::sendNotification); };

    freqP1.setup(GuiTextButton::Config{ .parent = parent, .title = "+1", .isReset = false, .isResized = false });
    freqP1.setWantsKeyboardFocus(true);
    freqP1.setExplicitFocusOrder(++tabOrder);
    freqP1.onClick = [this] { freq.setValue(freq.getValue() + 1.0f, juce::sendNotification); };

    freqP2.setup(GuiTextButton::Config{ .parent = parent, .title = "+2", .isReset = false, .isResized = false });
    freqP2.setWantsKeyboardFocus(true);
    freqP2.setExplicitFocusOrder(++tabOrder);
    freqP2.onClick = [this] { freq.setValue(freq.getValue() + 2.0f, juce::sendNotification); };

    freqP5.setup(GuiTextButton::Config{ .parent = parent, .title = "+5", .isReset = false, .isResized = false });
    freqP5.setWantsKeyboardFocus(true);
    freqP5.setExplicitFocusOrder(++tabOrder);
    freqP5.onClick = [this] { freq.setValue(freq.getValue() + 5.0f, juce::sendNotification); };

    freqM50.setup(GuiTextButton::Config{ .parent = parent, .title = "-50", .isReset = false, .isResized = false });
    freqM50.setWantsKeyboardFocus(true);
    freqM50.setExplicitFocusOrder(++tabOrder);
    freqM50.onClick = [this] { freq.setValue(freq.getValue() - 50.0f, juce::sendNotification); };

    freqM20.setup(GuiTextButton::Config{ .parent = parent, .title = "-20", .isReset = false, .isResized = false });
    freqM20.setWantsKeyboardFocus(true);
    freqM20.setExplicitFocusOrder(++tabOrder);
    freqM20.onClick = [this] { freq.setValue(freq.getValue() - 20.0f, juce::sendNotification); };

    freqM10.setup(GuiTextButton::Config{ .parent = parent, .title = "-10", .isReset = false, .isResized = false });
    freqM10.setWantsKeyboardFocus(true);
    freqM10.setExplicitFocusOrder(++tabOrder);
    freqM10.onClick = [this] { freq.setValue(freq.getValue() - 10.0f, juce::sendNotification); };

    freqP10.setup(GuiTextButton::Config{ .parent = parent, .title = "+10", .isReset = false, .isResized = false });
    freqP10.setWantsKeyboardFocus(true);
    freqP10.setExplicitFocusOrder(++tabOrder);
    freqP10.onClick = [this] { freq.setValue(freq.getValue() + 10.0f, juce::sendNotification); };

    freqP20.setup(GuiTextButton::Config{ .parent = parent, .title = "+20", .isReset = false, .isResized = false });
    freqP20.setWantsKeyboardFocus(true);
    freqP20.setExplicitFocusOrder(++tabOrder);
    freqP20.onClick = [this] { freq.setValue(freq.getValue() + 20.0f, juce::sendNotification); };

    freqP50.setup(GuiTextButton::Config{ .parent = parent, .title = "+50", .isReset = false, .isResized = false });
    freqP50.setWantsKeyboardFocus(true);
    freqP50.setExplicitFocusOrder(++tabOrder);
    freqP50.onClick = [this] { freq.setValue(freq.getValue() + 50.0f, juce::sendNotification); };

    freqM500.setup(GuiTextButton::Config{ .parent = parent, .title = "-500", .isReset = false, .isResized = false });
    freqM500.setWantsKeyboardFocus(true);
    freqM500.setExplicitFocusOrder(++tabOrder);
    freqM500.onClick = [this] { freq.setValue(freq.getValue() - 500.0f, juce::sendNotification); };

    freqM200.setup(GuiTextButton::Config{ .parent = parent, .title = "-200", .isReset = false, .isResized = false });
    freqM200.setWantsKeyboardFocus(true);
    freqM200.setExplicitFocusOrder(++tabOrder);
    freqM200.onClick = [this] { freq.setValue(freq.getValue() - 200.0f, juce::sendNotification); };

    freqM100.setup(GuiTextButton::Config{ .parent = parent, .title = "-100", .isReset = false, .isResized = false });
    freqM100.setWantsKeyboardFocus(true);
    freqM100.setExplicitFocusOrder(++tabOrder);
    freqM100.onClick = [this] { freq.setValue(freq.getValue() - 100.0f, juce::sendNotification); };

    freqP100.setup(GuiTextButton::Config{ .parent = parent, .title = "+100", .isReset = false, .isResized = false });
    freqP100.setWantsKeyboardFocus(true);
    freqP100.setExplicitFocusOrder(++tabOrder);
    freqP100.onClick = [this] { freq.setValue(freq.getValue() + 100.0f, juce::sendNotification); };

    freqP200.setup(GuiTextButton::Config{ .parent = parent, .title = "+200", .isReset = false, .isResized = false });
    freqP200.setWantsKeyboardFocus(true);
    freqP200.setExplicitFocusOrder(++tabOrder);
    freqP200.onClick = [this] { freq.setValue(freq.getValue() + 200.0f, juce::sendNotification); };

    freqP500.setup(GuiTextButton::Config{ .parent = parent, .title = "+500", .isReset = false, .isResized = false });
    freqP500.setWantsKeyboardFocus(true);
    freqP500.setExplicitFocusOrder(++tabOrder);
    freqP500.onClick = [this] { freq.setValue(freq.getValue() + 500.0f, juce::sendNotification); };

    freqM1000.setup(GuiTextButton::Config{ .parent = parent, .title = "-1k", .isReset = false, .isResized = false });
    freqM1000.setWantsKeyboardFocus(true);
    freqM1000.setExplicitFocusOrder(++tabOrder);
    freqM1000.onClick = [this] { freq.setValue(freq.getValue() - 1000.0f, juce::sendNotification); };

    freqP1000.setup(GuiTextButton::Config{ .parent = parent, .title = "+1k", .isReset = false, .isResized = false });
    freqP1000.setWantsKeyboardFocus(true);
    freqP1000.setExplicitFocusOrder(++tabOrder);
    freqP1000.onClick = [this] { freq.setValue(freq.getValue() + 1000.0f, juce::sendNotification); };

    parent.addAndMakeVisible(freqNoteSeparator);
    freqNoteSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    freqNote.setup({ .parent = parent, .title = "Note", .isReset = false });
    freqNote.setWantsKeyboardFocus(true);
    freqNote.setExplicitFocusOrder(++tabOrder);
    freqNote.setRange(0.0, 127.0, 1.0);
    freqNote.setNumDecimalPlacesToDisplay(0);
    freqNote.setValue(60.0);
    freqNote.onValueChange = [this] {
        note.setText(noteLabelText[(int)freqNote.getValue()], juce::NotificationType::dontSendNotification);
        };

    note.setup({ .parent = parent, .title = "" });
    note.setText(noteLabelText[(int)freqNote.getValue()], juce::NotificationType::dontSendNotification);

    applyNote.setup(GuiTextButton::Config{ .parent = parent, .title = "Apply", .bgColor = juce::Colours::blue.darker(0.1f).withAlpha(0.2f), .isReset = false, .isResized = false});
    applyNote.setWantsKeyboardFocus(true);
    applyNote.setExplicitFocusOrder(++tabOrder);
    applyNote.onClick = [this] {
        freq.setValue(440.0 * std::pow(2.0, (freqNote.getValue() - 69.0) / 12.0));
        };

    applyToC3.setup(GuiTextButton::Config{ .parent = parent, .title = "-> C3", .isReset = false, .isResized = false });
    applyToC3.setWantsKeyboardFocus(true);
    applyToC3.setExplicitFocusOrder(++tabOrder);
    applyToC3.onClick = [this] {
        freqNote.setValue(60.0);
        };
}

void GuiComponentFix::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

	enable.setVisible(visible);
    freq.setVisibleWithLabel(visible);
    freqToSeparator.setVisible(visible);
	freqTo.setVisible(visible);
    freqToZero.setVisible(visible);
    freqTo05.setVisible(visible);
    freqTo1.setVisible(visible);
    freqTo2.setVisible(visible);
    freqAddSeparator.setVisible(visible);
    freqM05.setVisible(visible);
    freqM02.setVisible(visible);
    freqM01.setVisible(visible);
    freqP01.setVisible(visible);
    freqP02.setVisible(visible);
    freqP05.setVisible(visible);
    freqM5.setVisible(visible);
    freqM2.setVisible(visible);
    freqM1.setVisible(visible);
    freqP1.setVisible(visible);
    freqP2.setVisible(visible);
    freqP5.setVisible(visible);
    freqM50.setVisible(visible);
    freqM20.setVisible(visible);
    freqM10.setVisible(visible);
    freqP10.setVisible(visible);
    freqP20.setVisible(visible);
    freqP50.setVisible(visible);
    freqM500.setVisible(visible);
    freqM200.setVisible(visible);
    freqM100.setVisible(visible);
    freqP100.setVisible(visible);
    freqP200.setVisible(visible);
    freqP500.setVisible(visible);
    freqM1000.setVisible(visible);
    freqP1000.setVisible(visible);
    freqNoteSeparator.setVisible(visible);
    freqNote.setVisible(visible);
    note.setVisible(visible);
    applyNote.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &enable });
        layoutMain({ .mainRect = rect, .label = &freq.label, .component = &freq });
        layoutMain({ .mainRect = rect, .component = &freqToSeparator });
		layoutMain({ .mainRect = rect, .component = &freqTo });
        layoutMainFourComps({ .rect = rect, .comp1 = &freqToZero, .comp2 = &freqTo05, .comp3 = &freqTo1, .comp4 = &freqTo2 });
        layoutMain({ .mainRect = rect, .component = &freqAddSeparator });
        layoutMainSixComps({
            .rect = rect,
            .comp1 = &freqM05,
            .comp2 = &freqM02,
            .comp3 = &freqM01,
            .comp4 = &freqP01,
            .comp5 = &freqP02,
            .comp6 = &freqP05
            });
        layoutMainSixComps({
            .rect = rect,
            .comp1 = &freqM5,
            .comp2 = &freqM2,
            .comp3 = &freqM1,
            .comp4 = &freqP1,
            .comp5 = &freqP2,
            .comp6 = &freqP5
            });
        layoutMainSixComps({
            .rect = rect,
            .comp1 = &freqM50,
            .comp2 = &freqM20,
            .comp3 = &freqM10,
            .comp4 = &freqP10,
            .comp5 = &freqP20,
            .comp6 = &freqP50
            });
        layoutMainSixComps({
            .rect = rect,
            .comp1 = &freqM500,
            .comp2 = &freqM200,
            .comp3 = &freqM100,
            .comp4 = &freqP100,
            .comp5 = &freqP200,
            .comp6 = &freqP500
            });
        layoutMainTwoComps({
            .rect = rect,
            .comp1 = &freqM1000,
            .comp2 = &freqP1000,
            });
        layoutMain({ .mainRect = rect, .component = &freqNoteSeparator });
        layoutMain({ .mainRect = rect, .label = &freqNote.label, .component = &freqNote });
        layoutMainThreeComps({ .rect = rect, .comp1 = &note, .comp2 = &applyNote, .comp3 = &applyToC3 });
    }
}

void GuiComponentFix::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    enable.setVisible(visible);
    freq.setVisibleWithLabel(visible);
    freqToSeparator.setVisible(visible);
    freqTo.setVisible(visible);
    freqToZero.setVisible(visible);
    freqTo05.setVisible(visible);
    freqTo1.setVisible(visible);
    freqTo2.setVisible(visible);
    freqAddSeparator.setVisible(visible);
    freqM05.setVisible(visible);
    freqM02.setVisible(visible);
    freqM01.setVisible(visible);
    freqP01.setVisible(visible);
    freqP02.setVisible(visible);
    freqP05.setVisible(visible);
    freqM5.setVisible(visible);
    freqM2.setVisible(visible);
    freqM1.setVisible(visible);
    freqP1.setVisible(visible);
    freqP2.setVisible(visible);
    freqP5.setVisible(visible);
    freqM50.setVisible(visible);
    freqM20.setVisible(visible);
    freqM10.setVisible(visible);
    freqP10.setVisible(visible);
    freqP20.setVisible(visible);
    freqP50.setVisible(visible);
    freqM500.setVisible(visible);
    freqM200.setVisible(visible);
    freqM100.setVisible(visible);
    freqP100.setVisible(visible);
    freqP200.setVisible(visible);
    freqP500.setVisible(visible);
    freqM1000.setVisible(visible);
    freqP1000.setVisible(visible);
    freqNoteSeparator.setVisible(visible);
    freqNote.setVisible(visible);
    note.setVisible(visible);
    applyNote.setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &enable });
        layoutRow({ .rowRect = rect, .label = &freq.label, .component = &freq });
        layoutRow({ .rowRect = rect, .component = &freqToSeparator });
        layoutRow({ .rowRect = rect, .component = &freqTo });
        layoutRowFourComps({ .rect = rect, .comp1 = &freqToZero, .comp2 = &freqTo05, .comp3 = &freqTo1, .comp4 = &freqTo2 });
        layoutRow({ .rowRect = rect, .component = &freqAddSeparator });
        layoutRowSixComps({
            .rect = rect,
            .comp1 = &freqM05, 
            .comp2 = &freqM02,
            .comp3 = &freqM01,
            .comp4 = &freqP01,
            .comp5 = &freqP02,
            .comp6 = &freqP05
            });
        layoutRowSixComps({
            .rect = rect,
            .comp1 = &freqM5,
            .comp2 = &freqM2,
            .comp3 = &freqM1,
            .comp4 = &freqP1,
            .comp5 = &freqP2,
            .comp6 = &freqP5
            });
        layoutRowSixComps({
            .rect = rect,
            .comp1 = &freqM50,
            .comp2 = &freqM20,
            .comp3 = &freqM10,
            .comp4 = &freqP10,
            .comp5 = &freqP20,
            .comp6 = &freqP50
            });
        layoutRowSixComps({
            .rect = rect,
            .comp1 = &freqM500,
            .comp2 = &freqM200,
            .comp3 = &freqM100,
            .comp4 = &freqP100,
            .comp5 = &freqP200,
            .comp6 = &freqP500
            });
        layoutRowTwoComps({
            .rect = rect,
            .comp1 = &freqM1000,
            .comp2 = &freqP1000,
            });
        layoutRow({ .rowRect = rect, .component = &freqNoteSeparator });
        layoutRow({ .rowRect = rect, .label = &freqNote.label, .component = &freqNote });
        layoutRowThreeComps({ .rect = rect, .comp1 = &note, .comp2 = &applyNote, .comp3 = &applyToC3 });
    }
}

void GuiComponentFix::setEnables(bool enabled)
{
    enable.setEnabled(enabled);
    freq.setEnabled(enabled);
    freqTo.setEnabled(enabled);
    freqToZero.setEnabled(enabled);
    freqTo05.setEnabled(enabled);
    freqTo1.setEnabled(enabled);
    freqTo2.setEnabled(enabled);
    freqM05.setEnabled(enabled);
    freqM02.setEnabled(enabled);
    freqM01.setEnabled(enabled);
    freqP01.setEnabled(enabled);
    freqP02.setEnabled(enabled);
    freqP05.setEnabled(enabled);
    freqM5.setEnabled(enabled);
    freqM2.setEnabled(enabled);
    freqM1.setEnabled(enabled);
    freqP1.setEnabled(enabled);
    freqP2.setEnabled(enabled);
    freqP5.setEnabled(enabled);
    freqM50.setEnabled(enabled);
    freqM20.setEnabled(enabled);
    freqM10.setEnabled(enabled);
    freqP10.setEnabled(enabled);
    freqP20.setEnabled(enabled);
    freqP50.setEnabled(enabled);
    freqM500.setEnabled(enabled);
    freqM200.setEnabled(enabled);
    freqM100.setEnabled(enabled);
    freqP100.setEnabled(enabled);
    freqP200.setEnabled(enabled);
    freqP500.setEnabled(enabled);
    freqM1000.setEnabled(enabled);
    freqP1000.setEnabled(enabled);
    freqNote.setEnabled(enabled);
    note.setEnabled(enabled);
    applyNote.setEnabled(enabled);
}
