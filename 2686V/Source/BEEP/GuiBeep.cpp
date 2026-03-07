#include "GuiBeep.h"

#include "../core/PrKeys.h"
#include "../core/PrValues.h"

#include "../gui/GuiHelpers.h"
#include "../gui/GuiValues.h"
#include "../gui/GuiText.h"

void GuiBeep::setup() {
    juce::String code = PrKey::Prefix::beep;

    mainGroup.setup(*this, GuiText::Group::mainGroup); // GuiText 等に置換

    mainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    volSlider.setup({ .parent = *this, .id = code + PrKey::Post::Beep::level, .title = GuiText::Beep::Level, .isReset = true });

    catFix.setup({ .parent = *this, .title = GuiText::Category::fix });

    fixToggle.setup({ .parent = *this, .id = code + PrKey::Post::Beep::fix, .title = GuiText::Beep::Fix, .isReset = true });
    freqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Beep::fixFreq, .title = GuiText::Beep::FFreq, .isReset = true });

    freqTo2kBtn.setup({ .parent = *this, .title = GuiText::Beep::To2k, .isReset = false, .isResized = false });
    freqTo2kBtn.onClick = [this] { freqSlider.setValue(2000.0, juce::sendNotification); };

    adsrCat.setup({ .parent = *this, .title = GuiText::Category::adsr });

    bypassToggle.setup({ .parent = *this, .id = code + PrKey::Innder::adsr + PrKey::Post::bypass, .title = GuiText::Beep::Adsr::Bypass, .isReset = true });
    ar.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::ar, .title = GuiText::Beep::Adsr::Ar, .isReset = true });
    dr.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::dr, .title = GuiText::Beep::Adsr::Dr, .isReset = true });
    sl.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::sl, .title = GuiText::Beep::Adsr::Sl, .isReset = true });
    rr.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::rr, .title = GuiText::Beep::Adsr::Rr, .isReset = true });

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
}

void GuiBeep::layout(juce::Rectangle<int> content) {
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &volSlider.label, .component = &volSlider, .paddingBottom = GuiValue::Category::paddingBotton });

    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &catFix, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &fixToggle });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &freqSlider.label, .component = &freqSlider});
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &freqTo2kBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &adsrCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &bypassToggle });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &ar.label, .component = &ar });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &dr.label, .component = &dr });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &sl.label, .component = &sl });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &rr.label, .component = &rr, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0});
}
