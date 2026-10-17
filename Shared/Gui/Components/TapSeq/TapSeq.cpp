#include "./TapSeq.h"

#include "Shared/Core/Gui/GuiRefresh.h"
#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiParamEdit.h"
#include "Shared/Core/Editor/EditorParamBrowserText.h"
#include "Shared/Core/Const/ConstFileValues.h"

// ---------------------------------------------------------------- 棒
int GuiTapBars::getNaturalHeight() const
{
    const int n = juce::jmax(1, (int)values.size());

    // 1 本は 2〜10 px。全体はおよそ 160 px に収める
    const int row = juce::jlimit(2, 10, 160 / n);

    return row * n;
}

void GuiTapBars::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();

    g.setColour(GuiColor::StepValues::Bg.get());
    g.fillRect(area);

    const int n = (int)values.size();

    if (n > 0 && maxValue > minValue) {
        const float rowH = area.getHeight() / (float)n;
        const float range = maxValue - minValue;

        auto xOf = [&](float v) {
            return area.getX() + area.getWidth() * (std::clamp(v, minValue, maxValue) - minValue) / range;
        };

        const float x0 = xOf(zero);

        for (int i = 0; i < n; ++i) {
            const float y = area.getY() + rowH * (float)i;
            const float x1 = xOf(values[(size_t)i]);

            auto bar = juce::Rectangle<float>(std::min(x0, x1), y, std::abs(x1 - x0), rowH).reduced(0.0f, rowH > 3.0f ? 0.5f : 0.0f);

            if (i == cursor) {
                // 次に押したときに使う段は、行ごと地を変える
                g.setColour(GuiColor::StepValues::SelectedBg.get());
                g.fillRect(juce::Rectangle<float>(area.getX(), y, area.getWidth(), rowH));
            }

            g.setColour(i == selected ? GuiColor::StepValues::Selected.get() : GuiColor::StepValues::Text.get());
            g.fillRect(bar);
        }

        // 0 の線
        g.setColour(GuiColor::StepValues::DimText.get());
        g.drawVerticalLine((int)x0, area.getY(), area.getBottom());
    }

    g.setColour(GuiColor::StepValues::Frame.get());
    g.drawRect(area, 1.0f);
}

// ---------------------------------------------------------------- 区分
GuiComponentTapSeq::GuiComponentTapSeq(const GuiContext& context, Kind k) :
    GuiBase(context),
    kind(k),
    spec(k == Kind::Amp ? TapSeq::amp() : TapSeq::pitch()),
    cat(context),
    enable(context),
    enableSeparator(context),
    steps(context),
    stepsSeparator(context),
    target(context),
    value(context),
    ampBtns(context),
    pitchBtns(context),
    listSeparator(context),
    increment(context),
    decrement(context),
    reset(context)
{
}

StepTable* GuiComponentTapSeq::table() const
{
    if (paramCode.isEmpty()) return nullptr;

    return ctx.audioProcessor.stepTables.find(paramCode + spec.keys.base);
}

void GuiComponentTapSeq::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder)
{
    paramCode = code;

    const GuiCategoryLabel::Config catConfig{ .parent = parent, .title = isAmp() ? "TAP AMP SEQUENCER" : "TAP PITCH SEQUENCER", .enableChangeDetailVisible = true };

    if (isAmp()) cat.setupSwAmpCategory(catConfig);
    else cat.setupSwPitchCategory(catConfig);

    enable.setup({ .parent = parent, .id = code + spec.keys.enable, .title = "Enable", .isReset = true });
    enable.setWantsKeyboardFocus(true);
    enable.setExplicitFocusOrder(++tabOrder);
    enableSeparator.setupComponent(parent);

    steps.setup({ .parent = parent, .id = code + spec.keys.steps, .title = "STEPS", .isReset = true, .labelFont = labelFont });
    steps.setWantsKeyboardFocus(true);
    steps.setExplicitFocusOrder(++tabOrder);
    steps.onValueChange = [this] { applyStepCount(); };
    stepsSeparator.setupComponent(parent);

    target.setup({ .parent = parent, .title = "S.TARGET", .isReset = false, .labelFont = labelFont });
    target.setRange(1.0, 2.0, 1.0);
    target.setNumDecimalPlacesToDisplay(0);
    target.setWantsKeyboardFocus(true);
    target.setExplicitFocusOrder(++tabOrder);
    target.onValueChange = [this] { rebindValue(); };

    // 値は表にあるので、つまみはパラメータへ繋がない
    value.setupComponent(parent, "", isAmp() ? "RATIO" : "CENT", tabOrder, std::nullopt, labelFont, false);
    value.getSlider().setRange(spec.valueMin, spec.valueMax, spec.integer ? 1.0 : 0.001);
    value.getSlider().setNumDecimalPlacesToDisplay(spec.integer ? 0 : 3);
    value.getSlider().onValueChange = [this] { onValueChanged(); };

    if (isAmp()) ampBtns.setupComponent(parent, value.getSlider(), tabOrder, labelFont);
    else pitchBtns.setupComponent(parent, value.getSlider(), tabOrder, labelFont);

    bars.minValue = spec.valueMin;
    bars.maxValue = spec.valueMax;
    bars.zero = isAmp() ? 0.0f : 0.0f;
    parent.addAndMakeVisible(bars);
    parent.addAndMakeVisible(list);
    listSeparator.setupComponent(parent);

    increment.setup({ .parent = parent, .title = "INCREMENT", .isReset = false });
    increment.setWantsKeyboardFocus(true);
    increment.setExplicitFocusOrder(++tabOrder);
    increment.onClick = [this] { moveCursor(1, false); };

    decrement.setup({ .parent = parent, .title = "DECREMENT", .isReset = false });
    decrement.setWantsKeyboardFocus(true);
    decrement.setExplicitFocusOrder(++tabOrder);
    decrement.onClick = [this] { moveCursor(-1, false); };

    reset.setup({ .parent = parent, .title = "RESET", .isReset = false });
    reset.setWantsKeyboardFocus(true);
    reset.setExplicitFocusOrder(++tabOrder);
    reset.onClick = [this] { moveCursor(0, true); };

    // 区分の右クリックの COPY / PASTE で表も写す
    value.getSlider().getProperties().set(ParamEdit::Property::stepTable, code + spec.keys.base);

    enable.watchToggle([this] { applyActive(); });

    target.setValue(1, juce::dontSendNotification);

    applyStepCount();
    applyActive();

    isReady = true;

    startTimerHz(10);
}

// ---------------------------------------------------------------- 段
void GuiComponentTapSeq::applyStepCount()
{
    const int n = stepCount();
    const int rowsBefore = list.getRowCount();
    const int barsBefore = bars.getNaturalHeight();

    target.setRange(1.0, (double)juce::jmax(2, n), 1.0);

    if ((int)target.getValue() > n) target.setValue(n, juce::dontSendNotification);

    rebindValue();

    // 行の数や棒の高さが変わると区分の高さが変わるので、並べ直してもらう
    if (isReady && (rowsBefore != list.getRowCount() || barsBefore != bars.getNaturalHeight())) ctx.editor.resized();
}

void GuiComponentTapSeq::rebindValue()
{
    const int idx = juce::jlimit(1, stepCount(), (int)target.getValue());

    if (auto* t = table()) {
        const juce::ScopedValueSetter<bool> loading(isLoadingValue, true);

        value.getSlider().setValue(t->level(idx), juce::dontSendNotification);
    }

    refresh();
}

void GuiComponentTapSeq::onValueChanged()
{
    if (isLoadingValue) return;

    if (auto* t = table()) t->setLevel(juce::jlimit(1, stepCount(), (int)target.getValue()), (float)value.getSlider().getValue());

    refresh();
}

// 次に使う段を動かす。音の側も同じ cursor を見る
void GuiComponentTapSeq::moveCursor(int delta, bool toStart)
{
    auto* t = table();

    if (t == nullptr) return;

    const int n = stepCount();
    int c = juce::jlimit(0, n - 1, t->cursor.load());

    c = toStart ? 0 : (c + delta + n) % n;

    t->cursor.store(c);

    refresh();
}

void GuiComponentTapSeq::refresh()
{
    const int n = stepCount();
    const auto* t = table();

    bars.values.clear();
    list.labels.clear();
    list.values.clear();

    for (int s = 1; s <= n; ++s) {
        const float v = t != nullptr ? t->level(s) : spec.valueInitial;

        bars.values.push_back(v);
        list.labels.push_back("S" + juce::String(s));
        list.values.push_back(v);
    }

    bars.selected = juce::jlimit(1, n, (int)target.getValue()) - 1;
    bars.cursor = t != nullptr ? juce::jlimit(0, n - 1, t->cursor.load()) : -1;
    bars.repaint();

    list.selected = bars.selected;
    list.activeCount = -1;
    list.decimals = spec.integer ? 0 : 3;
    list.repaint();

    if (t != nullptr) {
        seenVersion = t->version.load();
        seenCursor = t->cursor.load();
    }
}

// 表は外から書き換わる (読み込み・初期化・貼り付け)。次に使う段は鍵を押すと進む。
void GuiComponentTapSeq::timerCallback()
{
    auto* t = table();

    if (t == nullptr) return;
    if (t->version.load() == seenVersion && t->cursor.load() == seenCursor) return;

    rebindValue();
}

// ---------------------------------------------------------------- 並べる
void GuiComponentTapSeq::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    const bool visible = cat.isDetailVisible();

    enable.setVisible(visible);
    enableSeparator.setVisible(visible);
    steps.setVisibleWithLabel(visible);
    stepsSeparator.setVisible(visible);
    target.setVisibleWithLabel(visible);
    value.setVisibleWithLabel(visible);
    ampBtns.setVisibles(visible && isAmp() && value.isVisibleNudge());
    pitchBtns.setVisibles(visible && !isAmp() && value.isVisibleNudge());
    bars.setVisible(visible);
    list.setVisible(visible);
    listSeparator.setVisible(visible);
    increment.setVisible(visible);
    decrement.setVisible(visible);
    reset.setVisible(visible);

    if (!visible) return;

    constexpr int h = 13;

    layoutMain({ .mainRect = rect, .component = &enable });
    enableSeparator.layoutComponent(rect);
    layoutMain({ .mainRect = rect, .label = &steps.label, .component = &steps, .rowHeight = h });
    stepsSeparator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .label = &target.label, .component = &target, .rowHeight = h });
    value.layoutComponent(rect, h);

    if (value.isVisibleNudge()) {
        if (isAmp()) ampBtns.layoutComponent(rect, h);
        else pitchBtns.layoutComponent(rect, h);
    }

    bars.setBounds(rect.removeFromTop(bars.getNaturalHeight()));
    rect.removeFromTop(2);
    list.setBounds(rect.removeFromTop(list.getNaturalHeight()));

    listSeparator.layoutComponent(rect);
    layoutMainThreeComps({ .rect = rect, .comp1 = &increment, .comp2 = &decrement, .comp3 = &reset, .rowHeight = h });

    rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

// ---------------------------------------------------------------- 止める
void GuiComponentTapSeq::setEnabled(bool enabled)
{
    outerEnabled = enabled;

    cat.setEnabled(enabled);

    applyActive();
}

void GuiComponentTapSeq::applyActive()
{
    const bool active = outerEnabled && enable.getToggleState();

    enable.setEnabled(outerEnabled);
    enableSeparator.setEnabled(active);
    steps.setEnabledWithLabel(active);
    stepsSeparator.setEnabled(active);
    target.setEnabledWithLabel(active);
    value.setEnabled(active);
    ampBtns.setEnables(active);
    pitchBtns.setEnables(active);
    bars.setEnabled(active);
    list.setEnabled(active);
    listSeparator.setEnabled(active);
    increment.setEnabled(active);
    decrement.setEnabled(active);
    reset.setEnabled(active);
}

// ---------------------------------------------------------------- ファイル
void GuiComponentTapSeq::importParams()
{
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultSsgSwEnvParamDir,
        { isAmp() ? EditorGuiText::ParamBrowser::kindTapAmp : EditorGuiText::ParamBrowser::kindTapPitch },
        [this](const juce::File& file) { applyParamsFile(file); });
}

void GuiComponentTapSeq::applyParamsFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;

    ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

    auto reader = Io::ParamReader::open(file, Io::ParamFormat{ isAmp() ? "tapAmp" : "tapPitch", 1 });

    if (!reader.has_value()) return;

    GuiRefresh::Batch batch;

    readValues(*reader);
}

void GuiComponentTapSeq::exportParams()
{
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultSsgSwEnvParamDir,
        { isAmp() ? EditorGuiText::ParamBrowser::kindTapAmp : EditorGuiText::ParamBrowser::kindTapPitch },
        isAmp() ? Io::Extension::TapAmpParam : Io::Extension::TapPitchParam,
        [this](const juce::File& file) { writeParamsFile(file); });
}

void GuiComponentTapSeq::writeParamsFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(Io::ParamFormat{ isAmp() ? "tapAmp" : "tapPitch", 1 });

    writeValues(writer);

    writer.writeTo(file);
}

void GuiComponentTapSeq::readValues(const Io::ParamReader& r)
{
    enable.setToggleState(r.getBool("flag", enable.getToggleState()), juce::sendNotification);
    steps.setValue(r.getInt("steps", (int)steps.getValue()), juce::sendNotification);

    if (auto* t = table()) {
        const auto values = r.getFloatArray("values");

        for (int s = 1; s <= StepTable::size; ++s) t->setLevel(s, s <= (int)values.size() ? values[(size_t)s - 1] : t->defaultL);
    }

    applyStepCount();
}

void GuiComponentTapSeq::writeValues(Io::ParamWriter& w)
{
    w.set("flag", enable.getToggleState());
    w.set("steps", (float)steps.getValue());

    // 後ろの初期値のままの段は書かない
    std::vector<float> values;

    if (auto* t = table()) {
        int last = StepTable::size;

        while (last > 0 && t->level(last) == t->defaultL) --last;

        for (int s = 1; s <= last; ++s) values.push_back(t->level(s));
    }

    w.setArray("values", values);
}

void GuiComponentTapSeq::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    if (r.keys().isEmpty()) return;

    readValues(r);
}

void GuiComponentTapSeq::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    writeValues(w);
}
