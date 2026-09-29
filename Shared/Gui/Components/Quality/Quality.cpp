#include "./Quality.h"
#include "./QualityPcm.h"
#include "Shared/Core/Editor/EditorParamBrowserText.h"

#include "../../../Core/Io/ParamFile.h"

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Shared/Core/Const/ConstGlobal.h"

namespace
{
    // DAC の行。ラベルは他の行と揃え、ボタンのぶんをコンボボックスから削る。
    void layoutDacRow(GuiComponentDac& dac, juce::Rectangle<int>& rect, int rowHeight, int paddingTop, int paddingBottom)
    {
        namespace Row = CoreGuiValue::MainGroup::Row;

        dac.layoutComponent(rect, rowHeight, paddingTop, paddingBottom,
            CoreGuiValue::MainGroup::Label::width,
            CoreGuiValue::MainGroup::Value::width - Row::Dac::ApplyBtn::width,
            Row::Dac::ApplyBtn::width);
    }
}

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

// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.55k 14: 4.41k 15: 2.21k
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
    {.name = "13: 5.55kHz",  .value = 13 },
    {.name = "14: 4.41kHz",  .value = 14 },
    {.name = "15: 2.21kHz",  .value = 15 },
};

void Quality::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder) {
    qualityCat.setupCategory({ .parent = parent, .title = juce::String("") + "QUALITY", .enableChangeDetailVisible = true }, GuiColor::Category::QualityBg);

    // 機種を選んで「適応」を押すと、下の 3 つへまとめて入れる
    dac.setupComponent(parent, GuiComponentDac::Kind::Quality, tabOrder, [this](const GuiComponentDac::Values& v) {
        bitSelector.setSelectedItemIndex(v.bit, juce::sendNotification);
        rateSelector.setSelectedItemIndex(v.rate, juce::sendNotification);
        interpSelector.setSelectedItemIndex(v.interp, juce::sendNotification);
    });

    bitSelector.setup({ .parent = parent, .id = code + CPK::Quality::bit, .title = "BIT RATE", .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = parent, .id = code + CPK::Quality::rate, .title = "SMP.RATE", .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    // 目標レートで作った点の間の埋め方。選べる中身は QUALITY(PCM) と同じ。
    interpSelector.setup({ .parent = parent, .id = code + CPK::Quality::interp, .title = "INTERP", .items = QualityPcm::interpItems(), .isReset = true });
    interpSelector.setWantsKeyboardFocus(true);
    interpSelector.setExplicitFocusOrder(++tabOrder);
}

// 束縛先を丸ごと差し替える。
//
// 同じ部品を並べる代わりに 1 つだけ置き、TARGET で指し先を切り替える
// ための口。setup で組んだ見た目はそのままに、APVTS への繋ぎだけを
// 張り替える。
void Quality::rebind(const juce::String& code)
{
    bitSelector.rebind(code + CPK::Quality::bit);
    rateSelector.rebind(code + CPK::Quality::rate);
    interpSelector.rebind(code + CPK::Quality::interp);
}

void Quality::layoutComponent(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visible = qualityCat.isDetailVisible();

    dac.setVisibles(visible);
    bitSelector.setVisibleWithLabel(visible);
    rateSelector.setVisibleWithLabel(visible);
    interpSelector.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutDacRow(dac, rect, CoreGuiValue::MainGroup::Row::height, CoreGuiValue::MainGroup::Row::paddingTop, CoreGuiValue::MainGroup::Row::paddingBottom);
        layoutMain({ .mainRect = rect, .label = &bitSelector.label, .component = &bitSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
        layoutMain({ .mainRect = rect, .label = &interpSelector.label, .component = &interpSelector, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void Quality::layoutComponentRow(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &qualityCat });

    bool visible = qualityCat.isDetailVisible();

    dac.setVisibles(visible);
    bitSelector.setVisibleWithLabel(visible);
    rateSelector.setVisibleWithLabel(visible);
    interpSelector.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutDacRow(dac, rect, CoreGuiValue::ParamGroup::Row::height, CoreGuiValue::ParamGroup::Row::paddingTop, CoreGuiValue::ParamGroup::Row::paddingBottom);
        layoutRow({ .rowRect = rect, .label = &bitSelector.label, .component = &bitSelector });
        layoutRow({ .rowRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
        layoutRow({ .rowRect = rect, .label = &interpSelector.label, .component = &interpSelector, });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// 3.0.0 より前の行並びの形式。行数を変えられないので INTERP は読まない
// (読んだあとも、いまの値のまま)。
void Quality::setImportingParams(juce::StringArray& lines, int& index) {
    bitSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    rateSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
}

void Quality::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    bitSelector.setSelectedItemIndex(r.getInt("bit", bitSelector.getSelectedItemIndex()), juce::sendNotification);
    rateSelector.setSelectedItemIndex(r.getInt("rate", rateSelector.getSelectedItemIndex()), juce::sendNotification);

    // INTERP は 3.6.0 で足したもの。持たないファイルは、いまの値のままにする。
    interpSelector.setSelectedItemIndex(r.getInt("interp", interpSelector.getSelectedItemIndex()), juce::sendNotification);
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
    w.set("interp", interpSelector.getSelectedItemIndex());
}
