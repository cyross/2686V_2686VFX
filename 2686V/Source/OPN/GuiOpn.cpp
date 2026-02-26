#include "GuiOpn.h"

#include "../core/GuiConstants.h"
#include "../core/GuiLabels.h"
#include "../core/OpConstants.h"
#include "../core/MmlConstants.h"
#include "../core/MmlValues.h"

#include "../fm/RegisterConverter.h"

void GuiOpn::setup()
{
    const juce::String code = codeOpn;

    mainGroup.setup(*this, mGroupTitle);
    bitSelector.setup({ .parent = *this, .id = code + postBit, .title = mBitTitle, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + postRate, .title = mRateTitle, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + postAlg, .title = mAlgTitle, .items = opnaAlgItems, .isReset = true });
    feedbackSlider.setup({ .parent = *this, .id = code + postFb0, .title = mFb0Title, .isReset = true });
    feedback2Slider.setup({ .parent = *this, .id = code + postFb2, .title = mFb2Title, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = masterVolumeLabel, .isReset = true });

    // Operators
    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, opGroupPrefix + juce::String(i + 1));

        juce::String paramPrefix = code + codeOp + juce::String(i);

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 DR:0 SR:0 SL:0 RR:15 TL:0 MUL:1 DT:0",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });
        addAndMakeVisible(mml[i]);

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postMul, .title = opMulLabel, .isReset = true, .regType = RegisterType::FmMul });
        dt[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + postDt, .title = opDtLabel, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postTl, .title = opTlLabel, .isReset = true, .regType = RegisterType::FmTl });

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postAr, .title = opArLabel, .isReset = true, .regType = RegisterType::FmAr });
        dr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postDr, .title = opDrLabel, .isReset = true, .regType = RegisterType::FmDr });
        sl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postSl, .title = opSlLabel, .isReset = true, .regType = RegisterType::FmSl });
        sr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postSr, .title = opSrLabel, .isReset = true, .regType = RegisterType::FmSr });
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

void GuiOpn::layout(juce::Rectangle<int> content)
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
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &feedback2Slider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &feedback2Slider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &masterVolSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &masterVolSlider, { MainRegValueWidth, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        // 枠線
        opGroups[i].setBounds(opArea.reduced(2));

        // 枠線の内側
        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &mul[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &mul[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &dt[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dt[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &ar[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &ar[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &dr[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &sr[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &sr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &sl[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &sl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &rr[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &rr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &tl[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &tl[i], { OpRegValueWidth, 0}} });
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
void GuiOpn::applyMmlString(const juce::String& mml, int opIndex)
{
    juce::String input = mml.toUpperCase();
    int val;

    // MUL
    val = RegisterConverter::getValue(input, mmlPrefixMul, mmlValues::opn::mul);
    if (RegisterConverter::isValidVal(val)) mul[opIndex].setValue((double)RegisterConverter::convertFmMul(val), juce::sendNotification);

    // DT
    val = RegisterConverter::getValue(input, mmlPrefixDt, mmlValues::opn::dt);
    if (RegisterConverter::isValidVal(val)) {
        int regVal = RegisterConverter::convertMmlDtToReg(val);

        dt[opIndex].setSelectedItemIndex((double)regVal, juce::sendNotification);
    }

    // TL
    val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opn::tl);
    if (RegisterConverter::isValidVal(val)) tl[opIndex].setValue(RegisterConverter::convertFmTl(val), juce::sendNotification);

    // AR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRar, mmlValues::opn::ar);
    if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(mmlValues::opn::ar - val), juce::sendNotification);
    // AR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opn::ar);
        if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(val), juce::sendNotification);
    }

    // DR
    val = RegisterConverter::getValue(input, mmlPrefixRdr, mmlValues::opn::dr);
    if (RegisterConverter::isValidVal(val)) dr[opIndex].setValue(RegisterConverter::convertFmDr(mmlValues::opn::dr - val), juce::sendNotification);
    // DR
    else
    {
        val = RegisterConverter::getValue(input, mmlPrefixDr, mmlValues::opn::dr);
        if (RegisterConverter::isValidVal(val)) dr[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
    }

    // SR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRsr, mmlValues::opn::sr);
    if (RegisterConverter::isValidVal(val)) sr[opIndex].setValue(RegisterConverter::convertFmSr(mmlValues::opn::sr - val), juce::sendNotification);
    // SR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixSr, mmlValues::opn::sr);
        if (RegisterConverter::isValidVal(val)) sr[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
    }

    // SL
    val = RegisterConverter::getValue(input, mmlPrefixSl, mmlValues::opn::sl);
    if (RegisterConverter::isValidVal(val)) sl[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

    // RR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRrr, mmlValues::opn::rr);
    if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(mmlValues::opn::rr - val), juce::sendNotification);
    // RR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opn::rr);
        if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(val), juce::sendNotification);
    }
}
