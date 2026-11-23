#include "GuiGroupFolds.h"

#include "Shared/Core/Gui/GuiI18n.h"
#include "Shared/Core/Gui/GuiChecklistMenu.h"

namespace
{
    const juce::String separator = "|";
}

GuiGroupFolds::GuiGroupFolds(const GuiContext& context, const juce::String& stateKey) :
    openAllButton(context),
    closeAllButton(context),
    ctx(context),
    key(stateKey)
{
    seenLoads = ctx.audioProcessor.stateLoads.load(std::memory_order_acquire);

    startTimer(pollMs);
}

GuiGroupFolds::~GuiGroupFolds()
{
    stopTimer();

    if (stripViewport != nullptr) stripViewport->removeMouseListener(this);

    // 出したままのポップアップが、消えたこの組を指さないように閉じる
    juce::PopupMenu::dismissAllActiveMenus();
}

void GuiGroupFolds::add(ColoredGroupComponent& group, const juce::String& name)
{
    entries.push_back({ &group, name.isNotEmpty() ? name : group.getText() });

    group.setFoldable(true);

    group.onTitleClick = [this, &group] {
        group.setFolded(!group.isFolded());

        save();

        ctx.editor.resized();
        };

    // 見出しを右クリック: 「グループを非表示」(4.5.0)。その下に区切り線を引き、
    // 枠の中の背景を右クリックしたときと同じ、区分の開閉のチェックリストを並べる
    group.onTitleMenu = [this, &group] {
        std::vector<GuiChecklistMenu::Action> leading{
            { I18n::pick(u8"グループを非表示", u8"Hide group"), [this, &group] {
                setHidden(group, true);

                saveHidden();

                ctx.editor.resized();
                } },
        };

        if (auto* scroll = dynamic_cast<GuiScrollGroup*>(&group)) {
            scroll->showCategoryMenu(std::move(leading));
            return;
        }

        GuiChecklistMenu::show({}, {}, {}, std::move(leading));
        };

    load();
    loadHidden();
}

void GuiGroupFolds::attachStrip(juce::Viewport& strip)
{
    if (stripViewport != nullptr) stripViewport->removeMouseListener(this);

    stripViewport = &strip;

    // 送り台は既定では押下を受けず、中身より右・下の空いたところを押すと親 (タブ) へ抜けてしまう。
    // 自分で受けて、そこを右クリックしてもチェックリストを出す
    strip.setInterceptsMouseClicks(true, true);

    // 中の部品の押下も受けて、どこを押したかで振り分ける
    strip.addMouseListener(this, true);
}

void GuiGroupFolds::mouseDown(const juce::MouseEvent& e)
{
    if (stripViewport == nullptr || !e.mods.isPopupMenu()) return;

    auto* origin = e.originalComponent;

    // 送り棒は帯の操作なので相手にしない
    if (dynamic_cast<juce::ScrollBar*>(origin) != nullptr || origin->findParentComponentOfClass<juce::ScrollBar>() != nullptr) return;

    auto* canvas = stripViewport->getViewedComponent();

    // 帯の送り台そのもの・中身のキャンバスそのものを押したときだけ。枠の中の部品は
    // それぞれが受ける (枠は押下を受けないので、枠の余白を右クリックするとキャンバスへ来る)
    const bool onBackground = origin == stripViewport.getComponent()
        || origin == canvas
        || (canvas != nullptr && origin == canvas->getParentComponent());

    if (!onBackground) return;

    // 枠の上 (中身の外の余白) なら、その枠の区分のチェックリスト
    if (canvas != nullptr) {
        const auto pos = canvas->getLocalPoint(origin, e.getPosition());

        for (auto& entry : entries) {
            auto* g = entry.group;

            if (!g->isVisible() || g->getParentComponent() != canvas || !g->getBounds().contains(pos)) continue;

            if (auto* scroll = dynamic_cast<GuiScrollGroup*>(g)) scroll->showCategoryMenu();

            return;
        }
    }

    showGroupMenu();
}

void GuiGroupFolds::showGroupMenu()
{
    std::vector<GuiChecklistMenu::Check> checks;

    for (auto& entry : entries) {
        auto* g = entry.group;

        checks.push_back({
            .name = g->getText(),
            .isOn = [g] { return !g->isGroupHidden(); },
            .set = [this, g](bool on) {
                setHidden(*g, !on);

                save();
                saveHidden();

                ctx.editor.resized();
                },
            });
    }

    GuiChecklistMenu::show({}, std::move(checks), {
        { I18n::pick(u8"すべてのグループを表示", u8"Show all groups"), [this] { setAllHidden(false); } },
        { I18n::pick(u8"すべてのグループを非表示", u8"Hide all groups"), [this] { setAllHidden(true); } },
        });
}

void GuiGroupFolds::setAllHidden(bool hidden)
{
    for (auto& e : entries) setHidden(*e.group, hidden);

    save();
    saveHidden();

    ctx.editor.resized();
}

void GuiGroupFolds::setHidden(ColoredGroupComponent& group, bool hidden)
{
    if (group.isGroupHidden() == hidden) return;

    group.setGroupHidden(hidden);

    // 出し直すときは畳んだ状態で出す
    if (!hidden) group.setFolded(true);
}

void GuiGroupFolds::saveHidden()
{
    juce::StringArray names;

    for (auto& e : entries) {
        if (e.group->isGroupHidden()) names.add(e.name);
    }

    seenHidden = names.joinIntoString(separator);

    ctx.audioProcessor.hiddenGroups[key] = seenHidden;
}

bool GuiGroupFolds::loadHidden()
{
    seenHidden = ctx.audioProcessor.hiddenGroups[key];

    juce::StringArray names;

    names.addTokens(seenHidden, separator, {});

    bool changed = false;

    for (auto& e : entries) {
        const bool hidden = names.contains(e.name);

        if (e.group->isGroupHidden() == hidden) continue;

        // 読み直しで出すものも畳んだ状態にそろえる
        setHidden(*e.group, hidden);

        changed = true;
    }

    return changed;
}

void GuiGroupFolds::setAllFolded(bool folded)
{
    for (auto& e : entries) e.group->setFolded(folded);

    save();

    ctx.editor.resized();
}

void GuiGroupFolds::setupButtons(juce::Component& parent, int& tabOrder)
{
    // 開くほうは濃い緑、閉じるほうは濃い青
    openAllButton.setup({ .parent = parent, .title = "<>", .textColor = GuiColor::GroupFoldBtn::Text, .textOnColor = GuiColor::GroupFoldBtn::Text, .bgColor = GuiColor::GroupFoldBtn::OpenBg });
    openAllButton.setTooltip(I18n::pick(u8"枠をすべて開く", u8"Open all groups"));
    openAllButton.setWantsKeyboardFocus(true);
    openAllButton.setExplicitFocusOrder(++tabOrder);
    openAllButton.onClick = [this] { setAllFolded(false); };

    closeAllButton.setup({ .parent = parent, .title = "><", .textColor = GuiColor::GroupFoldBtn::Text, .textOnColor = GuiColor::GroupFoldBtn::Text, .bgColor = GuiColor::GroupFoldBtn::CloseBg });
    closeAllButton.setTooltip(I18n::pick(u8"枠をすべて閉じる", u8"Close all groups"));
    closeAllButton.setWantsKeyboardFocus(true);
    closeAllButton.setExplicitFocusOrder(++tabOrder);
    closeAllButton.onClick = [this] { setAllFolded(true); };
}

void GuiGroupFolds::layoutButtons(juce::Rectangle<int>& row, int height)
{
    row.removeFromLeft(buttonGap);

    openAllButton.setBounds(row.removeFromLeft(buttonWidth).withHeight(height));
    closeAllButton.setBounds(row.removeFromLeft(buttonWidth).withHeight(height));
}

void GuiGroupFolds::save()
{
    juce::StringArray names;

    for (auto& e : entries) {
        if (e.group->isFolded()) names.add(e.name);
    }

    // 何も畳んでいなくても空の文字で書く。属性が無いと「既定 (全て畳む)」に読まれる
    ctx.apvts.state.setProperty(key, names.joinIntoString(separator), nullptr);
}

bool GuiGroupFolds::load()
{
    // 属性が無ければ (立ち上げたばかり・属性を持たないプリセット) 全て畳む
    const bool foldAll = !ctx.apvts.state.hasProperty(key);

    juce::StringArray names;

    names.addTokens(ctx.apvts.state.getProperty(key).toString(), separator, {});

    bool changed = false;

    for (auto& e : entries) {
        const bool folded = foldAll || names.contains(e.name);

        if (e.group->isFolded() == folded) continue;

        e.group->setFolded(folded);

        changed = true;
    }

    return changed;
}

void GuiGroupFolds::timerCallback()
{
    // 設定ファイルを読み直したら、非表示を当て直す (4.5.0)
    if (ctx.audioProcessor.hiddenGroups[key] != seenHidden && loadHidden()) ctx.editor.resized();

    const int loads = ctx.audioProcessor.stateLoads.load(std::memory_order_acquire);

    if (loads == seenLoads) return;

    seenLoads = loads;

    if (load()) ctx.editor.resized();
}
