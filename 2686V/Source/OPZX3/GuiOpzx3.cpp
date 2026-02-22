#include "GuiOpzx3.h"

#include "../gui/GuiConstants.h"
#include "../gui/LabelConstants.h"
#include "../fm/OpConstants.h"
#include "../fm/RegisterConverter.h"
#include "../fm/MmlConstants.h"

void GuiOpzx3::setup()
{
    const juce::String code = codeOpzx3;

    std::vector<SelectItem> pmsItems = createItems(8, "Pms: ");
    std::vector<SelectItem> amsItems = createItems(4, "Ams: ");
    std::vector<SelectItem> algItems = createAlgItems(27);
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
    std::vector<SelectItem> wsItems = {
        {.name = "00: Sine", .value = 1},
        {.name = "01: Half Sine", .value = 2},
        {.name = "02: Abs Sine", .value = 3},
        {.name = "03: Alternating Half Sine", .value = 4},
        {.name = "04: Alternating Sine", .value = 5},
        {.name = "05: Alternating Abs Sine", .value = 6},
        {.name = "06: Square", .value = 7},
        {.name = "07: Derived Square", .value = 8},
        {.name = "08: Saw Down", .value = 9},
        {.name = "09: Saw Up", .value = 10},
        {.name = "10: Triangle", .value = 11},
        {.name = "11: Saw + Sine", .value = 12},
        {.name = "12: Log Saw", .value = 13},
        {.name = "13: Pulse 25%", .value = 14},
        {.name = "14: Pulse 12.5%", .value = 15},
        {.name = "15: Pulse 6.25%", .value = 16},
        {.name = "16: Round Square", .value = 17},
        {.name = "17: Impulse Train", .value = 18},
        {.name = "18: Comb / Multi-pulse", .value = 19},
        {.name = "19: Resonant Saw (Low)", .value = 20},
        {.name = "20: Resonant Saw (High)", .value = 21},
        {.name = "21: Resonant Triangle", .value = 22},
        {.name = "22: Bulb Sine", .value = 23},
        {.name = "23: Double Hump", .value = 24},
        {.name = "24: Pseudo Voice Formant 1", .value = 25},
        {.name = "25: Pseudo Voice Formant 2", .value = 26},
        {.name = "26: Metallic 1", .value = 27},
        {.name = "27: Metallic 2", .value = 28},
        {.name = "28: Noise-Like", .value = 29},
    };

    mainGroup.setup(*this, mGroupTitle);
    bitSelector.setup({ .parent = *this, .id = code + postBit, .title = mBitTitle, .items = bdItems });
    rateSelector.setup({ .parent = *this, .id = code + postRate, .title = mRateTitle, .items = rateItems });

    algSelector.setup({ .parent = *this, .id = code + postAlg, .title = mAlgTitle, .items = algItems });
    feedbackSlider.setup({ .parent = *this, .id = code + postFb0, .title = mFb0Title });
    feedback2Slider.setup({ .parent = *this, .id = code + postFb2, .title = mFb2Title });

    lfoFreqSlider.setup({ .parent = *this, .id = code + postLFreq, .title = mLfoFreq });
    lfoPmsSelector.setup({ .parent = *this, .id = codeOpna + postLPms, .title = mLfoPms, .items = pmsItems });
    lfoAmsSelector.setup({ .parent = *this, .id = codeOpna + postLAms, .title = mLfoAms, .items = amsItems });

    masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = masterVolumeLabel });

    // Operators
    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, opGroupPrefix + juce::String(i + 1));

        juce::String paramPrefix = code + codeOp + juce::String(i);

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 D1R:0 D1L:0 D2R:0 RR:15 TL:0 MUL:1 DT1:0 DT2:0",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postMul, .title = opMulLabel, .regType = RegisterType::FmMul });
        dt1[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postDt1, .title = opDt1Label, .regType = RegisterType::FmDt });
        dt2[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postDt2, .title = opDt2Label, .regType = RegisterType::FmDt2 });
        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postRr, .title = opTlLabel, .regType = RegisterType::FmTl });

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postAr, .title = opArLabel, .regType = RegisterType::FmAr });
        d1r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postD1r, .title = opD1rLabel, .regType = RegisterType::FmDr });
        d1l[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postD1l, .title = opD1lLabel, .regType = RegisterType::FmSl });
        d2r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postD2r, .title = opD2rLabel, .regType = RegisterType::FmSr });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postRr, .title = mRrLabel, .regType = RegisterType::FmRr });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postFix, .title = opFixLabel });
        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postFixFreq, .title = opFFreqLabel });
        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .id = paramPrefix + postRr, .title = opFreqTo0Label, .isReset = false, .isResized = false });
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };
        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .id = paramPrefix + postRr, .title = opFreqTo440Label, .isReset = false, .isResized = false });
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        ws[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + postWs, .title = opWsLabel, .items = wsItems });
        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postMask, .title = opMaskLabel });
    }
}
void GuiOpzx3::layout(juce::Rectangle<int> content)
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
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &dt1[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dt1[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &dt2[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dt2[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &ar[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &ar[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &d1r[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &d1r[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &d1l[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &d1l[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &d2r[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &d2r[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &rr[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &rr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &tl[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &tl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &ks[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &ks[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &fix[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &freq[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &freq[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &freqToZero[i], { OpRegFreqChangeButtonWidth, OpRegPaddingRight} }, { &freqTo440[i], { OpRegFreqChangeButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &ws[i].label, { OpRegLabelWidth, OpRegPaddingRight} }, { &ws[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &mask[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpLastRowPaddingBottom, { { &mml[i], { OpRegButtonWidth, 0} } });
    }
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpzx3::applyMmlString(const juce::String& mml, int opIndex)
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

    // DT1
    val = getValue(mmlPrefixDt1, 7);
    if (val >= 0) dt1[opIndex].setValue((double)val, juce::sendNotification);

    // DT2
    val = getValue(mmlPrefixDt2, 7);
    if (val >= 0) dt2[opIndex].setValue((double)val, juce::sendNotification);

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

    // D1R(Reverse)
    val = getValue(mmlPrefixRd1r, 31);
    if (val >= 0) d1r[opIndex].setValue(RegisterConverter::convertFmDr(31 - val), juce::sendNotification);
    // D1R
    else {
        val = getValue(mmlPrefixD1r, 31);
        if (val >= 0) d1r[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
    }

    // D1L
    val = getValue(mmlPrefixD1l, 15);
    if (val >= 0) d1l[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

    // D2R(Reverse)
    val = getValue(mmlPrefixRd2r, 15);
    if (val >= 0) d2r[opIndex].setValue(RegisterConverter::convertFmSr(15 - val), juce::sendNotification);
    // D2R
    else {
        val = getValue(mmlPrefixD2r, 15);
        if (val >= 0) d2r[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
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
