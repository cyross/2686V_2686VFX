#include "GuiBeep.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"

void GuiBeep::setup() {
    juce::String code = PrKey::Prefix::beep;

    mainGroup.setup(*this, GuiText::Group::mainGroup); // GuiText 等に置換
    volSlider.setup({ .parent = *this, .id = code + PrKey::Post::Beep::level, .title = GuiText::Group::Beep::Level, .isReset = true });

    fixToggle.setup({ .parent = *this, .id = code + PrKey::Post::Beep::fix, .title = GuiText::Group::Beep::Fix, .isReset = true });
    freqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Beep::fixFreq, .title = GuiText::Group::Beep::FFreq, .isReset = true });

    freqTo2kBtn.setup({ .parent = *this, .title = GuiText::Group::Beep::To2k, .isReset = false, .isResized = false });
    freqTo2kBtn.onClick = [this] { freqSlider.setValue(2000.0, juce::sendNotification); };

    bypassToggle.setup({ .parent = *this, .id = code + PrKey::Innder::adsr + PrKey::Post::bypass, .title = GuiText::Group::Beep::Adsr::Bypass, .isReset = true });
    ar.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::ar, .title = GuiText::Group::Beep::Adsr::Ar, .isReset = true });
    dr.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::dr, .title = GuiText::Group::Beep::Adsr::Dr, .isReset = true });
    sl.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::sl, .title = GuiText::Group::Beep::Adsr::Sl, .isReset = true });
    rr.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::rr, .title = GuiText::Group::Beep::Adsr::Rr, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
}

void GuiBeep::layout(juce::Rectangle<int> content) {
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, {
        { &volSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} },
        { &volSlider, { GuiValue::MainGroup::Value::width, 0} } 
        });

    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &fixToggle, { GuiValue::MainGroup::Button::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, {
        { &freqSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} },
        { &freqSlider, { GuiValue::MainGroup::Value::width, 0} }
        });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, {
        { &freqTo2kBtn, {  GuiValue::MainGroup::Button::width, 0 } }
        });

    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &bypassToggle, { GuiValue::MainGroup::Button::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, {
        { &ar.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} },
        { &ar, { GuiValue::MainGroup::Value::width, 0} }
        });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, {
        { &dr.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} },
        { &dr, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, {
        { &sl.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} },
        { &sl, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, {
        { &rr.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} },
        { &rr, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::MainVol::height, 0, {
        { &masterVolSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} },
        { &masterVolSlider, { GuiValue::MainGroup::Value::width, 0} }
        });
}
