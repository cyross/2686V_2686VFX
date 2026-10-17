#pragma once

#include <JuceHeader.h>
#include <array>
#include <functional>

#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "../WavePreview/WavePreview.h"

// 枠に映すエンベロープの種類。
//
// どれを映すかは枠ごとではなく、TARGET の隣の札でまとめて決める。
// 並べた枠は見比べるためのものなので、一枚ずつ違うものを映しても
// 比べようがない。
enum class GuiEnvGraphMode { Amp, Pitch, SsgSw256, SsgSwP256 };

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
class GuiTargetCell : public GuiBase, private juce::Timer
{
    int m_index = 0;

    GuiLabel titleLabel;

    // 波形を出すかどうかは持ち主が決める。FM のオペレータには出さない。
    GuiWavePreview m_preview;
    bool hasPreview = false;

    GuiEnvelopeGraph m_graph;

    // TARGET が指しているか。指している枠だけ濃い線で囲う。
    bool isActive = false;

    // 写し元 (TARGET の大きな枠のときだけ)。見出し・波形・絵をそのまま写す。
    const GuiTargetCell* m_source = nullptr;

    void timerCallback() override;
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

    ~GuiTargetCell() override { stopTimer(); }

    // TARGET の大きな枠にする。source の見出し・波形・絵を写し続ける
    // (小さな枠は値が変わるたびに描き直されるので、それを追う)。
    void follow(const GuiTargetCell* source);

    // 絵のほかに枠が取る丈 (余白・見出し・波形)
    static int chromeHeight(bool withPreview);

    // 枠の内側の余白
    static constexpr int insetPx = 4;

    // ---- 4.0.0: 左に TARGET の大きな枠、右に全部の小さな枠 ----
    //
    // 大きな枠の丈は波形つきの枠 2 段ぶん (OPZX7S / RHYTHM の絵の丈) にそろえ、
    // 幅はこれまでの枠の縦横比 (横 4 枚に割った幅 × 1 枚の丈) を保つ。
    // 小さな枠は横 cols 枚ずつ並べ、絵の縦横比を大きな枠の絵とそろえる
    // (波形の丈は変えない)。丈は大きな枠と小さな枠の並びの高いほう。
    template <size_t N>
    static void layoutBigAndSmall(juce::Rectangle<int>& pageArea, GuiTargetCell& big, std::array<GuiTargetCell, N>& cells, int count, bool withPreview, int cols)
    {
        cols = juce::jmax(1, cols);

        const int rows = juce::jmax(1, (count + cols - 1) / cols);
        const int oldW = pageArea.getWidth() / 4;
        const int oldH = naturalHeight(withPreview);
        const int bigH = naturalHeight(true) * 2;
        const int chrome = chromeHeight(withPreview);

        // 小さな枠が潰れないよう、大きな枠は幅の半分までにする
        const int bigW = juce::jmin(pageArea.getWidth() / 2, (int)((double)bigH * oldW / juce::jmax(1, oldH)));

        // 大きな枠の絵の縦横比
        const double aspect = (double)juce::jmax(1, bigW - insetPx * 2) / (double)juce::jmax(1, bigH - chrome);

        const int cellW = (pageArea.getWidth() - bigW) / cols;
        const int smallGraphH = (int)std::round((double)juce::jmax(1, cellW - insetPx * 2) / aspect);
        const int cellH = smallGraphH + chrome;

        auto area = pageArea.removeFromTop(juce::jmax(bigH, cellH * rows));

        big.layout(area.removeFromLeft(bigW).withHeight(bigH));

        for (int r = 0; r < rows; ++r) {
            auto rowArea = area.removeFromTop(cellH);

            for (int c = 0; c < cols; ++c) {
                const int i = r * cols + c;

                auto cellArea = rowArea.removeFromLeft(cellW);

                if (i < count && i < (int)N) cells[(size_t)i].layout(cellArea);
            }
        }
    }
};
