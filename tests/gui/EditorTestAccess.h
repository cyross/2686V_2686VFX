#pragma once

// 画面の中で、テストだけが触る口。PluginEditor が friend にしている。
//
// いくつかのテストで使うので、ここへ 1 つだけ置く。
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiGroupFolds.h"

struct EditorTestAccess
{
    // タブの中身を後から作る仕掛けを持たない本 (2686VFX) もある
    //
    // 型に依存させないと、無い関数を requires の中で書いた時点で弾かれる。
    template <typename Editor>
    static void materializeAll(Editor& e)
    {
        if constexpr (requires { e.materializeAllTabs(); }) e.materializeAllTabs();
    }
    static juce::TabbedComponent& tabs(AudioPlugin2686VEditor& e) { return e.tabs; }

    // 畳める枠をすべて開いた state にする。画面を作る前に呼ぶ。
    // 立ち上げた直後は枠を畳んで出すので、中の部品の場所を調べるテストはこれで開いておく
    static void unfoldAllGroups(juce::AudioProcessorValueTreeState& apvts)
    {
        for (const auto& key : { GuiGroupFoldKey::opn, GuiGroupFoldKey::opna, GuiGroupFoldKey::opl, GuiGroupFoldKey::opl3,
            GuiGroupFoldKey::opm, GuiGroupFoldKey::opzx7, GuiGroupFoldKey::rhythm, GuiGroupFoldKey::fx })
            apvts.state.setProperty(key, juce::String(), nullptr);
    }
};
