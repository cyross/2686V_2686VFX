#pragma once

#include <array>
#include <functional>

class Opzx7Alg
{
    struct AlgRouting {
        float in2_1;             // OP2への入力 (1からの割合)
        bool  fb2_1;             // OP2からOP1へのフィードバック有無
        float in3_1, in3_2;      // OP3への入力 (1, 2からの割合)
        float in4_1, in4_2, in4_3; // OP4への入力 (1, 2, 3からの割合)
        float out_1, out_2, out_3, out_4; // 最終出力へのミックス割合
    };

    static const std::array<AlgRouting, 36> routings;
public:
    int idx = 0;

    using OpGetSampleFunc = std::function<void(float& out, float in, float pmLfoVal)>;
    using OpFbPushBack = std::function<void(float fb)>;

    float process(std::array<OpGetSampleFunc, 4>& gsFuncs, std::array<OpFbPushBack, 4>& fpbFuncs, float pmLfoVal) const;
    bool isUseExtFb() const;
};
