#include "./SsgHwEnv.h"

#include "../../../Core/Gui/GuiRefresh.h"

#include "../../../Core/Io/ParamFile.h"

#include "../WavePreview/WavePreviewSource.h"

#include "../../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Processor/ProcessorValues.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat ssgHwEnvFormat{ "ssgHwEnv", 1 };
}

static std::vector<SelectItem> ssgEnvItems = {
    {.name = "0: Saw Down",                     .value =  1 },
    {.name = "1: Saw Down & Hold",              .value =  2 },
    {.name = "2: Triangle",                     .value =  3 },
    {.name = "3: Alternative Saw Down & Hold",  .value =  4 },
    {.name = "4: Saw Up",                       .value =  5 },
    {.name = "5: Saw Up & Hold",                .value =  6 },
    {.name = "6: Triangle Invert",              .value =  7 },
    {.name = "7: Alternative Saw Up & Hold",    .value =  8 },
    {.name = "8: Square 75%",                   .value =  9 },
    {.name = "9: Square 50%",                   .value = 10 },
    {.name = "10: Square 25%",                  .value = 11 },
    {.name = "11: Square 12.5%",                .value = 12 },
    {.name = "12: Square -75%",                 .value = 13 },
    {.name = "13: Square -50%",                 .value = 14 },
    {.name = "14: Square -25%",                 .value = 15 },
    {.name = "15: Square -12.5%",               .value = 16 },
    {.name = "16: Step Up 4",                   .value = 17 },
    {.name = "17: Step Up 5",                   .value = 18 },
    {.name = "18: Step Up 6",                   .value = 19 },
    {.name = "19: Step Up 11",                  .value = 20 },
    {.name = "20: Step Down 4",                 .value = 21 },
    {.name = "21: Step Down 5",                 .value = 22 },
    {.name = "22: Step Down 6",                 .value = 23 },
    {.name = "23: Step Down 11",                .value = 24 },
    {.name = "24: Step Peak 4",                 .value = 25 },
    {.name = "25: Step Peak 5",                 .value = 26 },
    {.name = "26: Step Peak 6",                 .value = 27 },
    {.name = "27: Step Peak 11",                .value = 28 },
    {.name = "28: Step Valley 4",               .value = 29 },
    {.name = "29: Step Valley 5",               .value = 30 },
    {.name = "30: Step Valley 6",               .value = 31 },
    {.name = "31: Step Valley 11",              .value = 32 },
    {.name = "32: Sample & Hold",               .value = 33 },
    {.name = "33: Sample & Hold 4",             .value = 34 },
    {.name = "34: Sample & Hold 8",             .value = 35 },
    {.name = "35: Sample & Hold 16",            .value = 36 },
    {.name = "36: Sample & Hold 32",            .value = 37 },
    {.name = "37: Sample & Hold 64",            .value = 38 },
    {.name = "38: Exp Decay",                   .value = 39 },
    {.name = "39: Exp Attack",                  .value = 40 },
    {.name = "40: Sine",                        .value = 41 },
    {.name = "41: Attack & Decay",              .value = 42 },
    {.name = "42: Double Pulse",                .value = 43 },
    {.name = "43: Random",                      .value = 44 },
};

void GuiComponentSsgHwEnv::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder,
    juce::Colour categoryBg)
{
    cat.setupCategory({
        .parent = parent,
        .title = juce::String("") + "SSG HW AMP ENV",
        .enableChangeDetailVisible = true
        }, categoryBg);

    envEnableButton.setup({ .parent = parent, .id = code + CPK::SsgHwEnv::enable, .title = "Enable", .isReset = true});
    envEnableButton.setWantsKeyboardFocus(true);
    envEnableButton.setExplicitFocusOrder(++tabOrder);

    // Period を大きくすると波形の折り返しの段差でブツブツ音が出るため、
    // 出力を鈍らせて和らげるスイッチ
    smoothEnableButton.setup({ .parent = parent, .id = code + CPK::SsgHwEnv::smooth, .title = "Smooth", .isReset = true });
    smoothEnableButton.setWantsKeyboardFocus(true);
    smoothEnableButton.setExplicitFocusOrder(++tabOrder);

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
    // MIN と MAX は追い越せない。追い越したときは「相手を押す」形にする。
    // 自分へ書き戻すとドラッグ中にマウスと値の押し合いになり、
    // つまみが動かせなくなる。
    //
    // setValue は同期で onValueChange を呼び返すので、押した相手から
    // 戻ってくる 1 回は弾く。
    minSlider.onValueChange = [this] {
        if (isClampingRange) return;

        const juce::ScopedValueSetter<bool> guard(isClampingRange, true);

        if (minSlider.getValue() > maxSlider.getValue()) {
            maxSlider.setValue(minSlider.getValue(), juce::sendNotification);
        }

        updatePreview();
        };

    maxSlider.setup({ .parent = parent, .id = code + CPK::SsgHwEnv::max, .title = "MAX", .isReset = true });
    maxSlider.setWantsKeyboardFocus(true);
    maxSlider.setExplicitFocusOrder(++tabOrder);
    preview.setup(parent, GuiColor::WavePreview::AmpEnv);

    auto refreshPreview = [this] { updatePreview(); };

    shapeSelector.onChange = refreshPreview;
    smoothEnableButton.onStateChange = refreshPreview;

    maxSlider.onValueChange = [this] {
        if (isClampingRange) return;

        const juce::ScopedValueSetter<bool> guard(isClampingRange, true);

        if (maxSlider.getValue() < minSlider.getValue()) {
            minSlider.setValue(maxSlider.getValue(), juce::sendNotification);
        }

        updatePreview();
        };

    updatePreview();
}

void GuiComponentSsgHwEnv::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    envEnableButton.setVisible(visible);
    smoothEnableButton.setVisible(visible);
    preview.setVisible(visible);
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
        layoutMain({ .mainRect = rect, .component = &smoothEnableButton });

        preview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentSsgHwEnv::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    envEnableButton.setVisible(visible);
    smoothEnableButton.setVisible(visible);
    preview.setVisible(visible);
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
        layoutRow({ .rowRect = rect, .component = &smoothEnableButton });

        preview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentSsgHwEnv::setEnabled(bool enabled) {
    cat.setEnabled(enabled);
    envEnableButton.setEnabled(enabled);
    smoothEnableButton.setEnabled(enabled);
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
    copyObj.smooth = smoothEnableButton.getToggleState();
}

void GuiComponentSsgHwEnv::pasteParams(CopyEnvSsgHw& copyObj) {
    envEnableButton.setToggleState(copyObj.enable, juce::sendNotification);
    shapeSelector.setSelectedItemIndex(copyObj.shape, juce::sendNotification);
    periodSlider.setValue(copyObj.period, juce::sendNotification);
    minSlider.setValue(copyObj.min, juce::sendNotification);
    maxSlider.setValue(copyObj.max, juce::sendNotification);
    smoothEnableButton.setToggleState(copyObj.smooth, juce::sendNotification);
}

void GuiComponentSsgHwEnv::importParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgHwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importSsgHwEnvParamFile, defaultDir, Io::ExtensionGlob::SsgHwEnvParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgHwEnvParamDir = file.getParentDirectory().getFullPathName();

                auto reader = Io::ParamReader::open(file, ssgHwEnvFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す
                GuiRefresh::Batch batch;

                // チャンネルファイルの中に入る形と同じ中身にしてある
                readParams(*reader, "ssgHwEnv");
            }
        });
}

void GuiComponentSsgHwEnv::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgHwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportSsgHwEnvParamFile, defaultDir.getChildFile("default." + Io::Extension::SsgHwEnvParam), Io::ExtensionGlob::SsgHwEnvParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgHwEnvParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(ssgHwEnvFormat);

                writeParams(writer, "ssgHwEnv");

                writer.writeTo(file);
            }
        });
}

void GuiComponentSsgHwEnv::setImportingParams(juce::StringArray& lines, int& index) {
    envEnableButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    shapeSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    periodSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    minSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    maxSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    smoothEnableButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
}

void GuiComponentSsgHwEnv::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    envEnableButton.setToggleState(r.getBool("envEnable", envEnableButton.getToggleState()), juce::sendNotification);
    shapeSelector.setSelectedItemIndex(r.getInt("shape", shapeSelector.getSelectedItemIndex()), juce::sendNotification);
    periodSlider.setValue(r.getFloat("period", (float)periodSlider.getValue()), juce::sendNotification);
    minSlider.setValue(r.getFloat("min", (float)minSlider.getValue()), juce::sendNotification);
    maxSlider.setValue(r.getFloat("max", (float)maxSlider.getValue()), juce::sendNotification);
    smoothEnableButton.setToggleState(r.getBool("smoothEnable", smoothEnableButton.getToggleState()), juce::sendNotification);
}

juce::String GuiComponentSsgHwEnv::getExportedParams() {
    juce::String content = "";

    content += juce::String(envEnableButton.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(shapeSelector.getSelectedItemIndex()) + "\n";
    content += juce::String(periodSlider.getValue()) + "\n";
    content += juce::String(minSlider.getValue()) + "\n";
    content += juce::String(maxSlider.getValue()) + "\n";
    content += juce::String(smoothEnableButton.getToggleState() ? 1 : 0) + "\n";

    return content;
}

void GuiComponentSsgHwEnv::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    w.set("envEnable", envEnableButton.getToggleState());
    w.set("shape", shapeSelector.getSelectedItemIndex());
    w.set("period", (float)periodSlider.getValue());
    w.set("min", (float)minSlider.getValue());
    w.set("max", (float)maxSlider.getValue());
    w.set("smoothEnable", smoothEnableButton.getToggleState());
}

// 選んだ Shape を実際のエンベロープで走らせ、折れ線にして渡す。
// 描画のたびに計算すると重いので、値が変わったときだけここを通す。
void GuiComponentSsgHwEnv::updatePreview()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updatePreview(); })) return;

    // エンベロープは 0〜1 の片側なので、下端を 0 として描く
    preview.setPoints(
        WavePreviewSource::ssgHwEnv(
            shapeSelector.getSelectedItemIndex(),
            (float)minSlider.getValue(),
            (float)maxSlider.getValue(),
            smoothEnableButton.getToggleState()),
        false);
}
