#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiOpna.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat opnaFormat{ "opna", 1 };
	const Io::ParamFormat opnaOpFormat{ "opnaOp", 1 };

	// OPN のファイルを読み込む口があるため、その印も要る
	const Io::ParamFormat opnFormat{ "opn", 1 };
	const Io::ParamFormat opnOpFormat{ "opnOp", 1 };
	const Io::ParamFormat n88LfoFormat{ "n88Lfo", 1 };
	const Io::ParamFormat opnaHwLfoFormat{ "opnaHwLfo", 1 };
	const Io::ParamFormat qualityFormat{ "quality", 1 };
}

#include "../Components/WavePreview/WavePreviewSource.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Const/ConstFileValues.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Opna/ProcessorOpnaKeys.h"
#include "../../Processor/Opna/ProcessorOpnaValues.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiGraphValues.h"
#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOpnaValues.h"
#include "./GuiOpnaText.h"
#include "../../Core/Gui/GuiStructs.h"

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
            mainGroup.contentCanvas.addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    const juce::String code = OpnaPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, OpnaGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    algFbCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpnaGuiText::Category::algFb });

    algSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::alg, .title = OpnaGuiText::Fm::alg, .items = opnaAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
    };

    mainGroup.contentCanvas.addAndMakeVisible(&algStaticGraphComp);

    algFbSep.setupComponent(mainGroup.contentCanvas);

    feedbackSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::fb, .title = OpnaGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    panCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpnaGuiText::Category::pan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::pan, .title = OpnaGuiText::Fm::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    panToLBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = OpnaGuiText::Fm::Pan::l, .isReset = false });
    panToLBtn.setWantsKeyboardFocus(true);
    panToLBtn.setExplicitFocusOrder(++tabOrder);
    panToLBtn.onClick = [this]() {
        panSlider.setValue(-1, juce::sendNotification);
        };

    panToCBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = OpnaGuiText::Fm::Pan::c, .isReset = false });
    panToCBtn.setWantsKeyboardFocus(true);
    panToCBtn.setExplicitFocusOrder(++tabOrder);
    panToCBtn.onClick = [this]() {
        panSlider.setValue(0, juce::sendNotification);
        };

    panToRBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = OpnaGuiText::Fm::Pan::r, .isReset = false });
    panToRBtn.setWantsKeyboardFocus(true);
    panToRBtn.setExplicitFocusOrder(++tabOrder);
    panToRBtn.onClick = [this]() {
        panSlider.setValue(1, juce::sendNotification);
        };

    lfoCat.setupSwLfoCategory({ .parent = mainGroup.contentCanvas, .title = OpnaGuiText::Category::n88Lfo, .enableChangeDetailVisible = true });

    lfoFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::freq, .title = OpnaGuiText::Fm::lfoSpeed, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoShapeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::shape, .title = OpnaGuiText::Fm::lfoShape, .items = lfoShapeItems, .isReset = true });
    lfoShapeSelector.setWantsKeyboardFocus(true);
    lfoShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::amSmoothRatio, .title = OpnaGuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmPreview.setup(mainGroup.contentCanvas, GuiColor::WavePreview::Lfo);
    lfoAmPreview.setup(mainGroup.contentCanvas, GuiColor::WavePreview::Lfo);

    auto refreshLfoPreviews = [this]() { this->updateLfoPreviews(); };

    lfoShapeSelector.onChange = refreshLfoPreviews;
    lfoAmSmRtSlider.onValueChange = refreshLfoPreviews;

    updateLfoPreviews();

    lfoSyncDelaySlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::syncDelay, .title = OpnaGuiText::Fm::lfoSyncDelay, .isReset = true });
    lfoSyncDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoSyncDelaySlider.setWantsKeyboardFocus(true);
    lfoSyncDelaySlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelayToZeroBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Async", .isReset = false, .isResized = false });
    lfoSyncDelayToZeroBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToZeroBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToZeroBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(0.0f);
        };

    lfoSyncDelayToOneBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Sync", .isReset = false, .isResized = false });
    lfoSyncDelayToOneBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToOneBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToOneBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(1.0f);
        };

    lfoPmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::pm, .title = OpnaGuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::pmd, .title = OpnaGuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::pms, .title = OpnaGuiText::Fm::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::am, .title = OpnaGuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::amd, .title = OpnaGuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoSep1.setupComponent(mainGroup.contentCanvas);
    lfoSep2.setupComponent(mainGroup.contentCanvas);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    modComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ampModComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgHwEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgHwPEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgSwEnv11g.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, "Bypass");
    ssgSwPEnv11g.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, "Bypass");

    setupGlobalGraph();

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    // 大区分。音量と音程にかかわる区分を、それぞれまとめて畳めるようにする。
    // 最初は閉じておく。
    ampMajorCat.setupMajorCategory({ .parent = mainGroup.contentCanvas, .title = CoreGuiText::MajorCategory::ampEnv, .enableChangeDetailVisible = true });
    pitchMajorCat.setupMajorCategory({ .parent = mainGroup.contentCanvas, .title = CoreGuiText::MajorCategory::pitchEnv, .enableChangeDetailVisible = true });

    // チャンネルの設定を OPZX7S のパラメータファイルとして書き出す
    exportOpzx7Btn.setup({ .parent = mainGroup.contentCanvas, .title = "[EX]OPZX7S Params", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    exportOpzx7Btn.setWantsKeyboardFocus(true);
    exportOpzx7Btn.setExplicitFocusOrder(++tabOrder);
    exportOpzx7Btn.onClick = [this] { exportOpzx7Params(); };

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = OpnaGuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnaGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    copyHwLfoParamsBtn.setup({ .parent = mainGroup.contentCanvas, .title = "HW LFO Params -> OPs", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyHwLfoParamsBtn.setWantsKeyboardFocus(true);
    copyHwLfoParamsBtn.setExplicitFocusOrder(++tabOrder);
    copyHwLfoParamsBtn.onClick = [this] {
        // 元のオペレータの値をパラメータから読んで、ほかへ配る。
        // つまみは TARGET のぶんしか無いので、つまみ越しには写せない。
        const int from = juce::jlimit(0, OpnaPrValue::ops - 1, (int)copyHwLfoFromSlider.getValue() - 1);
        const juce::String fromCode = OpnaPrKey::prefix + CPK::op + juce::String(from);

        const juce::String keys[] = {
            CPK::Fm::freqs, CPK::Fm::syncDelay, CPK::Fm::vib,
            CPK::Fm::pms, CPK::Fm::am, CPK::Fm::ams
            };

        for (int i = 0; i < OpnaPrValue::ops; i++) {
            if (i == from) continue;

            const juce::String code = OpnaPrKey::prefix + CPK::op + juce::String(i);

            for (const auto& key : keys) setParamValue(code + key, getParamValue(fromCode + key));
        }

        // 指しているオペレータの値も変わっているので、映し直す。
        rebind(currentOp());
        };

    copyHwLfoFromSlider.setup({ .parent = mainGroup.contentCanvas, .title = "FROM", .isReset = false });
    copyHwLfoFromSlider.setRange(1.0, 4.0, 1.0);
    copyHwLfoFromSlider.setNumDecimalPlacesToDisplay(0);
    copyHwLfoFromSlider.setWantsKeyboardFocus(true);
    copyHwLfoFromSlider.setExplicitFocusOrder(++tabOrder);

    uSep002.setupComponent(mainGroup.contentCanvas);

    copyParamsToOpnBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Params -> OPN", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyParamsToOpnBtn.setWantsKeyboardFocus(true);
    copyParamsToOpnBtn.setExplicitFocusOrder(++tabOrder);
    copyParamsToOpnBtn.onClick = [this] {
        ctx.editor.copyOpnaParamsToOpn();
        };

    copyParamsToOpmBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Params -> OPM", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyParamsToOpmBtn.setWantsKeyboardFocus(true);
    copyParamsToOpmBtn.setExplicitFocusOrder(++tabOrder);
    copyParamsToOpmBtn.onClick = [this] {
        ctx.editor.copyOpnaParamsToOpm();
        };

    uSep003.setupComponent(mainGroup.contentCanvas);

    copyOpParamBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Copy Op Params", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamBtn.setWantsKeyboardFocus(true);
    copyOpParamBtn.setExplicitFocusOrder(++tabOrder);
    copyOpParamBtn.onClick = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        ctx.editor.copyOpnaOpParams(from, to);
        };

    copyOpFromSlider.setup({ .parent = mainGroup.contentCanvas, .title = "FROM", .isReset = false });
    copyOpFromSlider.setRange(1.0, 4.0, 1.0);
    copyOpFromSlider.setNumDecimalPlacesToDisplay(0);
    copyOpFromSlider.setValue(1, juce::sendNotification);
    copyOpFromSlider.setWantsKeyboardFocus(true);
    copyOpFromSlider.setExplicitFocusOrder(++tabOrder);
    copyOpFromSlider.onValueChange = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        copyOpParamBtn.setEnabled(from != to);
        };

    copyOpToSlider.setup({ .parent = mainGroup.contentCanvas, .title = "TO", .isReset = false });
    copyOpToSlider.setRange(1.0, 4.0, 1.0);
    copyOpToSlider.setNumDecimalPlacesToDisplay(0);
    copyOpToSlider.setValue(2, juce::sendNotification);
    copyOpToSlider.setWantsKeyboardFocus(true);
    copyOpToSlider.setExplicitFocusOrder(++tabOrder);
    copyOpToSlider.onValueChange = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        copyOpParamBtn.setEnabled(from != to);
        };

    uSep004.setupComponent(mainGroup.contentCanvas);

    ieHwLfo.setupComponentOp(mainGroup.contentCanvas, tabOrder, "HW LFO");
    ieHwLfo.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importHwLfoParam(opIndex); };
    ieHwLfo.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportHwLfoParam(opIndex); };

    ieOpPitchEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "HW LFO");
    ieOpPitchEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importPitchEnvParam(opIndex); };
    ieOpPitchEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportPitchEnvParam(opIndex); };

    ieOpSsgSwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW Env");
    ieOpSsgSwEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwEnvParam(opIndex); };
    ieOpSsgSwEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwEnvParam(opIndex); };

    ieOpSsgSwEnv11.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW E11");
    ieOpSsgSwEnv11.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwEnv11Param(opIndex); };
    ieOpSsgSwEnv11.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwEnv11Param(opIndex); };

    ieOpSsgSwPEnv11.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW P11");
    ieOpSsgSwPEnv11.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwPEnv11Param(opIndex); };
    ieOpSsgSwPEnv11.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwPEnv11Param(opIndex); };
    ieOpSsgHwPEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG HW PEnv");
    ieOpSsgHwPEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpSsgHwPEnvParam(opIndex); };
    ieOpSsgHwPEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpSsgHwPEnvParam(opIndex); };
    ieOpWtAmpMod.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Amp Mod");
    ieOpWtAmpMod.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpWtAmpModParam(opIndex); };
    ieOpWtAmpMod.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpWtAmpModParam(opIndex); };
    ieOpSsgHwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG HW Env");
    ieOpSsgHwEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpSsgHwEnvParam(opIndex); };
    ieOpSsgHwEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpSsgHwEnvParam(opIndex); };
    ieOpWtMod.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Modulation");
    ieOpWtMod.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpWtModParam(opIndex); };
    ieOpWtMod.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpWtModParam(opIndex); };

    // TARGET。設定ひとそろいの帯へ置く。値が動いたら束縛と枠線を付け替える。
    targerOpSlider.setup({ .parent = *this, .title = "TARGET", .isReset = false });
    targerOpSlider.setRange(1.0, (double)OpnaPrValue::ops, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);
    targerOpSlider.onValueChange = [this] { applyOpTarget(); };

    uSep005.setupComponent(mainGroup.contentCanvas);

    ieOpPitchEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Pitch Env");
    ieOpPitchEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importPitchEnvParam(opIndex); };
    ieOpPitchEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportPitchEnvParam(opIndex); };

    ieOpSsgSwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW Env");
    ieOpSsgSwEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwEnvParam(opIndex); };
    ieOpSsgSwEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwEnvParam(opIndex); };

    ieOpChParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OP Params");
    ieOpChParam.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpChParam(opIndex); };
    ieOpChParam.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpChParam(opIndex); };

    imOpnOpChParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OPN OP Params");
    imOpnOpChParam.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpnOpChParam(opIndex); };

    ieAmpEnvG.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Env", ampEnvComponent);
    ieSsgHwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW Env", ssgHwEnv);
    ieSsgHwPEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW PEnv", ssgHwPEnv);
    ieWtMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Modulation", modComponent);
    ieWtAmpMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Mod", ampModComponent);

    ieSsgSwEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW E11", ssgSwEnv11g);
    ieSsgSwPEnv11g.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW P11", ssgSwPEnv11g);

    ieLfo.setupComponent(mainGroup.contentCanvas, tabOrder, "N88 LFO");
    ieLfo.onClickImport = [this] { importLfoParam(); };
    ieLfo.onClickExport = [this] { exportLfoParam(); };

    ieUnison.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Unison", unisonComponent);

    ieQuality.setupComponent(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { importQualityParam(); };
    ieQuality.onClickExport = [this] { exportQualityParam(); };

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };

    imOpnChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "OPN CH Params");
    imOpnChParam.onClickImport = [this] { importOpnChParam(); };

    const juce::String opCode = code + CPK::op;

    // ---- 上の絵と、下の設定 ----
    //
    // 絵はオペレータの数だけ。設定はひとそろいだけ置き、TARGET で
    // 指し先を切り替える。
    for (int i = 0; i < OpnaPrValue::ops; ++i)
    {
        cells[(size_t)i].setup(*this, i, OpnaGuiText::Group::opPrefix + juce::String(i + 1), false);
        cells[(size_t)i].onSelect = [this](int index) {
            targerOpSlider.setValue(index + 1, juce::sendNotification);
            };
    }

    // 区分は縦に積まず、横へ並べる。1 列 1 区分が基本。
    stripViewport.setViewedComponent(&stripCanvas, false);
    stripViewport.setScrollBarsShown(false, true);
    stripViewport.setOpaque(false);

    addAndMakeVisible(stripViewport);

    colAmp.setup(stripCanvas, juce::String("") + "AMP ENV / OPTIONAL / SSG ENVELOPE");
    colSsgHwEnv.setup(stripCanvas, juce::String("") + "SSG HW AMP ENV");
    colSsgSwEnv.setup(stripCanvas, juce::String("") + "SSG SW AMP ENV");
    colSsgSwEnv11.setup(stripCanvas, juce::String("") + "SSG SW AMP ENV[11]");
    colAmpMod.setup(stripCanvas, juce::String("") + "WT AMP MOD");
    colPitchEnv.setup(stripCanvas, juce::String("") + "PITCH ENV");
    colSsgHwPEnv.setup(stripCanvas, juce::String("") + "SSG HW PITCH ENV");
    colSsgSwPEnv11.setup(stripCanvas, juce::String("") + "SSG SW PITCH ENV[11]");
    colMod.setup(stripCanvas, juce::String("") + "WT PITCH MOD");
    colKs.setup(stripCanvas, juce::String("") + "KEY SCALE / MUL/DET / FIX");
    colHwLfo.setup(stripCanvas, juce::String("") + "HW LFO / N88 LFO");
    colMask.setup(stripCanvas, juce::String("") + "MASK / MML");

    // 今どのオペレータを触っているか。名前は rebind で入れ替える。
    opNameLabel.setup({ .parent = *this, .title = Io::empty,
        .justification = juce::Justification::centredLeft });

    opTargetSeparator.setupComponent(*this);

    // どのエンベロープを映すかの切り替え。TARGET のすぐ右へ置く。
    auto setupModeBtn = [this, &tabOrder](GuiToggleButton& btn, const juce::String& text,
        GuiEnvGraphMode mode) {
        btn.setup({ .parent = *this, .title = text, .isReset = false, .isResized = false });
        btn.setWantsKeyboardFocus(true);
        btn.setExplicitFocusOrder(++tabOrder);
        btn.onClick = [this, mode] { this->setGraphMode(mode); };
        };

    setupModeBtn(graphBtnAmp, "AMP", GuiEnvGraphMode::Amp);
    setupModeBtn(graphBtnPitch, "PIT", GuiEnvGraphMode::Pitch);
    setupModeBtn(graphBtnSsg, "SSG", GuiEnvGraphMode::SsgSw);
    setupModeBtn(graphBtnSsg11, "S11", GuiEnvGraphMode::SsgSw11);
    setupModeBtn(graphBtnSsgP11, "P11", GuiEnvGraphMode::SsgSwP11);

    graphBtnAmp.setToggleState(true, juce::dontSendNotification);

    // 組み立ての間だけ 1 番のオペレータへ繋いでおく。
    // 実際の指し先は、組み上がったあと rebind で入れ替える。
    const int i = 0;

    juce::String paramPrefix = opCode + juce::String(i);

    catDet.setupHwCategory({ .parent = colKs.contentCanvas, .title = OpnaGuiText::Category::detune, .enableChangeDetailVisible = true });
    catAmp.setupHwCategory({ .parent = colAmp.contentCanvas, .title = OpnaGuiText::Category::ampEnv, .detailVisible = true, .enableChangeDetailVisible = true });

    mul.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::mul, .title = OpnaGuiText::Fm::Op::Mul, .items = multems, .isReset = true });
    mul.setWantsKeyboardFocus(true);
    mul.setExplicitFocusOrder(++tabOrder);

    dt.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::dt, .title = OpnaGuiText::Fm::Op::Dt, .items = dtItems, .isReset = true });
    dt.setWantsKeyboardFocus(true);
    dt.setExplicitFocusOrder(++tabOrder);

    rgAr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgAr, .title = OpnaGuiText::Fm::Op::Ar, .isReset = true });
    rgAr.setWantsKeyboardFocus(true);
    rgAr.setExplicitFocusOrder(++tabOrder);

    rgDr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgDr, .title = OpnaGuiText::Fm::Op::Dr, .isReset = true });
    rgDr.setWantsKeyboardFocus(true);
    rgDr.setExplicitFocusOrder(++tabOrder);

    rgSl.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgSl, .title = OpnaGuiText::Fm::Op::Sl, .isReset = true });
    rgSl.setWantsKeyboardFocus(true);
    rgSl.setExplicitFocusOrder(++tabOrder);

    rgSr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgSr, .title = OpnaGuiText::Fm::Op::Sr, .isReset = true });
    rgSr.setWantsKeyboardFocus(true);
    rgSr.setExplicitFocusOrder(++tabOrder);

    rgRr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgRr, .title = OpnaGuiText::Fm::Op::Rr, .isReset = true });
    rgRr.setWantsKeyboardFocus(true);
    rgRr.setExplicitFocusOrder(++tabOrder);

    rgTl.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgTl, .title = OpnaGuiText::Fm::Op::Tl, .isReset = true });
    rgTl.setWantsKeyboardFocus(true);
    rgTl.setExplicitFocusOrder(++tabOrder);

    ksCat.setupHwCategory({ .parent = colKs.contentCanvas, .title = OpnaGuiText::Category::ks, .enableChangeDetailVisible = true });

    ks.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ks, .title = OpnaGuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });
    ks.setWantsKeyboardFocus(true);
    ks.setExplicitFocusOrder(++tabOrder);

    catOptional.setupSwCategory({ .parent = colAmp.contentCanvas, .title = OpnaGuiText::Category::optional, .enableChangeDetailVisible = true });

    opDelay.setupComponent(colAmp.contentCanvas, paramPrefix + CPK::delay, "DELAY", tabOrder, std::nullopt);

    opDelayNudge.setupComponent(colAmp.contentCanvas, opDelay.getSlider(), tabOrder);

    opDelaySeparator.setupComponent(colAmp.contentCanvas);

    xof.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::xof, .title = OpnaGuiText::Fm::Op::xof, .isReset = true });
    xof.setWantsKeyboardFocus(true);
    xof.setExplicitFocusOrder(++tabOrder);

    kor.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::kor, .title = OpnaGuiText::Fm::Op::kor, .isReset = true });
    kor.setWantsKeyboardFocus(true);
    kor.setExplicitFocusOrder(++tabOrder);

    bypass.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::bypass, .title = OpnaGuiText::Fm::Op::bypass, .isReset = true });
    bypass.setWantsKeyboardFocus(true);
    bypass.setExplicitFocusOrder(++tabOrder);

    pitchEnv.setupComponent(colPitchEnv.contentCanvas, paramPrefix, tabOrder, CPK::PitchAdsr::enable, OpnaGuiText::PitchAdsr::enable, true);

    ssgSwEnv.setupComponent(colSsgSwEnv.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv::enable, OpnaGuiText::SsgSwEnv::enable, true);

    ssgSwEnv11.setupComponent(colSsgSwEnv11.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv11::enable, OpnaGuiText::SsgSwEnv11::enable, true);

    ssgSwPEnv11.setupComponent(colSsgSwPEnv11.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwPEnv11::enable, OpnaGuiText::SsgSwPEnv11::enable, true);
    ssgHwPEnvOp.setupComponent(colSsgHwPEnv.contentCanvas, paramPrefix, tabOrder);
    wtAmpModOp.setupComponent(colAmpMod.contentCanvas, paramPrefix, tabOrder);
    ssgHwEnvOp.setupComponent(colSsgHwEnv.contentCanvas, paramPrefix, tabOrder);
    wtModOp.setupComponent(colMod.contentCanvas, paramPrefix, tabOrder);

    catSsgEnv.setupHwCategory({ .parent = colAmp.contentCanvas, .title = OpnaGuiText::Category::ssgEnv, .enableChangeDetailVisible = true });

    se.setup(GuiComboBox::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::se, .title = OpnaGuiText::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
    se.setWantsKeyboardFocus(true);
    se.setExplicitFocusOrder(++tabOrder);

    seFreq.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::seFreq, .title = OpnaGuiText::Fm::Op::SFreq, .isReset = true });
    seFreq.setWantsKeyboardFocus(true);
    seFreq.setExplicitFocusOrder(++tabOrder);

    sePreview.setup(colAmp.contentCanvas, GuiColor::WavePreview::AmpEnv);

    // 形と周期のどちらが変わっても描き直す
    se.onChange = [this] { updateSePreview(); };
    seFreq.onValueChange = [this] { updateSePreview(); };

    updateSePreview();

    catLfo.setupHwCategory({ .parent = colHwLfo.contentCanvas, .title = OpnaGuiText::Category::hwLfo, .enableChangeDetailVisible = true });

    freqs.setup(GuiComboBox::Config{ .parent = colHwLfo.contentCanvas, .id = paramPrefix + CPK::Fm::freqs, .title = OpnaGuiText::Fm::Op::Freqs, .items = lfoFreqsItems, .isReset = true });
    freqs.setWantsKeyboardFocus(true);
    freqs.setExplicitFocusOrder(++tabOrder);

    syncDelay.setup({.parent = colHwLfo.contentCanvas, .id = paramPrefix + CPK::Fm::syncDelay, .title = OpnaGuiText::Fm::Op::SyncDelay, .isReset = true});
    syncDelay.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    syncDelay.setWantsKeyboardFocus(true);
    syncDelay.setExplicitFocusOrder(++tabOrder);

    syncDelayToZero.setup({ .parent = colHwLfo.contentCanvas, .title = "Async", .isReset = false, .isResized = false });
    syncDelayToZero.setWantsKeyboardFocus(true);
    syncDelayToZero.setExplicitFocusOrder(++tabOrder);
    syncDelayToZero.onClick = [this, i] {
        syncDelay.setValue(0.0f);
        };

    syncDelayToOne.setup({ .parent = colHwLfo.contentCanvas, .title = "Sync", .isReset = false, .isResized = false });
    syncDelayToOne.setWantsKeyboardFocus(true);
    syncDelayToOne.setExplicitFocusOrder(++tabOrder);
    syncDelayToOne.onClick = [this, i] {
        syncDelay.setValue(1.0f);
        };

    pm.setup(GuiToggleButton::Config{ .parent = colHwLfo.contentCanvas, .id = paramPrefix + CPK::Fm::vib, .title = OpnaGuiText::Fm::Op::PmEn, .isReset = true });
    pm.setWantsKeyboardFocus(true);
    pm.setExplicitFocusOrder(++tabOrder);

    pms.setup(GuiComboBox::Config{ .parent = colHwLfo.contentCanvas, .id = paramPrefix + CPK::Fm::pms, .title = OpnaGuiText::Fm::Op::Pms, .items = hwPmsItems, .isReset = true });
    pms.setWantsKeyboardFocus(true);
    pms.setExplicitFocusOrder(++tabOrder);

    am.setup(GuiToggleButton::Config{ .parent = colHwLfo.contentCanvas, .id = paramPrefix + CPK::Fm::am, .title = OpnaGuiText::Fm::Op::AmEn, .isReset = true });
    am.setWantsKeyboardFocus(true);
    am.setExplicitFocusOrder(++tabOrder);

    ams.setup(GuiComboBox::Config{ .parent = colHwLfo.contentCanvas, .id = paramPrefix + CPK::Fm::ams, .title = OpnaGuiText::Fm::Op::Ams, .items = hwAmsItems, .isReset = true });
    ams.setWantsKeyboardFocus(true);
    ams.setExplicitFocusOrder(++tabOrder);

    catN88Lfo.setupSwLfoCategory({ .parent = colHwLfo.contentCanvas, .title = OpnaGuiText::Category::n88Lfo, .enableChangeDetailVisible = true });

    n88Ams.setup(GuiSlider::Config{ .parent = colHwLfo.contentCanvas, .id = paramPrefix + CPK::N88Lfo::ams, .title = OpnaGuiText::Fm::Op::Ams, .isReset = true });
    n88Ams.setWantsKeyboardFocus(true);
    n88Ams.setExplicitFocusOrder(++tabOrder);

    fix.setupComponent(colKs.contentCanvas, paramPrefix, tabOrder, OpnaGuiText::Fm::Op::Opzx7FreqTo440, 440, true);

    catMask.setupHwCategory({ .parent = colMask.contentCanvas, .title = OpnaGuiText::Category::mask, .enableChangeDetailVisible = true });

    mask.setup(GuiToggleButton::Config{ .parent = colMask.contentCanvas, .id = paramPrefix + CPK::Fm::mask, .title = OpnaGuiText::Fm::Op::Mask, .isReset = true });
    mask.setWantsKeyboardFocus(true);
    mask.setExplicitFocusOrder(++tabOrder);

    mmlSeparator.setupComponent(colMask.contentCanvas);

    mml.setup({ .parent = colMask.contentCanvas, .title = juce::String("") + "MML風入力", .isReset = false, .isResized = false });
    mml.setWantsKeyboardFocus(true);
    mml.setExplicitFocusOrder(++tabOrder);
    // 札は TARGET が指しているオペレータへ入れる。番号は rebind で
    // 入れ替わるので、ここでは押されたときに引き直す。
    mml.onClick = [this] {
        const int op = currentOp();

        GuiMml::openDialog(this, op, mmlHint,
            [this](juce::String text) { this->applyMmlString(text); });
        };

    setupOpGraphWiring();

    // 区分の中身は最初から開いておく。1 列 1 区分にしたので、
    // 畳んだままだと見出しだけの列が並ぶことになる。
    for (auto* group : {
        &colAmp, &colSsgHwEnv, &colSsgSwEnv, &colSsgSwEnv11,
        &colAmpMod, &colPitchEnv, &colSsgHwPEnv, &colSsgSwPEnv11,
        &colMod, &colKs, &colHwLfo, &colMask,
        })
    {
        for (auto* child : group->contentCanvas.getChildren())
        {
            if (auto* cat = dynamic_cast<GuiCategoryLabel*>(child)) cat->setDetailVisible(true);
        }
    }

    // 前に開いていたときの指し先から始める。
    const int saved = (int)ctx.audioProcessor.apvts.state.getProperty(ProcessorStateKey::opnaTarget, 0);

    targerOpSlider.setValue(juce::jlimit(0, OpnaPrValue::ops - 1, saved) + 1, juce::dontSendNotification);

    applyOpTarget();
}

void GuiOpna::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(OpnaGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(OpnaGuiValue::Group::Padding::width, OpnaGuiValue::Group::Padding::height);

    mmRect.removeFromTop(OpnaGuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    // グラフはスクロールさせない固定ヘッダに置く。
    // 他のチャンネルやオペレータと同じく、プリセット名の下に来る。
    layoutGlobalGraph(mmRect);
    updateGlobalGraph();

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.getContentWidth(), 2000);

    levelComponent.layoutComponent(mRect);

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });

    mRect.removeFromTop(OpnaGuiValue::Category::paddingTop);

    auto graphArea = mRect.removeFromTop(150); // 描画領域確保
    algStaticGraphComp.setBounds(graphArea.reduced(10));

    algFbSep.layoutComponent(mRect);

    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    mRect.removeFromTop(CoreGuiValue::Category::gapBelow);

    // [[AMP ENV]] の大区分。閉じているあいだは、中の区分を見出しごと出さない。
    const bool ampOpen = layoutMajorCategory(ampMajorCat, mRect,
        ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)
        || ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));

    ampEnvComponent.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv));
    ampEnvComponent.layoutComponent(mRect);
    ssgHwEnv.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv));
    ssgHwEnv.layoutComponent(mRect);
    ssgSwEnv11g.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11));
    ssgSwEnv11g.layoutComponent(mRect);
    ampModComponent.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));
    ampModComponent.layoutComponent(mRect);

    ampMajorCat.endMajor(mRect);

    // [[PITCH ENV]] の大区分。閉じているあいだは、中の区分を見出しごと出さない。
    const bool pitchOpen = layoutMajorCategory(pitchMajorCat, mRect,
        ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)
        || ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));

    ssgHwPEnv.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv));
    ssgHwPEnv.layoutComponent(mRect);
    ssgSwPEnv11g.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11));
    ssgSwPEnv11g.layoutComponent(mRect);
    modComponent.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));
    modComponent.layoutComponent(mRect);

    pitchMajorCat.endMajor(mRect);

    layoutN88LfoCat(mRect);

    layoutPanCat(mRect);

    unisonComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::Unison));
    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    // ---- 上は絵、下は設定 ----
    //
    // 絵は横 4 枚ずつ並べる。オペレータが 4 の倍数でないときは、
    // 足りないところを空けたままにする (詰めると番号と場所がずれる)。
    constexpr int cols = 4;
    constexpr int rows = (OpnaPrValue::ops + cols - 1) / cols;

    const int cellW = pageArea.getWidth() / cols;
    const int cellH = GuiTargetCell::naturalHeight(false);

    auto cellsArea = pageArea.removeFromTop(cellH * rows);

    for (int r = 0; r < rows; ++r) {
        auto rowArea = cellsArea.removeFromTop(cellH);

        for (int col = 0; col < cols; ++col) {
            const int i = r * cols + col;

            auto cellArea = rowArea.removeFromLeft(cellW);

            if (i >= OpnaPrValue::ops) continue;

            cells[(size_t)i].layout(cellArea);

            updateOpGraph(i);
        }
    }

    // 残りはすべて設定。
    layoutOpPanel(pageArea);
    updateAlgorithmDisplay();
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpna::applyMmlString(const juce::String& mml)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mul.setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMl,   [&](int v) { mul.setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixDt,   [&](int v) { dt.setSelectedItemIndex(RegisterConverter::convertMmlDtToReg(v), juce::sendNotification); } },
        { mmlPrefixKs,   [&](int v) { ks.setSelectedItemIndex(RegisterConverter::convertFmKs(v), juce::sendNotification); } },
        { mmlPrefixMask, [&](int v) { mask.setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },

        // --- TL系 (RGモードで分岐) ---
        { mmlPrefixTl,   [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixO,    [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixOl,   [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},

        // --- エンベロープ系 (RGモードで分岐) ---
        { mmlPrefixAr,   [&](int v) { rgAr.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixDr,   [&](int v) { rgDr.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixSr,   [&](int v) { rgSr.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixSl,   [&](int v) { rgSl.setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
        { mmlPrefixRr,   [&](int v) { rgRr.setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }}
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

// 鳴っていないオペレータは触れないようにする。
//
// つまみは TARGET のぶんしか無いので、指しているオペレータの話で
// なければ何もしない。指し先を変えたときに引き直す。
void GuiOpna::updateOpEnable(int idx, bool enable)
{
    if (idx != currentOp()) return;

    mul.setEnabledWithLabel(enable);
    dt.setEnabledWithLabel(enable);
    ks.setEnabledWithLabel(enable);
    catOptional.setEnabled(enable);
    opDelay.setEnabled(enable);
    opDelayNudge.setEnables(enable);
    xof.setEnabled(enable);
    kor.setEnabled(enable);
    bypass.setEnabled(enable);
    catSsgEnv.setEnabled(enable);
    se.setEnabledWithLabel(enable);
    seFreq.setEnabledWithLabel(enable);
    fix.setEnables(enable);
    catLfo.setEnabled(enable);
    pm.setEnabled(enable);
    pms.setEnabledWithLabel(enable);
    am.setEnabled(enable);
    ams.setEnabledWithLabel(enable);
    catMask.setEnabled(enable);
    mask.setEnabled(enable);
    mmlSeparator.setEnabled(enable);
    mml.setEnabled(enable);
    pitchEnv.setEnabled(enable);
    ssgSwEnv.setEnabled(enable);
}

void GuiOpna::updateRgDisplayAsOp(bool rgMode)
{
    rgAr.setVisibleWithLabel(rgMode);
    rgDr.setVisibleWithLabel(rgMode);
    rgSl.setVisibleWithLabel(rgMode);
    rgSr.setVisibleWithLabel(rgMode);
    rgRr.setVisibleWithLabel(rgMode);
    rgTl.setVisibleWithLabel(rgMode);
}

// TARGET が今どのオペレータを指しているか (0 から数える)。
int GuiOpna::currentOp() const
{
    return juce::jlimit(0, OpnaPrValue::ops - 1, (int)targerOpSlider.getValue() - 1);
}

// TARGET が指すオペレータへ、設定の束縛を丸ごと移す。
//
// オペレータごとに部品を並べるのをやめたので、指し先はここで差し替える。
// 見た目は組み直さない。
void GuiOpna::rebind(int opIndex)
{
    const juce::String code = OpnaPrKey::prefix + CPK::op + juce::String(opIndex);

    opNameLabel.setText(OpnaGuiText::Group::opPrefix + juce::String(opIndex + 1), juce::dontSendNotification);

    mul.rebind(code + CPK::mul);
    dt.rebind(code + CPK::dt);
    ks.rebind(code + CPK::Fm::ks);

    rgAr.rebind(code + CPK::Fm::rgAr);
    rgDr.rebind(code + CPK::Fm::rgDr);
    rgSl.rebind(code + CPK::Fm::rgSl);
    rgSr.rebind(code + CPK::Fm::rgSr);
    rgRr.rebind(code + CPK::Fm::rgRr);
    rgTl.rebind(code + CPK::Fm::rgTl);

    opDelay.getSlider().rebind(code + CPK::delay);

    xof.rebind(code + CPK::Fm::xof);
    kor.rebind(code + CPK::Fm::kor);
    bypass.rebind(code + CPK::Fm::bypass);

    se.rebind(code + CPK::Fm::se);
    seFreq.rebind(code + CPK::Fm::seFreq);

    freqs.rebind(code + CPK::Fm::freqs);
    syncDelay.rebind(code + CPK::Fm::syncDelay);
    pm.rebind(code + CPK::Fm::vib);
    pms.rebind(code + CPK::Fm::pms);
    am.rebind(code + CPK::Fm::am);
    ams.rebind(code + CPK::Fm::ams);

    n88Ams.rebind(code + CPK::N88Lfo::ams);

    mask.rebind(code + CPK::Fm::mask);

    pitchEnv.rebind(code);
    ssgSwEnv.rebind(code);
    ssgSwEnv11.rebind(code);
    ssgSwPEnv11.rebind(code);
    ssgHwPEnvOp.rebind(code);
    wtAmpModOp.rebind(code);
    ssgHwEnvOp.rebind(code);
    wtModOp.rebind(code);
    fix.rebind(code);

    updateSePreview();
}

// 指し先を切り替える。
//
// 設定の束縛を移し、枠線を付け替える。開き直したときに続きから
// 触れるよう、指し先は覚えておく。
void GuiOpna::applyOpTarget()
{
    const int op = currentOp();

    ctx.audioProcessor.apvts.state.setProperty(ProcessorStateKey::opnaTarget, op, nullptr);

    rebind(op);

    for (int i = 0; i < OpnaPrValue::ops; ++i) cells[(size_t)i].setActive(i == op);

    // 鳴っていないオペレータは触れないようにしてある。指し先が変わったので
    // 入り切りを引き直す。
    updateAlgorithmDisplay();
}

// 指し先を一時的に動かして何かをする。
//
// 設定はひとそろいしか無いので、TARGET が指していないオペレータを
// 読み書きするには、いったんそこへ繋ぎ替えるしかない。
// 終わったら必ず元へ戻す。
void GuiOpna::withOp(int opIndex, const std::function<void()>& fn)
{
    const int keep = currentOp();

    if (opIndex != keep) rebind(opIndex);

    fn();

    if (opIndex != keep) rebind(keep);
}

// 絵をまとめて切り替える。
//
// どれを映すかは絵ごとではなく、全部そろえる。並べた絵を見比べる
// ためのものなので、一枚ずつ違うものを映しても比べようがない。
void GuiOpna::setGraphMode(GuiEnvGraphMode mode)
{
    currentGraphMode = mode;

    // 札は排他。押したものだけを入れる。
    graphBtnAmp.setToggleState(mode == GuiEnvGraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GuiEnvGraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GuiEnvGraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11.setToggleState(mode == GuiEnvGraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11.setToggleState(mode == GuiEnvGraphMode::SsgSwP11, juce::dontSendNotification);

    for (int i = 0; i < OpnaPrValue::ops; ++i) updateOpGraph(i);
}

void GuiOpna::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

void GuiOpna::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > OpnaPrValue::Alg::max) return;

    // Coreのルーティング情報から FmAlgState を生成
    const auto& routing = OpnaCore::routings[algIndex];
    FmAlgState s;
    s.numOps = OpnaPrValue::ops;

    for (int i = 0; i < s.numOps; ++i) { // i = src
        s.isCarrier[i] = (routing.out[i] > 0.0f);
        for (int j = 0; j < s.numOps; ++j) { // j = dest
            s.mod[i][j] = (routing.mod[j][i] > 0.0f);
            s.fbMod[i][j] = (routing.fbMod[j][i] > 0.0f);
        }
    }

    // 生成したステートでグラフを描画
    algStaticGraphComp.updateState(s);

    // AlgRouting から出力に到達可能なオペレータを動的に判定
    auto activeOps = s.getActiveOperators();

    for (int i = 0; i < OpnaPrValue::ops; ++i)
    {
        // 配列を使わず、到達可能性から判定したフラグをセット
        opActive[(size_t)i] = activeOps[i];

        updateOpEnable(i, activeOps[i]);
    }
}

bool GuiOpna::keyPressed(const juce::KeyPress& key)
{
    // Ctrl を押しているときは TARGET を動かす。数字だけを押したときの
    // MML より先に見ること。あちらは修飾キーを見ていないので、
    // 後ろへ置くと Ctrl + 数字まで MML が食べてしまう。
    if (moveTargetByKey(targerOpSlider, key)) return true;

    int opIndex = -1;
    int code = key.getKeyCode();
    juce::ModifierKeys metaKeys = key.getModifiers();

    // 通常の 1〜4キー、または テンキーの 1〜4 を判定
    if (code == '1' || code == juce::KeyPress::numberPad1) opIndex = 0;
    else if (code == '2' || code == juce::KeyPress::numberPad2) opIndex = 1;
    else if (code == '3' || code == juce::KeyPress::numberPad3) opIndex = 2;
    else if (code == '4' || code == juce::KeyPress::numberPad4) opIndex = 3;

    if (opIndex == -1) return false; // 他のキーなら無視（通常処理へ）

    // 数字はそのままオペレータ番号を指す。TARGET は動かさない。
    //
    // 設定はひとそろいしか無いので、ここで指し先を動かすと、MML を
    // 入れるつもりで押しただけで今見ているオペレータが入れ替わる。
    //
    // 鳴っていないオペレータには何もしない。以前はつまみが押せるかどうかで
    // 見ていたが、つまみは TARGET のぶんしか無くなったので、
    // アルゴリズムから出した表を見る。
    if (!opActive[(size_t)opIndex]) return true;

    const juce::String opPrefix = OpnaPrKey::prefix + CPK::op + juce::String(opIndex);

    // Altキーを押しながら -> マスクのON/OFF、それ以外 -> MML の窓
    if (metaKeys.isAltDown()) {
        const juce::String id = opPrefix + CPK::Fm::mask;

        setParamValue(id, getParamValue(id) >= 0.5f ? 0.0f : 1.0f);
    }
    else {
        GuiMml::openDialog(this, opIndex, mmlHint,
            [this, opIndex](juce::String mml) {
                // 書き込む先は押された番号のオペレータ。TARGET は元へ戻す。
                withOp(opIndex, [this, mml] { this->applyMmlString(mml); });
            });
    }

    return true; // キー入力を消費したことをJUCEに伝える
}

void GuiOpna::copyFmParamsToString()
{
    // オペレータごとに部品を並べるのをやめたので、値はつまみからではなく
    // パラメータから直に採る。画面へ出ているのは TARGET の 1 本だけで、
    // MML には 4 本ぶんが要る。
    //
    // 選ぶ形のパラメータは、つまみの選択番号から 1 を引いたものと同じ値を
    // 持っている。以前 getSelectedId() - 1 と書いていたところがこれに当たる。
    auto& apvts = ctx.audioProcessor.apvts;

    auto opVal = [&apvts](int index, const juce::String& key) {
        return (int)GuiGraphValues::value(apvts, OpnaPrKey::prefix + CPK::op + juce::String(index) + key);
        };

    auto opMask = [&apvts](int index) {
        return FmMml::bool2Int(!GuiGraphValues::flag(apvts,
            OpnaPrKey::prefix + CPK::op + juce::String(index) + CPK::Fm::mask));
        };

    int maskVal = FmMml::genMask4(opMask(0), opMask(1), opMask(2), opMask(3));

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
    auto formatOpBasic = [&opVal](int index) {
        return juce::String::formatted(
            // ' MUL    DT   AR   DR   SL   RR   SR   TL   KS  AMS
            u8"  %2d, %+1d, %2d, %2d, %2d, %2d, %2d, %3d, %1d, %+d\n",
            opVal(index, CPK::mul),                    // MUL
            FmMml::int2dt(opVal(index, CPK::dt)),      // DT
            opVal(index, CPK::Fm::rgAr),               // AR
            opVal(index, CPK::Fm::rgDr),               // DR
            opVal(index, CPK::Fm::rgSl),               // SL
            opVal(index, CPK::Fm::rgSr),               // SR
            opVal(index, CPK::Fm::rgRr),               // RR
            opVal(index, CPK::Fm::rgTl),               // TL
            opVal(index, CPK::Fm::ks),                 // KS
            opVal(index, CPK::N88Lfo::ams)             // AMS
        );
        };
    auto formatOpsBasic = [&formatOpBasic]() {
        return formatOpBasic(0) + formatOpBasic(1) + formatOpBasic(2) + formatOpBasic(3);
        };
    auto formatOpExt = [&opVal](int index) {
        return juce::String::formatted(
            // ' MUL    DT   AR   DR   SL   SR   RR   TL   KS
            u8"MUL%d DT%+d AR%d DR%d SL%d SR%d RR%d TL%d KS%d\n",
            opVal(index, CPK::mul),
            FmMml::int2dt(opVal(index, CPK::dt)),
            opVal(index, CPK::Fm::rgAr),
            opVal(index, CPK::Fm::rgDr),
            opVal(index, CPK::Fm::rgSl),
            opVal(index, CPK::Fm::rgSr),
            opVal(index, CPK::Fm::rgRr),
            opVal(index, CPK::Fm::rgTl),
            opVal(index, CPK::Fm::ks)
        );
        };
    auto formatOpsExt = [&formatOpExt]() {
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

// オペレータの設定ひとそろい。
//
// 一番上の帯に TARGET、映すものの切り替え、指しているオペレータの名前。
// その下は区分を横へ並べ、あふれたぶんは横へ送る。
//
// 縦に積むと、下のほうの区分が畳まれた見出しの列に埋もれて見つけられない。
// 横に並べれば、どの区分も丈をいっぱいに使える。
void GuiOpna::layoutOpPanel(juce::Rectangle<int> area)
{
    {
        auto row = area.removeFromTop(20);
        auto slot = row.removeFromLeft(OpnaGuiValue::Fm::Op::Col::width);

        layoutMain({ .mainRect = slot, .label = &targerOpSlider.label, .component = &targerOpSlider });

        row.removeFromLeft(OpnaGuiValue::Fm::Op::Col::gap);

        // TARGET のすぐ右が、いま指しているものの名前。
        opNameLabel.setBounds(row.removeFromLeft(OpnaGuiValue::Fm::Op::Col::nameWidth).withHeight(18));

        // 名前と切り替えの間だけを空ける。
        row.removeFromLeft(OpnaGuiValue::Fm::Op::Col::nameGap);

        // その右が、どのエンベロープを映すかの切り替え。
        {
            const int w = OpnaGuiValue::ParamGroup::Graph::ModeButtonWidth;

            auto btnRow = row.removeFromLeft(w * 5).withHeight(OpnaGuiValue::ParamGroup::Graph::ButtonHeight);

            graphBtnAmp.setBounds(btnRow.removeFromLeft(w));
            graphBtnPitch.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg11.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsgP11.setBounds(btnRow);
        }
    }

    opTargetSeparator.layoutComponent(area);

    stripViewport.setBounds(area);

    const int colW = OpnaGuiValue::Fm::Op::Col::width;
    const int colGap = OpnaGuiValue::Fm::Op::Col::gap;

    // 横の送り棒が出るぶんだけ、中身の丈を短くする。
    const int colH = juce::jmax(1, area.getHeight() - stripViewport.getScrollBarThickness());

    int x = 0;

    // 1 列を切り出して、中身を上から積む。
    // 簡易表示モードで隠す区分は、列ごと出さない。隠したぶんだけ
    // 右の列が左へ詰まる。
    auto layoutCol = [&](GuiScrollGroup& group, bool show, auto&& body)
        {
            if (!show) {
                group.setVisible(false);

                return;
            }

            group.setVisible(true);

            juce::Rectangle<int> colArea(x, 0, colW, colH);

            x += colW + colGap;

            group.setBounds(colArea);

            auto inner = colArea.reduced(OpnaGuiValue::Group::Padding::width, OpnaGuiValue::Group::Padding::height);

            inner.removeFromTop(OpnaGuiValue::Group::TitlePaddingTop);

            group.setViewportCustomBounds(inner.translated(-colArea.getX(), -colArea.getY()));

            juce::Rectangle<int> rect(0, 0, group.getContentWidth(), 20000);

            body(rect);

            group.setContentHeight(rect.getY() + 20);
        };

    const auto shown = [this](SimpleView::Cat cat) { return ctx.audioProcessor.isSimpleShown(cat); };

    // 1 区分ずつでは丈が余るので、いくつかの区分は 1 列へ積んである。
    layoutCol(colAmp, true, [&](juce::Rectangle<int>& rect) {
        updateRgDisplayAsOp(true);
        layoutOpAmpCat(rect);
        layoutOpOptionalCat(rect);
        layoutOpSsgEnvelopeCat(rect);
        });

    layoutCol(colSsgHwEnv, shown(SimpleView::SsgHwAmpEnv), [&](juce::Rectangle<int>& rect) {
        ssgHwEnvOp.setCategoryVisible(true);
        ssgHwEnvOp.layoutComponent(rect);
        });


    layoutCol(colSsgSwEnv, shown(SimpleView::SsgSwAmpEnv), [&](juce::Rectangle<int>& rect) {
        ssgSwEnv.setCategoryVisible(true);
        ssgSwEnv.layoutComponent(rect);
        });

    layoutCol(colSsgSwEnv11, shown(SimpleView::SsgSwAmpEnv11), [&](juce::Rectangle<int>& rect) {
        ssgSwEnv11.setCategoryVisible(true);
        ssgSwEnv11.layoutComponent(rect);
        });

    layoutCol(colAmpMod, shown(SimpleView::WtAmpMod), [&](juce::Rectangle<int>& rect) {
        wtAmpModOp.setCategoryVisible(true);
        wtAmpModOp.layoutComponent(rect);
        });

    layoutCol(colPitchEnv, shown(SimpleView::PitchEnv), [&](juce::Rectangle<int>& rect) {
        pitchEnv.setCategoryVisible(true);
        pitchEnv.layoutComponent(rect);
        });

    layoutCol(colSsgHwPEnv, shown(SimpleView::SsgHwPitchEnv), [&](juce::Rectangle<int>& rect) {
        ssgHwPEnvOp.setCategoryVisible(true);
        ssgHwPEnvOp.layoutComponent(rect);
        });

    layoutCol(colSsgSwPEnv11, shown(SimpleView::SsgSwPitchEnv11), [&](juce::Rectangle<int>& rect) {
        ssgSwPEnv11.setCategoryVisible(true);
        ssgSwPEnv11.layoutComponent(rect);
        });

    layoutCol(colMod, shown(SimpleView::WtPitchMod), [&](juce::Rectangle<int>& rect) {
        wtModOp.setCategoryVisible(true);
        wtModOp.layoutComponent(rect);
        });

    layoutCol(colKs, true, [&](juce::Rectangle<int>& rect) {
        layoutOpKsCat(rect);
        layoutOpDetCat(rect);

        fix.setCategoryVisible(shown(SimpleView::Fix));
        fix.layoutComponent(rect);
        });

    layoutCol(colHwLfo, true, [&](juce::Rectangle<int>& rect) {
        layoutOpHwLfoCat(rect);
        layoutOpN88LfoCat(rect);
        });

    // MASK と MML の札も 1 列へまとめてある。
    layoutCol(colMask, true, [&](juce::Rectangle<int>& rect) {
        layoutOpMaskCat(rect);

        mmlSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &mml, .paddingBottom = 0 });
        });

    stripCanvas.setSize(juce::jmax(1, x - colGap), colH);
}

void GuiOpna::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    exportOpzx7Btn.setVisible(visible);

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    copyParamsToOpnBtn.setVisible(visible);
    copyParamsToOpmBtn.setVisible(visible);
    uSep002.setVisible(visible);
    copyHwLfoParamsBtn.setVisible(visible);
    copyHwLfoFromSlider.setVisibleWithLabel(visible);
    uSep003.setVisible(visible);
    copyOpParamBtn.setVisible(visible);
    copyOpFromSlider.setVisibleWithLabel(visible);
    copyOpToSlider.setVisibleWithLabel(visible);
    uSep004.setVisible(visible);
    ieHwLfo.setVisible(visible);
    ieOpPitchEnv.setVisible(visible);
    ieOpSsgSwEnv.setVisible(visible);
    ieOpSsgSwEnv11.setVisible(visible);
    ieOpSsgSwPEnv11.setVisible(visible);
    ieOpSsgHwPEnv.setVisible(visible);
    ieOpWtAmpMod.setVisible(visible);
    ieOpSsgHwEnv.setVisible(visible);
    ieOpWtMod.setVisible(visible);
    ieOpChParam.setVisible(visible);
    imOpnOpChParam.setVisible(visible);
    uSep005.setVisible(visible);
    ieAmpEnvG.setVisible(visible);
    ieSsgHwEnv.setVisible(visible);
    ieSsgHwPEnv.setVisible(visible);
    ieWtMod.setVisible(visible);
    ieWtAmpMod.setVisible(visible);
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11g.setVisible(visible);
    ieLfo.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);
    ieChParam.setVisible(visible);
    imOpnChParam.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyHwLfoParamsBtn });
        layoutMain({ .mainRect = rect, .label = &copyHwLfoFromSlider.label, .component = &copyHwLfoFromSlider });

        uSep002.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyParamsToOpnBtn });
        layoutMain({ .mainRect = rect, .component = &copyParamsToOpmBtn });

        uSep003.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyOpParamBtn });
        layoutMain({ .mainRect = rect, .label = &copyOpFromSlider.label, .component = &copyOpFromSlider });
        layoutMain({ .mainRect = rect, .label = &copyOpToSlider.label, .component = &copyOpToSlider });

        uSep004.layoutComponent(rect);

        ieHwLfo.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpPitchEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwPEnv11.layoutComponent(rect);
        ieOpSsgHwPEnv.layoutComponent(rect);
        ieOpWtAmpMod.layoutComponent(rect);
        ieOpSsgHwEnv.layoutComponent(rect);
        ieOpWtMod.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpChParam.layoutComponent(rect);
        rect.removeFromTop(4);
        imOpnOpChParam.layoutComponent(rect);
        rect.removeFromTop(4);

        uSep005.layoutComponent(rect);

        ieAmpEnvG.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgHwEnv.layoutComponent(rect);
        ieSsgHwPEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieWtMod.layoutComponent(rect);
        ieWtAmpMod.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwPEnv11g.layoutComponent(rect);
        rect.removeFromTop(4);
        ieLfo.layoutComponent(rect);
        rect.removeFromTop(4);
        ieUnison.layoutComponent(rect);
        rect.removeFromTop(4);
        ieQuality.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);
        rect.removeFromTop(4);
        imOpnChParam.layoutComponent(rect);

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .component = &exportOpzx7Btn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpna::layoutOpMaskCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask });

    bool visibleMask = catMask.isDetailVisible();

    mask.setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }

    // MASK の後ろには分類の外の行 (セパレータや MML) が続くので、
    // 板をここで閉じないと 1 行ぶん下まで伸びてしまう。
    closeCategoryBackdrops(catMask.getParentComponent(), rect.getY());
}

void GuiOpna::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
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

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// 選んだ Shape を実際の LFO で走らせ、折れ線にして渡す。
// 値が変わったときだけ通るので、常時の負荷は無い。
void GuiOpna::updateLfoPreviews()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updateLfoPreviews(); })) return;

    // Shape は 1 つだが、同じ番号でも PM と AM で波形が違うので両方出す。
    int shape = lfoShapeSelector.getSelectedItemIndex();

    // PM は -1.0〜1.0 の両振り
    lfoPmPreview.setPoints(WavePreviewSource::n88LfoPm(shape), true);

    // AM は 0.0〜1.0 の片側。スムースの効きも見えるよう実際の値を渡す。
    lfoAmPreview.setPoints(WavePreviewSource::n88LfoAm(shape, (float)lfoAmSmRtSlider.getValue()), false);
}

void GuiOpna::layoutN88LfoCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &lfoCat });

    bool visible = lfoCat.isDetailVisible();

    lfoFreqSlider.setVisibleWithLabel(visible);
    lfoShapeSelector.setVisibleWithLabel(visible);
    lfoPmPreview.setVisible(visible);
    lfoAmPreview.setVisible(visible);
    lfoAmSmRtSlider.setVisibleWithLabel(visible);
    lfoSyncDelaySlider.setVisibleWithLabel(visible);
    lfoSyncDelayToZeroBtn.setVisible(visible);
    lfoSyncDelayToOneBtn.setVisible(visible);
    lfoSep1.setVisible(visible);
    lfoPmToggle.setVisible(visible);
    lfoPmsSlider.setVisibleWithLabel(visible);
    lfoPmdSlider.setVisibleWithLabel(visible);
    lfoSep2.setVisible(visible);
    lfoAmToggle.setVisible(visible);
    lfoAmdSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoShapeSelector.label, .component = &lfoShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
        layoutMain({ .mainRect = rect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
        layoutMainTwoComps({ .rect = rect, .comp1 = &lfoSyncDelayToZeroBtn, .comp2 = &lfoSyncDelayToOneBtn });
        lfoSep1.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &lfoPmToggle });
        lfoPmPreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        layoutMain({ .mainRect = rect, .label = &lfoPmsSlider.label, .component = &lfoPmsSlider });
        lfoSep2.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        lfoAmPreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// オペレータの SSG HW ENV の形を描き直す。
//
// 選べる並びは先頭が「掛けない」で、そのあとに実機の 8 種類が続く。
// 実際の形の番号は 1 つずれるので、そのぶんを引いて渡す。
void GuiOpna::updateSePreview()
{
    int selected = se.getSelectedItemIndex();

    if (selected <= 0)
    {
        // 掛けないときは平らな線にする
        sePreview.setPoints(std::vector<float>(2, 1.0f), false);

        return;
    }

    sePreview.setPoints(
        WavePreviewSource::ssgHwEnv(selected - 1, 0.0f, 1.0f, false), false);
}

void GuiOpna::layoutOpSsgEnvelopeCat(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catSsgEnv });

    bool visible = catSsgEnv.isDetailVisible();

    se.setVisibleWithLabel(visible);
    seFreq.setVisibleWithLabel(visible);
    sePreview.setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &se.label, .component = &se });
        layoutRow({ .rowRect = rect, .label = &seFreq.label, .component = &seFreq });

        sePreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(3);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpna::layoutOpHwLfoCat(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catLfo });

    bool visible = catLfo.isDetailVisible();

    freqs.setVisibleWithLabel(visible);
    syncDelay.setVisibleWithLabel(visible);
    syncDelayToZero.setVisible(visible);
    syncDelayToOne.setVisible(visible);
    pm.setVisible(visible);
    pms.setVisibleWithLabel(visible);
    am.setVisible(visible);
    ams.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &freqs.label, .component = &freqs });
        layoutRow({ .rowRect = rect, .label = &syncDelay.label, .component = &syncDelay });
        layoutRowTwoComps({ .rect = rect, .comp1 = &syncDelayToZero, .comp2 = &syncDelayToOne });
        layoutRow({ .rowRect = rect, .component = &pm });
        layoutRow({ .rowRect = rect, .label = &pms.label, .component = &pms });
        layoutRow({ .rowRect = rect, .component = &am });
        layoutRow({ .rowRect = rect, .label = &ams.label, .component = &ams });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpna::layoutOpN88LfoCat(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catN88Lfo });

    bool visible = catN88Lfo.isDetailVisible();

    n88Ams.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &n88Ams.label, .component = &n88Ams });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpna::layoutOpKsCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat });

    bool visible = ksCat.isDetailVisible();

    ks.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &ks.label, .component = &ks });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpna::layoutOpDetCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catDet });

    bool visible = catDet.isDetailVisible();

    mul.setVisibleWithLabel(visible);
    dt.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &mul.label, .component = &mul });
        layoutRow({ .rowRect = rect, .label = &dt.label, .component = &dt });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpna::layoutOpAmpCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catAmp });

    bool visible = catAmp.isDetailVisible();

    rgAr.setVisibleWithLabel(visible);
    rgDr.setVisibleWithLabel(visible);
    rgSr.setVisibleWithLabel(visible);
    rgSl.setVisibleWithLabel(visible);
    rgRr.setVisibleWithLabel(visible);
    rgTl.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &rgAr.label, .component = &rgAr });
        layoutRow({ .rowRect = rect, .label = &rgDr.label, .component = &rgDr });
        layoutRow({ .rowRect = rect, .label = &rgSr.label, .component = &rgSr });
        layoutRow({ .rowRect = rect, .label = &rgSl.label, .component = &rgSl });
        layoutRow({ .rowRect = rect, .label = &rgRr.label, .component = &rgRr });
        layoutRow({ .rowRect = rect, .label = &rgTl.label, .component = &rgTl });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// ============================================================================
// チップ全体のエンベロープを見るグラフ
// ============================================================================
// オペレータごとのグラフと同じ作りで、対象がチップ全体の 3 つになる。
// 値が変わったときだけ描き直すので、常時の負荷は無い。
void GuiOpna::setupGlobalGraph()
{
    addAndMakeVisible(&gGraph);

    gGraphBtnAmp.setup({ .parent = *this, .title = "AMP", .isReset = false, .isResized = false });
    gGraphBtnAmp.setToggleState(true, juce::dontSendNotification);
    gGraphBtnAmp.onClick = [this] { setGlobalGraphMode(GlobalGraphMode::Amp); };

    gGraphBtnSsg11.setup({ .parent = *this, .title = "S11", .isReset = false, .isResized = false });
    gGraphBtnSsg11.onClick = [this] { setGlobalGraphMode(GlobalGraphMode::SsgSw11); };

    gGraphBtnSsgP11.setup({ .parent = *this, .title = "P11", .isReset = false, .isResized = false });
    gGraphBtnSsgP11.onClick = [this] { setGlobalGraphMode(GlobalGraphMode::SsgSwP11); };

    auto repaintGraph = [this]() {
        if (this->isUpdatingGraph) return;

        // 旗は必ず下ろす。途中で抜けたときに立ちっぱなしになると、
        // 以後グラフの更新が全部素通りしてしまうため。
        const juce::ScopedValueSetter<bool> guard(this->isUpdatingGraph, true);

        this->updateGlobalGraph();
        };

    ampEnvComponent.setupGraph(repaintGraph);
    ssgSwEnv11g.setupGraph(repaintGraph);
    ssgSwPEnv11g.setupGraph(repaintGraph);

    gGraphSeparator.setupComponent(*this);
}

void GuiOpna::layoutGlobalGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OpnaGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    gGraphSeparator.layoutComponentBottom(mainArea);

    auto btnArea = mainArea.removeFromBottom(OpnaGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    gGraphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsg11.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsgP11.setBounds(btnArea);

    gGraph.setBounds(mainArea);
}

void GuiOpna::updateGlobalGraph()
{

    if (currentGlobalGraphMode == GlobalGraphMode::SsgSw11) {
        ssgSwEnv11g.updateGraph(gGraph);
    }
    else if (currentGlobalGraphMode == GlobalGraphMode::SsgSwP11) {
        ssgSwPEnv11g.updateGraph(gGraph);
    }
    else {
        ampEnvComponent.updateGraph(gGraph);
    }
}

void GuiOpna::setGlobalGraphMode(GlobalGraphMode mode)
{
    currentGlobalGraphMode = mode;

    // ラジオボタン的な排他制御
    gGraphBtnAmp.setToggleState(mode == GlobalGraphMode::Amp, juce::dontSendNotification);
    gGraphBtnSsg11.setToggleState(mode == GlobalGraphMode::SsgSw11, juce::dontSendNotification);
    gGraphBtnSsgP11.setToggleState(mode == GlobalGraphMode::SsgSwP11, juce::dontSendNotification);

    updateGlobalGraph();
}

// つまみが動いたら、上の絵を描き直してもらうためのつなぎ。
//
// グラフそのものはここには無い。オペレータの数だけ同時に出るので、
// 設定の側で持つわけにいかない。描き直すのは TARGET が指している
// 1 枚だけでよい。ほかのオペレータの値は変わっていない。
void GuiOpna::setupOpGraphWiring()
{
    auto repaintGraph = [this]() {
        if (this->isUpdatingGraph) return; // 既に更新中なら無視

        // 旗は必ず下ろす。途中で抜けたときに立ちっぱなしになると、
        // 以後グラフの更新が全部素通りしてしまうため。
        const juce::ScopedValueSetter<bool> guard(this->isUpdatingGraph, true);

        this->updateOpGraph(this->currentOp());
        };

    bypass.onStateChange = repaintGraph;
    xof.onStateChange = repaintGraph;
    kor.onStateChange = repaintGraph;

    rgAr.onValueChange = repaintGraph;
    rgDr.onValueChange = repaintGraph;
    rgSr.onValueChange = repaintGraph;
    rgSl.onValueChange = repaintGraph;
    rgRr.onValueChange = repaintGraph;
    rgTl.onValueChange = repaintGraph;

    pitchEnv.setupGraph(repaintGraph);
    ssgSwEnv.setupGraph(repaintGraph);
    ssgSwEnv11.setupGraph(repaintGraph);
    ssgSwPEnv11.setupGraph(repaintGraph);
}

// 枠に出すエンベロープを描き直す。
//
// 枠はオペレータの数だけ同時に出るので、値をつまみから読むわけには
// いかない。接頭辞を頼りにパラメータから直に採る。
void GuiOpna::updateOpGraph(int opIndex)
{
    auto& apvts = ctx.audioProcessor.apvts;
    auto& graph = cells[(size_t)opIndex].graph();

    const juce::String code = OpnaPrKey::prefix + CPK::op + juce::String(opIndex);

    // -------------------------------------------------------------
    // Helper: 幅の計算 (Amp 用)
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return maxWidth;

        if (rateValue <= 0.0f) return maxWidth;

        float norm = 1.0f - (rateValue / maxRate);

        return maxWidth * norm;
        };

    // =============================================================
    // Pitch Env
    // =============================================================
    if (currentGraphMode == GuiEnvGraphMode::Pitch) {
        // オペレータの区分は「入れる」札なので、切ってあるときが素通し。
        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::PitchAdsr::enable));
        graph.setKeepLevels(GuiGraphValues::pitchEnvKeep(apvts, code));
        graph.updatePitchEnv(GuiGraphValues::pitchEnv(apvts, code));
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSw) {
        const auto v = GuiGraphValues::ssgSwEnv(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwEnv::enable));
        graph.updateSsgSwEnv(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    // =============================================================
    // SSG SW Env 11
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSw11) {
        const auto v = GuiGraphValues::ssgSwEnv11(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwEnv11::enable));
        graph.setKeepLevels(GuiGraphValues::ssgSwEnv11Keep(apvts, code));
        graph.updateSsgSwEnv11(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    // =============================================================
    // SSG SW PEnv 11
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSwP11) {
        const auto v = GuiGraphValues::ssgSwPEnv11(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwPEnv11::enable));
        graph.setKeepLevels(GuiGraphValues::ssgSwPEnv11Keep(apvts, code));
        graph.updateSsgSwPEnv11(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        bool isBypass = GuiGraphValues::flag(apvts, code + CPK::Fm::bypass);

        graph.updateBypass(isBypass);

        if (isBypass) {
            graph.repaint();

            return;
        }

        bool isXof = GuiGraphValues::flag(apvts, code + CPK::Fm::xof);
        bool isKor = GuiGraphValues::flag(apvts, code + CPK::Fm::kor);

        float arMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgAr);
        float drMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgDr);
        float srMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgSr);
        float slMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgSl);
        float rrMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgRr);
        float tlMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgTl);

        float arVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgAr);
        float drVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgDr);
        float srVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgSr);
        float slVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgSl);
        float rrVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgRr);
        float tlVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgTl);

        float sl = (slMax - slVal) / slMax; // 15=0.0, 0=1.0
        float tlScale = 1.0f - (tlVal / tlMax); // TL=127で無音

        if (std::isnan(sl) || std::isinf(sl)) sl = 0.0f;
        if (std::isnan(tlScale) || std::isinf(tlScale)) tlScale = 1.0f;

        std::vector<GuiEnvelopeGraph::PhaseDef> phases;
        auto color = juce::Colours::cyan;

        float currentTotalWidth = 0.0f;

        // 1. Attack
        float attackWidth = rateToWidth(arVal, arMax);
        phases.push_back({
            .widthPx = attackWidth, .startLevel = 0.0f, .endLevel = 1.0f * tlScale, .color = color,
            .phaseLineColor = juce::Colours::red
            });
        currentTotalWidth += attackWidth;

        // 2. Decay 1
        float decayWidth = rateToWidth(drVal, drMax);
        phases.push_back({
            .widthPx = decayWidth, .startLevel = 1.0f * tlScale, .endLevel = sl * tlScale, .color = color,
            .phaseLineColor = juce::Colours::blue
            });
        currentTotalWidth += decayWidth;

        // 3. Sustain (SR) の実線部分 (キーオン中)
        float releaseStartLevel = sl;
        float keyOnWidth = 60.0f;
        float sustainTotalWidth = 0.0f;

        if (srVal > 0.0f && srVal < srMax) {
            sustainTotalWidth = rateToWidth(srVal, srMax, 300.0f);

            releaseStartLevel = sl - (sl * 0.5f);

            phases.push_back({
                .widthPx = sustainTotalWidth, .startLevel = sl * tlScale, .endLevel = 0.0f, .color = color,
                .phaseLineColor = juce::Colours::green
                });

            currentTotalWidth += sustainTotalWidth * 0.5f;
        }
        else {
            phases.push_back({ .widthPx = keyOnWidth, .startLevel = sl * tlScale, .endLevel = sl * tlScale, .color = color, .phaseLineColor = juce::Colours::green });

            currentTotalWidth += keyOnWidth;
        }
        float noteOffPositionX = currentTotalWidth;

        // 4. Release
        if (isXof) {
            phases.push_back({
                .widthPx = 100.0f,
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = releaseStartLevel * tlScale,
                .isDashed = true,
                .color = juce::Colours::yellow,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX
                });
        }
        else if (isKor) {
            phases.push_back({
                .widthPx = rateToWidth(rrVal, rrMax),
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = releaseStartLevel * tlScale,
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
                .color = srVal > 0.0f ? juce::Colours::yellow : color,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX,
                .isMax = (rrVal == rrMax)
                });
        }

        graph.setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }

    graph.repaint();
}

void GuiOpna::layoutOpOptionalCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catOptional });

    bool visible = catOptional.isDetailVisible();

    opDelay.setVisibles(visible);
    opDelayNudge.setVisibles(visible && opDelay.isVisibleNudge());
    opDelaySeparator.setVisible(visible);
    xof.setVisible(visible);
    kor.setVisible(visible);
    bypass.setVisible(visible);

    if (visible)
    {
        opDelay.layoutComponentRow(rect);

        if (opDelay.isVisibleNudge()) opDelayNudge.layoutComponentRow(rect);

        opDelaySeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &xof });
        layoutRow({ .rowRect = rect, .component = &kor });
        layoutRow({ .rowRect = rect, .component = &bypass });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpna::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiOpna::copyParams(CopyOpna& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();
    copyObj.pan.pan = panSlider.getValue();

    copyObj.n88Lfo.freq = lfoFreqSlider.getValue();
    copyObj.n88Lfo.wave = lfoShapeSelector.getSelectedId();
    copyObj.n88Lfo.amSmRt =lfoAmSmRtSlider.getValue();
    copyObj.n88Lfo.syncDelay = lfoSyncDelaySlider.getValue();
    copyObj.n88Lfo.pmEnable = lfoPmToggle.getToggleState();
    copyObj.n88Lfo.amEnable = lfoAmToggle.getToggleState();
    copyObj.n88Lfo.pmd = lfoPmdSlider.getValue();
    copyObj.n88Lfo.pms = lfoPmsSlider.getValue();
    copyObj.n88Lfo.amd = lfoAmdSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

// 指しているオペレータとは限らない番号が来るので、いったんそこへ
// 繋ぎ替えてから読む。設定はひとそろいしか無いため。
void GuiOpna::copyOpParams(int p, CopyOpnaOp& copyObj) {
    withOp(p, [this, &copyObj] { copyOpParamsHere(copyObj); });
}

void GuiOpna::copyOpParamsHere(CopyOpnaOp& copyObj) {
    copyObj.detune.mul = mul.getSelectedId();
    copyObj.detune.dt = dt.getSelectedId();
    copyObj.aAdsr.ar = rgAr.getValue();
    copyObj.aAdsr.dr = rgDr.getValue();
    copyObj.aAdsr.sl = rgSl.getValue();
    copyObj.aAdsr.sr = rgSr.getValue();
    copyObj.aAdsr.rr = rgRr.getValue();
    copyObj.aAdsr.tl = rgTl.getValue();
    copyObj.ssgEg.ssgEg = se.getSelectedId();
    copyObj.ssgEg.fmSsgEgFreq = seFreq.getValue();
    copyObj.aAdsr.bypass = bypass.getToggleState();
    copyObj.aAdsr.kor = kor.getToggleState();
    copyObj.aAdsr.xof = xof.getToggleState();
    copyObj.aAdsr.ks = ks.getSelectedId();

    copyObj.opnaLfo.freqsIndex = freqs.getSelectedId();
    copyObj.opnaLfo.syncDelay = syncDelay.getValue();
    copyObj.opnaLfo.pm = pm.getToggleState();
    copyObj.opnaLfo.pms = pms.getSelectedId();
    copyObj.opnaLfo.am = am.getToggleState();
    copyObj.opnaLfo.ams = ams.getSelectedId();

    copyObj.n88Lfo.ams = n88Ams.getValue();

    copyObj.mask.mask = mask.getToggleState();
        
    fix.copyParams(copyObj.fix);
    pitchEnv.copyParams(copyObj.pAdsr);
    ssgSwEnv.copyParams(copyObj.aSsgSw);
}

void GuiOpna::pasteParams(CopyOpna& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);
    panSlider.setValue(copyObj.pan.pan, juce::sendNotification);

    lfoFreqSlider.setValue(copyObj.n88Lfo.freq, juce::sendNotification);
    lfoShapeSelector.setSelectedId(copyObj.n88Lfo.wave, juce::sendNotification);
    lfoAmSmRtSlider.setValue(copyObj.n88Lfo.amSmRt, juce::sendNotification);
    lfoSyncDelaySlider.setValue(copyObj.n88Lfo.syncDelay, juce::sendNotification);
    lfoPmToggle.setToggleState(copyObj.n88Lfo.pmEnable, juce::sendNotification);
    lfoAmToggle.setToggleState(copyObj.n88Lfo.amEnable, juce::sendNotification);
    lfoPmdSlider.setValue(copyObj.n88Lfo.pmd, juce::sendNotification);
    lfoPmsSlider.setValue(copyObj.n88Lfo.pms, juce::sendNotification);
    lfoAmdSlider.setValue(copyObj.n88Lfo.amd, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpna::pasteOpParams(int p, CopyOpnaOp& copyObj) {
    withOp(p, [this, &copyObj] { pasteOpParamsHere(copyObj); });
}

void GuiOpna::pasteOpParamsHere(CopyOpnaOp& copyObj) {
    mul.setSelectedId(copyObj.detune.mul, juce::sendNotification);
    dt.setSelectedId(copyObj.detune.dt, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr.setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl.setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgSr.setValue(copyObj.aAdsr.sr, juce::sendNotification);
    rgRr.setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl.setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass.setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor.setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof.setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ks.setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);
    se.setSelectedId(copyObj.ssgEg.ssgEg, juce::sendNotification);
    seFreq.setValue(copyObj.ssgEg.fmSsgEgFreq, juce::sendNotification);

    freqs.setSelectedId(copyObj.opnaLfo.freqsIndex, juce::sendNotification);
    syncDelay.setValue(copyObj.opnaLfo.syncDelay, juce::sendNotification);
    pm.setToggleState(copyObj.opnaLfo.pm, juce::sendNotification);
    pms.setSelectedId(copyObj.opnaLfo.pms, juce::sendNotification);
    am.setToggleState(copyObj.opnaLfo.am, juce::sendNotification);
    ams.setSelectedId(copyObj.opnaLfo.ams, juce::sendNotification);

    n88Ams.setValue(copyObj.n88Lfo.ams, juce::sendNotification);

    mask.setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix.pasteParams(copyObj.fix);
    pitchEnv.pasteParams(copyObj.pAdsr);
    ssgSwEnv.pasteParams(copyObj.aSsgSw);
}

void GuiOpna::copyParamsOpm(CopyOpnOpm& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpna::copyOpParamsOpm(int p, CopyOpnOpmOp& copyObj) {
    withOp(p, [this, &copyObj] { copyOpParamsOpmHere(copyObj); });
}

void GuiOpna::copyOpParamsOpmHere(CopyOpnOpmOp& copyObj) {
    copyObj.detune.mul = mul.getSelectedId();
    copyObj.detune.dt = dt.getSelectedId();
    copyObj.aAdsr.ar = rgAr.getValue();
    copyObj.aAdsr.dr = rgDr.getValue();
    copyObj.aAdsr.sl = rgSl.getValue();
    copyObj.aAdsr.sr = rgSr.getValue();
    copyObj.aAdsr.rr = rgRr.getValue();
    copyObj.aAdsr.tl = rgTl.getValue();
    copyObj.aAdsr.bypass = bypass.getToggleState();
    copyObj.aAdsr.kor = kor.getToggleState();
    copyObj.aAdsr.xof = xof.getToggleState();
    copyObj.aAdsr.ks = ks.getSelectedId();

    copyObj.mask.mask = mask.getToggleState();

    fix.copyParams(copyObj.fix);
    pitchEnv.copyParams(copyObj.pAdsr);
    ssgSwEnv.copyParams(copyObj.aSsgSw);
}

void GuiOpna::pasteParamsOpm(CopyOpnOpm& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpna::pasteOpParamsOpm(int p, CopyOpnOpmOp& copyObj) {
    withOp(p, [this, &copyObj] { pasteOpParamsOpmHere(copyObj); });
}

void GuiOpna::pasteOpParamsOpmHere(CopyOpnOpmOp& copyObj) {
    mul.setSelectedId(copyObj.detune.mul, juce::sendNotification);
    dt.setSelectedId(copyObj.detune.dt, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr.setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl.setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgSr.setValue(copyObj.aAdsr.sr, juce::sendNotification);
    rgRr.setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl.setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass.setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor.setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof.setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ks.setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);

    mask.setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix.pasteParams(copyObj.fix);
    pitchEnv.pasteParams(copyObj.pAdsr);
    ssgSwEnv.pasteParams(copyObj.aSsgSw);
}

void GuiOpna::copyParamsOpnOpm(CopyOpnaOpnOpm& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpna::copyOpParamsOpnOpm(int p, CopyOpnaOpnOpmOp& copyObj) {
    withOp(p, [this, &copyObj] { copyOpParamsOpnOpmHere(copyObj); });
}

void GuiOpna::copyOpParamsOpnOpmHere(CopyOpnaOpnOpmOp& copyObj) {
    copyObj.detune.mul = mul.getSelectedItemIndex();
    copyObj.detune.dt = dt.getSelectedItemIndex();
    copyObj.aAdsr.ar = rgAr.getValue();
    copyObj.aAdsr.dr = rgDr.getValue();
    copyObj.aAdsr.sl = rgSl.getValue();
    copyObj.aAdsr.sr = rgSr.getValue();
    copyObj.aAdsr.rr = rgRr.getValue();
    copyObj.aAdsr.tl = rgTl.getValue();
    copyObj.aAdsr.bypass = bypass.getToggleState();
    copyObj.aAdsr.kor = kor.getToggleState();
    copyObj.aAdsr.xof = xof.getToggleState();
    copyObj.aAdsr.ks = ks.getSelectedId();

    copyObj.mask.mask = mask.getToggleState();

    fix.copyParams(copyObj.fix);
    pitchEnv.copyParams(copyObj.pAdsr);
    ssgSwEnv.copyParams(copyObj.aSsgSw);
}

void GuiOpna::pasteParamsOpnOpm(CopyOpnaOpnOpm& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpna::pasteOpParamsOpnOpm(int p, CopyOpnaOpnOpmOp& copyObj) {
    withOp(p, [this, &copyObj] { pasteOpParamsOpnOpmHere(copyObj); });
}

void GuiOpna::pasteOpParamsOpnOpmHere(CopyOpnaOpnOpmOp& copyObj) {
    mul.setSelectedItemIndex(copyObj.detune.mul, juce::sendNotification);
    dt.setSelectedItemIndex(copyObj.detune.dt, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr.setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl.setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgSr.setValue(copyObj.aAdsr.sr, juce::sendNotification);
    rgRr.setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl.setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass.setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor.setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof.setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ks.setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);

    mask.setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix.pasteParams(copyObj.fix);
    pitchEnv.pasteParams(copyObj.pAdsr);
    ssgSwEnv.pasteParams(copyObj.aSsgSw);
}

void GuiOpna::importHwLfoParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultLfoParamDir,
        { EditorGuiText::ParamBrowser::kindHwLfo },
        [this, opIndex](const juce::File& file) { applyHwLfoParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpna::applyHwLfoParamFile(int opIndex, const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingHwLfoParams(opIndex, lines, index);
        }

        Io::ParamWriter writer(opnaHwLfoFormat);

        writeHwLfoParams(opIndex, writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opnaHwLfoFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    freqs.setSelectedItemIndex(reader->getInt("freq", freqs.getSelectedItemIndex()), juce::sendNotification);
    syncDelay.setValue(reader->getInt("syncDelay", (int)syncDelay.getValue()), juce::sendNotification);
    pm.setToggleState(reader->getBool("pm", pm.getToggleState()), juce::sendNotification);
    pms.setSelectedItemIndex(reader->getInt("pms", pms.getSelectedItemIndex()), juce::sendNotification);
    am.setToggleState(reader->getBool("am", am.getToggleState()), juce::sendNotification);
    ams.setSelectedItemIndex(reader->getInt("ams", ams.getSelectedItemIndex()), juce::sendNotification);
}

void GuiOpna::exportHwLfoParam(int opIndex)
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultLfoParamDir,
        { EditorGuiText::ParamBrowser::kindHwLfo }, Io::Extension::OpnaHwLfoParam,
        [this, opIndex](const juce::File& file) { writeHwLfoParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpna::writeHwLfoParamFile(int opIndex, const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opnaHwLfoFormat);
    writeHwLfoParams(opIndex, writer);

    writer.writeTo(file);
}

void GuiOpna::importPitchEnvParam(int opIndex) {
    pitchEnv.importParams();
}

void GuiOpna::exportPitchEnvParam(int opIndex) {
    pitchEnv.exportParams();
}

void GuiOpna::importSsgSwEnvParam(int opIndex) {
    ssgSwEnv.importParams();
}

void GuiOpna::exportSsgSwEnvParam(int opIndex) {
    ssgSwEnv.exportParams();
}

void GuiOpna::importLfoParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultLfoParamDir,
        { EditorGuiText::ParamBrowser::kindLfoN88 },
        [this](const juce::File& file) { applyLfoParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpna::applyLfoParamFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingLfoParams(lines, index);
        }

        Io::ParamWriter writer(n88LfoFormat);

        writeLfoParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, n88LfoFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    lfoFreqSlider.setValue(reader->getInt("lfoFreq", (int)lfoFreqSlider.getValue()), juce::sendNotification);
    lfoShapeSelector.setSelectedItemIndex(reader->getInt("lfoShape", lfoShapeSelector.getSelectedItemIndex()), juce::sendNotification);
    lfoSyncDelaySlider.setValue(reader->getInt("lfoSyncDelay", (int)lfoSyncDelaySlider.getValue()), juce::sendNotification);
    lfoPmToggle.setToggleState(reader->getBool("lfoPm", lfoPmToggle.getToggleState()), juce::sendNotification);
    lfoPmsSlider.setValue(reader->getInt("lfoPms", (int)lfoPmsSlider.getValue()), juce::sendNotification);
    lfoPmdSlider.setValue(reader->getInt("lfoPmd", (int)lfoPmdSlider.getValue()), juce::sendNotification);
    lfoAmToggle.setToggleState(reader->getBool("lfoAm", lfoAmToggle.getToggleState()), juce::sendNotification);
    lfoAmSmRtSlider.setValue(reader->getFloat("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue()), juce::sendNotification);
    lfoAmdSlider.setValue(reader->getInt("lfoAmd", (int)lfoAmdSlider.getValue()), juce::sendNotification);

    auto amsValues = reader->getIntArray("n88Ams");

    for (int i = 0; i < OpnaPrValue::ops && i < (int)amsValues.size(); i++) {
        n88Ams.setValue(amsValues[(size_t)i], juce::sendNotification);
    }
}

void GuiOpna::exportLfoParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultLfoParamDir,
        { EditorGuiText::ParamBrowser::kindLfoN88 }, Io::Extension::N88LfoParam,
        [this](const juce::File& file) { writeLfoParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpna::writeLfoParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(n88LfoFormat);
    writeLfoParams(writer);

    writer.writeTo(file);
}

void GuiOpna::importQualityParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindQuality },
        [this](const juce::File& file) { applyQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpna::applyQualityParamFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingQualityParams(lines, index);
        }

        Io::ParamWriter writer(qualityFormat);

        writeQualityParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, qualityFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    qualityComponent.setBit(reader->getInt("bit", qualityComponent.getBit()));
    qualityComponent.setRate(reader->getInt("rate", qualityComponent.getRate()));
}

void GuiOpna::exportQualityParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindQuality }, Io::Extension::QualityParam,
        [this](const juce::File& file) { writeQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpna::writeQualityParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(qualityFormat);
    writeQualityParams(writer);

    writer.writeTo(file);
}

void GuiOpna::importSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11.importParams();
}

void GuiOpna::exportSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11.exportParams();
}

void GuiOpna::importSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11.importParams();
}

void GuiOpna::exportSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11.exportParams();
}

void GuiOpna::importOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp.importParams();
}

void GuiOpna::exportOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp.exportParams();
}

void GuiOpna::importOpWtAmpModParam(int opIndex) {
    wtAmpModOp.importParams();
}

void GuiOpna::exportOpWtAmpModParam(int opIndex) {
    wtAmpModOp.exportParams();
}

void GuiOpna::importOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp.importParams();
}

void GuiOpna::exportOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp.exportParams();
}

void GuiOpna::importOpWtModParam(int opIndex) {
    wtModOp.importParams();
}

void GuiOpna::exportOpWtModParam(int opIndex) {
    wtModOp.exportParams();
}

void GuiOpna::importChParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "OPNA" },
        [this](const juce::File& file) { applyChParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiOpna::applyChParamFile(const juce::File& file) {
    if (!file.existsAsFile()) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingChParams(lines, index);
        }

        Io::ParamWriter writer(opnaFormat);

        writeChParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opnaFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    readChParams(*reader);
}

void GuiOpna::exportChParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { "OPNA" }, Io::Extension::opnaParam,
        [this](const juce::File& file) { writeChParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpna::writeChParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opnaFormat);
    writeChParams(writer);

    writer.writeTo(file);
}

void GuiOpna::importOpChParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpnaOp },
        [this, opIndex](const juce::File& file) { applyOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpna::applyOpChParamFile(int opIndex, const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingOpChFileParams(opIndex, lines, index);
        }

        Io::ParamWriter writer(opnaOpFormat);

        writeOpChFileParams(opIndex, writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opnaOpFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    updateAlgorithmDisplay();

    readOpParams(opIndex, *reader);
}

void GuiOpna::exportOpChParam(int opIndex)
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpnaOp }, Io::Extension::opnaOpParam,
        [this, opIndex](const juce::File& file) { writeOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpna::writeOpChParamFile(int opIndex, const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opnaOpFormat);
    writeOpChFileParams(opIndex, writer);

    writer.writeTo(file);
}


// チャンネル 1 つぶん。OPN のファイルを読むときにも使う。
void GuiOpna::readChParams(const Io::ParamReader& reader) {
    // Level
    levelComponent.readParams(reader, "level");

    // Algorithm & Feedback
    algSelector.setSelectedId(reader.getInt("alg", algSelector.getSelectedId()), juce::sendNotification);
    feedbackSlider.setValue(reader.getInt("feedback", (int)feedbackSlider.getValue()), juce::sendNotification);

    updateAlgorithmDisplay();

    // Pan
    panSlider.setValue(reader.getFloat("pan", (float)panSlider.getValue()), juce::sendNotification);

    // N88 LFO
    lfoFreqSlider.setValue(reader.getFloat("lfoFreq", (float)lfoFreqSlider.getValue()), juce::sendNotification);
    lfoShapeSelector.setSelectedId(reader.getInt("lfoShape", lfoShapeSelector.getSelectedId()), juce::sendNotification);
    lfoAmSmRtSlider.setValue(reader.getFloat("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue()), juce::sendNotification);
    lfoSyncDelaySlider.setValue(reader.getFloat("lfoSyncDelay", (float)lfoSyncDelaySlider.getValue()), juce::sendNotification);
    lfoPmToggle.setToggleState(reader.getBool("lfoPm", lfoPmToggle.getToggleState()), juce::sendNotification);
    lfoPmsSlider.setValue(reader.getFloat("lfoPms", (float)lfoPmsSlider.getValue()), juce::sendNotification);
    lfoPmdSlider.setValue(reader.getFloat("lfoPmd", (float)lfoPmdSlider.getValue()), juce::sendNotification);
    lfoAmToggle.setToggleState(reader.getBool("lfoAm", lfoAmToggle.getToggleState()), juce::sendNotification);
    lfoAmdSlider.setValue(reader.getFloat("lfoAmd", (float)lfoAmdSlider.getValue()), juce::sendNotification);

    // Components (Global)
    ssgHwEnv.readParams(reader, "ssgHwEnv");
    ssgHwPEnv.readParams(reader, "ssgHwPEnv");
    ssgSwEnv11g.readParams(reader, "ssgSwEnv11");
    qualityComponent.readParams(reader, "quality");
    unisonComponent.readParams(reader, "unison");
    ampEnvComponent.readParams(reader, "ampEnv");
    ssgSwPEnv11g.readParams(reader, "ssgSwPEnv11");
    modComponent.readParams(reader, "wtMod");
    ampModComponent.readParams(reader, "wtAmpMod");

    for (int i = 0; i < OpnaPrValue::ops; i++) {
        withOp(i, [this, &reader, i] { readOpParams(i, reader.arrayItem(Io::ParamKey::ops, i)); });
}
}

// オペレータ 1 つぶん。並びの中のひとつを渡してもらう。
void GuiOpna::readOpParams(int opIndex, const Io::ParamReader& r) {
    // Mul / Dt
    mul.setSelectedId(r.getInt("mul", mul.getSelectedId()), juce::sendNotification);
    dt.setSelectedId(r.getInt("dt", dt.getSelectedId()), juce::sendNotification);

    // Env
    rgAr.setValue(r.getFloat("ar", (float)rgAr.getValue()), juce::sendNotification);
    rgDr.setValue(r.getFloat("dr", (float)rgDr.getValue()), juce::sendNotification);
    rgSl.setValue(r.getFloat("sl", (float)rgSl.getValue()), juce::sendNotification);
    rgSr.setValue(r.getFloat("sr", (float)rgSr.getValue()), juce::sendNotification);
    rgRr.setValue(r.getFloat("rr", (float)rgRr.getValue()), juce::sendNotification);
    rgTl.setValue(r.getFloat("tl", (float)rgTl.getValue()), juce::sendNotification);

    // Key Scale
    ks.setSelectedId(r.getInt("ks", ks.getSelectedId()), juce::sendNotification);

    // HW LFO (単体のファイルと同じ名前にしてある)
    freqs.setSelectedId(r.getInt("freq", freqs.getSelectedId()), juce::sendNotification);
    syncDelay.setValue(r.getFloat("syncDelay", (float)syncDelay.getValue()), juce::sendNotification);
    pm.setToggleState(r.getBool("pm", pm.getToggleState()), juce::sendNotification);
    pms.setSelectedId(r.getInt("pms", pms.getSelectedId()), juce::sendNotification);
    am.setToggleState(r.getBool("am", am.getToggleState()), juce::sendNotification);
    ams.setSelectedId(r.getInt("ams", ams.getSelectedId()), juce::sendNotification);

    // N88 AMS
    n88Ams.setValue(r.getFloat("n88Ams", (float)n88Ams.getValue()), juce::sendNotification);

    // SSG Env
    se.setSelectedId(r.getInt("ssgEnv", se.getSelectedId()), juce::sendNotification);
    seFreq.setValue(r.getFloat("ssgEnvFreq", (float)seFreq.getValue()), juce::sendNotification);

    // Optional / Mask
    bypass.setToggleState(r.getBool("bypass", bypass.getToggleState()), juce::sendNotification);
    xof.setToggleState(r.getBool("xof", xof.getToggleState()), juce::sendNotification);
    kor.setToggleState(r.getBool("kor", kor.getToggleState()), juce::sendNotification);
    mask.setToggleState(r.getBool("mask", mask.getToggleState()), juce::sendNotification);

    // Components
    fix.readParams(r, "fix");
    pitchEnv.readParams(r, "pitchEnv");
    ssgSwEnv.readParams(r, "ssgSwEnv");
    ssgSwEnv11.readParams(r, "ssgSwEnv11");
    ssgSwPEnv11.readParams(r, "ssgSwPEnv11");
    ssgHwPEnvOp.readParams(r, "ssgHwPEnv");
    wtAmpModOp.readParams(r, "wtAmpMod");
    ssgHwEnvOp.readParams(r, "ssgHwEnv");
    wtModOp.readParams(r, "wtMod");
}

void GuiOpna::writeOpParams(int opIndex, Io::ParamWriter& w) {
    // Mul / Dt
    w.set("mul", mul.getSelectedId());
    w.set("dt", dt.getSelectedId());

    // Env
    w.set("ar", (float)rgAr.getValue());
    w.set("dr", (float)rgDr.getValue());
    w.set("sl", (float)rgSl.getValue());
    w.set("sr", (float)rgSr.getValue());
    w.set("rr", (float)rgRr.getValue());
    w.set("tl", (float)rgTl.getValue());

    // Key Scale
    w.set("ks", ks.getSelectedId());

    // HW LFO
    w.set("freq", freqs.getSelectedId());
    w.set("syncDelay", (float)syncDelay.getValue());
    w.set("pm", pm.getToggleState());
    w.set("pms", pms.getSelectedId());
    w.set("am", am.getToggleState());
    w.set("ams", ams.getSelectedId());

    // N88 AMS
    w.set("n88Ams", (float)n88Ams.getValue());

    // SSG Env
    w.set("ssgEnv", se.getSelectedId());
    w.set("ssgEnvFreq", (float)seFreq.getValue());

    // Optional / Mask
    w.set("bypass", bypass.getToggleState());
    w.set("xof", xof.getToggleState());
    w.set("kor", kor.getToggleState());
    w.set("mask", mask.getToggleState());

    // Components
    fix.writeParams(w, "fix");
    pitchEnv.writeParams(w, "pitchEnv");
    ssgSwEnv.writeParams(w, "ssgSwEnv");
    ssgSwEnv11.writeParams(w, "ssgSwEnv11");
    ssgSwPEnv11.writeParams(w, "ssgSwPEnv11");
    ssgHwPEnvOp.writeParams(w, "ssgHwPEnv");
    wtAmpModOp.writeParams(w, "wtAmpMod");
    ssgHwEnvOp.writeParams(w, "ssgHwEnv");
    wtModOp.writeParams(w, "wtMod");
}

void GuiOpna::importOpnChParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "OPN" },
        [this](const juce::File& file) { applyOpnChParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiOpna::applyOpnChParamFile(const juce::File& file) {
    if (!file.existsAsFile()) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    // OPN のファイル。こちらにしか無い項目は名前で引けないので
    // 今の値のまま残る。
    auto reader = Io::ParamReader::open(file, opnFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    readChParams(*reader);
}

void GuiOpna::importOpnOpChParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpnOp },
        [this, opIndex](const juce::File& file) { applyOpnOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpna::applyOpnOpChParamFile(int opIndex, const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    auto reader = Io::ParamReader::open(file, opnOpFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    updateAlgorithmDisplay();

    readOpParams(opIndex, *reader);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。並び順を写し直すより確実で、
// 当時の互換の工夫もそのまま残る。
void GuiOpna::setImportingChParams(juce::StringArray& lines, int& index) {
	// Level
	levelComponent.setImportingParams(lines, index);

	// Algorithm & Feedback
	algSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
	feedbackSlider.setValue(lines[index++].getIntValue(), juce::sendNotification);

	updateAlgorithmDisplay();

	// Pan
	panSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	// N88 LFO
	lfoFreqSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoShapeSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
	lfoAmSmRtSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoSyncDelaySlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoPmToggle.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	lfoPmsSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoPmdSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoAmToggle.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	lfoAmdSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	// Components (Global)
	ssgHwEnv.setImportingParams(lines, index);
	ssgSwEnv11g.setImportingParams(lines, index);
	qualityComponent.setImportingParams(lines, index);
	unisonComponent.setImportingParams(lines, index);

	for (int i = 0; i < OpnaPrValue::ops; i++) {
        withOp(i, [this, &lines, &index, i] { getImportingOpParams(i, lines, index); });
	}

	// AMP ENV は後から足したので、旧フォーマットとの互換のため
	// ファイル末尾から読む。行が無ければ既定のままにする。
	if (index < lines.size()) {
	    ampEnvComponent.setImportingParams(lines, index);
	}

	if (index < lines.size()) {
	    ssgSwPEnv11g.setImportingParams(lines, index);
	}

	if (index < lines.size()) {
	    modComponent.setImportingBaseParams(lines, index);
	    modComponent.setImportingShapeParam(lines, index);
	}

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpna::writeChParams(Io::ParamWriter& writer) {
	// Level
	levelComponent.writeParams(writer, "level");

	// Algorithm & Feedback
	writer.set("alg", algSelector.getSelectedId());
	writer.set("feedback", (int)feedbackSlider.getValue());

	// Pan
	writer.set("pan", (float)panSlider.getValue());

	// N88 LFO
	writer.set("lfoFreq", (float)lfoFreqSlider.getValue());
	writer.set("lfoShape", lfoShapeSelector.getSelectedId());
	writer.set("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue());
	writer.set("lfoSyncDelay", (float)lfoSyncDelaySlider.getValue());
	writer.set("lfoPm", lfoPmToggle.getToggleState());
	writer.set("lfoPms", (float)lfoPmsSlider.getValue());
	writer.set("lfoPmd", (float)lfoPmdSlider.getValue());
	writer.set("lfoAm", lfoAmToggle.getToggleState());
	writer.set("lfoAmd", (float)lfoAmdSlider.getValue());

	// Components (Global)
	// 名前で持つので、後から足した項目を末尾へ置く必要はない。
	ssgHwEnv.writeParams(writer, "ssgHwEnv");
	ssgHwPEnv.writeParams(writer, "ssgHwPEnv");
	ssgSwEnv11g.writeParams(writer, "ssgSwEnv11");
	qualityComponent.writeParams(writer, "quality");
	unisonComponent.writeParams(writer, "unison");
	ampEnvComponent.writeParams(writer, "ampEnv");
	ssgSwPEnv11g.writeParams(writer, "ssgSwPEnv11");
	modComponent.writeParams(writer, "wtMod");
	ampModComponent.writeParams(writer, "wtAmpMod");

	for (int i = 0; i < OpnaPrValue::ops; i++) {
	    auto op = writer.arrayItem(Io::ParamKey::ops, i);

        withOp(i, [this, &op, i] { writeOpParams(i, op); });
	}

	
}

// 3.0.0 より前の形式を読むための補助。履歴から戻したもの。
void GuiOpna::getImportingOpParams(int opIndex, juce::StringArray& lines, int& index) {
    // Mul / Dt
    mul.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    dt.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // Env
    rgAr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgDr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgSl.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgSr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgRr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgTl.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Key Scale
    ks.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // HW LFO
    freqs.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    syncDelay.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    pm.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    pms.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    am.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    ams.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // N88 AMS
    n88Ams.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // SSG Env
    se.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    seFreq.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Optional / Mask
    bypass.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    xof.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    kor.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    mask.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

    // Components
    fix.setImportingParams(lines, index);
    pitchEnv.setImportingParams(lines, index);
    ssgSwEnv.setImportingParams(lines, index);
    ssgSwEnv11.setImportingParams(lines, index);
    ssgSwPEnv11.setImportingParams(lines, index);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpna::setImportingLfoParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 13) return;

	lfoFreqSlider.setValue(lines[0].getIntValue(), juce::sendNotification);
	lfoShapeSelector.setSelectedItemIndex(lines[1].getIntValue(), juce::sendNotification);
	lfoSyncDelaySlider.setValue(lines[2].getIntValue(), juce::sendNotification);
	lfoPmToggle.setToggleState(lines[3].getIntValue() == 1, juce::sendNotification);
	lfoPmsSlider.setValue(lines[4].getIntValue(), juce::sendNotification);
	lfoPmdSlider.setValue(lines[5].getIntValue(), juce::sendNotification);
	lfoAmToggle.setToggleState(lines[6].getIntValue() == 1, juce::sendNotification);
	lfoAmSmRtSlider.setValue(lines[7].getFloatValue(), juce::sendNotification);
	lfoAmdSlider.setValue(lines[8].getIntValue(), juce::sendNotification);

	for (int i = 0; i < OpnaPrValue::ops; i++) {
	    n88Ams.setValue(lines[9 + i].getIntValue(), juce::sendNotification);
	}

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpna::writeLfoParams(Io::ParamWriter& writer) {
	writer.set("lfoFreq", (int)lfoFreqSlider.getValue());
	writer.set("lfoShape", lfoShapeSelector.getSelectedItemIndex());
	writer.set("lfoSyncDelay", (int)lfoSyncDelaySlider.getValue());
	writer.set("lfoPm", lfoPmToggle.getToggleState());
	writer.set("lfoPms", (int)lfoPmsSlider.getValue());
	writer.set("lfoPmd", (int)lfoPmdSlider.getValue());
	writer.set("lfoAm", lfoAmToggle.getToggleState());
	writer.set("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue());
	writer.set("lfoAmd", (int)lfoAmdSlider.getValue());

	std::vector<int> amsValues;

	for (int i = 0; i < OpnaPrValue::ops; i++) {
	    amsValues.push_back((int)n88Ams.getValue());
	}

	writer.setArray("n88Ams", amsValues);

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpna::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 2) return;

	qualityComponent.setBit(lines[0].getIntValue());
	qualityComponent.setRate(lines[1].getIntValue());

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpna::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("bit", qualityComponent.getBit());
	writer.set("rate", qualityComponent.getRate());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpna::setImportingHwLfoParams(int opIndex, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 6) return;

	freqs.setSelectedItemIndex(lines[0].getIntValue(), juce::sendNotification);
	syncDelay.setValue(lines[1].getIntValue(), juce::sendNotification);
	pm.setToggleState(lines[2].getIntValue() == 1, juce::sendNotification);
	pms.setSelectedItemIndex(lines[3].getIntValue(), juce::sendNotification);
	am.setToggleState(lines[4].getIntValue() == 1, juce::sendNotification);
	ams.setSelectedItemIndex(lines[5].getIntValue(), juce::sendNotification);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpna::writeHwLfoParams(int opIndex, Io::ParamWriter& writer) {
	writer.set("freq", freqs.getSelectedItemIndex());
	writer.set("syncDelay", (int)syncDelay.getValue());
	writer.set("pm", pm.getToggleState());
	writer.set("pms", pms.getSelectedItemIndex());
	writer.set("am", am.getToggleState());
	writer.set("ams", ams.getSelectedItemIndex());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpna::setImportingOpChFileParams(int opIndex, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	updateAlgorithmDisplay();

	getImportingOpParams(opIndex, lines, index);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpna::writeOpChFileParams(int opIndex, Io::ParamWriter& writer) {
	writeOpParams(opIndex, writer);

	
}

void GuiOpna::bypassHiddenCategories()
{
    // いま隠れている区分だけを切る。出したままの区分は触らない。
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv)) ampEnvComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)) ssgHwEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)) ssgSwEnv11g.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod)) ampModComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)) ssgHwPEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)) ssgSwPEnv11g.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod)) modComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::Unison)) unisonComponent.setCategoryBypassed(true);

    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)) ssgHwEnvOp.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv)) ssgSwEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)) ssgSwEnv11.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod)) wtAmpModOp.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv)) pitchEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)) ssgHwPEnvOp.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)) ssgSwPEnv11.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod)) wtModOp.setCategoryBypassed(true);
}

void GuiOpna::openEnabledCategories()
{
    // 効いている区分を開く。札を持たない区分は触らない。
    if (ampEnvComponent.hasBypassSwitch() && !ampEnvComponent.isCategoryBypassed()) ampEnvComponent.setCategoryOpen(true);
    if (ssgHwEnv.hasBypassSwitch() && !ssgHwEnv.isCategoryBypassed()) ssgHwEnv.setCategoryOpen(true);
    if (ssgSwEnv11g.hasBypassSwitch() && !ssgSwEnv11g.isCategoryBypassed()) ssgSwEnv11g.setCategoryOpen(true);
    if (ampModComponent.hasBypassSwitch() && !ampModComponent.isCategoryBypassed()) ampModComponent.setCategoryOpen(true);
    if (ssgHwPEnv.hasBypassSwitch() && !ssgHwPEnv.isCategoryBypassed()) ssgHwPEnv.setCategoryOpen(true);
    if (ssgSwPEnv11g.hasBypassSwitch() && !ssgSwPEnv11g.isCategoryBypassed()) ssgSwPEnv11g.setCategoryOpen(true);
    if (modComponent.hasBypassSwitch() && !modComponent.isCategoryBypassed()) modComponent.setCategoryOpen(true);
    if (unisonComponent.hasBypassSwitch() && !unisonComponent.isCategoryBypassed()) unisonComponent.setCategoryOpen(true);

    if (ssgHwEnvOp.hasBypassSwitch() && !ssgHwEnvOp.isCategoryBypassed()) ssgHwEnvOp.setCategoryOpen(true);
    if (ssgSwEnv.hasBypassSwitch() && !ssgSwEnv.isCategoryBypassed()) ssgSwEnv.setCategoryOpen(true);
    if (ssgSwEnv11.hasBypassSwitch() && !ssgSwEnv11.isCategoryBypassed()) ssgSwEnv11.setCategoryOpen(true);
    if (wtAmpModOp.hasBypassSwitch() && !wtAmpModOp.isCategoryBypassed()) wtAmpModOp.setCategoryOpen(true);
    if (pitchEnv.hasBypassSwitch() && !pitchEnv.isCategoryBypassed()) pitchEnv.setCategoryOpen(true);
    if (ssgHwPEnvOp.hasBypassSwitch() && !ssgHwPEnvOp.isCategoryBypassed()) ssgHwPEnvOp.setCategoryOpen(true);
    if (ssgSwPEnv11.hasBypassSwitch() && !ssgSwPEnv11.isCategoryBypassed()) ssgSwPEnv11.setCategoryOpen(true);
    if (wtModOp.hasBypassSwitch() && !wtModOp.isCategoryBypassed()) wtModOp.setCategoryOpen(true);

    // 大区分は中の区分に合わせる。効いている区分があれば開く。
    if (anyCategoryEnabled(ampEnvComponent, ssgHwEnv, ssgSwEnv11g, ampModComponent)) ampMajorCat.setDetailVisible(true);
    if (anyCategoryEnabled(ssgHwPEnv, ssgSwPEnv11g, modComponent)) pitchMajorCat.setDetailVisible(true);
}

void GuiOpna::closeBypassedCategories()
{
    // 切ってある区分を閉じる。札を持たない区分は触らない。
    if (ampEnvComponent.hasBypassSwitch() && ampEnvComponent.isCategoryBypassed()) ampEnvComponent.setCategoryOpen(false);
    if (ssgHwEnv.hasBypassSwitch() && ssgHwEnv.isCategoryBypassed()) ssgHwEnv.setCategoryOpen(false);
    if (ssgSwEnv11g.hasBypassSwitch() && ssgSwEnv11g.isCategoryBypassed()) ssgSwEnv11g.setCategoryOpen(false);
    if (ampModComponent.hasBypassSwitch() && ampModComponent.isCategoryBypassed()) ampModComponent.setCategoryOpen(false);
    if (ssgHwPEnv.hasBypassSwitch() && ssgHwPEnv.isCategoryBypassed()) ssgHwPEnv.setCategoryOpen(false);
    if (ssgSwPEnv11g.hasBypassSwitch() && ssgSwPEnv11g.isCategoryBypassed()) ssgSwPEnv11g.setCategoryOpen(false);
    if (modComponent.hasBypassSwitch() && modComponent.isCategoryBypassed()) modComponent.setCategoryOpen(false);
    if (unisonComponent.hasBypassSwitch() && unisonComponent.isCategoryBypassed()) unisonComponent.setCategoryOpen(false);

    if (ssgHwEnvOp.hasBypassSwitch() && ssgHwEnvOp.isCategoryBypassed()) ssgHwEnvOp.setCategoryOpen(false);
    if (ssgSwEnv.hasBypassSwitch() && ssgSwEnv.isCategoryBypassed()) ssgSwEnv.setCategoryOpen(false);
    if (ssgSwEnv11.hasBypassSwitch() && ssgSwEnv11.isCategoryBypassed()) ssgSwEnv11.setCategoryOpen(false);
    if (wtAmpModOp.hasBypassSwitch() && wtAmpModOp.isCategoryBypassed()) wtAmpModOp.setCategoryOpen(false);
    if (pitchEnv.hasBypassSwitch() && pitchEnv.isCategoryBypassed()) pitchEnv.setCategoryOpen(false);
    if (ssgHwPEnvOp.hasBypassSwitch() && ssgHwPEnvOp.isCategoryBypassed()) ssgHwPEnvOp.setCategoryOpen(false);
    if (ssgSwPEnv11.hasBypassSwitch() && ssgSwPEnv11.isCategoryBypassed()) ssgSwPEnv11.setCategoryOpen(false);
    if (wtModOp.hasBypassSwitch() && wtModOp.isCategoryBypassed()) wtModOp.setCategoryOpen(false);

    // 大区分は中の区分に合わせる。どれも切ってあれば閉じる。
    if (allCategoriesBypassed(ampEnvComponent, ssgHwEnv, ssgSwEnv11g, ampModComponent)) ampMajorCat.setDetailVisible(false);
    if (allCategoriesBypassed(ssgHwPEnv, ssgSwPEnv11g, modComponent)) pitchMajorCat.setDetailVisible(false);
}

// ----------------------------------------------------------------------------
// OPZX7S のパラメータファイルへ書き出す
// ----------------------------------------------------------------------------
// 書き出すのは CH Params と同じ中身。直し方は OPZX7S の [IM]FM Params と同じ
void GuiOpna::exportOpzx7Params()
{
    FmToOpzx7::exportFile(ctx, FmToOpzx7::Source::opna, [this](Io::ParamWriter& w) { writeChParams(w); });
}
