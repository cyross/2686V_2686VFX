#include "Shared/Core/Gui/GuiI18n.h"
#include "./QualityPcm.h"
#include "Shared/Core/Editor/EditorParamBrowserText.h"

#include "../../../Core/Io/ParamFile.h"

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Shared/Core/Processor/ProcessorValues.h"
#include "Shared/Core/Const/ConstGlobal.h"

// 1:32bit, 2:24bit, 3:20bit, 4:16bit, 5:12bit, 6:10bit, 7:9bit, 8:8bit, 9:7bit, 10:6bit, 11:5bit, 12:4bit PCM
// 13: YM2608 ADPCM, 14: 1bit DPCM, 15: SNES BRR, 16: PS1 VAG, 17: IMA ADPCM,
// 18: CD-ROM XA, 19: YMZ280B, 20: K053260, 21: K054539
std::vector<SelectItem> QualityPcm::qualityItems = {
    {.name = " 1: Raw (32bit)", .value = 1 },
    {.name = " 2: 24-bit PCM",  .value = 2 },
    {.name = " 3: 20-bit PCM",  .value = 3 },
    {.name = " 4: 16-bit PCM",  .value = 4 },
    {.name = " 5: 12-bit PCM",  .value = 5 },
    {.name = " 6: 10-bit PCM",  .value = 6 },
    {.name = " 7: 9-bit PCM",   .value = 7 },
    {.name = " 8: 8-bit PCM",   .value = 8 },
    {.name = " 9: 7-bit PCM",   .value = 9 },
    {.name = "10: 6-bit PCM",   .value = 10 },
    {.name = "11: 5-bit PCM",   .value = 11 },
    {.name = "12: 4-bit PCM",   .value = 12 },
    {.name = "13: 4-bit ADPCM", .value = 13 },
    {.name = "14: 1-bit DPCM",  .value = 14 },
    {.name = "15: SNES BRR",    .value = 15 },
    {.name = "16: PS1 VAG",     .value = 16 },
    {.name = "17: IMA ADPCM",   .value = 17 },
    {.name = "18: CD-ROM XA",   .value = 18 },
    {.name = "19: YMZ280B",     .value = 19 },
    {.name = "20: K053260",     .value = 20 },
    {.name = "21: K054539",     .value = 21 },
};

// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
std::vector<SelectItem> QualityPcm::rateItems = {
    {.name = " 1: 96kHz",    .value = 1 },
    {.name = " 2: 55.5kHz",  .value = 2 },
    {.name = " 3: 49.7kHz",  .value = 3 },
    {.name = " 4: 48kHz",    .value = 4 },
    {.name = " 5: 44.1kHz",  .value = 5 },
    {.name = " 6: 33.08kHz", .value = 6 },
    {.name = " 7: 32kHz",    .value = 7 },
    {.name = " 8: 22.05kHz", .value = 8 },
    {.name = " 9: 16kHz",    .value = 9 },
    {.name = "10: 12kHz",    .value = 10 },
    {.name = "11: 11kHz",    .value = 11 },
    {.name = "12: 8kHz",     .value = 12 },
    {.name = "13: 5.5kHz",   .value = 13 },
    {.name = "14: 4kHz",     .value = 14 },
    {.name = "15: 2kHz",     .value = 15 },
};

// 言語で名前が変わるので、静的な置き場には持たない。プログラムが
// 始まる前に作ると、言語が決まる前の文字列で固まってしまう。
std::vector<SelectItem> QualityPcm::interpItems()
{
    return {
    {.name = I18n::pick(u8"1: 補完なし (Nearest)", u8"1: Nearest (none)"), .value = 1 },
    {.name = I18n::pick(u8"2: 線形補間 (Linear)", u8"2: Linear"), .value = 2 },
    {.name = I18n::pick(u8"3: ガウス補完 (Gaussian)", u8"3: Gaussian"), .value = 3 },
    {.name = I18n::pick(u8"4: ZOH (Zero-Order Hold)", u8"4: ZOH (zero-order hold)"), .value = 4 },
    {.name = I18n::pick(u8"5: コサイン補間 (Cosine)", u8"5: Cosine"), .value = 5 },
    {.name = I18n::pick(u8"6: B-スプライン補間 (B-Spline)", u8"6: B-spline"), .value = 6 },
    {.name = I18n::pick(u8"7: ラグランジュ補間 (Lagrange)", u8"7: Lagrange"), .value = 7 }
    };
}

// 高域カットの段階。言語で名前が変わるので、その場で作る。
std::vector<SelectItem> QualityPcm::nrLpfItems()
{
    return {
        {.name = I18n::pick(u8"1: 切", u8"1: Off"), .value = 1 },
        {.name = I18n::pick(u8"2: 弱", u8"2: Light"), .value = 2 },
        {.name = I18n::pick(u8"3: 中", u8"3: Medium"), .value = 3 },
        {.name = I18n::pick(u8"4: 強", u8"4: Strong"), .value = 4 },
    };
}

void QualityPcm::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder) {
    qualityCat.setupCategory({ .parent = parent, .title = juce::String("") + "QUALITY", .enableChangeDetailVisible = true }, GuiColor::Category::QualityBg);

    modeSelector.setup({ .parent = parent, .id = code + CPK::QualityPcm::mode, .title = "BIT RATE", .items = qualityItems, .isReset = true });
    modeSelector.setWantsKeyboardFocus(true);
    modeSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = parent, .id = code + CPK::QualityPcm::rate, .title = "SMP.RATE", .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    interpSelector.setup({ .parent = parent, .id = code + CPK::QualityPcm::interp, .title = "INTERP", .items = interpItems(), .isReset = true });
    interpSelector.setWantsKeyboardFocus(true);
    interpSelector.setExplicitFocusOrder(++tabOrder);

    // ---------------- ノイズリダクション ----------------
    nrSeparator.setupComponent(parent);

    nrResampleToggle.setup({ .parent = parent, .id = code + CPK::QualityPcm::nrResample, .title = "NR: Resample", .isReset = true });
    nrResampleToggle.setWantsKeyboardFocus(true);
    nrResampleToggle.setExplicitFocusOrder(++tabOrder);

    nrGateToggle.setup({ .parent = parent, .id = code + CPK::QualityPcm::nrGate, .title = "NR: Gate", .isReset = true });
    nrGateToggle.setWantsKeyboardFocus(true);
    nrGateToggle.setExplicitFocusOrder(++tabOrder);

    nrGateLevelSlider.setup({ .parent = parent, .id = code + CPK::QualityPcm::nrGateLevel, .title = "GATE.LV", .isReset = true });
    nrGateLevelSlider.setTextValueSuffix(" dB");
    nrGateLevelSlider.setWantsKeyboardFocus(true);
    nrGateLevelSlider.setExplicitFocusOrder(++tabOrder);

    nrLpfSelector.setup({ .parent = parent, .id = code + CPK::QualityPcm::nrLpf, .title = "NR.LPF", .items = nrLpfItems(), .isReset = true });
    nrLpfSelector.setWantsKeyboardFocus(true);
    nrLpfSelector.setExplicitFocusOrder(++tabOrder);

    // モードや札は、押したときだけでなく TARGET の切り替えやプリセットの
    // 読み込みでも変わる。どこから変わっても追えるよう、変化を受ける。
    modeSelector.onChange = [this] { applyActive(); };
    nrGateToggle.watchToggle([this] { applyActive(); });

    applyActive();
}

// 効いていない設定は押せなくする。触れてしまうと、動かしたのに音が
// 変わらない、という形で迷う。
void QualityPcm::applyActive()
{
    // きれいな間引きは、符号化するモード (13 以降) の素材づくりにだけ効く。
    // 1〜12 は素材をそのまま鳴らすので、間引かない。
    const bool encoded = modeSelector.getSelectedItemIndex() + 1 >= CPV::QualityPcm::Bit::encodedFirst;

    // トグルは止めずに薄くする。止めると、そのあいだに来た値を捨ててしまう。
    nrSeparator.setEnabled(outerEnabled);

    nrResampleToggle.setEnabled(outerEnabled);
    nrResampleToggle.setDimmed(!encoded);

    nrGateToggle.setEnabled(outerEnabled);
    nrGateLevelSlider.setEnabledWithLabel(outerEnabled && nrGateToggle.getToggleState());

    nrLpfSelector.setEnabledWithLabel(outerEnabled);
}

// 束縛先を丸ごと差し替える。
//
// 同じ部品を並べる代わりに 1 つだけ置き、TARGET で指し先を切り替える
// ための口。setup で組んだ見た目はそのままに、APVTS への繋ぎだけを
// 張り替える。
void QualityPcm::rebind(const juce::String& code)
{
    modeSelector.rebind(code + CPK::QualityPcm::mode);
    rateSelector.rebind(code + CPK::QualityPcm::rate);
    interpSelector.rebind(code + CPK::QualityPcm::interp);
    nrResampleToggle.rebind(code + CPK::QualityPcm::nrResample);
    nrGateToggle.rebind(code + CPK::QualityPcm::nrGate);
    nrGateLevelSlider.rebind(code + CPK::QualityPcm::nrGateLevel);
    nrLpfSelector.rebind(code + CPK::QualityPcm::nrLpf);
}

void QualityPcm::layoutComponent(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visible = qualityCat.isDetailVisible();

    modeSelector.setVisibleWithLabel(visible);
    rateSelector.setVisibleWithLabel(visible);
    interpSelector.setVisibleWithLabel(visible);
    nrSeparator.setVisible(visible);
    nrResampleToggle.setVisible(visible);
    nrGateToggle.setVisible(visible);
    nrGateLevelSlider.setVisibleWithLabel(visible);
    nrLpfSelector.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &modeSelector.label, .component = &modeSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
        layoutMain({ .mainRect = rect, .label = &interpSelector.label, .component = &interpSelector, });

        nrSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &nrResampleToggle });
        layoutMain({ .mainRect = rect, .component = &nrGateToggle });
        layoutMain({ .mainRect = rect, .label = &nrGateLevelSlider.label, .component = &nrGateLevelSlider });
        layoutMain({ .mainRect = rect, .label = &nrLpfSelector.label, .component = &nrLpfSelector });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void QualityPcm::layoutComponentRow(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &qualityCat });

    bool visible = qualityCat.isDetailVisible();

    modeSelector.setVisibleWithLabel(visible);
    rateSelector.setVisibleWithLabel(visible);
    interpSelector.setVisibleWithLabel(visible);
    nrSeparator.setVisible(visible);
    nrResampleToggle.setVisible(visible);
    nrGateToggle.setVisible(visible);
    nrGateLevelSlider.setVisibleWithLabel(visible);
    nrLpfSelector.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &modeSelector.label, .component = &modeSelector });
        layoutRow({ .rowRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
        layoutRow({ .rowRect = rect, .label = &interpSelector.label, .component = &interpSelector, });

        nrSeparator.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .component = &nrResampleToggle });
        layoutRow({ .rowRect = rect, .component = &nrGateToggle });
        layoutRow({ .rowRect = rect, .label = &nrGateLevelSlider.label, .component = &nrGateLevelSlider });
        layoutRow({ .rowRect = rect, .label = &nrLpfSelector.label, .component = &nrLpfSelector });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void QualityPcm::setImportingParams(juce::StringArray& lines, int& index) {
    modeSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    rateSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
	interpSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
}

void QualityPcm::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    modeSelector.setSelectedItemIndex(r.getInt("mode", modeSelector.getSelectedItemIndex()), juce::sendNotification);
    rateSelector.setSelectedItemIndex(r.getInt("rate", rateSelector.getSelectedItemIndex()), juce::sendNotification);
	interpSelector.setSelectedItemIndex(r.getInt("interp", interpSelector.getSelectedItemIndex()), juce::sendNotification);

    readNrParams(r);
}

void QualityPcm::readNrParams(const Io::ParamReader& reader)
{
    nrResampleToggle.setToggleState(reader.getBool("nrResample", nrResampleToggle.getToggleState()), juce::sendNotification);
    nrGateToggle.setToggleState(reader.getBool("nrGate", nrGateToggle.getToggleState()), juce::sendNotification);
    nrGateLevelSlider.setValue(reader.getFloat("nrGateLevel", (float)nrGateLevelSlider.getValue()), juce::sendNotification);
    nrLpfSelector.setSelectedItemIndex(reader.getInt("nrLpf", nrLpfSelector.getSelectedItemIndex()), juce::sendNotification);
}

void QualityPcm::writeNrParams(Io::ParamWriter& writer)
{
    writer.set("nrResample", nrResampleToggle.getToggleState());
    writer.set("nrGate", nrGateToggle.getToggleState());
    writer.set("nrGateLevel", (float)nrGateLevelSlider.getValue());
    writer.set("nrLpf", nrLpfSelector.getSelectedItemIndex());
}

juce::String QualityPcm::getExportedParams() {
    juce::String content = "";

    content += juce::String(modeSelector.getSelectedItemIndex()) + "\n";
    content += juce::String(rateSelector.getSelectedItemIndex()) + "\n";
    content += juce::String(interpSelector.getSelectedItemIndex()) + "\n";

    return content;
}

void QualityPcm::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    w.set("mode", modeSelector.getSelectedItemIndex());
    w.set("rate", rateSelector.getSelectedItemIndex());
    w.set("interp", interpSelector.getSelectedItemIndex());

    writeNrParams(w);
}
