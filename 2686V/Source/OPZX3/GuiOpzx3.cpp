#include "GuiOpzx3.h"

#include "../processor/PluginProcessor.h"

#include "../core/GuiConstants.h"
#include "../core/LabelConstants.h"
#include "../core/OpConstants.h"
#include "../core/MmlConstants.h"

#include "../fm/RegisterConverter.h"

void GuiOpzx3::setup()
{
    const juce::String code = codeOpzx3;

    mainGroup.setup(*this, mGroupTitle);
    bitSelector.setup({ .parent = *this, .id = code + postBit, .title = mBitTitle, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + postRate, .title = mRateTitle, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + postAlg, .title = mAlgTitle, .items = opzx3AlgItems, .isReset = true });
    feedbackSlider.setup({ .parent = *this, .id = code + postFb0, .title = mFb0Title, .isReset = true });

    lfoFreqSlider.setup({ .parent = *this, .id = code + postLFreq, .title = mLfoFreq, .isReset = true });
    lfoPmsSelector.setup({ .parent = *this, .id = codeOpna + postLPms, .title = mLfoPms, .items = pmsItems, .isReset = true });
    lfoAmsSelector.setup({ .parent = *this, .id = codeOpna + postLAms, .title = mLfoAms, .items = amsItems, .isReset = true });

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

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postMul, .title = opMulLabel, .isReset = true, .regType = RegisterType::FmMul });
        dt1[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + postDt, .title = opDt1Label, .isReset = true, .regType = RegisterType::FmDt });
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

        ws[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + postWs, .title = opWsLabel, .items = opzx3WsItems, .isReset = true });

        loadPcmBtn[i].setup({ .parent = *this, .title = opPcmLabel, .isReset = false, .isResized = false });
        loadPcmBtn[i].onClick = [this, i] {
            auto fileFilter = ctx.audioProcessor.formatManager.getWildcardForAllFormats();
            ctx.editor.openFileChooser(
                "Load PCM for OP" + juce::String(i + 1),
                ctx.audioProcessor.lastSampleDirectory,
                fileFilter,
                [this, i](const juce::FileChooser& fc) {
                    auto file = fc.getResult();
                    if (file.existsAsFile()) {
                        ctx.audioProcessor.loadOpzx3PcmFile(i, file);
                        updatePcmFileName(i, file.getFileName());
                        ctx.audioProcessor.lastSampleDirectory = file.getParentDirectory();
                    }
                }
            );
        };

        clearPcmBtn[i].setup({ .parent = *this, .title = opPcmClearLabel, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = false });
        clearPcmBtn[i].onClick = [this, i] {
            ctx.audioProcessor.unloadOpzx3PcmFile(i);
            updatePcmFileName(i, emptyFilename);
        };

        pcmFileNameLabel[i].setup({ .parent = *this, .title = emptyFilename });
        if (ctx.audioProcessor.opzx3PcmFilePaths[i].isNotEmpty()) {
            updatePcmFileName(i, juce::File(ctx.audioProcessor.opzx3PcmFilePaths[i]).getFileName());
        }


        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + postMask, .title = opMaskLabel, .isReset = true });
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
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &loadPcmBtn[i], { OpRegLabelWidth, OpRegPaddingRight } }, { &pcmFileNameLabel[i], { OpRegPcmFilePathWidth, OpRegPaddingRight } }, { &clearPcmBtn[i], { OpRegClearPcmFileWidth, 0 } } });
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
