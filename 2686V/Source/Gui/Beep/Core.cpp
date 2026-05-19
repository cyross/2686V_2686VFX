#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Beep/Keys.h"
#include "../../Processor/Beep/Values.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiValues.h"
#include "./GuiText.h"

// DT (デチューン1) 用のコンボボックスアイテム
// レジスタ仕様: 0=0, 1=+1, 2=+2, 3=+3, 4=0, 5=-1, 6=-2, 7=-3
static std::vector<SelectItem> dtItems = {
    {.name = " 0", .value = 1 },
    {.name = "-3", .value = 2 },
    {.name = "-2", .value = 3 },
    {.name = "-1", .value = 4 },
    {.name = " 0", .value = 5 }, // 実質0ですが、レジスタ4として一応用意
    {.name = "+1", .value = 6 },
    {.name = "+2", .value = 7 },
    {.name = "+3", .value = 8 }
};
void GuiBeep::setup() {
    juce::String code = BeepPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, BeepGuiText::Group::mainGroup); // GuiText 等に置換

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

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

    stl.setup({ .parent = *this, .id = code + BeepPrKey::Adsr::stl, .title = BeepGuiText::Beep::Adsr::Stl, .isReset = true });
    stl.setWantsKeyboardFocus(true);
    stl.setExplicitFocusOrder(++tabOrder);

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

    pitchAdsrCat.setup({ .parent = *this, .title = BeepGuiText::Category::visiblePitchAdsr, .invisibleTitle = BeepGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

    pitchAdsrBypassButton.setup({ .parent = *this, .id = code + BeepPrKey::pitchAdsr + BeepPrKey::bypass, .title = BeepGuiText::PitchAdsr::bypass, .isReset = true });
    pitchAdsrBypassButton.setWantsKeyboardFocus(true);
    pitchAdsrBypassButton.setExplicitFocusOrder(++tabOrder);

    pitchAttackSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::ar, .title = BeepGuiText::PitchAdsr::ar, .isReset = true });
    pitchAttackSlider.setWantsKeyboardFocus(true);
    pitchAttackSlider.setExplicitFocusOrder(++tabOrder);

    pitchDecaySlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::dr , .title = BeepGuiText::PitchAdsr::dr, .isReset = true });
    pitchDecaySlider.setWantsKeyboardFocus(true);
    pitchDecaySlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::rr, .title = BeepGuiText::PitchAdsr::rr, .isReset = true });
    pitchReleaseSlider.setWantsKeyboardFocus(true);
    pitchReleaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchStartLevelSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::stl, .title = BeepGuiText::PitchAdsr::stl, .isReset = true });
    pitchStartLevelSlider.setWantsKeyboardFocus(true);
    pitchStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchAttackLevelSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::atl, .title = BeepGuiText::PitchAdsr::atl, .isReset = true });
    pitchAttackLevelSlider.setWantsKeyboardFocus(true);
    pitchAttackLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchSustainLevelSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::ssl, .title = BeepGuiText::PitchAdsr::ssl, .isReset = true });
    pitchSustainLevelSlider.setWantsKeyboardFocus(true);
    pitchSustainLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseLevelSlider.setup({ .parent = *this, .id = code + BeepPrKey::PitchAdsr::rll, .title = BeepGuiText::PitchAdsr::rll, .isReset = true });
    pitchReleaseLevelSlider.setWantsKeyboardFocus(true);
    pitchReleaseLevelSlider.setExplicitFocusOrder(++tabOrder);

    detuneCat.setup({ .parent = *this, .title = BeepGuiText::Category::visibleDetune, .invisibleTitle = BeepGuiText::Category::invisibleDetune, .enableChangeDetailVisible = true });

    dt1.setup({ .parent = *this, .id = code + BeepPrKey::dt, .title = BeepGuiText::Detune::Dt1, .items = dtItems, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup({ .parent = *this, .id = code + BeepPrKey::dt2, .title = BeepGuiText::Detune::Dt2, .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

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

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    layoutMain({ .mainRect = mRect, .label = &volSlider.label, .component = &volSlider });

    layoutFixCat(mRect);

    layoutAdsrCat(mRect);

	layoutPitchEnvCat(mRect);

	layoutDetuneCat(mRect);

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
    stl.setVisibleWithLabel(visible);
    ar.setVisibleWithLabel(visible);
    dr.setVisibleWithLabel(visible);
    sl.setVisibleWithLabel(visible);
    rr.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &bypassToggle });
        layoutMain({ .mainRect = rect, .label = &stl.label, .component = &stl });
        layoutMain({ .mainRect = rect, .label = &ar.label, .component = &ar });
        layoutMain({ .mainRect = rect, .label = &dr.label, .component = &dr });
        layoutMain({ .mainRect = rect, .label = &sl.label, .component = &sl });
        layoutMain({ .mainRect = rect, .label = &rr.label, .component = &rr });
    }
}

void GuiBeep::layoutPitchEnvCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &pitchAdsrCat });

    bool visible = pitchAdsrCat.isDetailVisible();

    pitchAdsrBypassButton.setVisible(visible);
    pitchAttackSlider.setVisibleWithLabel(visible);
    pitchDecaySlider.setVisibleWithLabel(visible);
    pitchReleaseSlider.setVisibleWithLabel(visible);
    pitchStartLevelSlider.setVisibleWithLabel(visible);
    pitchAttackLevelSlider.setVisibleWithLabel(visible);
    pitchSustainLevelSlider.setVisibleWithLabel(visible);
    pitchReleaseLevelSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &pitchAdsrBypassButton });
        layoutMain({ .mainRect = rect, .label = &pitchAttackSlider.label, .component = &pitchAttackSlider });
        layoutMain({ .mainRect = rect, .label = &pitchDecaySlider.label, .component = &pitchDecaySlider });
        layoutMain({ .mainRect = rect, .label = &pitchReleaseSlider.label, .component = &pitchReleaseSlider });
        layoutMain({ .mainRect = rect, .label = &pitchStartLevelSlider.label, .component = &pitchStartLevelSlider });
        layoutMain({ .mainRect = rect, .label = &pitchAttackLevelSlider.label, .component = &pitchAttackLevelSlider });
        layoutMain({ .mainRect = rect, .label = &pitchSustainLevelSlider.label, .component = &pitchSustainLevelSlider });
        layoutMain({ .mainRect = rect, .label = &pitchReleaseLevelSlider.label, .component = &pitchReleaseLevelSlider });
    }
}

void GuiBeep::layoutDetuneCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &detuneCat });

    bool visible = detuneCat.isDetailVisible();

    dt1.setVisibleWithLabel(visible);
    dt2.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &dt1.label, .component = &dt1 });
        layoutMain({ .mainRect = rect, .label = &dt2.label, .component = &dt2 });
    }
}
