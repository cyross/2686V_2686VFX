#include "GuiOpna.h"

#include <vector>

#include "../processor/PluginProcessor.h"

#include "../core/FileValues.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/MmlKeys.h"
#include "../core/MmlValues.h"

#include "../fm/RegisterConverter.h"

#include "../gui/GuiHelpers.h"
#include "../gui/GuiValues.h"
#include "../gui/GuiStructs.h"
#include "../gui/GuiText.h"

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

static std::vector<SelectItem> opnaAlgItems = {
    {.name = "00: <OPNA-00>", .value = 1 },
    {.name = "01: <OPNA-01>", .value = 2 },
    {.name = "02: <OPNA-02>", .value = 3 },
    {.name = "03: <OPNA-03>", .value = 4 },
    {.name = "04: <OPNA-04>", .value = 5 },
    {.name = "05: <OPNA-05>", .value = 6 },
    {.name = "06: <OPNA-06>", .value = 7 },
    {.name = "07: <OPNA-07>", .value = 8 },
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

static std::vector<SelectItem> ksItems = {
    {.name = "0 OFF",      .value = 1},
    {.name = "1 (Weak)",   .value = 2},
    {.name = "2 (Mid)",    .value = 3},
    {.name = "3 (Strong)", .value = 4}
};

static std::vector<SelectItem> opnaLfoShapeItems = {
    {.name = "0: Saw Up",   .value = 1 },
    {.name = "1: Square",   .value = 2 },
    {.name = "2: Triangle", .value = 3 },
    {.name = "3: Noise",    .value = 4 },
};

static std::vector<SelectItem> opnaSeItems = {
    {.name = "0: Normal",                      .value = 1 },
    {.name = "1: Saw Down",                    .value = 2 },
    {.name = "2: Saw Down & Hold",             .value = 3 },
    {.name = "3: Triangle",                    .value = 4 },
    {.name = "4: Alternative Saw Down & Hold", .value = 5 },
    {.name = "5: Saw Up",                      .value = 6 },
    {.name = "6: Saw Up & Hold",               .value = 7 },
    {.name = "7: Triangle Invert",             .value = 8 },
    {.name = "8: Alternative Saw Up & Hold",   .value = 9 },
};

static std::vector<SelectItem> ssgEnvItems = {
    {.name = "0: Saw Down",                    .value = 1 },
    {.name = "1: Saw Down & Hold",             .value = 2 },
    {.name = "2: Triangle",                    .value = 3 },
    {.name = "3: Alternative Saw Down & Hold", .value = 4 },
    {.name = "4: Saw Up",                      .value = 5 },
    {.name = "5: Saw Up & Hold",               .value = 6 },
    {.name = "6: Triangle Invert",             .value = 7 },
    {.name = "7: Alternative Saw Up & Hold",   .value = 8 },
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

static std::vector<SelectItem> lfoShapeItems = {
    {.name = "0: Sine",     .value = 1 },
    {.name = "1: Saw Down", .value = 2 },
    {.name = "2: Square",   .value = 3 },
    {.name = "3: Triangle", .value = 4 },
    {.name = "4: Noise",    .value = 5 },
};

static std::vector<SelectItem> lfoFreqsItems = {
    {.name = "0: 3.98Hz", .value = 1 },
    {.name = "1: 5.56Hz", .value = 2 },
    {.name = "2: 6.02Hz", .value = 3 },
    {.name = "3: 6.37Hz", .value = 4 },
    {.name = "4: 6.88Hz", .value = 5 },
    {.name = "5: 9.63Hz", .value = 6 },
    {.name = "6: 48.1Hz", .value = 7 },
    {.name = "7: 72.2Hz", .value = 8 }
};

static std::vector<SelectItem> hwPmsItems = {
    {.name = "0: 0cent",   .value = 1 },
    {.name = "1: +-3.4cent", .value = 2 },
    {.name = "2: +-6.7cent", .value = 3 },
    {.name = "3: +-10cent",  .value = 4 },
    {.name = "4: +-14cent",  .value = 5 },
    {.name = "5: +-20cent",  .value = 6 },
    {.name = "6: +-40cent",  .value = 7 },
    {.name = "7: +-80cent",  .value = 8 },
};

static std::vector<SelectItem> hwAmsItems = {
    {.name = "0: 0dB", .value = 1 },
    {.name = "1: 1.4dB", .value = 2 },
    {.name = "2: 5.9dB", .value = 3 },
    {.name = "3: 11.8dB", .value = 4 },
};

void GuiOpna::setup()
{
    const juce::String code = PrKey::Prefix::opna;
    int tabOrder = 1;

    mainGroup.setup(*this, GuiText::Group::mainGroup);

    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });

    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setup({ .parent = *this, .title = GuiText::Category::algFb });

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Fm::alg, .items = opnaAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
    };

    feedbackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb0, .title = GuiText::Fm::fb0, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    feedback2Slider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb2, .title = GuiText::Fm::fb2, .isReset = true });
    feedback2Slider.setWantsKeyboardFocus(true);
    feedback2Slider.setExplicitFocusOrder(++tabOrder);

    lfoCat.setup({ .parent = *this, .title = GuiText::Category::n88Lfo });

    lfoFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::freq, .title = GuiText::Fm::lfoFreq, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoShapeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::shape, .title = GuiText::Fm::lfoShape, .items = lfoShapeItems, .isReset = true });
    lfoShapeSelector.setWantsKeyboardFocus(true);
    lfoShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::amSmoothRatio, .title = GuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::syncDelay, .title = GuiText::Fm::lfoSyncDelay, .isReset = true });
    lfoSyncDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoSyncDelaySlider.setWantsKeyboardFocus(true);
    lfoSyncDelaySlider.setExplicitFocusOrder(++tabOrder);

    lfoPmToggle.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pm, .title = GuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pms, .title = GuiText::Fm::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pmd, .title = GuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::am, .title = GuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmsSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::ams, .title = GuiText::Fm::ams, .isReset = true });
    lfoAmsSlider.setWantsKeyboardFocus(true);
    lfoAmsSlider.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setup({ .parent = *this, .title = GuiText::Category::monoMode });

    monoModeToggle.setup({ .parent = *this, .id = PrKey::monoMode, .title = GuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    for (int i = 0; i < 8; ++i)
    {
        // "%02d" で i が 0〜7 のときに "00" 〜 "07" という文字列を作ります
        juce::String fileName = juce::String::formatted(Io::Folder::asset + "/" + Io::Folder::resource + "/ALG_OPNA_OPN_OPM_%02d.png", i);
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

        catMain[i].setup({ .parent = *this, .title = GuiText::Category::m });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mul, .title = GuiText::Fm::Op::Mul, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);

        dt[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dt, .title = GuiText::Fm::Op::Dt, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        dt[i].setWantsKeyboardFocus(true);
        dt[i].setExplicitFocusOrder(++tabOrder);

        rgEn[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgEn, .title = GuiText::Fm::Op::RgEn, .isReset = true });
        rgEn[i].setWantsKeyboardFocus(true);
        rgEn[i].setExplicitFocusOrder(++tabOrder);
        rgEn[i].onStateChange = [this, i] {
            ctx.editor.resized();
            };

        rgAr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgAr, .title = GuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgDr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgDr, .title = GuiText::Fm::Op::Dr, .isReset = true });
        rgDr[i].setWantsKeyboardFocus(true);
        rgDr[i].setExplicitFocusOrder(++tabOrder);

        rgSl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgSl, .title = GuiText::Fm::Op::Sl, .isReset = true });
        rgSl[i].setWantsKeyboardFocus(true);
        rgSl[i].setExplicitFocusOrder(++tabOrder);

        rgSr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgSr, .title = GuiText::Fm::Op::Sr, .isReset = true });
        rgSr[i].setWantsKeyboardFocus(true);
        rgSr[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgRr, .title = GuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgTl, .title = GuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ar, .title = GuiText::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        ar[i].setWantsKeyboardFocus(true);
        ar[i].setExplicitFocusOrder(++tabOrder);

        arTo000[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::ArTo000, .isReset = false, .isResized = false });
        arTo000[i].setWantsKeyboardFocus(true);
        arTo000[i].setExplicitFocusOrder(++tabOrder);
        arTo000[i].onClick = [this, index = i] { ar[index].setValue(0.00, juce::sendNotification); };

        arTo003[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::ArTo003, .isReset = false, .isResized = false });
        arTo003[i].setWantsKeyboardFocus(true);
        arTo003[i].setExplicitFocusOrder(++tabOrder);
        arTo003[i].onClick = [this, index = i] { ar[index].setValue(0.03, juce::sendNotification); };

        dr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dr, .title = GuiText::Fm::Op::Dr, .isReset = true, .regType = RegisterType::FmDr });
        dr[i].setWantsKeyboardFocus(true);
        dr[i].setExplicitFocusOrder(++tabOrder);

        sl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sl, .title = GuiText::Fm::Op::Sl, .isReset = true, .regType = RegisterType::FmSl });
        sl[i].setWantsKeyboardFocus(true);
        sl[i].setExplicitFocusOrder(++tabOrder);

        sr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::sr, .title = GuiText::Fm::Op::Sr, .isReset = true, .regType = RegisterType::FmSr });
        sr[i].setWantsKeyboardFocus(true);
        sr[i].setExplicitFocusOrder(++tabOrder);

        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rr, .title = GuiText::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });
        rr[i].setWantsKeyboardFocus(true);
        rr[i].setExplicitFocusOrder(++tabOrder);

        rrTo000[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::RrTo000, .isReset = false, .isResized = false });
        rrTo000[i].onClick = [this, index = i] { rr[index].setValue(0.00, juce::sendNotification); };

        rrTo003[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::RrTo003, .isReset = false, .isResized = false });
        rrTo003[i].onClick = [this, index = i] { rr[index].setValue(0.03, juce::sendNotification); };

        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::tl, .title = GuiText::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });
        tl[i].setWantsKeyboardFocus(true);
        tl[i].setExplicitFocusOrder(++tabOrder);

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ks, .title = GuiText::Fm::Op::Ks, .items = ksItems, .isReset = true, .regType = RegisterType::FmKs });
        ks[i].setWantsKeyboardFocus(true);
        ks[i].setExplicitFocusOrder(++tabOrder);

        catShape[i].setup({ .parent = *this, .title = GuiText::Category::shape });

        se[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::se, .title = GuiText::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
        se[i].setWantsKeyboardFocus(true);
        se[i].setExplicitFocusOrder(++tabOrder);

        seFreq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::seFreq, .title = GuiText::Fm::Op::SFreq, .isReset = true });
        seFreq[i].setWantsKeyboardFocus(true);
        seFreq[i].setExplicitFocusOrder(++tabOrder);

        catLfo[i].setup({ .parent = *this, .title = GuiText::Category::hwLfo });

        freqs[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::freqs, .title = GuiText::Fm::Op::Freqs, .items = lfoFreqsItems, .isReset = true });
        freqs[i].setWantsKeyboardFocus(true);
        freqs[i].setExplicitFocusOrder(++tabOrder);

        syncDelay[i].setup({.parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::syncDelay, .title = GuiText::Fm::Op::SyncDelay, .isReset = true});
        syncDelay[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        syncDelay[i].setWantsKeyboardFocus(true);
        syncDelay[i].setExplicitFocusOrder(++tabOrder);

        pm[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::vib, .title = GuiText::Fm::Op::PmEn, .isReset = true });
        pm[i].setWantsKeyboardFocus(true);
        pm[i].setExplicitFocusOrder(++tabOrder);

        pms[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pms, .title = GuiText::Fm::Op::Pms, .items = hwPmsItems, .isReset = true });
        pms[i].setWantsKeyboardFocus(true);
        pms[i].setExplicitFocusOrder(++tabOrder);

        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::am, .title = GuiText::Fm::Op::AmEn, .isReset = true });
        am[i].setWantsKeyboardFocus(true);
        am[i].setExplicitFocusOrder(++tabOrder);

        ams[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ams, .title = GuiText::Fm::Op::Ams, .items = hwAmsItems, .isReset = true });
        ams[i].setWantsKeyboardFocus(true);
        ams[i].setExplicitFocusOrder(++tabOrder);

        catN88Lfo[i].setup({ .parent = *this, .title = GuiText::Category::n88Lfo });

        amd[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::amd, .title = GuiText::Fm::Op::Amd, .isReset = true });
        amd[i].setWantsKeyboardFocus(true);
        amd[i].setExplicitFocusOrder(++tabOrder);

        cafFix[i].setup({ .parent = *this, .title = GuiText::Category::fix });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fix, .title = GuiText::Fm::Op::Fix, .isReset = true });
        fix[i].setWantsKeyboardFocus(true);
        fix[i].setExplicitFocusOrder(++tabOrder);

        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fixFreq, .title = GuiText::Fm::Op::FFreq, .isReset = true });
        freq[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        freq[i].setWantsKeyboardFocus(true);
        freq[i].setExplicitFocusOrder(++tabOrder);

        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].setWantsKeyboardFocus(true);
        freqToZero[i].setExplicitFocusOrder(++tabOrder);
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };

        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].setWantsKeyboardFocus(true);
        freqTo440[i].setExplicitFocusOrder(++tabOrder);
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        catMask[i].setup({ .parent = *this, .title = GuiText::Category::mask });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mask, .title = GuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        catMml[i].setup({ .parent = *this, .title = GuiText::Category::mml });

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setWantsKeyboardFocus(true);
        mml[i].setExplicitFocusOrder(++tabOrder);
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 DR:0 SL:0 SR:0 RR:15 TL:0 MUL:1 DT:0",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });
    }
}

void GuiOpna::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMainCategory({ .mainRect = mRect, .label = &presetNameCat });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = GuiValue::PresetName::paddingBottom });
    layoutMainCategory({ .mainRect = mRect, .label = &qualityCat });
    layoutMain({ .mainRect = mRect, .label = &bitSelector.label, .component = &bitSelector });
    layoutMain({ .mainRect = mRect, .label = &rateSelector.label, .component = &rateSelector });
    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });
    layoutMain({ .mainRect = mRect, .label = &feedback2Slider.label, .component = &feedback2Slider });
    layoutMainCategory({ .mainRect = mRect, .label = &lfoCat });
    layoutMain({ .mainRect = mRect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
    layoutMain({ .mainRect = mRect, .label = &lfoShapeSelector.label, .component = &lfoShapeSelector });
    layoutMain({ .mainRect = mRect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
    layoutMain({ .mainRect = mRect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
    layoutMain({ .mainRect = mRect, .component = &lfoPmToggle });
    layoutMain({ .mainRect = mRect, .label = &lfoPmsSlider.label, .component = &lfoPmsSlider });
    layoutMain({ .mainRect = mRect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
    layoutMain({ .mainRect = mRect, .component = &lfoAmToggle });
    layoutMain({ .mainRect = mRect, .label = &lfoAmsSlider.label, .component = &lfoAmsSlider });
    layoutMainCategory({ .mainRect = mRect, .label = &mvolCat, .paddingTop = GuiValue::MVol::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider });
    layoutMainCategory({ .mainRect = mRect, .label = &monoPolyCat });
    layoutMain({ .mainRect = mRect, .component = &monoModeToggle, .paddingBottom = 0 });

    auto imgArea = mRect.removeFromBottom(100);
    algImageComp.setBounds(imgArea);
    mRect.removeFromTop(GuiValue::Category::paddingTop);

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

        opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(GuiValue::Fm::Op::Padding::width, GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        bool rgMode = rgEn[i].getToggleState();

        layoutRowCategory({ .rowRect = innerRect, .component = &catMain[i] });
        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt[i].label, .component = &dt[i] });
        layoutRow({ .rowRect = innerRect, .component = &rgEn[i] });
        updateRgDisplayAsOp(i, rgMode);
        if (rgMode)
        {
            layoutRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i] });
            layoutRow({ .rowRect = innerRect, .label = &rgDr[i].label, .component = &rgDr[i] });
            layoutRow({ .rowRect = innerRect, .label = &rgSr[i].label, .component = &rgSr[i] });
            layoutRow({ .rowRect = innerRect, .label = &rgSl[i].label, .component = &rgSl[i] });
            layoutRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i] });
            layoutRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i] });
        }
        else
        {
            layoutRow({ .rowRect = innerRect, .label = &ar[i].label, .component = &ar[i] });
            layoutRowTwoComps({ .rect = innerRect, .comp1 = &arTo000[i], .comp2 = &arTo003[i] });
            layoutRow({ .rowRect = innerRect, .label = &dr[i].label, .component = &dr[i] });
            layoutRow({ .rowRect = innerRect, .label = &sr[i].label, .component = &sr[i] });
            layoutRow({ .rowRect = innerRect, .label = &sl[i].label, .component = &sl[i] });
            layoutRow({ .rowRect = innerRect, .label = &rr[i].label, .component = &rr[i] });
            layoutRowTwoComps({ .rect = innerRect, .comp1 = &rrTo000[i], .comp2 = &rrTo003[i] });
            layoutRow({ .rowRect = innerRect, .label = &tl[i].label, .component = &tl[i] });
        }
        layoutRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i] });
        layoutRow({ .rowRect = innerRect, .component = &catShape[i] });
        layoutRow({ .rowRect = innerRect, .label = &se[i].label, .component = &se[i] });
        layoutRow({ .rowRect = innerRect, .label = &seFreq[i].label, .component = &seFreq[i] });
        layoutRowCategory({ .rowRect = innerRect, .component = &catLfo[i] });
        layoutRow({ .rowRect = innerRect, .label = &freqs[i].label, .component = &freqs[i] });
        layoutRow({ .rowRect = innerRect, .label = &syncDelay[i].label, .component = &syncDelay[i] });
        layoutRow({ .rowRect = innerRect, .component = &pm[i] });
        layoutRow({ .rowRect = innerRect, .label = &pms[i].label, .component = &pms[i] });
        layoutRow({ .rowRect = innerRect, .component = &am[i] });
        layoutRow({ .rowRect = innerRect, .label = &ams[i].label, .component = &ams[i] });
        layoutRowCategory({ .rowRect = innerRect, .component = &catN88Lfo[i] });
        layoutRow({ .rowRect = innerRect, .label = &amd[i].label, .component = &amd[i] });
        layoutRowCategory({ .rowRect = innerRect, .component = &cafFix[i] });
        layoutRow({ .rowRect = innerRect, .component = &fix[i] });
        layoutRow({ .rowRect = innerRect, .label = &freq[i].label, .component = &freq[i] });
        layoutRowTwoComps({ .rect = innerRect, .comp1 = &freqToZero[i], .comp2 = &freqTo440[i] });
        layoutRowCategory({ .rowRect = innerRect, .component = &catMask[i] });
        layoutRow({ .rowRect = innerRect, .component = &mask[i] });
        layoutRowCategory({ .rowRect = innerRect, .component = &catMml[i] });
        layoutRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });
    }

    updateAlgorithmDisplay();
}


// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpna::applyMmlString(const juce::String& mml, int opIndex)
{
    juce::String input = mml.toUpperCase();
    int val;


    // MUL
    val = RegisterConverter::getValue(input, mmlPrefixMul, mmlValues::opna::mul);
    if (RegisterConverter::isValidVal(val)) mul[opIndex].setValue((double)RegisterConverter::convertFmMul(val), juce::sendNotification);

    // DT
    val = RegisterConverter::getValue(input, mmlPrefixDt, mmlValues::opna::dt);
    if (RegisterConverter::isValidVal(val)) {
        int regVal = RegisterConverter::convertMmlDtToReg(val);

        dt[opIndex].setSelectedItemIndex((double)regVal, juce::sendNotification);
    }
    
    // KS
    val = RegisterConverter::getValue(input, mmlPrefixKs, mmlValues::opna::ks);
    if (RegisterConverter::isValidVal(val)) ks[opIndex].setSelectedItemIndex(RegisterConverter::convertFmKs(val), juce::sendNotification);

    // MASK
    val = RegisterConverter::getValue(input, mmlPrefixMask, mmlValues::opna::mask);
    if (RegisterConverter::isValidVal(val)) mask[opIndex].setToggleState(RegisterConverter::convertFmMask(val), juce::sendNotification);

    bool rgMode = rgEn[opIndex].getToggleState();

    if (rgMode)
    {
        // TL
        val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opna::tl);
        if (RegisterConverter::isValidVal(val)) rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(val), juce::sendNotification);

        // AR
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opna::ar);
        if (RegisterConverter::isValidVal(val)) rgAr[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // DR
        val = RegisterConverter::getValue(input, mmlPrefixDr, mmlValues::opna::dr);
        if (RegisterConverter::isValidVal(val)) rgDr[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // SR
        val = RegisterConverter::getValue(input, mmlPrefixSr, mmlValues::opna::sr);
        if (RegisterConverter::isValidVal(val)) rgSr[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // SL
        val = RegisterConverter::getValue(input, mmlPrefixSl, mmlValues::opna::sl);
        if (RegisterConverter::isValidVal(val)) rgSl[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        // RR
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opna::rr);
        if (RegisterConverter::isValidVal(val)) rgRr[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        return;
    }

    // TL
    val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opna::tl);
    if (RegisterConverter::isValidVal(val)) tl[opIndex].setValue(RegisterConverter::convertFmTl(val), juce::sendNotification);

    // AR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRar, mmlValues::opna::ar);
    if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(mmlValues::opna::ar - val), juce::sendNotification);
    // AR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opna::ar);
        if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(val), juce::sendNotification);
    }

    // DR
    val = RegisterConverter::getValue(input, mmlPrefixRdr, mmlValues::opna::dr);
    if (RegisterConverter::isValidVal(val)) dr[opIndex].setValue(RegisterConverter::convertFmDr(mmlValues::opna::dr - val), juce::sendNotification);
    // DR
    else
    {
        val = RegisterConverter::getValue(input, mmlPrefixDr, mmlValues::opna::dr);
        if (RegisterConverter::isValidVal(val)) dr[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
    }

    // SR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRsr, mmlValues::opna::sr);
    if (RegisterConverter::isValidVal(val)) sr[opIndex].setValue(RegisterConverter::convertFmSr(mmlValues::opna::sr - val), juce::sendNotification);
    // SR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixSr, mmlValues::opna::sr);
        if (RegisterConverter::isValidVal(val)) sr[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
    }

    // SL
    val = RegisterConverter::getValue(input, mmlPrefixSl, mmlValues::opna::sl);
    if (RegisterConverter::isValidVal(val)) sl[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

    // RR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRrr, mmlValues::opna::rr);
    if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(mmlValues::opna::rr - val), juce::sendNotification);
    // RR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opna::rr);
        if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(val), juce::sendNotification);
    }
}

void GuiOpna::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    catMain[idx].setEnabled(enable);
    mul[idx].setEnabled(enable);
    mul[idx].label.setEnabled(enable);
    dt[idx].setEnabled(enable);
    dt[idx].label.setEnabled(enable);
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
    sr[idx].setEnabled(enable);
    sr[idx].label.setEnabled(enable);
    tl[idx].setEnabled(enable);
    tl[idx].label.setEnabled(enable);
    ks[idx].setEnabled(enable);
    ks[idx].label.setEnabled(enable);
    catShape[idx].setEnabled(enable);
    se[idx].setEnabled(enable);
    se[idx].label.setEnabled(enable);
    seFreq[idx].setEnabled(enable);
    seFreq[idx].label.setEnabled(enable);
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

void GuiOpna::updateRgDisplayAsOp(int idx, bool rgMode)
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
    arTo000[idx].setVisible(!rgMode);
    arTo003[idx].setVisible(!rgMode);
    dr[idx].label.setVisible(!rgMode);
    dr[idx].setVisible(!rgMode);
    sl[idx].label.setVisible(!rgMode);
    sl[idx].setVisible(!rgMode);
    sr[idx].label.setVisible(!rgMode);
    sr[idx].setVisible(!rgMode);
    rr[idx].label.setVisible(!rgMode);
    rr[idx].setVisible(!rgMode);
    rrTo000[idx].setVisible(!rgMode);
    rrTo003[idx].setVisible(!rgMode);
    tl[idx].label.setVisible(!rgMode);
    tl[idx].setVisible(!rgMode);
}

void GuiOpna::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

void GuiOpna::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > 7) return;

    // 1. 文字列の更新（既存）
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