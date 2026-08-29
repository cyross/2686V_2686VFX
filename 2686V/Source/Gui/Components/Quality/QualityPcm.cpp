#include "./QualityPcm.h"

#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Const/ConstGlobal.h"

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
    {.name = "12: 5.5kHz",   .value = 13 },
    {.name = "13: 4kHz",     .value = 14 },
    {.name = "15: 2kHz",     .value = 15 },
};

std::vector<SelectItem> QualityPcm::interpItems = {
    {.name = juce::String("") + "1: 補完なし (Nearest)", .value = 1 },
    {.name = juce::String("") + "2: 線形補間 (Linear)", .value = 2 },
    {.name = juce::String("") + "3: ガウス補完 (Gaussian)", .value = 3 },
    {.name = juce::String("") + "4: ZOH (Zero-Order Hold)", .value = 4 },
    {.name = juce::String("") + "5: コサイン補間 (Cosine)", .value = 5 },
    {.name = juce::String("") + "6: B-スプライン補間 (B-Spline)", .value = 6 },
    {.name = juce::String("") + "7: ラグランジュ補間 (Lagrange)", .value = 7 }
};

void QualityPcm::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder) {
    qualityCat.setupCategory({ .parent = parent, .title = juce::String("") + "QUALITY", .enableChangeDetailVisible = true }, GuiColor::Category::QualityBg);

    modeSelector.setup({ .parent = parent, .id = code + CPK::QualityPcm::mode, .title = "BIT", .items = qualityItems, .isReset = true });
    modeSelector.setWantsKeyboardFocus(true);
    modeSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = parent, .id = code + CPK::QualityPcm::rate, .title = "RATE", .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    interpSelector.setup({ .parent = parent, .id = code + CPK::QualityPcm::interp, .title = "INTP", .items = interpItems, .isReset = true });
    interpSelector.setWantsKeyboardFocus(true);
    interpSelector.setExplicitFocusOrder(++tabOrder);
}

void QualityPcm::layoutComponent(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visible = qualityCat.isDetailVisible();

    modeSelector.setVisibleWithLabel(visible);
    rateSelector.setVisibleWithLabel(visible);
    interpSelector.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &modeSelector.label, .component = &modeSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
        layoutMain({ .mainRect = rect, .label = &interpSelector.label, .component = &interpSelector, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void QualityPcm::layoutComponentRow(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &qualityCat });

    bool visible = qualityCat.isDetailVisible();

    modeSelector.setVisibleWithLabel(visible);
    rateSelector.setVisibleWithLabel(visible);
    interpSelector.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &modeSelector.label, .component = &modeSelector });
        layoutRow({ .rowRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
        layoutRow({ .rowRect = rect, .label = &interpSelector.label, .component = &interpSelector, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void QualityPcm::setImportingParams(juce::StringArray& lines, int& index) {
    modeSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    rateSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
	interpSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
}

juce::String QualityPcm::getExportedParams() {
    juce::String content = "";

    content += juce::String(modeSelector.getSelectedItemIndex()) + "\n";
    content += juce::String(rateSelector.getSelectedItemIndex()) + "\n";
    content += juce::String(interpSelector.getSelectedItemIndex()) + "\n";

    return content;
}
