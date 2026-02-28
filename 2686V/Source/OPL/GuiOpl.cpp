#include "GuiOpl.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/MmlKeys.h"
#include "../core/MmlValues.h"

#include "../fm/RegisterConverter.h"


void GuiOpl::setup()
{
    const juce::String code = PrKey::Prefix::opl;

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::bit, .title = GuiText::Group::Fm::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::rate, .title = GuiText::Group::Fm::rate, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Group::Fm::alg, .items = oplAlgItems, .isReset = true });
    feedbackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb0, .title = GuiText::Group::Fm::fb0, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    const juce::String opCode = code + PrKey::Innder::op;

    for (int i = 0; i < 2; ++i)
    {
        opGroups[i].setup(*this, GuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 DR:0 SL:0 RR:15 MUL:1 DT:0",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mul, .title = GuiText::Group::Fm::Op::Mul, .isReset = true, .regType = RegisterType::FmMul });
        dt[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dt, .title = GuiText::Group::Fm::Op::Dt, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::tl, .title = GuiText::Group::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ar, .title = GuiText::Group::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        dr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dr, .title = GuiText::Group::Fm::Op::Dr, .isReset = true, .regType = RegisterType::FmDr });
        sl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sl, .title = GuiText::Group::Fm::Op::Sl, .isReset = true, .regType = RegisterType::FmSl });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rr, .title = GuiText::Group::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });

        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::am, .title = GuiText::Group::Fm::Op::Am, .isReset = true });
        vib[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::vib, .title = GuiText::Group::Fm::Op::Vib, .isReset = true });
        egType[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::egType, .title = GuiText::Group::Fm::Op::EgType, .isReset = true });
        ksr[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ksr, .title = GuiText::Group::Fm::Op::Ksr, .isReset = true });
        ksl[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ksl, .title = GuiText::Group::Fm::Op::Ksl, .items = kslItems, .isReset = true });

        eg[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::eg, .title = GuiText::Group::Fm::Op::Eg, .items = oplEgItems, .isReset = true });
        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mask, .title = GuiText::Group::Fm::Op::Mask, .isReset = true });
    }
}

void GuiOpl::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &bitSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &bitSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &rateSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &rateSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &algSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &algSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &feedbackSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &feedbackSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::MainVol::height, 0, { { &masterVolSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &masterVolSlider, { GuiValue::MainGroup::Value::width, 0} } });

    // --- B. Operators Section ---
    for (int i = 0; i < 2; ++i)
    {
        auto opArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);
        opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(GuiValue::Fm::Op::Padding::width, GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &mul[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &mul[i], { GuiValue::Fm::Op::Row::Value::width, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &dt[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &dt[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &ar[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &ar[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &dr[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &dr[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &sl[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &sl[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rr[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rr[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &tl[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &tl[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &am[i], { GuiValue::Fm::Op::Row::Button::wdth, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &vib[i], { GuiValue::Fm::Op::Row::Button::wdth, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &egType[i], { GuiValue::Fm::Op::Row::Button::wdth, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &ksr[i], { GuiValue::Fm::Op::Row::Button::wdth, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &ksl[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &ksl[i], { GuiValue::Fm::Op::Row::Value::width, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &eg[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &eg[i], { GuiValue::Fm::Op::Row::Value::width, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &mask[i], { GuiValue::Fm::Op::Row::Button::wdth, GuiValue::Fm::Op::Row::Padding::right}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, 0, { { &mml[i], { GuiValue::Fm::Op::Row::Button::wdth, 0}} });
    }
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpl::applyMmlString(const juce::String& mml, int opIndex)
{
    juce::String input = mml.toUpperCase();
    int val;

    // MUL
    val = RegisterConverter::getValue(input, mmlPrefixMul, mmlValues::opl::mul);
    if (RegisterConverter::isValidVal(val)) mul[opIndex].setValue((double)RegisterConverter::convertOplMul(val), juce::sendNotification);

    // DT
    val = RegisterConverter::getValue(input, mmlPrefixDt, mmlValues::opn::dt);
    if (RegisterConverter::isValidVal(val)) {
        int regVal = RegisterConverter::convertMmlDtToReg(val);

        dt[opIndex].setSelectedItemIndex((double)regVal, juce::sendNotification);
    }

    // TL
    val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opl::tl);
    if (RegisterConverter::isValidVal(val)) tl[opIndex].setValue(RegisterConverter::convertOplTl(val), juce::sendNotification);

    // AR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRar, mmlValues::opl::ar);
    if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertOplAr(mmlValues::opl::ar - val), juce::sendNotification);
    // AR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opl::ar);
        if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertOplAr(val), juce::sendNotification);
    }

    // DR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRdr, mmlValues::opl::dr);
    if (RegisterConverter::isValidVal(val)) dr[opIndex].setValue(RegisterConverter::convertOplDr(mmlValues::opl::dr - val), juce::sendNotification);
    // DR
    else
    {
        val = RegisterConverter::getValue(input, mmlPrefixDr, mmlValues::opl::dr);
        if (RegisterConverter::isValidVal(val)) dr[opIndex].setValue(RegisterConverter::convertOplDr(val), juce::sendNotification);
    }

    // SL
    val = RegisterConverter::getValue(input, mmlPrefixSl, mmlValues::opl::sl);
    if (RegisterConverter::isValidVal(val)) sl[opIndex].setValue(RegisterConverter::convertOplSl(val), juce::sendNotification);

    // RR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRrr, mmlValues::opl::rr);
    if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertOplRr(mmlValues::opl::rr - val), juce::sendNotification);
    // RR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opl::rr);
        if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertOplRr(val), juce::sendNotification);
    }
}
