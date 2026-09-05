#include "./SsgHwPEnv.h"

#include "../../../Core/Gui/GuiRefresh.h"

#include "../../../Core/Io/ParamFile.h"

#include "../WavePreview/WavePreviewSource.h"

#include "../../../Effect/Envelope/Pitch/SsgHw/EnvSsgHw.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Processor/ProcessorValues.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"
#include "../SsgHwEnv/SsgHwEnvItems.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat ssgHwPEnvFormat{ "ssgHwPEnv", 1 };
}

void GuiComponentSsgHwPEnv::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder,
    juce::Colour categoryBg)
{
    cat.setupCategory({
        .parent = parent,
        .title = juce::String("") + "SSG HW PITCH ENV",
        .enableChangeDetailVisible = true
        }, categoryBg);

    envEnableButton.setup({ .parent = parent, .id = code + CPK::SsgHwPEnv::enable, .title = "Enable", .isReset = true});
    envEnableButton.setWantsKeyboardFocus(true);
    envEnableButton.setExplicitFocusOrder(++tabOrder);

    // 波形の折り返しで生じる段差は、ピッチだとそのまま音程の飛びになる。
    // 出力を鈍らせて和らげるスイッチ
    smoothEnableButton.setup({ .parent = parent, .id = code + CPK::SsgHwPEnv::smooth, .title = "Smooth", .isReset = true });
    smoothEnableButton.setWantsKeyboardFocus(true);
    smoothEnableButton.setExplicitFocusOrder(++tabOrder);

    hwEnvSeparator.setupComponent(parent);

    shapeSelector.setup({ .parent = parent, .id = code + CPK::SsgHwPEnv::shape, .title = "SHPE", .items = SsgHwEnvItems::shapes(), .isReset = true});
    shapeSelector.setWantsKeyboardFocus(true);
    shapeSelector.setExplicitFocusOrder(++tabOrder);

    periodSlider.setup({ .parent = parent, .id = code + CPK::SsgHwPEnv::period, .title = "PERD", .isReset = true});
    periodSlider.setWantsKeyboardFocus(true);
    periodSlider.setExplicitFocusOrder(++tabOrder);

    // MIN と MAX は追い越せない。追い越したときは「相手を押す」形にする。
    // 自分へ書き戻すとドラッグ中にマウスと値の押し合いになり、
    // つまみが動かせなくなる。
    //
    // setValue は同期で onValueChange を呼び返すので、押した相手から
    // 戻ってくる 1 回は弾く。
    minSlider.setupComponent(parent, code + CPK::SsgHwPEnv::min, "MIN", tabOrder, [this] {
        if (isClampingRange) return;

        const juce::ScopedValueSetter<bool> guard(isClampingRange, true);

        if (minSlider.getValue() > maxSlider.getValue()) {
            maxSlider.setValue(minSlider.getValue(), juce::sendNotification);
        }

        updatePreview();
        }, labelFont);

    minButtons.setupComponent(parent, minSlider.getSlider(), tabOrder, labelFont);

    maxSlider.setupComponent(parent, code + CPK::SsgHwPEnv::max, "MAX", tabOrder, [this] {
        if (isClampingRange) return;

        const juce::ScopedValueSetter<bool> guard(isClampingRange, true);

        if (maxSlider.getValue() < minSlider.getValue()) {
            minSlider.setValue(maxSlider.getValue(), juce::sendNotification);
        }

        updatePreview();
        }, labelFont);

    maxButtons.setupComponent(parent, maxSlider.getSlider(), tabOrder, labelFont);

    preview.setup(parent, GuiColor::WavePreview::PitchEnv);

    auto refreshPreview = [this] { updatePreview(); };

    shapeSelector.onChange = refreshPreview;
    smoothEnableButton.onStateChange = refreshPreview;

    updatePreview();
}

void GuiComponentSsgHwPEnv::layoutComponent(juce::Rectangle<int>& rect)
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
    minButtons.setVisibles(visible && minSlider.isVisibleNudge());
    maxSlider.setVisibleWithLabel(visible);
    maxButtons.setVisibles(visible && maxSlider.isVisibleNudge());

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &envEnableButton });
        hwEnvSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &shapeSelector.label, .component = &shapeSelector, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &periodSlider.label, .component = &periodSlider, .rowHeight = 13 });
        minSlider.layoutComponent(rect, 13);
        if (minSlider.isVisibleNudge()) minButtons.layoutComponent(rect, 13);
        maxSlider.layoutComponent(rect, 13);
        if (maxSlider.isVisibleNudge()) maxButtons.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .component = &smoothEnableButton });

        preview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentSsgHwPEnv::layoutComponentRow(juce::Rectangle<int>& rect)
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
    minButtons.setVisibles(visible && minSlider.isVisibleNudge());
    maxSlider.setVisibleWithLabel(visible);
    maxButtons.setVisibles(visible && maxSlider.isVisibleNudge());

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &envEnableButton });
        hwEnvSeparator.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .label = &shapeSelector.label, .component = &shapeSelector, .rowHeight = 13 });
        layoutRow({ .rowRect = rect, .label = &periodSlider.label, .component = &periodSlider, .rowHeight = 13 });
        minSlider.layoutComponentRow(rect, 12);
        if (minSlider.isVisibleNudge()) minButtons.layoutComponentRow(rect, 12);
        maxSlider.layoutComponentRow(rect, 12);
        if (maxSlider.isVisibleNudge()) maxButtons.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .component = &smoothEnableButton });

        preview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentSsgHwPEnv::setEnabled(bool enabled) {
    cat.setEnabled(enabled);
    envEnableButton.setEnabled(enabled);
    smoothEnableButton.setEnabled(enabled);
    hwEnvSeparator.setEnabled(enabled);
    shapeSelector.setEnabledWithLabel(enabled);
    periodSlider.setEnabledWithLabel(enabled);
    minSlider.setEnabledWithLabel(enabled);
    minButtons.setEnables(enabled);
    maxSlider.setEnabledWithLabel(enabled);
    maxButtons.setEnables(enabled);
}

void GuiComponentSsgHwPEnv::copyParams(CopyPEnvSsgHw& copyObj) {
    copyObj.enable = envEnableButton.getToggleState();
    copyObj.shape = shapeSelector.getSelectedItemIndex();
    copyObj.period = periodSlider.getValue();
    copyObj.min = (int)minSlider.getValue();
    copyObj.max = (int)maxSlider.getValue();
    copyObj.smooth = smoothEnableButton.getToggleState();
}

void GuiComponentSsgHwPEnv::pasteParams(CopyPEnvSsgHw& copyObj) {
    envEnableButton.setToggleState(copyObj.enable, juce::sendNotification);
    shapeSelector.setSelectedItemIndex(copyObj.shape, juce::sendNotification);
    periodSlider.setValue(copyObj.period, juce::sendNotification);
    minSlider.setValue((float)copyObj.min, juce::sendNotification);
    maxSlider.setValue((float)copyObj.max, juce::sendNotification);
    smoothEnableButton.setToggleState(copyObj.smooth, juce::sendNotification);
}

void GuiComponentSsgHwPEnv::importParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgHwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importSsgHwPEnvParamFile, defaultDir, Io::ExtensionGlob::SsgHwPEnvParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgHwEnvParamDir = file.getParentDirectory().getFullPathName();

                auto reader = Io::ParamReader::open(file, ssgHwPEnvFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す
                GuiRefresh::Batch batch;

                // チャンネルファイルの中に入る形と同じ中身にしてある
                readParams(*reader, "ssgHwPEnv");
            }
        });
}

void GuiComponentSsgHwPEnv::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgHwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportSsgHwPEnvParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::SsgHwPEnvParam)), Io::saveGlob(Io::Extension::SsgHwPEnvParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgHwEnvParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(ssgHwPEnvFormat);

                writeParams(writer, "ssgHwPEnv");

                writer.writeTo(file);
            }
        });
}

// 3.1.0 で足したものなので、3.0.0 より前の並び順のファイルには入っていない。
// 読み込みの位置がずれないよう、ここでは何も進めずに既定値のままにする。
void GuiComponentSsgHwPEnv::setImportingParams(juce::StringArray& lines, int& index) {
    juce::ignoreUnused(lines, index);
}

void GuiComponentSsgHwPEnv::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    envEnableButton.setToggleState(r.getBool("envEnable", envEnableButton.getToggleState()), juce::sendNotification);
    shapeSelector.setSelectedItemIndex(r.getInt("shape", shapeSelector.getSelectedItemIndex()), juce::sendNotification);
    periodSlider.setValue(r.getFloat("period", (float)periodSlider.getValue()), juce::sendNotification);
    minSlider.setValue((float)r.getInt("min", (int)minSlider.getValue()), juce::sendNotification);
    maxSlider.setValue((float)r.getInt("max", (int)maxSlider.getValue()), juce::sendNotification);
    smoothEnableButton.setToggleState(r.getBool("smoothEnable", smoothEnableButton.getToggleState()), juce::sendNotification);
}

juce::String GuiComponentSsgHwPEnv::getExportedParams() {
    juce::String content = "";

    content += juce::String(envEnableButton.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(shapeSelector.getSelectedItemIndex()) + "\n";
    content += juce::String(periodSlider.getValue()) + "\n";
    content += juce::String((int)minSlider.getValue()) + "\n";
    content += juce::String((int)maxSlider.getValue()) + "\n";
    content += juce::String(smoothEnableButton.getToggleState() ? 1 : 0) + "\n";

    return content;
}

void GuiComponentSsgHwPEnv::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    w.set("envEnable", envEnableButton.getToggleState());
    w.set("shape", shapeSelector.getSelectedItemIndex());
    w.set("period", (float)periodSlider.getValue());
    w.set("min", (int)minSlider.getValue());
    w.set("max", (int)maxSlider.getValue());
    w.set("smoothEnable", smoothEnableButton.getToggleState());
}

// 選んだ Shape を実際のエンベロープで走らせ、折れ線にして渡す。
// 描画のたびに計算すると重いので、値が変わったときだけここを通す。
void GuiComponentSsgHwPEnv::updatePreview()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updatePreview(); })) return;

    // セント値は上下に振れるので、0 を中央にして描く
    preview.setPoints(
        WavePreviewSource::ssgHwPEnv(
            shapeSelector.getSelectedItemIndex(),
            (int)minSlider.getValue(),
            (int)maxSlider.getValue(),
            smoothEnableButton.getToggleState()),
        true);
}
