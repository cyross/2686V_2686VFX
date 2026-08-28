#pragma once

#include <JuceHeader.h>
#include <functional>
#include <vector>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiContext.h"

// ==========================================================
// パラメータを棒グラフで直接編集するエディタの土台
// ==========================================================
// WT のカスタム波形 (WaveformContainer)、WT2 のカスタム波形
// (Waveform2Container)、MODULATION の FDS テーブル (FdsTableEditor) は、
// 扱う値の型も描き方も違うが、
//
//   ・APVTS のパラメータへ直接ポインタを持ち、リスナーで再描画する
//   ・マウスの押下/ドラッグで値を書き、移動でホバー位置を追う
//   ・ホバー中の値を小さな黒い箱で出す
//
// という土台は共通なので、そこだけをここへ集めている。
// 値の意味に踏み込む処理 (paint / updateSliderValue / updateHoverState) は
// 派生側に残す。
class ParamBarEditorBase :
    public juce::Component,
    public GuiBaseComponent,
    public juce::AudioProcessorValueTreeState::Listener
{
protected:
    // APVTS のパラメータへの直接ポインタを保持して高速にアクセスする
    std::vector<juce::RangedAudioParameter*> m_params;
    juce::StringArray m_paramIds; // リスナー削除用に ID を格納

    bool isEnabledState = false;
    int hoveredIndex = -1;
    juce::Point<int> lastMousePos;
    juce::ModifierKeys lastModifiers;

    // idPrefix + 0 〜 idPrefix + (count-1) のパラメータを引いてリスナーを張る
    void attachParams(const juce::String& idPrefix, int count);

    // ホバー中の値を出す小さな箱。中身の文字列は派生側が組み立てる。
    void paintHoverText(juce::Graphics& g, const juce::String& text);

    virtual void updateSliderValue(const juce::MouseEvent& e) = 0;
    virtual void updateHoverState(const juce::MouseEvent& e) = 0;
public:
    ParamBarEditorBase(const GuiContext& context) : GuiBaseComponent(context) {}

    // 値が変わったときに呼ばれる。棒グラフの見た目とは別に、
    // 編集結果を使った表示 (プレビューなど) を持つ側がつなぐ。
    std::function<void()> onParamChanged;
    ~ParamBarEditorBase() override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void setCustomEnabled(bool shouldBeEnabled);

    void mouseMove(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
};
