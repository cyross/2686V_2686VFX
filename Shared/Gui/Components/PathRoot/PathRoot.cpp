#include "./PathRoot.h"

#include <algorithm>
#include <vector>

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiI18n.h"
#include "Shared/Core/Io/IoPathRoot.h"

namespace
{
    // 並んでいる部品。どれかで変えたら、同じプロセッサのものをそろえる。
    // 画面の部品はメッセージスレッドでしか作らず、触らないので鍵は要らない。
    std::vector<GuiComponentPathRoot*>& instances()
    {
        static std::vector<GuiComponentPathRoot*> list;

        return list;
    }
}

GuiComponentPathRoot::GuiComponentPathRoot(const GuiContext& context) :
    ctx(context),
    selector(context)
{
    instances().push_back(this);
}

GuiComponentPathRoot::~GuiComponentPathRoot()
{
    auto& list = instances();

    list.erase(std::remove(list.begin(), list.end(), this), list.end());
}

void GuiComponentPathRoot::setupComponent(juce::Component& parent, int& tabOrder)
{
    const std::vector<SelectItem> items = {
        { .name = I18n::pick(u8"指定のディレクトリ", u8"Settings folder"), .value = Io::PathRoot::settingsFolder + 1 },
        { .name = I18n::pick(u8"ファイルのディレクトリ", u8"File's folder"), .value = Io::PathRoot::documentFolder + 1 },
    };

    // パラメータへは束ねない。環境設定の値を直に書き換える。
    selector.setup({ .parent = parent, .title = "REL.PATH", .items = items, .isReset = false });
    selector.setWantsKeyboardFocus(true);
    selector.setExplicitFocusOrder(++tabOrder);

    selector.onChange = [this] {
        const int index = selector.getSelectedItemIndex();

        if (index < 0) return;

        ctx.audioProcessor.relativePathRoot = index;

        for (auto* other : instances()) {
            if (other != this && &other->ctx.audioProcessor == &ctx.audioProcessor) other->refresh();
        }
    };

    refresh();
}

void GuiComponentPathRoot::refresh()
{
    const int root = juce::jlimit(Io::PathRoot::settingsFolder, Io::PathRoot::documentFolder,
        ctx.audioProcessor.relativePathRoot);

    selector.setSelectedItemIndex(root, juce::dontSendNotification);
}

void GuiComponentPathRoot::layoutComponent(juce::Rectangle<int>& rect)
{
    // 環境設定を読み直したあとも合わせておく
    refresh();

    layoutMain({ .mainRect = rect, .label = &selector.label, .component = &selector });
}

void GuiComponentPathRoot::layoutRow(juce::Rectangle<int> row, int labelWidth, int selectorWidth)
{
    refresh();

    selector.label.setBounds(row.removeFromLeft(labelWidth));
    selector.setBounds(row.removeFromLeft(selectorWidth));
}

void GuiComponentPathRoot::setVisible(bool visible)
{
    selector.setVisibleWithLabel(visible);
}
