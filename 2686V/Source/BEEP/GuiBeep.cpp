#include "GuiBeep.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"

void GuiBeep::setup() {
    juce::String code = PrKey::Prefix::beep;

    mainGroup.setup(*this, GuiText::Group::mainGroup); // GuiText 等に置換
    volSlider.setup({ .parent = *this, .id = code + PrKey::Post::Beep::level, .title = GuiText::Group::Beep::Post::level, .isReset = true });
    bypassToggle.setup({ .parent = *this, .id = code + PrKey::Innder::adsr + PrKey::Post::bypass, .title = "Bypass ADSR", .isReset = true });
    ar.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::ar, .title = GuiText::Group::Beep::Post::Adsr::ar, .isReset = true });
    dr.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::dr, .title = GuiText::Group::Beep::Post::Adsr::dr, .isReset = true });
    sl.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::sl, .title = GuiText::Group::Beep::Post::Adsr::sl, .isReset = true });
    rr.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::rr, .title = GuiText::Group::Beep::Post::Adsr::rr, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
}

void GuiBeep::layout(juce::Rectangle<int> content) {
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);
    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &volSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &volSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &bypassToggle, { GuiValue::MainGroup::Button::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &ar.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &ar, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &dr.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &dr, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &sl.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &sl, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &rr.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &rr, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::MainVol::height, 0, { { &masterVolSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &masterVolSlider, { GuiValue::MainGroup::Value::width, 0} } });
}
