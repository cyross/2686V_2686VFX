#pragma once

#include <JuceHeader.h>

#include <functional>
#include <vector>

#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiColor.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiValues.h"

// ============================================================================
// 段のループの一覧 (描くだけ)
// ============================================================================
// 1 行に 1 つの組を、No / START / END / COUNT の列で並べる。頭に見出しの行を置く。
// 組が増えるたびに 1 行ずつ伸びる。行を押すと、その組を選ぶ (onSelect)。
//
//   STEPS より後ろの段にかかる組は効かないので、薄く出す。
//   無限 (COUNT 0) のまま、外側や跨いでいる側へ進めなくしている組は、
//   COUNT を警告の色で出す (StepLoop::isTrapped)。
class GuiLoopList : public juce::Component
{
public:
    void enablementChanged() override { setAlpha(isEnabled() ? 1.0f : 0.4f); }

    // 1 行の高さ。値の帯 (GuiStepValues) と揃える
    static inline constexpr int rowHeight = 16;

    std::vector<StepLoop::Entry> entries;
    std::vector<bool> active;
    std::vector<bool> trapped;

    // 値のつまみが指している組 (0 から)。-1 なら無し
    int selected = -1;

    std::function<void(int)> onSelect;

    // 見出し + 組の数
    int getRowCount() const { return 1 + (int)entries.size(); }
    int getNaturalHeight() const { return getRowCount() * rowHeight; }

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
};

// ============================================================================
// 段のループの組を編む (Rich Mode / TAP SEQUENCER の LOOP)
// ============================================================================
// 組は最大 256 個あり、パラメータではなく段の表 (StepTable) に持つ。つまみは
// 「対象を選ぶもの + 値」の 1 組だけ置き、選んでいない組は一覧へ描く。
//
//   LP.TARGET            どの組を編むか
//   START / END / COUNT  選んだ組の値 (COUNT 0 は無限)
//   ADD / DELETE         組を足す・消す
//   一覧                 No / START / END / COUNT
//
// 組はいつも START・END の昇順に並べるので、START や END を動かすと並びが
// 変わることがある。そのときも LP.TARGET は動かした組を指し続ける。
class GuiComponentStepLoops : public GuiBase
{
public:
    GuiComponentStepLoops(const GuiContext& context);

    // table : 編む表を返す。steps : いまの段の数を返す。
    // changed : 組を変えたあとに呼ぶ (絵の描き直し・並べ直し)
    void setupComponent(juce::Component& parent, int& tabOrder, const juce::Font& labelFont,
        std::function<StepTable*()> table, std::function<int()> steps, std::function<void()> changed);

    void layoutComponent(juce::Rectangle<int>& rect, int rowHeight);

    void setVisibles(bool visible);
    void setEnables(bool enabled);

    // 表や段の数が外から変わったときに、つまみと一覧を映し直す
    void refresh();

    // 一覧の行数。区分の高さはこれで変わる
    int getRowCount() const { return list.getRowCount(); }

private:
    GuiSlider target;
    GuiSlider start;
    GuiSlider end;
    GuiSlider count;
    GuiTextButton add;
    GuiTextButton remove;
    GuiLoopList list;

    std::function<StepTable*()> tableOf;
    std::function<int()> stepsOf;
    std::function<void()> onChanged;

    bool outerEnabled = true;

    // つまみへ値を入れているところ。知らせを表へ返さないための印
    bool isLoading = false;

    enum class Edited { Start, End, Count };

    int entryCount() const;
    int stepCount() const;
    int selectedIndex() const;

    void select(int index);
    void onEdited(Edited which);
    void addEntry();
    void removeEntry();
    void applyEnables();
};
