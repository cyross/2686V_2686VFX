#include "./SsgHwEnv.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Processor/ProcessorValues.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

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

void GuiComponentSsgHwEnv::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder)
{
    cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- SSG HW ENV ---",
        .invisibleTitle = juce::String("") + "[□]--- SSG HW ENV ---",
        .enableChangeDetailVisible = true
        });

    envEnableButton.setup({ .parent = parent, .id = code + CPK::SsgHwEnv::enable, .title = "Enable", .isReset = true});
    envEnableButton.setWantsKeyboardFocus(true);
    envEnableButton.setExplicitFocusOrder(++tabOrder);

    hwEnvSeparator.setupComponent(parent);

    shapeSelector.setup({ .parent = parent, .id = code + CPK::SsgHwEnv::shape, .title = "SHPE", .items = ssgEnvItems, .isReset = true});
    shapeSelector.setWantsKeyboardFocus(true);
    shapeSelector.setExplicitFocusOrder(++tabOrder);

    periodSlider.setup({ .parent = parent, .id = code + CPK::SsgHwEnv::period, .title = "PERD", .isReset = true});
    periodSlider.setWantsKeyboardFocus(true);
    periodSlider.setExplicitFocusOrder(++tabOrder);

    minSlider.setup({ .parent = parent, .id = code + CPK::SsgHwEnv::min, .title = "MIN", .isReset = true });
    minSlider.setWantsKeyboardFocus(true);
    minSlider.setExplicitFocusOrder(++tabOrder);
    minSlider.onValueChange = [this] {
        float min = minSlider.getValue();
        float max = maxSlider.getValue();

        if (min > max) {
            maxSlider.setValue(min, juce::sendNotification);
        }
        };

    maxSlider.setup({ .parent = parent, .id = code + CPK::SsgHwEnv::max, .title = "MAX", .isReset = true });
    maxSlider.setWantsKeyboardFocus(true);
    maxSlider.setExplicitFocusOrder(++tabOrder);
    maxSlider.onValueChange = [this] {
        float min = minSlider.getValue();
        float max = maxSlider.getValue();

        if (min > max) {
            maxSlider.setValue(min, juce::sendNotification);
        }
        };
}

void GuiComponentSsgHwEnv::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    envEnableButton.setVisible(visible);
    hwEnvSeparator.setVisible(visible);
    shapeSelector.setVisibleWithLabel(visible);
    periodSlider.setVisibleWithLabel(visible);
    minSlider.setVisibleWithLabel(visible);
    maxSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &envEnableButton });
        hwEnvSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &shapeSelector.label, .component = &shapeSelector, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &periodSlider.label, .component = &periodSlider, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &minSlider.label, .component = &minSlider, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &maxSlider.label, .component = &maxSlider, .rowHeight = 13 });
    }
}

void GuiComponentSsgHwEnv::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    envEnableButton.setVisible(visible);
    hwEnvSeparator.setVisible(visible);
    shapeSelector.setVisibleWithLabel(visible);
    periodSlider.setVisibleWithLabel(visible);
    minSlider.setVisibleWithLabel(visible);
    maxSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &envEnableButton });
        hwEnvSeparator.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .label = &shapeSelector.label, .component = &shapeSelector, .rowHeight = 13 });
        layoutRow({ .rowRect = rect, .label = &periodSlider.label, .component = &periodSlider, .rowHeight = 13 });
        layoutRow({ .rowRect = rect, .label = &minSlider.label, .component = &minSlider, .rowHeight = 13 });
        layoutRow({ .rowRect = rect, .label = &maxSlider.label, .component = &maxSlider, .rowHeight = 13 });
    }
}

void GuiComponentSsgHwEnv::setEnabled(bool enabled) {
    cat.setEnabled(enabled);
    envEnableButton.setEnabled(enabled);
    hwEnvSeparator.setEnabled(enabled);
    shapeSelector.setEnabledWithLabel(enabled);
    periodSlider.setEnabledWithLabel(enabled);
    minSlider.setEnabledWithLabel(enabled);
    maxSlider.setEnabledWithLabel(enabled);
}

void GuiComponentSsgHwEnv::copyParams(CopyEnvSsgHw& copyObj) {
    copyObj.enable = envEnableButton.getToggleState();
    copyObj.shape = shapeSelector.getSelectedItemIndex();
    copyObj.period = periodSlider.getValue();
    copyObj.min = minSlider.getValue();
    copyObj.max = maxSlider.getValue();
}

void GuiComponentSsgHwEnv::pasteParams(CopyEnvSsgHw& copyObj) {
    envEnableButton.setToggleState(copyObj.enable, juce::sendNotification);
    shapeSelector.setSelectedItemIndex(copyObj.shape, juce::sendNotification);
    periodSlider.setValue(copyObj.period, juce::sendNotification);
    minSlider.setValue(copyObj.period, juce::sendNotification);
    maxSlider.setValue(copyObj.period, juce::sendNotification);
}

void GuiComponentSsgHwEnv::importParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgHwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importSsgHwEnvParamFile, defaultDir, Io::ExtensionGlob::SsgHwEnvParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgHwEnvParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();
                int index = 0;

                if (size < 6) return;

                setImportingParams(lines, index);
            }
        });
}

void GuiComponentSsgHwEnv::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgHwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportSsgHwEnvParamFile, defaultDir.getChildFile("default." + Io::Extension::SsgHwEnvParam), Io::ExtensionGlob::SsgHwEnvParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgHwEnvParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = getExportedParams();

                file.replaceWithText(content);
            }
        });
}

void GuiComponentSsgHwEnv::setImportingParams(juce::StringArray& lines, int& index) {
    envEnableButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    shapeSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    periodSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    minSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    maxSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
}

juce::String GuiComponentSsgHwEnv::getExportedParams() {
    juce::String content = "";

    content += juce::String(envEnableButton.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(shapeSelector.getSelectedItemIndex()) + "\n";
    content += juce::String(periodSlider.getValue()) + "\n";
    content += juce::String(minSlider.getValue()) + "\n";
    content += juce::String(maxSlider.getValue()) + "\n";

    return content;
}
