#pragma once

#include <JuceHeader.h>

// 枠の畳み・非表示を覚えるときのタブごとの名前 (GuiGroupFolds)。
// プロセッサが設定ファイルへ非表示の枠を書くときにも使うので、軽いヘッダーに分けてある
namespace GuiGroupFoldKey
{
    // apvts.state へ書く属性の名前。タブごとに分ける
    static inline const juce::String opn = "opnGroupFolds";
    static inline const juce::String opna = "opnaGroupFolds";
    static inline const juce::String opl = "oplGroupFolds";
    static inline const juce::String opl3 = "opl3GroupFolds";
    static inline const juce::String opm = "opmGroupFolds";
    static inline const juce::String opzx7 = "opzx7GroupFolds";
    static inline const juce::String rhythm = "rhythmGroupFolds";
    static inline const juce::String fx = "fxGroupFolds";

    // 全部のタブ。設定ファイルへ非表示の枠を書くときに回す (4.5.0)
    inline juce::StringArray all()
    {
        return { opn, opna, opl, opl3, opm, opzx7, rhythm, fx };
    }

    // 非表示の枠を設定ファイルへ書くときの名前 (4.5.0)。opnaGroupFolds → opnaHiddenGroups
    inline juce::String hiddenSettingsKey(const juce::String& foldKey)
    {
        return foldKey.replace("GroupFolds", "HiddenGroups");
    }
}
