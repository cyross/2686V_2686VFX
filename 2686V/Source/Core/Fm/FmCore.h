#pragma once

#include <JuceHeader.h> // 数学定数(MathConstants)を使うため必要
#include <array>
#include <cmath>

#include "../Synth/SynthCore.h"

class FmCore : public SynthCore
{
public:
    FmCore() : SynthCore() {}

    struct BaseAlgRouting {
        float in2_1;             // OP2への入力 (1からの割合)
        float in3_1, in3_2;      // OP3への入力 (1, 2からの割合)
        float in4_1, in4_2, in4_3; // OP4への入力 (1, 2, 3からの割合)
        float out_1, out_2, out_3, out_4; // 最終出力へのミックス割合
    };

    static const std::array<BaseAlgRouting, 8> baseRoutings;
};
