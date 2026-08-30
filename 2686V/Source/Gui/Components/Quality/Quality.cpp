#include "./Quality.h"

#include "../../../Core/Io/ParamFile.h"

#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Const/ConstGlobal.h"

// 1:4bit, 2:5bit, 3:6bit, 4:7bit, 5:8bit, 6:9bit, 7:10bit, 8:12bit, 9:16bit, 10:20bit, 11:24bit, 12:raw(32bit)
std::vector<SelectItem> Quality::bdItems = {
    {.name = " 1:  4-bit (16 steps)",       .value = 1 },
    {.name = " 2:  5-bit (32 steps)",       .value = 2 },
    {.name = " 3:  6-bit (64 steps)",       .value = 3 },
    {.name = " 4:  7-bit (128 steps)",      .value = 4 },
    {.name = " 5:  8-bit (256 steps)",      .value = 5 },
    {.name = " 6:  9-bit (512 steps)",      .value = 6 },
    {.name = " 7: 10-bit (1024 steps)",     .value = 7 },
    {.name = " 8: 12-bit (4096 steps)",     .value = 8 },
    {.name = " 9: 16-bit (32768 steps)",    .value = 9 },
    {.name = "10: 20-bit (1048576 steps)",  .value = 10 },
    {.name = "11: 24-bit (16777216 steps)", .value = 11 },
    {.name = "12: Raw",                     .value = 12 }
};

// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
std::vector<SelectItem> Quality::rateItems = {
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

void Quality::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder) {
    qualityCat.setupCategory({ .parent = parent, .title = juce::String("") + "QUALITY", .enableChangeDetailVisible = true }, GuiColor::Category::QualityBg);

    bitSelector.setup({ .parent = parent, .id = code + CPK::Quality::bit, .title = "BIT", .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = parent, .id = code + CPK::Quality::rate, .title = "RATE", .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);
}

void Quality::layoutComponent(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visible = qualityCat.isDetailVisible();

    bitSelector.setVisibleWithLabel(visible);
    rateSelector.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &bitSelector.label, .component = &bitSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void Quality::layoutComponentRow(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &qualityCat });

    bool visible = qualityCat.isDetailVisible();

    bitSelector.setVisibleWithLabel(visible);
    rateSelector.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &bitSelector.label, .component = &bitSelector });
        layoutRow({ .rowRect = rect, .label = &rateSelector.label, .component = &rateSelector, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void Quality::setImportingParams(juce::StringArray& lines, int& index) {
    bitSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    rateSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
}

void Quality::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    bitSelector.setSelectedItemIndex(r.getInt("bit", bitSelector.getSelectedItemIndex()), juce::sendNotification);
    rateSelector.setSelectedItemIndex(r.getInt("rate", rateSelector.getSelectedItemIndex()), juce::sendNotification);
}

juce::String Quality::getExportedParams() {
    juce::String content = "";

    content += juce::String(bitSelector.getSelectedItemIndex()) + "\n";
    content += juce::String(rateSelector.getSelectedItemIndex()) + "\n";

    return content;
}

void Quality::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    w.set("bit", bitSelector.getSelectedItemIndex());
    w.set("rate", rateSelector.getSelectedItemIndex());
}
