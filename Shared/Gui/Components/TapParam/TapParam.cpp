#include "./TapParam.h"

#include "Shared/Core/Gui/GuiRefresh.h"
#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiParamEdit.h"
#include "Shared/Core/Editor/EditorParamBrowserText.h"
#include "Shared/Core/Const/ConstFileValues.h"

namespace
{
    const juce::String fileFormat = "tapParam";
}

// ---------------------------------------------------------------- 一覧
void GuiTapPairList::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();

    g.setColour(GuiColor::StepValues::Bg.get());
    g.fillRect(area);

    juce::Font font(juce::FontOptions(12.0f));

    g.setFont(font);

    for (int i = 0; i < (int)lines.size(); ++i) {
        auto row = juce::Rectangle<float>(area.getX(), area.getY() + (float)(i * rowHeight), area.getWidth(), (float)rowHeight);

        // いまの段は、行ごと地を変える (開始前は cursor が負で、どこも変えない)
        if (i == cursor) {
            g.setColour(GuiColor::StepValues::SelectedBg.get());
            g.fillRect(row);
        }

        const bool isSkip = i < (int)skips.size() && skips[(size_t)i];

        if (isSkip) g.setColour(i == selected ? GuiColor::StepValues::SkipSelected.get() : GuiColor::StepValues::Skip.get());
        else g.setColour(i == selected ? GuiColor::StepValues::Selected.get() : GuiColor::StepValues::Text.get());

        g.drawFittedText(lines[(size_t)i], row.reduced(3.0f, 0.0f).toNearestInt(), juce::Justification::centredLeft, 1, 0.6f);
    }

    g.setColour(GuiColor::StepValues::Frame.get());
    g.drawRect(area, 1.0f);
}

// ---------------------------------------------------------------- 区分
GuiComponentTapParam::GuiComponentTapParam(const GuiContext& context, const ParamModCatalog::Catalog& c, int ops) :
    GuiBase(context),
    catalog(c),
    opCount(c.op.empty() ? 0 : juce::jmax(0, ops)),
    cat(context),
    enable(context),
    enableSeparator(context),
    steps(context),
    stepsSeparator(context),
    loop(context),
    loops(context),
    loopSeparator(context),
    target(context),
    skipLabel(context),
    skip(context),
    skipSeparator(context),
    pairTarget(context),
    scope(context),
    param(context),
    value(context),
    valueMin(context),
    valueDown(context),
    valueUp(context),
    valueMax(context),
    addPair(context),
    deletePair(context),
    clearPairs(context),
    listSeparator(context),
    increment(context),
    decrement(context),
    clearStep(context)
{
}

StepTable* GuiComponentTapParam::table() const
{
    if (paramCode.isEmpty()) return nullptr;

    return ctx.audioProcessor.stepTables.find(paramCode + TapParam::spec().keys.base);
}

juce::RangedAudioParameter* GuiComponentTapParam::paramOf(int scopeIndex, int index) const
{
    const auto id = ParamModCatalog::paramId(paramCode, catalog, scopeIndex, index);

    return id.isEmpty() ? nullptr : ctx.apvts.getParameter(id);
}

juce::String GuiComponentTapParam::valueText(const StepPair& p) const
{
    auto* prm = paramOf(p.scope, p.index);

    return prm != nullptr ? prm->getText(juce::jlimit(0.0f, 1.0f, p.value), 0) : juce::String(p.value, 3);
}

double GuiComponentTapParam::stepOf(int scopeIndex, int index) const
{
    auto* p = paramOf(scopeIndex, index);

    const bool stepped = p != nullptr && (dynamic_cast<juce::AudioParameterInt*>(p) != nullptr
        || dynamic_cast<juce::AudioParameterChoice*>(p) != nullptr
        || dynamic_cast<juce::AudioParameterBool*>(p) != nullptr);

    if (!stepped) return 0.0;

    const auto range = p->getNormalisableRange();
    const int span = juce::roundToInt(range.end - range.start);

    return span > 0 ? 1.0 / (double)span : 1.0;
}

void GuiComponentTapParam::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder)
{
    paramCode = code;

    const auto& keys = TapParam::spec().keys;

    cat.setupSwCategory({ .parent = parent, .title = "TAP PARAM SEQUENCER", .enableChangeDetailVisible = true });

    enable.setup({ .parent = parent, .id = code + keys.enable, .title = "Enable", .isReset = true });
    enable.setWantsKeyboardFocus(true);
    enable.setExplicitFocusOrder(++tabOrder);
    enableSeparator.setupComponent(parent);

    steps.setup({ .parent = parent, .id = code + keys.steps, .title = "STEPS", .isReset = true, .labelFont = labelFont });
    steps.setWantsKeyboardFocus(true);
    steps.setExplicitFocusOrder(++tabOrder);
    steps.onValueChange = [this] { applyStepCount(); };
    stepsSeparator.setupComponent(parent);

    loop.setup({ .parent = parent, .id = code + keys.loop, .title = "LOOP", .isReset = true });
    loop.setWantsKeyboardFocus(true);
    loop.setExplicitFocusOrder(++tabOrder);

    loops.setupComponent(parent, tabOrder, labelFont,
        [this] { return table(); },
        [this] { return stepCount(); },
        [this] {
            refresh();
            relayoutIfNeeded();
        });

    loopSeparator.setupComponent(parent);

    target.setup({ .parent = parent, .title = "S.TARGET", .isReset = false, .labelFont = labelFont });
    target.setRange(1.0, 2.0, 1.0);
    target.setNumDecimalPlacesToDisplay(0);
    target.setWantsKeyboardFocus(true);
    target.setExplicitFocusOrder(++tabOrder);
    target.onValueChange = [this] { rebindValue(); };

    // S.TARGET の段で置き換えない (組を持ったまま休む)。立っているあいだはボタンを赤くして ON と出す
    skipLabel.setup({ .parent = parent, .title = "SKIP", .bgColor = GuiColor::Label::RowBg });
    skip.setup({ .parent = parent, .title = "OFF", .isReset = false });
    skip.setColour(juce::TextButton::buttonOnColourId, GuiColor::StepValues::Skip.get());
    skip.setWantsKeyboardFocus(true);
    skip.setExplicitFocusOrder(++tabOrder);
    skip.onClick = [this] { toggleSkip(); };
    skipSeparator.setupComponent(parent);

    // ---- 段の組。P.TARGET で組を選び、OP / PARAM / VALUE で中身を決める
    pairTarget.setup({ .parent = parent, .title = "P.TARGET", .isReset = false, .labelFont = labelFont });
    pairTarget.setRange(1.0, 2.0, 1.0);
    pairTarget.setNumDecimalPlacesToDisplay(0);
    pairTarget.setWantsKeyboardFocus(true);
    pairTarget.setExplicitFocusOrder(++tabOrder);
    pairTarget.onValueChange = [this] { rebindPair(); };

    // 0 がチャンネル (CH)、1 からがオペレーター (RHYTHM はパッド)
    scope.setup({ .parent = parent, .title = catalog.opLabel, .isReset = false, .labelFont = labelFont });
    scope.setRange(0.0, (double)juce::jmax(1, opCount), 1.0);
    scope.textFromValueFunction = [this](double v) {
        const int i = juce::roundToInt(v);

        return i <= 0 ? juce::String("CH") : catalog.opLabel + juce::String(i);
    };
    scope.valueFromTextFunction = [this](const juce::String& t) {
        return t.trim().equalsIgnoreCase("CH") ? 0.0 : (double)t.retainCharacters("0123456789").getIntValue();
    };
    scope.setWantsKeyboardFocus(true);
    scope.setExplicitFocusOrder(++tabOrder);
    scope.onValueChange = [this] {
        listParams(currentScope());
        writePair();
        rebindPair();
    };

    // RHYTHM のようにチャンネルの指し先が少なければ、初めはパッド 1 を指す
    scope.setValue(catalog.ch.size() <= 1 && opCount > 0 ? 1.0 : 0.0, juce::dontSendNotification);
    scope.updateText();

    static const std::vector<SelectItem> noItems;

    param.setup({ .parent = parent, .id = "", .title = "PARAM", .items = noItems, .isReset = false, .labelFont = labelFont });
    param.setWantsKeyboardFocus(true);
    param.setExplicitFocusOrder(++tabOrder);
    param.onChange = [this] {
        writePair();
        rebindPair();
    };

    listParams(currentScope());

    // 値は表にあるので、つまみはパラメータへ繋がない
    value.setupComponent(parent, "", "VALUE", tabOrder, std::nullopt, labelFont, false);
    value.getSlider().setRange(0.0, 1.0, 0.0);
    value.getSlider().onValueChange = [this] { writePair(); };

    auto setupButton = [this, &parent, &tabOrder](GuiTextButton& b, const juce::String& text, std::function<void()> fn) {
        b.setup({ .parent = parent, .title = text, .isReset = false, .darkenWhenPressed = true });
        b.setWantsKeyboardFocus(true);
        b.setExplicitFocusOrder(++tabOrder);
        b.onClick = std::move(fn);
    };

    // 両端と、指し先の 1 刻み
    setupButton(valueMin, "MIN", [this] { nudgeValue(0.0, true); });
    setupButton(valueDown, "-1", [this] { nudgeValue(-1.0, false); });
    setupButton(valueUp, "+1", [this] { nudgeValue(1.0, false); });
    setupButton(valueMax, "MAX", [this] { nudgeValue(1.0, true); });

    setupButton(addPair, "ADD PAIR", [this] { addCurrentPair(); });
    setupButton(deletePair, "DELETE PAIR", [this] { deleteCurrentPair(); });
    setupButton(clearPairs, "CLEAR PAIRS", [this] { clearCurrentPairs(); });

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

    // 開始前へ戻す (次に押すと 1 段目)
    clearStep.setup({ .parent = parent, .title = "CLEAR STEP", .isReset = false });
    clearStep.setWantsKeyboardFocus(true);
    clearStep.setExplicitFocusOrder(++tabOrder);
    clearStep.onClick = [this] { moveCursor(0, true); };

    // 区分の右クリックの COPY / PASTE で表も写す
    value.getSlider().getProperties().set(ParamEdit::Property::stepTable, code + keys.base);

    enable.watchToggle([this] { applyActive(); });
    loop.watchToggle([this] { applyActive(); });

    target.setValue(1, juce::dontSendNotification);
    pairTarget.setValue(1, juce::dontSendNotification);

    applyStepCount();
    applyActive();

    isReady = true;

    startTimerHz(10);
}

// PARAM に並べる名前を scope の一覧にする
void GuiComponentTapParam::listParams(int scopeIndex)
{
    if (scopeIndex == listedScope) return;

    listedScope = scopeIndex;

    const int before = param.getSelectedItemIndex();

    param.clear(juce::dontSendNotification);

    const auto& entries = scopeIndex < 0 ? catalog.ch : catalog.op;

    for (int i = 0; i < (int)entries.size(); ++i) param.addItem(entries[(size_t)i].label, i + 1);

    param.setSelectedItemIndex(juce::jlimit(0, juce::jmax(0, (int)entries.size() - 1), before), juce::dontSendNotification);
}

// ---------------------------------------------------------------- 段
void GuiComponentTapParam::applyStepCount()
{
    const int n = stepCount();
    const int before = list.getNaturalHeight();

    target.setRange(1.0, (double)juce::jmax(2, n), 1.0);

    if ((int)target.getValue() > n) target.setValue(n, juce::dontSendNotification);

    rebindValue();

    // 行の数が変わると区分の高さが変わるので、並べ直してもらう
    if (isReady && before != list.getNaturalHeight()) ctx.editor.resized();
    else relayoutIfNeeded();
}

void GuiComponentTapParam::relayoutIfNeeded()
{
    if (isReady && laidOutLoopRows > 0 && laidOutLoopRows != loops.getRowCount()) ctx.editor.resized();
}

// S.TARGET の段へ合わせる
void GuiComponentTapParam::rebindValue()
{
    auto* t = table();
    const int count = t != nullptr ? t->pairCount(currentStep()) : 0;

    pairTarget.setRange(1.0, (double)juce::jmax(2, count), 1.0);

    if ((int)pairTarget.getValue() > juce::jmax(1, count)) pairTarget.setValue(juce::jmax(1, count), juce::dontSendNotification);

    if (t != nullptr) showSkip(t->skipped(currentStep()));

    rebindPair();
}

// P.TARGET の組へ合わせる。組が無ければ、つまみは次に足す組の中身として残す
void GuiComponentTapParam::rebindPair()
{
    auto* t = table();
    const int step = currentStep();
    const int count = t != nullptr ? t->pairCount(step) : 0;

    {
        const juce::ScopedValueSetter<bool> loading(isLoadingValue, true);

        if (count > 0) {
            const auto p = t->pairAt(step, juce::jlimit(1, count, (int)pairTarget.getValue()) - 1);

            scope.setValue(juce::jlimit(0, opCount, p.scope + 1), juce::dontSendNotification);
            listParams(currentScope());
            param.setSelectedItemIndex(p.index, juce::dontSendNotification);
            value.getSlider().setValue(p.value, juce::dontSendNotification);
        }

        const int index = juce::jmax(0, param.getSelectedItemIndex());
        auto* prm = paramOf(currentScope(), index);

        // 値は指し先の書き方で出し、打ち込みも指し先の値で受ける
        value.getSlider().textFromValueFunction = [prm](double v) { return prm != nullptr ? prm->getText((float)juce::jlimit(0.0, 1.0, v), 0) : juce::String(v, 3); };
        value.getSlider().valueFromTextFunction = [prm](const juce::String& s) { return prm != nullptr ? (double)prm->getValueForText(s) : s.getDoubleValue(); };
        value.getSlider().setRange(0.0, 1.0, stepOf(currentScope(), index));
        value.getSlider().updateText();
        scope.updateText();
    }

    refresh();
}

// つまみで変えた中身を、P.TARGET の組へ書く。組を変えたら、その段の SKIP は外す
void GuiComponentTapParam::writePair()
{
    if (isLoadingValue) return;

    auto* t = table();

    if (t == nullptr) return;

    const int step = currentStep();
    auto pairs = t->pairList(step);

    if (pairs.empty()) {
        refresh();
        return;
    }

    const int at = juce::jlimit(1, (int)pairs.size(), (int)pairTarget.getValue()) - 1;

    pairs[(size_t)at] = { currentScope(), juce::jmax(0, param.getSelectedItemIndex()), (float)value.getSlider().getValue() };

    t->setPairs(step, pairs);

    if (t->skipped(step)) t->setSkip(step, false);

    showSkip(false);
    refresh();
}

// いまの OP / PARAM / VALUE の組を、段の後ろへ足して選ぶ
void GuiComponentTapParam::addCurrentPair()
{
    auto* t = table();

    if (t == nullptr) return;

    const int step = currentStep();
    auto pairs = t->pairList(step);

    if ((int)pairs.size() >= StepTable::maxPairs) return;

    pairs.push_back({ currentScope(), juce::jmax(0, param.getSelectedItemIndex()), (float)value.getSlider().getValue() });

    t->setPairs(step, pairs);

    pairTarget.setRange(1.0, (double)juce::jmax(2, (int)pairs.size()), 1.0);
    pairTarget.setValue((int)pairs.size(), juce::dontSendNotification);

    rebindValue();
}

void GuiComponentTapParam::deleteCurrentPair()
{
    auto* t = table();

    if (t == nullptr) return;

    const int step = currentStep();
    auto pairs = t->pairList(step);

    if (pairs.empty()) return;

    const int at = juce::jlimit(1, (int)pairs.size(), (int)pairTarget.getValue()) - 1;

    pairs.erase(pairs.begin() + at);

    t->setPairs(step, pairs);

    rebindValue();
}

void GuiComponentTapParam::clearCurrentPairs()
{
    if (auto* t = table()) t->setPairs(currentStep(), {});

    pairTarget.setValue(1, juce::dontSendNotification);

    rebindValue();
}

void GuiComponentTapParam::nudgeValue(double delta, bool set)
{
    auto& s = value.getSlider();
    const double step = stepOf(currentScope(), juce::jmax(0, param.getSelectedItemIndex()));

    // 実数の指し先は 0.001 ずつ
    s.setValue(juce::jlimit(0.0, 1.0, set ? delta : s.getValue() + delta * (step > 0.0 ? step : 0.001)));
}

void GuiComponentTapParam::toggleSkip()
{
    auto* t = table();

    if (t == nullptr) return;

    const int idx = currentStep();

    t->setSkip(idx, !t->skipped(idx));
    showSkip(t->skipped(idx));

    refresh();
}

void GuiComponentTapParam::showSkip(bool on)
{
    skip.setToggleState(on, juce::dontSendNotification);
    skip.setButtonText(on ? "ON" : "OFF");
}

// いまの段を動かす。音の側も同じ cursor を見る (TAP AMP / PITCH SEQUENCER と同じ)
void GuiComponentTapParam::moveCursor(int delta, bool toStart)
{
    auto* t = table();

    if (t == nullptr) return;

    if (toStart) {
        tapSeqRewind(*t);
        refresh();
        return;
    }

    const int n = stepCount();
    int c = t->cursor.load();

    if (delta > 0) {
        TapSeqParams p;

        p.enable = true;
        p.steps = n;
        p.loop = loop.getToggleState();
        p.table = t;

        c = tapSeqForward(p, c);
    }
    else {
        c = tapSeqBack(n, c);
    }

    t->cursor.store(c);

    refresh();
}

void GuiComponentTapParam::refresh()
{
    const int n = stepCount();
    const auto* t = table();

    list.lines.clear();
    list.skips.clear();

    for (int s = 1; s <= n; ++s) {
        const bool skipped = t != nullptr && t->skipped(s);

        juce::StringArray parts;

        if (t != nullptr) {
            for (const auto& p : t->pairList(s)) parts.add(ParamModCatalog::label(catalog, p.scope, p.index) + " " + valueText(p));
        }

        juce::String body = skipped ? juce::String("SKIP") : (parts.isEmpty() ? juce::String("-") : parts.joinIntoString(", "));

        list.lines.push_back("S" + juce::String(s) + ": " + body);
        list.skips.push_back(skipped);
    }

    list.selected = currentStep() - 1;

    // 開始前 (負) や、STEPS を減らして外へ出た段は、どの行も変えない
    const int c = t != nullptr ? t->cursor.load() : -1;

    list.cursor = (c >= 0 && c < n) ? c : -1;
    list.repaint();

    loops.refresh();

    if (t != nullptr) {
        seenVersion = t->version.load();
        seenCursor = t->cursor.load();
    }
}

// 表は外から書き換わる (読み込み・初期化・貼り付け)。いまの段は鍵を押すと進み、
// DAW の再生を止めると開始前へ戻る。
void GuiComponentTapParam::timerCallback()
{
    auto* t = table();

    if (t == nullptr) return;
    if (t->version.load() == seenVersion && t->cursor.load() == seenCursor) return;

    const int before = list.getNaturalHeight();

    rebindValue();

    if (before != list.getNaturalHeight()) ctx.editor.resized();
}

// ---------------------------------------------------------------- 並べる
void GuiComponentTapParam::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    const bool visible = cat.isDetailVisible();

    enable.setVisible(visible);
    enableSeparator.setVisible(visible);
    steps.setVisibleWithLabel(visible);
    stepsSeparator.setVisible(visible);
    loop.setVisible(visible);
    loops.setVisibles(visible);
    loopSeparator.setVisible(visible);
    target.setVisibleWithLabel(visible);
    skipLabel.setVisible(visible);
    skip.setVisible(visible);
    skipSeparator.setVisible(visible);
    pairTarget.setVisibleWithLabel(visible);
    scope.setVisibleWithLabel(visible && opCount > 0);
    param.setVisibleWithLabel(visible);
    value.setVisibleWithLabel(visible);
    valueMin.setVisible(visible);
    valueDown.setVisible(visible);
    valueUp.setVisible(visible);
    valueMax.setVisible(visible);
    addPair.setVisible(visible);
    deletePair.setVisible(visible);
    clearPairs.setVisible(visible);
    list.setVisible(visible);
    listSeparator.setVisible(visible);
    increment.setVisible(visible);
    decrement.setVisible(visible);
    clearStep.setVisible(visible);

    if (!visible) return;

    constexpr int h = 13;

    layoutMain({ .mainRect = rect, .component = &enable });
    enableSeparator.layoutComponent(rect);
    layoutMain({ .mainRect = rect, .label = &steps.label, .component = &steps, .rowHeight = h });
    stepsSeparator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .component = &loop, .rowHeight = h });
    loops.layoutComponent(rect, h);
    laidOutLoopRows = loops.getRowCount();
    loopSeparator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .label = &target.label, .component = &target, .rowHeight = h });
    layoutMain({ .mainRect = rect, .label = &skipLabel, .component = &skip, .rowHeight = h });
    skipSeparator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .label = &pairTarget.label, .component = &pairTarget, .rowHeight = h });

    if (opCount > 0) layoutMain({ .mainRect = rect, .label = &scope.label, .component = &scope, .rowHeight = h });

    layoutMain({ .mainRect = rect, .label = &param.label, .component = &param, .rowHeight = h });
    value.layoutComponent(rect, h);
    layoutMainFourComps({ .rect = rect, .comp1 = &valueMin, .comp2 = &valueDown, .comp3 = &valueUp, .comp4 = &valueMax, .rowHeight = h });
    layoutMainThreeComps({ .rect = rect, .comp1 = &addPair, .comp2 = &deletePair, .comp3 = &clearPairs, .rowHeight = h });

    list.setBounds(rect.removeFromTop(list.getNaturalHeight()));

    listSeparator.layoutComponent(rect);
    layoutMainThreeComps({ .rect = rect, .comp1 = &increment, .comp2 = &decrement, .comp3 = &clearStep, .rowHeight = h });

    rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

// ---------------------------------------------------------------- 止める
void GuiComponentTapParam::setEnabled(bool enabled)
{
    outerEnabled = enabled;

    cat.setEnabled(enabled);

    applyActive();
}

void GuiComponentTapParam::applyActive()
{
    const bool active = outerEnabled && enable.getToggleState();

    enable.setEnabled(outerEnabled);
    enableSeparator.setEnabled(active);
    steps.setEnabledWithLabel(active);
    stepsSeparator.setEnabled(active);

    // トグルは止めずに薄くする。止めると、そのあいだに来た値を捨ててしまう。
    loop.setEnabled(outerEnabled);
    loop.setDimmed(!active);
    loops.setEnables(active && loop.getToggleState());
    loopSeparator.setEnabled(active);

    target.setEnabledWithLabel(active);
    skipLabel.setEnabled(active);
    skip.setEnabled(active);
    skipSeparator.setEnabled(active);
    pairTarget.setEnabledWithLabel(active);
    scope.setEnabledWithLabel(active);
    param.setEnabledWithLabel(active);
    value.setEnabled(active);

    for (auto* b : { &valueMin, &valueDown, &valueUp, &valueMax, &addPair, &deletePair, &clearPairs, &increment, &decrement, &clearStep }) b->setEnabled(active);

    list.setEnabled(active);
    listSeparator.setEnabled(active);
}

// ---------------------------------------------------------------- ファイル
void GuiComponentTapParam::importParams()
{
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultSsgSwEnvParamDir,
        { EditorGuiText::ParamBrowser::kindTapParam },
        [this](const juce::File& file) { applyParamsFile(file); });
}

void GuiComponentTapParam::applyParamsFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;

    ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

    auto reader = Io::ParamReader::open(file, Io::ParamFormat{ fileFormat, 1 });

    if (!reader.has_value()) return;

    GuiRefresh::Batch batch;

    readValues(*reader);
}

void GuiComponentTapParam::exportParams()
{
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultSsgSwEnvParamDir,
        { EditorGuiText::ParamBrowser::kindTapParam },
        Io::Extension::TapParamParam,
        [this](const juce::File& file) { writeParamsFile(file); });
}

void GuiComponentTapParam::writeParamsFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(Io::ParamFormat{ fileFormat, 1 });

    writeValues(writer);

    writer.writeTo(file);
}

// 組は "段:scope:index:値:名前" を並べる。番号は音源で違うので、名前が合えば名前で選ぶ
void GuiComponentTapParam::readValues(const Io::ParamReader& r)
{
    enable.setToggleState(r.getBool("flag", enable.getToggleState()), juce::sendNotification);
    steps.setValue(r.getInt("steps", (int)steps.getValue()), juce::sendNotification);
    loop.setToggleState(r.getBool("loop", false), juce::sendNotification);

    if (auto* t = table()) {
        const auto flat = r.getFloatArray("loops");
        std::vector<StepLoop::Entry> entries;

        for (size_t i = 0; i + 2 < flat.size(); i += 3) entries.push_back({ (int)flat[i], (int)flat[i + 1], (int)flat[i + 2] });

        const auto skips = r.getFloatArray("skips");

        for (int s = 1; s <= StepTable::size; ++s) t->skips[(size_t)s - 1].store(false);
        for (const float s : skips) if ((int)s >= 1 && (int)s <= StepTable::size) t->skips[(size_t)s - 1].store(true);

        std::array<std::vector<StepPair>, StepTable::size> pairs;

        for (const auto& text : r.getStringArray("pairs")) {
            const auto v = juce::StringArray::fromTokens(text, ":", "");

            if (v.size() < 4) continue;

            const int s = v[0].getIntValue();

            if (s < 1 || s > StepTable::size) continue;

            StepPair p{ v[1].getIntValue(), v[2].getIntValue(), juce::jlimit(0.0f, 1.0f, v[3].getFloatValue()) };

            if (v.size() >= 5) {
                const auto& entries = p.scope < 0 ? catalog.ch : catalog.op;

                for (int i = 0; i < (int)entries.size(); ++i) {
                    if (entries[(size_t)i].label == v[4]) p.index = i;
                }
            }

            pairs[(size_t)s - 1].push_back(p);
        }

        for (int s = 1; s <= StepTable::size; ++s) t->setPairs(s, pairs[(size_t)s - 1]);

        t->setLoops(entries);
    }

    applyStepCount();
}

void GuiComponentTapParam::writeValues(Io::ParamWriter& w)
{
    w.set("flag", enable.getToggleState());
    w.set("steps", (float)steps.getValue());
    w.set("loop", loop.getToggleState());

    std::vector<float> flat;
    std::vector<float> skips;
    std::vector<juce::String> pairs;

    if (auto* t = table()) {
        for (const auto& e : t->loopList()) {
            flat.push_back((float)e.start);
            flat.push_back((float)e.end);
            flat.push_back((float)e.count);
        }

        for (int s = 1; s <= StepTable::size; ++s) {
            if (t->skipped(s)) skips.push_back((float)s);

            for (const auto& p : t->pairList(s)) {
                const auto& entries = p.scope < 0 ? catalog.ch : catalog.op;
                const auto name = p.index >= 0 && p.index < (int)entries.size() ? entries[(size_t)p.index].label : juce::String();

                pairs.push_back(juce::String(s) + ":" + juce::String(p.scope) + ":" + juce::String(p.index) + ":" + juce::String(p.value, 6) + ":" + name);
            }
        }
    }

    w.setArray("loops", flat);
    w.setArray("skips", skips);
    w.setArray("pairs", pairs);
}

void GuiComponentTapParam::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    if (r.keys().isEmpty()) return;

    readValues(r);
}

void GuiComponentTapParam::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    writeValues(w);
}
