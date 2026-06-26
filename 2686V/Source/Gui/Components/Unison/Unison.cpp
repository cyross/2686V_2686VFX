#include "./Unison.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

void GuiComponentUnison::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder)
{
    cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- UNISON/HARMONY ---",
        .invisibleTitle = juce::String("") + "[□]--- UNISON/HARMONY ---",
        .enableChangeDetailVisible = true
        });

    voices.setup({ .parent = parent, .id = code + "_UNI_VOICES", .title = "VICE", .isReset = true });
    voices.setWantsKeyboardFocus(true);
    voices.setExplicitFocusOrder(++tabOrder);

    detune.setup({ .parent = parent, .id = code + "_UNI_DETUNE", .title = "DT", .isReset = true });
    detune.setWantsKeyboardFocus(true);
    detune.setExplicitFocusOrder(++tabOrder);

    detuneButtons.setupComponent(parent, detune, tabOrder);

    spread.setup({ .parent = parent, .id = code + "_UNI_SPREAD", .title = "SPR", .isReset = true });
    spread.setWantsKeyboardFocus(true);
    spread.setExplicitFocusOrder(++tabOrder);
}

void GuiComponentUnison::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    voices.setVisibleWithLabel(visible);
    detune.setVisibleWithLabel(visible);
    detuneButtons.setVisibles(visible);
    spread.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &voices.label, .component = &voices });
        layoutMain({ .mainRect = rect, .label = &detune.label, .component = &detune });
        detuneButtons.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &spread.label, .component = &spread });
    }
}
void GuiComponentUnison::copyParams(CopyUnison& copyObj) {
    copyObj.voices = voices.getValue();
    copyObj.detune = detune.getValue();
    copyObj.spread = spread.getValue();
}

void GuiComponentUnison::pasteParams(CopyUnison& copyObj) {
    voices.setValue(copyObj.voices, juce::sendNotification);
    detune.setValue(copyObj.detune, juce::sendNotification);
    spread.setValue(copyObj.spread, juce::sendNotification);
}

void GuiComponentUnison::importParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultUnisonParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importUnisonParamFile, defaultDir, Io::ExtensionGlob::UnisonParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultUnisonParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 3) return;

                voices.setValue(lines[0].getIntValue(), juce::sendNotification);
                detune.setValue(lines[1].getIntValue(), juce::sendNotification);
                spread.setValue(lines[2].getFloatValue(), juce::sendNotification);
            }
        });

}

void GuiComponentUnison::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultUnisonParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportUnisonParamFile, defaultDir.getChildFile("default.unison"), Io::ExtensionGlob::UnisonParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultUnisonParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(voices.getValue()) + "\n";
                content += juce::String(detune.getValue()) + "\n";
                content += juce::String(spread.getValue(), Global::floatDecimalPlaces) + "\n";

                file.replaceWithText(content);
            }
        });
}
