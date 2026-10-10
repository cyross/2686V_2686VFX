#pragma once

// 画面の中で、テストだけが触る口。PluginEditor が friend にしている。
//
// いくつかのテストで使うので、ここへ 1 つだけ置く。
#include "Core/Editor/PluginEditor.h"

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
};
