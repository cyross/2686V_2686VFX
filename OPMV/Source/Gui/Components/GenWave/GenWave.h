#pragma once

#include <array>

#include <JuceHeader.h>

#include "./GenWaveRender.h"
#include "../../../Core/Gui/GuiContext.h"

// ============================================================================
// 生成波形のプレビュー
// ============================================================================
// リアルタイムのオシロスコープの下に置く。オシロは今出ている音をそのまま
// 映すだけなので、鍵盤を押していないと何も見えない。こちらは鍵盤を押さ
// なくても「この設定はどう鳴るか」を見られるようにするためのもの。
//
// 押したときだけ計算する。10 秒ぶんを作り、その上を窓が動いていく形で
// 見せる。窓の幅は 1・2・5・10 周期から選べる。値を触るたびに作り直す
// ようなことはしない。負担を軽くするための決め事。
//
// 計算そのものは GenWaveRender が受け持つ。ここは押す・見せるだけ。
class GuiGenWave : public juce::Component, private juce::Timer
{
public:
    static constexpr int labelHeight = 30;
    static constexpr int waveHeight = 100;
    static constexpr int cycleRowHeight = 22;
    static constexpr int buttonRowHeight = 26;
    static constexpr int gap = 6;
    static constexpr int deleteWidth = 72;
    static constexpr int cycleLabelWidth = 52;

    // 窓の幅の選択肢 (周期数)
    static constexpr std::array<int, 4> cycleChoices{ 1, 2, 5, 10 };

    // 30fps。棒の伸び縮みほど細かくなくてよい。
    static constexpr int frameMs = 33;

    // 置き場に要る高さ。エディタ側はこれを見て区画を取る。
    static constexpr int totalHeight =
        labelHeight + gap + waveHeight + gap + cycleRowHeight + gap + buttonRowHeight;

    explicit GuiGenWave(const GuiContext& context);
    ~GuiGenWave() override;

    void setup(juce::Component& parent);

    // 左上の位置と幅を渡す。高さは totalHeight で決まっている。
    void layout(int x, int y, int width);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;

    // 出来上がりを受け取る。別のスレッドから直に呼ばず、
    // かならずメッセージスレッドへ渡してから呼ぶこと。
    void finishGenerate(const GenWaveRender::Wave& wave);
private:
    GuiContext ctx;

    juce::Label title;
    juce::TextButton generateBtn;
    juce::TextButton deleteBtn;

    // 作ったもの。空なら「まだ作っていない」。
    GenWaveRender::Wave m_wave;

    int m_cycleIndex = 1;   // 既定は 2 周期

    // 動かし始めた時刻。経過秒から窓の位置を出す。
    double m_startMs = 0.0;

    // 作っている最中は押させない
    bool m_busy = false;

    // 計算は別のスレッドで回す。1 本あればよい。
    juce::ThreadPool m_pool{ 1 };

    void startGenerate();
    void clearWave();

    juce::Rectangle<int> waveArea() const;
    juce::Rectangle<int> cycleArea() const;
    juce::Rectangle<int> cycleCell(int index) const;

    void drawWave(juce::Graphics& g, juce::Rectangle<int> area);
    void drawCycles(juce::Graphics& g);

    void timerCallback() override { repaint(); }
};
