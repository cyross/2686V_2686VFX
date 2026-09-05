#pragma once

#include <vector>

#include "../../../Core/Gui/GuiStructs.h"

// ============================================================================
// SSG HW ENV の Shape 一覧
// ============================================================================
// 波形スロットは SSG HW AMP ENV と SSG HW PITCH ENV で共通なので、
// 選択肢もここへ一本化する。波形を足したときに片方だけ増えるのを防ぐため。
//
// 0〜7 は実機 AY-3-8910 / YM2149 の shape 8〜15、8 以降はこのプラグインの
// 追加分。実体は Effect/Envelope/Amp/SsgHw/EnvSsgHwParams.h の SsgHwShape。
namespace SsgHwEnvItems
{
    inline std::vector<SelectItem>& shapes()
    {
        static std::vector<SelectItem> items = {
            {.name = "0: Saw Down",                     .value =  1 },
            {.name = "1: Saw Down & Hold",              .value =  2 },
            {.name = "2: Triangle",                     .value =  3 },
            {.name = "3: Alternative Saw Down & Hold",  .value =  4 },
            {.name = "4: Saw Up",                       .value =  5 },
            {.name = "5: Saw Up & Hold",                .value =  6 },
            {.name = "6: Triangle Invert",              .value =  7 },
            {.name = "7: Alternative Saw Up & Hold",    .value =  8 },
            {.name = "8: Square 75%",                   .value =  9 },
            {.name = "9: Square 50%",                   .value = 10 },
            {.name = "10: Square 25%",                  .value = 11 },
            {.name = "11: Square 12.5%",                .value = 12 },
            {.name = "12: Square -75%",                 .value = 13 },
            {.name = "13: Square -50%",                 .value = 14 },
            {.name = "14: Square -25%",                 .value = 15 },
            {.name = "15: Square -12.5%",               .value = 16 },
            {.name = "16: Step Up 4",                   .value = 17 },
            {.name = "17: Step Up 5",                   .value = 18 },
            {.name = "18: Step Up 6",                   .value = 19 },
            {.name = "19: Step Up 11",                  .value = 20 },
            {.name = "20: Step Down 4",                 .value = 21 },
            {.name = "21: Step Down 5",                 .value = 22 },
            {.name = "22: Step Down 6",                 .value = 23 },
            {.name = "23: Step Down 11",                .value = 24 },
            {.name = "24: Step Peak 4",                 .value = 25 },
            {.name = "25: Step Peak 5",                 .value = 26 },
            {.name = "26: Step Peak 6",                 .value = 27 },
            {.name = "27: Step Peak 11",                .value = 28 },
            {.name = "28: Step Valley 4",               .value = 29 },
            {.name = "29: Step Valley 5",               .value = 30 },
            {.name = "30: Step Valley 6",               .value = 31 },
            {.name = "31: Step Valley 11",              .value = 32 },
            {.name = "32: Sample & Hold",               .value = 33 },
            {.name = "33: Sample & Hold 4",             .value = 34 },
            {.name = "34: Sample & Hold 8",             .value = 35 },
            {.name = "35: Sample & Hold 16",            .value = 36 },
            {.name = "36: Sample & Hold 32",            .value = 37 },
            {.name = "37: Sample & Hold 64",            .value = 38 },
            {.name = "38: Exp Decay",                   .value = 39 },
            {.name = "39: Exp Attack",                  .value = 40 },
            {.name = "40: Sine",                        .value = 41 },
            {.name = "41: Attack & Decay",              .value = 42 },
            {.name = "42: Double Pulse",                .value = 43 },
            {.name = "43: Random",                      .value = 44 },
        };

        return items;
    }
}
