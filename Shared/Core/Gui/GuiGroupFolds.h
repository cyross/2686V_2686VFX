#pragma once

#include <JuceHeader.h>
#include <vector>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiContext.h"

// ============================================================================
// 枠の幅の畳み (4.3.0)
// ============================================================================
// 横へ並べた枠の見出しを押すと、その枠の幅を 30px まで縮める。もう一度押すと戻る。
// 全て開く「<>」・全て閉じる「><」のボタンもここで持つ。
//
// どれを畳んだかは apvts.state の属性へ書く。属性はプリセットにも DAW の保存にも
// そのまま入るので、別に書き出す手間が要らない。プリセットを読んで state が
// 差し替わったときは、畳み具合を当て直す。DAW はほかのスレッドで state を差し替える
// ことがあるので、apvts.state へリスナーは付けない (付け外しと差し替えが重なると
// 落ちる)。プロセッサが数える差し替えの回数 (stateLoads) を時々見に行く。
//
// 並べ直しはタブの側で行う。列の幅を columnWidth() で決めるだけでよい。
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
}

class GuiGroupFolds : private juce::Timer
{
public:
    GuiGroupFolds(const GuiContext& context, const juce::String& stateKey);
    ~GuiGroupFolds() override;

    // 畳めるようにして、覚えている状態を当てる。
    // name は覚えるときの名前。省くと見出しの文字を使う (言語で変わる見出しは名前を渡す)
    void add(ColoredGroupComponent& group, const juce::String& name = {});

    // 全て開く / 全て閉じる
    void setAllFolded(bool folded);

    // 全て開く「<>」・全て閉じる「><」
    GuiTextButton openAllButton;
    GuiTextButton closeAllButton;

    void setupButtons(juce::Component& parent, int& tabOrder);

    // 帯の左から 2 つ並べる。置いたぶん row を削る
    void layoutButtons(juce::Rectangle<int>& row, int height);

    // ボタン 1 つの幅
    static inline constexpr int buttonWidth = 26;

    // ボタンの左に置く隙間
    static inline constexpr int buttonGap = 8;

private:
    struct Entry
    {
        ColoredGroupComponent* group;
        juce::String name;
    };

    GuiContext ctx;
    juce::String key;

    std::vector<Entry> entries;

    // 最後に当てたときの差し替えの回数
    int seenLoads = 0;

    // 差し替えを見に行く間隔 (ms)
    static inline constexpr int pollMs = 250;

    // 畳んでいる枠の名前を | でつないで書く
    void save();

    // 書いてあるとおりに畳む。何か変わったら true
    bool load();

    void timerCallback() override;
};
