#include "GuiOpn.h"

#include "../gui/GuiConstants.h"
#include "../gui/LabelConstants.h"
#include "../fm/OpConstants.h"
#include "../fm/RegisterConverter.h"
#include "../fm/MmlConstants.h"

void GuiOpn::setup()
{
    const juce::String code = codeOpn;

    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> ksItems = { {.name = "0 OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    std::vector<SelectItem> bdItems = {
        {.name = "0: 4-bit (16 steps)",  .value = 1 },
        {.name = "1: 5-bit (32 steps)",  .value = 2 },
        {.name = "2: 6-bit (64 steps)",  .value = 3 },
        {.name = "3: 8-bit (256 steps)", .value = 4 },
        {.name = "4: Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    mainGroup.setup(*this, mGroupTitle);
    bitSelector.setup({ .parent = *this, .id = code + postBit, .title = mBitTitle, .items = bdItems });
    rateSelector.setup({ .parent = *this, .id = code + postRate, .title = mRateTitle, .items = rateItems });

    algSelector.setup({ .parent = *this, .id = code + postAlg, .title = mAlgTitle, .items = algItems });
    feedbackSlider.setup({ .parent = *this, .id = code + postFb0, .title = mFb0Title });
    feedback2Slider.setup({ .parent = *this, .id = code + postFb2, .title = mFb2Title });

    masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = masterVolumeLabel });

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

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postMul, .title = opMulLabel, .regType = RegisterType::FmMul });
        dt[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postDt, .title = opDtLabel, .regType = RegisterType::FmDt });

        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postRr, .title = opTlLabel, .regType = RegisterType::FmTl });
        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postAr, .title = opArLabel, .regType = RegisterType::FmAr });
        dr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postDr, .title = opDrLabel, .regType = RegisterType::FmDr });
        sl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postSl, .title = opSlLabel, .regType = RegisterType::FmSl });
        sr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postSr, .title = opSrLabel, .regType = RegisterType::FmSr });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postRr, .title = mRrLabel, .regType = RegisterType::FmRr });

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + postKs, .title = opKsLabel, .items = ksItems });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postFix, .title = opFixLabel });
        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postFixFreq, .title = opFFreqLabel });
        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .id = paramPrefix + postRr, .title = opFreqTo0Label, .isReset = false, .isResized = false });
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };
        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .id = paramPrefix + postRr, .title = opFreqTo440Label, .isReset = false, .isResized = false });
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postMask, .title = opMaskLabel });
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

    // DT
    val = getValue(mmlPrefixDt, 7);
    if (val >= 0) dt[opIndex].setValue((double)val, juce::sendNotification);

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

    // SR(Reverse)
    val = getValue(mmlPrefixRsr, 31);
    if (val >= 0) sr[opIndex].setValue(RegisterConverter::convertFmSr(31 - val), juce::sendNotification);
    // SR
    else {
        val = getValue(mmlPrefixSr, 31);
        if (val >= 0) sr[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
    }

    // RR(Reverse)
    val = getValue(mmlPrefixRrr, 15);
    if (val >= 0) rr[opIndex].setValue(RegisterConverter::convertFmRr(15 - val), juce::sendNotification);
    // RR
    else {
        val = getValue(mmlPrefixRr, 15);
        if (val >= 0) rr[opIndex].setValue(RegisterConverter::convertFmRr(val), juce::sendNotification);
    }
}
