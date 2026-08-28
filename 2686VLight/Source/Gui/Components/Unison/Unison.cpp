#include "./Unison.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
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

    voices.setup({ .parent = parent, .id = code + CPK::Unison::voices, .title = "VOIC", .isReset = true });
    voices.setWantsKeyboardFocus(true);
    voices.setExplicitFocusOrder(++tabOrder);

    detune.setupComponent(parent, code + CPK::Unison::detune, "DT", tabOrder, std::nullopt);

    detuneButtons.setupComponent(parent, detune.getSlider(), tabOrder);

    spread.setup({ .parent = parent, .id = code + CPK::Unison::spread, .title = "SPR", .isReset = true });
    spread.setWantsKeyboardFocus(true);
    spread.setExplicitFocusOrder(++tabOrder);

    // 疑似高速アルペジオ
    arpSeparator.setupComponent(parent);

    arpEnable.setup({ .parent = parent, .id = code + CPK::Unison::arpEnable, .title = "Arpeggio", .isReset = true });
    arpEnable.setWantsKeyboardFocus(true);
    arpEnable.setExplicitFocusOrder(++tabOrder);

    arpFreq.setup({ .parent = parent, .id = code + CPK::Unison::arpFreq, .title = "ARFQ", .isReset = true });
    arpFreq.setWantsKeyboardFocus(true);
    arpFreq.setExplicitFocusOrder(++tabOrder);

    arpSmooth.setup({ .parent = parent, .id = code + CPK::Unison::arpSmooth, .title = "Arp Smooth", .isReset = true });
    arpSmooth.setWantsKeyboardFocus(true);
    arpSmooth.setExplicitFocusOrder(++tabOrder);

    // ボイス単位の設定
    // ボイス0はメイン(素の音程・定位)なので対象外。1〜7 を切り替えて編集する
    paraSeparator.setupComponent(parent);

    paramCode = code;

    targetVoice.setup({ .parent = parent, .title = "VOIC", .isReset = false });
    targetVoice.setRange(1.0, (double)Global::unisonParaVoices, 1.0);
    targetVoice.setNumDecimalPlacesToDisplay(0);
    targetVoice.setWantsKeyboardFocus(true);
    targetVoice.setExplicitFocusOrder(++tabOrder);

    paraDistance.setup({ .parent = parent, .title = "P-SP", .isReset = false });
    paraDistance.setWantsKeyboardFocus(true);
    paraDistance.setExplicitFocusOrder(++tabOrder);

    paraDetune.setupComponent(parent, "", "P-DT", tabOrder, std::nullopt, std::nullopt, false);

    // DT と同じ補正ボタン群。スライダー参照経由で値を動かすので、
    // 対象ボイスを切り替えても常に現在の束縛先へ反映される。
    paraDetuneButtons.setupComponent(parent, paraDetune.getSlider(), tabOrder);

    targetVoice.onValueChange = [this] { rebindParaSliders(); };
    targetVoice.setValue(1, juce::sendNotification);

    // onValueChange は値が変わらないと呼ばれないため、初期束縛はここで明示的に行う
    rebindParaSliders();
}

void GuiComponentUnison::rebindParaSliders()
{
    const juce::String no = juce::String((int)targetVoice.getValue());

    paraDistance.rebind(paramCode + CPK::Unison::paraDistance + no);
    paraDetune.getSlider().rebind(paramCode + CPK::Unison::paraDetune + no);
}

// ボイス単位の設定はスライダーに1組しか束縛されていないため、
// 保存・読込・コピーでは APVTS から全ボイス分を直接読み書きする。
float GuiComponentUnison::getParaValue(const juce::String& key, int voiceIndex)
{
    auto* v = ctx.apvts.getRawParameterValue(paramCode + key + juce::String(voiceIndex + 1));
    return (v != nullptr) ? v->load() : 0.0f;
}

void GuiComponentUnison::setParaValue(const juce::String& key, int voiceIndex, float value)
{
    if (auto* p = ctx.apvts.getParameter(paramCode + key + juce::String(voiceIndex + 1)))
    {
        p->setValueNotifyingHost(p->convertTo0to1(value));
    }
}

void GuiComponentUnison::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    voices.setVisibleWithLabel(visible);
    detune.setVisibleWithLabel(visible);
    detuneButtons.setVisibles(visible && detune.isVisibleNudge());
    spread.setVisibleWithLabel(visible);
    arpSeparator.setVisible(visible);
    arpEnable.setVisible(visible);
    arpFreq.setVisibleWithLabel(visible);
    arpSmooth.setVisible(visible);
    paraSeparator.setVisible(visible);
    targetVoice.setVisibleWithLabel(visible);
    paraDistance.setVisibleWithLabel(visible);
    paraDetune.setVisibleWithLabel(visible);
    paraDetuneButtons.setVisibles(visible && paraDetune.isVisibleNudge());

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &voices.label, .component = &voices });
        layoutMain({ .mainRect = rect, .label = &spread.label, .component = &spread });
        detune.layoutComponent(rect);
        if (detune.isVisibleNudge()) detuneButtons.layoutComponent(rect);
        arpSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &arpEnable });
        layoutMain({ .mainRect = rect, .label = &arpFreq.label, .component = &arpFreq });
        layoutMain({ .mainRect = rect, .component = &arpSmooth });
        paraSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &targetVoice.label, .component = &targetVoice });
        layoutMain({ .mainRect = rect, .label = &paraDistance.label, .component = &paraDistance });
        paraDetune.layoutComponent(rect);
        if (paraDetune.isVisibleNudge()) paraDetuneButtons.layoutComponent(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}
void GuiComponentUnison::copyParams(CopyUnison& copyObj) {
    copyObj.voices = voices.getValue();
    copyObj.detune = detune.getValue();
    copyObj.spread = spread.getValue();
    copyObj.arpEnable = arpEnable.getToggleState();
    copyObj.arpFreq = arpFreq.getValue();
    copyObj.arpSmooth = arpSmooth.getToggleState();

    for (int i = 0; i < Global::unisonParaVoices; ++i) {
        copyObj.paraDistance[i] = getParaValue(CPK::Unison::paraDistance, i);
        copyObj.paraDetune[i] = (int)getParaValue(CPK::Unison::paraDetune, i);
    }
}

void GuiComponentUnison::pasteParams(CopyUnison& copyObj) {
    voices.setValue(copyObj.voices, juce::sendNotification);
    detune.setValue(copyObj.detune, juce::sendNotification);
    spread.setValue(copyObj.spread, juce::sendNotification);
    arpEnable.setToggleState(copyObj.arpEnable, juce::sendNotification);
    arpFreq.setValue(copyObj.arpFreq, juce::sendNotification);
    arpSmooth.setToggleState(copyObj.arpSmooth, juce::sendNotification);

    for (int i = 0; i < Global::unisonParaVoices; ++i) {
        setParaValue(CPK::Unison::paraDistance, i, copyObj.paraDistance[i]);
        setParaValue(CPK::Unison::paraDetune, i, (float)copyObj.paraDetune[i]);
    }
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

                // アルペジオは後から追加したため、旧フォーマットのファイルも読めるようにする
                if (size < 6) return;

                arpEnable.setToggleState(lines[3].getIntValue() != 0, juce::sendNotification);
                arpFreq.setValue(lines[4].getIntValue(), juce::sendNotification);
                arpSmooth.setToggleState(lines[5].getIntValue() != 0, juce::sendNotification);

                // ボイス単位の設定も後から追加したため、無ければ既定値のままにする
                if (size < 6 + Global::unisonParaVoices * 2) return;

                for (int i = 0; i < Global::unisonParaVoices; ++i) {
                    setParaValue(CPK::Unison::paraDistance, i, lines[6 + i * 2].getFloatValue());
                    setParaValue(CPK::Unison::paraDetune, i, (float)lines[7 + i * 2].getIntValue());
                }
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
                content += juce::String(arpEnable.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(arpFreq.getValue()) + "\n";
                content += juce::String(arpSmooth.getToggleState() ? 1 : 0) + "\n";

                for (int i = 0; i < Global::unisonParaVoices; ++i) {
                    content += juce::String(getParaValue(CPK::Unison::paraDistance, i), Global::floatDecimalPlaces) + "\n";
                    content += juce::String((int)getParaValue(CPK::Unison::paraDetune, i)) + "\n";
                }

                file.replaceWithText(content);
            }
        });
}

void GuiComponentUnison::setImportingParams(juce::StringArray& lines, int& index) {
    voices.setValue(lines[index++].getIntValue(), juce::sendNotification);
    detune.setValue(lines[index++].getIntValue(), juce::sendNotification);
    spread.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // アルペジオは後から追加した項目で、チャンネルパラメータの末尾に置いている。
    // 旧フォーマットのファイルにはこの3行が無いため、足りなければ既定値のままにする。
    if (index + 2 >= lines.size()) return;

    arpEnable.setToggleState(lines[index++].getIntValue() != 0, juce::sendNotification);
    arpFreq.setValue(lines[index++].getIntValue(), juce::sendNotification);
    arpSmooth.setToggleState(lines[index++].getIntValue() != 0, juce::sendNotification);

    // ボイス単位の設定も後から追加したため、無ければ既定値のままにする
    if (index + Global::unisonParaVoices * 2 - 1 >= lines.size()) return;

    for (int i = 0; i < Global::unisonParaVoices; ++i) {
        setParaValue(CPK::Unison::paraDistance, i, lines[index++].getFloatValue());
        setParaValue(CPK::Unison::paraDetune, i, (float)lines[index++].getIntValue());
    }
}

juce::String GuiComponentUnison::getExportedParams() {
    juce::String content = "";

    content += juce::String(voices.getValue()) + "\n";
    content += juce::String(detune.getValue()) + "\n";
    content += juce::String(spread.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(arpEnable.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(arpFreq.getValue()) + "\n";
    content += juce::String(arpSmooth.getToggleState() ? 1 : 0) + "\n";

    for (int i = 0; i < Global::unisonParaVoices; ++i) {
        content += juce::String(getParaValue(CPK::Unison::paraDistance, i), Global::floatDecimalPlaces) + "\n";
        content += juce::String((int)getParaValue(CPK::Unison::paraDetune, i)) + "\n";
    }

    return content;
}
