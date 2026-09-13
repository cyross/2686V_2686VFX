#pragma once

#include <JuceHeader.h>
#include <functional>

#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"
#include "../WavePreview/WavePreview.h"

// 枠に映すエンベロープの種類。
//
// どれを映すかは枠ごとではなく、TARGET の隣の札でまとめて決める。
// 並べた枠は見比べるためのものなので、一枚ずつ違うものを映しても
// 比べようがない。
enum class GuiEnvGraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };

// ==========================================================
// TARGET で選ぶ枠
// ==========================================================
// オペレータやパッドごとに設定をひとそろい並べるのをやめ、設定は
// 1 組へまとめた。代わりに上へ並ぶのがこの枠で、持っているのは
// 見出しと絵だけになる。
//
// 中身 (波形やエンベロープの形) を決めるのは持ち主の側。枠は数だけ
// 同時に出るのでつまみから値を読むわけにいかず、そのうえ音源ごとに
// パラメータの名前も違うため、ここで面倒を見ようとすると音源の数だけ
// 枝が生える。
//
// 枠を押すと TARGET がそこへ移る。下の設定はまとめて切り替わる。
class GuiTargetCell : public GuiBase
{
    int m_index = 0;

    GuiLabel titleLabel;

    // 波形を出すかどうかは持ち主が決める。FM のオペレータには出さない。
    GuiWavePreview m_preview;
    bool hasPreview = false;

    GuiEnvelopeGraph m_graph;

    // TARGET が指しているか。指している枠だけ濃い線で囲う。
    bool isActive = false;
public:
    GuiTargetCell(const GuiContext& context) :
        GuiBase(context),
        titleLabel(context),
        m_preview(context)
    {
    }

    // 枠を押したときに呼ぶ。TARGET をここへ動かすために使う。
    std::function<void(int)> onSelect;

    void setup(juce::Component& parent, int index, const juce::String& title, bool withPreview);
    void layout(juce::Rectangle<int> rect);

    // TARGET が指しているかを伝える。枠線の濃さが変わる。
    void setActive(bool active);

    GuiWavePreview& preview() { return m_preview; }
    GuiEnvelopeGraph& graph() { return m_graph; }

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;

    // 見出し・波形・グラフを積んだときの丈。枠を並べる側が区画を取るのに使う。
    static int naturalHeight(bool withPreview);
};
