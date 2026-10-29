#include "GuiGroupFolds.h"

#include "Shared/Core/Gui/GuiI18n.h"

namespace
{
    const juce::String separator = "|";
}

GuiGroupFolds::GuiGroupFolds(const GuiContext& context, const juce::String& stateKey) :
    openAllButton(context),
    closeAllButton(context),
    ctx(context),
    key(stateKey),
    keyId(stateKey)
{
    // apvts.state そのものへ付ける。プリセットを読むと中身が差し替わるが、
    // 付けた先は同じなので valueTreeRedirected で知らせが来る
    ctx.apvts.state.addListener(this);
}

GuiGroupFolds::~GuiGroupFolds()
{
    cancelPendingUpdate();

    ctx.apvts.state.removeListener(this);
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

    load();
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

    // 何も畳んでいなければ属性ごと消す。古いプリセットと同じ形になる
    if (names.isEmpty()) {
        ctx.apvts.state.removeProperty(key, nullptr);

        return;
    }

    ctx.apvts.state.setProperty(key, names.joinIntoString(separator), nullptr);
}

bool GuiGroupFolds::load()
{
    juce::StringArray names;

    names.addTokens(ctx.apvts.state.getProperty(key).toString(), separator, {});

    bool changed = false;

    for (auto& e : entries) {
        const bool folded = names.contains(e.name);

        if (e.group->isFolded() == folded) continue;

        e.group->setFolded(folded);

        changed = true;
    }

    return changed;
}

void GuiGroupFolds::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if (property == keyId) triggerAsyncUpdate();
}

void GuiGroupFolds::valueTreeRedirected(juce::ValueTree&)
{
    // DAW がほかのスレッドから state を差し替えることもあるので、画面は後で触る
    triggerAsyncUpdate();
}

void GuiGroupFolds::handleAsyncUpdate()
{
    if (load()) ctx.editor.resized();
}
