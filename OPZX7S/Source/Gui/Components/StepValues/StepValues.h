#pragma once

#include <JuceHeader.h>

#include <vector>

#include "../../../Core/Gui/GuiColor.h"

// 段ごとの値を横に並べて描くだけの帯。
//
// SSG SW エンベロープのように「同じ形のつまみが段の数だけ並ぶ」場所で、
// つまみは対象を選ぶもの 1 つと値のもの 1 つに減らし、選んでいない段の値は
// ここへまとめて描く。段ごとにつまみを置くと、それだけで部品が 200 個近くになり、
// その一つ一つが束縛と描き方を抱えるため。
//
// 自分では値を持たず、描くたびに呼び出し側が入れた中身をそのまま出す。
class GuiStepValues : public juce::Component
{
public:
    // 段の見出し (R1, L2, STL など)。values と同じ数だけ入れる。
    std::vector<juce::String> labels;
    std::vector<float> values;

    // いま値のつまみが束縛されている段。-1 なら無し。
    int selected = -1;

    // STEP で実際に使われている段数。これより後ろは薄く出す。
    // 負なら全部を通常の濃さで出す。
    int activeCount = -1;

    // 値の小数桁。
    int decimals = 2;

    void paint(juce::Graphics& g) override;
};
