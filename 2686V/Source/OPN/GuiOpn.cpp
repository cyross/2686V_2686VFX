#include "GuiOpn.h"

#include <vector>

#include "../processor/PluginProcessor.h"

#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/MmlKeys.h"
#include "../core/MmlValues.h"

#include "../fm/RegisterConverter.h"

#include "../gui/GuiHelpers.h"
#include "../gui/GuiValues.h"
#include "../gui/GuiText.h"
#include "../gui/GuiStructs.h"

static std::vector<SelectItem> bdItems = {
    {.name = "1: 4-bit (16 steps)",  .value = 1 },
    {.name = "2: 5-bit (32 steps)",  .value = 2 },
    {.name = "3: 6-bit (64 steps)",  .value = 3 },
    {.name = "4: 8-bit (256 steps)", .value = 4 },
    {.name = "5: Raw",               .value = 5 },
    {.name = "6: 7-bit (OPLL/128 steps)", .value = 6 }
};

static std::vector<SelectItem> rateItems = {
    {.name = "1: 96kHz",    .value = 1 },
    {.name = "2: 55.5kHz",  .value = 2 },
    {.name = "3: 48kHz",    .value = 3 },
    {.name = "4: 44.1kHz",  .value = 4 },
    {.name = "5: 22.05kHz", .value = 5 },
    {.name = "6: 16kHz",    .value = 6 },
    {.name = "7: 8kHz",     .value = 7 },
};

static std::vector<SelectItem> opnAlgItems = {
    {.name = "00: <OPN-00>", .value = 1 },
    {.name = "01: <OPN-01>", .value = 2 },
    {.name = "02: <OPN-02>", .value = 3 },
    {.name = "03: <OPN-03>", .value = 4 },
    {.name = "04: <OPN-04>", .value = 5 },
    {.name = "05: <OPN-05>", .value = 6 },
    {.name = "06: <OPN-06>", .value = 7 },
    {.name = "07: <OPN-07>", .value = 8 },
};

// DT (デチューン1) 用のコンボボックスアイテム
// レジスタ仕様: 0=0, 1=+1, 2=+2, 3=+3, 4=0, 5=-1, 6=-2, 7=-3
static std::vector<SelectItem> dtItems = {
    {.name = " 0", .value = 1 },
    {.name = "-3", .value = 2 },
    {.name = "-2", .value = 3 },
    {.name = "-1", .value = 4 },
    {.name = " 0", .value = 5 }, // 実質0ですが、レジスタ4として一応用意
    {.name = "+1", .value = 6 },
    {.name = "+2", .value = 7 },
    {.name = "+3", .value = 8 }
};

static std::vector<SelectItem> ksItems = {
    {.name = "0 OFF",      .value = 1},
    {.name = "1 (Weak)",   .value = 2},
    {.name = "2 (Mid)",    .value = 3},
    {.name = "3 (Strong)", .value = 4}
};

void GuiOpn::setup()
{
    const juce::String code = PrKey::Prefix::opn;

    monoPolyCat.setup({ .parent = *this, .title = GuiText::Category::monoMode });
    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });
    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    algFbCat.setup({ .parent = *this, .title = GuiText::Category::algFb });

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Fm::alg, .items = opnAlgItems, .isReset = true });
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };
    feedbackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb0, .title = GuiText::Fm::fb0, .isReset = true });
    feedback2Slider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb2, .title = GuiText::Fm::fb2, .isReset = true });

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    monoModeToggle.setup({ .parent = *this, .id = PrKey::monoMode, .title = GuiText::monoPoly, .isReset = true });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    for (int i = 0; i < 8; ++i)
    {
        juce::String fileName = juce::String::formatted(Io::Folder::asset + "/" + Io::Folder::resource + "/ALG_OPNA_OPN_OPM_%02d.png", i);
        auto imgFile = docDir.getChildFile(fileName);

        if (imgFile.existsAsFile()) {
            algImages[i] = juce::ImageFileFormat::loadFrom(imgFile);
        }
    }

    // 画像コンポーネントを画面に追加
    addAndMakeVisible(algImageComp);

    // Operators
    const juce::String opCode = code + PrKey::Innder::op;

    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, GuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 DR:0 SR:0 SL:0 RR:15 TL:0 MUL:1 DT:0",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });
        addAndMakeVisible(mml[i]);

        catMain[i].setup({ .parent = *this, .title = GuiText::Category::m });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mul, .title = GuiText::Fm::Op::Mul, .isReset = true, .regType = RegisterType::FmMul });
        dt[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dt, .title = GuiText::Fm::Op::Dt, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::tl, .title = GuiText::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ar, .title = GuiText::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        dr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dr, .title = GuiText::Fm::Op::Dr, .isReset = true, .regType = RegisterType::FmDr });
        sl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sl, .title = GuiText::Fm::Op::Sl, .isReset = true, .regType = RegisterType::FmSl });
        sr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sr, .title = GuiText::Fm::Op::Sr, .isReset = true, .regType = RegisterType::FmSr });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rr, .title = GuiText::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ks, .title = GuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });

        cafFix[i].setup({ .parent = *this, .title = GuiText::Category::fix });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fix, .title = GuiText::Fm::Op::Fix, .isReset = true });
        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fixFreq, .title = GuiText::Fm::Op::FFreq, .isReset = true });
        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };
        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        catMask[i].setup({ .parent = *this, .title = GuiText::Category::mask });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mask, .title = GuiText::Fm::Op::Mask, .isReset = true });

        catMml[i].setup({ .parent = *this, .title = GuiText::Category::mml });

        rgEn[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgEn, .title = GuiText::Fm::Op::RgEn, .isReset = true });
        rgEn[i].onStateChange = [this, i] {
            ctx.editor.resized();
            };
        rgAr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgAr, .title = GuiText::Fm::Op::Ar, .isReset = true });
        rgDr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgDr, .title = GuiText::Fm::Op::Dr, .isReset = true });
        rgSl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgSl, .title = GuiText::Fm::Op::Sl, .isReset = true });
        rgSr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgSr, .title = GuiText::Fm::Op::Sr, .isReset = true });
        rgRr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgRr, .title = GuiText::Fm::Op::Rr, .isReset = true });
        rgTl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgTl, .title = GuiText::Fm::Op::Tl, .isReset = true });
    }
}

void GuiOpn::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &presetNameCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = GuiValue::PresetName::paddingBottom });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &qualityCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &bitSelector.label, .component = &bitSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &rateSelector.label, .component = &rateSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &algFbCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &feedback2Slider.label, .component = &feedback2Slider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &monoPolyCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &monoModeToggle, .paddingBottom = 0 });

    auto imgArea = mRect.removeFromBottom(100);
    algImageComp.setBounds(imgArea);
    mRect.removeFromTop(GuiValue::Category::paddingTop);

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

        // 枠線
        opGroups[i].setBounds(opArea.reduced(2));

        // 枠線の内側
        auto innerRect = opArea.reduced(GuiValue::Fm::Op::Padding::width, GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        bool rgMode = rgEn[i].getToggleState();

        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMain[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &dt[i].label, .component = &dt[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &rgEn[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        updateRgDisplayAsOp(i, rgMode);
        if (rgMode)
        {
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgDr[i].label, .component = &rgDr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgSr[i].label, .component = &rgSr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgSl[i].label, .component = &rgSl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        }
        else
        {
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ar[i].label, .component = &ar[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &dr[i].label, .component = &dr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &sr[i].label, .component = &sr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &sl[i].label, .component = &sl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rr[i].label, .component = &rr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &tl[i].label, .component = &tl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        }
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &cafFix[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &fix[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &freq[i].label, .component = &freq[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRFixFreqBtns({ .rect = innerRect, .to0Btn = &freqToZero[i], .to440Btn = &freqTo440[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMask[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &mask[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMml[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &mml[i] });
    }

    updateAlgorithmDisplay();
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

    // KS
    val = RegisterConverter::getValue(input, mmlPrefixKs, mmlValues::opn::ks);
    if (RegisterConverter::isValidVal(val)) ks[opIndex].setSelectedItemIndex(RegisterConverter::convertFmKs(val), juce::sendNotification);

    // MASK
    val = RegisterConverter::getValue(input, mmlPrefixMask, mmlValues::opn::mask);
    if (RegisterConverter::isValidVal(val)) mask[opIndex].setToggleState(RegisterConverter::convertFmMask(val), juce::sendNotification);

    bool rgMode = rgEn[opIndex].getToggleState();

    if (rgMode)
    {
        // TL
        val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opn::tl);
        if (RegisterConverter::isValidVal(val)) rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(val), juce::sendNotification);

        // AR
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opn::ar);
        if (RegisterConverter::isValidVal(val)) rgAr[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // DR
        val = RegisterConverter::getValue(input, mmlPrefixDr, mmlValues::opn::dr);
        if (RegisterConverter::isValidVal(val)) rgDr[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // SR
        val = RegisterConverter::getValue(input, mmlPrefixSr, mmlValues::opn::sr);
        if (RegisterConverter::isValidVal(val)) rgSr[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // SL
        val = RegisterConverter::getValue(input, mmlPrefixSl, mmlValues::opn::sl);
        if (RegisterConverter::isValidVal(val)) rgSl[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        // RR
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opn::rr);
        if (RegisterConverter::isValidVal(val)) rgRr[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        return;
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

void GuiOpn::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    catMain[idx].setEnabled(enable);
    mul[idx].setEnabled(enable);
    mul[idx].label.setEnabled(enable);
    dt[idx].setEnabled(enable);
    dt[idx].label.setEnabled(enable);
    ar[idx].setEnabled(enable);
    ar[idx].label.setEnabled(enable);
    dr[idx].setEnabled(enable);
    dr[idx].label.setEnabled(enable);
    sl[idx].setEnabled(enable);
    sl[idx].label.setEnabled(enable);
    rr[idx].setEnabled(enable);
    rr[idx].label.setEnabled(enable);
    sr[idx].setEnabled(enable);
    sr[idx].label.setEnabled(enable);
    tl[idx].setEnabled(enable);
    tl[idx].label.setEnabled(enable);
    ks[idx].setEnabled(enable);
    ks[idx].label.setEnabled(enable);
    cafFix[idx].setEnabled(enable);
    fix[idx].setEnabled(enable);
    freq[idx].setEnabled(enable);
    freq[idx].label.setEnabled(enable);
    freqToZero[idx].setEnabled(enable);
    freqTo440[idx].setEnabled(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    catMml[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
}

void GuiOpn::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > 7) return;

    for (int i = 0; i < 4; ++i)
    {
        juce::String newTitle = GuiText::Group::opPrefix + juce::String(i + 1) + algOpPrefix[algIndex][i];

        opGroups[i].setText(newTitle);
    }

    // ==========================================================
    // 画像の切り替え
    // ==========================================================
    if (algImages[algIndex].isValid())
    {
        // 読み込めている場合はその画像をセット
        // centred | onlyReduceInSize を指定すると、アスペクト比を保ったまま綺麗に収まります
        algImageComp.setImage(algImages[algIndex], juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
    }
    else
    {
        // 画像がない場合（ファイルが見つからなかった時など）はクリア
        algImageComp.setImage(juce::Image());
    }
}

void GuiOpn::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].label.setVisible(rgMode);
    rgAr[idx].setVisible(rgMode);
    rgDr[idx].label.setVisible(rgMode);
    rgDr[idx].setVisible(rgMode);
    rgSl[idx].label.setVisible(rgMode);
    rgSl[idx].setVisible(rgMode);
    rgSr[idx].label.setVisible(rgMode);
    rgSr[idx].setVisible(rgMode);
    rgRr[idx].label.setVisible(rgMode);
    rgRr[idx].setVisible(rgMode);
    rgTl[idx].label.setVisible(rgMode);
    rgTl[idx].setVisible(rgMode);

    ar[idx].label.setVisible(!rgMode);
    ar[idx].setVisible(!rgMode);
    dr[idx].label.setVisible(!rgMode);
    dr[idx].setVisible(!rgMode);
    sl[idx].label.setVisible(!rgMode);
    sl[idx].setVisible(!rgMode);
    sr[idx].label.setVisible(!rgMode);
    sr[idx].setVisible(!rgMode);
    rr[idx].label.setVisible(!rgMode);
    rr[idx].setVisible(!rgMode);
    tl[idx].label.setVisible(!rgMode);
    tl[idx].setVisible(!rgMode);
}

void GuiOpn::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}
