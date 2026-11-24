#pragma once

#include <JuceHeader.h>

#include <functional>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Processor/ProcessorPattern.h"
#include "../Separator/NormalSeparator.h"

// ============================================================================
// パターンの切り替え (4.5.0)
// ============================================================================
// SSG HW AMP / PITCH ENV・SSG SW AMP / PITCH ENV[256]・TAP AMP / PITCH SEQUENCER・
// UNISON / HARMONY の区分の一番下に、区切り線を引いて置く。
//
//   PATTERNS          組の数 (1〜32)
//   PATTERN           使う組 (1〜PATTERNS)
//   RESET TO CHANGE   入れていると、組を切り替えたときに鳴っている音を頭から始め直す
//
// 区分のつまみは選んでいる組の値を出す (切り替えるとプロセッサが APVTS へ戻す)。
// 段の表を持つ区分は、組が変わったら onPatternChange で表を引き直して描き直す。
// 区分のインポート・エクスポートは、選んでいる組に対して行われる。
class GuiComponentPattern : public GuiBase, private juce::Timer
{
public:
    explicit GuiComponentPattern(const GuiContext& context);
    ~GuiComponentPattern() override { stopTimer(); }

    // base は区分の頭 (code + EnvPattern::ssgHwEnv など)。そのパラメータが無ければ何も置かない
    void setupComponent(juce::Component& parent, const juce::String& base, int& tabOrder);

    // 束縛先を丸ごと差し替える (TARGET で指し先を切り替えるとき)
    void rebind(const juce::String& base);

    // 区分の中身を出しているときだけ呼ぶ。区切り線から下を並べる
    void layoutComponent(juce::Rectangle<int>& rect);

    void setPartsVisible(bool visible);
    void setPartsEnabled(bool enabled);

    // 組が変わったとき (画面で選んだとき・オートメーションで切り替わったとき)
    std::function<void()> onPatternChange;

    // 区分にパターンがあるか
    bool exists() const { return available; }

private:
    void timerCallback() override;

    // PATTERN の範囲を PATTERNS に合わせる。外れていたら PATTERNS へ寄せる
    void clampPattern();

    NormalSeparator separator;
    GuiSlider patterns;
    GuiSlider pattern;
    GuiToggleButton reset;

    juce::Font labelFont = juce::Font(juce::FontOptions(12.0f));

    juce::String base;
    bool available = false;

    // 前に見た、画面で選んでいる組
    int seen = 1;

    bool partsEnabled = true;
};
