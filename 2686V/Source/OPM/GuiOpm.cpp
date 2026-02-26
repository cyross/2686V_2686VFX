#include "GuiOpm.h"

#include "../core/GuiConstants.h"
#include "../core/GuiLabels.h"
#include "../core/OpConstants.h"
#include "../core/MmlConstants.h"
#include "../core/MmlValues.h"

#include "../fm/RegisterConverter.h"

void GuiOpm::setup()
{
    const juce::String code = codeOpm;

    mainGroup.setup(*this, mGroupTitle);
    bitSelector.setup({ .parent = *this, .id = code + postBit, .title = mBitTitle, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + postRate, .title = mRateTitle, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + postAlg, .title = mAlgTitle, .items = opmAlgItems, .isReset = true });
    feedbackSlider.setup({ .parent = *this, .id = code + postFb0, .title = mFb0Title, .isReset = true });

    lfoFreqSlider.setup({ .parent = *this, .id = code + postLFreq, .title = mLfoFreq, .isReset = true });
    lfoPmsSelector.setup({ .parent = *this, .id = codeOpna + postLPms, .title = mLfoPms, .items = pmsItems, .isReset = true });
    lfoAmsSelector.setup({ .parent = *this, .id = codeOpna + postLAms, .title = mLfoAms, .items = amsItems, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = masterVolumeLabel, .isReset = true });

    // Operators
    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, opGroupPrefix + juce::String(i + 1));

        juce::String paramPrefix = code + codeOp + juce::String(i);

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 D1R:0 D1L:0 D2R:0 RR:15 TL:0 MUL:1 DT:10 DT:20",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postMul, .title = opMulLabel, .isReset = true, .regType = RegisterType::FmMul });
        dt1[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + postDt, .title = opDt1Label, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        dt2[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postDt2, .title = opDt2Label, .isReset = true, .regType = RegisterType::FmDt2 });
        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postTl, .title = opTlLabel, .isReset = true, .regType = RegisterType::FmTl });

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postAr, .title = opArLabel, .isReset = true, .regType = RegisterType::FmAr });
        d1r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postD1r, .title = opD1rLabel, .isReset = true, .regType = RegisterType::FmDr });
        d1l[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postD1l, .title = opD1lLabel, .isReset = true, .regType = RegisterType::FmSl });
        d2r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postD2r, .title = opD2rLabel, .isReset = true, .regType = RegisterType::FmSr });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postRr, .title = mRrLabel, .isReset = true, .regType = RegisterType::FmRr });

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + postKs, .title = opKsLabel, .items = ksItems, .isReset = true });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postFix, .title = opFixLabel, .isReset = true });
        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postFixFreq, .title = opFFreqLabel, .isReset = true });
        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = opFreqTo0Label, .isReset = false, .isResized = false });
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };
        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = opFreqTo440Label, .isReset = false, .isResized = false });
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postMask, .title = opMaskLabel, .isReset = true });
    }
}

void GuiOpm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &bitSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &rateSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &rateSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &algSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &algSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &feedbackSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &feedbackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &lfoFreqSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &lfoFreqSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &lfoPmsSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &lfoPmsSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &lfoAmsSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &lfoAmsSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &masterVolSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &masterVolSlider, { MainRegValueWidth, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        // 枠線
        opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &mul[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &mul[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &dt1[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dt1[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &dt2[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dt2[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &ar[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &ar[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &d1r[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &d1r[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &d1l[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &d1l[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &d2r[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &d2r[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &rr[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &rr[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &tl[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &tl[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &ks[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &ks[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &fix[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &freq[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &freq[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &freqToZero[i], { OpRegFreqChangeButtonWidth, OpRegPaddingRight} }, { &freqTo440[i], { OpRegFreqChangeButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &mask[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpLastRowPaddingBottom, { { &mml[i], { OpRegButtonWidth, 0} } });
    }
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpm::applyMmlString(const juce::String& mml, int opIndex)
{
    juce::String input = mml.toUpperCase();
    int val;

    // MUL
    val = RegisterConverter::getValue(input, mmlPrefixMul, mmlValues::opm::mul);
    if (RegisterConverter::isValidVal(val)) mul[opIndex].setValue((double)RegisterConverter::convertFmMul(val), juce::sendNotification);

    // DT1
    val = RegisterConverter::getValue(input, mmlPrefixDt1, mmlValues::opm::dt1);
    if (RegisterConverter::isValidVal(val)) {
        int regVal = RegisterConverter::convertMmlDtToReg(val);

        dt1[opIndex].setSelectedItemIndex((double)regVal, juce::sendNotification);
    }

    // DT2
    val = RegisterConverter::getValue(input, mmlPrefixDt2, mmlValues::opm::dt2);
    if (RegisterConverter::isValidVal(val)) dt2[opIndex].setValue((double)val, juce::sendNotification);

    // TL
    val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opm::tl);
    if (RegisterConverter::isValidVal(val)) tl[opIndex].setValue(RegisterConverter::convertFmTl(val), juce::sendNotification);

    // AR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRar, mmlValues::opm::ar);
    if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(mmlValues::opm::ar - val), juce::sendNotification);
    // AR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opm::ar);
        if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(val), juce::sendNotification);
    }

    // D1R(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRd1r, mmlValues::opm::d1r);
    if (RegisterConverter::isValidVal(val)) d1r[opIndex].setValue(RegisterConverter::convertFmDr(mmlValues::opm::d1r - val), juce::sendNotification);
    // D1R
    else {
        val = RegisterConverter::getValue(input, mmlPrefixD1r, mmlValues::opm::d1r);
        if (RegisterConverter::isValidVal(val)) d1r[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
    }

    // D1L
    val = RegisterConverter::getValue(input, mmlPrefixD1l, mmlValues::opm::d1l);
    if (RegisterConverter::isValidVal(val)) d1l[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

    // D2R(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRd2r, mmlValues::opm::d2r);
    if (RegisterConverter::isValidVal(val)) d2r[opIndex].setValue(RegisterConverter::convertFmSr(mmlValues::opm::d2r - val), juce::sendNotification);
    // D2R
    else {
        val = RegisterConverter::getValue(input, mmlPrefixD2r, mmlValues::opm::d2r);
        if (RegisterConverter::isValidVal(val)) d2r[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
    }

    // RR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRrr, mmlValues::opm::rr);
    if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(mmlValues::opm::rr - val), juce::sendNotification);
    // RR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opm::rr);
        if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(val), juce::sendNotification);
    }
}
