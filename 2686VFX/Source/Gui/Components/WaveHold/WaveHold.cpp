#include "./WaveHold.h"

#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Processor/ProcessorValues.h"

namespace
{
    // 止まる側。パラメータは 0 = MIN / 1 = MAX なので、並びもその順。
    std::vector<SelectItem> holdTargetItems = {
        {.name = "MIN", .value = 1 },
        {.name = "MAX", .value = 2 },
    };
}

void GuiComponentWaveHold::setupComponent(juce::Component& parent, const juce::String& idPrefix, int& tabOrder,
    WaveHoldUnit unit, std::optional<std::function<void()>> onChanged)
{
    m_unit = unit;

    if (onChanged.has_value()) m_onChanged = onChanged.value();

    // ---------------- ホールド ----------------
    holdSeparator.setupComponent(parent);

    holdEnableBtn.setup({ .parent = parent, .id = idPrefix + CPK::WaveHold::holdEnable, .title = "HOLD", .isReset = true, .isResized = true });
    holdEnableBtn.setWantsKeyboardFocus(true);
    holdEnableBtn.setExplicitFocusOrder(++tabOrder);

    holdCount.setupComponent(parent, idPrefix + CPK::WaveHold::holdCount, "COUNT", tabOrder, std::nullopt);

    holdCountButtons.setupComponent(parent, holdCount.getSlider(), tabOrder);

    holdTarget.setup({ .parent = parent, .id = idPrefix + CPK::WaveHold::holdTarget, .title = "TARGET", .items = holdTargetItems, .isReset = true, .isResized = true });
    holdTarget.setWantsKeyboardFocus(true);
    holdTarget.setExplicitFocusOrder(++tabOrder);

    holdMin.setupComponent(parent, idPrefix + CPK::WaveHold::holdMin, "HOLD MIN", tabOrder, std::nullopt);
    holdMax.setupComponent(parent, idPrefix + CPK::WaveHold::holdMax, "HOLD MAX", tabOrder, std::nullopt);

    // 刻みは単位で使い分ける。使わない側は親へ付けないので何も起きない。
    if ((m_unit == WaveHoldUnit::Cent)) {
        holdMinPitch.setupComponent(parent, holdMin.getSlider(), tabOrder);
        holdMaxPitch.setupComponent(parent, holdMax.getSlider(), tabOrder);
    }
    else {
        holdMinNudge.setupComponent(parent, holdMin.getSlider(), tabOrder);
        holdMaxNudge.setupComponent(parent, holdMax.getSlider(), tabOrder);
    }

    // ---------------- 部分再生 ----------------
    keepSeparator.setupComponent(parent);

    keepEnableBtn.setup({ .parent = parent, .id = idPrefix + CPK::WaveHold::keepEnable, .title = "KEEP", .isReset = true, .isResized = true });
    keepEnableBtn.setWantsKeyboardFocus(true);
    keepEnableBtn.setExplicitFocusOrder(++tabOrder);
    keepEnableBtn.onClick = [this] { notifyChanged(); };

    // START は END を追い越さない。押し合いが往復しないよう印で弾く。
    waveStart.setupComponent(parent, idPrefix + CPK::WaveHold::waveStart, "START", tabOrder, [this] {
        if (!isClampingWindow) {
            isClampingWindow = true;

            if (waveStart.getValue() > waveEnd.getValue()) {
                waveEnd.setValue(waveStart.getValue(), juce::sendNotification);
            }

            isClampingWindow = false;
        }

        notifyChanged();
        });

    waveStartNudge.setupComponent(parent, waveStart.getSlider(), tabOrder);

    keepStartBtn.setup({ .parent = parent, .id = idPrefix + CPK::WaveHold::keepStart, .title = "KEEP START", .isReset = true, .isResized = true });
    keepStartBtn.setWantsKeyboardFocus(true);
    keepStartBtn.setExplicitFocusOrder(++tabOrder);
    keepStartBtn.onClick = [this] { notifyChanged(); };

    waveEnd.setupComponent(parent, idPrefix + CPK::WaveHold::waveEnd, "END", tabOrder, [this] {
        if (!isClampingWindow) {
            isClampingWindow = true;

            if (waveEnd.getValue() < waveStart.getValue()) {
                waveStart.setValue(waveEnd.getValue(), juce::sendNotification);
            }

            isClampingWindow = false;
        }

        notifyChanged();
        });

    waveEndNudge.setupComponent(parent, waveEnd.getSlider(), tabOrder);

    keepEndBtn.setup({ .parent = parent, .id = idPrefix + CPK::WaveHold::keepEnd, .title = "KEEP END", .isReset = true, .isResized = true });
    keepEndBtn.setWantsKeyboardFocus(true);
    keepEndBtn.setExplicitFocusOrder(++tabOrder);
    keepEndBtn.onClick = [this] { notifyChanged(); };
}

void GuiComponentWaveHold::rebind(const juce::String& idPrefix)
{
    holdEnableBtn.rebind(idPrefix + CPK::WaveHold::holdEnable);
    holdCount.getSlider().rebind(idPrefix + CPK::WaveHold::holdCount);
    holdTarget.rebind(idPrefix + CPK::WaveHold::holdTarget);
    holdMin.getSlider().rebind(idPrefix + CPK::WaveHold::holdMin);
    holdMax.getSlider().rebind(idPrefix + CPK::WaveHold::holdMax);

    keepEnableBtn.rebind(idPrefix + CPK::WaveHold::keepEnable);
    waveStart.getSlider().rebind(idPrefix + CPK::WaveHold::waveStart);
    keepStartBtn.rebind(idPrefix + CPK::WaveHold::keepStart);
    waveEnd.getSlider().rebind(idPrefix + CPK::WaveHold::waveEnd);
    keepEndBtn.rebind(idPrefix + CPK::WaveHold::keepEnd);
}

void GuiComponentWaveHold::setVisibles(bool visible)
{
    holdSeparator.setVisible(visible);
    holdEnableBtn.setVisible(visible);
    holdCount.setVisibles(visible);
    holdCountButtons.setVisibles(visible && holdCount.isVisibleNudge());
    holdTarget.setVisibleWithLabel(visible);
    holdMin.setVisibles(visible);
    holdMax.setVisibles(visible);

    const bool minNudge = visible && holdMin.isVisibleNudge();
    const bool maxNudge = visible && holdMax.isVisibleNudge();

    if ((m_unit == WaveHoldUnit::Cent)) {
        holdMinPitch.setVisibles(minNudge);
        holdMaxPitch.setVisibles(maxNudge);
    }
    else {
        holdMinNudge.setVisibles(minNudge);
        holdMaxNudge.setVisibles(maxNudge);
    }

    keepSeparator.setVisible(visible);
    keepEnableBtn.setVisible(visible);
    waveStart.setVisibles(visible);
    waveStartNudge.setVisibles(visible && waveStart.isVisibleNudge());
    keepStartBtn.setVisible(visible);
    waveEnd.setVisibles(visible);
    waveEndNudge.setVisibles(visible && waveEnd.isVisibleNudge());
    keepEndBtn.setVisible(visible);
}

void GuiComponentWaveHold::layoutComponent(juce::Rectangle<int>& rect)
{
    holdSeparator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .component = &holdEnableBtn });

    holdCount.layoutComponent(rect);

    if (holdCount.isVisibleNudge()) holdCountButtons.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .label = &holdTarget.label, .component = &holdTarget });

    holdMin.layoutComponent(rect);

    if (holdMin.isVisibleNudge()) {
        if ((m_unit == WaveHoldUnit::Cent)) holdMinPitch.layoutComponent(rect);
        else          holdMinNudge.layoutComponent(rect);
    }

    holdMax.layoutComponent(rect);

    if (holdMax.isVisibleNudge()) {
        if ((m_unit == WaveHoldUnit::Cent)) holdMaxPitch.layoutComponent(rect);
        else          holdMaxNudge.layoutComponent(rect);
    }

    keepSeparator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .component = &keepEnableBtn });

    waveStart.layoutComponent(rect);

    if (waveStart.isVisibleNudge()) waveStartNudge.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .component = &keepStartBtn });

    waveEnd.layoutComponent(rect);

    if (waveEnd.isVisibleNudge()) waveEndNudge.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .component = &keepEndBtn });
}

void GuiComponentWaveHold::layoutComponentRow(juce::Rectangle<int>& rect)
{
    holdSeparator.layoutComponent(rect);

    layoutRow({ .rowRect = rect, .component = &holdEnableBtn });

    holdCount.layoutComponentRow(rect);

    if (holdCount.isVisibleNudge()) holdCountButtons.layoutComponentRow(rect);

    layoutRow({ .rowRect = rect, .label = &holdTarget.label, .component = &holdTarget });

    holdMin.layoutComponentRow(rect);

    if (holdMin.isVisibleNudge()) {
        if ((m_unit == WaveHoldUnit::Cent)) holdMinPitch.layoutComponentRow(rect);
        else          holdMinNudge.layoutComponentRow(rect);
    }

    holdMax.layoutComponentRow(rect);

    if (holdMax.isVisibleNudge()) {
        if ((m_unit == WaveHoldUnit::Cent)) holdMaxPitch.layoutComponentRow(rect);
        else          holdMaxNudge.layoutComponentRow(rect);
    }

    keepSeparator.layoutComponent(rect);

    layoutRow({ .rowRect = rect, .component = &keepEnableBtn });

    waveStart.layoutComponentRow(rect);

    if (waveStart.isVisibleNudge()) waveStartNudge.layoutComponentRow(rect);

    layoutRow({ .rowRect = rect, .component = &keepStartBtn });

    waveEnd.layoutComponentRow(rect);

    if (waveEnd.isVisibleNudge()) waveEndNudge.layoutComponentRow(rect);

    layoutRow({ .rowRect = rect, .component = &keepEndBtn });
}

void GuiComponentWaveHold::setEnables(bool enabled)
{
    m_enabled = enabled;

    applyEnables();
}

void GuiComponentWaveHold::setHoldAvailable(bool available)
{
    m_holdAvailable = available;

    applyEnables();
}

void GuiComponentWaveHold::applyEnables()
{
    // ホールドと部分再生は、それぞれの入り切りで別々に閉じる。
    // もう止まる形を選んでいるときは、ホールドは丸ごと閉じる。
    const bool enabled = m_enabled;
    const bool canHold = enabled && m_holdAvailable;
    const bool isHold = canHold && holdEnableBtn.getToggleState();
    const bool isKeep = enabled && keepEnableBtn.getToggleState();

    holdEnableBtn.setEnabled(canHold);
    holdCount.setEnabled(isHold);
    holdCountButtons.setEnables(isHold);
    holdTarget.setEnabledWithLabel(isHold);
    holdMin.setEnabled(isHold);
    holdMax.setEnabled(isHold);

    if ((m_unit == WaveHoldUnit::Cent)) {
        holdMinPitch.setEnables(isHold);
        holdMaxPitch.setEnables(isHold);
    }
    else {
        holdMinNudge.setEnables(isHold);
        holdMaxNudge.setEnables(isHold);
    }

    keepEnableBtn.setEnabled(enabled);
    waveStart.setEnabled(isKeep);
    waveStartNudge.setEnables(isKeep);
    keepStartBtn.setEnabled(isKeep);
    waveEnd.setEnabled(isKeep);
    waveEndNudge.setEnables(isKeep);
    keepEndBtn.setEnabled(isKeep);
}

// 名前で持つ形式だけを相手にする。行の並びで持つ旧形式は、
// 項目を足すと後ろが全部ずれるので増やさない。
void GuiComponentWaveHold::readParams(const Io::ParamReader& reader)
{
    holdEnableBtn.setToggleState(reader.getBool("holdEnable", holdEnableBtn.getToggleState()), juce::sendNotification);
    holdCount.setValue(reader.getInt("holdCount", holdCount.getValue()), juce::sendNotification);
    holdTarget.setSelectedItemIndex(reader.getInt("holdTarget", holdTarget.getSelectedItemIndex()), juce::sendNotification);
    holdMin.setValue(reader.getFloat("holdMin", (float)holdMin.getValue()), juce::sendNotification);
    holdMax.setValue(reader.getFloat("holdMax", (float)holdMax.getValue()), juce::sendNotification);

    keepEnableBtn.setToggleState(reader.getBool("keepEnable", keepEnableBtn.getToggleState()), juce::sendNotification);
    waveStart.setValue(reader.getFloat("waveStart", (float)waveStart.getValue()), juce::sendNotification);
    keepStartBtn.setToggleState(reader.getBool("keepStart", keepStartBtn.getToggleState()), juce::sendNotification);
    waveEnd.setValue(reader.getFloat("waveEnd", (float)waveEnd.getValue()), juce::sendNotification);
    keepEndBtn.setToggleState(reader.getBool("keepEnd", keepEndBtn.getToggleState()), juce::sendNotification);
}

void GuiComponentWaveHold::writeParams(Io::ParamWriter& writer)
{
    writer.set("holdEnable", holdEnableBtn.getToggleState());
    writer.set("holdCount", holdCount.getValue());
    writer.set("holdTarget", holdTarget.getSelectedItemIndex());
    writer.set("holdMin", (float)holdMin.getValue());
    writer.set("holdMax", (float)holdMax.getValue());

    writer.set("keepEnable", keepEnableBtn.getToggleState());
    writer.set("waveStart", (float)waveStart.getValue());
    writer.set("keepStart", keepStartBtn.getToggleState());
    writer.set("waveEnd", (float)waveEnd.getValue());
    writer.set("keepEnd", keepEndBtn.getToggleState());
}

// 画面へ出ていない組も含めて読み書きする。並べる名前は
// readParams / writeParams と同じにしてあるので、どちらで
// 書いたファイルも読める。
void GuiComponentWaveHold::readParamsFor(const juce::String& idPrefix, const Io::ParamReader& reader)
{
    setParamValue(idPrefix + CPK::WaveHold::holdEnable, reader.getBool("holdEnable", getParamValue(idPrefix + CPK::WaveHold::holdEnable) > CPV::boolThread) ? 1.0f : 0.0f);
    setParamValue(idPrefix + CPK::WaveHold::holdCount, (float)reader.getInt("holdCount", (int)getParamValue(idPrefix + CPK::WaveHold::holdCount)));
    setParamValue(idPrefix + CPK::WaveHold::holdTarget, (float)reader.getInt("holdTarget", (int)getParamValue(idPrefix + CPK::WaveHold::holdTarget)));
    setParamValue(idPrefix + CPK::WaveHold::holdMin, reader.getFloat("holdMin", getParamValue(idPrefix + CPK::WaveHold::holdMin)));
    setParamValue(idPrefix + CPK::WaveHold::holdMax, reader.getFloat("holdMax", getParamValue(idPrefix + CPK::WaveHold::holdMax)));

    setParamValue(idPrefix + CPK::WaveHold::keepEnable, reader.getBool("keepEnable", getParamValue(idPrefix + CPK::WaveHold::keepEnable) > CPV::boolThread) ? 1.0f : 0.0f);
    setParamValue(idPrefix + CPK::WaveHold::waveStart, reader.getFloat("waveStart", getParamValue(idPrefix + CPK::WaveHold::waveStart)));
    setParamValue(idPrefix + CPK::WaveHold::keepStart, reader.getBool("keepStart", getParamValue(idPrefix + CPK::WaveHold::keepStart) > CPV::boolThread) ? 1.0f : 0.0f);
    setParamValue(idPrefix + CPK::WaveHold::waveEnd, reader.getFloat("waveEnd", getParamValue(idPrefix + CPK::WaveHold::waveEnd)));
    setParamValue(idPrefix + CPK::WaveHold::keepEnd, reader.getBool("keepEnd", getParamValue(idPrefix + CPK::WaveHold::keepEnd) > CPV::boolThread) ? 1.0f : 0.0f);
}

void GuiComponentWaveHold::writeParamsFor(const juce::String& idPrefix, Io::ParamWriter& writer)
{
    writer.set("holdEnable", getParamValue(idPrefix + CPK::WaveHold::holdEnable) > CPV::boolThread);
    writer.set("holdCount", (int)getParamValue(idPrefix + CPK::WaveHold::holdCount));
    writer.set("holdTarget", (int)getParamValue(idPrefix + CPK::WaveHold::holdTarget));
    writer.set("holdMin", getParamValue(idPrefix + CPK::WaveHold::holdMin));
    writer.set("holdMax", getParamValue(idPrefix + CPK::WaveHold::holdMax));

    writer.set("keepEnable", getParamValue(idPrefix + CPK::WaveHold::keepEnable) > CPV::boolThread);
    writer.set("waveStart", getParamValue(idPrefix + CPK::WaveHold::waveStart));
    writer.set("keepStart", getParamValue(idPrefix + CPK::WaveHold::keepStart) > CPV::boolThread);
    writer.set("waveEnd", getParamValue(idPrefix + CPK::WaveHold::waveEnd));
    writer.set("keepEnd", getParamValue(idPrefix + CPK::WaveHold::keepEnd) > CPV::boolThread);
}

WaveHoldParams GuiComponentWaveHold::getParams()
{
    WaveHoldParams params;

    params.holdEnable = holdEnableBtn.getToggleState();
    params.holdCount = holdCount.getValue();
    params.holdTarget = holdTarget.getSelectedItemIndex();
    params.holdMin = (float)holdMin.getValue();
    params.holdMax = (float)holdMax.getValue();

    params.keepEnable = keepEnableBtn.getToggleState();
    params.waveStart = (float)waveStart.getValue();
    params.keepStart = keepStartBtn.getToggleState();
    params.waveEnd = (float)waveEnd.getValue();
    params.keepEnd = keepEndBtn.getToggleState();

    return params;
}
