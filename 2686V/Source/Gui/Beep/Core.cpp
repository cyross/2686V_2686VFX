#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Beep/Keys.h"
#include "../../Processor/Beep/Values.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiValues.h"
#include "./GuiText.h"

void GuiBeep::setup() {
    juce::String code = BeepPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, BeepGuiText::Group::mainGroup); // GuiText 等に置換

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    volSlider.setup({ .parent = *this, .id = code + BeepPrKey::level, .title = BeepGuiText::Beep::Level, .isReset = true });
    volSlider.setWantsKeyboardFocus(true);
    volSlider.setExplicitFocusOrder(++tabOrder);

    catFix.setup({ .parent = *this, .title = BeepGuiText::Category::visibleFix, .invisibleTitle = BeepGuiText::Category::invisibleFix, .enableChangeDetailVisible = true });

    fixToggle.setup({ .parent = *this, .id = code + BeepPrKey::fix, .title = BeepGuiText::Beep::Fix, .isReset = true });
    fixToggle.setWantsKeyboardFocus(true);
    fixToggle.setExplicitFocusOrder(++tabOrder);

    freqSlider.setup({ .parent = *this, .id = code + BeepPrKey::fixFreq, .title = BeepGuiText::Beep::FFreq, .isReset = true });
    freqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    freqSlider.setWantsKeyboardFocus(true);
    freqSlider.setExplicitFocusOrder(++tabOrder);

    freqTo2kBtn.setup({ .parent = *this, .title = BeepGuiText::Beep::To2k, .isReset = false, .isResized = false });
    freqTo2kBtn.setWantsKeyboardFocus(true);
    freqTo2kBtn.setExplicitFocusOrder(++tabOrder);

    freqTo2kBtn.onClick = [this] { freqSlider.setValue(2000.0, juce::sendNotification); };
    freqTo2kBtn.setWantsKeyboardFocus(true);
    freqTo2kBtn.setExplicitFocusOrder(++tabOrder);

    adsrCat.setup({ .parent = *this, .title = BeepGuiText::Category::visibleAdsr, .invisibleTitle = BeepGuiText::Category::invisibleAdsr, .enableChangeDetailVisible = true });

    bypassToggle.setup({ .parent = *this, .id = code + BeepPrKey::adsr + BeepPrKey::bypass, .title = BeepGuiText::Beep::Adsr::Bypass, .isReset = true });
    bypassToggle.setWantsKeyboardFocus(true);
    bypassToggle.setExplicitFocusOrder(++tabOrder);

    ar.setup({ .parent = *this, .id = code + BeepPrKey::Adsr::ar, .title = BeepGuiText::Beep::Adsr::Ar, .isReset = true });
    ar.setWantsKeyboardFocus(true);
    ar.setExplicitFocusOrder(++tabOrder);

    dr.setup({ .parent = *this, .id = code + BeepPrKey::Adsr::dr, .title = BeepGuiText::Beep::Adsr::Dr, .isReset = true });
    dr.setWantsKeyboardFocus(true);
    dr.setExplicitFocusOrder(++tabOrder);

    sl.setup({ .parent = *this, .id = code + BeepPrKey::Adsr::sl, .title = BeepGuiText::Beep::Adsr::Sl, .isReset = true });
    sl.setWantsKeyboardFocus(true);
    sl.setExplicitFocusOrder(++tabOrder);

    rr.setup({ .parent = *this, .id = code + BeepPrKey::Adsr::rr, .title = BeepGuiText::Beep::Adsr::Rr, .isReset = true });
    rr.setWantsKeyboardFocus(true);
    rr.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setup({ .parent = *this, .title = BeepGuiText::Category::visibleMonoMode, .invisibleTitle = BeepGuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });

    monoModeToggle.setup({ .parent = *this, .id = BeepPrKey::monoMode, .title = BeepGuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = BeepGuiText::Category::visibleMvol, .invisibleTitle = BeepGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

    masterVolSlider.setup({ .parent = *this, .id = BeepPrKey::masterVol, .title = BeepGuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);
}

void GuiBeep::layout(juce::Rectangle<int> content) {
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(BeepGuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(BeepGuiValue::Group::Padding::width, BeepGuiValue::Group::Padding::height);
    mRect.removeFromTop(BeepGuiValue::Group::TitlePaddingTop);

    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = BeepGuiValue::PresetName::paddingBottom });
    layoutMain({ .mainRect = mRect, .label = &volSlider.label, .component = &volSlider });

    layoutFixCat(mRect);

    layoutAdsrCat(mRect);

    layoutMonoModeCat(mRect);

    layoutMvolCat(mRect);
}

void GuiBeep::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

void GuiBeep::initParams()
{
    this->ctx.audioProcessor.initParams("BEEP_");
}

void GuiBeep::layoutFixCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &catFix });

    bool visibleFix = catFix.isDetailVisible();

    fixToggle.setVisible(visibleFix);
    freqSlider.setVisibleWithLabel(visibleFix);
    freqTo2kBtn.setVisible(visibleFix);

    if (visibleFix)
    {
        layoutMain({ .mainRect = rect, .component = &fixToggle });
        layoutMain({ .mainRect = rect, .label = &freqSlider.label, .component = &freqSlider });
        layoutMain({ .mainRect = rect, .component = &freqTo2kBtn, });
    }
}

void GuiBeep::layoutMonoModeCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &monoPolyCat });

    bool visible = monoPolyCat.isDetailVisible();

    monoModeToggle.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoModeToggle });
    }
}

void GuiBeep::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
    }
}

void GuiBeep::layoutAdsrCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &adsrCat });

    bool visible = adsrCat.isDetailVisible();

    bypassToggle.setVisible(visible);
    ar.setVisibleWithLabel(visible);
    dr.setVisibleWithLabel(visible);
    sl.setVisibleWithLabel(visible);
    rr.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &bypassToggle });
        layoutMain({ .mainRect = rect, .label = &ar.label, .component = &ar });
        layoutMain({ .mainRect = rect, .label = &dr.label, .component = &dr });
        layoutMain({ .mainRect = rect, .label = &sl.label, .component = &sl });
        layoutMain({ .mainRect = rect, .label = &rr.label, .component = &rr });
    }
}
