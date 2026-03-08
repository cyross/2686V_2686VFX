#include "GuiOpm.h"

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

static std::vector<SelectItem> opmAlgItems = {
    {.name = "00: <OPM-00>", .value = 1 },
    {.name = "01: <OPM-01>", .value = 2 },
    {.name = "02: <OPM-02>", .value = 3 },
    {.name = "03: <OPM-03>", .value = 4 },
    {.name = "04: <OPM-04>", .value = 5 },
    {.name = "05: <OPM-05>", .value = 6 },
    {.name = "06: <OPM-06>", .value = 7 },
    {.name = "07: <OPM-07>", .value = 8 },
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

static std::vector<SelectItem> lfoShapeItems = {
    {.name = "0: Sine",     .value = 1 },
    {.name = "1: Saw Down", .value = 2 },
    {.name = "2: Square",   .value = 3 },
    {.name = "3: Triangle", .value = 4 },
    {.name = "4: Noise",    .value = 5 },
};

static std::vector<SelectItem> pmsItems = {
    {.name = "1: Pms 0", .value = 1 },
    {.name = "2: Pms 1", .value = 2 },
    {.name = "3: Pms 2", .value = 3 },
    {.name = "4: Pms 3", .value = 4 },
    {.name = "5: Pms 4", .value = 5 },
    {.name = "6: Pms 5", .value = 6 },
    {.name = "7: Pms 6", .value = 7 },
    {.name = "8: Pms 7", .value = 8 },
};

static std::vector<SelectItem> amsItems = {
    {.name = "1: Ams 0", .value = 1 },
    {.name = "2: Ams 1", .value = 2 },
    {.name = "3: Ams 2", .value = 3 },
    {.name = "4: Ams 3", .value = 4 },
};

void GuiOpm::setup()
{
    const juce::String code = PrKey::Prefix::opm;

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    algFbCat.setup({ .parent = *this, .title = GuiText::Category::algFb });

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Fm::alg, .items = opmAlgItems, .isReset = true });
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };
    feedbackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb0, .title = GuiText::Fm::fb0, .isReset = true });
    feedback2Slider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb2, .title = GuiText::Fm::fb2, .isReset = true });

    lfoCat.setup({ .parent = *this, .title = GuiText::Category::lfo });

    lfoFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::freq, .title = GuiText::Fm::lfoFreq, .isReset = true });
    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::amSmoothRatio, .title = GuiText::Fm::amSmoothRatio, .isReset = true });
    lfoShapeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::shape, .title = GuiText::Fm::lfoShape, .items = lfoShapeItems, .isReset = true });
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
            .hintMessage = "e.g. AR:31/RAR:0 D1R:0 D1L:0 D2R:0 RR:15 TL:0 MUL:1 DT:10 DT:20",
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
        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };
        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        catLfo[i].setup({ .parent = *this, .title = GuiText::Category::lfo });

        pms[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pms, .title = GuiText::Fm::Op::Pms, .items = pmsItems, .isReset = true });
        pm[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::vib, .title = GuiText::Fm::Op::PmEn, .isReset = true });
        ams[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ams, .title = GuiText::Fm::Op::Ams, .items = amsItems, .isReset = true });
        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::am, .title = GuiText::Fm::Op::AmEn, .isReset = true });

        catMask[i].setup({ .parent = *this, .title = GuiText::Category::mask });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mask, .title = GuiText::Fm::Op::Mask, .isReset = true });

        catMml[i].setup({ .parent = *this, .title = GuiText::Category::mml });

        rgEn[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgEn, .title = GuiText::Fm::Op::RgEn, .isReset = true });
        rgEn[i].onStateChange = [this, i] {
            ctx.editor.resized();
            };
        rgAr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgAr, .title = GuiText::Fm::Op::Ar, .isReset = true });
        rgD1r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgD1r, .title = GuiText::Fm::Op::D1r, .isReset = true });
        rgD1l[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgD1l, .title = GuiText::Fm::Op::D1l, .isReset = true });
        rgD2r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgD2r, .title = GuiText::Fm::Op::D2r, .isReset = true });
        rgRr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgRr, .title = GuiText::Fm::Op::Rr, .isReset = true });
        rgTl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgTl, .title = GuiText::Fm::Op::Tl, .isReset = true });
    }
}

void GuiOpm::layout(juce::Rectangle<int> content)
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
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoShapeSelector.label, .component = &lfoShapeSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &lfoPmToggle });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoPmsSelector.label, .component = &lfoPmsSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &lfoAmToggle });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoAmsSelector.label, .component = &lfoAmsSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0});

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

        // 枠線
        opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(GuiValue::Fm::Op::Padding::width, GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        bool rgMode = rgEn[i].getToggleState();

        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMain[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &dt1[i].label, .component = &dt1[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &dt2[i].label, .component = &dt2[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &rgEn[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        updateRgDisplayAsOp(i, rgMode);
        if (rgMode)
        {
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgD1r[i].label, .component = &rgD1r[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgD1l[i].label, .component = &rgD1l[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgD2r[i].label, .component = &rgD2r[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        }
        else
        {
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ar[i].label, .component = &ar[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &d1r[i].label, .component = &d1r[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &d1l[i].label, .component = &d1l[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &d2r[i].label, .component = &d2r[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &rr[i].label, .component = &rr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &tl[i].label, .component = &tl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        }
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catLfo[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &pm[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &pms[i].label, .component = &pms[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &am[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &ams[i].label, .component = &ams[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &cafFix[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &fix[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .label = &freq[i].label, .component = &freq[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRFixFreqBtns({ .rect = innerRect, .to0Btn = &freqToZero[i], .to440Btn = &freqTo440[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMask[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &mask[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &catMml[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });
    }

    updateAlgorithmDisplay();
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

    // KS
    val = RegisterConverter::getValue(input, mmlPrefixKs, mmlValues::opm::ks);
    if (RegisterConverter::isValidVal(val)) ks[opIndex].setSelectedItemIndex(RegisterConverter::convertFmKs(val), juce::sendNotification);

    // MASK
    val = RegisterConverter::getValue(input, mmlPrefixMask, mmlValues::opm::mask);
    if (RegisterConverter::isValidVal(val)) mask[opIndex].setToggleState(RegisterConverter::convertFmMask(val), juce::sendNotification);

    bool rgMode = rgEn[opIndex].getToggleState();

    if (rgMode)
    {
        // TL
        val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opm::tl);
        if (RegisterConverter::isValidVal(val)) rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(val), juce::sendNotification);

        // AR
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opm::ar);
        if (RegisterConverter::isValidVal(val)) rgAr[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // DR
        val = RegisterConverter::getValue(input, mmlPrefixD1r, mmlValues::opm::d1r);
        if (RegisterConverter::isValidVal(val)) rgD1r[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // SR
        val = RegisterConverter::getValue(input, mmlPrefixD2r, mmlValues::opm::d2r);
        if (RegisterConverter::isValidVal(val)) rgD2r[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // SL
        val = RegisterConverter::getValue(input, mmlPrefixD1l, mmlValues::opm::d1l);
        if (RegisterConverter::isValidVal(val)) rgD1l[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        // RR
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opm::rr);
        if (RegisterConverter::isValidVal(val)) rgRr[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        return;
    }

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

void GuiOpm::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    catMain[idx].setEnabled(enable);
    mul[idx].setEnabled(enable);
    mul[idx].label.setEnabled(enable);
    dt1[idx].setEnabled(enable);
    dt1[idx].label.setEnabled(enable);
    dt2[idx].setEnabled(enable);
    dt2[idx].label.setEnabled(enable);
    ar[idx].setEnabled(enable);
    ar[idx].label.setEnabled(enable);
    d1r[idx].setEnabled(enable);
    d1r[idx].label.setEnabled(enable);
    d1l[idx].setEnabled(enable);
    d1l[idx].label.setEnabled(enable);
    rr[idx].setEnabled(enable);
    rr[idx].label.setEnabled(enable);
    d2r[idx].setEnabled(enable);
    d2r[idx].label.setEnabled(enable);
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
    catLfo[idx].setEnabled(enable);
    pm[idx].setEnabled(enable);
    pms[idx].setEnabled(enable);
    pms[idx].label.setEnabled(enable);
    am[idx].setEnabled(enable);
    ams[idx].setEnabled(enable);
    ams[idx].label.setEnabled(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    catMml[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
}

void GuiOpm::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > 7) return;

    for (int i = 0; i < 4; ++i)
    {
        juce::String newTitle = GuiText::Group::opPrefix + juce::String(i + 1) + algOpPrefix[algIndex][i];

        opGroups[i].setText(newTitle);
    }
}

void GuiOpm::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].label.setVisible(rgMode);
    rgAr[idx].setVisible(rgMode);
    rgD1r[idx].label.setVisible(rgMode);
    rgD1r[idx].setVisible(rgMode);
    rgD1l[idx].label.setVisible(rgMode);
    rgD1l[idx].setVisible(rgMode);
    rgD2r[idx].label.setVisible(rgMode);
    rgD2r[idx].setVisible(rgMode);
    rgRr[idx].label.setVisible(rgMode);
    rgRr[idx].setVisible(rgMode);
    rgTl[idx].label.setVisible(rgMode);
    rgTl[idx].setVisible(rgMode);

    ar[idx].label.setVisible(!rgMode);
    ar[idx].setVisible(!rgMode);
    d1r[idx].label.setVisible(!rgMode);
    d1r[idx].setVisible(!rgMode);
    d1l[idx].label.setVisible(!rgMode);
    d1l[idx].setVisible(!rgMode);
    d2r[idx].label.setVisible(!rgMode);
    d2r[idx].setVisible(!rgMode);
    rr[idx].label.setVisible(!rgMode);
    rr[idx].setVisible(!rgMode);
    tl[idx].label.setVisible(!rgMode);
    tl[idx].setVisible(!rgMode);
}
