#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Core/Const/FileValues.h"
#include "../../Processor/Opna/Keys.h"
#include "../../Processor/Opna/Values.h"
#include "../../Core/Const/MmlKeys.h"
#include "../../Core/Const/MmlValues.h"

#include "../../Core/Fm/RegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiValues.h"
#include "./GuiText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> bdItems = {
    {.name = "1: 4-bit (16 steps)",  .value = 1 },
    {.name = "2: 5-bit (32 steps)",  .value = 2 },
    {.name = "3: 6-bit (64 steps)",  .value = 3 },
    {.name = "4: 8-bit (256 steps)", .value = 4 },
    {.name = "5: Raw",               .value = 5 },
    {.name = "6: 7-bit (OPLL/128 steps)", .value = 6 }
};

static std::vector<SelectItem> rateItems = {
    {.name = " 1: 96kHz",    .value = 1 },
    {.name = " 2: 55.5kHz",  .value = 2 },
    {.name = " 3: 49.7kHz",  .value = 3 },
    {.name = " 4: 48kHz",    .value = 4 },
    {.name = " 5: 44.1kHz",  .value = 5 },
    {.name = " 6: 22.05kHz", .value = 6 },
    {.name = " 7: 16kHz",    .value = 7 },
    {.name = " 8: 12kHz",    .value = 8 },
    {.name = " 9: 11kHz",    .value = 9 },
    {.name = "10: 8kHz",     .value = 10 },
    {.name = "11: 5.5kHz",   .value = 11 },
    {.name = "12: 4kHz",     .value = 12 },
    {.name = "13: 2kHz",     .value = 13 },
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

static std::vector<SelectItem> multems = {
    {.name = " 0:   0.5x", .value = 1 },
    {.name = " 1:   1x", .value = 2 },
    {.name = " 2:   2x", .value = 3 },
    {.name = " 3:   3x", .value = 4 },
    {.name = " 4:   4x", .value = 5 },
    {.name = " 5:   5x", .value = 6 },
    {.name = " 6:   6x", .value = 7 },
    {.name = " 7:   7x", .value = 8 },
    {.name = " 8:   8x", .value = 9 },
    {.name = " 9:   9x", .value = 10 },
    {.name = "10:  10x", .value = 11 },
    {.name = "11:  11x", .value = 12 },
    {.name = "12:  12x", .value = 13 },
    {.name = "13:  13x", .value = 14 },
    {.name = "14:  14x", .value = 15 },
    {.name = "15:  15x", .value = 16 }
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
    {.name = "0: Saw Up",              .value = 1 },
    {.name = "1: Square",              .value = 2 },
    {.name = "2: Triangle",            .value = 3 },
    {.name = "3: Sample & Hold",       .value = 4 },
    {.name = "4: Saw Down & One Shot", .value = 5 },
    {.name = "5: Triangle & One Shot", .value = 6 },
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
    auto setupPanBtn = [this](GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    const juce::String code = OpnaPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, OpnaGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    qualityCat.setupHwCategory({ .parent = *this, .title = OpnaGuiText::Category::visibleQuality, .invisibleTitle = OpnaGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = *this, .id = code + OpnaPrKey::bit, .title = OpnaGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + OpnaPrKey::rate, .title = OpnaGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setupHwCategory({ .parent = *this, .title = OpnaGuiText::Category::algFb });

    algSelector.setup({ .parent = *this, .id = code + OpnaPrKey::alg, .title = OpnaGuiText::Fm::alg, .items = opnaAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
    };

    feedbackSlider.setup({ .parent = *this, .id = code + OpnaPrKey::fb, .title = OpnaGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    panCat.setupHwCategory({ .parent = *this, .title = OpnaGuiText::Category::visiblePan, .invisibleTitle = OpnaGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = *this, .id = code + OpnaPrKey::pan, .title = OpnaGuiText::Fm::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    setupPanBtn(panToLBtn, OpnaGuiText::Fm::Pan::l, tabOrder);
    panToLBtn.onClick = [this] {
        panSlider.setValue(-1, juce::sendNotification);
        };

    setupPanBtn(panToCBtn, OpnaGuiText::Fm::Pan::c, tabOrder);
    panToCBtn.onClick = [this] {
        panSlider.setValue(0, juce::sendNotification);
        };

    setupPanBtn(panToRBtn, OpnaGuiText::Fm::Pan::r, tabOrder);
    panToRBtn.onClick = [this] {
        panSlider.setValue(1, juce::sendNotification);
        };

    lfoCat.setupSwCategory({ .parent = *this, .title = OpnaGuiText::Category::visibleN88Lfo, .invisibleTitle = OpnaGuiText::Category::invisibleN88Lfo, .enableChangeDetailVisible = true });

    lfoFreqSlider.setup({ .parent = *this, .id = code + OpnaPrKey::N88Lfo::freq, .title = OpnaGuiText::Fm::lfoSpeed, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoShapeSelector.setup({ .parent = *this, .id = code + OpnaPrKey::N88Lfo::shape, .title = OpnaGuiText::Fm::lfoShape, .items = lfoShapeItems, .isReset = true });
    lfoShapeSelector.setWantsKeyboardFocus(true);
    lfoShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + OpnaPrKey::N88Lfo::amSmoothRatio, .title = OpnaGuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = *this, .id = code + OpnaPrKey::N88Lfo::syncDelay, .title = OpnaGuiText::Fm::lfoSyncDelay, .isReset = true });
    lfoSyncDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoSyncDelaySlider.setWantsKeyboardFocus(true);
    lfoSyncDelaySlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelayToZeroBtn.setup({ .parent = *this, .title = "Async", .isReset = false, .isResized = false });
    lfoSyncDelayToZeroBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToZeroBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToZeroBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(0.0f);
        };

    lfoSyncDelayToOneBtn.setup({ .parent = *this, .title = "Sync", .isReset = false, .isResized = false });
    lfoSyncDelayToOneBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToOneBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToOneBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(1.0f);
        };

    lfoPmToggle.setup({ .parent = *this, .id = code + OpnaPrKey::N88Lfo::pm, .title = OpnaGuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + OpnaPrKey::N88Lfo::pmd, .title = OpnaGuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = *this, .id = code + OpnaPrKey::N88Lfo::pms, .title = OpnaGuiText::Fm::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + OpnaPrKey::N88Lfo::am, .title = OpnaGuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = *this, .id = code + OpnaPrKey::N88Lfo::amd, .title = OpnaGuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setupOtherCategory({ .parent = *this, .title = OpnaGuiText::Category::visibleMonoMode, .invisibleTitle = OpnaGuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });

    monoModeToggle.setup({ .parent = *this, .id = OpnaPrKey::monoMode, .title = OpnaGuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setupOtherCategory({ .parent = *this, .title = OpnaGuiText::Category::visibleMvol, .invisibleTitle = OpnaGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

    masterVolSlider.setup({ .parent = *this, .id = OpnaPrKey::masterVol, .title = OpnaGuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

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

    const juce::String opCode = code + OpnaPrKey::op;

    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, OpnaGuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mul[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::mul, .title = OpnaGuiText::Fm::Op::Mul, .items = multems, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);

        dt[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::dt, .title = OpnaGuiText::Fm::Op::Dt, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        dt[i].setWantsKeyboardFocus(true);
        dt[i].setExplicitFocusOrder(++tabOrder);

        rgAr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::rgAr, .title = OpnaGuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgDr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::rgDr, .title = OpnaGuiText::Fm::Op::Dr, .isReset = true });
        rgDr[i].setWantsKeyboardFocus(true);
        rgDr[i].setExplicitFocusOrder(++tabOrder);

        rgSl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::rgSl, .title = OpnaGuiText::Fm::Op::Sl, .isReset = true });
        rgSl[i].setWantsKeyboardFocus(true);
        rgSl[i].setExplicitFocusOrder(++tabOrder);

        rgSr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::rgSr, .title = OpnaGuiText::Fm::Op::Sr, .isReset = true });
        rgSr[i].setWantsKeyboardFocus(true);
        rgSr[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::rgRr, .title = OpnaGuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::rgTl, .title = OpnaGuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::ks, .title = OpnaGuiText::Fm::Op::Ks, .items = ksItems, .isReset = true, .regType = RegisterType::FmKs });
        ks[i].setWantsKeyboardFocus(true);
        ks[i].setExplicitFocusOrder(++tabOrder);

        catPitchEnv[i].setupSwCategory({ .parent = *this, .title = OpnaGuiText::Category::visiblePitchAdsr, .invisibleTitle = OpnaGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

        pitchEnvEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::PitchAdsr::enable, .title = OpnaGuiText::PitchAdsr::enable, .isReset = true });
        pitchEnvEnable[i].setWantsKeyboardFocus(true);
        pitchEnvEnable[i].setExplicitFocusOrder(++tabOrder);

        pitchAttack[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::PitchAdsr::ar, .title = OpnaGuiText::PitchAdsr::ar, .isReset = true });
        pitchAttack[i].setWantsKeyboardFocus(true);
        pitchAttack[i].setExplicitFocusOrder(++tabOrder);

        pitchDecay[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::PitchAdsr::dr , .title = OpnaGuiText::PitchAdsr::dr, .isReset = true });
        pitchDecay[i].setWantsKeyboardFocus(true);
        pitchDecay[i].setExplicitFocusOrder(++tabOrder);

        pitchRelease[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::PitchAdsr::rr, .title = OpnaGuiText::PitchAdsr::rr, .isReset = true });
        pitchRelease[i].setWantsKeyboardFocus(true);
        pitchRelease[i].setExplicitFocusOrder(++tabOrder);

        pitchStartLevel[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::PitchAdsr::stl, .title = OpnaGuiText::PitchAdsr::stl, .isReset = true });
        pitchStartLevel[i].setWantsKeyboardFocus(true);
        pitchStartLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchAttackLevel[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::PitchAdsr::atl, .title = OpnaGuiText::PitchAdsr::atl, .isReset = true });
        pitchAttackLevel[i].setWantsKeyboardFocus(true);
        pitchAttackLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchSustainLevel[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::PitchAdsr::ssl, .title = OpnaGuiText::PitchAdsr::ssl, .isReset = true });
        pitchSustainLevel[i].setWantsKeyboardFocus(true);
        pitchSustainLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchReleaseLevel[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::PitchAdsr::rll, .title = OpnaGuiText::PitchAdsr::rll, .isReset = true });
        pitchReleaseLevel[i].setWantsKeyboardFocus(true);
        pitchReleaseLevel[i].setExplicitFocusOrder(++tabOrder);

        catSsgSwEnv[i].setupSwCategory({ .parent = *this, .title = OpnaGuiText::Category::visibleSwEnv, .invisibleTitle = OpnaGuiText::Category::invisibleSwEnv, .enableChangeDetailVisible = true });

        ssgSwEnvEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::enable, .title = OpnaGuiText::SsgSwEnv::enable, .isReset = true });
        ssgSwEnvEnable[i].setWantsKeyboardFocus(true);
        ssgSwEnvEnable[i].setExplicitFocusOrder(++tabOrder);

        ssgSwSteps[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::steps, .title = OpnaGuiText::SsgSwEnv::steps, .isReset = true });
        ssgSwSteps[i].setWantsKeyboardFocus(true);
        ssgSwSteps[i].setExplicitFocusOrder(++tabOrder);

        ssgSwEnvLoop[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::loop, .title = OpnaGuiText::SsgSwEnv::loop, .isReset = true });
        ssgSwEnvLoop[i].setWantsKeyboardFocus(true);
        ssgSwEnvLoop[i].setExplicitFocusOrder(++tabOrder);

        ssgSwLoopTo[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::loopTo, .title = OpnaGuiText::SsgSwEnv::loopTo, .isReset = true });
        ssgSwLoopTo[i].setWantsKeyboardFocus(true);
        ssgSwLoopTo[i].setExplicitFocusOrder(++tabOrder);

        ssgSwLoopCount[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::loopCount, .title = OpnaGuiText::SsgSwEnv::loopCount, .isReset = true });
        ssgSwLoopCount[i].setWantsKeyboardFocus(true);
        ssgSwLoopCount[i].setExplicitFocusOrder(++tabOrder);

        ssgSwStartLevel[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::stl, .title = OpnaGuiText::SsgSwEnv::stl, .isReset = true });
        ssgSwStartLevel[i].setWantsKeyboardFocus(true);
        ssgSwStartLevel[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR1[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::r1, .title = OpnaGuiText::SsgSwEnv::r1, .isReset = true });
        ssgSwR1[i].setWantsKeyboardFocus(true);
        ssgSwR1[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL1[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::l1, .title = OpnaGuiText::SsgSwEnv::l1, .isReset = true });
        ssgSwL1[i].setWantsKeyboardFocus(true);
        ssgSwL1[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR2[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::r2, .title = OpnaGuiText::SsgSwEnv::r2, .isReset = true });
        ssgSwR2[i].setWantsKeyboardFocus(true);
        ssgSwR2[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL2[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::l2, .title = OpnaGuiText::SsgSwEnv::l2, .isReset = true });
        ssgSwL2[i].setWantsKeyboardFocus(true);
        ssgSwL2[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR3[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::r3, .title = OpnaGuiText::SsgSwEnv::r3, .isReset = true });
        ssgSwR3[i].setWantsKeyboardFocus(true);
        ssgSwR3[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL3[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::l3, .title = OpnaGuiText::SsgSwEnv::l3, .isReset = true });
        ssgSwL3[i].setWantsKeyboardFocus(true);
        ssgSwL3[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR4[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::r4, .title = OpnaGuiText::SsgSwEnv::r4, .isReset = true });
        ssgSwR4[i].setWantsKeyboardFocus(true);
        ssgSwR4[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL4[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::l4, .title = OpnaGuiText::SsgSwEnv::l4, .isReset = true });
        ssgSwL4[i].setWantsKeyboardFocus(true);
        ssgSwL4[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR5[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::r5, .title = OpnaGuiText::SsgSwEnv::r5, .isReset = true });
        ssgSwR5[i].setWantsKeyboardFocus(true);
        ssgSwR5[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL5[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::l5, .title = OpnaGuiText::SsgSwEnv::l5, .isReset = true });
        ssgSwL5[i].setWantsKeyboardFocus(true);
        ssgSwL5[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR6[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::r6, .title = OpnaGuiText::SsgSwEnv::r6, .isReset = true });
        ssgSwR6[i].setWantsKeyboardFocus(true);
        ssgSwR6[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL6[i].setup({ .parent = *this, .id = paramPrefix + OpnaPrKey::SsgSwEnv::l6, .title = OpnaGuiText::SsgSwEnv::l6, .isReset = true });
        ssgSwL6[i].setWantsKeyboardFocus(true);
        ssgSwL6[i].setExplicitFocusOrder(++tabOrder);

        catSsgEnv[i].setupHwCategory({ .parent = *this, .title = OpnaGuiText::Category::visibleSsgEnv, .invisibleTitle = OpnaGuiText::Category::invisibleSsgEnv, .enableChangeDetailVisible = true });

        se[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::se, .title = OpnaGuiText::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
        se[i].setWantsKeyboardFocus(true);
        se[i].setExplicitFocusOrder(++tabOrder);

        seFreq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::seFreq, .title = OpnaGuiText::Fm::Op::SFreq, .isReset = true });
        seFreq[i].setWantsKeyboardFocus(true);
        seFreq[i].setExplicitFocusOrder(++tabOrder);

        catLfo[i].setupHwCategory({ .parent = *this, .title = OpnaGuiText::Category::visibleHwLfo, .invisibleTitle = OpnaGuiText::Category::invisibleHwLfo, .enableChangeDetailVisible = true });

        freqs[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::freqs, .title = OpnaGuiText::Fm::Op::Freqs, .items = lfoFreqsItems, .isReset = true });
        freqs[i].setWantsKeyboardFocus(true);
        freqs[i].setExplicitFocusOrder(++tabOrder);

        syncDelay[i].setup({.parent = *this, .id = paramPrefix + OpnaPrKey::syncDelay, .title = OpnaGuiText::Fm::Op::SyncDelay, .isReset = true});
        syncDelay[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        syncDelay[i].setWantsKeyboardFocus(true);
        syncDelay[i].setExplicitFocusOrder(++tabOrder);

        syncDelayToZero[i].setup({ .parent = *this, .title = "Async", .isReset = false, .isResized = false });
        syncDelayToZero[i].setWantsKeyboardFocus(true);
        syncDelayToZero[i].setExplicitFocusOrder(++tabOrder);
        syncDelayToZero[i].onClick = [this, i] {
            syncDelay[i].setValue(0.0f);
            };

        syncDelayToOne[i].setup({ .parent = *this, .title = "Sync", .isReset = false, .isResized = false });
        syncDelayToOne[i].setWantsKeyboardFocus(true);
        syncDelayToOne[i].setExplicitFocusOrder(++tabOrder);
        syncDelayToOne[i].onClick = [this, i] {
            syncDelay[i].setValue(1.0f);
            };

        pm[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::vib, .title = OpnaGuiText::Fm::Op::PmEn, .isReset = true });
        pm[i].setWantsKeyboardFocus(true);
        pm[i].setExplicitFocusOrder(++tabOrder);

        pms[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::pms, .title = OpnaGuiText::Fm::Op::Pms, .items = hwPmsItems, .isReset = true });
        pms[i].setWantsKeyboardFocus(true);
        pms[i].setExplicitFocusOrder(++tabOrder);

        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::am, .title = OpnaGuiText::Fm::Op::AmEn, .isReset = true });
        am[i].setWantsKeyboardFocus(true);
        am[i].setExplicitFocusOrder(++tabOrder);

        ams[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::ams, .title = OpnaGuiText::Fm::Op::Ams, .items = hwAmsItems, .isReset = true });
        ams[i].setWantsKeyboardFocus(true);
        ams[i].setExplicitFocusOrder(++tabOrder);

        catN88Lfo[i].setupSwCategory({ .parent = *this, .title = OpnaGuiText::Category::visibleN88Lfo, .invisibleTitle = OpnaGuiText::Category::invisibleN88Lfo, .enableChangeDetailVisible = true });

        n88Ams[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::n88Ams, .title = OpnaGuiText::Fm::Op::Ams, .isReset = true });
        n88Ams[i].setWantsKeyboardFocus(true);
        n88Ams[i].setExplicitFocusOrder(++tabOrder);

        cafFix[i].setupHwCategory({ .parent = *this, .title = OpnaGuiText::Category::visibleFix, .invisibleTitle = OpnaGuiText::Category::invisibleFix, .enableChangeDetailVisible = true });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::fix, .title = OpnaGuiText::Fm::Op::Fix, .isReset = true });
        fix[i].setWantsKeyboardFocus(true);
        fix[i].setExplicitFocusOrder(++tabOrder);

        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::fixFreq, .title = OpnaGuiText::Fm::Op::FFreq, .isReset = true });
        freq[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        freq[i].setWantsKeyboardFocus(true);
        freq[i].setExplicitFocusOrder(++tabOrder);

        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = OpnaGuiText::Fm::Op::FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].setWantsKeyboardFocus(true);
        freqToZero[i].setExplicitFocusOrder(++tabOrder);
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };

        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = OpnaGuiText::Fm::Op::FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].setWantsKeyboardFocus(true);
        freqTo440[i].setExplicitFocusOrder(++tabOrder);
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        catMask[i].setupHwCategory({ .parent = *this, .title = OpnaGuiText::Category::visibleMask, .invisibleTitle = OpnaGuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnaPrKey::mask, .title = OpnaGuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        catMml[i].setupOtherCategory({ .parent = *this, .title = OpnaGuiText::Category::mml });

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

    auto mainArea = pageArea.removeFromLeft(OpnaGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(OpnaGuiValue::Group::Padding::width, OpnaGuiValue::Group::Padding::height);

    mRect.removeFromTop(OpnaGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    layoutPanCat(mRect);

    layoutN88LfoCat(mRect);

    layoutQualityCat(mRect);

    layoutMonoModeCat(mRect);

    layoutMvolCat(mRect);

    auto imgArea = mRect.removeFromBottom(100);
    algImageComp.setBounds(imgArea);
    mRect.removeFromTop(OpnaGuiValue::Category::paddingTop);

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(OpnaGuiValue::Fm::Op::width);

        opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(OpnaGuiValue::Fm::Op::Padding::width, OpnaGuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(OpnaGuiValue::Group::TitlePaddingTop);

        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt[i].label, .component = &dt[i] });
        updateRgDisplayAsOp(i, true);
        layoutRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgDr[i].label, .component = &rgDr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgSr[i].label, .component = &rgSr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgSl[i].label, .component = &rgSl[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i] });
        layoutRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i] });

        layoutOpSsgEnvelopeCat(i, innerRect);

        layoutOpPitchEnvCat(i, innerRect);

        layoutOpSsgSwEnvCat(i, innerRect);

        layoutOpHwLfoCat(i, innerRect);

        layoutOpN88LfoCat(i, innerRect);

        layoutOpFixCat(i, innerRect);

        layoutOpMaskCat(i, innerRect);

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
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMl,   [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixDt,   [&](int v) { dt[opIndex].setSelectedItemIndex(RegisterConverter::convertMmlDtToReg(v), juce::sendNotification); } },
        { mmlPrefixKs,   [&](int v) { ks[opIndex].setSelectedItemIndex(RegisterConverter::convertFmKs(v), juce::sendNotification); } },
        { mmlPrefixMask, [&](int v) { mask[opIndex].setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },

        // --- TL系 (RGモードで分岐) ---
        { mmlPrefixTl,   [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixO,    [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixOl,   [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},

        // --- エンベロープ系 (RGモードで分岐) ---
        { mmlPrefixAr,   [&](int v) { rgAr[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixDr,   [&](int v) { rgDr[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixSr,   [&](int v) { rgSr[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixSl,   [&](int v) { rgSl[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
        { mmlPrefixRr,   [&](int v) { rgRr[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }}
    };

    for (const auto& rUnit : units)
    {
        // actionMapの中に rUnit.key と一致するものがあるか検索
        auto it = actionMap.find(rUnit.key);

        if (it != actionMap.end())
        {
            // 見つかったら、そこに登録されたラムダ式(関数)に value を渡して実行
            it->second(rUnit.value);
        }
    }
}

void GuiOpna::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    mul[idx].setEnabledWithLabel(enable);
    dt[idx].setEnabledWithLabel(enable);
    ks[idx].setEnabledWithLabel(enable);
    catSsgEnv[idx].setEnabled(enable);
    se[idx].setEnabledWithLabel(enable);
    seFreq[idx].setEnabledWithLabel(enable);
    cafFix[idx].setEnabled(enable);
    fix[idx].setEnabled(enable);
    freq[idx].setEnabledWithLabel(enable);
    freqToZero[idx].setEnabled(enable);
    freqTo440[idx].setEnabled(enable);
    catLfo[idx].setEnabled(enable);
    pm[idx].setEnabled(enable);
    pms[idx].setEnabledWithLabel(enable);
    am[idx].setEnabled(enable);
    ams[idx].setEnabledWithLabel(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    catMml[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
}

void GuiOpna::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].setVisibleWithLabel(rgMode);
    rgDr[idx].setVisibleWithLabel(rgMode);
    rgSl[idx].setVisibleWithLabel(rgMode);
    rgSr[idx].setVisibleWithLabel(rgMode);
    rgRr[idx].setVisibleWithLabel(rgMode);
    rgTl[idx].setVisibleWithLabel(rgMode);
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
        juce::String newTitle = OpnaGuiText::Group::opPrefix + juce::String(i + 1) + algOpPrefix[algIndex][i];
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

// ==============================================================================
// Keyboard Shortcut Logic
// ==============================================================================
bool GuiOpna::keyPressed(const juce::KeyPress& key)
{
    int opIndex = -1;
    int code = key.getKeyCode();
    juce::ModifierKeys metaKeys = key.getModifiers();

    // 通常の 1〜4キー、または テンキーの 1〜4 を判定
    if (code == '1' || code == juce::KeyPress::numberPad1) opIndex = 0;
    else if (code == '2' || code == juce::KeyPress::numberPad2) opIndex = 1;
    else if (code == '3' || code == juce::KeyPress::numberPad3) opIndex = 2;
    else if (code == '4' || code == juce::KeyPress::numberPad4) opIndex = 3;

    // 対応するキーが押されていたら、該当する処理を実行
    if (opIndex != -1)
    {
        // Altキーを押しながら -> マスクのON/OFF、それ以外 -> MMLボタンクリック
        // 該当オペレータが有効(Enabled)な時のみ反応させる
        if (metaKeys.isAltDown() && mask[opIndex].isEnabled()) {
            mask[opIndex].setToggleState(!mask[opIndex].getToggleState(), juce::sendNotification);
        }
        else if (mml[opIndex].isEnabled()) {
            mml[opIndex].triggerClick();
        }

        return true; // キー入力を消費したことをJUCEに伝える
    }

    return false; // 他のキーなら無視（通常処理へ）
}

void GuiOpna::copyFmParamsToString()
{
    int mask1 = FmMml::bool2Int(!mask[0].getToggleState());
    int mask2 = FmMml::bool2Int(!mask[1].getToggleState());
    int mask3 = FmMml::bool2Int(!mask[2].getToggleState());
    int mask4 = FmMml::bool2Int(!mask[3].getToggleState());
    int maskVal = FmMml::genMask4(mask1, mask2, mask2, mask3);

    auto formatCoreBasic = [this, maskVal]() {
        return juce::String::formatted(
            // '   ALG    FB   MSK    SHP  SPD  SYC  PMD  PMS  AMD 
            u8"    %1d,  %1d,  %2d,   %2d, %5d, %3d,  %d, %+d,  %d\n",
            algSelector.getSelectedId() - 1,      // ALG
            (int)feedbackSlider.getValue(),       // FB
            maskVal,                              // MASK
            lfoShapeSelector.getSelectedId() - 1, // SHAPE
            (int)lfoFreqSlider.getValue(),        // SPEED
            (int)lfoSyncDelaySlider.getValue(),   // SYNC
            (int)lfoPmdSlider.getValue(),         // PMD
            (int)lfoPmsSlider.getValue(),         // PMS
            (int)lfoAmdSlider.getValue()          // AMD
        );
        };
    auto formatOpBasic = [this](int index) {
        return juce::String::formatted(
            // ' MUL    DT   AR   DR   SL   RR   SR   TL   KS  AMS
            u8"  %2d, %+1d, %2d, %2d, %2d, %2d, %2d, %3d, %1d, %+d\n",
            (int)this->mul[index].getSelectedId() - 1,          // MUL
            FmMml::int2dt(this->dt[index].getSelectedId() - 1), // DT
            (int)this->rgAr[index].getValue(),                  // AR
            (int)this->rgDr[index].getValue(),                  // DR
            (int)this->rgSl[index].getValue(),                  // SL
            (int)this->rgSr[index].getValue(),                  // SR
            (int)this->rgRr[index].getValue(),                  // RR
            (int)this->rgTl[index].getValue(),                  // TL
            this->ks[index].getSelectedId() - 1,                // KS
            (int)this->n88Ams[index].getValue()                 // AMS
        );
        };
    auto formatOpsBasic = [this, formatOpBasic]() {
        return formatOpBasic(0) + formatOpBasic(1) + formatOpBasic(2) + formatOpBasic(3);
        };
    auto formatOpExt = [this](int index) {
        return juce::String::formatted(
            // ' MUL    DT   AR   DR   SL   SR   RR   TL   KS
            u8"MUL%d DT%+d AR%d DR%d SL%d SR%d RR%d TL%d KS%d\n",
            (int)this->mul[index].getSelectedId() - 1,
            FmMml::int2dt(this->dt[index].getSelectedId() - 1),
            (int)this->rgAr[index].getValue(),
            (int)this->rgDr[index].getValue(),
            (int)this->rgSl[index].getValue(),
            (int)this->rgSr[index].getValue(),
            (int)this->rgRr[index].getValue(),
            (int)this->rgTl[index].getValue(),
            this->ks[index].getSelectedId() - 1
        );
        };
    auto formatOpsExt = [this, formatOpExt]() {
        return formatOpExt(0) + formatOpExt(1) + formatOpExt(2) + formatOpExt(3);
        };

    juce::String mml = juce::String("[OPNA]\n")
        + FmMml::basicMmlHeader
        + juce::String(u8"' ALG FB0 FB2 MASK SHAPE  SPEED SYNC PMD PMS AMD\n")
        + formatCoreBasic()
        + juce::String(u8"' MUL DT  AR  DR  SL  SR  RR   TL KS AMS\n")
        + formatOpsBasic()
        + juce::String(u8"\n")
        + FmMml::extMmlHeader
        + formatOpsExt();
    juce::SystemClipboard::copyTextToClipboard(mml);
}

void GuiOpna::copyFmParamsToObject()
{

}

void GuiOpna::pasteFmParamsFromObject()
{

}

void GuiOpna::initParams()
{
    this->ctx.audioProcessor.initParams("OPNA_");
}

void GuiOpna::layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask[opIndex] });

    bool visibleMask = catMask[opIndex].isDetailVisible();

    mask[opIndex].setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask[opIndex] });
    }
}

void GuiOpna::layoutOpFixCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &cafFix[opIndex] });

    bool visibleFix = cafFix[opIndex].isDetailVisible();

    fix[opIndex].setVisible(visibleFix);
    freq[opIndex].setVisible(visibleFix);
    freq[opIndex].label.setVisible(visibleFix);
    freqToZero[opIndex].setVisible(visibleFix);
    freqTo440[opIndex].setVisible(visibleFix);

    if (visibleFix)
    {
        layoutRow({ .rowRect = rect, .component = &fix[opIndex] });
        layoutRow({ .rowRect = rect, .label = &freq[opIndex].label, .component = &freq[opIndex] });
        layoutRowTwoComps({ .rect = rect, .comp1 = &freqToZero[opIndex], .comp2 = &freqTo440[opIndex] });
    }
}

void GuiOpna::layoutQualityCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visibleQuality = qualityCat.isDetailVisible();

    bitSelector.setVisibleWithLabel(visibleQuality);
    rateSelector.setVisibleWithLabel(visibleQuality);

    if (visibleQuality)
    {
        layoutMain({ .mainRect = rect, .label = &bitSelector.label, .component = &bitSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
    }
}

void GuiOpna::layoutMonoModeCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &monoPolyCat });

    bool visible = monoPolyCat.isDetailVisible();

    monoModeToggle.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoModeToggle });
    }
}

void GuiOpna::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
    }
}

void GuiOpna::layoutPanCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &panCat });

    bool visible = panCat.isDetailVisible();

    panSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &panSlider.label, .component = &panSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn });
    }
}

void GuiOpna::layoutN88LfoCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &lfoCat });

    bool visible = lfoCat.isDetailVisible();

    lfoFreqSlider.setVisibleWithLabel(visible);
    lfoShapeSelector.setVisibleWithLabel(visible);
    lfoAmSmRtSlider.setVisibleWithLabel(visible);
    lfoSyncDelaySlider.setVisibleWithLabel(visible);
    lfoSyncDelayToZeroBtn.setVisible(visible);
    lfoSyncDelayToOneBtn.setVisible(visible);
    lfoPmToggle.setVisible(visible);
    lfoPmsSlider.setVisibleWithLabel(visible);
    lfoPmdSlider.setVisibleWithLabel(visible);
    lfoAmToggle.setVisible(visible);
    lfoAmdSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoShapeSelector.label, .component = &lfoShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
        layoutMain({ .mainRect = rect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
        layoutMainTwoComps({ .rect = rect, .comp1 = &lfoSyncDelayToZeroBtn, .comp2 = &lfoSyncDelayToOneBtn });
        layoutMain({ .mainRect = rect, .component = &lfoPmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        layoutMain({ .mainRect = rect, .label = &lfoPmsSlider.label, .component = &lfoPmsSlider });
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });
    }
}

void GuiOpna::layoutOpSsgEnvelopeCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRow({ .rowRect = rect, .component = &catSsgEnv[opIndex] });

    bool visible = catSsgEnv[opIndex].isDetailVisible();

    se[opIndex].setVisibleWithLabel(visible);
    seFreq[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &se[opIndex].label, .component = &se[opIndex] });
        layoutRow({ .rowRect = rect, .label = &seFreq[opIndex].label, .component = &seFreq[opIndex] });
    }
}

void GuiOpna::layoutOpHwLfoCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catLfo[opIndex] });

    bool visible = catLfo[opIndex].isDetailVisible();

    freqs[opIndex].setVisibleWithLabel(visible);
    syncDelay[opIndex].setVisibleWithLabel(visible);
    syncDelayToZero[opIndex].setVisible(visible);
    syncDelayToOne[opIndex].setVisible(visible);
    pm[opIndex].setVisible(visible);
    pms[opIndex].setVisibleWithLabel(visible);
    am[opIndex].setVisible(visible);
    ams[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &freqs[opIndex].label, .component = &freqs[opIndex] });
        layoutRow({ .rowRect = rect, .label = &syncDelay[opIndex].label, .component = &syncDelay[opIndex] });
        layoutRowTwoComps({ .rect = rect, .comp1 = &syncDelayToZero[opIndex], .comp2 = &syncDelayToOne[opIndex] });
        layoutRow({ .rowRect = rect, .component = &pm[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pms[opIndex].label, .component = &pms[opIndex] });
        layoutRow({ .rowRect = rect, .component = &am[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ams[opIndex].label, .component = &ams[opIndex] });
    }
}

void GuiOpna::layoutOpN88LfoCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catN88Lfo[opIndex] });

    bool visible = catN88Lfo[opIndex].isDetailVisible();

    n88Ams[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &n88Ams[opIndex].label, .component = &n88Ams[opIndex] });
    }
}

void GuiOpna::layoutOpPitchEnvCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catPitchEnv[opIndex] });

    bool visible = catPitchEnv[opIndex].isDetailVisible();

    pitchEnvEnable[opIndex].setVisible(visible);
    pitchAttack[opIndex].setVisibleWithLabel(visible);
    pitchDecay[opIndex].setVisibleWithLabel(visible);
    pitchRelease[opIndex].setVisibleWithLabel(visible);
    pitchStartLevel[opIndex].setVisibleWithLabel(visible);
    pitchAttackLevel[opIndex].setVisibleWithLabel(visible);
    pitchSustainLevel[opIndex].setVisibleWithLabel(visible);
    pitchReleaseLevel[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &pitchEnvEnable[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchAttack[opIndex].label, .component = &pitchAttack[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchDecay[opIndex].label, .component = &pitchDecay[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchRelease[opIndex].label, .component = &pitchRelease[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchStartLevel[opIndex].label, .component = &pitchStartLevel[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchAttackLevel[opIndex].label, .component = &pitchAttackLevel[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchSustainLevel[opIndex].label, .component = &pitchSustainLevel[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchReleaseLevel[opIndex].label, .component = &pitchReleaseLevel[opIndex] });
    }
}

void GuiOpna::layoutOpSsgSwEnvCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catSsgSwEnv[opIndex] });

    bool visible = catSsgSwEnv[opIndex].isDetailVisible();

    ssgSwEnvEnable[opIndex].setVisible(visible);
    ssgSwSteps[opIndex].setVisibleWithLabel(visible);
    ssgSwEnvLoop[opIndex].setVisible(visible);
    ssgSwLoopTo[opIndex].setVisibleWithLabel(visible);
    ssgSwLoopCount[opIndex].setVisibleWithLabel(visible);
    ssgSwStartLevel[opIndex].setVisibleWithLabel(visible);
    ssgSwR1[opIndex].setVisibleWithLabel(visible);
    ssgSwL1[opIndex].setVisibleWithLabel(visible);
    ssgSwR2[opIndex].setVisibleWithLabel(visible);
    ssgSwL2[opIndex].setVisibleWithLabel(visible);
    ssgSwR3[opIndex].setVisibleWithLabel(visible);
    ssgSwL3[opIndex].setVisibleWithLabel(visible);
    ssgSwR4[opIndex].setVisibleWithLabel(visible);
    ssgSwL4[opIndex].setVisibleWithLabel(visible);
    ssgSwR5[opIndex].setVisibleWithLabel(visible);
    ssgSwL5[opIndex].setVisibleWithLabel(visible);
    ssgSwR6[opIndex].setVisibleWithLabel(visible);
    ssgSwL6[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &ssgSwEnvEnable[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwSteps[opIndex].label, .component = &ssgSwSteps[opIndex] });
        layoutRow({ .rowRect = rect, .component = &ssgSwEnvLoop[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwLoopTo[opIndex].label, .component = &ssgSwLoopTo[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwLoopCount[opIndex].label, .component = &ssgSwLoopCount[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwStartLevel[opIndex].label, .component = &ssgSwStartLevel[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR1[opIndex].label, .component = &ssgSwR1[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL1[opIndex].label, .component = &ssgSwL1[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR2[opIndex].label, .component = &ssgSwR2[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL2[opIndex].label, .component = &ssgSwL2[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR3[opIndex].label, .component = &ssgSwR3[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL3[opIndex].label, .component = &ssgSwL3[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR4[opIndex].label, .component = &ssgSwR4[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL4[opIndex].label, .component = &ssgSwL4[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR5[opIndex].label, .component = &ssgSwR5[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL5[opIndex].label, .component = &ssgSwL5[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR6[opIndex].label, .component = &ssgSwR6[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL6[opIndex].label, .component = &ssgSwL6[opIndex] });
    }
}
