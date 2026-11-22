#pragma once

#include <JuceHeader.h>
#include <vector>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiGroupFoldKeys.h"

// ============================================================================
// 枠の幅の畳み (4.3.0)
// ============================================================================
// 横へ並べた枠の見出しを押すと、その枠の幅を 30px まで縮める。もう一度押すと戻る。
// 全て開く「<>」・全て閉じる「><」のボタンもここで持つ。
//
// 既定では畳める枠を全て畳んでおく。属性が無いときがそれに当たる。
//
// どれを畳んだかは apvts.state の属性へ書く。属性はプリセットにも DAW の保存にも
// そのまま入るので、別に書き出す手間が要らない。プリセットを読んで state が
// 差し替わったときは、畳み具合を当て直す。DAW はほかのスレッドで state を差し替える
// ことがあるので、apvts.state へリスナーは付けない (付け外しと差し替えが重なると
// 落ちる)。プロセッサが数える差し替えの回数 (stateLoads) を時々見に行く。
//
// 並べ直しはタブの側で行う。列の幅を columnWidth() で決めるだけでよい。
//
// ---- 非表示 (4.5.0) ----
// 見出しを右クリックすると「グループを非表示」を出し、押すと枠を丸ごと隠す。
// 枠の並ぶ帯の、枠の無いところを右クリックすると、枠のチェックリストを出す。入れると
// (畳んだ状態で) 出し、外すと隠す。「すべてのグループを表示」「すべてのグループを
// 非表示」もある。帯の中でも枠の上 (中身の外の余白) を右クリックしたときは、その枠の
// 区分のチェックリスト (GuiScrollGroup::showCategoryMenu) を出す。
//
// どれを隠したかは、プリセットではなく設定ファイル (SETTINGS) の項目として、
// プロセッサの hiddenGroups にタブごとに持つ。並べる側は isGroupHidden() の
// 枠を列ごと出さない。

class GuiGroupFolds : private juce::Timer, private juce::MouseListener
{
public:
    GuiGroupFolds(const GuiContext& context, const juce::String& stateKey);

    GuiGroupFolds(const GuiGroupFolds&) = delete;
    GuiGroupFolds& operator=(const GuiGroupFolds&) = delete;
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

    // 枠の並ぶ帯。枠の無いところを右クリックすると、枠のチェックリストを出す (4.5.0)。
    // 帯の送り台 (Viewport) を渡す。中身のキャンバスの外 (右の空き) も拾う
    void attachStrip(juce::Viewport& strip);

    // 枠のチェックリストを出す (4.5.0)
    void showGroupMenu();

    // 全て出す / 全て隠す (4.5.0)
    void setAllHidden(bool hidden);

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

    // 帯 (attachStrip で渡されたもの)
    juce::Component::SafePointer<juce::Viewport> stripViewport;

    // 最後に当てた非表示の並び (設定ファイルを読み直したら変わる)
    juce::String seenHidden;

    // 1 つの枠を出す / 隠す。出すときは畳んだ状態で出す
    void setHidden(ColoredGroupComponent& group, bool hidden);

    // 隠している枠の名前を | でつないで、プロセッサの hiddenGroups へ書く
    void saveHidden();

    // プロセッサの hiddenGroups のとおりに隠す。何か変わったら true
    bool loadHidden();

    void mouseDown(const juce::MouseEvent& e) override;

    // 畳んでいる枠の名前を | でつないで書く
    void save();

    // 書いてあるとおりに畳む。何か変わったら true
    bool load();

    void timerCallback() override;
};
