#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Const/ConstGlobal.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/Fix/Fix.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Gui/Components/PitchButtons/PitchButtons.h"
#include "../../Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "../../Gui/Components/MulDetune/MulDetune.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ViewMode/ViewMode.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"
#include "../../Gui/Components/Level/Level.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/AlgMatrix/GuiOpzx7AlgMatrix.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiOpzx7 : public GuiBase
{
    /*
     * アルゴリズムのオペレータ表記凡例
     * 2026.3.7 CYROSS
     *
     * [C] : キャリアー(出力はオーディオ出力)
     * [M->n] : n番オペレータへ出力するモジュレーター
     * [C:FB] : 自身へフィードバックもするキャリアー
     * [M:FB->n] : 自身へフィードバックもする、n番オペレーターへ出力するモジュレーター
     * [C:FBm] : m番オペレータへフィードバックもするキャリア―
     * [M:FBm->n] : m番オペレータへフィードバックもする、n番オペレーターへ出力するモジュレーター
     * /を挟んでnが複数ある場合: それぞれのオペレータに出力する
     * 複数のnが存在する場合 : 各オペレーターからの出力を足し合わせて、n番のオペレータへ出力
     * -- : 未使用
     */
    static inline const std::array<std::array<juce::String, Opzx7PrValue::ops>, Opzx7PrValue::algorithms> algOpPrefix = { {
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},          // 00: <OPX-00>
        {{"([M->2])", "([M:FB1->3])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},         // 01: <OPX-01>
        {{"([M:FB->3])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},          // 02: <OPX-02>
        {{"([M:FB->4])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},          // 03: <OPX-03>
        {{"([M:FB->2])", "([M->4])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},          // 04: <OPX-04>
        {{"([M:FB->2])", "([M:FB1->4])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},      // 05: <OPX-05>
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},             // 06: <OPX-06>
        {{"([M->2])", "([C:FB1])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},            // 07: <OPX-07>
        {{"([C:FB])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},             // 08: <OPX-08>
        {{"([C:FB])", "([M->4])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},             // 09: <OPX-09>
        {{"([M:FB->2])", "([C])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)"}},                // 10: <OPX-10>
        {{"([M->2])", "([C:FB1])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)"}},               // 11: <OPX-11>
        {{"([M:FB->2/3/4])", "([C])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)"}},            // 12: <OPX-12>
        {{"([C:FB])", "([M->3])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)"}},                // 13: <OPX-13>
        {{"([C:FB][M:FB->2])", "([C])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},       // 14: <OPX-14>
        {{"([C:FB])", "([C])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)"}},                   // 15: <OPX-15>
        {{"([M:FB->2])", "([M->3])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)"}},              // 16: <OPX-16>
        {{"([M->2])", "([M:FB1->3])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)"}},             // 17: <OPX-17>
        {{"([M:FB->3])", "([M->3])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)"}},              // 18: <OPX-18>
        {{"([C:FB])", "([M->3])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)"}},                 // 19: <OPX-19>
        {{"([M:FB->2])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)"}},                 // 20: <OPX-20>
        {{"([M->2])", "([C:FB1])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)"}},                // 21: <OPX-21>
        {{"([C:FB])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)"}},                    // 22: <OPX-22>
        {{"([C:FB][M:FB->2])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)"}},           // 23: <OPX-23>
        {{"([M:FB->2])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)", "(--)"}},                  // 24: <OPX-24>
        {{"([M->2])", "([C:FB1])", "(--)", "(--)", "(--)", "(--)", "(--)", "(--)"}},                 // 25: <OPX-25>
        {{"([C:FB])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)", "(--)"}},                     // 26: <OPX-26>
        {{"([C:FB][M->2])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)", "(--)"}},               // 27: <OPX-27>
        {{"([M:FB->2])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)", "(--)"}},                  // 28: <MA3-00>
        {{"([C:FB])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)", "(--)"}},                     // 29: <MA3-01>
        {{"([C:FB])", "([C])", "([C:FB])", "([C])", "(--)", "(--)", "(--)", "(--)"}},                // 30: <MA3-02>
        {{"([M:FB->4])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},          // 31: <MA3-03>
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},          // 32: <MA3-04>
        {{"([M:FB->2])", "([C])", "([M:FB->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},          // 33: <MA3-05>
        {{"([C:FB])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)", "(--)", "(--)"}},             // 34: <MA3-06>
        {{"([C:FB])", "([M->3])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)"}},                // 35: <MA3-07>
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])", "([M->6])", "([C])", "(--)", "(--)"}},     // 36: <OPS-00>
        {{"([M->2])", "([M->3])", "([M->4])", "([C])", "([M:FB->6])", "([C])", "(--)", "(--)"}},     // 37: <OPS-01>
        {{"([M:FB->2])", "([M->3])", "([C])", "([M->5])", "([M->6])", "([C])", "(--)", "(--)"}},     // 38: <OPS-02>
        {{"([M->2])", "([M->3])", "([C:FB1])", "([M->5])", "([M->6])", "([C])", "(--)", "(--)"}},    // 39: <OPS-03>
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])", "([M->6])", "([C])", "(--)", "(--)"}},        // 40: <OPS-04>
        {{"([M->2])", "([C:FB1])", "([M->4])", "([C])", "([M->6])", "([C])", "(--)", "(--)"}},       // 41: <OPS-05>
        {{"([M:FB->2])", "([M->4])", "([M->4])", "([C])", "([M->6])", "([C])", "(--)", "(--)"}},     // 42: <OPS-06>
        {{"([M->2])", "([M->4])", "([M:FB->4])", "([C])", "([M->6])", "([C])", "(--)", "(--)"}},     // 43: <OPS-07>
        {{"([M->2])", "([M->4])", "([M->4])", "([C])", "([M:FB->6])", "([C])", "(--)", "(--)"}},     // 44: <OPS-08>
        {{"([M->3])", "([M->3])", "([C])", "([M:FB->5])", "([M->6])", "([C])", "(--)", "(--)"}},     // 45: <OPS-09>
        {{"([M:FB->3])", "([M->3])", "([C])", "([M->5])", "([M->6])", "([C])", "(--)", "(--)"}},     // 46: <OPS-10>
        {{"([M->4])", "([M->4])", "([M->4])", "([C])", "([M:FB->6])", "([C])", "(--)", "(--)"}},     // 47: <OPS-11>
        {{"([M:FB->4])", "([M->4])", "([M->4])", "([C])", "([M->6])", "([C])", "(--)", "(--)"}},     // 48: <OPS-12>
        {{"([M:FB->3])", "([M->3])", "([M->4])", "([C])", "([M->6])", "([C])", "(--)", "(--)"}},     // 49: <OPS-13>
        {{"([M->3])", "([M->3])", "([M->4])", "([C])", "([M:FB->6])", "([C])", "(--)", "(--)"}},     // 50: <OPS-14>
        {{"([M:FB->2])", "([C])", "([M->4])", "([M->6])", "([M->6])", "([C])", "(--)", "(--)"}},     // 51: <OPS-15>
        {{"([M->2])", "([C])", "([M->4])", "([M->6])", "([M:FB->6])", "([C])", "(--)", "(--)"}},     // 52: <OPS-16>
        {{"([M->2])", "([M->3])", "([C])", "([M:FB->6])", "([M->6])", "([C])", "(--)", "(--)"}},     // 53: <OPS-17>
        {{"([M:FB->2/3])", "([C])", "([C])", "([M->5])", "([M->6])", "([C])", "(--)", "(--)"}},      // 54: <OPS-18>
        {{"([M->2])", "([M->3])", "([C])", "([M:FB->5/6])", "([C])", "([C])", "(--)", "(--)"}},      // 55: <OPS-19>
        {{"([M->2/3])", "([C])", "([C])", "([M:FB->5/6])", "([C])", "([C])", "(--)", "(--)"}},       // 56: <OPS-20>
        {{"([M:FB->2/3/4])", "([C])", "([C])", "([C])", "([M->6])", "([C])", "(--)", "(--)"}},       // 57: <OPS-21>
        {{"([M:FB->2/3])", "([C])", "([C])", "([M->5])", "([C])", "([C])", "(--)", "(--)"}},         // 58: <OPS-22>
        {{"([M:FB->2/3/4/5/6])", "([C])", "([C])", "([C])", "([C])", "([C])", "(--)", "(--)"}},      // 59: <OPS-23>
        {{"([M:FB->2/3])", "([C])", "([C])", "([C])", "([C])", "([C])", "(--)", "(--)"}},            // 60: <OPS-24>
        {{"([M:FB->3])", "([M->3])", "([C])", "([M->5])", "([C])", "([C])", "(--)", "(--)"}},        // 61: <OPS-25>
        {{"([M->3])", "([M->3])", "([C])", "([M:FB->5])", "([C])", "([C])", "(--)", "(--)"}},        // 62: <OPS-26>
        {{"([C])", "([M:FB->3])", "([M->4])", "([C])", "([M->6])", "([C])", "(--)", "(--)"}},        // 63: <OPS-27>
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])", "([C])", "([C])", "(--)", "(--)"}},           // 64: <OPS-28>
        {{"([C])", "([M:FB->3])", "([M->4])", "([C])", "([C])", "([C])", "(--)", "(--)"}},           // 65: <OPS-29>
        {{"([M:FB->2])", "([C])", "([C])", "([C])", "([C])", "([C])", "(--)", "(--)"}},              // 66: <OPS-30>
        {{"([C:FB])", "([C])", "([C])", "([C])", "([C])", "([C])", "(--)", "(--)"}},                 // 67: <OPS-31>
        {{"([C:FB])", "(--)", "(--)", "(--)", "(--)", "(--)", "(--)", "(--)"}},                      // 68: <OPZX7S-000>
        {{"([C:FB])", "([C:FB])", "(--)", "(--)", "(--)", "(--)", "(--)", "(--)"}},                  // 69: <OPZX7S-001>
        {{"([M->2])", "([C:FB1])", "([M->4])", "([C:FB3])", "(--)", "(--)", "(--)", "(--)"}},        // 70: <OPZX7S-002>
        {{"([C:FB])", "([C:FB])", "([C:FB])", "([C:FB])", "(--)", "(--)", "(--)", "(--)"}},          // 71: <OPZX7S-003>
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([M->5])", "([M->6])", "([C])", "(--)", "(--)"}},  // 72
        {{"([M->2])", "([M:FB1->3])", "([M->4])", "([M->5])", "([M->6])", "([C])", "(--)", "(--)"}}, // 73
        {{"([M->2])", "([M->3])", "([M:FB1->4])", "([M->5])", "([M->6])", "([C])", "(--)", "(--)"}}, // 74
        {{"([M->2])", "([M->3])", "([M->4])", "([M:FB1->5])", "([M->6])", "([C])", "(--)", "(--)"}}, // 75
        {{"([M->2])", "([M->3])", "([M->4])", "([M->5])", "([M:FB1->6])", "([C])", "(--)", "(--)"}}, // 76
        {{"([C:FB])", "([C:FB])", "([C:FB])", "([C:FB])", "([C:FB])", "([C:FB])", "(--)", "(--)"}},  // 77
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([M->5])", "([C])", "(--)", "(--)", "(--)"}},      // 78
        {{"([M->2])", "([M:FB1->3])", "([M->4])", "([M->5])", "([C])", "(--)", "(--)", "(--)"}},     // 79
        {{"([M->2])", "([M->3])", "([M:FB1->4])", "([M->5])", "([C])", "(--)", "(--)", "(--)"}},     // 80
        {{"([M->2])", "([M->3])", "([M->4])", "([M:FB1->5])", "([C])", "(--)", "(--)", "(--)"}},     // 81
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])", "([C])", "(--)", "(--)", "(--)"}},         // 82
        {{"([M->2])", "([M:FB1->3])", "([M->4])", "([C])", "([C])", "(--)", "(--)", "(--)"}},        // 83
        {{"([M->2])", "([M->3])", "([M:FB1->4])", "([C])", "([C])", "(--)", "(--)", "(--)"}},        // 84
        {{"([M->2])", "([M->3])", "([M->4])", "([C])", "([C:FB])", "(--)", "(--)", "(--)"}},         // 85
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])", "([C:FB])", "(--)", "(--)", "(--)"}},      // 86
        {{"([M->2])", "([M:FB1->3])", "([M->4])", "([C])", "([C:FB])", "(--)", "(--)", "(--)"}},     // 87
        {{"([M->2])", "([M->3])", "([M:FB1->4])", "([C])", "([C:FB])", "(--)", "(--)", "(--)"}},     // 88
        {{"([M:FB->2])", "([M->3])", "([C])", "([M->5])", "([C])", "(--)", "(--)", "(--)"}},         // 89
        {{"([M->2])", "([M:FB1->3])", "([C])", "([M->5])", "([C])", "(--)", "(--)", "(--)"}},        // 90
        {{"([M->2])", "([M->3])", "([C])", "([M:FB->5])", "([C])", "(--)", "(--)", "(--)"}},         // 91
        {{"([M:FB->2])", "([M->3])", "([C])", "([M:FB->5])", "([C])", "(--)", "(--)", "(--)"}},      // 92
        {{"([M->2])", "([M:FB1->3])", "([C])", "([M:FB->5])", "([C])", "(--)", "(--)", "(--)"}},     // 93
        {{"([M:FB->2/4])", "([M->3])", "([C])", "([M->5])", "([C])", "(--)", "(--)", "(--)"}},       // 94
        {{"([C:FB])", "([M->3])", "([M->4])", "([C])", "([C])", "(--)", "(--)", "(--)"}},            // 95
        {{"([C])", "([M:FB->3])", "([M->4])", "([C])", "([C])", "(--)", "(--)", "(--)"}},            // 96
        {{"([C])", "([M->3])", "([M:FB2->4])", "([C])", "([C])", "(--)", "(--)", "(--)"}},           // 97
        {{"([C:FB])", "([M:FB->3])", "([M->4])", "([C])", "([C])", "(--)", "(--)", "(--)"}},         // 98
        {{"([C:FB])", "([M->3])", "([M:FB2->4])", "([C])", "([C])", "(--)", "(--)", "(--)"}},        // 99
        {{"([C:FB])", "([M->3])", "([M->4])", "([C])", "([C:FB])", "(--)", "(--)", "(--)"} },        // 100
        { {"([C:FB])", "([M:FB->3])", "([M->4])", "([C])", "([C:FB])", "(--)", "(--)", "(--)"} },    // 101
        { {"([C])", "([M->3])", "([M:FB2->4])", "([C])", "([C])", "(--)", "(--)", "(--)"} },         // 102
        { {"([C])", "([M:FB->4])", "([M->4])", "([C])", "([C])", "(--)", "(--)", "(--)"} },          // 103
        { {"([C])", "([M:FB->4])", "([M-:FB>4])", "([C])", "([C])", "(--)", "(--)", "(--)"} },       // 104
        { {"([M:FB->2])", "([C])", "([M->4])", "([C])", "([C])", "(--)", "(--)", "(--)"} },          // 105
        { {"([M:FB->2])", "([C])", "([M:FB->4])", "([C])", "([C])", "(--)", "(--)", "(--)"} },       // 106
        { {"([M:FB->2])", "([C])", "([C])", "([C])", "([C])", "(--)", "(--)", "(--)"} },             // 107
        { {"([M->2])", "([C:FB1])", "([C])", "([C])", "([C])", "(--)", "(--)", "(--)"} },            // 108
        { {"([M:FB->2/3])", "([C])", "([C])", "([C])", "([C])", "(--)", "(--)", "(--)"} },              // 109
        { {"([M:FB->2/3/4])", "([C])", "([C])", "([C])", "([C])", "(--)", "(--)", "(--)"} },              // 110
        { {"([C:FB])", "([C])", "([C])", "([C])", "([C])", "(--)", "(--)", "(--)"} },                // 111
        { {"([C:FB])", "([C:FB])", "([C:FB])", "([C:FB])", "([C:FB])", "(--)", "(--)", "(--)"} },             // 112
        { {"([M:FB->2/3])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)", "(--)"} },                       // 113
        { {"([M:FB->2/3])", "([C])", "([C])", "([C])", "(--)", "(--)", "(--)", "(--)"} },                       // 114
        { {"([M:FB->2])", "([M->3])", "([M->4])", "([M->5])", "([M->6])", "[M->7]", "([C])", "(--)"} },       // 115
        { {"([M:FB->2])", "([M->3])", "([M->4])", "([C])", "([M->6])", "[M->7]", "([C])", "(--)"} },       // 116
        { {"([M:FB->2])", "([M->4])", "([M->4])", "([C])", "([M->7])", "[M->7]", "([C])", "(--)"} },       // 117
        { {"([M:FB->2])", "([M->3])", "([C])", "([M->5])", "([C])", "[M->7]", "([C])", "(--)"} },       // 118
        { {"([M:FB->2/3])", "([M->3])", "([C])", "([M->5])", "([C])", "[M->7]", "([C])", "(--)"} },       // 119
        { {"([M:FB->2])", "([C])", "([M->4])", "([C])", "([M->6])", "([C])", "([C])", "(--)"} },                       // 120
        { {"([C:FB])", "([C])", "([C])", "([C])", "([C])", "([C])", "([C])", "(--)"} },                       // 121
        { {"([M:FB->2])", "([M->3])", "([M->4])", "([M->5])", "([M->6])", "([M->7])", "([M->8])", "([C])"} },  // 122
        { {"([M:FB->2])", "([M->3])", "([M->4])", "([C])", "([M->6])", "([M->7])", "([M->8])", "([C])"} },  // 123
        { {"([M:FB->3])", "([M->3])", "([M->4])", "([C])", "([M->7])", "([M->7])", "([M->8])", "([C])"} },  // 124
        { {"([M:FB->2])", "([M->3])", "([C])", "([M->5])", "([M->6])", "([C])", "([M->8])", "([C])"} },  // 125
        { {"([M:FB->3])", "([M->3])", "([C])", "([M->6])", "([M->6])", "([C])", "([M->8])", "([C])"} },  // 126
        { {"([M:FB->2])", "([C])", "([M->4])", "([C])", "([M->6])", "([C])", "([M->8])", "([C])"} },  // 127
        { {"([M:FB->2])", "([C])", "([M->4])", "([C])", "([M->6])", "([C])", "([C])", "([C])"} },  // 128
        { {"([C:FB])", "([C])", "([C])", "([C])", "([C])", "([C])", "([C])", "([C])"} },                      // 129
    }};

    // アルゴリズムごとに利用可能なオペレーターを制限
    static inline const std::array<std::array<bool, Opzx7PrValue::ops>, Opzx7PrValue::algorithms> opEnableOnAlg = { {
    {{true, true, true, true, false, false, false, false}},    // 00: <OPX-00>
    {{true, true, true, true, false, false, false, false}},    // 01: <OPX-01>
    {{true, true, true, true, false, false, false, false}},    // 02: <OPX-02>
    {{true, true, true, true, false, false, false, false}},    // 03: <OPX-03>
    {{true, true, true, true, false, false, false, false}},    // 04: <OPX-04>
    {{true, true, true, true, false, false, false, false}},    // 05: <OPX-05>
    {{true, true, true, true, false, false, false, false}},    // 06: <OPX-06>
    {{true, true, true, true, false, false, false, false}},    // 07: <OPX-07>
    {{true, true, true, true, false, false, false, false}},    // 08: <OPX-08>
    {{true, true, true, true, false, false, false, false}},    // 09: <OPX-09>
    {{true, true, true, true, false, false, false, false}},    // 10: <OPX-10>
    {{true, true, true, true, false, false, false, false}},    // 11: <OPX-11>
    {{true, true, true, true, false, false, false, false}},    // 12: <OPX-12>
    {{true, true, true, true, false, false, false, false}},    // 13: <OPX-13>
    {{true, true, true, true, false, false, false, false}},    // 14: <OPX-14>
    {{true, true, true, true, false, false, false, false}},    // 15: <OPX-15>
    {{true, true, true, false, false, false, false, false}},   // 16: <OPX-16>
    {{true, true, true, false, false, false, false, false}},   // 17: <OPX-17>
    {{true, true, true, false, false, false, false, false}},   // 18: <OPX-18>
    {{true, true, true, false, false, false, false, false}},   // 19: <OPX-19>
    {{true, true, true, false, false, false, false, false}},   // 20: <OPX-20>
    {{true, true, true, false, false, false, false, false}},   // 21: <OPX-21>
    {{true, true, true, false, false, false, false, false}},   // 22: <OPX-22>
    {{true, true, true, false, false, false, false, false}},   // 23: <OPX-23>
    {{true, true, false, false, false, false, false, false}},  // 24: <OPX-24>
    {{true, true, false, false, false, false, false, false}},  // 25: <OPX-25>
    {{true, true, false, false, false, false, false, false}},  // 26: <OPX-26>
    {{true, true, false, false, false, false, false, false}},  // 27: <OPX-27>
    {{true, true, false, false, false, false, false, false}},  // 28: <MA3-00>
    {{true, true, false, false, false, false, false, false}},  // 29: <MA3-01>
    {{true, true, true, true, false, false, false, false}},    // 30: <MA3-02>
    {{true, true, true, true, false, false, false, false}},    // 31: <MA3-03>
    {{true, true, true, true, false, false, false, false}},    // 32: <MA3-04>
    {{true, true, true, true, false, false, false, false}},    // 33: <MA3-05>
    {{true, true, true, true, false, false, false, false}},    // 34: <MA3-06>
    {{true, true, true, true, false, false, false, false}},    // 35: <MA3-07>
    {{true, true, true, true, true, true, false, false}},      // 36: <OPS-00>
    {{true, true, true, true, true, true, false, false}},      // 37: <OPS-01>
    {{true, true, true, true, true, true, false, false}},      // 38: <OPS-02>
    {{true, true, true, true, true, true, false, false}},      // 39: <OPS-03>
    {{true, true, true, true, true, true, false, false}},      // 40: <OPS-04>
    {{true, true, true, true, true, true, false, false}},      // 41: <OPS-05>
    {{true, true, true, true, true, true, false, false}},      // 42: <OPS-06>
    {{true, true, true, true, true, true, false, false}},      // 43: <OPS-07>
    {{true, true, true, true, true, true, false, false}},      // 44: <OPS-08>
    {{true, true, true, true, true, true, false, false}},      // 45: <OPS-09>
    {{true, true, true, true, true, true, false, false}},      // 46: <OPS-10>
    {{true, true, true, true, true, true, false, false}},      // 47: <OPS-11>
    {{true, true, true, true, true, true, false, false}},      // 48: <OPS-12>
    {{true, true, true, true, true, true, false, false}},      // 49: <OPS-13>
    {{true, true, true, true, true, true, false, false}},      // 50: <OPS-14>
    {{true, true, true, true, true, true, false, false}},      // 51: <OPS-15>
    {{true, true, true, true, true, true, false, false}},      // 52: <OPS-16>
    {{true, true, true, true, true, true, false, false}},      // 53: <OPS-17>
    {{true, true, true, true, true, true, false, false}},      // 54: <OPS-18>
    {{true, true, true, true, true, true, false, false}},      // 55: <OPS-19>
    {{true, true, true, true, true, true, false, false}},      // 56: <OPS-20>
    {{true, true, true, true, true, true, false, false}},      // 57: <OPS-21>
    {{true, true, true, true, true, true, false, false}},      // 58: <OPS-22>
    {{true, true, true, true, true, true, false, false}},      // 59: <OPS-23>
    {{true, true, true, true, true, true, false, false}},      // 60: <OPS-24>
    {{true, true, true, true, true, true, false, false}},      // 61: <OPS-25>
    {{true, true, true, true, true, true, false, false}},      // 62: <OPS-26>
    {{true, true, true, true, true, true, false, false}},      // 63: <OPS-27>
    {{true, true, true, true, true, true, false, false}},      // 64: <OPS-28>
    {{true, true, true, true, true, true, false, false}},      // 65: <OPS-29>
    {{true, true, true, true, true, true, false, false}},      // 66: <OPS-30>
    {{true, true, true, true, true, true, false, false}},      // 67: <OPS-31>
    {{true, false, false, false, false, false, false, false}}, // 68: <OPZX7S-000>
    {{true, true, false, false, false, false, false, false}},  // 69: <OPZX7S-001>
    {{true, true, true, true, false, false, false, false}},    // 70: <OPZX7S-002>
    {{true, true, true, true, false, false, false, false}},    // 71: <OPZX7S-003>
    {{true, true, true, true, true, true, false, false}},      // 72
    {{true, true, true, true, true, true, false, false}},      // 73
    {{true, true, true, true, true, true, false, false}},      // 74
    {{true, true, true, true, true, true, false, false}},      // 75
    {{true, true, true, true, true, true, false, false}},      // 76
    {{true, true, true, true, true, true, false, false}},      // 77
    {{true, true, true, true, true, false, false, false}},     // 78
    {{true, true, true, true, true, false, false, false}},     // 79
    {{true, true, true, true, true, false, false, false}},     // 80
    {{true, true, true, true, true, false, false, false}},     // 81
    {{true, true, true, true, true, false, false, false}},     // 82
    {{true, true, true, true, true, false, false, false}},     // 83
    {{true, true, true, true, true, false, false, false}},     // 84
    {{true, true, true, true, true, false, false, false}},     // 85
    {{true, true, true, true, true, false, false, false}},     // 86
    {{true, true, true, true, true, false, false, false}},     // 87
    {{true, true, true, true, true, false, false, false}},     // 88
    {{true, true, true, true, true, false, false, false}},     // 89
    {{true, true, true, true, true, false, false, false}},     // 90
    {{true, true, true, true, true, false, false, false}},     // 91
    {{true, true, true, true, true, false, false, false}},     // 92
    {{true, true, true, true, true, false, false, false}},     // 93
    {{true, true, true, true, true, false, false, false}},     // 94
    {{true, true, true, true, true, false, false, false}},     // 95
    {{true, true, true, true, true, false, false, false}},     // 96
    {{true, true, true, true, true, false, false, false}},     // 97
    {{true, true, true, true, true, false, false, false}},     // 98
    {{true, true, true, true, true, false, false, false}},     // 99
    { {true, true, true, true, true, false, false, false} },   // 100
    { {true, true, true, true, true, false, false, false} },   // 101
    { {true, true, true, true, true, false, false, false} },   // 102
    { {true, true, true, true, true, false, false, false} },   // 103
    { {true, true, true, true, true, false, false, false} },   // 104
    { {true, true, true, true, true, false, false, false} },   // 105
    { {true, true, true, true, true, false, false, false} },   // 106
    { {true, true, true, true, true, false, false, false} },   // 107
    { {true, true, true, true, true, false, false, false} },   // 108
    { {true, true, true, true, true, false, false, false} },   // 109
    { {true, true, true, true, true, false, false, false} },   // 110
    { {true, true, true, true, true, false, false, false} },   // 111
    { {true, true, true, true, true, false, false, false} },   // 112
    { { true, true, true, false, false, false, false, false } },   // 113
    { { true, true, true, true, false, false, false, false } },   // 114
    { { true, true, true, true, true, true, true, false } },   // 115
    { { true, true, true, true, true, true, true, false } },   // 116
    { { true, true, true, true, true, true, true, false } },   // 117
    { { true, true, true, true, true, true, true, false } },   // 118
    { { true, true, true, true, true, true, true, false } },   // 119
    { { true, true, true, true, true, true, true, false } },   // 120
    { { true, true, true, true, true, true, true, false } },   // 121
    { { true, true, true, true, true, true, true, true } },    // 122
    { { true, true, true, true, true, true, true, true } },    // 123
    { { true, true, true, true, true, true, true, true } },    // 124
    { { true, true, true, true, true, true, true, true } },    // 125
    { { true, true, true, true, true, true, true, true } },    // 126
    { { true, true, true, true, true, true, true, true } },    // 127
    { { true, true, true, true, true, true, true, true } },    // 128
    { { true, true, true, true, true, true, true, true } }     // 129
} };

    GuiComponentViewModes viewMode = GuiComponentViewModes::Twin;

    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;
    GuiComponentViewMode viewModeComp;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    GuiCategoryLabel algFbCat;

    // AlgMatrix
    GuiComboBox algModeSelector;
    GuiOpzx7AlgMatrix algMatrixComp;
    GuiOpzx7AlgGraph algGraphComp;

    // Global
    GuiComboBox algSelector;
    NormalSeparator algFbSep;
    GuiFbSlider feedbackSlider;

    GuiCategoryLabel panCat;
    GuiToggleButton panpotEnableToggle;
    GuiSlider panpotSlider;
    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    // LFO
    GuiComponentLfoOpzx7 glLfo;

    // UNISON/HARMONY
	GuiComponentUnison unisonComponent;

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiTextButton copyOpParamBtn;
    GuiSlider copyOpFromSlider;
    GuiSlider copyOpToSlider;
    NormalSeparator uSep002;
    GuiComponentImportExport ieOpLfo;
    GuiComponentImportExport ieOpDetune;
    GuiComponentImportExport ieOpPitchEnv;
    GuiComponentImportExport ieOpSsgSwEnv;
    GuiComponentImportExport ieOpSsgSwEnv11;
    GuiComponentImportExport ieOpSsgSwPEnv11;
    GuiComponentImportExport ieOpPcmPlay;
    GuiSlider targerOpSlider;
    NormalSeparator uSep003;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::ImageComponent algImageComp;
    std::array<juce::Image, Opzx7PrValue::algorithms> algImages;

    std::array<GuiScrollGroup, Opzx7PrValue::ops> opGroups;
    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<GuiComponentMulDetune, Opzx7PrValue::ops> mulDetune;
    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catAmp;
    std::array<GuiSlider, Opzx7PrValue::ops> tl;
    std::array<GuiSlider, Opzx7PrValue::ops> ar;
    std::array<GuiSlider, Opzx7PrValue::ops> d1r;
    std::array<GuiSlider, Opzx7PrValue::ops> d1l;
    std::array<GuiSlider, Opzx7PrValue::ops> d2r;
    std::array<GuiSlider, Opzx7PrValue::ops> rr;

    std::array<GuiCategoryLabel, Opzx7PrValue::ops> ksCat;
    std::array<GuiToggleButton, Opzx7PrValue::ops> ksEn;
    std::array<GuiComboBox, Opzx7PrValue::ops> ksMode;
    std::array<GuiToggleButton, Opzx7PrValue::ops> ksrMA7;
    std::array<GuiComboBox, Opzx7PrValue::ops> kslMA7;
    std::array<GuiSlider, Opzx7PrValue::ops> ksrOPZ;
    std::array<GuiSlider, Opzx7PrValue::ops> kslOPZ;
    std::array<GuiSlider, Opzx7PrValue::ops> ksBp;
    std::array<GuiComboBox, Opzx7PrValue::ops> ksLc;
    std::array<GuiComboBox, Opzx7PrValue::ops> ksRc;
    std::array<GuiSlider, Opzx7PrValue::ops> ksLd;
    std::array<GuiSlider, Opzx7PrValue::ops> ksRd;
    std::array<GuiSlider, Opzx7PrValue::ops> ksRs;

    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catOptional;
    std::array<GuiToggleButton, Opzx7PrValue::ops> bypass;
    std::array<GuiComponentFix, Opzx7PrValue::ops> fix;
    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catWaveShape;
    std::array<GuiComboBox, Opzx7PrValue::ops> ws;
    std::array<NormalSeparator, Opzx7PrValue::ops> wsSeparator;
    std::array<GuiTextButton, Opzx7PrValue::ops> loadPcmBtn;
    std::array<GuiTextButton, Opzx7PrValue::ops> clearPcmBtn;
    std::array<GuiLabel, Opzx7PrValue::ops> pcmFileNameLabel;
    std::array<GuiSlider, Opzx7PrValue::ops> pcmOffset;
    std::array<GuiSlider, Opzx7PrValue::ops> pcmRatio;
    std::array<GuiToggleButton, Opzx7PrValue::ops> loopPointEnable;
    std::array<GuiSlider, Opzx7PrValue::ops> loopPointStart;
    std::array<GuiSlider, Opzx7PrValue::ops> loopPointEnd;
    std::array<GuiTextButton, Opzx7PrValue::ops> loadWtBtn;
    std::array<GuiTextButton, Opzx7PrValue::ops> clearWtBtn;
    std::array<GuiLabel, Opzx7PrValue::ops> wtFileNameLabel;
    std::array<GuiTextButton, Opzx7PrValue::ops> loadWt2Btn;
    std::array<GuiTextButton, Opzx7PrValue::ops> clearWt2Btn;
    std::array<GuiLabel, Opzx7PrValue::ops> wt2FileNameLabel;
    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catSsgEnv;
    std::array<GuiComboBox, Opzx7PrValue::ops> se;
    std::array<GuiSlider, Opzx7PrValue::ops> seFreq;

    // LFO
    std::array<GuiComponentLfoOpzx7, Opzx7PrValue::ops> lfo;

    // Pitch ADSR
    std::array<GuiComponentPitchEnv, Opzx7PrValue::ops> pitchEnv;
    // SSG SW Env
    std::array<GuiComponentSsgSwEnv, Opzx7PrValue::ops> ssgSwEnv;

    std::array<GuiComponentSsgSwEnv11, Opzx7PrValue::ops> ssgSwEnv11;
    std::array<GuiComponentSsgSwPEnv11, Opzx7PrValue::ops> ssgSwPEnv11;

    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catMask;
    std::array<GuiToggleButton, Opzx7PrValue::ops> mask; // Mask
    std::array<NormalSeparator, Opzx7PrValue::ops> mmlSeparator;
    std::array<GuiMmlButton, Opzx7PrValue::ops> mml;

    std::array<GuiToggleButton, Opzx7PrValue::ops> rgEn;
    std::array<NormalSeparator, Opzx7PrValue::ops> rgSeparator;
    std::array<GuiSlider, Opzx7PrValue::ops> rgAr;
    std::array<GuiSlider, Opzx7PrValue::ops> rgD1r;
    std::array<GuiSlider, Opzx7PrValue::ops> rgD2r;
    std::array<GuiSlider, Opzx7PrValue::ops> rgD1l;
    std::array<GuiSlider, Opzx7PrValue::ops> rgRr;
    std::array<GuiSlider, Opzx7PrValue::ops> rgTl;

    std::array<NormalSeparator, Opzx7PrValue::ops> optionalSeparator;
    std::array<GuiToggleButton, Opzx7PrValue::ops> sus; // Sus
    std::array<GuiToggleButton, Opzx7PrValue::ops> xof; // Xof
    std::array<GuiToggleButton, Opzx7PrValue::ops> kor;

    void applyMmlString(const juce::String& mml, int opIndex);

    std::array<GuiEnvelopeGraph, Opzx7PrValue::ops> opGraphs;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnSsg;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnSsg11;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnSsgP11;
    std::array<NormalSeparator, Opzx7PrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };
    std::array<GraphMode, Opzx7PrValue::ops> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    bool isUpdatingGraph = false;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
    GuiOpzx7(const GuiContext& context);

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePcmFileName(int opIndex, const juce::String& fileName) {
        pcmFileNameLabel[opIndex].setText(fileName, juce::dontSendNotification);
    }
    void updateWtFileName(int opIndex, const juce::String& fileName) {
		wtFileNameLabel[opIndex].setText(fileName, juce::dontSendNotification);
    }
    void updateWt2FileName(int opIndex, const juce::String& fileName) {
        wt2FileNameLabel[opIndex].setText(fileName, juce::dontSendNotification);
    }
    void updateAllPcmFileName(const juce::String& fileName) {
        for (int i = 0; i < Opzx7PrValue::ops; i++)
        {
            pcmFileNameLabel[i].setText(fileName, juce::dontSendNotification);
        }
    }
    void updateAllWtFileName(const juce::String& fileName) {
        for (int i = 0; i < Opzx7PrValue::ops; i++)
        {
            wtFileNameLabel[i].setText(fileName, juce::dontSendNotification);
        }
    }
    void updateAllWt2FileName(const juce::String& fileName) {
        for (int i = 0; i < Opzx7PrValue::ops; i++)
        {
            wt2FileNameLabel[i].setText(fileName, juce::dontSendNotification);
        }
    }
    void updateOpVisible(int idx, bool visible);
    void updateOpEnable(int idx, bool enable);
	void updateOnWsChange(int idx);
    void updateAlgorithmDisplay();
    void updateRgDisplayAsOp(int idx, bool rgMode);
    void updatePresetName(const juce::String& name);
    bool keyPressed(const juce::KeyPress& key) override;
    void copyFmParamsToString();
    void copyFmParamsToObject();
    void pasteFmParamsFromObject();
    void initParams();
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutOp(int opIndex, int width, juce::Rectangle<int>& rect);
    void layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanpotCat(juce::Rectangle<int>& rect);
    void layoutOpSsgEnvCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect, bool rgMode);
    void layoutOpAmpCat(int opIndex, juce::Rectangle<int>& rect, bool rgMode);
    void layoutOpWsCat(int opIndex, juce::Rectangle<int>& rect, int selectedWs);
    void setupGraph(int opIndex);
    void layoutOpGraph(int opIndex, juce::Rectangle<int>& rect);
    void setLevel(float level);
    void copyParams(CopyOpzx7& copyObj);
    void copyOpParams(int p, CopyOpzx7Op& copyObj);
    void pasteParams(CopyOpzx7& copyObj);
    void pasteOpParams(int p, CopyOpzx7Op& copyObj);
    void importOpLfoParam(int opIndex);
    void exportOpLfoParam(int opIndex);
    void importLfoParam();
    void exportLfoParam();
    void importPitchEnvParam(int opIndex);
    void exportPitchEnvParam(int opIndex);
    void importSsgSwEnvParam(int opIndex);
    void exportSsgSwEnvParam(int opIndex);
    void importSsgSwEnv11Param(int opIndex);
    void exportSsgSwEnv11Param(int opIndex);
    void importSsgSwPEnv11Param(int opIndex);
    void exportSsgSwPEnv11Param(int opIndex);
    void importDetuneParam(int opIndex);
    void exportDetuneParam(int opIndex);
    void importUnisonParam();
    void exportUnisonParam();
    void importQualityParam();
    void exportQualityParam();
    void importOpPcmPlayParam(int opIndex);
    void exportOpPcmPlayParam(int opIndex);
};
