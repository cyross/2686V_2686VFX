#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <span>

#include "./GuiContext.h"

class GuiBase: public juce::Component
{
public:
	GuiBase(const GuiContext& context) : ctx(context) {}

    virtual void setup() {};
    virtual void layout(juce::Rectangle<int> content) {};

    // 簡易表示モードで隠す区分への一括操作。
    //
    // どの区分が対象かはタブごとに違うので、実際の中身はタブ側で書く。
    // 対象を持たないタブは何もしない。
    //
    // 入り切りの札を持たない区分 (MUL・DET / UNISON・HARMONY) は、
    // どの操作でも触らない。
    virtual void bypassHiddenCategories() {};
    virtual void openEnabledCategories() {};
    virtual void closeBypassedCategories() {};
protected:
	// GuiOpnaなどでも使うので、using宣言でエイリアスを作っておく
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    GuiContext ctx;
};
