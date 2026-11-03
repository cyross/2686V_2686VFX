#include "./StepLoops.h"

#include "Shared/Core/Gui/GuiHelpers.h"

// ---------------------------------------------------------------- 一覧
void GuiLoopList::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();

    g.setColour(GuiColor::StepValues::Bg.get());
    g.fillRect(area);

    const int rows = getRowCount();
    const float rowH = area.getHeight() / (float)juce::jmax(1, rows);
    const float colW = area.getWidth() / 4.0f;

    juce::Font font(juce::FontOptions(juce::jmin(13.0f, rowH * 0.8f)));

    g.setFont(font);

    auto cellOf = [&](int row, int col) {
        return juce::Rectangle<float>(area.getX() + colW * (float)col, area.getY() + rowH * (float)row, colW, rowH).reduced(2.0f, 0.0f);
    };

    // 見出し
    const char* heads[] = { "No", "START", "END", "COUNT" };

    g.setColour(GuiColor::StepValues::DimText.get());

    for (int c = 0; c < 4; ++c) g.drawFittedText(heads[c], cellOf(0, c).toNearestInt(), juce::Justification::centred, 1, 0.7f);

    for (int i = 0; i < (int)entries.size(); ++i) {
        const auto& e = entries[(size_t)i];
        const int row = i + 1;
        const bool isSelected = i == selected;
        const bool isActive = i < (int)active.size() && active[(size_t)i];
        const bool isTrapped = i < (int)trapped.size() && trapped[(size_t)i];

        if (isSelected) {
            g.setColour(GuiColor::StepValues::SelectedBg.get());
            g.fillRect(juce::Rectangle<float>(area.getX(), area.getY() + rowH * (float)row, area.getWidth(), rowH).reduced(0.5f));
        }

        const auto text = isSelected ? GuiColor::StepValues::Selected.get()
                                     : (isActive ? GuiColor::StepValues::Text.get() : GuiColor::StepValues::DimText.get());

        const juce::String cells[] = { "L" + juce::String(i + 1), juce::String(e.start), juce::String(e.end),
                                       e.count == 0 ? juce::String("INF") : juce::String(e.count) };

        for (int c = 0; c < 4; ++c) {
            g.setColour(c == 3 && isTrapped ? GuiColor::StepValues::Warning.get() : text);
            g.drawFittedText((c == 3 && isTrapped ? "! " : "") + cells[c], cellOf(row, c).toNearestInt(), juce::Justification::centred, 1, 0.7f);
        }
    }

    // 列の区切り
    g.setColour(GuiColor::StepValues::Frame.get().withMultipliedAlpha(0.5f));

    for (int c = 1; c < 4; ++c) g.drawVerticalLine((int)(area.getX() + colW * (float)c), area.getY(), area.getBottom());

    g.drawHorizontalLine((int)(area.getY() + rowH), area.getX(), area.getRight());

    g.setColour(GuiColor::StepValues::Frame.get());
    g.drawRect(area, 1.0f);
}

void GuiLoopList::mouseDown(const juce::MouseEvent& e)
{
    if (!isEnabled() || e.mods.isRightButtonDown()) return;

    const int row = e.y / rowHeight - 1;

    if (row >= 0 && row < (int)entries.size() && onSelect) onSelect(row);
}

// ---------------------------------------------------------------- 組み立て
GuiComponentStepLoops::GuiComponentStepLoops(const GuiContext& context) :
    GuiBase(context),
    target(context),
    start(context),
    end(context),
    count(context),
    add(context),
    remove(context)
{
}

void GuiComponentStepLoops::setupComponent(juce::Component& parent, int& tabOrder, const juce::Font& labelFont,
    std::function<StepTable*()> table, std::function<int()> steps, std::function<void()> changed)
{
    tableOf = std::move(table);
    stepsOf = std::move(steps);
    onChanged = std::move(changed);

    // 値は表にあるので、つまみはパラメータへ繋がない
    target.setup({ .parent = parent, .title = "LP.TARGET", .isReset = false, .labelFont = labelFont });
    target.setRange(1.0, 2.0, 1.0);
    target.setNumDecimalPlacesToDisplay(0);
    target.setWantsKeyboardFocus(true);
    target.setExplicitFocusOrder(++tabOrder);
    target.onValueChange = [this] { if (!isLoading) refresh(); };

    start.setup({ .parent = parent, .title = "START", .isReset = false, .labelFont = labelFont });
    start.setRange(1.0, 2.0, 1.0);
    start.setNumDecimalPlacesToDisplay(0);
    start.setWantsKeyboardFocus(true);
    start.setExplicitFocusOrder(++tabOrder);
    start.onValueChange = [this] { onEdited(Edited::Start); };

    end.setup({ .parent = parent, .title = "END", .isReset = false, .labelFont = labelFont });
    end.setRange(1.0, 2.0, 1.0);
    end.setNumDecimalPlacesToDisplay(0);
    end.setWantsKeyboardFocus(true);
    end.setExplicitFocusOrder(++tabOrder);
    end.onValueChange = [this] { onEdited(Edited::End); };

    count.setup({ .parent = parent, .title = "COUNT", .isReset = false, .labelFont = labelFont });
    count.setRange((double)StepLoop::countMin, (double)StepLoop::countMax, 1.0);
    count.setNumDecimalPlacesToDisplay(0);
    count.setWantsKeyboardFocus(true);
    count.setExplicitFocusOrder(++tabOrder);
    count.onValueChange = [this] { onEdited(Edited::Count); };

    add.setup({ .parent = parent, .title = "ADD", .isReset = false });
    add.setWantsKeyboardFocus(true);
    add.setExplicitFocusOrder(++tabOrder);
    add.onClick = [this] { addEntry(); };

    remove.setup({ .parent = parent, .title = "DELETE", .isReset = false });
    remove.setWantsKeyboardFocus(true);
    remove.setExplicitFocusOrder(++tabOrder);
    remove.onClick = [this] { removeEntry(); };

    list.onSelect = [this](int index) { select(index); };
    parent.addAndMakeVisible(list);

    refresh();
}

// ---------------------------------------------------------------- 値
int GuiComponentStepLoops::entryCount() const
{
    const auto* t = tableOf ? tableOf() : nullptr;

    return t != nullptr ? t->loopCount() : 0;
}

int GuiComponentStepLoops::stepCount() const
{
    return juce::jlimit(1, StepTable::size, stepsOf ? stepsOf() : 1);
}

int GuiComponentStepLoops::selectedIndex() const
{
    const int n = entryCount();

    return n > 0 ? juce::jlimit(0, n - 1, (int)target.getValue() - 1) : -1;
}

void GuiComponentStepLoops::select(int index)
{
    {
        const juce::ScopedValueSetter<bool> loading(isLoading, true);

        target.setValue(index + 1, juce::dontSendNotification);
    }

    refresh();
}

// 表から、つまみの範囲と値と一覧を作り直す
void GuiComponentStepLoops::refresh()
{
    const juce::ScopedValueSetter<bool> loading(isLoading, true);

    const auto* t = tableOf ? tableOf() : nullptr;
    const auto entries = t != nullptr ? t->loopList() : std::vector<StepLoop::Entry>{};
    const int n = (int)entries.size();
    const int steps = stepCount();

    // 範囲は幅を持たせる必要があるので、1 つのときも 1〜2 にして先で丸める
    target.setRange(1.0, (double)juce::jmax(2, n), 1.0);

    if ((int)target.getValue() > juce::jmax(1, n)) target.setValue(juce::jmax(1, n), juce::dontSendNotification);

    // STEPS より後ろへかかる組もそのまま出せるよう、範囲は今の値も含める
    const int idx = selectedIndex();
    const auto e = idx >= 0 ? entries[(size_t)idx] : StepLoop::Entry{};
    const int stepsRange = juce::jmax(2, steps, e.start, e.end);

    start.setRange(1.0, (double)stepsRange, 1.0);
    end.setRange(1.0, (double)stepsRange, 1.0);

    start.setValue(e.start, juce::dontSendNotification);
    end.setValue(e.end, juce::dontSendNotification);
    count.setValue(e.count, juce::dontSendNotification);

    list.entries = entries;
    list.active.assign((size_t)n, false);
    list.trapped.assign((size_t)n, false);

    for (int i = 0; i < n; ++i) {
        list.active[(size_t)i] = StepLoop::isValid(entries[(size_t)i], steps);
        list.trapped[(size_t)i] = StepLoop::isTrapped(entries, n, steps, i);
    }

    list.selected = idx;
    list.repaint();

    applyEnables();
}

// 動かした値を表へ書く。START と END が入れ違ったら、動かしていない側を押す
void GuiComponentStepLoops::onEdited(Edited which)
{
    if (isLoading) return;

    auto* t = tableOf ? tableOf() : nullptr;
    const int idx = selectedIndex();

    if (t == nullptr || idx < 0) return;

    auto entries = t->loopList();
    auto e = entries[(size_t)idx];

    e.start = (int)start.getValue();
    e.end = (int)end.getValue();
    e.count = (int)count.getValue();

    if (which == Edited::Start && e.start > e.end) e.end = e.start;
    if (which == Edited::End && e.end < e.start) e.start = e.end;

    entries[(size_t)idx] = e;

    t->setLoops(entries);

    // 並び直したあとも、動かした組を指し続ける
    const auto sorted = t->loopList();
    const auto it = std::find(sorted.begin(), sorted.end(), e);

    select(it != sorted.end() ? (int)(it - sorted.begin()) : idx);

    if (onChanged) onChanged();
}

// 段の並び全体を 1 回戻る組を足す
void GuiComponentStepLoops::addEntry()
{
    auto* t = tableOf ? tableOf() : nullptr;

    if (t == nullptr) return;

    auto entries = t->loopList();

    if ((int)entries.size() >= StepLoop::maxEntries) return;

    const StepLoop::Entry e{ 1, stepCount(), StepLoop::countInitial };

    entries.push_back(e);
    t->setLoops(entries);

    const auto sorted = t->loopList();

    // 同じ組があれば並びの後ろへ入るので、後ろから探す
    int found = (int)sorted.size() - 1;

    for (int i = (int)sorted.size() - 1; i >= 0; --i) {
        if (sorted[(size_t)i] == e) {
            found = i;
            break;
        }
    }

    select(found);

    if (onChanged) onChanged();
}

void GuiComponentStepLoops::removeEntry()
{
    auto* t = tableOf ? tableOf() : nullptr;
    const int idx = selectedIndex();

    if (t == nullptr || idx < 0) return;

    auto entries = t->loopList();

    entries.erase(entries.begin() + idx);
    t->setLoops(entries);

    select(juce::jmax(0, idx - 1));

    if (onChanged) onChanged();
}

// ---------------------------------------------------------------- 並べる
void GuiComponentStepLoops::layoutComponent(juce::Rectangle<int>& rect, int rowHeight)
{
    layoutMain({ .mainRect = rect, .label = &target.label, .component = &target, .rowHeight = rowHeight });
    layoutMain({ .mainRect = rect, .label = &start.label, .component = &start, .rowHeight = rowHeight });
    layoutMain({ .mainRect = rect, .label = &end.label, .component = &end, .rowHeight = rowHeight });
    layoutMain({ .mainRect = rect, .label = &count.label, .component = &count, .rowHeight = rowHeight });
    layoutMainTwoComps({ .rect = rect, .comp1 = &add, .comp2 = &remove, .rowHeight = rowHeight });

    list.setBounds(rect.removeFromTop(list.getNaturalHeight()));
}

void GuiComponentStepLoops::setVisibles(bool visible)
{
    target.setVisibleWithLabel(visible);
    start.setVisibleWithLabel(visible);
    end.setVisibleWithLabel(visible);
    count.setVisibleWithLabel(visible);
    add.setVisible(visible);
    remove.setVisible(visible);
    list.setVisible(visible);
}

void GuiComponentStepLoops::setEnables(bool enabled)
{
    outerEnabled = enabled;

    applyEnables();
}

// 組が無いあいだは、組を足すボタンのほかは押せない。組が 1 つなら選ぶものが無い
void GuiComponentStepLoops::applyEnables()
{
    const bool has = entryCount() > 0;

    target.setEnabledWithLabel(outerEnabled && entryCount() > 1);
    start.setEnabledWithLabel(outerEnabled && has);
    end.setEnabledWithLabel(outerEnabled && has);
    count.setEnabledWithLabel(outerEnabled && has);
    add.setEnabled(outerEnabled && entryCount() < StepLoop::maxEntries);
    remove.setEnabled(outerEnabled && has);
    list.setEnabled(outerEnabled);
}
