#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"


#include "../../Processor/Opl/Keys.h"
#include "../../Processor/Opl/Values.h"
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

static std::vector<SelectItem> oplAlgItems = {
    {.name = "00: <OPL-00>", .value = 1 },
    {.name = "01: <OPL-01>", .value = 2 },
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

static std::vector<SelectItem> kslItems = {
    {.name = "KSL: 0", .value = 1},
    {.name = "KSL: 1", .value = 2},
    {.name = "KSL: 2", .value = 3},
    {.name = "KSL: 3", .value = 4}
};

static std::vector<SelectItem> oplEgItems = {
    {.name = "0: Sine",       .value = 1},
    {.name = "1: Half Sine",  .value = 2},
    {.name = "2: Abs Sine",   .value = 3},
    {.name = "3: Pulse Sine", .value = 4}
};

void GuiOpl::setup()
{
    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    const juce::String code = OplPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, OplGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    qualityCat.setupHwCategory({ .parent = *this, .title = OplGuiText::Category::visibleQuality, .invisibleTitle = OplGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });
    bitSelector.setup({ .parent = *this, .id = code + OplPrKey::bit, .title = OplGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);
    rateSelector.setup({ .parent = *this, .id = code + OplPrKey::rate, .title = OplGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setupHwCategory({ .parent = *this, .title = OplGuiText::Category::algFb });
    algSelector.setup({ .parent = *this, .id = code + OplPrKey::alg, .title = OplGuiText::Fm::alg, .items = oplAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };
    feedbackSlider.setup({ .parent = *this, .id = code + OplPrKey::fb, .title = OplGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    initCat.setupSwCategory({ .parent = *this, .title = OplGuiText::Category::visibleInitialize, .invisibleTitle = OplGuiText::Category::invisibleInitialize, .enableChangeDetailVisible = true });

    initLfoToOplBtn.setup({ .parent = *this, .title = OplGuiText::Fm::initLfoToOpl });
    initLfoToOplBtn.setWantsKeyboardFocus(true);
    initLfoToOplBtn.setExplicitFocusOrder(++tabOrder);
    initLfoToOplBtn.onClick = [this] {
        for (int i = 0; i < 2; i++)
        {
            ams[i].setValue(3.7, juce::sendNotification);
            amd[i].setValue(4.8, juce::sendNotification);

            pms[i].setValue(6.4, juce::sendNotification);
            pmd[i].setValue(14.0, juce::sendNotification);
        }
        };
    initLfoToOpllBtn.setup({ .parent = *this, .title = OplGuiText::Fm::initLfoToOpll });
    initLfoToOpllBtn.setWantsKeyboardFocus(true);
    initLfoToOpllBtn.setExplicitFocusOrder(++tabOrder);
    initLfoToOpllBtn.onClick = [this] {
        for (int i = 0; i < 2; i++)
        {
            ams[i].setValue(6.06, juce::sendNotification);
            amd[i].setValue(1.2, juce::sendNotification);

            pms[i].setValue(6.06, juce::sendNotification);
            pmd[i].setValue(13.7, juce::sendNotification);
        }
        };

    monoPolyCat.setupOtherCategory({ .parent = *this, .title = OplGuiText::Category::visibleMonoMode, .invisibleTitle = OplGuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });
    monoModeToggle.setup({ .parent = *this, .id = OplPrKey::monoMode, .title = OplGuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setupOtherCategory({ .parent = *this, .title = OplGuiText::Category::visibleMvol, .invisibleTitle = OplGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });
    masterVolSlider.setup({ .parent = *this, .id = OplPrKey::masterVol, .title = OplGuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    for (int i = 0; i < 2; ++i)
    {
        juce::String fileName = juce::String::formatted(Io::Folder::asset + "/" + Io::Folder::resource + "/ALG_OPL_%02d.png", i);
        auto imgFile = docDir.getChildFile(fileName);

        if (imgFile.existsAsFile()) {
            algImages[i] = juce::ImageFileFormat::loadFrom(imgFile);
        }
    }

    // 画像コンポーネントを画面に追加
    addAndMakeVisible(algImageComp);

    const juce::String opCode = code + OplPrKey::op;

    for (int i = 0; i < 2; ++i)
    {
        opGroups[i].setup(*this, OplGuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mul[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OplPrKey::mul, .title = OplGuiText::Fm::Op::Mul, .items = multems, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);

        rgAr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OplPrKey::rgAr, .title = OplGuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgDr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OplPrKey::rgDr, .title = OplGuiText::Fm::Op::Dr, .isReset = true });
        rgDr[i].setWantsKeyboardFocus(true);
        rgDr[i].setExplicitFocusOrder(++tabOrder);

        rgSl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OplPrKey::rgSl, .title = OplGuiText::Fm::Op::Sl, .isReset = true });
        rgSl[i].setWantsKeyboardFocus(true);
        rgSl[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OplPrKey::rgRr, .title = OplGuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OplPrKey::rgTl, .title = OplGuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        egType[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OplPrKey::egType, .title = OplGuiText::Fm::Op::EgType, .isReset = true });
        egType[i].setWantsKeyboardFocus(true);
        egType[i].setExplicitFocusOrder(++tabOrder);

        ksr[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OplPrKey::ksr, .title = OplGuiText::Fm::Op::Ksr, .isReset = true });
        ksr[i].setWantsKeyboardFocus(true);
        ksr[i].setExplicitFocusOrder(++tabOrder);

        ksl[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OplPrKey::ksl, .title = OplGuiText::Fm::Op::Ksl, .items = kslItems, .isReset = true });
        ksl[i].setWantsKeyboardFocus(true);
        ksl[i].setExplicitFocusOrder(++tabOrder);

        catPitchEnv[i].setupSwCategory({ .parent = *this, .title = OplGuiText::Category::visiblePitchAdsr, .invisibleTitle = OplGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

        pitchEnvEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OplPrKey::PitchAdsr::enable, .title = OplGuiText::PitchAdsr::enable, .isReset = true });
        pitchEnvEnable[i].setWantsKeyboardFocus(true);
        pitchEnvEnable[i].setExplicitFocusOrder(++tabOrder);

        pitchAttack[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::PitchAdsr::ar, .title = OplGuiText::PitchAdsr::ar, .isReset = true });
        pitchAttack[i].setWantsKeyboardFocus(true);
        pitchAttack[i].setExplicitFocusOrder(++tabOrder);

        pitchDecay[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::PitchAdsr::dr , .title = OplGuiText::PitchAdsr::dr, .isReset = true });
        pitchDecay[i].setWantsKeyboardFocus(true);
        pitchDecay[i].setExplicitFocusOrder(++tabOrder);

        pitchRelease[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::PitchAdsr::rr, .title = OplGuiText::PitchAdsr::rr, .isReset = true });
        pitchRelease[i].setWantsKeyboardFocus(true);
        pitchRelease[i].setExplicitFocusOrder(++tabOrder);

        pitchStartLevel[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::PitchAdsr::stl, .title = OplGuiText::PitchAdsr::stl, .isReset = true });
        pitchStartLevel[i].setWantsKeyboardFocus(true);
        pitchStartLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchAttackLevel[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::PitchAdsr::atl, .title = OplGuiText::PitchAdsr::atl, .isReset = true });
        pitchAttackLevel[i].setWantsKeyboardFocus(true);
        pitchAttackLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchSustainLevel[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::PitchAdsr::ssl, .title = OplGuiText::PitchAdsr::ssl, .isReset = true });
        pitchSustainLevel[i].setWantsKeyboardFocus(true);
        pitchSustainLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchReleaseLevel[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::PitchAdsr::rll, .title = OplGuiText::PitchAdsr::rll, .isReset = true });
        pitchReleaseLevel[i].setWantsKeyboardFocus(true);
        pitchReleaseLevel[i].setExplicitFocusOrder(++tabOrder);

        catSsgSwEnv[i].setupSwCategory({ .parent = *this, .title = OplGuiText::Category::visibleSwEnv, .invisibleTitle = OplGuiText::Category::invisibleSwEnv, .enableChangeDetailVisible = true });

        ssgSwEnvEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::enable, .title = OplGuiText::SsgSwEnv::enable, .isReset = true });
        ssgSwEnvEnable[i].setWantsKeyboardFocus(true);
        ssgSwEnvEnable[i].setExplicitFocusOrder(++tabOrder);

        ssgSwSteps[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::steps, .title = OplGuiText::SsgSwEnv::steps, .isReset = true });
        ssgSwSteps[i].setWantsKeyboardFocus(true);
        ssgSwSteps[i].setExplicitFocusOrder(++tabOrder);
        ssgSwSteps[i].onValueChange = [this, i] {
            bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

            applyOpSsgSwEnvLoopValues(i, ssgEnvLoopEnable);
            };

        ssgSwEnvLoop[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::loop, .title = OplGuiText::SsgSwEnv::loop, .isReset = true });
        ssgSwEnvLoop[i].setWantsKeyboardFocus(true);
        ssgSwEnvLoop[i].setExplicitFocusOrder(++tabOrder);
        ssgSwEnvLoop[i].onClick = [this, i] {
            bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

            ssgSwLoopTo[i].setEnabled(ssgEnvLoopEnable);
            ssgSwLoopTo[i].label.setEnabled(ssgEnvLoopEnable);
            ssgSwLoopCount[i].setEnabled(ssgEnvLoopEnable);
            ssgSwLoopCount[i].label.setEnabled(ssgEnvLoopEnable);

            applyOpSsgSwEnvLoopValues(i, ssgEnvLoopEnable);
            };

        ssgSwLoopTo[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::loopTo, .title = OplGuiText::SsgSwEnv::loopTo, .isReset = true });
        ssgSwLoopTo[i].setWantsKeyboardFocus(true);
        ssgSwLoopTo[i].setExplicitFocusOrder(++tabOrder);
        ssgSwLoopTo[i].onValueChange = [this, i] {
            bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

            applyOpSsgSwEnvLoopValues(i, ssgEnvLoopEnable);
            };

        ssgSwLoopCount[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::loopCount, .title = OplGuiText::SsgSwEnv::loopCount, .isReset = true });
        ssgSwLoopCount[i].setWantsKeyboardFocus(true);
        ssgSwLoopCount[i].setExplicitFocusOrder(++tabOrder);

        bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

        ssgSwLoopTo[i].setEnabled(ssgEnvLoopEnable);
        ssgSwLoopTo[i].label.setEnabled(ssgEnvLoopEnable);
        ssgSwLoopCount[i].setEnabled(ssgEnvLoopEnable);
        ssgSwLoopCount[i].label.setEnabled(ssgEnvLoopEnable);

        ssgSwStartLevel[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::stl, .title = OplGuiText::SsgSwEnv::stl, .isReset = true });
        ssgSwStartLevel[i].setWantsKeyboardFocus(true);
        ssgSwStartLevel[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR1[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::r1, .title = OplGuiText::SsgSwEnv::r1, .isReset = true });
        ssgSwR1[i].setWantsKeyboardFocus(true);
        ssgSwR1[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL1[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::l1, .title = OplGuiText::SsgSwEnv::l1, .isReset = true });
        ssgSwL1[i].setWantsKeyboardFocus(true);
        ssgSwL1[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR2[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::r2, .title = OplGuiText::SsgSwEnv::r2, .isReset = true });
        ssgSwR2[i].setWantsKeyboardFocus(true);
        ssgSwR2[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL2[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::l2, .title = OplGuiText::SsgSwEnv::l2, .isReset = true });
        ssgSwL2[i].setWantsKeyboardFocus(true);
        ssgSwL2[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR3[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::r3, .title = OplGuiText::SsgSwEnv::r3, .isReset = true });
        ssgSwR3[i].setWantsKeyboardFocus(true);
        ssgSwR3[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL3[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::l3, .title = OplGuiText::SsgSwEnv::l3, .isReset = true });
        ssgSwL3[i].setWantsKeyboardFocus(true);
        ssgSwL3[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR4[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::r4, .title = OplGuiText::SsgSwEnv::r4, .isReset = true });
        ssgSwR4[i].setWantsKeyboardFocus(true);
        ssgSwR4[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL4[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::l4, .title = OplGuiText::SsgSwEnv::l4, .isReset = true });
        ssgSwL4[i].setWantsKeyboardFocus(true);
        ssgSwL4[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR5[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::r5, .title = OplGuiText::SsgSwEnv::r5, .isReset = true });
        ssgSwR5[i].setWantsKeyboardFocus(true);
        ssgSwR5[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL5[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::l5, .title = OplGuiText::SsgSwEnv::l5, .isReset = true });
        ssgSwL5[i].setWantsKeyboardFocus(true);
        ssgSwL5[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR6[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::r6, .title = OplGuiText::SsgSwEnv::r6, .isReset = true });
        ssgSwR6[i].setWantsKeyboardFocus(true);
        ssgSwR6[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL6[i].setup({ .parent = *this, .id = paramPrefix + OplPrKey::SsgSwEnv::l6, .title = OplGuiText::SsgSwEnv::l6, .isReset = true });
        ssgSwL6[i].setWantsKeyboardFocus(true);
        ssgSwL6[i].setExplicitFocusOrder(++tabOrder);

        catShape[i].setupHwCategory({ .parent = *this, .title = OplGuiText::Category::shape });

        eg[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OplPrKey::eg, .title = OplGuiText::Fm::Op::Eg, .items = oplEgItems, .isReset = true });
        eg[i].setWantsKeyboardFocus(true);
        eg[i].setExplicitFocusOrder(++tabOrder);

        adsrCat[i].setupHwCategory({ .parent = *this, .title = OplGuiText::Category::visibleAdsr, .invisibleTitle = OplGuiText::Category::invisibleAdsr, .enableChangeDetailVisible = true });

        sus[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OplPrKey::sus, .title = OplGuiText::Fm::Op::sus, .isReset = true });

        catLfo[i].setupHwCategory({ .parent = *this, .title = OplGuiText::Category::visibleLfo, .invisibleTitle = OplGuiText::Category::invisibleLfo, .enableChangeDetailVisible = true });

        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OplPrKey::am, .title = OplGuiText::Fm::Op::Am, .isReset = true });
        am[i].setWantsKeyboardFocus(true);
        am[i].setExplicitFocusOrder(++tabOrder);

        ams[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OplPrKey::ams, .title = OplGuiText::Fm::Op::Ams, .isReset = true });

        amsTo37[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->3.7Hz", .isReset = false, .isResized = false });
        amsTo37[i].setWantsKeyboardFocus(true);
        amsTo37[i].setExplicitFocusOrder(++tabOrder);
        amsTo37[i].onClick = [this, index = i] { ams[index].setValue(3.7, juce::sendNotification); };

        amsTo606[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->6.06Hz", .isReset = false, .isResized = false });
        amsTo606[i].setWantsKeyboardFocus(true);
        amsTo606[i].setExplicitFocusOrder(++tabOrder);
        amsTo606[i].onClick = [this, index = i] { ams[index].setValue(6.06, juce::sendNotification); };

        amd[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OplPrKey::amd, .title = OplGuiText::Fm::Op::Amd, .isReset = true });
        amd[i].setWantsKeyboardFocus(true);
        amd[i].setExplicitFocusOrder(++tabOrder);

        amdTo1[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->1dB", .isReset = false, .isResized = false });
        amdTo1[i].setWantsKeyboardFocus(true);
        amdTo1[i].setExplicitFocusOrder(++tabOrder);
        amdTo1[i].onClick = [this, index = i] { amd[index].setValue(1.0, juce::sendNotification); };

        amdTo12[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->1.2dB", .isReset = false, .isResized = false });
        amdTo12[i].setWantsKeyboardFocus(true);
        amdTo12[i].setExplicitFocusOrder(++tabOrder);
        amdTo12[i].onClick = [this, index = i] { amd[index].setValue(1.2, juce::sendNotification); };

        amdTo48[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->4.8dB", .isReset = false, .isResized = false });
        amdTo48[i].setWantsKeyboardFocus(true);
        amdTo48[i].setExplicitFocusOrder(++tabOrder);
        amdTo48[i].onClick = [this, index = i] { amd[index].setValue(4.8, juce::sendNotification); };

        vib[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OplPrKey::vib, .title = OplGuiText::Fm::Op::Vib, .isReset = true });
        vib[i].setWantsKeyboardFocus(true);
        vib[i].setExplicitFocusOrder(++tabOrder);

        pms[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OplPrKey::pms, .title = OplGuiText::Fm::Op::Pms, .isReset = true });
        pms[i].setWantsKeyboardFocus(true);
        pms[i].setExplicitFocusOrder(++tabOrder);

        pmsTo606[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->6.06Hz", .isReset = false, .isResized = false });
        pmsTo606[i].setWantsKeyboardFocus(true);
        pmsTo606[i].setExplicitFocusOrder(++tabOrder);
        pmsTo606[i].onClick = [this, index = i] { pms[index].setValue(6.06, juce::sendNotification); };

        pmsTo64[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->6.4Hz", .isReset = false, .isResized = false });
        pmsTo64[i].setWantsKeyboardFocus(true);
        pmsTo64[i].setExplicitFocusOrder(++tabOrder);
        pmsTo64[i].onClick = [this, index = i] { pms[index].setValue(6.4, juce::sendNotification); };

        pmd[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OplPrKey::pmd, .title = OplGuiText::Fm::Op::Pmd, .isReset = true });
        pmd[i].setWantsKeyboardFocus(true);
        pmd[i].setExplicitFocusOrder(++tabOrder);

        pmdTo7[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->7cent", .isReset = false, .isResized = false});
        pmdTo7[i].setWantsKeyboardFocus(true);
        pmdTo7[i].setExplicitFocusOrder(++tabOrder);
        pmdTo7[i].onClick = [this, index = i] { pmd[index].setValue(7.0, juce::sendNotification); };

        pmdTo137[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->13.7cent", .isReset = false, .isResized = false });
        pmdTo137[i].setWantsKeyboardFocus(true);
        pmdTo137[i].setExplicitFocusOrder(++tabOrder);
        pmdTo137[i].onClick = [this, index = i] { pmd[index].setValue(13.7, juce::sendNotification); };

        pmdTo14[i].setup(GuiTextButton::Config{ .parent = *this, .title = "->14cent", .isReset = false, .isResized = false });
        pmdTo14[i].setWantsKeyboardFocus(true);
        pmdTo14[i].setExplicitFocusOrder(++tabOrder);
        pmdTo14[i].onClick = [this, index = i] { pmd[index].setValue(14.0, juce::sendNotification); };

        catMask[i].setupHwCategory({ .parent = *this, .title = OplGuiText::Category::visibleMask, .invisibleTitle = OplGuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OplPrKey::mask, .title = OplGuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        addAndMakeVisible(mmlSeparator[i]);
        mmlSeparator[i].setup({.lineThick = 2.0f, .lineColour = juce::Colours::grey});

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setWantsKeyboardFocus(true);
        mml[i].setExplicitFocusOrder(++tabOrder);
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 DR:0 SL:0 RR:15 MUL:1 DT:0",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        setupGraph(i);
        updateOpGraph(i);
    }
}

void GuiOpl::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(OplGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(OplGuiValue::Group::Padding::width, OplGuiValue::Group::Padding::height);
    mRect.removeFromTop(OplGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mRect.removeFromTop(OplGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });

    mRect.removeFromTop(OplGuiValue::Category::paddingTop);

    auto imgArea = mRect.removeFromTop(120);
    algImageComp.setBounds(imgArea);

    mRect.removeFromTop(OplGuiValue::Category::paddingTop);

    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    layoutInitializeCat(mRect);

    layoutQualityCat(mRect);

    layoutMonoModeCat(mRect);

    layoutMvolCat(mRect);

    // --- B. Operators Section ---
    for (int i = 0; i < 2; ++i)
    {
        auto opArea = pageArea.removeFromLeft(OplGuiValue::Fm::Op::width);
        opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(OplGuiValue::Fm::Op::Padding::width, OplGuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(OplGuiValue::Group::TitlePaddingTop);

        // グラフ用の区画を確保
        layoutOpGraph(i, innerRect);
        updateOpGraph(i);

        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        updateRgDisplayAsOp(i, true);

        layoutRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgDr[i].label, .component = &rgDr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgSl[i].label, .component = &rgSl[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i] });

        layoutRow({ .rowRect = innerRect, .component = &egType[i] });
        layoutRow({ .rowRect = innerRect, .component = &ksr[i] });
        layoutRow({ .rowRect = innerRect, .label = &ksl[i].label, .component = &ksl[i] });
        layoutRowCategory({ .rowRect = innerRect, .component = &catShape[i] });
        layoutRow({ .rowRect = innerRect, .label = &eg[i].label, .component = &eg[i] });

        layoutOpAdsrCat(i, innerRect);

        layoutOpPitchEnvCat(i, innerRect);

        layoutOpSsgSwEnvCat(i, innerRect);

        layoutOpLfoCat(i, innerRect);

        layoutOpMaskCat(i, innerRect);

        // 区切り線エリアを確保
        auto mmlSeparatorArea = innerRect.removeFromTop(OplGuiValue::ParamGroup::Separator::height);
        mmlSeparator[i].setBounds(mmlSeparatorArea);

        layoutRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });
    }

    updateAlgorithmDisplay();
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpl::applyMmlString(const juce::String& mml, int opIndex)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMl,   [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMask, [&](int v) { mask[opIndex].setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },
        { mmlPrefixAm,   [&](int v) { am[opIndex].setToggleState(RegisterConverter::convertOplAm(v), juce::sendNotification); } },
        { mmlPrefixVib,   [&](int v) { vib[opIndex].setToggleState(RegisterConverter::convertOplVib(v), juce::sendNotification); } },
        { mmlPrefixVb,   [&](int v) { vib[opIndex].setToggleState(RegisterConverter::convertOplVib(v), juce::sendNotification); } },
        { mmlPrefixPm,   [&](int v) { vib[opIndex].setToggleState(RegisterConverter::convertOplVib(v), juce::sendNotification); } },
        { mmlPrefixEgType, [&](int v) { egType[opIndex].setToggleState(RegisterConverter::convertOplEgType(v), juce::sendNotification); } },
        { mmlPrefixEt,   [&](int v) { egType[opIndex].setToggleState(RegisterConverter::convertOplEgType(v), juce::sendNotification); } },
        { mmlPrefixKsr,   [&](int v) { ksr[opIndex].setToggleState(RegisterConverter::convertOplKsr(v), juce::sendNotification); } },
        { mmlPrefixKr,   [&](int v) { ksr[opIndex].setToggleState(RegisterConverter::convertOplKsr(v), juce::sendNotification); } },
        { mmlPrefixKsl,   [&](int v) { ksl[opIndex].setSelectedItemIndex(RegisterConverter::convertOplKsl(v), juce::sendNotification); } },
        { mmlPrefixKl,   [&](int v) { ksl[opIndex].setSelectedItemIndex(RegisterConverter::convertOplKsl(v), juce::sendNotification); } },

        // --- TL系 ---
        { mmlPrefixTl,   [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg63(v), juce::sendNotification); }},
        { mmlPrefixO,    [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg63(v), juce::sendNotification); }},
        { mmlPrefixOl,   [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg63(v), juce::sendNotification); }},

        // --- エンベロープ系 ---
        { mmlPrefixAr,   [&](int v) { rgAr[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
        { mmlPrefixDr,   [&](int v) { rgDr[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
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

void GuiOpl::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    mul[idx].setEnabledWithLabel(enable);
    ksr[idx].setEnabled(enable);
    ksl[idx].setEnabledWithLabel(enable);
    egType[idx].setEnabled(enable);
    eg[idx].setEnabledWithLabel(enable);
    catShape[idx].setEnabled(enable);
    catLfo[idx].setEnabled(enable);
    vib[idx].setEnabled(enable);
    pms[idx].setEnabledWithLabel(enable);
    pmsTo64[idx].setEnabled(enable);
    pmd[idx].setEnabledWithLabel(enable);
    pmdTo7[idx].setEnabled(enable);
    pmdTo14[idx].setEnabled(enable);
    am[idx].setEnabled(enable);
    ams[idx].setEnabledWithLabel(enable);
    amsTo37[idx].setEnabled(enable);
    amd[idx].setEnabledWithLabel(enable);
    amdTo1[idx].setEnabled(enable);
    amdTo48[idx].setEnabled(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    mmlSeparator[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
}

void GuiOpl::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > 1) return;

    for (int i = 0; i < 2; ++i)
    {
        juce::String newTitle = OplGuiText::Group::opPrefix + juce::String(i + 1) + algOpPrefix[algIndex][i];

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

void GuiOpl::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].setVisibleWithLabel(rgMode);
    rgDr[idx].setVisibleWithLabel(rgMode);
    rgSl[idx].setVisibleWithLabel(rgMode);
    rgRr[idx].setVisibleWithLabel(rgMode);
    rgTl[idx].setVisibleWithLabel(rgMode);
}

void GuiOpl::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

// ==============================================================================
// Keyboard Shortcut Logic
// ==============================================================================
bool GuiOpl::keyPressed(const juce::KeyPress& key)
{
    int opIndex = -1;
    int code = key.getKeyCode();
    juce::ModifierKeys metaKeys = key.getModifiers();

    // 通常の 1〜4キー、または テンキーの 1〜4 を判定
    if (code == '1' || code == juce::KeyPress::numberPad1) opIndex = 0;
    else if (code == '2' || code == juce::KeyPress::numberPad2) opIndex = 1;

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

void GuiOpl::copyFmParamsToString()
{
    int mask1 = FmMml::bool2Int(!mask[0].getToggleState());
    int mask2 = FmMml::bool2Int(!mask[1].getToggleState());
    int maskVal = FmMml::genMask2(mask1, mask2);

    auto formatCoreBasic = [this, maskVal]() {
        return juce::String::formatted(
            u8"    %1d, %1d,  %2d\n",
            algSelector.getSelectedId() - 1, // ALG
            (int)feedbackSlider.getValue(),  // FB
            maskVal);                        // MASK
        };
    auto formatOpBasic = [this](int index) {
        // ' MUL AR DR SL RR  TL KSR KSL AM VIB WS EGTYPE PMS PMD AMS AMD
        return juce::String::formatted(
            u8"  %3d, %2d, %2d, %2d, %2d, %3d,  %1d,  %1d, %1d,  %1d, %1d,     %1d, %5.2f, %5.2f, %5.2f, %5.2f\n",
            (int)this->mul[index].getSelectedId() - 1,             // MUL
            (int)this->rgAr[index].getValue(),                     // AR
            (int)this->rgDr[index].getValue(),                     // DR
            (int)this->rgSl[index].getValue(),                     // SL
            (int)this->rgRr[index].getValue(),                     // RR
            (int)this->rgTl[index].getValue(),                     // TL
            FmMml::bool2Int(this->ksr[index].getToggleState()),    // KSR
            this->ksl[0].getSelectedId() - 1,                      // KSL
            FmMml::bool2Int(this->am[index].getToggleState()),     // AM
            FmMml::bool2Int(this->vib[index].getToggleState()),    // VIB
            this->eg[index].getSelectedId() - 1,                   // WS
            FmMml::bool2Int(this->egType[index].getToggleState()), // EGTYPE
            this->pms[index].getValue(),                           // PMS
            this->pmd[index].getValue(),                           // PMD
            this->ams[index].getValue(),                           // AMS
            this->amd[index].getValue()                            // AMD
        );
        };
    auto formatOpsBasic = [this, formatOpBasic]() {
        return formatOpBasic(0) + formatOpBasic(1);
        };
    auto formatOpExt = [this](int index) {
        // ' MUL AR DR SL RR TL KSR KSL
        return juce::String::formatted(
            u8"mul%d ar%d dr%d rr%d sl%d tl%d ksr%d ksl%d\n",
            (int)this->mul[index].getSelectedId() - 1,
            (int)this->rgAr[index].getValue(),
            (int)this->rgDr[index].getValue(),
            (int)this->rgSl[index].getValue(),
            (int)this->rgRr[index].getValue(),
            (int)this->rgTl[index].getValue(),
            FmMml::bool2Int(this->ksr[index].getToggleState()),
            this->ksl[index].getSelectedId() - 1
        );
        };
    auto formatOpsExt = [this, formatOpExt]() {
        return formatOpExt(0) + formatOpExt(1);
        };

    juce::String mml = juce::String("[OPL]\n")
        + FmMml::basicMmlHeader
        + juce::String(u8"' ALG FB MASK\n")
        + formatCoreBasic()
        + juce::String(u8"' MUL  AR  DR  SL  RR   TL KSR KSL AM VIB WS EGTYPE    PMS    PMD    AMS    AMD\n")
        + formatOpsBasic()
        + juce::String(u8"\n")
        + FmMml::extMmlHeader
        + formatOpsExt();

    juce::SystemClipboard::copyTextToClipboard(mml);
}

void GuiOpl::copyFmParamsToObject()
{

}

void GuiOpl::pasteFmParamsFromObject()
{

}

void GuiOpl::initParams()
{
    this->ctx.audioProcessor.initParams("OPL_");
}

void GuiOpl::layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask[opIndex] });

    bool visibleMask = catMask[opIndex].isDetailVisible();

    mask[opIndex].setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask[opIndex] });
    }
}

void GuiOpl::layoutQualityCat(juce::Rectangle<int>& rect) {
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

void GuiOpl::layoutMonoModeCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &monoPolyCat });

    bool visible = monoPolyCat.isDetailVisible();

    monoModeToggle.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoModeToggle });
    }
}

void GuiOpl::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
    }
}

void GuiOpl::layoutInitializeCat(Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &initCat });

    bool visible = initCat.isDetailVisible();

    initLfoToOplBtn.setVisible(visible);
    initLfoToOpllBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &initLfoToOplBtn });
        layoutMain({ .mainRect = rect, .component = &initLfoToOpllBtn });
    }
}

void GuiOpl::layoutOpAdsrCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &adsrCat[opIndex] });

    bool visible = adsrCat[opIndex].isDetailVisible();

    sus[opIndex].setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &sus[opIndex] });
    }
}

void GuiOpl::layoutOpLfoCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catLfo[opIndex] });

    bool visible = catLfo[opIndex].isDetailVisible();

    am[opIndex].setVisible(visible);
    ams[opIndex].setVisibleWithLabel(visible);
    amsTo37[opIndex].setVisible(visible);
    amsTo606[opIndex].setVisible(visible);
    amd[opIndex].setVisibleWithLabel(visible);
    amdTo1[opIndex].setVisible(visible);
    amdTo12[opIndex].setVisible(visible);
    amdTo48[opIndex].setVisible(visible);
    vib[opIndex].setVisible(visible);
    pms[opIndex].setVisibleWithLabel(visible);
    pmsTo606[opIndex].setVisible(visible);
    pmsTo64[opIndex].setVisible(visible);
    pmd[opIndex].setVisibleWithLabel(visible);
    pmdTo137[opIndex].setVisible(visible);
    pmdTo14[opIndex].setVisible(visible);
    pmdTo7[opIndex].setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &am[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ams[opIndex].label, .component = &ams[opIndex] });
        layoutRowTwoComps({ .rect = rect, .comp1 = &amsTo37[opIndex], .comp2 = &amsTo606[opIndex] });
        layoutRow({ .rowRect = rect, .label = &amd[opIndex].label, .component = &amd[opIndex] });
        layoutRowThreeComps({ .rect = rect, .comp1 = &amdTo1[opIndex], .comp2 = &amdTo12[opIndex], .comp3 = &amdTo48[opIndex] });
        layoutRow({ .rowRect = rect, .component = &vib[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pms[opIndex].label, .component = &pms[opIndex] });
        layoutRowTwoComps({ .rect = rect, .comp1 = &pmsTo606[opIndex], .comp2 = &pmsTo64[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pmd[opIndex].label, .component = &pmd[opIndex] });
        layoutRowThreeComps({ .rect = rect, .comp1 = &pmdTo7[opIndex], .comp2 = &pmdTo137[opIndex], .comp3 = &pmdTo14[opIndex] });
    }
}

void GuiOpl::layoutOpPitchEnvCat(int opIndex, juce::Rectangle<int>& rect)
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

void GuiOpl::layoutOpSsgSwEnvCat(int opIndex, juce::Rectangle<int>& rect)
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

void GuiOpl::applyOpSsgSwEnvLoopValues(int opIndex, bool enabled)
{
    if (enabled)
    {
        int steps = static_cast<int>(ssgSwSteps[opIndex].getValue());

        // Steps が 1 のときはループできないため、Steps を 2 にする
        if (steps < 2) {
            steps = 2;
            ssgSwSteps[opIndex].setValue(steps);
        }

        int loopTo = static_cast<int>(ssgSwLoopTo[opIndex].getValue());

        // Steps - LoopTo が 2未満のときは、LoopTo を Steps - 2 にする
        if (steps - loopTo < 2) {
            ssgSwLoopTo[opIndex].setValue(steps - 2);
        }
    }
}

void GuiOpl::setupGraph(int opIndex)
{
    addAndMakeVisible(&opGraphs[opIndex]); // グラフを追加

    graphBtnAmp[opIndex].setup({ .parent = *this, .title = "Amp", .isReset = false, .isResized = false });
    graphBtnAmp[opIndex].setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::Amp); };

    graphBtnPitch[opIndex].setup({ .parent = *this, .title = "Pitch", .isReset = false, .isResized = false });
    graphBtnPitch[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::Pitch); };

    graphBtnSsg[opIndex].setup({ .parent = *this, .title = "SSG SW", .isReset = false, .isResized = false });
    graphBtnSsg[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::SsgSw); };

    auto repaintGraph = [this, opIndex]() { updateOpGraph(opIndex); };

    rgAr[opIndex].onValueChange = repaintGraph;
    rgDr[opIndex].onValueChange = repaintGraph;
    rgSl[opIndex].onValueChange = repaintGraph;
    rgRr[opIndex].onValueChange = repaintGraph;
    rgTl[opIndex].onValueChange = repaintGraph;
    sus[opIndex].onStateChange = repaintGraph;

    pitchAttack[opIndex].onValueChange = repaintGraph;
    pitchDecay[opIndex].onValueChange = repaintGraph;
    pitchRelease[opIndex].onValueChange = repaintGraph;
    pitchStartLevel[opIndex].onValueChange = repaintGraph;
    pitchAttackLevel[opIndex].onValueChange = repaintGraph;
    pitchSustainLevel[opIndex].onValueChange = repaintGraph;
    pitchReleaseLevel[opIndex].onValueChange = repaintGraph;

    ssgSwEnvLoop[opIndex].onStateChange = repaintGraph;

    ssgSwSteps[opIndex].onValueChange = [this, opIndex, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoop[opIndex].getToggleState();
        applyOpSsgSwEnvLoopValues(opIndex, ssgEnvLoopEnable);
        repaintGraph();
        };
    ssgSwLoopTo[opIndex].onValueChange = [this, opIndex, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoop[opIndex].getToggleState();
        applyOpSsgSwEnvLoopValues(opIndex, ssgEnvLoopEnable);
        repaintGraph();
        };
    ssgSwLoopCount[opIndex].onValueChange = [this, opIndex, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoop[opIndex].getToggleState();
        applyOpSsgSwEnvLoopValues(opIndex, ssgEnvLoopEnable);
        repaintGraph();
        };

    ssgSwR1[opIndex].onValueChange = repaintGraph;
    ssgSwR2[opIndex].onValueChange = repaintGraph;
    ssgSwR3[opIndex].onValueChange = repaintGraph;
    ssgSwR4[opIndex].onValueChange = repaintGraph;
    ssgSwR5[opIndex].onValueChange = repaintGraph;
    ssgSwR6[opIndex].onValueChange = repaintGraph;

    ssgSwStartLevel[opIndex].onValueChange = repaintGraph;
    ssgSwL1[opIndex].onValueChange = repaintGraph;
    ssgSwL2[opIndex].onValueChange = repaintGraph;
    ssgSwL3[opIndex].onValueChange = repaintGraph;
    ssgSwL4[opIndex].onValueChange = repaintGraph;
    ssgSwL5[opIndex].onValueChange = repaintGraph;
    ssgSwL6[opIndex].onValueChange = repaintGraph;

    addAndMakeVisible(graphSeparator[opIndex]);
    graphSeparator[opIndex].setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void GuiOpl::setGraphMode(int opIndex, GraphMode mode)
{
    currentGraphMode[opIndex] = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp[opIndex].setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch[opIndex].setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg[opIndex].setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateOpGraph(opIndex);
}

void GuiOpl::layoutOpGraph(int opIndex, juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OplGuiValue::ParamGroup::Graph::height + OplGuiValue::ParamGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(OplGuiValue::ParamGroup::Separator::height);

    graphSeparator[opIndex].setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(OplGuiValue::ParamGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg[opIndex].setBounds(btnArea);

    // 残りをグラフエリアにする
    opGraphs[opIndex].setBounds(mainArea);
}

// グラフを再計算して描画
void GuiOpl::updateOpGraph(int opIndex)
{
    GraphMode mode = currentGraphMode[opIndex];

    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

    // -------------------------------------------------------------
    // Helper: 幅の計算 (Amp 用)
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (rateValue <= 0.0f) return maxWidth;
        float norm = 1.0f - (rateValue / maxRate);
        return maxWidth * norm;
        };

    // -------------------------------------------------------------
    // Helper: カーブ関数を生成する
    // -------------------------------------------------------------
    auto getCurveFunc = [this, isCurveMode](int posIdx, int targetIdx, int prmIdx) {
        return [this, isCurveMode, posIdx, targetIdx, prmIdx](float progress) -> float {
            if (!isCurveMode || p_curveCore == nullptr) return progress;
            return p_curveCore->process(posIdx, targetIdx, prmIdx, progress);
            };
        };

    int posIdx = opIndex + 1; // Position::Op1 = 1, Op2 = 2 ... (Common=0) に合わせる

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
        opGraphs[opIndex].updatePitchEnv(
            pitchAttack[opIndex],
            pitchDecay[opIndex],
            pitchRelease[opIndex],
            pitchStartLevel[opIndex],
            pitchAttackLevel[opIndex],
            pitchSustainLevel[opIndex],
            pitchReleaseLevel[opIndex],
            p_curveCore,
            isCurveMode,
            posIdx
        );
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        opGraphs[opIndex].updateSsgSwEnv(
            ssgSwSteps[opIndex],
            ssgSwEnvLoop[opIndex],
            ssgSwLoopTo[opIndex],
            ssgSwLoopCount[opIndex],
            { nullptr, &ssgSwR1[opIndex], &ssgSwR2[opIndex], &ssgSwR3[opIndex], &ssgSwR4[opIndex], &ssgSwR5[opIndex], &ssgSwR6[opIndex] },
            { &ssgSwStartLevel[opIndex], &ssgSwL1[opIndex], &ssgSwL2[opIndex], &ssgSwL3[opIndex], &ssgSwL4[opIndex], &ssgSwL5[opIndex], &ssgSwL6[opIndex] },
            p_curveCore,
            isCurveMode,
            posIdx
        );
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        bool isSus = sus[opIndex].getToggleState();

        float arMax = (float)rgAr[opIndex].getMaximum();
        float drMax = (float)rgDr[opIndex].getMaximum();
        float slMax = (float)rgSl[opIndex].getMaximum();
        float rrMax = (float)rgRr[opIndex].getMaximum();
        float tlMax = (float)rgTl[opIndex].getMaximum();

        float arVal = (float)rgAr[opIndex].getValue();
        float drVal = (float)rgDr[opIndex].getValue();
        float slVal = (float)rgSl[opIndex].getValue();
        float rrVal = (float)rgRr[opIndex].getValue();
        float tlVal = (float)rgTl[opIndex].getValue();

        float sl = (slMax - slVal) / slMax; // 15=0.0, 0=1.0
        float tlScale = 1.0f - (tlVal / tlMax); // TL=127で無音

        std::vector<GuiEnvelopeGraph::PhaseDef> phases;
        auto color = juce::Colours::cyan;
        int targetIdx = (int)CurveParams::Target::AmpEnv; // または RegValue

        float currentTotalWidth = 0.0f;

        // 1. Attack
        float attackWidth = rateToWidth(arVal, arMax);
        phases.push_back({
            .widthPx = attackWidth, .startLevel = 0.0f, .endLevel = 1.0f * tlScale, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Ar),
            .phaseLineColor = juce::Colours::red
            });
        currentTotalWidth += attackWidth;

        // 2. Decay
        float decayWidth = rateToWidth(drVal, drMax);
        phases.push_back({
            .widthPx = decayWidth, .startLevel = 1.0f * tlScale, .endLevel = sl * tlScale, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Dr),
            .phaseLineColor = juce::Colours::blue
            });
        currentTotalWidth += decayWidth;

        // 3. Sustain
        float releaseStartLevel = sl;
        float keyOnWidth = 60.0f;
        float sustainTotalWidth = 0.0f;

        phases.push_back({ .widthPx = keyOnWidth, .startLevel = sl * tlScale, .endLevel = sl * tlScale, .color = color, .phaseLineColor = juce::Colours::green });

        currentTotalWidth += keyOnWidth;
        float noteOffPositionX = currentTotalWidth;

        // 4. Release (通常時のみカーブを適用)
        if (isSus) {
            phases.push_back({
                .widthPx = rateToWidth(5.0f, rrMax, 200.0f),
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = 0.0f,
                .isDashed = true,
                .color = juce::Colours::yellow,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX
                });
        }
        else {
            phases.push_back({
                .widthPx = rateToWidth(rrVal, rrMax),
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = 0.0f,
                .color = color,
                .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Rr),
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX
                });
        }

        opGraphs[opIndex].setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }
}
