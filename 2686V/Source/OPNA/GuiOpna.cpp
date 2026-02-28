#include "GuiOpna.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/MmlKeys.h"
#include "../core/MmlValues.h"

#include "../fm/RegisterConverter.h"

void GuiOpna::setup()
{
    const juce::String code = PrKey::Prefix::opna;

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::bit, .title = GuiText::Group::Fm::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::rate, .title = GuiText::Group::Fm::rate, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Group::Fm::alg, .items = opnaAlgItems, .isReset = true });
    feedbackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb0, .title = GuiText::Group::Fm::fb0, .isReset = true });
    feedback2Slider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb2, .title = GuiText::Group::Fm::fb2, .isReset = true });

    lfoFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::freq, .title = GuiText::Group::Fm::lfoFreq, .isReset = true });
    lfoPmsSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pms, .title = GuiText::Group::Fm::pms, .items = pmsItems, .isReset = true });
    lfoAmsSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::ams, .title = GuiText::Group::Fm::ams, .items = amsItems, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    const juce::String opCode = code + PrKey::Innder::op;

    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, GuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

		mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 DR:0 SL:0 SR:0 RR:15 TL:0 MUL:1 DT:0",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
		});

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mul, .title = GuiText::Group::Fm::Op::Mul, .isReset = true, .regType = RegisterType::FmMul });
        dt[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dt, .title = GuiText::Group::Fm::Op::Dt, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });

        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::tl, .title = GuiText::Group::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });
        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ar, .title = GuiText::Group::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        dr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dr, .title = GuiText::Group::Fm::Op::Dr, .isReset = true, .regType = RegisterType::FmDr });
        sl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sl, .title = GuiText::Group::Fm::Op::Sl, .isReset = true, .regType = RegisterType::FmSl });
        sr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sr, .title = GuiText::Group::Fm::Op::Sr, .isReset = true, .regType = RegisterType::FmSr });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rr, .title = GuiText::Group::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ks, .title = GuiText::Group::Fm::Op::Ks, .items = ksItems, .isReset = true });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fix, .title = GuiText::Group::Fm::Op::Fix, .isReset = true });
        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fixFreq, .title = GuiText::Group::Fm::Op::FFreq, .isReset = true });
        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Group::Fm::Op::FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };
        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Group::Fm::Op::FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::am, .title = GuiText::Group::Fm::Op::Am, .isReset = true });

        se[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::se, .title = GuiText::Group::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
        seFreq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::seFreq, .title = GuiText::Group::Fm::Op::SFreq, .isReset = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mask, .title = GuiText::Group::Fm::Op::Mask, .isReset = true });
    }
}

void GuiOpna::layout(juce::Rectangle<int> content)
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
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &feedback2Slider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &feedback2Slider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &lfoFreqSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &lfoFreqSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &lfoPmsSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &lfoPmsSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &lfoAmsSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &lfoAmsSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::MainVol::height, 0, { { &masterVolSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &masterVolSlider, { GuiValue::MainGroup::Value::width, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

        opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(GuiValue::Fm::Op::Padding::width, GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &mul[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &mul[i], { GuiValue::Fm::Op::Row::Value::width, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &dt[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &dt[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &ar[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &ar[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &dr[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &dr[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &sr[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &sr[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &sl[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &sl[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rr[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rr[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &tl[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &tl[i], { GuiValue::Fm::Op::Row::Value::width, 0}} });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &ks[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &ks[i], { GuiValue::Fm::Op::Row::Value::width, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &se[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &se[i], { GuiValue::Fm::Op::Row::Value::width, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &seFreq[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &seFreq[i], { GuiValue::Fm::Op::Row::Value::width, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &fix[i], { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &freq[i].label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &freq[i], { GuiValue::Fm::Op::Row::Value::width, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &freqToZero[i], { GuiValue::Fm::Op::Row::Button::Freq::width, GuiValue::Fm::Op::Row::Padding::right} }, { &freqTo440[i], { GuiValue::Fm::Op::Row::Button::Freq::width, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &am[i], { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &mask[i], { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
        layoutComponentsLtoR(innerRect, GuiValue::Fm::Op::Row::height, 0, { { &mml[i], { GuiValue::Fm::Op::Row::Button::wdth, 0 } } });
    }
}


// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpna::applyMmlString(const juce::String& mml, int opIndex)
{
    juce::String input = mml.toUpperCase();
    int val;

    // MUL
    val = RegisterConverter::getValue(input, mmlPrefixMul, mmlValues::opna::mul);
    if (RegisterConverter::isValidVal(val)) mul[opIndex].setValue((double)RegisterConverter::convertFmMul(val), juce::sendNotification);

    // DT
    val = RegisterConverter::getValue(input, mmlPrefixDt, mmlValues::opna::dt);
    if (RegisterConverter::isValidVal(val)) {
        int regVal = RegisterConverter::convertMmlDtToReg(val);

        dt[opIndex].setSelectedItemIndex((double)regVal, juce::sendNotification);
    }

    // TL
    val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opna::tl);
    if (RegisterConverter::isValidVal(val)) tl[opIndex].setValue(RegisterConverter::convertFmTl(val), juce::sendNotification);

    // AR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRar, mmlValues::opna::ar);
    if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(mmlValues::opna::ar - val), juce::sendNotification);
    // AR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opna::ar);
        if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(val), juce::sendNotification);
    }

    // DR
    val = RegisterConverter::getValue(input, mmlPrefixRdr, mmlValues::opna::dr);
    if (RegisterConverter::isValidVal(val)) dr[opIndex].setValue(RegisterConverter::convertFmDr(mmlValues::opna::dr - val), juce::sendNotification);
    // DR
    else
    {
        val = RegisterConverter::getValue(input, mmlPrefixDr, mmlValues::opna::dr);
        if (RegisterConverter::isValidVal(val)) dr[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
    }

    // SR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRsr, mmlValues::opna::sr);
    if (RegisterConverter::isValidVal(val)) sr[opIndex].setValue(RegisterConverter::convertFmSr(mmlValues::opna::sr - val), juce::sendNotification);
    // SR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixSr, mmlValues::opna::sr);
        if (RegisterConverter::isValidVal(val)) sr[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
    }

    // SL
    val = RegisterConverter::getValue(input, mmlPrefixSl, mmlValues::opna::sl);
    if (RegisterConverter::isValidVal(val)) sl[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

    // RR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRrr, mmlValues::opna::rr);
    if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(mmlValues::opna::rr - val), juce::sendNotification);
    // RR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opna::rr);
        if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(val), juce::sendNotification);
    }
}
