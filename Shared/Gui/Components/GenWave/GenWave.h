#pragma once

#include <array>

#include <JuceHeader.h>

#include "./GenWavePlayer.h"
#include "./GenWaveRender.h"
#include "Shared/Core/Gui/GuiContext.h"

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
// 波形の下に、一時停止・コマ送りのシークバー・縦の拡大率を置く
// (GenWavePlayer)。ある瞬間の形をじっくり見るためのもの。
//
// 計算そのものは GenWaveRender が受け持つ。ここは押す・見せるだけ。
class GuiGenWave : public juce::Component, private juce::Timer
{
public:
    static constexpr int labelHeight = 30;
    static constexpr int waveHeight = 150;
    static constexpr int playerHeight = GenWavePlayer::height;
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
        labelHeight + gap + waveHeight + gap + playerHeight + gap + cycleRowHeight + gap + buttonRowHeight;

    explicit GuiGenWave(const GuiContext& context);
    ~GuiGenWave() override;

    void setup(juce::Component& parent);

    // 左上の位置と幅を渡す。高さは totalHeight で決まっている。
    void layout(int x, int y, int width);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    // 出来上がりを受け取る。別のスレッドから直に呼ばず、
    // かならずメッセージスレッドへ渡してから呼ぶこと。
    void finishGenerate(const GenWaveRender::Wave& wave);

    // ショートカットキーから作らせる。ボタンを押したときと同じ。
    void requestGenerate() { startGenerate(); }
private:
    GuiContext ctx;

    juce::Label title;
    juce::TextButton generateBtn;
    juce::TextButton deleteBtn;

    // 作ったもの。空なら「まだ作っていない」。
    GenWaveRender::Wave m_wave;

    int m_cycleIndex = 1;   // 既定は 2 周期

    // 一時停止・コマ送り・縦の拡大率
    GenWavePlayer m_player;

    // 作っている最中は押させない
    bool m_busy = false;

    // 計算は別のスレッドで回す。1 本あればよい。
    juce::ThreadPool m_pool{ 1 };

    void startGenerate();
    void clearWave();

    juce::Rectangle<int> waveArea() const;
    juce::Rectangle<int> playerArea() const;
    juce::Rectangle<int> cycleArea() const;
    juce::Rectangle<int> cycleCell(int index) const;

    void drawWave(juce::Graphics& g, juce::Rectangle<int> area);
    void drawCycles(juce::Graphics& g);

    void timerCallback() override { repaint(); }
};
