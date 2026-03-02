#include "GuiOpna.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/MmlKeys.h"
#include "../core/MmlValues.h"

#include "../fm/RegisterConverter.h"

#include "../gui/GuiHelpers.h"

void GuiOpna::setup()
{
    const juce::String code = PrKey::Prefix::opna;

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::bit, .title = GuiText::Group::Fm::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::rate, .title = GuiText::Group::Fm::rate, .items = rateItems, .isReset = true });

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    algFbCat.setup({ .parent = *this, .title = GuiText::Category::algFb });

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Group::Fm::alg, .items = opnaAlgItems, .isReset = true });
    feedbackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb0, .title = GuiText::Group::Fm::fb0, .isReset = true });
    feedback2Slider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb2, .title = GuiText::Group::Fm::fb2, .isReset = true });

    lfoCat.setup({ .parent = *this, .title = GuiText::Category::lfo });

    lfoFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::freq, .title = GuiText::Group::Fm::lfoFreq, .isReset = true });
    lfoPmToggle.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pm, .title = GuiText::Group::Fm::pmEn, .isReset = true });
    lfoAmToggle.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::am, .title = GuiText::Group::Fm::amEn, .isReset = true });
    lfoPmsSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pms, .title = GuiText::Group::Fm::pms, .items = pmsItems, .isReset = true });
    lfoAmsSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::ams, .title = GuiText::Group::Fm::ams, .items = amsItems, .isReset = true });

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

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

        catMain[i].setup({ .parent = *this, .title = GuiText::Category::m });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mul, .title = GuiText::Group::Fm::Op::Mul, .isReset = true, .regType = RegisterType::FmMul });
        dt[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dt, .title = GuiText::Group::Fm::Op::Dt, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });

        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::tl, .title = GuiText::Group::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });
        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ar, .title = GuiText::Group::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        dr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dr, .title = GuiText::Group::Fm::Op::Dr, .isReset = true, .regType = RegisterType::FmDr });
        sl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sl, .title = GuiText::Group::Fm::Op::Sl, .isReset = true, .regType = RegisterType::FmSl });
        sr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sr, .title = GuiText::Group::Fm::Op::Sr, .isReset = true, .regType = RegisterType::FmSr });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rr, .title = GuiText::Group::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ks, .title = GuiText::Group::Fm::Op::Ks, .items = ksItems, .isReset = true });

        cafFix[i].setup({.parent = *this, .title = GuiText::Category::fix });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fix, .title = GuiText::Group::Fm::Op::Fix, .isReset = true });
        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fixFreq, .title = GuiText::Group::Fm::Op::FFreq, .isReset = true });
        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Group::Fm::Op::FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };
        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Group::Fm::Op::FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        catShape[i].setup({ .parent = *this, .title = GuiText::Category::shape });

        se[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::se, .title = GuiText::Group::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
        seFreq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::seFreq, .title = GuiText::Group::Fm::Op::SFreq, .isReset = true });

        catLfo[i].setup({ .parent = *this, .title = GuiText::Category::lfo });

        pms[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pms, .title = GuiText::Group::Fm::Op::Pms, .items = pmsItems, .isReset = true });
        pm[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::vib, .title = GuiText::Group::Fm::Op::PmEn, .isReset = true });
        ams[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ams, .title = GuiText::Group::Fm::Op::Ams, .items = amsItems, .isReset = true });
        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::am, .title = GuiText::Group::Fm::Op::AmEn, .isReset = true });

        catMask[i].setup({ .parent = *this, .title = GuiText::Category::mask });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mask, .title = GuiText::Group::Fm::Op::Mask, .isReset = true });

        catMml[i].setup({ .parent = *this, .title = GuiText::Category::mml });
    }
}

void GuiOpna::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &qualityCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &bitSelector.label, .component = &bitSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &rateSelector.label, .component = &rateSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &algFbCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &feedback2Slider.label, .component = &feedback2Slider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &lfoPmToggle });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &lfoPmsSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &lfoAmToggle });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &lfoAmsSelector, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

        opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(GuiValue::Fm::Op::Padding::width, GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMain[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &dt[i].label, .component = &dt[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ar[i].label, .component = &ar[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &dr[i].label, .component = &dr[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &sr[i].label, .component = &sr[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &sl[i].label, .component = &sl[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rr[i].label, .component = &rr[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &tl[i].label, .component = &tl[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catShape[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &se[i].label, .component = &se[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &seFreq[i].label, .component = &seFreq[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catLfo[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &pm[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &pms[i].label, .component = &pms[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &am[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ams[i].label, .component = &ams[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &cafFix[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &fix[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &freq[i].label, .component = &freq[i] });
        layoutComponentsLtoRFixFreqBtns({ .rect = innerRect, .to0Btn = &freqToZero[i], .to440Btn = &freqTo440[i], .paddingBottom = GuiValue::PaddingTop::mask });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMask[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &mask[i], .paddingBottom = GuiValue::PaddingTop::mmlBtn });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMml[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });
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
