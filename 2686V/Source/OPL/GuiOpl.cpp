#include "GuiOpl.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/MmlKeys.h"
#include "../core/MmlValues.h"

#include "../fm/RegisterConverter.h"

#include "../gui/GuiHelpers.h"

void GuiOpl::setup()
{
    const juce::String code = PrKey::Prefix::opl;

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    algFbCat.setup({ .parent = *this, .title = GuiText::Category::algFb });

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Fm::alg, .items = oplAlgItems, .isReset = true });
    feedbackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb0, .title = GuiText::Fm::fb0, .isReset = true });

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

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

        catMain[i].setup({ .parent = *this, .title = GuiText::Category::m });
        catLfo[i].setup({ .parent = *this, .title = GuiText::Category::lfo });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mul, .title = GuiText::Fm::Op::Mul, .isReset = true, .regType = RegisterType::FmMul });
        dt[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dt, .title = GuiText::Fm::Op::Dt, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::tl, .title = GuiText::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ar, .title = GuiText::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        dr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dr, .title = GuiText::Fm::Op::Dr, .isReset = true, .regType = RegisterType::FmDr });
        sl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sl, .title = GuiText::Fm::Op::Sl, .isReset = true, .regType = RegisterType::FmSl });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rr, .title = GuiText::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });

        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::am, .title = GuiText::Fm::Op::Am, .isReset = true });
        vib[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::vib, .title = GuiText::Fm::Op::Vib, .isReset = true });
        egType[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::egType, .title = GuiText::Fm::Op::EgType, .isReset = true });
        ksr[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ksr, .title = GuiText::Fm::Op::Ksr, .isReset = true });
        ksl[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ksl, .title = GuiText::Fm::Op::Ksl, .items = kslItems, .isReset = true });

        ams[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ams, .title = GuiText::Fm::Op::Ams, .isReset = true });
        amd[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::amd, .title = GuiText::Fm::Op::Amd, .isReset = true });
        pms[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pms, .title = GuiText::Fm::Op::Pms, .isReset = true });
        pmd[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pmd, .title = GuiText::Fm::Op::Pmd, .isReset = true });

        amsTo37[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->3.7Hz", .isReset = false, .isResized = false });
        amsTo37[i].onClick = [this, index = i] { ams[index].setValue(3.7, juce::sendNotification); };
        amdTo1[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->1dB", .isReset = false, .isResized = false});
        amdTo1[i].onClick = [this, index = i] { amd[index].setValue(1.0, juce::sendNotification); };
        amdTo48[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->4.8dB", .isReset = false, .isResized = false});
        amdTo48[i].onClick = [this, index = i] { amd[index].setValue(4.8, juce::sendNotification); };
        pmsTo64[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->6.4Hz", .isReset = false, .isResized = false});
        pmsTo64[i].onClick = [this, index = i] { pms[index].setValue(6.4, juce::sendNotification); };
        pmdTo7[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->7cent", .isReset = false, .isResized = false});
        pmdTo7[i].onClick = [this, index = i] { pmd[index].setValue(7.0, juce::sendNotification); };
        pmdTo14[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->14cent", .isReset = false, .isResized = false });
        pmdTo14[i].onClick = [this, index = i] { pmd[index].setValue(14.0, juce::sendNotification); };

        catShape[i].setup({ .parent = *this, .title = GuiText::Category::shape });

        eg[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::eg, .title = GuiText::Fm::Op::Eg, .items = oplEgItems, .isReset = true });

        catMask[i].setup({ .parent = *this, .title = GuiText::Category::mask });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mask, .title = GuiText::Fm::Op::Mask, .isReset = true });

        catMml[i].setup({ .parent = *this, .title = GuiText::Category::mml });
    }
}

void GuiOpl::layout(juce::Rectangle<int> content)
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
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

    // --- B. Operators Section ---
    for (int i = 0; i < 2; ++i)
    {
        auto opArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);
        opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(GuiValue::Fm::Op::Padding::width, GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMain[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &dt[i].label, .component = &dt[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ar[i].label, .component = &ar[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &dr[i].label, .component = &dr[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &sl[i].label, .component = &sl[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rr[i].label, .component = &rr[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &tl[i].label, .component = &tl[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &egType[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &ksr[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ksl[i].label, .component = &ksl[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catShape[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &eg[i].label, .component = &eg[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catLfo[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &am[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ams[i].label, .component = &ams[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &amsTo37[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &amd[i].label, .component = &amd[i] });
        layoutComponentsLtoRFixFreqBtns({ .rect = innerRect, .to0Btn = &amdTo1[i], .to440Btn = &amdTo48[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &vib[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &pms[i].label, .component = &pms[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &pmsTo64[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &pmd[i].label, .component = &pmd[i] });
        layoutComponentsLtoRFixFreqBtns({ .rect = innerRect, .to0Btn = &pmdTo7[i], .to440Btn = &pmdTo14[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMask[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &mask[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMml[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });
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
