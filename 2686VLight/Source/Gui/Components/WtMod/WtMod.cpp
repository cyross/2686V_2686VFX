#include "./WtMod.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Editor/PluginEditor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Const/ConstFileValues.h"

static std::vector<SelectItem> wtModShapeItems = {
    {.name = "0: Sine",            .value = 1 },
    {.name = "1: FDS Triangle",    .value = 2 },
    {.name = "2: FDS Saw",         .value = 3 },
    {.name = "3: FDS Reset",       .value = 4 },
    {.name = "4: FDS Pulse",       .value = 5 },
    {.name = "5: WS Sweep Up",     .value = 6 },
    {.name = "6: WS Sweep Down",   .value = 7 },
    {.name = "7: HuC6280 Wave",    .value = 8 },
};

void GuiComponentWtMod::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, juce::String& wavePath)
{
    p_wavePath = &wavePath;

    cat.setupHwCategory({ .parent = parent, .title = juce::String("") + "[■]--- MODULATION ---", .invisibleTitle = juce::String("") + "[□]--- MODULATION ---", .enableChangeDetailVisible = true });

    enableButton.setup({ .parent = parent, .id = code + CPK::WtMod::enable, .title = "Enable", .isReset = true, .isResized = true });
    enableButton.setWantsKeyboardFocus(true);
    enableButton.setExplicitFocusOrder(++tabOrder);

    depthSlider.setup({ .parent = parent, .id = code + CPK::WtMod::depth, .title = "DPTH", .isReset = true });
    depthSlider.setWantsKeyboardFocus(true);
    depthSlider.setExplicitFocusOrder(++tabOrder);

    speedSlider.setup({ .parent = parent, .id = code + CPK::WtMod::speed, .title = "SPED", .isReset = true });
    speedSlider.setWantsKeyboardFocus(true);
    speedSlider.setExplicitFocusOrder(++tabOrder);

    shapeSelector.setup({ .parent = parent, .id = code + CPK::WtMod::shape, .title = "SHPE", .items = wtModShapeItems, .isReset = true, .isResized = true });
    shapeSelector.setWantsKeyboardFocus(true);
    shapeSelector.setExplicitFocusOrder(++tabOrder);

    waveWtBtn.setup({ .parent = parent, .title = "WT", .bgColor = juce::Colours::darkgrey.brighter(0.2f), .isReset = false, .isResized = true });
    waveWtBtn.setWantsKeyboardFocus(true);
    waveWtBtn.setExplicitFocusOrder(++tabOrder);
    waveWtBtn.onClick = [this] { importWave(false); };

    waveWt2Btn.setup({ .parent = parent, .title = "W2", .bgColor = juce::Colours::darkgrey.brighter(0.2f), .isReset = false, .isResized = true });
    waveWt2Btn.setWantsKeyboardFocus(true);
    waveWt2Btn.setExplicitFocusOrder(++tabOrder);
    waveWt2Btn.onClick = [this] { importWave(true); };

    waveClearBtn.setup({ .parent = parent, .title = "Clear", .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = true });
    waveClearBtn.setWantsKeyboardFocus(true);
    waveClearBtn.setExplicitFocusOrder(++tabOrder);
    waveClearBtn.onClick = [this] { clearWave(); };

    waveFileNameLabel.setup({ .parent = parent, .title = Io::empty });
    if (p_wavePath->isNotEmpty()) {
        updateWaveFileName(juce::File(*p_wavePath).getFileName());
    }

    waveSmoothBtn.setup({ .parent = parent, .id = code + CPK::WtMod::waveSmooth, .title = "Smooth", .isReset = true, .isResized = true });
    waveSmoothBtn.setWantsKeyboardFocus(true);
    waveSmoothBtn.setExplicitFocusOrder(++tabOrder);

    // HuC6280 モードの変調波形パラメータを引けるようにしておく
    for (int i = 0; i < 32; ++i) {
        waveParams[i] = ctx.audioProcessor.apvts.getParameter(code + CPK::WtMod::wave + juce::String(i));
    }
}

void GuiComponentWtMod::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &cat });

    bool visible = cat.isDetailVisible();

    enableButton.setVisible(visible);
    depthSlider.setVisibleWithLabel(visible);
    speedSlider.setVisibleWithLabel(visible);
    shapeSelector.setVisibleWithLabel(visible);
    waveWtBtn.setVisible(visible);
    waveWt2Btn.setVisible(visible);
    waveClearBtn.setVisible(visible);
    waveFileNameLabel.setVisible(visible);
    waveSmoothBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &enableButton });
        layoutMain({ .mainRect = rect, .label = &depthSlider.label, .component = &depthSlider });
        layoutMain({ .mainRect = rect, .label = &speedSlider.label, .component = &speedSlider, });
        layoutMain({ .mainRect = rect, .label = &shapeSelector.label, .component = &shapeSelector, });
        layoutMainWtFiles({ .rect = rect, .loadWtBtn = &waveWtBtn, .loadWt2Btn = &waveWt2Btn, .fileNameLabel = &waveFileNameLabel, .clearBtn = &waveClearBtn });
        layoutMain({ .mainRect = rect, .component = &waveSmoothBtn });
    }

    // Enable が OFF のときは中身を触れなくする
    bool isMod = enableButton.getToggleState();
    depthSlider.setEnabledWithLabel(isMod);
    speedSlider.setEnabledWithLabel(isMod);
    shapeSelector.setEnabledWithLabel(isMod);
    waveWtBtn.setEnabled(isMod);
    waveWt2Btn.setEnabled(isMod);
    waveClearBtn.setEnabled(isMod);
    waveFileNameLabel.setEnabled(isMod);
    waveSmoothBtn.setEnabled(isMod);
}

void GuiComponentWtMod::importWave(bool isWt2)
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    ctx.editor.openFileChooser(
        isWt2 ? "Load Mod Wave (.wt2)" : "Load Mod Wave (.wt)",
        defaultDir,
        isWt2 ? "*.wt2" : "*.wt",
        [this, isWt2](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (!file.existsAsFile()) return;

            updateWaveFileName("Loading...");

            juce::Timer::callAfterDelay(50, [this, isWt2, file]()
                {
                    if (!applyWaveFile(file, isWt2)) {
                        updateWaveFileName(Io::empty);
                        return;
                    }

                    updateWaveFileName(file.getFileName());
                    ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();
                    (*p_wavePath) = file.getFullPathName();
                });
        }
    );
}

bool GuiComponentWtMod::applyWaveFile(const juce::File& file, bool isWt2)
{
    juce::StringArray lines;
    file.readLines(lines);

    if (lines.size() == 0) return false;

    int sampleCount = lines[0].trim().getIntValue();

    if (sampleCount != 32 && sampleCount != 64 && sampleCount != 128 && sampleCount != 256) {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
            "Invalid WT File", "Sample count must be 32, 64, 128, or 256.");
        return false;
    }

    std::vector<float> values(sampleCount, 0.0f);

    if (isWt2) {
        int resNumber = (lines.size() > 1) ? lines[1].trim().getIntValue() : 0;

        if (resNumber != 16 && resNumber != 32 && resNumber != 64 && resNumber != 128 && resNumber != 256) {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                "Invalid WT File", "Resolution must be 16, 32, 64, 128, or 256.");
            return false;
        }

        // .wt2 は 3 行目以降が 0〜(解像度-1) の整数。中央を 0 として -1.0〜1.0 に正規化する。
        float center = (float)(resNumber >> 1);

        for (int i = 0; i < sampleCount; ++i) {
            if (i + 2 < lines.size()) {
                int raw = std::clamp(lines[i + 2].getIntValue(), 0, resNumber - 1);
                values[i] = std::clamp(((float)raw - center) / center, -1.0f, 1.0f);
            }
        }
    }
    else {
        // .wt は 2 行目以降が -1.0〜1.0 の実数
        for (int i = 0; i < sampleCount; ++i) {
            if (i + 1 < lines.size()) {
                values[i] = std::clamp(lines[i + 1].getFloatValue(), -1.0f, 1.0f);
            }
        }
    }

    // 実機の波形メモリは 32 サンプルなので、そこへ落とす。
    //   Smooth ON  : 区間平均してから元のピークへ正規化する。
    //                32 点のナイキスト(16 次)より上の成分が低い次数へ
    //                フルの振幅で折り返すのを抑えつつ、変調の振れ幅は保つ。
    //   Smooth OFF : 単純間引き。元波形の値をそのまま拾う。
    const int step = sampleCount / 32;
    std::array<float, 32> reduced = { 0.0f };

    if (waveSmoothBtn.getToggleState() && step > 1) {
        float srcPeak = 0.0f;
        for (int i = 0; i < sampleCount; ++i) {
            srcPeak = std::max(srcPeak, std::fabs(values[i]));
        }

        float dstPeak = 0.0f;
        for (int i = 0; i < 32; ++i) {
            float sum = 0.0f;
            for (int k = 0; k < step; ++k) sum += values[i * step + k];

            reduced[i] = sum / (float)step;
            dstPeak = std::max(dstPeak, std::fabs(reduced[i]));
        }

        // 平均でなまったぶんのピークを戻す (無音の波形はそのまま)
        if (srcPeak > 1.0e-6f && dstPeak > 1.0e-6f) {
            float gain = srcPeak / dstPeak;

            for (int i = 0; i < 32; ++i) {
                reduced[i] = std::clamp(reduced[i] * gain, -1.0f, 1.0f);
            }
        }
    }
    else {
        for (int i = 0; i < 32; ++i) reduced[i] = values[i * step];
    }

    for (int i = 0; i < 32; ++i) {
        if (waveParams[i] != nullptr) {
            waveParams[i]->setValueNotifyingHost(waveParams[i]->convertTo0to1(reduced[i]));
        }
    }

    return true;
}

void GuiComponentWtMod::clearWave()
{
    for (int i = 0; i < 32; ++i) {
        if (waveParams[i] != nullptr) {
            waveParams[i]->setValueNotifyingHost(waveParams[i]->convertTo0to1(0.0f));
        }
    }

    (*p_wavePath) = juce::String();

    updateWaveFileName(Io::empty);
}

void GuiComponentWtMod::updateWaveFileName(const juce::String& fileName)
{
    waveFileNameLabel.setText(fileName, juce::dontSendNotification);
}
void GuiComponentWtMod::setImportingBaseParams(juce::StringArray& lines, int& index)
{
    enableButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    depthSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    speedSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
}

juce::String GuiComponentWtMod::getExportedBaseParams()
{
    juce::String content = "";

    content += juce::String(enableButton.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(depthSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(speedSlider.getValue(), Global::floatDecimalPlaces) + "\n";

    return content;
}

// Modulation Shape は後から追加したパラメータなので、旧フォーマットのファイルと
// 互換を保つためファイル末尾に置いてある。行が無ければ既定(正弦波)のままにする。
void GuiComponentWtMod::setImportingShapeParam(juce::StringArray& lines, int& index)
{
    if (index < lines.size()) {
        shapeSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    }
}

juce::String GuiComponentWtMod::getExportedShapeParam()
{
    return juce::String(shapeSelector.getSelectedItemIndex()) + "\n";
}
