#include "GuiOpl3.h"

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

static std::vector<SelectItem> opl3AlgItems = {
    {.name = "00: <OPL3-00>", .value = 1 },
    {.name = "01: <OPL3-01>", .value = 2 },
    {.name = "02: <OPL3-02>", .value = 3 },
    {.name = "03: <OPL3-03>", .value = 4 },
};

static std::vector<SelectItem> kslItems = {
    {.name = "KSL: 0", .value = 1},
    {.name = "KSL: 1", .value = 2},
    {.name = "KSL: 2", .value = 3},
    {.name = "KSL: 3", .value = 4}
};

static std::vector<SelectItem> opl3EgItems = {
    {.name = "0: Sine",                 .value = 1},
    {.name = "1: Half Sine",            .value = 2},
    {.name = "2: Abs Sine",             .value = 3},
    {.name = "3: Pulse",                .value = 4},
    {.name = "4: Alternative Sine",     .value = 5},
    {.name = "5: Alternative Abs Sine", .value = 6},
    {.name = "6: Square",               .value = 7},
    {.name = "7: Derived Square",       .value = 8},
};

void GuiOpl3::setup()
{
    const juce::String code = PrKey::Prefix::opl3;

    monoPolyCat.setup({ .parent = *this, .title = GuiText::Category::monoMode });
    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });
    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    algFbCat.setup({ .parent = *this, .title = GuiText::Category::algFb });

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Fm::alg, .items = opl3AlgItems, .isReset = true });
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

    for (int i = 0; i < 4; ++i)
    {
        juce::String fileName = juce::String::formatted(Io::Folder::asset + "/" + Io::Folder::resource + "/ALG_OPL3_%02d.png", i);
        auto imgFile = docDir.getChildFile(fileName);

        if (imgFile.existsAsFile()) {
            algImages[i] = juce::ImageFileFormat::loadFrom(imgFile);
        }
    }

    // 画像コンポーネントを画面に追加
    addAndMakeVisible(algImageComp);

    const juce::String opCode = code + PrKey::Innder::op;

    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, GuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 DR:0 SL:0 RR:15 TL:0 MUL:1",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        catMain[i].setup({ .parent = *this, .title = GuiText::Category::m });
        catLfo[i].setup({ .parent = *this, .title = GuiText::Category::lfo });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mul, .title = GuiText::Fm::Op::Mul, .isReset = true, .regType = RegisterType::FmMul });
        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::tl, .title = GuiText::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ar, .title = GuiText::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        arTo000[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::ArTo000, .isReset = false, .isResized = false });
        arTo000[i].onClick = [this, index = i] { ar[index].setValue(0.00, juce::sendNotification); };
        arTo003[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::ArTo003, .isReset = false, .isResized = false });
        arTo003[i].onClick = [this, index = i] { ar[index].setValue(0.03, juce::sendNotification); };
        dr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dr, .title = GuiText::Fm::Op::Dr, .isReset = true, .regType = RegisterType::FmDr });
        sl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sl, .title = GuiText::Fm::Op::Sl, .isReset = true, .regType = RegisterType::FmSl });
        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rr, .title = GuiText::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });
        rrTo000[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::RrTo000, .isReset = false, .isResized = false });
        rrTo000[i].onClick = [this, index = i] { rr[index].setValue(0.00, juce::sendNotification); };
        rrTo003[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::RrTo003, .isReset = false, .isResized = false });
        rrTo003[i].onClick = [this, index = i] { rr[index].setValue(0.03, juce::sendNotification); };

        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::am, .title = GuiText::Fm::Op::Am, .isReset = true });

        vib[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::vib, .title = GuiText::Fm::Op::Vib, .isReset = true });

        ams[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ams, .title = GuiText::Fm::Op::Ams, .isReset = true });
        amd[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::amd, .title = GuiText::Fm::Op::Amd, .isReset = true });
        pms[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pms, .title = GuiText::Fm::Op::Pms, .isReset = true });
        pmd[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pmd, .title = GuiText::Fm::Op::Pmd, .isReset = true });

        amsTo37[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->3.7Hz", .isReset = false, .isResized = false });
        amsTo37[i].onClick = [this, index = i] { ams[index].setValue(3.7, juce::sendNotification); };
        amdTo1[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->1dB", .isReset = false, .isResized = false });
        amdTo1[i].onClick = [this, index = i] { amd[index].setValue(1.0, juce::sendNotification); };
        amdTo48[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->4.8dB", .isReset = false, .isResized = false });
        amdTo48[i].onClick = [this, index = i] { amd[index].setValue(4.8, juce::sendNotification); };
        pmsTo64[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->6.4Hz", .isReset = false, .isResized = false });
        pmsTo64[i].onClick = [this, index = i] { pms[index].setValue(6.4, juce::sendNotification); };
        pmdTo7[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->7cent", .isReset = false, .isResized = false });
        pmdTo7[i].onClick = [this, index = i] { pmd[index].setValue(7.0, juce::sendNotification); };
        pmdTo14[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->14cent", .isReset = false, .isResized = false });
        pmdTo14[i].onClick = [this, index = i] { pmd[index].setValue(14.0, juce::sendNotification); };

        egType[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::egType, .title = GuiText::Fm::Op::EgType, .isReset = true });

        ksr[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ksr, .title = GuiText::Fm::Op::Ksr, .isReset = true });
        ksl[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ksl, .title = GuiText::Fm::Op::Ksl, .items = kslItems, .isReset = true });

        catShape[i].setup({ .parent = *this, .title = GuiText::Category::shape });

        eg[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::eg, .title = GuiText::Fm::Op::Eg, .items = opl3EgItems, .isReset = true });

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
        rgRr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgRr, .title = GuiText::Fm::Op::Rr, .isReset = true });
        rgTl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgTl, .title = GuiText::Fm::Op::Tl, .isReset = true });
    }
}

void GuiOpl3::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMain({ .mainRect = mRect, .label = &presetNameCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = GuiValue::PresetName::paddingBottom });
    layoutMain({ .mainRect = mRect, .label = &qualityCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &bitSelector.label, .component = &bitSelector });
    layoutMain({ .mainRect = mRect, .label = &rateSelector.label, .component = &rateSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &algFbCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });
    layoutMain({ .mainRect = mRect, .label = &feedback2Slider.label, .component = &feedback2Slider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &monoPolyCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .component = &monoModeToggle, .paddingBottom = 0 });

    auto imgArea = mRect.removeFromBottom(100);
    algImageComp.setBounds(imgArea);
    mRect.removeFromTop(GuiValue::Category::paddingTop);

    // --- Operators Section ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);
        opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(GuiValue::Fm::Op::Padding::width, GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        bool rgMode = rgEn[i].getToggleState();

        layoutRow({ .rowRect = innerRect, .component = &catMain[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &rgEn[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        updateRgDisplayAsOp(i, rgMode);
        if (rgMode)
        {
            layoutRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rgDr[i].label, .component = &rgDr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rgSl[i].label, .component = &rgSl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        }
        else
        {
            layoutRow({ .rowRect = innerRect, .label = &ar[i].label, .component = &ar[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRowTwoComps({ .rect = innerRect, .comp1 = &arTo000[i], .comp2 = &arTo003[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &dr[i].label, .component = &dr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &sl[i].label, .component = &sl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rr[i].label, .component = &rr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRowTwoComps({ .rect = innerRect, .comp1 = &rrTo000[i], .comp2 = &rrTo003[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &tl[i].label, .component = &tl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        }
        layoutRow({ .rowRect = innerRect, .component = &egType[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &ksr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &ksl[i].label, .component = &ksl[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &catShape[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .label = &eg[i].label, .component = &eg[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &catLfo[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .component = &am[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &ams[i].label, .component = &ams[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &amsTo37[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &amd[i].label, .component = &amd[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRowTwoComps({ .rect = innerRect, .comp1 = &amdTo1[i], .comp2 = &amdTo48[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &vib[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &pms[i].label, .component = &pms[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &pmsTo64[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &pmd[i].label, .component = &pmd[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRowTwoComps({ .rect = innerRect, .comp1 = &pmdTo7[i], .comp2 = &pmdTo14[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &catMask[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .component = &mask[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &catMml[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });
    }

    updateAlgorithmDisplay();
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpl3::applyMmlString(const juce::String& mml, int opIndex)
{
    juce::String input = mml.toUpperCase();
    int val;

    // MUL
    val = RegisterConverter::getValue(input, mmlPrefixMul, mmlValues::opl3::mul);
    if (RegisterConverter::isValidVal(val)) mul[opIndex].setValue((double)RegisterConverter::convertOplMul(val), juce::sendNotification);

    // AM
    val = RegisterConverter::getValue(input, mmlPrefixAm, mmlValues::opl3::am);
    if (RegisterConverter::isValidVal(val)) am[opIndex].setToggleState(RegisterConverter::convertOplAm(val), juce::sendNotification);

    // VIB
    val = RegisterConverter::getValue(input, mmlPrefixVib, mmlValues::opl3::vib);
    if (RegisterConverter::isValidVal(val)) vib[opIndex].setToggleState(RegisterConverter::convertOplVib(val), juce::sendNotification);

    // EGTYPE
    val = RegisterConverter::getValue(input, mmlPrefixEgType, mmlValues::opl3::egtype);
    if (RegisterConverter::isValidVal(val)) egType[opIndex].setToggleState(RegisterConverter::convertOplEgType(val), juce::sendNotification);

    // KSR
    val = RegisterConverter::getValue(input, mmlPrefixKsr, mmlValues::opl3::ksr);
    if (RegisterConverter::isValidVal(val)) ksr[opIndex].setToggleState(RegisterConverter::convertOplKsr(val), juce::sendNotification);

    // KSL
    val = RegisterConverter::getValue(input, mmlPrefixKsl, mmlValues::opl3::ksl);
    if (RegisterConverter::isValidVal(val)) ksl[opIndex].setSelectedItemIndex(RegisterConverter::convertOplKsl(val), juce::sendNotification);

    // MASK
    val = RegisterConverter::getValue(input, mmlPrefixMask, mmlValues::opl3::mask);
    if (RegisterConverter::isValidVal(val)) mask[opIndex].setToggleState(RegisterConverter::convertFmMask(val), juce::sendNotification);

    bool rgMode = rgEn[opIndex].getToggleState();

    if (rgMode)
    {
        // TL
        val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opl3::tl);
        if (RegisterConverter::isValidVal(val)) rgTl[opIndex].setValue(RegisterConverter::convertFmRg63(val), juce::sendNotification);

        // AR
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opl3::ar);
        if (RegisterConverter::isValidVal(val)) rgAr[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        // DR
        val = RegisterConverter::getValue(input, mmlPrefixDr, mmlValues::opl3::dr);
        if (RegisterConverter::isValidVal(val)) rgDr[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        // SL
        val = RegisterConverter::getValue(input, mmlPrefixSl, mmlValues::opl3::sl);
        if (RegisterConverter::isValidVal(val)) rgSl[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        // RR
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opl3::rr);
        if (RegisterConverter::isValidVal(val)) rgRr[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        return;
    }

    // TL
    val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opl3::tl);
    if (RegisterConverter::isValidVal(val)) tl[opIndex].setValue(RegisterConverter::convertOplTl(val), juce::sendNotification);

    // AR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRar, mmlValues::opl3::ar);
    if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertOplAr(mmlValues::opl3::ar - val), juce::sendNotification);
    // AR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opl3::ar);
        if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertOplAr(val), juce::sendNotification);
    }

    // DR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRdr, mmlValues::opl3::dr);
    if (RegisterConverter::isValidVal(val)) dr[opIndex].setValue(RegisterConverter::convertOplDr(mmlValues::opl3::dr - val), juce::sendNotification);
    // DR
    else
    {
        val = RegisterConverter::getValue(input, mmlPrefixDr, mmlValues::opl3::dr);
        if (RegisterConverter::isValidVal(val)) dr[opIndex].setValue(RegisterConverter::convertOplDr(val), juce::sendNotification);
    }

    // SL
    val = RegisterConverter::getValue(input, mmlPrefixSl, mmlValues::opl3::sl);
    if (RegisterConverter::isValidVal(val)) sl[opIndex].setValue(RegisterConverter::convertOplSl(val), juce::sendNotification);

    // RR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRrr, mmlValues::opl3::rr);
    if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertOplRr(mmlValues::opl3::rr - val), juce::sendNotification);
    // RR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opl3::rr);
        if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertOplRr(val), juce::sendNotification);
    }
}

void GuiOpl3::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    catMain[idx].setEnabled(enable);
    mul[idx].setEnabled(enable);
    mul[idx].label.setEnabled(enable);
    ar[idx].setEnabled(enable);
    ar[idx].label.setEnabled(enable);
    arTo000[idx].setEnabled(enable);
    arTo003[idx].setEnabled(enable);
    dr[idx].setEnabled(enable);
    dr[idx].label.setEnabled(enable);
    sl[idx].setEnabled(enable);
    sl[idx].label.setEnabled(enable);
    rr[idx].setEnabled(enable);
    rr[idx].label.setEnabled(enable);
    rrTo000[idx].setEnabled(enable);
    rrTo003[idx].setEnabled(enable);
    tl[idx].setEnabled(enable);
    tl[idx].label.setEnabled(enable);
    ksr[idx].setEnabled(enable);
    ksl[idx].setEnabled(enable);
    ksl[idx].label.setEnabled(enable);
    egType[idx].setEnabled(enable);
    eg[idx].setEnabled(enable);
    eg[idx].label.setEnabled(enable);
    catShape[idx].setEnabled(enable);
    catLfo[idx].setEnabled(enable);
    vib[idx].setEnabled(enable);
    pms[idx].setEnabled(enable);
    pms[idx].label.setEnabled(enable);
    pmsTo64[idx].setEnabled(enable);
    pmd[idx].setEnabled(enable);
    pmd[idx].label.setEnabled(enable);
    pmdTo7[idx].setEnabled(enable);
    pmdTo14[idx].setEnabled(enable);
    am[idx].setEnabled(enable);
    ams[idx].setEnabled(enable);
    ams[idx].label.setEnabled(enable);
    amsTo37[idx].setEnabled(enable);
    amd[idx].setEnabled(enable);
    amd[idx].label.setEnabled(enable);
    amdTo1[idx].setEnabled(enable);
    amdTo48[idx].setEnabled(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    catMml[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
}

void GuiOpl3::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > 3) return;

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

void GuiOpl3::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].label.setVisible(rgMode);
    rgAr[idx].setVisible(rgMode);
    rgDr[idx].label.setVisible(rgMode);
    rgDr[idx].setVisible(rgMode);
    rgSl[idx].label.setVisible(rgMode);
    rgSl[idx].setVisible(rgMode);
    rgRr[idx].label.setVisible(rgMode);
    rgRr[idx].setVisible(rgMode);
    rgTl[idx].label.setVisible(rgMode);
    rgTl[idx].setVisible(rgMode);

    ar[idx].label.setVisible(!rgMode);
    ar[idx].setVisible(!rgMode);
    arTo000[idx].setVisible(!rgMode);
    arTo003[idx].setVisible(!rgMode);
    dr[idx].label.setVisible(!rgMode);
    dr[idx].setVisible(!rgMode);
    sl[idx].label.setVisible(!rgMode);
    sl[idx].setVisible(!rgMode);
    rr[idx].label.setVisible(!rgMode);
    rr[idx].setVisible(!rgMode);
    rrTo000[idx].setVisible(!rgMode);
    rrTo003[idx].setVisible(!rgMode);
    tl[idx].label.setVisible(!rgMode);
    tl[idx].setVisible(!rgMode);
}

void GuiOpl3::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}
