#include "./MulDetune.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

static std::vector<SelectItem> multems = {
    {.name = " 0: x  0.5",    .value = 1 },
    {.name = " 1: x  0.891",  .value = 2 },
    {.name = " 2: x  1",      .value = 3 },
    {.name = " 3: x  1.414",  .value = 4 },
    {.name = " 4: x  1.498",  .value = 5 },
    {.name = " 5: x  1.581",  .value = 6 },
    {.name = " 6: x  1.781",  .value = 7 },
    {.name = " 7: x  2",      .value = 8 },
    {.name = " 8: x  3",      .value = 9 },
    {.name = " 9: x  4",      .value = 10 },
    {.name = "10: x  5",      .value = 11 },
    {.name = "11: x  6",      .value = 12 },
    {.name = "12: x  7",      .value = 13 },
    {.name = "13: x  8",      .value = 14 },
    {.name = "14: x  9",      .value = 15 },
    {.name = "15: x 10",      .value = 16 },
    {.name = "16: x 11",      .value = 17 },
    {.name = "17: x 12",      .value = 18 },
    {.name = "18: x 13",      .value = 19 },
    {.name = "19: x 14",      .value = 20 },
    {.name = "20: x 15",      .value = 21 },
    {.name = "21: Use Ratio", .value = 22 }
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

void GuiComponentMulDetune::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder, bool isHw)
{
    if (isHw) {
        cat.setupHwCategory({
            .parent = parent,
            .title = juce::String("") + "[■]--- DETUNE ---",
            .invisibleTitle = juce::String("") + "[□]--- DETUNE ---",
            .enableChangeDetailVisible = true
            });
    }
    else {
        cat.setupSwCategory({
            .parent = parent,
            .title = juce::String("") + "[■]--- DETUNE ---",
            .invisibleTitle = juce::String("") + "[□]--- DETUNE ---",
            .enableChangeDetailVisible = true
            });
    }

    mul.setup({ .parent = parent, .id = code + "_MUL", .title = "Mul", .items = multems, .isReset = true });
    mul.setWantsKeyboardFocus(true);
    mul.setExplicitFocusOrder(++tabOrder);

    mulRatio.setup({ .parent = parent, .id = code + "_MUL_RATIO", .title = "M.Ratio", .isReset = true });
    mulRatio.setWantsKeyboardFocus(true);
    mulRatio.setExplicitFocusOrder(++tabOrder);

    dt1.setup({ .parent = parent, .id = code + "_DT", .title = "Dt1", .items = dtItems, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup({ .parent = parent, .id = code + "_DT2", .title = "Dt2", .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

    dt3.setup({ .parent = parent, .id = code + "_DT3", .title = "Dt3", .isReset = true });
    dt3.setWantsKeyboardFocus(true);
    dt3.setExplicitFocusOrder(++tabOrder);

    dt3Buttons.setupComponent(parent, dt3, tabOrder);
}

void GuiComponentMulDetune::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    mul.setVisibleWithLabel(visible);
    mulRatio.setVisibleWithLabel(visible);
	dt1.setVisibleWithLabel(visible);
	dt2.setVisibleWithLabel(visible);
    dt3.setVisibleWithLabel(visible);
    dt3Buttons.setVisibles(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &mul.label, .component = &mul });
        layoutMain({ .mainRect = rect, .label = &mulRatio.label, .component = &mulRatio });
        layoutMain({ .mainRect = rect, .label = &dt1.label, .component = &dt1 });
        layoutMain({ .mainRect = rect, .label = &dt2.label, .component = &dt2 });
        layoutMain({ .mainRect = rect, .label = &dt3.label, .component = &dt3 });
        dt3Buttons.layoutComponent(rect);
    }
}

void GuiComponentMulDetune::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    mul.setVisibleWithLabel(visible);
    mulRatio.setVisibleWithLabel(visible);
    dt1.setVisibleWithLabel(visible);
    dt2.setVisibleWithLabel(visible);
    dt3.setVisibleWithLabel(visible);
    dt3Buttons.setVisibles(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &mul.label, .component = &mul });
        layoutRow({ .rowRect = rect, .label = &mulRatio.label, .component = &mulRatio });
        layoutRow({ .rowRect = rect, .label = &dt1.label, .component = &dt1 });
        layoutRow({ .rowRect = rect, .label = &dt2.label, .component = &dt2 });
        layoutRow({ .rowRect = rect, .label = &dt3.label, .component = &dt3 });
        dt3Buttons.layoutComponentRow(rect);
    }
}

void GuiComponentMulDetune::copyParams(CopyDetuneOpzx7& copyObj) {
    copyObj.mul = mul.getSelectedId();
    copyObj.mulRatio = mulRatio.getValue();
    copyObj.dt = dt1.getSelectedId();
    copyObj.dt2 = dt2.getValue();
    copyObj.dt3 = dt2.getValue();
}

void GuiComponentMulDetune::pasteParams(CopyDetuneOpzx7& copyObj) {
    mul.setSelectedId(copyObj.mul, juce::sendNotification);
    mulRatio.setValue(copyObj.mulRatio, juce::sendNotification);
    dt1.setSelectedId(copyObj.dt, juce::sendNotification);
    dt2.setValue(copyObj.dt2, juce::sendNotification);
    dt3.setValue(copyObj.dt3, juce::sendNotification);
}

void GuiComponentMulDetune::importParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultDetuneParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importDetuneParamFile, defaultDir, Io::ExtensionGlob::DetuneParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultDetuneParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                if (lines.size() == 0) return;
            }
        });
}

void GuiComponentMulDetune::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultDetuneParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportDetuneParamFile, defaultDir.getChildFile("default.detune"), Io::ExtensionGlob::DetuneParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultDetuneParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "\n";

                file.replaceWithText(content);
            }
        });
}

void GuiComponentMulDetune::setMul(int m) {
    mul.setSelectedItemIndex(m, juce::sendNotification);
}

void GuiComponentMulDetune::setMulRatio(float r) {
    mulRatio.setValue(r, juce::sendNotification);
}

void GuiComponentMulDetune::setDt1(int d1) {
    dt1.setSelectedItemIndex(d1, juce::sendNotification);
}

void GuiComponentMulDetune::setDt2(int d2) {
    dt2.setValue(d2, juce::sendNotification);
}

void GuiComponentMulDetune::setDt3(int d3) {
    dt3.setValue(d3, juce::sendNotification);
}

int GuiComponentMulDetune::getMul() {
    return mul.getSelectedItemIndex();
}

float GuiComponentMulDetune::getMulRatio() {
    return mulRatio.getValue();
}

int GuiComponentMulDetune::getDt1() {
    return dt1.getSelectedItemIndex();
}

int GuiComponentMulDetune::getDt2() {
    return (int)dt2.getValue();
}

int GuiComponentMulDetune::getDt3() {
    return (int)dt3.getValue();
}

void GuiComponentMulDetune::setVisibles(bool visible){
    mul.setVisible(visible);
    mulRatio.setVisibleWithLabel(visible);
    dt1.setVisibleWithLabel(visible);
    dt2.setVisibleWithLabel(visible);
    dt3.setVisibleWithLabel(visible);
    dt3Buttons.setVisibles(visible);
}

void GuiComponentMulDetune::setEnables(bool enable) {
    mul.setEnabledWithLabel(enable);

    int mulIndex = mul.getSelectedId() - 1;
    bool enableMulRatio = mulIndex == 21; // mul = Ratio

    mulRatio.setEnabledWithLabel(enable && enableMulRatio);

    dt1.setEnabledWithLabel(enable);
    dt2.setEnabledWithLabel(enable);
    dt3.setEnabledWithLabel(enable);
    dt3Buttons.setEnables(enable);
}
