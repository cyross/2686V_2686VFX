#include "GuiBeep.h"

#include "../processor/PluginProcessor.h"

#include "../core/PrKeys.h"
#include "../core/PrValues.h"

#include "../gui/GuiHelpers.h"
#include "../gui/GuiValues.h"
#include "../gui/GuiText.h"

void GuiBeep::setup() {
    juce::String code = PrKey::Prefix::beep;
    int tabOrder = 1;

    mainGroup.setup(*this, GuiText::Group::mainGroup); // GuiText 等に置換

    mainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    monoPolyCat.setup({ .parent = *this, .title = GuiText::Category::monoMode });
    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });

    volSlider.setup({ .parent = *this, .id = code + PrKey::Post::Beep::level, .title = GuiText::Beep::Level, .isReset = true });
    volSlider.setWantsKeyboardFocus(true);
    volSlider.setExplicitFocusOrder(++tabOrder);

    catFix.setup({ .parent = *this, .title = GuiText::Category::fix });

    fixToggle.setup({ .parent = *this, .id = code + PrKey::Post::Beep::fix, .title = GuiText::Beep::Fix, .isReset = true });
    fixToggle.setWantsKeyboardFocus(true);
    fixToggle.setExplicitFocusOrder(++tabOrder);

    freqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Beep::fixFreq, .title = GuiText::Beep::FFreq, .isReset = true });
    freqSlider.setWantsKeyboardFocus(true);
    freqSlider.setExplicitFocusOrder(++tabOrder);

    freqTo2kBtn.setup({ .parent = *this, .title = GuiText::Beep::To2k, .isReset = false, .isResized = false });
    freqTo2kBtn.setWantsKeyboardFocus(true);
    freqTo2kBtn.setExplicitFocusOrder(++tabOrder);

    freqTo2kBtn.onClick = [this] { freqSlider.setValue(2000.0, juce::sendNotification); };
    freqTo2kBtn.setWantsKeyboardFocus(true);
    freqTo2kBtn.setExplicitFocusOrder(++tabOrder);

    adsrCat.setup({ .parent = *this, .title = GuiText::Category::adsr });

    bypassToggle.setup({ .parent = *this, .id = code + PrKey::Innder::adsr + PrKey::Post::bypass, .title = GuiText::Beep::Adsr::Bypass, .isReset = true });
    bypassToggle.setWantsKeyboardFocus(true);
    bypassToggle.setExplicitFocusOrder(++tabOrder);

    ar.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::ar, .title = GuiText::Beep::Adsr::Ar, .isReset = true });
    ar.setWantsKeyboardFocus(true);
    ar.setExplicitFocusOrder(++tabOrder);

    dr.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::dr, .title = GuiText::Beep::Adsr::Dr, .isReset = true });
    dr.setWantsKeyboardFocus(true);
    dr.setExplicitFocusOrder(++tabOrder);

    sl.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::sl, .title = GuiText::Beep::Adsr::Sl, .isReset = true });
    sl.setWantsKeyboardFocus(true);
    sl.setExplicitFocusOrder(++tabOrder);

    rr.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::rr, .title = GuiText::Beep::Adsr::Rr, .isReset = true });
    rr.setWantsKeyboardFocus(true);
    rr.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    monoModeToggle.setup({ .parent = *this, .id = PrKey::monoMode, .title = GuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));
}

void GuiBeep::layout(juce::Rectangle<int> content) {
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMain({ .mainRect = mRect, .label = &presetNameCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = GuiValue::PresetName::paddingBottom });
    layoutMain({ .mainRect = mRect, .label = &mainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &volSlider.label, .component = &volSlider, .paddingBottom = GuiValue::Category::paddingBotton });

    layoutMain({ .mainRect = mRect, .label = &catFix, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .component = &fixToggle });
    layoutMain({ .mainRect = mRect, .label = &freqSlider.label, .component = &freqSlider});
    layoutMain({ .mainRect = mRect, .component = &freqTo2kBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &adsrCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .component = &bypassToggle });
    layoutMain({ .mainRect = mRect, .label = &ar.label, .component = &ar });
    layoutMain({ .mainRect = mRect, .label = &dr.label, .component = &dr });
    layoutMain({ .mainRect = mRect, .label = &sl.label, .component = &sl });
    layoutMain({ .mainRect = mRect, .label = &rr.label, .component = &rr, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &monoPolyCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .component = &monoModeToggle, .paddingBottom = 0 });
}

void GuiBeep::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}
