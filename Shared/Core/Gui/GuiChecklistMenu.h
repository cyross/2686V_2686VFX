#pragma once

#include <JuceHeader.h>

#include <functional>
#include <vector>

// ============================================================================
// チェックリストのポップアップ (4.5.0)
// ============================================================================
// 行を押しても閉じないポップアップ。枠の表示・非表示 (GuiGroupFolds) と、枠の中の
// 区分の開閉 (GuiScrollGroup) で使う。
//
// - 入り切りの行は、押すとその場で印が変わる。閉じない
// - 下に並べる操作の行 (「すべて …」) も、押しても閉じない
// - ESC かポップアップの外を押すと閉じる (JUCE の PopupMenu がそうする)
//
// 行は PopupMenu::CustomComponent で作る。自分で押されたことを扱う (勝手に
// 選ばれない) ようにすると、押しても PopupMenu が閉じない。
namespace GuiChecklistMenu
{
    struct Check
    {
        juce::String name;

        // いま入っているか。押したあと、全部の行の印をこれで描き直す
        std::function<bool()> isOn;

        // 押されたとき。渡すのは押したあとの状態
        std::function<void(bool)> set;
    };

    struct Action
    {
        juce::String name;
        std::function<void()> run;
    };

    // マウスの位置に出す
    void show(const juce::String& title, std::vector<Check> checks, std::vector<Action> actions);
}
