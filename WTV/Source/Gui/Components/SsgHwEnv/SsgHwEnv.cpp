#include "./SsgHwEnv.h"
#include "../../../Core/Editor/EditorGuiText.h"

#include "../../../Core/Editor/PluginEditor.h"

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
#include "./SsgHwEnvItems.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat ssgHwEnvFormat{ "ssgHwEnv", 1 };
}


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

    shapeSelector.setup({ .parent = parent, .id = code + CPK::SsgHwEnv::shape, .title = "SHPE", .items = SsgHwEnvItems::shapes(), .isReset = true});
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

void GuiComponentSsgHwEnv::importParams()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultSsgHwEnvParamDir,
        { EditorGuiText::ParamBrowser::kindSsgHwEnv },
        [this](const juce::File& file) { applyParamsFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiComponentSsgHwEnv::applyParamsFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultSsgHwEnvParamDir = file.getParentDirectory().getFullPathName();

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

            setImportingParams(lines, index);
        }

        // 単体のファイルは入れ子にせず、そのまま中身として書く
        Io::ParamWriter writer(ssgHwEnvFormat);

        writeParams(writer, Io::ParamKey::values);
        writer.hoist(Io::ParamKey::values);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, ssgHwEnvFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    // チャンネルファイルの中に入る形と同じ中身にしてある
    readParams(*reader, "ssgHwEnv");
}

void GuiComponentSsgHwEnv::exportParams()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultSsgHwEnvParamDir,
        { EditorGuiText::ParamBrowser::kindSsgHwEnv }, Io::Extension::SsgHwEnvParam,
        [this](const juce::File& file) { writeParamsFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiComponentSsgHwEnv::writeParamsFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultSsgHwEnvParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(ssgHwEnvFormat);

    writeParams(writer, "ssgHwEnv");

    writer.writeTo(file);
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
