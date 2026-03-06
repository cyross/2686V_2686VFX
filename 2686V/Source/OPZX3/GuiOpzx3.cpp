#include "GuiOpzx3.h"

#include "../processor/PluginProcessor.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/MmlKeys.h"
#include "../core/MmlValues.h"

#include "../fm/RegisterConverter.h"

#include "../gui/GuiHelpers.h"

void GuiOpzx3::setup()
{
    const juce::String code = PrKey::Prefix::opzx3;

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    algFbCat.setup({ .parent = *this, .title = GuiText::Category::algFb });

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Fm::alg, .items = opzx3AlgItems, .isReset = true });
    feedbackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb0, .title = GuiText::Fm::fb0, .isReset = true });
    feedback2Slider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb2, .title = GuiText::Fm::fb2, .isReset = true });

    lfoCat.setup({ .parent = *this, .title = GuiText::Category::lfo });

    lfoFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::freq, .title = GuiText::Fm::lfoFreq, .isReset = true });
    lfoPmToggle.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pm, .title = GuiText::Fm::pmEn, .isReset = true });
    lfoAmToggle.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::am, .title = GuiText::Fm::amEn, .isReset = true });
    lfoPmsSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pms, .title = GuiText::Fm::pms, .items = pmsItems, .isReset = true });
    lfoAmsSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::ams, .title = GuiText::Fm::ams, .items = amsItems, .isReset = true });
    lfoPmdSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pmd, .title = GuiText::Fm::pmd, .isReset = true });
    lfoAmdSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::amd, .title = GuiText::Fm::amd, .isReset = true });

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    // Operators
    const juce::String opCode = code + PrKey::Innder::op;

    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, GuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 D1R:0 D1L:0 D2R:0 RR:15 TL:0 MUL:1 DT1:0 DT2:0",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        catMain[i].setup({ .parent = *this, .title = GuiText::Category::m });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mul, .title = GuiText::Fm::Op::Mul, .isReset = true, .regType = RegisterType::FmMul });
        dt1[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dt, .title = GuiText::Fm::Op::Dt1, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        dt2[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dt2, .title = GuiText::Fm::Op::Dt2, .isReset = true, .regType = RegisterType::FmDt2 });
        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::tl, .title = GuiText::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ar, .title = GuiText::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        d1r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::d1r, .title = GuiText::Fm::Op::D1r, .isReset = true, .regType = RegisterType::FmDr });
        d1l[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::d1l, .title = GuiText::Fm::Op::D1l, .isReset = true, .regType = RegisterType::FmSl });
        d2r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::d2r, .title = GuiText::Fm::Op::D2r, .isReset = true, .regType = RegisterType::FmSr });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rr, .title = GuiText::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ks, .title = GuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });

        cafFix[i].setup({ .parent = *this, .title = GuiText::Category::fix });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fix, .title = GuiText::Fm::Op::Fix, .isReset = true });
        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fixFreq, .title = GuiText::Fm::Op::FFreq, .isReset = true });
        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::Opzx3FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };
        freqTo05[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::Opzx3FreqTo05, .isReset = false, .isResized = false });
        freqTo05[i].onClick = [this, index = i] { freq[index].setValue(0.5, juce::sendNotification); };
        freqTo1[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::Opzx3FreqTo1, .isReset = false, .isResized = false });
        freqTo1[i].onClick = [this, index = i] { freq[index].setValue(1, juce::sendNotification); };
        freqTo2[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::Opzx3FreqTo2, .isReset = false, .isResized = false });
        freqTo2[i].onClick = [this, index = i] { freq[index].setValue(2, juce::sendNotification); };
        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::Opzx3FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        catShape[i].setup({ .parent = *this, .title = GuiText::Category::shape });

        ws[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ws, .title = GuiText::Fm::Op::Ws, .items = opzx3WsItems, .isReset = true });

        loadPcmBtn[i].setup({ .parent = *this, .title = GuiText::File::Pcm, .isReset = false, .isResized = false });
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

        clearPcmBtn[i].setup({ .parent = *this, .title = GuiText::File::clear, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = false });
        clearPcmBtn[i].onClick = [this, i] {
            ctx.audioProcessor.unloadOpzx3PcmFile(i);
            updatePcmFileName(i, Io::empty);
        };

        pcmFileNameLabel[i].setup({ .parent = *this, .title = Io::empty });
        if (ctx.audioProcessor.opzx3PcmFilePaths[i].isNotEmpty()) {
            updatePcmFileName(i, juce::File(ctx.audioProcessor.opzx3PcmFilePaths[i]).getFileName());
        }

        pcmOffset[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pcmOffset, .title = GuiText::Fm::Op::PcmOffset, .isReset = true });
        pcmRatio[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pcmRatio, .title = GuiText::Fm::Op::PcmRatio, .isReset = true });

        catLfo[i].setup({ .parent = *this, .title = GuiText::Category::lfo });

        pms[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pms, .title = GuiText::Fm::Op::Pms, .items = pmsItems, .isReset = true });
        pm[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::vib, .title = GuiText::Fm::Op::PmEn, .isReset = true });
        ams[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ams, .title = GuiText::Fm::Op::Ams, .items = amsItems, .isReset = true });
        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::am, .title = GuiText::Fm::Op::AmEn, .isReset = true });

        catMask[i].setup({ .parent = *this, .title = GuiText::Category::mask });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mask, .title = GuiText::Fm::Op::Mask, .isReset = true });

        catMml[i].setup({ .parent = *this, .title = GuiText::Category::mml });
    }
}
void GuiOpzx3::layout(juce::Rectangle<int> content)
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
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoPmsSelector.label, .component = &lfoPmsSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &lfoAmToggle });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoAmsSelector.label, .component = &lfoAmsSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

        // 枠線
        opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(GuiValue::Fm::Op::Padding::width, GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMain[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &dt1[i].label, .component = &dt1[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &dt2[i].label, .component = &dt2[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ar[i].label, .component = &ar[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &d1r[i].label, .component = &d1r[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &d1l[i].label, .component = &d1l[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &d2r[i].label, .component = &d2r[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rr[i].label, .component = &rr[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &tl[i].label, .component = &tl[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catShape[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ws[i].label, .component = &ws[i] });
        layoutComponentsLtoROpzx3PcmRow({ .rect = innerRect, .loadPcmBtn = &loadPcmBtn[i], .pcmFileNameLabel = &pcmFileNameLabel[i], .clearPcmBtn = &clearPcmBtn[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &pcmOffset[i].label, .component = &pcmOffset[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &pcmRatio[i].label, .component = &pcmRatio[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catLfo[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &pm[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &pms[i].label, .component = &pms[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &am[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ams[i].label, .component = &ams[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &cafFix[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &fix[i] });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &freq[i].label, .component = &freq[i] });
        layoutComponentsLtoROpzx3FixFreqBtns({ .rect = innerRect, .to0Btn = &freqToZero[i], .to05Btn = &freqTo05[i], .to1Btn = &freqTo1[i], .to2Btn = &freqTo2[i], .to440Btn = &freqTo440[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMask[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &mask[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMml[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });
    }
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpzx3::applyMmlString(const juce::String& mml, int opIndex)
{
    juce::String input = mml.toUpperCase();
    int val;

    // MUL
    val = RegisterConverter::getValue(input, mmlPrefixMul, mmlValues::opzx3::mul);
    if (RegisterConverter::isValidVal(val)) mul[opIndex].setValue((double)RegisterConverter::convertFmMul(val), juce::sendNotification);

    // DT1
    val = RegisterConverter::getValue(input, mmlPrefixDt1, mmlValues::opzx3::dt1);
    if (RegisterConverter::isValidVal(val)) {
        int regVal = RegisterConverter::convertMmlDtToReg(val);
 
        dt1[opIndex].setSelectedItemIndex((double)regVal, juce::sendNotification);
    }

    // DT2
    val = RegisterConverter::getValue(input, mmlPrefixDt2, mmlValues::opzx3::dt2);
    if (RegisterConverter::isValidVal(val)) dt2[opIndex].setValue((double)val, juce::sendNotification);

    // TL
    val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opzx3::tl);
    if (RegisterConverter::isValidVal(val)) tl[opIndex].setValue(RegisterConverter::convertFmTl(val), juce::sendNotification);

    // AR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRar, mmlValues::opzx3::ar);
    if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(mmlValues::opzx3::ar - val), juce::sendNotification);
    // AR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opzx3::ar);
        if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(val), juce::sendNotification);
    }

    // D1R(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRd1r, mmlValues::opzx3::d1r);
    if (RegisterConverter::isValidVal(val)) d1r[opIndex].setValue(RegisterConverter::convertFmDr(mmlValues::opzx3::d1r - val), juce::sendNotification);
    // D1R
    else {
        val = RegisterConverter::getValue(input, mmlPrefixD1r, mmlValues::opzx3::d1r);
        if (RegisterConverter::isValidVal(val)) d1r[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
    }

    // D1L
    val = RegisterConverter::getValue(input, mmlPrefixD1l, mmlValues::opzx3::d1l);
    if (RegisterConverter::isValidVal(val)) d1l[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

    // D2R(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRd2r, mmlValues::opzx3::d2r);
    if (RegisterConverter::isValidVal(val)) d2r[opIndex].setValue(RegisterConverter::convertFmSr(mmlValues::opzx3::d2r - val), juce::sendNotification);
    // D2R
    else {
        val = RegisterConverter::getValue(input, mmlPrefixD2r, mmlValues::opzx3::d2r);
        if (RegisterConverter::isValidVal(val)) d2r[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
    }

    // RR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRrr, mmlValues::opzx3::rr);
    if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(mmlValues::opzx3::rr - val), juce::sendNotification);
    // RR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opzx3::rr);
        if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(val), juce::sendNotification);
    }
}
