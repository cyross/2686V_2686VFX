#include "./MulDetune.h"

#include "../../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat detuneFormat{ "detune", 1 };
}


#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
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
            .title = juce::String("") + "MUL/DET",
            .enableChangeDetailVisible = true
            });
    }
    else {
        cat.setupSwCategory({
            .parent = parent,
            .title = juce::String("") + "MUL/DET",
            .enableChangeDetailVisible = true
            });
    }

    mul.setup({ .parent = parent, .id = code + CPK::mul, .title = "MUL", .items = multems, .isReset = true });
    mul.setWantsKeyboardFocus(true);
    mul.setExplicitFocusOrder(++tabOrder);

    mulRatio.setupComponent(parent, code + CPK::mulRatio, "MURT", tabOrder, std::nullopt);

    mulRatioTo001.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.01", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    mulRatioTo001.setWantsKeyboardFocus(true);
    mulRatioTo001.setExplicitFocusOrder(++tabOrder);
    mulRatioTo001.onClick = [this]() {
        mulRatio.setValue(0.01f, juce::sendNotification);
        };

    mulRatioTo005.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.05", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    mulRatioTo005.setWantsKeyboardFocus(true);
    mulRatioTo005.setExplicitFocusOrder(++tabOrder);
    mulRatioTo005.onClick = [this]() {
        mulRatio.setValue(0.05f, juce::sendNotification);
        };

    mulRatioTo1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "1", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    mulRatioTo1.setWantsKeyboardFocus(true);
    mulRatioTo1.setExplicitFocusOrder(++tabOrder);
    mulRatioTo1.onClick = [this]() {
        mulRatio.setValue(1.0f, juce::sendNotification);
        };

    mulRatioTo10.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "10", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    mulRatioTo10.setWantsKeyboardFocus(true);
    mulRatioTo10.setExplicitFocusOrder(++tabOrder);
    mulRatioTo10.onClick = [this]() {
        mulRatio.setValue(10.0f, juce::sendNotification);
        };

    mulRatioTo2757.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "27.57", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    mulRatioTo2757.setWantsKeyboardFocus(true);
    mulRatioTo2757.setExplicitFocusOrder(++tabOrder);
    mulRatioTo2757.onClick = [this]() {
        mulRatio.setValue(27.57f, juce::sendNotification);
        };

    mulRatioTo02.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.2", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    mulRatioTo02.setWantsKeyboardFocus(true);
    mulRatioTo02.setExplicitFocusOrder(++tabOrder);
    mulRatioTo02.onClick = [this]() {
        mulRatio.setValue(0.2f, juce::sendNotification);
        };

    mulRatioTo025.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.25", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    mulRatioTo025.setWantsKeyboardFocus(true);
    mulRatioTo025.setExplicitFocusOrder(++tabOrder);
    mulRatioTo025.onClick = [this]() {
        mulRatio.setValue(0.25f, juce::sendNotification);
        };

    mulRatioTo04.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.4", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    mulRatioTo04.setWantsKeyboardFocus(true);
    mulRatioTo04.setExplicitFocusOrder(++tabOrder);
    mulRatioTo04.onClick = [this]() {
        mulRatio.setValue(0.4f, juce::sendNotification);
        };

    mulRatioTo05.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.5", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    mulRatioTo05.setWantsKeyboardFocus(true);
    mulRatioTo05.setExplicitFocusOrder(++tabOrder);
    mulRatioTo05.onClick = [this]() {
        mulRatio.setValue(0.5f, juce::sendNotification);
        };

    mulRatioTo06.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.6", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    mulRatioTo06.setWantsKeyboardFocus(true);
    mulRatioTo06.setExplicitFocusOrder(++tabOrder);
    mulRatioTo06.onClick = [this]() {
        mulRatio.setValue(0.6f, juce::sendNotification);
        };

    mulRatioTo075.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.75", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    mulRatioTo075.setWantsKeyboardFocus(true);
    mulRatioTo075.setExplicitFocusOrder(++tabOrder);
    mulRatioTo075.onClick = [this]() {
        mulRatio.setValue(0.75f, juce::sendNotification);
        };

    mulRatioTo08.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.8", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    mulRatioTo08.setWantsKeyboardFocus(true);
    mulRatioTo08.setExplicitFocusOrder(++tabOrder);
    mulRatioTo08.onClick = [this]() {
        mulRatio.setValue(0.8f, juce::sendNotification);
        };

    mulRatioPM10.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-10", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioPM10.setWantsKeyboardFocus(true);
    mulRatioPM10.setExplicitFocusOrder(++tabOrder);
    mulRatioPM10.onClick = [this]() {
        mulRatio.setValue(mulRatio.getValue() - 10.0f, juce::sendNotification);
        };

    mulRatioP10.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+10", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioP10.setWantsKeyboardFocus(true);
    mulRatioP10.setExplicitFocusOrder(++tabOrder);
    mulRatioP10.onClick = [this]() {
        mulRatio.setValue(mulRatio.getValue() + 10.0f, juce::sendNotification);
        };

    mulRatioPM1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioPM1.setWantsKeyboardFocus(true);
    mulRatioPM1.setExplicitFocusOrder(++tabOrder);
    mulRatioPM1.onClick = [this]() {
        mulRatio.setValue(mulRatio.getValue() - 1.0f, juce::sendNotification);
        };

    mulRatioP1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioP1.setWantsKeyboardFocus(true);
    mulRatioP1.setExplicitFocusOrder(++tabOrder);
    mulRatioP1.onClick = [this]() {
        mulRatio.setValue(mulRatio.getValue() + 1.0f, juce::sendNotification);
        };

    mulRatioPM01.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-0.1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioPM01.setWantsKeyboardFocus(true);
    mulRatioPM01.setExplicitFocusOrder(++tabOrder);
    mulRatioPM01.onClick = [this]() {
        mulRatio.setValue(mulRatio.getValue() - 0.1f, juce::sendNotification);
        };

    mulRatioP01.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+0.1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioP01.setWantsKeyboardFocus(true);
    mulRatioP01.setExplicitFocusOrder(++tabOrder);
    mulRatioP01.onClick = [this]() {
        mulRatio.setValue(mulRatio.getValue() + 0.1f, juce::sendNotification);
        };

    mulRatioPM001.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-0.01", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioPM001.setWantsKeyboardFocus(true);
    mulRatioPM001.setExplicitFocusOrder(++tabOrder);
    mulRatioPM001.onClick = [this]() {
        mulRatio.setValue(mulRatio.getValue() - 0.01f, juce::sendNotification);
        };

    mulRatioP001.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+0.01", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioP001.setWantsKeyboardFocus(true);
    mulRatioP001.setExplicitFocusOrder(++tabOrder);
    mulRatioP001.onClick = [this]() {
        mulRatio.setValue(mulRatio.getValue() + 0.01f, juce::sendNotification);
        };

	mulDetSep.setupComponent(parent);

    dt1.setup({ .parent = parent, .id = code + CPK::dt, .title = "DT1", .items = dtItems, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup({ .parent = parent, .id = code + CPK::dt2, .title = "DT2", .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

    dt3.setupComponent(parent, code + CPK::dt3, "DT3", tabOrder, std::nullopt);

    dt3Buttons.setupComponent(parent, dt3.getSlider(), tabOrder);
}

void GuiComponentMulDetune::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    mul.setVisibleWithLabel(visible);
    mulRatio.setVisibleWithLabel(visible);
    mulRatioTo001.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo005.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo1.setVisible(visible && mulRatio.isVisibleNudge());
	mulRatioTo10.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo2757.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo02.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo025.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo04.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo05.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo06.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo075.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo08.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM10.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP10.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM1.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP1.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM01.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP01.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM001.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP001.setVisible(visible && mulRatio.isVisibleNudge());
    mulDetSep.setVisible(visible);
	dt1.setVisibleWithLabel(visible);
	dt2.setVisibleWithLabel(visible);
    dt3.setVisibleWithLabel(visible);
    dt3Buttons.setVisibles(visible && dt3.isVisibleNudge());

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &mul.label, .component = &mul });
        mulRatio.layoutComponent(rect);
        if (mulRatio.isVisibleNudge())
        {
            layoutMainFiveComps({ .rect = rect, .comp1 = &mulRatioTo001, .comp2 = &mulRatioTo005, .comp3 = &mulRatioTo1, .comp4 = &mulRatioTo10, .comp5 = &mulRatioTo2757 });
            layoutMainThreeComps({ .rect = rect, .comp1 = &mulRatioTo025, .comp2 = &mulRatioTo05, .comp3 = &mulRatioTo075 });
            layoutMainFourComps({ .rect = rect, .comp1 = &mulRatioTo02, .comp2 = &mulRatioTo04, .comp3 = &mulRatioTo06, .comp4 = &mulRatioTo08 });
            layoutMainFourComps({ .rect = rect, .comp1 = &mulRatioPM10, .comp2 = &mulRatioPM1, .comp3 = &mulRatioP1, .comp4 = &mulRatioP10 });
            layoutMainFourComps({ .rect = rect, .comp1 = &mulRatioPM001, .comp2 = &mulRatioPM01, .comp3 = &mulRatioP01, .comp4 = &mulRatioP001 });
        }
        mulDetSep.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &dt1.label, .component = &dt1 });
        layoutMain({ .mainRect = rect, .label = &dt2.label, .component = &dt2 });
        dt3.layoutComponent(rect);
        if (dt3.isVisibleNudge()) dt3Buttons.layoutComponent(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentMulDetune::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    mul.setVisibleWithLabel(visible);
    mulRatio.setVisibleWithLabel(visible);
    mulRatioTo001.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo005.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo1.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo10.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo2757.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo02.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo025.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo04.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo05.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo06.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo075.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo08.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM10.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP10.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM1.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP1.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM01.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP01.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM001.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP001.setVisible(visible && mulRatio.isVisibleNudge());
    mulDetSep.setVisible(visible);
    dt1.setVisibleWithLabel(visible);
    dt2.setVisibleWithLabel(visible);
    dt3.setVisibleWithLabel(visible);
    dt3Buttons.setVisibles(visible && dt3.isVisibleNudge());

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &mul.label, .component = &mul });
        mulRatio.layoutComponentRow(rect);
        if (mulRatio.isVisibleNudge())
        {
            layoutRowFiveComps({ .rect = rect, .comp1 = &mulRatioTo001, .comp2 = &mulRatioTo005, .comp3 = &mulRatioTo1, .comp4 = &mulRatioTo10, .comp5 = &mulRatioTo2757 });
            layoutRowThreeComps({ .rect = rect, .comp1 = &mulRatioTo025, .comp2 = &mulRatioTo05, .comp3 = &mulRatioTo075 });
            layoutRowFourComps({ .rect = rect, .comp1 = &mulRatioTo02, .comp2 = &mulRatioTo04, .comp3 = &mulRatioTo06, .comp4 = &mulRatioTo08 });
            layoutRowFourComps({ .rect = rect, .comp1 = &mulRatioPM10, .comp2 = &mulRatioPM1, .comp3 = &mulRatioP1, .comp4 = &mulRatioP10 });
            layoutRowFourComps({ .rect = rect, .comp1 = &mulRatioPM001, .comp2 = &mulRatioPM01, .comp3 = &mulRatioP01, .comp4 = &mulRatioP001 });
        }
        mulDetSep.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .label = &dt1.label, .component = &dt1 });
        layoutRow({ .rowRect = rect, .label = &dt2.label, .component = &dt2 });
        dt3.layoutComponentRow(rect);
        if (dt3.isVisibleNudge()) dt3Buttons.layoutComponentRow(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
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

                auto reader = Io::ParamReader::open(file, detuneFormat);

                if (!reader.has_value()) return;

                // 古いファイルは項目が欠けていることがあるので、
                // 読めなかったものは今の値のままにしておく。
                mul.setSelectedItemIndex(reader->getInt("mul", mul.getSelectedItemIndex()), juce::sendNotification);
                mulRatio.setValue(reader->getFloat("mulRatio", (float)mulRatio.getValue()), juce::sendNotification);
                dt1.setSelectedItemIndex(reader->getInt("dt1", dt1.getSelectedItemIndex()), juce::sendNotification);
                dt2.setValue(reader->getInt("dt2", (int)dt2.getValue()), juce::sendNotification);
                dt3.setValue(reader->getInt("dt3", (int)dt3.getValue()), juce::sendNotification);
            }
        });
}

void GuiComponentMulDetune::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultDetuneParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportDetuneParamFile, defaultDir.getChildFile("default." + Io::Extension::DetuneParamNew), Io::ExtensionGlob::DetuneParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultDetuneParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(detuneFormat);

                writer.set("mul", mul.getSelectedItemIndex());
                writer.set("mulRatio", (float)mulRatio.getValue());
                writer.set("dt1", dt1.getSelectedItemIndex());
                writer.set("dt2", (int)dt2.getValue());
                writer.set("dt3", (int)dt3.getValue());

                writer.writeTo(file);
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
    dt3Buttons.setVisibles(visible && dt3.isVisibleNudge());
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

void GuiComponentMulDetune::setImportingParams(juce::StringArray& lines, int& index) {
    mul.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    mulRatio.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    dt1.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    dt2.setValue(lines[index++].getIntValue(), juce::sendNotification);
    dt3.setValue(lines[index++].getIntValue(), juce::sendNotification);
}

juce::String GuiComponentMulDetune::getExportedParams() {
    juce::String content = "";

    content += juce::String(mul.getSelectedItemIndex()) + "\n";
    content += juce::String(mulRatio.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(dt1.getSelectedItemIndex()) + "\n";
    content += juce::String(dt2.getValue()) + "\n";
    content += juce::String(dt3.getValue()) + "\n";

    return content;
}
