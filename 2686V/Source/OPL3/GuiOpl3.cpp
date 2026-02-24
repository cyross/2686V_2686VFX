#include "GuiOpl3.h"

#include "../gui/GuiConstants.h"
#include "../gui/LabelConstants.h"
#include "../fm/OpConstants.h"
#include "../fm/RegisterConverter.h"
#include "../fm/MmlConstants.h"

void GuiOpl3::setup()
{
    const juce::String code = codeOpl3;

    mainGroup.setup(*this, mGroupTitle);
    bitSelector.setup({ .parent = *this, .id = code + postBit, .title = mBitTitle, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + postRate, .title = mRateTitle, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + postAlg, .title = mAlgTitle, .items = opl3AlgItems, .isReset = true });
    feedbackSlider.setup({ .parent = *this, .id = code + postFb0, .title = mFb0Title, .isReset = true });
    feedback2Slider.setup({ .parent = *this, .id = code + postFb2, .title = mFb2Title, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = masterVolumeLabel, .isReset = true });

    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, opGroupPrefix + juce::String(i + 1));

        juce::String paramPrefix = code + codeOp + juce::String(i);

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 DR:0 SL:0 RR:15 TL:0 MUL:1",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postMul, .title = opMulLabel, .isReset = true, .regType = RegisterType::FmMul });
        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postTl, .title = opTlLabel, .isReset = true, .regType = RegisterType::FmTl });

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postAr, .title = opArLabel, .isReset = true, .regType = RegisterType::FmAr });
        dr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postDr, .title = opDrLabel, .isReset = true, .regType = RegisterType::FmDr });
        sl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postSl, .title = opSlLabel, .isReset = true, .regType = RegisterType::FmSl });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postRr, .title = mRrLabel, .isReset = true, .regType = RegisterType::FmRr });

        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postAm, .title = opVibLabel, .isReset = true });

        vib[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postVib, .title = opEgTypeLabel, .isReset = true });

        egType[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postEgType, .title = opAmLabel, .isReset = true });

        ksr[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postKsr, .title = opKsrLabel, .isReset = true });
        ksl[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + postKsl, .title = opKslLabel, .items = kslItems, .isReset = true });

        eg[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + postEg, .title = opWsLabel, .items = opl3EgItems, .isReset = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postMask, .title = opMaskLabel, .isReset = true });
    }
}

void GuiOpl3::layout(juce::Rectangle<int> content)
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

    // --- Operators Section ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);
        opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &mul[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &mul[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &ar[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &ar[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &dr[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &sl[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &sl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &rr[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &rr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &tl[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &tl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &am[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &vib[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &egType[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &ksr[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &ksl[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &ksl[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &eg[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &eg[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &mask[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpLastRowPaddingBottom, { { &mml[i], { OpRegButtonWidth, 0} } });
    }
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpl3::applyMmlString(const juce::String& mml, int opIndex)
{
    juce::String input = mml.toUpperCase();

    auto getValue = [&](const juce::String& key, int maxVal) -> int {
        int idx = input.indexOf(key);
        if (idx < 0) return -1;
        int valStart = idx + key.length();
        int valEnd = valStart;
        while (valEnd < input.length() && (juce::CharacterFunctions::isDigit(input[valEnd]))) {
            valEnd++;
        }
        if (valStart == valEnd) return -1;
        return input.substring(valStart, valEnd).getIntValue();
        };

    int val;

    // MUL
    val = getValue(mmlPrefixMul, 15);
    if (val >= 0) mul[opIndex].setValue((double)RegisterConverter::convertFmMul(val), juce::sendNotification);

    // TL
    val = getValue(mmlPrefixTl, 127);
    if (val >= 0) tl[opIndex].setValue(RegisterConverter::convertFmTl(val), juce::sendNotification);

    // AR(Reverse)
    val = getValue(mmlPrefixRar, 31);
    if (val >= 0) ar[opIndex].setValue(RegisterConverter::convertFmAr(31 - val), juce::sendNotification);
    // AR
    else {
        val = getValue(mmlPrefixAr, 31);
        if (val >= 0) ar[opIndex].setValue(RegisterConverter::convertFmAr(val), juce::sendNotification);
    }

    // DR
    val = getValue(mmlPrefixRdr, 31);
    if (val >= 0) dr[opIndex].setValue(RegisterConverter::convertFmDr(31 - val), juce::sendNotification);
    // DR
    else
    {
        val = getValue(mmlPrefixDr, 31);
        if (val >= 0) dr[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
    }

    // SL
    val = getValue(mmlPrefixSl, 15);
    if (val >= 0) sl[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

    // RR(Reverse)
    val = getValue(mmlPrefixRrr, 15);
    if (val >= 0) rr[opIndex].setValue(RegisterConverter::convertFmRr(15 - val), juce::sendNotification);
    // RR
    else {
        val = getValue(mmlPrefixRr, 15);
        if (val >= 0) rr[opIndex].setValue(RegisterConverter::convertFmRr(val), juce::sendNotification);
    }
}
