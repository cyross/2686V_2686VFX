#pragma once

#include <memory>

#include <JuceHeader.h>

#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Gui/Components/StageWave/StageWave.h"

// ============================================================================
// 生成中の波形の窓を開くボタン (4.4.0)
// ============================================================================
// リアルタイムのオシロスコープの下に置く。4.3.0 まではここに生成波形を
// 描いていたが、4.4.0 で段ごとの波形を見せる別の窓 (GuiStageWave) へ移した。
// ここに残るのは、その窓を開け閉めするボタンだけ。
//
// 窓はこの部品が持つ。エディタを閉じる・作り直すときは窓も一緒に閉じる。
class GuiGenWave : public juce::Component
{
public:
    static constexpr int labelHeight = 30;
    static constexpr int buttonHeight = 26;
    static constexpr int gap = 6;

    // 置き場に要る高さ。エディタ側はこれを見て区画を取る。
    static constexpr int totalHeight = labelHeight + gap + buttonHeight;

    explicit GuiGenWave(const GuiContext& context);
    ~GuiGenWave() override;

    void setup(juce::Component& parent);

    // 左上の位置と幅を渡す。高さは totalHeight で決まっている。
    void layout(int x, int y, int width);

    void resized() override;

    // ショートカットキー (Ctrl+M) から。窓を開いて作る
    void requestGenerate();

    // 窓を開け閉めする
    void setWindowOpen(bool open);
    bool isWindowOpen() const { return m_window != nullptr; }

    GuiStageWave* stageWave() { return m_window != nullptr ? &m_window->content() : nullptr; }

private:
    GuiContext ctx;

    juce::Label title;
    juce::TextButton openBtn;

    std::unique_ptr<GuiStageWaveWindow> m_window;

    void updateButton();
};
