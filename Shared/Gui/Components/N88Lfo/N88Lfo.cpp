#include "./N88Lfo.h"

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiStructs.h"
#include "Shared/Core/Gui/GuiRefresh.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "../WavePreview/WavePreviewSource.h"

static std::vector<SelectItem> lfoShapeItems = {
    {.name = "0: Saw Up",              .value = 1 },
    {.name = "1: Square",              .value = 2 },
    {.name = "2: Triangle",            .value = 3 },
    {.name = "3: Sample & Hold",       .value = 4 },
    {.name = "4: Saw Down & One Shot", .value = 5 },
    {.name = "5: Triangle & One Shot", .value = 6 },
};

void GuiComponentN88Lfo::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder)
{
    cat.setupSwLfoCategory({ .parent = parent, .title = u8"N88 LFO", .enableChangeDetailVisible = true });

    freq.setup({ .parent = parent, .id = code + CPK::N88Lfo::freq, .title = u8"SPEED", .isReset = true });
    freq.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    freq.setWantsKeyboardFocus(true);
    freq.setExplicitFocusOrder(++tabOrder);

    shape.setup({ .parent = parent, .id = code + CPK::N88Lfo::shape, .title = u8"SHAPE", .items = lfoShapeItems, .isReset = true });
    shape.setWantsKeyboardFocus(true);
    shape.setExplicitFocusOrder(++tabOrder);

    amSmRt.setup({ .parent = parent, .id = code + CPK::N88Lfo::amSmoothRatio, .title = u8"SM.RATIO", .isReset = true });
    amSmRt.setWantsKeyboardFocus(true);
    amSmRt.setExplicitFocusOrder(++tabOrder);

    pmPreview.setup(parent, GuiColor::WavePreview::Lfo);
    amPreview.setup(parent, GuiColor::WavePreview::Lfo);

    auto refreshPreviews = [this]() { updatePreviews(); };

    shape.onChange = refreshPreviews;
    amSmRt.onValueChange = refreshPreviews;

    updatePreviews();

    syncDelay.setup({ .parent = parent, .id = code + CPK::N88Lfo::syncDelay, .title = u8"SY.DELAY", .isReset = true });
    syncDelay.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    syncDelay.setWantsKeyboardFocus(true);
    syncDelay.setExplicitFocusOrder(++tabOrder);

    syncDelayToZero.setup({ .parent = parent, .title = "Async", .isReset = false, .isResized = false });
    syncDelayToZero.setWantsKeyboardFocus(true);
    syncDelayToZero.setExplicitFocusOrder(++tabOrder);
    syncDelayToZero.onClick = [this] {
        syncDelay.setValue(0.0f);
        };

    syncDelayToOne.setup({ .parent = parent, .title = "Sync", .isReset = false, .isResized = false });
    syncDelayToOne.setWantsKeyboardFocus(true);
    syncDelayToOne.setExplicitFocusOrder(++tabOrder);
    syncDelayToOne.onClick = [this] {
        syncDelay.setValue(1.0f);
        };

    pmEnable.setup({ .parent = parent, .id = code + CPK::N88Lfo::pm, .title = u8"PM Enable", .isReset = true });
    pmEnable.setWantsKeyboardFocus(true);
    pmEnable.setExplicitFocusOrder(++tabOrder);

    pmd.setup({ .parent = parent, .id = code + CPK::N88Lfo::pmd, .title = u8"PMD", .isReset = true });
    pmd.setWantsKeyboardFocus(true);
    pmd.setExplicitFocusOrder(++tabOrder);

    pms.setup({ .parent = parent, .id = code + CPK::N88Lfo::pms, .title = u8"PMS", .isReset = true });
    pms.setWantsKeyboardFocus(true);
    pms.setExplicitFocusOrder(++tabOrder);

    amEnable.setup({ .parent = parent, .id = code + CPK::N88Lfo::am, .title = u8"AM Enable", .isReset = true });
    amEnable.setWantsKeyboardFocus(true);
    amEnable.setExplicitFocusOrder(++tabOrder);

    amd.setup({ .parent = parent, .id = code + CPK::N88Lfo::amd, .title = u8"AMD", .isReset = true });
    amd.setWantsKeyboardFocus(true);
    amd.setExplicitFocusOrder(++tabOrder);

    sep1.setupComponent(parent);
    sep2.setupComponent(parent);

    // 札の入り切りは、押したときだけでなくプリセットの読み込みでも
    // 変わる。どこから変わっても追えるよう、状態の変化を受ける。
    pmEnable.watchToggle([this] { applyActive(); });
    amEnable.watchToggle([this] { applyActive(); });

    applyActive();
}

// 効いていないつまみは押せなくする。触れてしまうと、動かしたのに
// 音が変わらない、という形で迷う。
void GuiComponentN88Lfo::applyActive()
{
    const bool pm = pmEnable.getToggleState();
    const bool am = amEnable.getToggleState();
    const bool any = pm || am;

    freq.setEnabledWithLabel(any);
    shape.setEnabledWithLabel(any);
    syncDelay.setEnabledWithLabel(any);
    syncDelayToZero.setEnabled(any);
    syncDelayToOne.setEnabled(any);
    sep1.setEnabled(any);
    sep2.setEnabled(any);

    pmPreview.setEnabled(pm);
    pmd.setEnabledWithLabel(pm);
    pms.setEnabledWithLabel(pm);

    amSmRt.setEnabledWithLabel(am);
    amPreview.setEnabled(am);
    amd.setEnabledWithLabel(am);
}

// 選んだ Shape を実際の LFO で走らせ、折れ線にして渡す。
// 値が変わったときだけ通るので、常時の負荷は無い。
void GuiComponentN88Lfo::updatePreviews()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updatePreviews(); })) return;

    int shapeIndex = shape.getSelectedItemIndex();

    // PM は -1.0〜1.0 の両振り
    pmPreview.setPoints(WavePreviewSource::n88LfoPm(shapeIndex), true);

    // AM は 0.0〜1.0 の片側。スムースの効きも見えるよう実際の値を渡す。
    amPreview.setPoints(WavePreviewSource::n88LfoAm(shapeIndex, (float)amSmRt.getValue()), false);
}

void GuiComponentN88Lfo::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &cat });

    bool visible = cat.isDetailVisible();

    freq.setVisibleWithLabel(visible);
    shape.setVisibleWithLabel(visible);
    pmPreview.setVisible(visible);
    amPreview.setVisible(visible);
    amSmRt.setVisibleWithLabel(visible);
    syncDelay.setVisibleWithLabel(visible);
    syncDelayToZero.setVisible(visible);
    syncDelayToOne.setVisible(visible);
    sep1.setVisible(visible);
    pmEnable.setVisible(visible);
    pms.setVisibleWithLabel(visible);
    pmd.setVisibleWithLabel(visible);
    sep2.setVisible(visible);
    amEnable.setVisible(visible);
    amd.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &freq.label, .component = &freq });
        layoutMain({ .mainRect = rect, .label = &shape.label, .component = &shape });
        layoutMain({ .mainRect = rect, .label = &amSmRt.label, .component = &amSmRt });
        layoutMain({ .mainRect = rect, .label = &syncDelay.label, .component = &syncDelay });
        layoutMainTwoComps({ .rect = rect, .comp1 = &syncDelayToZero, .comp2 = &syncDelayToOne });
        sep1.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &pmEnable });
        pmPreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        layoutMain({ .mainRect = rect, .label = &pmd.label, .component = &pmd });
        layoutMain({ .mainRect = rect, .label = &pms.label, .component = &pms });
        sep2.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &amEnable });
        amPreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        layoutMain({ .mainRect = rect, .label = &amd.label, .component = &amd });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentN88Lfo::copyParams(CopyLfoN88& copyObj)
{
    copyObj.freq = (float)freq.getValue();
    copyObj.wave = shape.getSelectedId();
    copyObj.amSmRt = (float)amSmRt.getValue();
    copyObj.syncDelay = (int)syncDelay.getValue();
    copyObj.pmEnable = pmEnable.getToggleState();
    copyObj.amEnable = amEnable.getToggleState();
    copyObj.pmd = (float)pmd.getValue();
    copyObj.pms = (float)pms.getValue();
    copyObj.amd = (float)amd.getValue();
}

void GuiComponentN88Lfo::pasteParams(CopyLfoN88& copyObj)
{
    freq.setValue(copyObj.freq, juce::sendNotification);
    shape.setSelectedId(copyObj.wave, juce::sendNotification);
    amSmRt.setValue(copyObj.amSmRt, juce::sendNotification);
    syncDelay.setValue(copyObj.syncDelay, juce::sendNotification);
    pmEnable.setToggleState(copyObj.pmEnable, juce::sendNotification);
    amEnable.setToggleState(copyObj.amEnable, juce::sendNotification);
    pmd.setValue(copyObj.pmd, juce::sendNotification);
    pms.setValue(copyObj.pms, juce::sendNotification);
    amd.setValue(copyObj.amd, juce::sendNotification);
}

void GuiComponentN88Lfo::readChParams(const Io::ParamReader& reader)
{
    freq.setValue(reader.getFloat("lfoFreq", (float)freq.getValue()), juce::sendNotification);
    shape.setSelectedId(reader.getInt("lfoShape", shape.getSelectedId()), juce::sendNotification);
    amSmRt.setValue(reader.getFloat("lfoAmSmRt", (float)amSmRt.getValue()), juce::sendNotification);
    syncDelay.setValue(reader.getFloat("lfoSyncDelay", (float)syncDelay.getValue()), juce::sendNotification);
    pmEnable.setToggleState(reader.getBool("lfoPm", pmEnable.getToggleState()), juce::sendNotification);
    pms.setValue(reader.getFloat("lfoPms", (float)pms.getValue()), juce::sendNotification);
    pmd.setValue(reader.getFloat("lfoPmd", (float)pmd.getValue()), juce::sendNotification);
    amEnable.setToggleState(reader.getBool("lfoAm", amEnable.getToggleState()), juce::sendNotification);
    amd.setValue(reader.getFloat("lfoAmd", (float)amd.getValue()), juce::sendNotification);
}

void GuiComponentN88Lfo::writeChParams(Io::ParamWriter& writer)
{
    writer.set("lfoFreq", (float)freq.getValue());
    writer.set("lfoShape", shape.getSelectedId());
    writer.set("lfoAmSmRt", (float)amSmRt.getValue());
    writer.set("lfoSyncDelay", (float)syncDelay.getValue());
    writer.set("lfoPm", pmEnable.getToggleState());
    writer.set("lfoPms", (float)pms.getValue());
    writer.set("lfoPmd", (float)pmd.getValue());
    writer.set("lfoAm", amEnable.getToggleState());
    writer.set("lfoAmd", (float)amd.getValue());
}

void GuiComponentN88Lfo::setImportingChParams(juce::StringArray& lines, int& index)
{
    freq.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    shape.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    amSmRt.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    syncDelay.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    pmEnable.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    pms.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    pmd.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    amEnable.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    amd.setValue(lines[index++].getFloatValue(), juce::sendNotification);
}

void GuiComponentN88Lfo::readFileParams(const Io::ParamReader& reader)
{
    freq.setValue(reader.getInt("lfoFreq", (int)freq.getValue()), juce::sendNotification);
    shape.setSelectedItemIndex(reader.getInt("lfoShape", shape.getSelectedItemIndex()), juce::sendNotification);
    syncDelay.setValue(reader.getInt("lfoSyncDelay", (int)syncDelay.getValue()), juce::sendNotification);
    pmEnable.setToggleState(reader.getBool("lfoPm", pmEnable.getToggleState()), juce::sendNotification);
    pms.setValue(reader.getInt("lfoPms", (int)pms.getValue()), juce::sendNotification);
    pmd.setValue(reader.getInt("lfoPmd", (int)pmd.getValue()), juce::sendNotification);
    amEnable.setToggleState(reader.getBool("lfoAm", amEnable.getToggleState()), juce::sendNotification);
    amSmRt.setValue(reader.getFloat("lfoAmSmRt", (float)amSmRt.getValue()), juce::sendNotification);
    amd.setValue(reader.getInt("lfoAmd", (int)amd.getValue()), juce::sendNotification);
}

void GuiComponentN88Lfo::writeFileParams(Io::ParamWriter& writer)
{
    writer.set("lfoFreq", (int)freq.getValue());
    writer.set("lfoShape", shape.getSelectedItemIndex());
    writer.set("lfoSyncDelay", (int)syncDelay.getValue());
    writer.set("lfoPm", pmEnable.getToggleState());
    writer.set("lfoPms", (int)pms.getValue());
    writer.set("lfoPmd", (int)pmd.getValue());
    writer.set("lfoAm", amEnable.getToggleState());
    writer.set("lfoAmSmRt", (float)amSmRt.getValue());
    writer.set("lfoAmd", (int)amd.getValue());
}

void GuiComponentN88Lfo::setImportingFileParams(const juce::StringArray& lines)
{
    freq.setValue(lines[0].getIntValue(), juce::sendNotification);
    shape.setSelectedItemIndex(lines[1].getIntValue(), juce::sendNotification);
    syncDelay.setValue(lines[2].getIntValue(), juce::sendNotification);
    pmEnable.setToggleState(lines[3].getIntValue() == 1, juce::sendNotification);
    pms.setValue(lines[4].getIntValue(), juce::sendNotification);
    pmd.setValue(lines[5].getIntValue(), juce::sendNotification);
    amEnable.setToggleState(lines[6].getIntValue() == 1, juce::sendNotification);
    amSmRt.setValue(lines[7].getFloatValue(), juce::sendNotification);
    amd.setValue(lines[8].getIntValue(), juce::sendNotification);
}
