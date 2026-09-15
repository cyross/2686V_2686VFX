#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "../../Core/Editor/EditorGuiText.h"
#include "./GuiOpzx7.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat qualityFormat{ "quality", 1 };
	const Io::ParamFormat opzx7Format{ "opzx7", 1 };
	const Io::ParamFormat opzx7OpFormat{ "opzx7Op", 1 };
	const Io::ParamFormat pcmPlayFormat{ "pcmPlay", 1 };
}

#include "../Components/WavePreview/WavePreviewSource.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Keys.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"
#include "../../Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7AdddrParams.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiGraphValues.h"
#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOpzx7Values.h"
#include "./GuiOpzx7Text.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiOpzx7Helpers.h"

#include "../../Core/Processor/PluginProcessorStateKey.h"

static std::vector<SelectItem> opzx7AlgItems = {
    {.name = "000: <OPX-00> / 4OP", .value = 1 },
    {.name = "001: <OPX-01> / 4OP", .value = 2 },
    {.name = "002: <OPX-02> / 4OP", .value = 3 },
    {.name = "003: <OPX-03> / 4OP", .value = 4 },
    {.name = "004: <OPX-04> / 4OP", .value = 5 },
    {.name = "005: <OPX-05> / 4OP", .value = 6 },
    {.name = "006: <OPX-06> / 2OPx2", .value = 7 },
    {.name = "007: <OPX-07> / 2OPx2", .value = 8 },
    {.name = "008: <OPX-08> / 4OP", .value = 9 },
    {.name = "009: <OPX-09> / 4OP", .value = 10 },
    {.name = "010: <OPX-10> / 4OP", .value = 11 },
    {.name = "011: <OPX-11> / 4OP", .value = 12 },
    {.name = "012: <OPX-12> / 4OP", .value = 13 },
    {.name = "013: <OPX-13> / 4OP", .value = 14 },
    {.name = "014: <OPX-14> / 4OP", .value = 15 },
    {.name = "015: <OPX-15> / 4OP", .value = 16 },
    {.name = "016: <OPX-16> / 3OP", .value = 17 },
    {.name = "017: <OPX-17> / 3OP", .value = 18 },
    {.name = "018: <OPX-18> / 3OP", .value = 19 },
    {.name = "019: <OPX-19> / 3OP", .value = 20 },
    {.name = "020: <OPX-20> / 3OP", .value = 21 },
    {.name = "021: <OPX-21> / 3OP", .value = 22 },
    {.name = "022: <OPX-22> / 3OP", .value = 23 },
    {.name = "023: <OPX-23> / 3OP", .value = 24 },
    {.name = "024: <OPX-24> / 2OP", .value = 25 },
    {.name = "025: <OPX-25> / 2OP", .value = 26 },
    {.name = "026: <OPX-26> / 2OP", .value = 27 },
    {.name = "027: <OPX-27> / 2OP", .value = 28 },
    {.name = "028: <MA7-00> / 2OP", .value = 29 },
    {.name = "029: <MA7-01> / 2OP", .value = 30 },
    {.name = "030: <MA7-02> / 2OPx2", .value = 31 },
    {.name = "031: <MA7-03> / 4OP", .value = 32 },
    {.name = "032: <MA7-04> / 4OP", .value = 33 },
    {.name = "033: <MA7-05> / 2OPx2", .value = 34 },
    {.name = "034: <MA7-06> / 4OP", .value = 35 },
    {.name = "035: <MA7-07> / 4OP", .value = 36 },
    {.name = "036: <OPS-00> / 6OP", .value = 37 },
    {.name = "037: <OPS-01> / 6OP", .value = 38 },
    {.name = "038: <OPS-02> / 3OPx2", .value = 39 },
    {.name = "039: <OPS-03> / 3OPx2", .value = 40 },
    {.name = "040: <OPS-04> / 2OPx3", .value = 41 },
    {.name = "041: <OPS-05> / 2OPx3", .value = 42 },
    {.name = "042: <OPS-06> / 6OP", .value = 43 },
    {.name = "043: <OPS-07> / 6OP", .value = 44 },
    {.name = "044: <OPS-08> / 6OP", .value = 45 },
    {.name = "045: <OPS-09> / 6OP", .value = 46 },
    {.name = "046: <OPS-10> / 6OP", .value = 47 },
    {.name = "047: <OPS-11> / 6OP", .value = 48 },
    {.name = "048: <OPS-12> / 6OP", .value = 49 },
    {.name = "049: <OPS-13> / 6OP", .value = 50 },
    {.name = "050: <OPS-14> / 6OP", .value = 51 },
    {.name = "051: <OPS-15> / 6OP", .value = 52 },
    {.name = "052: <OPS-16> / 6OP", .value = 53 },
    {.name = "053: <OPS-17> / 6OP", .value = 54 },
    {.name = "054: <OPS-18> / 6OP", .value = 55 },
    {.name = "055: <OPS-19> / 6OP", .value = 56 },
    {.name = "056: <OPS-20> / 6OP", .value = 57 },
    {.name = "057: <OPS-21> / 6OP", .value = 58 },
    {.name = "058: <OPS-22> / 6OP", .value = 59 },
    {.name = "059: <OPS-23> / 6OP", .value = 60 },
    {.name = "060: <OPS-24> / 6OP", .value = 61 },
    {.name = "061: <OPS-25> / 6OP", .value = 62 },
    {.name = "062: <OPS-26> / 6OP", .value = 63 },
    {.name = "063: <OPS-27> / 6OP", .value = 64 },
    {.name = "064: <OPS-28> / 6OP", .value = 65 },
    {.name = "065: <OPS-29> / 6OP", .value = 66 },
    {.name = "066: <OPS-30> / 6OP", .value = 67 },
    {.name = "067: <OPS-31> / 6OP", .value = 68 },
    {.name = "068: <OPZX7S-000> / 1OP", .value = 69 },
    {.name = "069: <OPZX7S-001> / 1OPx2", .value = 70 },
    {.name = "070: <OPZX7S-002> / 2OPx2", .value = 71 },
    {.name = "071: <OPZX7S-003> / 4OP", .value = 72 },
    {.name = "072: <OPZX7S-004> / 6OP", .value = 73 },
    {.name = "073: <OPZX7S-005> / 6OP", .value = 74 },
    {.name = "074: <OPZX7S-006> / 6OP", .value = 75 },
    {.name = "075: <OPZX7S-007> / 6OP", .value = 76 },
    {.name = "076: <OPZX7S-008> / 6OP", .value = 77 },
    {.name = "077: <OPZX7S-009> / 6OP", .value = 78 },
    {.name = "078: <OPZX7S-010> / 5OP", .value = 79 },
    {.name = "079: <OPZX7S-011> / 5OP", .value = 80 },
    {.name = "080: <OPZX7S-012> / 5OP", .value = 81 },
    {.name = "081: <OPZX7S-013> / 5OP", .value = 82 },
    {.name = "082: <OPZX7S-014> / 5OP", .value = 83 },
    {.name = "083: <OPZX7S-015> / 5OP", .value = 84 },
    {.name = "084: <OPZX7S-016> / 5OP", .value = 85 },
    {.name = "085: <OPZX7S-017> / 5OP", .value = 86 },
    {.name = "086: <OPZX7S-018> / 5OP", .value = 87 },
    {.name = "087: <OPZX7S-019> / 5OP", .value = 88 },
    {.name = "088: <OPZX7S-020> / 5OP", .value = 89 },
    {.name = "089: <OPZX7S-021> / 5OP", .value = 90 },
    {.name = "090: <OPZX7S-022> / 5OP", .value = 91 },
    {.name = "091: <OPZX7S-023> / 5OP", .value = 92 },
    {.name = "092: <OPZX7S-024> / 5OP", .value = 93 },
    {.name = "093: <OPZX7S-025> / 5OP", .value = 94 },
    {.name = "094: <OPZX7S-026> / 5OP", .value = 95 },
    {.name = "095: <OPZX7S-027> / 5OP", .value = 96 },
    {.name = "096: <OPZX7S-028> / 5OP", .value = 97 },
    {.name = "097: <OPZX7S-029> / 5OP", .value = 98 },
    {.name = "098: <OPZX7S-030> / 5OP", .value = 99 },
    {.name = "099: <OPZX7S-031> / 5OP", .value = 100 },
    { .name = "100: <OPZX7S-032> / 5OP", .value = 101 },
    { .name = "101: <OPZX7S-033> / 5OP", .value = 102 },
    { .name = "102: <OPZX7S-034> / 5OP", .value = 103 },
    { .name = "103: <OPZX7S-035> / 5OP", .value = 104 },
    { .name = "104: <OPZX7S-036> / 5OP", .value = 105 },
    { .name = "105: <OPZX7S-037> / 5OP", .value = 106 },
    { .name = "106: <OPZX7S-038> / 5OP", .value = 107 },
    { .name = "107: <OPZX7S-039> / 5OP", .value = 108 },
    { .name = "108: <OPZX7S-040> / 5OP", .value = 109 },
    { .name = "109: <OPZX7S-041> / 5OP", .value = 110 },
    { .name = "110: <OPZX7S-042> / 5OP", .value = 111 },
    { .name = "111: <OPZX7S-043> / 5OP", .value = 112 },
    { .name = "112: <OPZX7S-044> / 5OP", .value = 113 },
    { .name = "113: <OPZX7S-045> / 3OP", .value = 114 },
    { .name = "114: <OPZX7S-046> / 4OP", .value = 115 },
    { .name = "115: <OPZX7S-047> / 7OP", .value = 116 },
    { .name = "116: <OPZX7S-048> / 7OP", .value = 117 },
    { .name = "117: <OPZX7S-049> / 7OP", .value = 118 },
    { .name = "118: <OPZX7S-050> / 7OP", .value = 119 },
    { .name = "119: <OPZX7S-051> / 7OP", .value = 120 },
    { .name = "120: <OPZX7S-052> / 7OP", .value = 121 },
    { .name = "121: <OPZX7S-053> / 7OP", .value = 122 },
    { .name = "122: <OPZX7S-054> / 8OP", .value = 123 },
    { .name = "123: <OPZX7S-055> / 8OP", .value = 124 },
    { .name = "124: <OPZX7S-056> / 8OP", .value = 125 },
    { .name = "125: <OPZX7S-057> / 8OP", .value = 126 },
    { .name = "126: <OPZX7S-058> / 8OP", .value = 127 },
    { .name = "127: <OPZX7S-059> / 8OP", .value = 128 },
    { .name = "128: <OPZX7S-060> / 8OP", .value = 129 },
    { .name = "129: <OPZX7S-061> / 8OP", .value = 130 },
};

static std::vector<SelectItem> multems = {
    { .name = " 0: x  0.5",    .value =  1 },
    { .name = " 1: x  0.891",  .value =  2 },
    { .name = " 2: x  1",      .value =  3 },
    { .name = " 3: x  1.414",  .value =  4 },
    { .name = " 4: x  1.498",  .value =  5 },
    { .name = " 5: x  1.581",  .value =  6 },
    { .name = " 6: x  1.781",  .value =  7 },
    { .name = " 7: x  2",      .value =  8 },
    { .name = " 8: x  3",      .value =  9 },
    { .name = " 9: x  4",      .value = 10 },
    { .name = "10: x  5",      .value = 11 },
    { .name = "11: x  6",      .value = 12 },
    { .name = "12: x  7",      .value = 13 },
    { .name = "13: x  8",      .value = 14 },
    { .name = "14: x  9",      .value = 15 },
    { .name = "15: x 10",      .value = 16 },
    { .name = "16: x 11",      .value = 17 },
    { .name = "17: x 12",      .value = 18 },
    { .name = "18: x 13",      .value = 19 },
    { .name = "19: x 14",      .value = 20 },
    { .name = "20: x 15",      .value = 21 },
    { .name = "21: Use Ratio", .value = 22 }
};

static std::vector<SelectItem> lfoShapeItems = {
    {.name = "0: Sine",                .value = 1 },
    {.name = "1: Saw Up",              .value = 2 },
    {.name = "2: Saw Down",            .value = 3 },
    {.name = "3: Square",              .value = 4 },
    {.name = "4: Triangle",            .value = 5 },
    {.name = "5: Sample & Hold",       .value = 6 },
    {.name = "6: Saw Down & One Shot", .value = 7 },
    {.name = "7: Triangle & One Shot", .value = 8 },
};

static std::vector<SelectItem> opnaSeItems = {
    {.name = "0: Normal",                      .value = 1 },
    {.name = "1: Saw Down",                    .value = 2 },
    {.name = "2: Saw Down & Hold",             .value = 3 },
    {.name = "3: Triangle",                    .value = 4 },
    {.name = "4: Alternative Saw Down & Hold", .value = 5 },
    {.name = "5: Saw Up",                      .value = 6 },
    {.name = "6: Saw Up & Hold",               .value = 7 },
    {.name = "7: Triangle Invert",             .value = 8 },
    {.name = "8: Alternative Saw Up & Hold",   .value = 9 },
};

static std::vector<SelectItem> opzx7WsItems = {
    {.name = "00 [MA-7:00]Sine/OPZ(TX81Z) W1",               .value = 1},
    {.name = "01 [MA-7:01]Half Sine",                        .value = 2},
    {.name = "02 [MA-7:02]Abs Sine",                         .value = 3},
    {.name = "03 [MA-7:03]Quadra Abs Half Sin",              .value = 4},
    {.name = "04 [MA-7:04]Alt Sine",                         .value = 5},
    {.name = "05 [MA-7:05]Alt Abs Sine",                     .value = 6},
    {.name = "06 [MA-7:06]Square",                           .value = 7},
    {.name = "07 [MA-7:07]Log Saw",                          .value = 8},
    {.name = "08 [MA-7:08]Pudding Sine",                     .value = 9},
    {.name = "09 [MA-7:09]Half Pudding Sine",                .value = 10},
    {.name = "10 [MA-7:10]Abs Pudding Sine",                 .value = 11},
    {.name = "11 [MA-7:11]Quad Abs Pudding Sine",            .value = 12},
    {.name = "12 [MA-7:12]Mini Alt Sine",                    .value = 13},
    {.name = "13 [MA-7:13]Mini Alt Abs Sine",                .value = 14},
    {.name = "14 [MA-7:14]Half Square",                      .value = 15},
    {.name = u8"15 ★★ WT File",                   .value = 16},
    {.name = "16 [MA-7:16]Triangle",                         .value = 17},
    {.name = "17 [MA-7:17]Half Triangle",                    .value = 18},
    {.name = "18 [MA-7:18]Abs Triangle",                     .value = 19},
    {.name = "19 [MA-7:19]Quad Abs Triangle",                .value = 20},
    {.name = "20 [MA-7:20]Alt Triangle",                     .value = 21},
    {.name = "21 [MA-7:21]Alt Abs Triangle",                 .value = 22},
    {.name = "22 [MA-7:22]Quad Half Square",                 .value = 23},
    {.name = u8"23 ★★ WT2 File",                  .value = 24},
    {.name = "24 [MA-7:24]Diagram",                          .value = 25},
    {.name = "25 [MA-7:25]Half Diagram",                     .value = 26},
    {.name = "26 [MA-7:26]Abs Half Saw Up",                  .value = 27},
    {.name = "27 [MA-7:27]Quad Abs Half Saw Up",             .value = 28},
    {.name = "28 [MA-7:28]Alt Diagram",                      .value = 29},
    {.name = "29 [MA-7:29]Alt Quad Abs Half Saw Up",         .value = 30},
    {.name = "30 [MA-7:30]Quad Square",                      .value = 31},
    {.name = u8"31 ★★ PCM(Audio) File",           .value = 32},
    {.name = "32 [EX000]Alternating Abs Sine",      .value = 33},
    {.name = "33 [EX001]Derived Square",            .value = 34},
    {.name = "34 [EX002]Saw Down",                  .value = 35},
    {.name = "35 [EX003]Saw Up",                    .value = 36},
    {.name = "36 [EX004]Saw + Sine",                .value = 37},
    {.name = "37 [EX005]Pulse 25%",                 .value = 38},
    {.name = "38 [EX006]Pulse 12.5%",               .value = 39},
    {.name = "39 [EX007]Pulse 6.25%",               .value = 40},
    {.name = "40 [EX008]Round Square",              .value = 41},
    {.name = "41 [EX009]Impulse Train",             .value = 42},
    {.name = "42 [EX010]Comb / Multi-pulse",        .value = 43},
    {.name = "43 [EX011]Resonant Saw (Low)",        .value = 44},
    {.name = "44 [EX012]Resonant Saw (High)",       .value = 45},
    {.name = "45 [EX013]Resonant Triangle",         .value = 46},
    {.name = "46 [EX014]Bulb Sine",                 .value = 47},
    {.name = "47 [EX015]Double Hump",               .value = 48},
    {.name = "48 [EX016]Pseudo Voice Formant 1",    .value = 49},
    {.name = "49 [EX017]Pseudo Voice Formant 2",    .value = 50},
    {.name = "50 [EX018]Metallic 1",                .value = 51},
    {.name = "51 [EX019]Metallic 2",                .value = 52},
    {.name = "52 [EX020]Noise-Like",                .value = 53},
    {.name = "53 [EX021]PD Resonance",              .value = 54},
    {.name = "54 [EX022]PD Resonance High",         .value = 55},
    {.name = "55 [EX023]4-Step Sine",               .value = 56},
    {.name = "56 [EX024]8-Step Sine",               .value = 57},
    {.name = "57 [EX025]Wavefolded Sine (Soft)",    .value = 58},
    {.name = "58 [EX026]Wavefolded Sine (Hard)",    .value = 59},
    {.name = "59 [EX027]Bitwise XOR Fractal",       .value = 60},
    {.name = "60 [EX028]Bitwise AND Texture",       .value = 61},
    {.name = "61 [EX029]Self-Modulated Sine(FB=1)", .value = 62},
    {.name = "62 [EX030]Self-Modulated Sine(FB=2)", .value = 63},
    {.name = "63 [EX031]OPZ(TX81Z) W2",             .value = 64},
    {.name = "64 [EX032]OPZ(TX81Z) W3",             .value = 65},
    {.name = "65 [EX033]OPZ(TX81Z) W4",             .value = 66},
    {.name = "66 [EX034]OPZ(TX81Z) W5",             .value = 67},
    {.name = "67 [EX035]OPZ(TX81Z) W6",             .value = 68},
    {.name = "68 [EX036]OPZ(TX81Z) W7",             .value = 69},
    {.name = "69 [EX037]OPZ(TX81Z) W8",             .value = 70},
    {.name = "70 [EX038]Cubic Triangle",            .value = 71},
    {.name = "71 [EX039]Inverse Circle",            .value = 72},
    {.name = "72 [EX040]Exponential Spike",         .value = 73},
};

static std::vector<SelectItem> ksModeItems = {
    {.name = "0: MA7", .value = 1 },
    {.name = "1: OPZ", .value = 2 },
    {.name = "2: OPS", .value = 3 }
};

static std::vector<SelectItem> kslMA7Items = {
    {.name = "KSL: 0 OFF",         .value = 1},
    {.name = "KSL: 1 (1.5dB/oct)", .value = 2},
    {.name = "KSL: 2 (3.0dB/oct)", .value = 3},
    {.name = "KSL: 3 (6.0db/oct)", .value = 4}
};

static std::vector<SelectItem> ksCurveItems = {
    {.name = "0: -LIN", .value = 1 },
    {.name = "1: -EXP", .value = 2 },
    {.name = "2: +EXP", .value = 3 },
    {.name = "3: +LIN", .value = 4 }
};

static std::vector<SelectItem> algModeItems = {
    {.name = juce::String("") + "0: 組み込みアルゴリズム", .value = 1 },
    {.name = juce::String("") + "1: アルゴリズムマトリックス", .value = 2 }
};

GuiOpzx7::GuiOpzx7(const GuiContext& context) :
    GuiBase(context),
    mainGroup(context),
    presetName(context),
    algFbCat(context),
    algModeSelector(context),
    algMatrixComp(context, Opzx7PrValue::ops), // 引数でオペレータ数を渡す
    levelComponent(context),
    qualityComponent(context),
    algSelector(context),
    algFbSep(context),
    feedbackTarget(context),
    feedbackSlider(context),
    feedbackNudge(context),
    panCat(context),
    panpotEnableToggle(context),
    panpotSlider(context),
    panToLBtn(context),
    panToCBtn(context),
    panToRBtn(context),
    glLfo(context),
    ampEnvComponent(context),
    modComponent(context),
    ampModComponent(context),
    ssgHwEnv(context),
    ssgHwPEnv(context),
    ssgSwEnv11g(context),
    ssgSwPEnv11g(context),
    unisonComponent(context),
    utilityCat(context),
    ampMajorCat(context),
    pitchMajorCat(context),
    broadcastLevelButton(context),
    uSep001(context),
    copyOpParamBtn(context),
    copyOpFromSlider(context),
    copyOpToSlider(context),
    uSep002(context),
    ieOpLfo(context),
    ieOpDetune(context),
    ieOpPitchEnv(context),
    ieOpSsgSwEnv(context),
    ieOpSsgSwEnv11(context),
    ieOpSsgSwPEnv11(context),
    ieOpSsgHwPEnv(context),
    ieOpWtAmpMod(context),
    ieOpSsgHwEnv(context),
    ieOpWtMod(context),
    ieOpPcmPlay(context),
    ieOpChParam(context),
    targerOpSlider(context),
    uSep003(context),
    ieAmpEnvG(context),
    ieSsgHwEnv(context),
    ieSsgHwPEnv(context),
    ieWtMod(context),
    ieWtAmpMod(context),
    ieSsgSwEnv11(context),
    ieSsgSwPEnv11g(context),
    ieLfo(context),
    ieUnison(context),
    ieQuality(context),
    ieChParam(context),
    opNameLabel(context),
    opTargetSeparator(context),
    colAmp(context),
    colWs(context),
    colSsgHwEnv(context),
    colSsgSwEnv(context),
    colSsgSwEnv11(context),
    colAmpMod(context),
    colPitchEnv(context),
    colSsgHwPEnv(context),
    colSsgSwPEnv11(context),
    colMod(context),
    colKs(context),
    colLfo(context),
    colMask(context),
    mulDetune(context),
    catAmp(context),
    tl(context),
    ar(context),
    d1r(context),
    d1l(context),
    d2r(context),
    rr(context),
    ksCat(context),
    ksEn(context),
    ksMode(context),
    ksrMA7(context),
    kslMA7(context),
    ksrOPZ(context),
    kslOPZ(context),
    ksBp(context),
    ksLc(context),
    ksRc(context),
    ksLd(context),
    ksRd(context),
    ksRs(context),
    catOptional(context),
    bypass(context),
    fix(context),
    catWaveShape(context),
    ws(context),
    wsSeparator(context),
    wsPreview(context),
    loadPcmBtn(context),
    clearPcmBtn(context),
    pcmFileNameLabel(context),
    loopCount(context),
    loopCountButtons(context),
    speed(context),
    pcmOffset(context),
    pcmRatio(context),
    loopPointEnable(context),
    loopPointStart(context),
    loopPointEnd(context),
    loadWtBtn(context),
    clearWtBtn(context),
    wtFileNameLabel(context),
    loadWt2Btn(context),
    clearWt2Btn(context),
    wt2FileNameLabel(context),
    catSsgEnv(context),
    se(context),
    seFreq(context),
    sePreview(context),
    lfo(context),
    pitchEnv(context),
    ssgSwEnv(context),
	ssgSwEnv11(context),
    ssgSwPEnv11(context),
        ssgHwPEnvOp(context),
        wtAmpModOp(context),
        ssgHwEnvOp(context),
        wtModOp(context),
    catMask(context),
    mask(context),
    mmlSeparator(context),
    mml(context),
    rgEn(context),
    rgSeparator(context),
    rgAr(context),
    rgD1r(context),
    rgD2r(context),
    rgD1l(context),
    rgRr(context),
    rgTl(context),
    optionalSeparator(context),
    sus(context),
    opDelay(context),
    opDelayNudge(context),
    opDelaySeparator(context),
    xof(context),
    kor(context),
    midiComponent(context),
    gGraph(),
    gGraphBtnAmp(context),
    gGraphBtnSsg11(context),
    gGraphBtnSsgP11(context),
    gGraphSeparator(context),
    graphBtnAmp(context),
    graphBtnPitch(context),
    graphBtnSsg(context),
    graphBtnSsg11(context),
    graphBtnSsgP11(context),
    cells{ GuiTargetCell(context), GuiTargetCell(context), GuiTargetCell(context), GuiTargetCell(context), GuiTargetCell(context), GuiTargetCell(context), GuiTargetCell(context), GuiTargetCell(context) }
{
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);

}

float GuiOpzx7::getFeedbackValue(int opIndex) const
{
    static const juce::String keys[] = { CPK::Fm::fb1, CPK::Fm::fb2, CPK::Fm::fb3, CPK::Fm::fb4,
                                         CPK::Fm::fb5, CPK::Fm::fb6, CPK::Fm::fb7, CPK::Fm::fb8 };

    if (opIndex < 0 || opIndex >= Opzx7PrValue::ops) return 0.0f;

    auto* v = ctx.apvts.getRawParameterValue(Opzx7PrKey::prefix + keys[opIndex]);

    return (v != nullptr) ? v->load() : 0.0f;
}

void GuiOpzx7::setFeedbackValue(int opIndex, float value)
{
    static const juce::String keys[] = { CPK::Fm::fb1, CPK::Fm::fb2, CPK::Fm::fb3, CPK::Fm::fb4,
                                         CPK::Fm::fb5, CPK::Fm::fb6, CPK::Fm::fb7, CPK::Fm::fb8 };

    if (opIndex < 0 || opIndex >= Opzx7PrValue::ops) return;

    if (auto* p = ctx.apvts.getParameter(Opzx7PrKey::prefix + keys[opIndex])) {
        p->setValueNotifyingHost(p->convertTo0to1(value));
    }
}

void GuiOpzx7::rebindFeedback()
{
    static const juce::String keys[] = { CPK::Fm::fb1, CPK::Fm::fb2, CPK::Fm::fb3, CPK::Fm::fb4,
                                         CPK::Fm::fb5, CPK::Fm::fb6, CPK::Fm::fb7, CPK::Fm::fb8 };

    const int idx = juce::jlimit(0, Opzx7PrValue::ops - 1, (int)feedbackTarget.getValue() - 1);

    feedbackSlider.getSlider().rebind(Opzx7PrKey::prefix + keys[idx]);

    refreshFeedbackValues();
}

void GuiOpzx7::refreshFeedbackValues()
{
    feedbackValues.labels.clear();
    feedbackValues.values.clear();

    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        feedbackValues.labels.push_back("OP" + juce::String(i + 1));
        feedbackValues.values.push_back(getFeedbackValue(i));
    }

    feedbackValues.selected = juce::jlimit(0, Opzx7PrValue::ops - 1, (int)feedbackTarget.getValue() - 1);
    feedbackValues.decimals = Global::floatDecimalPlaces;
    feedbackValues.repaint();
}

void GuiOpzx7::setup()
{
    auto setupPanBtn = [this](GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            mainGroup.contentCanvas.addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    const juce::String code = Opzx7PrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, Opzx7GuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    algFbCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = Opzx7GuiText::Category::algFb });

    algModeSelector.setup({ .parent = mainGroup.contentCanvas, .id = "", .title = "Mode", .items = algModeItems, .isReset = false });
    algModeSelector.setWantsKeyboardFocus(true);
    algModeSelector.setExplicitFocusOrder(++tabOrder);
    algModeSelector.onChange = [this] {
        int mode = algModeSelector.getSelectedItemIndex();

        ctx.editor.resized();

        ctx.audioProcessor.setOpzx7AlgMode(mode);
        };

    int initialMode = ctx.audioProcessor.getOpzx7AlgMode();
    algModeSelector.setSelectedItemIndex(initialMode, juce::dontSendNotification);

    mainGroup.contentCanvas.addAndMakeVisible(&algMatrixComp);
    mainGroup.contentCanvas.addAndMakeVisible(&algGraphComp);
    mainGroup.contentCanvas.addAndMakeVisible(&algStaticGraphComp);

    AlgMatrixState initialState = ctx.audioProcessor.getOpzx7AlgMatrix();
    algMatrixComp.setState(initialState);
    algGraphComp.updateState(initialState);

    algMatrixComp.onMatrixChanged = [this](const AlgMatrixState& state) {
        algGraphComp.updateState(state);

        ctx.audioProcessor.setOpzx7AlgMatrix(state);

        updateAlgorithmMatrixDisplay();
        };

    algSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::alg, .title = Opzx7GuiText::Fm::alg, .items = opzx7AlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    algFbSep.setupComponent(mainGroup.contentCanvas);

    // OP ごとにつまみを並べる代わりに、対象を選ぶつまみと値のつまみを 1 組ずつ置く。
    // 並びは 対象 → 値 → 各 OP の値。
    feedbackTarget.setup({ .parent = mainGroup.contentCanvas, .title = "FB.TARGET", .isReset = false });
    feedbackTarget.setRange(1.0, (double)Opzx7PrValue::ops, 1.0);
    feedbackTarget.setNumDecimalPlacesToDisplay(0);
    feedbackTarget.setWantsKeyboardFocus(true);
    feedbackTarget.setExplicitFocusOrder(++tabOrder);
    feedbackTarget.onValueChange = [this] { rebindFeedback(); };

    // 繋ぐ先は対象のつまみが決める。ここで空の名前へ繋ぎに行くと、無いパラメータを指して JUCE が止まる。
    feedbackSlider.setupComponent(mainGroup.contentCanvas, "", Opzx7GuiText::Fm::fb, tabOrder, std::nullopt, std::nullopt, false);
    feedbackSlider.getSlider().onValueChange = [this] { refreshFeedbackValues(); };

    feedbackNudge.setupComponent(mainGroup.contentCanvas, feedbackSlider.getSlider(), tabOrder);

    mainGroup.contentCanvas.addAndMakeVisible(feedbackValues);

    // onValueChange は値が変わらないと呼ばれないので、最初の束縛はここで明示的に行う。
    feedbackTarget.setValue(1, juce::dontSendNotification);

    rebindFeedback();

    panCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = Opzx7GuiText::Category::panpot, .enableChangeDetailVisible = true });

    panpotEnableToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::panpot_en, .title = Opzx7GuiText::Fm::panpotEnable, .isReset = true });
    panpotEnableToggle.setWantsKeyboardFocus(true);
    panpotEnableToggle.setExplicitFocusOrder(++tabOrder);

    panpotSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::panpot, .title = Opzx7GuiText::Fm::panpot, .isReset = true });
    panpotSlider.setWantsKeyboardFocus(true);
    panpotSlider.setExplicitFocusOrder(++tabOrder);

    panToLBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = Opzx7GuiText::Fm::Pan::l, .isReset = false });
    panToLBtn.setWantsKeyboardFocus(true);
    panToLBtn.setExplicitFocusOrder(++tabOrder);
    panToLBtn.onClick = [this]() {
        panpotEnableToggle.setToggleState(true, juce::sendNotification);
        panpotSlider.setValue(0, juce::sendNotification);
        };

    panToCBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = Opzx7GuiText::Fm::Pan::c, .isReset = false });
    panToCBtn.setWantsKeyboardFocus(true);
    panToCBtn.setExplicitFocusOrder(++tabOrder);
    panToCBtn.onClick = [this]() {
        panpotEnableToggle.setToggleState(false, juce::sendNotification);
        panpotSlider.setValue(15, juce::sendNotification);
        };

    panToRBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = Opzx7GuiText::Fm::Pan::r, .isReset = false });
    panToRBtn.setWantsKeyboardFocus(true);
    panToRBtn.setExplicitFocusOrder(++tabOrder);
    panToRBtn.onClick = [this]() {
        panpotEnableToggle.setToggleState(true, juce::sendNotification);
        panpotSlider.setValue(31, juce::sendNotification);
        };

    glLfo.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    modComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ampModComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgHwEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgHwPEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgSwEnv11g.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, "Bypass");
    ssgSwPEnv11g.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, "Bypass");

    setupGlobalGraph();

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    // 大区分。音量と音程にかかわる区分を、それぞれまとめて畳めるようにする。
    // 最初は閉じておく。
    ampMajorCat.setupMajorCategory({ .parent = mainGroup.contentCanvas, .title = CoreGuiText::MajorCategory::ampEnv, .enableChangeDetailVisible = true });
    pitchMajorCat.setupMajorCategory({ .parent = mainGroup.contentCanvas, .title = CoreGuiText::MajorCategory::pitchEnv, .enableChangeDetailVisible = true });

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = Opzx7GuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = Opzx7GuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    copyOpParamBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Copy Op Params", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamBtn.setWantsKeyboardFocus(true);
    copyOpParamBtn.setExplicitFocusOrder(++tabOrder);
    copyOpParamBtn.onClick = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        ctx.editor.copyOpzx7OpParams(from, to);

        // コピー先でロードしていたPCMファイル・波形メモリファイルをアンロードする
        ctx.audioProcessor.unloadOpzx7PcmFile(to);
        ctx.audioProcessor.unloadOpzx7WtFile(to);
        ctx.audioProcessor.unloadOpzx7Wt2File(to);
        };

    copyOpFromSlider.setup({ .parent = mainGroup.contentCanvas, .title = "FROM", .isReset = false });
    copyOpFromSlider.setRange(1.0, 6.0, 1.0);
    copyOpFromSlider.setNumDecimalPlacesToDisplay(0);
    copyOpFromSlider.setValue(1, juce::sendNotification);
    copyOpFromSlider.setWantsKeyboardFocus(true);
    copyOpFromSlider.setExplicitFocusOrder(++tabOrder);
    copyOpFromSlider.onValueChange = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        copyOpParamBtn.setEnabled(from != to);
        };

    copyOpToSlider.setup({ .parent = mainGroup.contentCanvas, .title = "TO", .isReset = false });
    copyOpToSlider.setRange(1.0, 6.0, 1.0);
    copyOpToSlider.setNumDecimalPlacesToDisplay(0);
    copyOpToSlider.setValue(2, juce::sendNotification);
    copyOpToSlider.setWantsKeyboardFocus(true);
    copyOpToSlider.setExplicitFocusOrder(++tabOrder);
    copyOpToSlider.onValueChange = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        copyOpParamBtn.setEnabled(from != to);
        };

    uSep002.setupComponent(mainGroup.contentCanvas);

    ieOpLfo.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OP LFO");
    ieOpLfo.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpLfoParam(opIndex); };
    ieOpLfo.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpLfoParam(opIndex); };

    ieOpDetune.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Detune");
    ieOpDetune.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importDetuneParam(opIndex); };
    ieOpDetune.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportDetuneParam(opIndex); };

    ieOpPitchEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Pitch Env");
    ieOpPitchEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importPitchEnvParam(opIndex); };
    ieOpPitchEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportPitchEnvParam(opIndex); };

    ieOpSsgSwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW Env");
    ieOpSsgSwEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwEnvParam(opIndex); };
    ieOpSsgSwEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwEnvParam(opIndex); };

    ieOpSsgSwEnv11.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW E11");
    ieOpSsgSwEnv11.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwEnv11Param(opIndex); };
    ieOpSsgSwEnv11.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwEnv11Param(opIndex); };

    ieOpSsgSwPEnv11.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW P11");
    ieOpSsgSwPEnv11.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwPEnv11Param(opIndex); };
    ieOpSsgSwPEnv11.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwPEnv11Param(opIndex); };
    ieOpSsgHwPEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG HW PEnv");
    ieOpSsgHwPEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpSsgHwPEnvParam(opIndex); };
    ieOpSsgHwPEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpSsgHwPEnvParam(opIndex); };
    ieOpWtAmpMod.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Amp Mod");
    ieOpWtAmpMod.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpWtAmpModParam(opIndex); };
    ieOpWtAmpMod.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpWtAmpModParam(opIndex); };
    ieOpSsgHwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG HW Env");
    ieOpSsgHwEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpSsgHwEnvParam(opIndex); };
    ieOpSsgHwEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpSsgHwEnvParam(opIndex); };
    ieOpWtMod.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Modulation");
    ieOpWtMod.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpWtModParam(opIndex); };
    ieOpWtMod.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpWtModParam(opIndex); };

    ieOpPcmPlay.setupComponentOp(mainGroup.contentCanvas, tabOrder, "PCM Play");
    ieOpPcmPlay.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpPcmPlayParam(opIndex); };
    ieOpPcmPlay.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpPcmPlayParam(opIndex); };

    ieOpChParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OP Params");
    ieOpChParam.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpChParam(opIndex); };
    ieOpChParam.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpChParam(opIndex); };

    uSep003.setupComponent(mainGroup.contentCanvas);

    // TARGET。設定ひとそろいの帯へ置く。値が動いたら束縛と枠線を付け替える。
    targerOpSlider.setup({ .parent = *this, .title = "TARGET", .isReset = false });
    targerOpSlider.setRange(1.0, (double)Opzx7PrValue::ops, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);
    targerOpSlider.onValueChange = [this] { applyOpTarget(); };

    ieAmpEnvG.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Env", ampEnvComponent);
    ieSsgHwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW Env", ssgHwEnv);
    ieSsgHwPEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW PEnv", ssgHwPEnv);
    ieWtMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Modulation", modComponent);
    ieWtAmpMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Mod", ampModComponent);

    ieSsgSwEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW E11", ssgSwEnv11g);
    ieSsgSwPEnv11g.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW P11", ssgSwPEnv11g);

    ieLfo.setupComponentFor(mainGroup.contentCanvas, tabOrder, "LFO", glLfo);

    ieUnison.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Unison", unisonComponent);

    ieQuality.setupComponent(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { importQualityParam(); };
    ieQuality.onClickExport = [this] { exportQualityParam(); };

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };


    // Operators
    const juce::String opCode = code + CPK::op;

    // ---- 上の絵と、下の設定 ----
    //
    // 絵はオペレータの数だけ。WS の波形とエンベロープを積む。
    // 設定はひとそろいだけ置き、TARGET で
    // 指し先を切り替える。
    for (int i = 0; i < Opzx7PrValue::ops; ++i)
    {
        cells[(size_t)i].setup(*this, i, Opzx7GuiText::Group::opPrefix + juce::String(i + 1), true);
        cells[(size_t)i].onSelect = [this](int index) {
            targerOpSlider.setValue(index + 1, juce::sendNotification);
            };
    }

    // 区分は縦に積まず、横へ並べる。1 列 1 区分が基本。
    stripViewport.setViewedComponent(&stripCanvas, false);
    stripViewport.setScrollBarsShown(false, true);
    stripViewport.setOpaque(false);

    addAndMakeVisible(stripViewport);

    colAmp.setup(stripCanvas, juce::String("") + "AMP ENV / OPTIONAL / SSG ENVELOPE");
    colWs.setup(stripCanvas, juce::String("") + "WAVE SHAPE");
    colSsgHwEnv.setup(stripCanvas, juce::String("") + "SSG HW AMP ENV");
    colSsgSwEnv.setup(stripCanvas, juce::String("") + "SSG SW AMP ENV");
    colSsgSwEnv11.setup(stripCanvas, juce::String("") + "SSG SW AMP ENV[11]");
    colAmpMod.setup(stripCanvas, juce::String("") + "WT AMP MOD");
    colPitchEnv.setup(stripCanvas, juce::String("") + "PITCH ENV");
    colSsgHwPEnv.setup(stripCanvas, juce::String("") + "SSG HW PITCH ENV");
    colSsgSwPEnv11.setup(stripCanvas, juce::String("") + "SSG SW PITCH ENV[11]");
    colMod.setup(stripCanvas, juce::String("") + "WT PITCH MOD");
    colKs.setup(stripCanvas, juce::String("") + "KEY SCALE / MUL/DET / FIX");
    colLfo.setup(stripCanvas, juce::String("") + "LFO");
    colMask.setup(stripCanvas, juce::String("") + "MASK / MML");

    // 今どのオペレータを触っているか。名前は rebind で入れ替える。
    opNameLabel.setup({ .parent = *this, .title = Io::empty,
        .justification = juce::Justification::centredLeft });

    opTargetSeparator.setupComponent(*this);

    // どのエンベロープを映すかの切り替え。TARGET のすぐ右へ置く。
    auto setupModeBtn = [this, &tabOrder](GuiToggleButton& btn, const juce::String& text,
        GuiEnvGraphMode mode) {
        btn.setup({ .parent = *this, .title = text, .isReset = false, .isResized = false });
        btn.setWantsKeyboardFocus(true);
        btn.setExplicitFocusOrder(++tabOrder);
        btn.onClick = [this, mode] { this->setGraphMode(mode); };
        };

    setupModeBtn(graphBtnAmp, "AMP", GuiEnvGraphMode::Amp);
    setupModeBtn(graphBtnPitch, "PIT", GuiEnvGraphMode::Pitch);
    setupModeBtn(graphBtnSsg, "SSG", GuiEnvGraphMode::SsgSw);
    setupModeBtn(graphBtnSsg11, "S11", GuiEnvGraphMode::SsgSw11);
    setupModeBtn(graphBtnSsgP11, "P11", GuiEnvGraphMode::SsgSwP11);

    graphBtnAmp.setToggleState(true, juce::dontSendNotification);

    // 組み立ての間だけ 1 番のオペレータへ繋いでおく。
    // 実際の指し先は、組み上がったあと rebind で入れ替える。
    const int i = 0;

    juce::String paramPrefix = opCode + juce::String(i);

    mulDetune.setupComponent(colKs.contentCanvas, paramPrefix, tabOrder, true);

    catAmp.setupHwCategory({ .parent = colAmp.contentCanvas, .title = Opzx7GuiText::Category::ampEnv, .detailVisible = true, .enableChangeDetailVisible = true });

    rgEn.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgEn, .title = Opzx7GuiText::Fm::Op::RgEn, .isReset = true });
    rgEn.setWantsKeyboardFocus(true);
    rgEn.setExplicitFocusOrder(++tabOrder);
    rgEn.onStateChange = [this] {
        const int op = currentOp();

        updateOpGraph(op); // RG_ENを切り替えるときにアンプエンベロープの内容を切り替える
        ctx.editor.resized();
        };

		rgSeparator.setupComponent(colAmp.contentCanvas);

    rgAr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgAr, .title = Opzx7GuiText::Fm::Op::Ar, .isReset = true });
    rgAr.setWantsKeyboardFocus(true);
    rgAr.setExplicitFocusOrder(++tabOrder);

    rgD1r.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgD1r, .title = Opzx7GuiText::Fm::Op::D1r, .isReset = true });
    rgD1r.setWantsKeyboardFocus(true);
    rgD1r.setExplicitFocusOrder(++tabOrder);

    rgD1l.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgD1l, .title = Opzx7GuiText::Fm::Op::D1l, .isReset = true });
    rgD1l.setWantsKeyboardFocus(true);
    rgD1l.setExplicitFocusOrder(++tabOrder);

    rgD2r.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgD2r, .title = Opzx7GuiText::Fm::Op::D2r, .isReset = true });
    rgD2r.setWantsKeyboardFocus(true);
    rgD2r.setExplicitFocusOrder(++tabOrder);

    rgRr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgRr, .title = Opzx7GuiText::Fm::Op::Rr, .isReset = true });
    rgRr.setWantsKeyboardFocus(true);
    rgRr.setExplicitFocusOrder(++tabOrder);

    rgTl.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgTl, .title = Opzx7GuiText::Fm::Op::Tl, .isReset = true });
    rgTl.setWantsKeyboardFocus(true);
    rgTl.setExplicitFocusOrder(++tabOrder);

    ar.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::ar, .title = Opzx7GuiText::Fm::Op::Ar, .isReset = true });
    ar.setWantsKeyboardFocus(true);
    ar.setExplicitFocusOrder(++tabOrder);

    d1r.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::d1r, .title = Opzx7GuiText::Fm::Op::D1r, .isReset = true });
    d1r.setWantsKeyboardFocus(true);
    d1r.setExplicitFocusOrder(++tabOrder);

    d1l.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::d1l, .title = Opzx7GuiText::Fm::Op::D1l, .isReset = true });
    d1l.setWantsKeyboardFocus(true);
    d1l.setExplicitFocusOrder(++tabOrder);

    d2r.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::d2r, .title = Opzx7GuiText::Fm::Op::D2r, .isReset = true });
    d2r.setWantsKeyboardFocus(true);
    d2r.setExplicitFocusOrder(++tabOrder);

    rr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rr, .title = Opzx7GuiText::Fm::Op::Rr, .isReset = true });
    rr.setWantsKeyboardFocus(true);
    rr.setExplicitFocusOrder(++tabOrder);

    tl.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::tl, .title = Opzx7GuiText::Fm::Op::Tl, .isReset = true });
    tl.setWantsKeyboardFocus(true);
    tl.setExplicitFocusOrder(++tabOrder);

    ksCat.setupHwCategory({ .parent = colKs.contentCanvas, .title = Opzx7GuiText::Category::ks, .enableChangeDetailVisible = true });

    ksEn.setup(GuiToggleButton::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksEn, .title = Opzx7GuiText::Fm::Op::KsEn, .isReset = true });
    ksEn.setWantsKeyboardFocus(true);
    ksEn.setExplicitFocusOrder(++tabOrder);

    ksMode.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksMode, .title = "Mode", .items = ksModeItems, .isReset = true });
    ksMode.setWantsKeyboardFocus(true);
    ksMode.setExplicitFocusOrder(++tabOrder);
    ksMode.onChange = [this]() {
        ctx.editor.resized();
        };

    ksrMA7.setup(GuiToggleButton::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksrMA7, .title = Opzx7GuiText::Fm::Op::Ksr, .isReset = true });
    ksrMA7.setWantsKeyboardFocus(true);
    ksrMA7.setExplicitFocusOrder(++tabOrder);

    kslMA7.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::kslMA7, .title = Opzx7GuiText::Fm::Op::Ksl, .items = kslMA7Items, .isReset = true });
    kslMA7.setWantsKeyboardFocus(true);
    kslMA7.setExplicitFocusOrder(++tabOrder);

    ksrOPZ.setup(GuiSlider::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksrOPZ, .title = Opzx7GuiText::Fm::Op::Ksr, .isReset = true });
    ksrOPZ.setWantsKeyboardFocus(true);
    ksrOPZ.setExplicitFocusOrder(++tabOrder);

    kslOPZ.setup(GuiSlider::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::kslOPZ, .title = Opzx7GuiText::Fm::Op::Ksl, .isReset = true });
    kslOPZ.setWantsKeyboardFocus(true);
    kslOPZ.setExplicitFocusOrder(++tabOrder);

    ksBp.setup(GuiSlider::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksBp, .title = "BREAK.PT", .isReset = true});
    ksBp.setWantsKeyboardFocus(true);
    ksBp.setExplicitFocusOrder(++tabOrder);

    ksLc.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksLc, .title = "L.CURVE", .items = ksCurveItems, .isReset = true});
    ksLc.setWantsKeyboardFocus(true);
    ksLc.setExplicitFocusOrder(++tabOrder);

    ksRc.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksRc, .title = "R.CURVE", .items = ksCurveItems, .isReset = true });
    ksRc.setWantsKeyboardFocus(true);
    ksRc.setExplicitFocusOrder(++tabOrder);

    ksLd.setup(GuiSlider::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksLd, .title = "L.DEPTH", .isReset = true });
    ksLd.setWantsKeyboardFocus(true);
    ksLd.setExplicitFocusOrder(++tabOrder);

    ksRd.setup(GuiSlider::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksRd, .title = "R.DEPTH", .isReset = true });
    ksRd.setWantsKeyboardFocus(true);
    ksRd.setExplicitFocusOrder(++tabOrder);

    ksRs.setup(GuiSlider::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksRs, .title = "RT.SCALE", .isReset = true});
    ksRs.setWantsKeyboardFocus(true);
    ksRs.setExplicitFocusOrder(++tabOrder);

    optionalSeparator.setupComponent(colAmp.contentCanvas);

    sus.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::sus, .title = Opzx7GuiText::Fm::Op::sus, .isReset = true });
    sus.setWantsKeyboardFocus(true);
    sus.setExplicitFocusOrder(++tabOrder);

    opDelay.setupComponent(colAmp.contentCanvas, paramPrefix + CPK::delay, "DELAY", tabOrder, std::nullopt);

    opDelayNudge.setupComponent(colAmp.contentCanvas, opDelay.getSlider(), tabOrder);

    opDelaySeparator.setupComponent(colAmp.contentCanvas);

    xof.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::xof, .title = Opzx7GuiText::Fm::Op::xof, .isReset = true });
    xof.setWantsKeyboardFocus(true);
    xof.setExplicitFocusOrder(++tabOrder);

    kor.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::kor, .title = Opzx7GuiText::Fm::Op::kor, .isReset = true });
    kor.setWantsKeyboardFocus(true);
    kor.setExplicitFocusOrder(++tabOrder);

    catWaveShape.setupHwCategory({ .parent = colWs.contentCanvas, .title = Opzx7GuiText::Category::waveShape, .enableChangeDetailVisible = true });

    ws.setup(GuiComboBox::Config{ .parent = colWs.contentCanvas, .id = paramPrefix + CPK::Fm::ws, .title = Opzx7GuiText::Fm::Op::Ws, .items = opzx7WsItems, .isReset = true });
    ws.setWantsKeyboardFocus(true);
    ws.setExplicitFocusOrder(++tabOrder);
    ws.onChange = [this] {
        const int op = currentOp();

        updateOnWsChange();

        ctx.editor.resized();
        };

    wsSeparator.setupComponent(colWs.contentCanvas);

    wsPreview.setup(colWs.contentCanvas, GuiColor::WavePreview::WaveMemory);

    loadPcmBtn.setup({ .parent = colWs.contentCanvas, .title = Opzx7GuiText::File::Pcm, .isReset = false, .isResized = true });
    loadPcmBtn.setWantsKeyboardFocus(true);
    loadPcmBtn.setExplicitFocusOrder(++tabOrder);
    loadPcmBtn.onClick = [this] {
        const int op = currentOp();

        // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
        ctx.editor.openAudioBrowser(
            [this, op](const juce::File& file) {
                if (file.existsAsFile()) {
                    updatePcmFileName(op, "Loading...");

                    // 発火するころには画面が消えているかもしれないので、弱い参照で見張る。
                    juce::Component::SafePointer<std::remove_pointer_t<decltype(this)>> safe(this);

                    juce::Timer::callAfterDelay(50, [this, safe, op, file]()
                        {
                            // 画面が閉じられていたら何もしない。callAfterDelay は取り消せず、
                            // メッセージが詰まっていれば 50ms よりずっと遅れて発火する。
                            if (safe == nullptr) return;

                            ctx.audioProcessor.loadOpzx7PcmFile(op, file);
                            updatePcmFileName(op, file.getFileName());
                            ctx.audioProcessor.lastSampleDirectory = file.getParentDirectory();
                        });
                }
            }
        );
        };

    clearPcmBtn.setup({ .parent = colWs.contentCanvas, .title = Opzx7GuiText::File::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = true });
    clearPcmBtn.setWantsKeyboardFocus(true);
    clearPcmBtn.setExplicitFocusOrder(++tabOrder);
    clearPcmBtn.onClick = [this] {
        const int op = currentOp();

        ctx.audioProcessor.unloadOpzx7PcmFile(op);
        updatePcmFileName(op, Io::empty);
        };

    pcmFileNameLabel.setup({ .parent = colWs.contentCanvas, .title = Io::empty });
    if (ctx.audioProcessor.opzx7PcmFilePaths[i].isNotEmpty()) {
        updatePcmFileName(i, juce::File(ctx.audioProcessor.opzx7PcmFilePaths[i]).getFileName());
    }

    speed.setup(GuiSlider::Config{ .parent = colWs.contentCanvas, .id = paramPrefix + CPK::speed, .title = "SPEED", .isReset = true });
    speed.setWantsKeyboardFocus(true);
    speed.setExplicitFocusOrder(++tabOrder);

    loopCount.setupComponent(colWs.contentCanvas, paramPrefix + CPK::lpCount, "CNT", tabOrder, std::nullopt);

    loopCountButtons.setupComponent(colWs.contentCanvas, loopCount.getSlider(), tabOrder);

    pcmOffset.setup(GuiSlider::Config{ .parent = colWs.contentCanvas, .id = paramPrefix + CPK::pcmOffset, .title = Opzx7GuiText::Fm::Op::PcmOffset, .isReset = true });
    pcmOffset.setWantsKeyboardFocus(true);
    pcmOffset.setExplicitFocusOrder(++tabOrder);

    pcmRatio.setup(GuiSlider::Config{ .parent = colWs.contentCanvas, .id = paramPrefix + CPK::pcmRatio, .title = Opzx7GuiText::Fm::Op::PcmRatio, .isReset = true });
    pcmRatio.setWantsKeyboardFocus(true);
    pcmRatio.setExplicitFocusOrder(++tabOrder);

    loopPointEnable.setup({ .parent = colWs.contentCanvas, .id = paramPrefix + CPK::lpEnable, .title = Opzx7GuiText::Fm::Op::loopPointEnable, .isReset = true });
    loopPointEnable.setWantsKeyboardFocus(true);
    loopPointEnable.setExplicitFocusOrder(++tabOrder);

    loopPointStart.setup(GuiSlider::Config{ .parent = colWs.contentCanvas, .id = paramPrefix + CPK::lpStart, .title = Opzx7GuiText::Fm::Op::loopPointStart, .isReset = true });
    loopPointStart.setWantsKeyboardFocus(true);
    loopPointStart.setExplicitFocusOrder(++tabOrder);

    loopPointEnd.setup(GuiSlider::Config{ .parent = colWs.contentCanvas, .id = paramPrefix + CPK::lpEnd, .title = Opzx7GuiText::Fm::Op::loopPointEnd, .isReset = true });
    loopPointEnd.setWantsKeyboardFocus(true);
    loopPointEnd.setExplicitFocusOrder(++tabOrder);

    // 切り出しとループの設定が変わったら、プレビューも合わせる
    auto refreshWsPreview = [this]() { this->updateWsPreview(); };

    pcmOffset.onValueChange = refreshWsPreview;
    pcmRatio.onValueChange = refreshWsPreview;
    loopPointStart.onValueChange = refreshWsPreview;
    loopPointEnd.onValueChange = refreshWsPreview;
    loopPointEnable.onStateChange = refreshWsPreview;

    loadWtBtn.setup({ .parent = colWs.contentCanvas, .title = "WT", .isReset = false, .isResized = true });
    loadWtBtn.setWantsKeyboardFocus(true);
    loadWtBtn.setExplicitFocusOrder(++tabOrder);
    loadWtBtn.onClick = [this] {
        const int op = currentOp();

        // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
        ctx.editor.openWaveBrowser({ EditorGuiText::ParamBrowser::waveWt },
            [this, op](const juce::File& file) {
                if (file.existsAsFile()) {
                    updateWtFileName(op, "Loading...");

                    // 発火するころには画面が消えているかもしれないので、弱い参照で見張る。
                    juce::Component::SafePointer<std::remove_pointer_t<decltype(this)>> safe(this);

                    juce::Timer::callAfterDelay(50, [this, safe, op, file]()
                        {
                            // 画面が閉じられていたら何もしない。callAfterDelay は取り消せず、
                            // メッセージが詰まっていれば 50ms よりずっと遅れて発火する。
                            if (safe == nullptr) return;

                            ctx.audioProcessor.loadOpzx7WtFile(op, file);
                            updateWtFileName(op, file.getFileName());
                            ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();
                        });
                }
            }
        );
        };

    clearWtBtn.setup({ .parent = colWs.contentCanvas, .title = Opzx7GuiText::File::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = true });
    clearWtBtn.setWantsKeyboardFocus(true);
    clearWtBtn.setExplicitFocusOrder(++tabOrder);
    clearWtBtn.onClick = [this] {
        const int op = currentOp();

        ctx.audioProcessor.unloadOpzx7WtFile(op);
        updateWtFileName(op, Io::empty);
        };

    wtFileNameLabel.setup({ .parent = colWs.contentCanvas, .title = Io::empty });
    if (ctx.audioProcessor.opzx7WtFilePaths[i].isNotEmpty()) {
        updateWtFileName(i, juce::File(ctx.audioProcessor.opzx7WtFilePaths[i]).getFileName());
    }

    loadWt2Btn.setup({ .parent = colWs.contentCanvas, .title = "WT2", .isReset = false, .isResized = true });
    loadWt2Btn.setWantsKeyboardFocus(true);
    loadWt2Btn.setExplicitFocusOrder(++tabOrder);
    loadWt2Btn.onClick = [this] {
        const int op = currentOp();

        // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
        ctx.editor.openWaveBrowser({ EditorGuiText::ParamBrowser::waveWt2 },
            [this, op](const juce::File& file) {
                if (file.existsAsFile()) {
                    updateWt2FileName(op, "Loading...");

                    // 発火するころには画面が消えているかもしれないので、弱い参照で見張る。
                    juce::Component::SafePointer<std::remove_pointer_t<decltype(this)>> safe(this);

                    juce::Timer::callAfterDelay(50, [this, safe, op, file]()
                        {
                            // 画面が閉じられていたら何もしない。callAfterDelay は取り消せず、
                            // メッセージが詰まっていれば 50ms よりずっと遅れて発火する。
                            if (safe == nullptr) return;

                            ctx.audioProcessor.loadOpzx7Wt2File(op, file);
                            updateWt2FileName(op, file.getFileName());
                            ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();
                        });
                }
            }
        );
        };

    clearWt2Btn.setup({ .parent = colWs.contentCanvas, .title = Opzx7GuiText::File::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = true });
    clearWt2Btn.setWantsKeyboardFocus(true);
    clearWt2Btn.setExplicitFocusOrder(++tabOrder);
    clearWt2Btn.onClick = [this] {
        const int op = currentOp();

        ctx.audioProcessor.unloadOpzx7Wt2File(op);
        updateWt2FileName(op, Io::empty);
        };

    wt2FileNameLabel.setup({ .parent = colWs.contentCanvas, .title = Io::empty });
    // 出すのは WT2 の名前なので、有無も WT2 側で見る
    if (ctx.audioProcessor.opzx7Wt2FilePaths[i].isNotEmpty()) {
        updateWt2FileName(i, juce::File(ctx.audioProcessor.opzx7Wt2FilePaths[i]).getFileName());
    }

    catOptional.setupSwCategory({ .parent = colAmp.contentCanvas, .title = Opzx7GuiText::Category::optional, .enableChangeDetailVisible = true });

    bypass.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::bypass, .title = Opzx7GuiText::Fm::Op::bypass, .isReset = true });
    bypass.setWantsKeyboardFocus(true);
    bypass.setExplicitFocusOrder(++tabOrder);

    catSsgEnv.setupSwCategory({ .parent = colAmp.contentCanvas, .title = Opzx7GuiText::Category::ssgEnv, .enableChangeDetailVisible = true });

    se.setup(GuiComboBox::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::se, .title = Opzx7GuiText::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
    se.setWantsKeyboardFocus(true);
    se.setExplicitFocusOrder(++tabOrder);

    seFreq.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::seFreq, .title = Opzx7GuiText::Fm::Op::SFreq, .isReset = true });
    seFreq.setWantsKeyboardFocus(true);
    seFreq.setExplicitFocusOrder(++tabOrder);

    sePreview.setup(colAmp.contentCanvas, GuiColor::WavePreview::AmpEnv);

    // 形と周期のどちらが変わっても描き直す
    se.onChange = [this] { updateSePreview(); };
    seFreq.onValueChange = [this] { updateSePreview(); };

    updateSePreview();

    pitchEnv.setupComponent(colPitchEnv.contentCanvas, paramPrefix, tabOrder, CPK::PitchAdsr::enable, Opzx7GuiText::PitchAdsr::enable, true);

    ssgSwEnv.setupComponent(colSsgSwEnv.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv::enable, Opzx7GuiText::SsgSwEnv::enable, true);

    ssgSwEnv11.setupComponent(colSsgSwEnv11.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv11::enable, Opzx7GuiText::SsgSwEnv11::enable, true);

    ssgSwPEnv11.setupComponent(colSsgSwPEnv11.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwPEnv11::enable, Opzx7GuiText::SsgSwPEnv11::enable, true);
    ssgHwPEnvOp.setupComponent(colSsgHwPEnv.contentCanvas, paramPrefix, tabOrder);
    wtAmpModOp.setupComponent(colAmpMod.contentCanvas, paramPrefix, tabOrder);
    ssgHwEnvOp.setupComponent(colSsgHwEnv.contentCanvas, paramPrefix, tabOrder);
    wtModOp.setupComponent(colMod.contentCanvas, paramPrefix, tabOrder);

    lfo.setupComponent(colLfo.contentCanvas, paramPrefix, tabOrder);

    fix.setupComponent(colKs.contentCanvas, paramPrefix, tabOrder, Opzx7GuiText::Fm::Op::Opzx7FreqTo440, 440);

    catMask.setupHwCategory({ .parent = colMask.contentCanvas, .title = Opzx7GuiText::Category::mask, .enableChangeDetailVisible = true });

    mask.setup(GuiToggleButton::Config{ .parent = colMask.contentCanvas, .id = paramPrefix + CPK::Fm::mask, .title = Opzx7GuiText::Fm::Op::Mask, .isReset = true });
    mask.setWantsKeyboardFocus(true);
    mask.setExplicitFocusOrder(++tabOrder);

    mmlSeparator.setupComponent(colMask.contentCanvas);

    mml.setup({ .parent = colMask.contentCanvas, .title = juce::String("") + "MML風入力", .isReset = false, .isResized = false });
    mml.setWantsKeyboardFocus(true);
    mml.setExplicitFocusOrder(++tabOrder);
    // 札は TARGET が指しているオペレータへ入れる。番号は rebind で
    // 入れ替わるので、ここでは押されたときに引き直す。
    mml.onClick = [this] {
        const int op = currentOp();

        GuiMml::openDialog(this, op, mmlHint,
            [this](juce::String text) { this->applyMmlString(text); });
        };

    setupOpGraphWiring();

    // 区分の中身は最初から開いておく。1 列 1 区分にしたので、
    // 畳んだままだと見出しだけの列が並ぶことになる。
    for (auto* group : {
        &colAmp, &colWs, &colSsgHwEnv, &colSsgSwEnv,
        &colSsgSwEnv11, &colAmpMod, &colPitchEnv, &colSsgHwPEnv,
        &colSsgSwPEnv11, &colMod, &colKs, &colLfo,
        &colMask,
        })
    {
        for (auto* child : group->contentCanvas.getChildren())
        {
            if (auto* cat = dynamic_cast<GuiCategoryLabel*>(child)) cat->setDetailVisible(true);
        }
    }

    // 前に開いていたときの指し先から始める。
    const int saved = (int)ctx.audioProcessor.apvts.state.getProperty(ProcessorStateKey::opzx7Target, 0);

    targerOpSlider.setValue(juce::jlimit(0, Opzx7PrValue::ops - 1, saved) + 1, juce::dontSendNotification);

    applyOpTarget();
}

void GuiOpzx7::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(Opzx7GuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(Opzx7GuiValue::Group::Padding::width, Opzx7GuiValue::Group::Padding::height);
    mmRect.removeFromTop(Opzx7GuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    // グラフはスクロールさせない固定ヘッダに置く。
    // 他のチャンネルやオペレータと同じく、プリセット名の下に来る。
    layoutGlobalGraph(mmRect);
    updateGlobalGraph();

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.getContentWidth(), 2000);

    levelComponent.layoutComponent(mRect);

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });

    layoutMain({ .mainRect = mRect, .label = &algModeSelector.label, .component = &algModeSelector });

    int currentAlgMode = algModeSelector.getSelectedItemIndex();

    if (currentAlgMode == 0) {
        // 従来のアルゴリズムモード
        algMatrixComp.setVisible(false);
        algGraphComp.setVisible(false);
        algSelector.setVisibleWithLabel(true);
        algStaticGraphComp.setVisible(true);

        layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
        mRect.removeFromTop(Opzx7GuiValue::Category::paddingTop);
        auto graphArea = mRect.removeFromTop(260); // 描画領域確保
        algStaticGraphComp.setBounds(graphArea.reduced(10));
    }
    else {
        // マトリックスモード
        algSelector.setVisibleWithLabel(false);
        algStaticGraphComp.setVisible(false);
        algMatrixComp.setVisible(true);
        algGraphComp.setVisible(true);

        mRect.removeFromTop(Opzx7GuiValue::Category::paddingTop);

        // マトリックスが必要とする高さぶんだけ取る。
        // 固定値で取ると下に余白が残り、グラフとの間が空きすぎる。
        auto matrixArea = mRect.removeFromTop(algMatrixComp.getNaturalHeight());

        // マトリックスは固定寸法なので、部品ごとグループの中央へ置く。
        // 中で座標をずらすと本体と枠がばらばらに動いてしまう。
        algMatrixComp.setBounds(matrixArea.withSizeKeepingCentre(
            algMatrixComp.getNaturalWidth(), matrixArea.getHeight()));

        auto graphArea = mRect.removeFromTop(260);
        algGraphComp.setBounds(graphArea.reduced(10));
    }

    algFbSep.layoutComponent(mRect);

    layoutMain({ .mainRect = mRect, .label = &feedbackTarget.label, .component = &feedbackTarget });
    feedbackSlider.layoutComponent(mRect);
    feedbackNudge.setVisibles(feedbackSlider.isVisibleNudge());
    if (feedbackSlider.isVisibleNudge()) feedbackNudge.layoutComponent(mRect);
    feedbackValues.setBounds(mRect.removeFromTop(feedbackValues.getNaturalHeight()));

    mRect.removeFromTop(CoreGuiValue::Category::gapBelow);

    // [[AMP ENV]] の大区分。閉じているあいだは、中の区分を見出しごと出さない。
    const bool ampOpen = layoutMajorCategory(ampMajorCat, mRect,
        ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)
        || ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));

    ampEnvComponent.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv));
    ampEnvComponent.layoutComponent(mRect);
    ssgHwEnv.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv));
    ssgHwEnv.layoutComponent(mRect);
    ssgSwEnv11g.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11));
    ssgSwEnv11g.layoutComponent(mRect);
    ampModComponent.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));
    ampModComponent.layoutComponent(mRect);

    ampMajorCat.endMajor(mRect);

    // [[PITCH ENV]] の大区分。閉じているあいだは、中の区分を見出しごと出さない。
    const bool pitchOpen = layoutMajorCategory(pitchMajorCat, mRect,
        ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)
        || ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));

    ssgHwPEnv.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv));
    ssgHwPEnv.layoutComponent(mRect);
    ssgSwPEnv11g.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11));
    ssgSwPEnv11g.layoutComponent(mRect);
    modComponent.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));
    modComponent.layoutComponent(mRect);

    pitchMajorCat.endMajor(mRect);

    glLfo.layoutComponent(mRect);

    layoutPanpotCat(mRect);

    unisonComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::Unison));
    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    // ---- 上は絵、下は設定 ----
    //
    // 絵は横 4 枚ずつ並べる。オペレータが 4 の倍数でないときは、
    // 足りないところを空けたままにする (詰めると番号と場所がずれる)。
    constexpr int cols = 4;
    constexpr int rows = (Opzx7PrValue::ops + cols - 1) / cols;

    const int cellW = pageArea.getWidth() / cols;
    const int cellH = GuiTargetCell::naturalHeight(true);

    auto cellsArea = pageArea.removeFromTop(cellH * rows);

    for (int r = 0; r < rows; ++r) {
        auto rowArea = cellsArea.removeFromTop(cellH);

        for (int col = 0; col < cols; ++col) {
            const int i = r * cols + col;

            auto cellArea = rowArea.removeFromLeft(cellW);

            if (i >= Opzx7PrValue::ops) continue;

            cells[(size_t)i].layout(cellArea);

            updateOpGraph(i);
            updateCellWs(i);
        }
    }

    // 残りはすべて設定。
    layoutOpPanel(pageArea);
    int mode = algModeSelector.getSelectedItemIndex();

    if (mode == 0) {
        updateAlgorithmDisplay();
    }
    else {
        updateAlgorithmMatrixDisplay();
    }
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpzx7::applyMmlString(const juce::String& mml)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);
    bool rgMode = rgEn.getToggleState();

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mulDetune.setMul(RegisterConverter::convertFmMulOpzx7(v)); } },
        { mmlPrefixMl,   [&](int v) { mulDetune.setMul(RegisterConverter::convertFmMulOpzx7(v)); } },
        { mmlPrefixDt,   [&](int v) { mulDetune.setDt1(RegisterConverter::convertFmDtOpzx7(v)); } },
        { mmlPrefixDt1,   [&](int v) { mulDetune.setDt1(RegisterConverter::convertFmDtOpzx7(v)); } },
        { mmlPrefixDto,   [&](int v) { mulDetune.setDt1(RegisterConverter::convertFmDtOpzx7(v)); } },
        { mmlPrefixDt2,  [&](int v) { mulDetune.setDt2(RegisterConverter::convertMmlDt2ToReg(v)); } },
        { mmlPrefixDtt,  [&](int v) { mulDetune.setDt2(RegisterConverter::convertMmlDt2ToReg(v)); } },
        { mmlPrefixMask, [&](int v) { mask.setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },

        // --- TL系 (RGモードで分岐) ---
        { mmlPrefixTl,   [&](int v) {
            rgMode ? rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification)
                   : tl.setValue(RegisterConverter::convertFmTl(v), juce::sendNotification);
        }},
        { mmlPrefixO,    [&](int v) {
            rgMode ? rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification)
                   : tl.setValue(RegisterConverter::convertFmTl(v), juce::sendNotification);
        }},
        { mmlPrefixOl,   [&](int v) {
            rgMode ? rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification)
                   : tl.setValue(RegisterConverter::convertFmTl(v), juce::sendNotification);
        }},

        // --- エンベロープ系 (RGモードで分岐) ---
        { mmlPrefixAr,   [&](int v) {
            rgMode ? rgAr.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification)
                   : ar.setValue(RegisterConverter::convertFmAr(v), juce::sendNotification);
        }},
        { mmlPrefixD1r,   [&](int v) {
            rgMode ? rgD1r.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification)
                   : d1r.setValue(RegisterConverter::convertFmDr(v), juce::sendNotification);
        }},
        { mmlPrefixD2r,   [&](int v) {
            rgMode ? rgD2r.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification)
                   : d2r.setValue(RegisterConverter::convertFmSr(v), juce::sendNotification);
        }},
        { mmlPrefixD1l,   [&](int v) {
            rgMode ? rgD1l.setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification)
                   : d1l.setValue(RegisterConverter::convertFmSl(v), juce::sendNotification);
        }},
        { mmlPrefixRr,   [&](int v) {
            rgMode ? rgRr.setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification)
                   : rr.setValue(RegisterConverter::convertFmRr(v), juce::sendNotification);
        }}
    };

    for (const auto& rUnit : units)
    {
        // actionMapの中に rUnit.key と一致するものがあるか検索
        auto it = actionMap.find(rUnit.key);

        if (it != actionMap.end())
        {
            // 見つかったら、そこに登録されたラムダ式(関数)に value を渡して実行
            it->second(rUnit.value);
        }
    }
}

// 鳴っていないオペレータは触れないようにする。
//
// つまみは TARGET のぶんしか無いので、指しているオペレータの話で
// なければ何もしない。指し先を変えたときに引き直す。
void GuiOpzx7::updateOpEnable(int idx, bool enable)
{
    if (idx != currentOp()) return;

    mulDetune.setEnables(enable);
    ar.setEnabledWithLabel(enable);
    d1r.setEnabledWithLabel(enable);
    d1l.setEnabledWithLabel(enable);
    rr.setEnabledWithLabel(enable);
    d2r.setEnabledWithLabel(enable);
    tl.setEnabledWithLabel(enable);
    ksEn.setEnabled(enable);
    ksrMA7.setEnabled(enable);
    kslMA7.setEnabledWithLabel(enable);
    ksrOPZ.setEnabled(enable);
    kslOPZ.setEnabledWithLabel(enable);
    ksBp.setEnabled(enable);
    ksBp.setEnabledWithLabel(enable);
    ksLc.setEnabled(enable);
    ksLc.setEnabledWithLabel(enable);
    ksRc.setEnabled(enable);
    ksRc.setEnabledWithLabel(enable);
    ksLd.setEnabled(enable);
    ksLd.setEnabledWithLabel(enable);
    ksRd.setEnabled(enable);
    ksRd.setEnabledWithLabel(enable);
    ksRs.setEnabled(enable);
    ksRs.setEnabledWithLabel(enable);
    se.setEnabledWithLabel(enable);
    seFreq.setEnabledWithLabel(enable);
    catOptional.setEnabled(enable);
    bypass.setEnabled(enable);
    fix.setEnables(enable);
    lfo.setEnabled(enable);
    catWaveShape.setEnabled(enable);
    ws.setEnabledWithLabel(enable);
    loadPcmBtn.setEnabled(enable);
    clearPcmBtn.setEnabled(enable);
    pcmFileNameLabel.setEnabled(enable);
    loopCount.setEnabledWithLabel(enable);
    loopCountButtons.setEnables(enable);
    speed.setEnabledWithLabel(enable);
    pcmOffset.setEnabledWithLabel(enable);
    pcmRatio.setEnabledWithLabel(enable);
    loadWtBtn.setEnabled(enable);
    clearWtBtn.setEnabled(enable);
    wtFileNameLabel.setEnabled(enable);
    loadWt2Btn.setEnabled(enable);
    clearWt2Btn.setEnabled(enable);
    wt2FileNameLabel.setEnabled(enable);
    catMask.setEnabled(enable);
    mask.setEnabled(enable);
    mmlSeparator.setEnabled(enable);
    mml.setEnabled(enable);
    rgEn.setEnabled(enable);
	rgSeparator.setEnabled(enable);
    rgAr.setEnabledWithLabel(enable);
    rgD1r.setEnabledWithLabel(enable);
    rgD1r.setEnabledWithLabel(enable);
    rgD1l.setEnabledWithLabel(enable);
    rgD1l.setEnabledWithLabel(enable);
    rgD2r.setEnabledWithLabel(enable);
    rgD2r.setEnabledWithLabel(enable);
    rgRr.setEnabledWithLabel(enable);
    rgRr.setEnabled(enable);
    rgTl.setEnabledWithLabel(enable);
    rgTl.setEnabled(enable);
	optionalSeparator.setEnabled(enable);
    sus.setEnabled(enable);
    opDelay.setEnabled(enable);
    opDelayNudge.setEnables(enable);
    xof.setEnabled(enable);
    kor.setEnabled(enable);
    pitchEnv.setEnabled(enable);
    ssgSwEnv.setEnabled(enable);
    ssgSwEnv11.setEnabled(enable);
    ssgSwPEnv11.setEnabled(enable);
    catSsgEnv.setEnabled(enable);
    se.setEnabled(enable);
    seFreq.setEnabled(enable);
}

// 設定の束にある WS の波形を描き直す。
//
// 描画のたびに計算すると重いので、形が変わったときだけここを通す。
// 上の枠のうち、TARGET が指している 1 枚も同じ形なので一緒に描く。
void GuiOpzx7::updateWsPreview()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す。
    if (GuiRefresh::defer(this, [this] { updateWsPreview(); })) return;

    const int opIndex = currentOp();

    fillWsPreview(wsPreview, opIndex);
    fillWsPreview(cells[(size_t)opIndex].preview(), opIndex);
}

// 上の枠 1 枚ぶんの WS の波形を描き直す。
//
// 素材の読み込みやプリセットの切り替えでは、TARGET が指していない
// オペレータの形も変わる。その 1 枚だけを引き直すときに使う。
void GuiOpzx7::updateCellWs(int opIndex)
{
    auto& cell = cells[(size_t)opIndex];

    // 枠ごとに溜める。まとめて 1 つの持ち主にすると、後から来たオペレータの
    // 求めで前のぶんが上書きされて、描かれない枠が残る。
    if (GuiRefresh::defer(&cell, [this, opIndex] { updateCellWs(opIndex); })) return;

    fillWsPreview(cell.preview(), opIndex);
}

// オペレータ 1 つぶんの WS の波形を、渡された表示へ描く。
//
// TARGET が指しているオペレータはつまみから読む。つまみを動かした
// 直後は、パラメータへ値が届く前にここへ来ることがあるため。
// ほかのオペレータはつまみを持たないので、パラメータから直に採る。
void GuiOpzx7::fillWsPreview(GuiWavePreview& preview, int opIndex)
{
    auto& apvts = ctx.audioProcessor.apvts;

    const juce::String code = Opzx7PrKey::prefix + CPK::op + juce::String(opIndex);

    const bool isTarget = opIndex == currentOp();

    // WS は 0 始まりの整数のパラメータなので、値がそのまま並びの番号になる。
    const int shape = isTarget
        ? ws.getSelectedItemIndex()
        : juce::roundToInt(GuiGraphValues::value(apvts, code + CPK::Fm::ws));

    const float offset = isTarget
        ? (float)pcmOffset.getValue()
        : GuiGraphValues::value(apvts, code + CPK::pcmOffset);

    const float ratio = isTarget
        ? (float)pcmRatio.getValue()
        : GuiGraphValues::value(apvts, code + CPK::pcmRatio);

    const bool loopOn = isTarget
        ? loopPointEnable.getToggleState()
        : GuiGraphValues::flag(apvts, code + CPK::lpEnable);

    // 波形そのものは処理側が持っている。
    // 波形メモリと PCM は未読込なら空。音源側と同じくサイン波になる。
    // PCM のときだけ P.OF / P.RT で切り出す範囲が変わるので、一緒に渡す。
    preview.setPoints(
        WavePreviewSource::opzx7Ws(
            shape,
            ctx.audioProcessor.opzx7WtBuffers[opIndex],
            ctx.audioProcessor.opzx7Wt2Buffers[opIndex],
            ctx.audioProcessor.opzx7PcmBuffers[opIndex],
            offset,
            ratio,
            ctx.audioProcessor.getSampleRate()),
        true);

    // 同じ枠が波形メモリとオーディオファイルの両方を映すので、
    // 何を出しているかに合わせて線の色も変える。
    const bool isPcm = shape == Opzx7PrValue::pcmIndex;

    preview.setLineColour(isPcm
        ? GuiColor::WavePreview::AudioFile
        : GuiColor::WavePreview::WaveMemory);

    // ループ位置は PCM のときだけ意味を持つ。切り出した範囲に対する 0.0〜1.0。
    std::vector<float> markers;

    if (isPcm && loopOn) {
        markers.push_back(isTarget
            ? (float)loopPointStart.getValue()
            : GuiGraphValues::value(apvts, code + CPK::lpStart));
        markers.push_back(isTarget
            ? (float)loopPointEnd.getValue()
            : GuiGraphValues::value(apvts, code + CPK::lpEnd));
    }

    preview.setMarkers(markers);
}

void GuiOpzx7::updateOnWsChange()
{
    updateWsPreview();

    int selectedWs = ws.getSelectedItemIndex();
    bool visible = catWaveShape.isDetailVisible();
    if (selectedWs == Opzx7PrValue::wtIndex)
    {
        // WT関連だけ表示
        wsSeparator.setVisible(visible);
        speed.setVisibleWithLabel(visible);
        loadPcmBtn.setVisible(false);
        clearPcmBtn.setVisible(false);
        pcmFileNameLabel.setVisible(false);
        loopCount.setVisibleWithLabel(false);
        loopCountButtons.setVisibles(false);
        pcmOffset.setVisibleWithLabel(false);
        pcmRatio.setVisibleWithLabel(false);
        loopPointEnable.setVisible(false);
        loopPointStart.setVisibleWithLabel(false);
        loopPointEnd.setVisibleWithLabel(false);
        loadWtBtn.setVisible(visible);
        clearWtBtn.setVisible(visible);
        wtFileNameLabel.setVisible(visible);
        loadWt2Btn.setVisible(false);
        clearWt2Btn.setVisible(false);
        wt2FileNameLabel.setVisible(false);
    }
    else if (selectedWs == Opzx7PrValue::wt2Index)
    {
        // WT2関連だけ表示
        wsSeparator.setVisible(visible);
        speed.setVisibleWithLabel(visible);
        loadPcmBtn.setVisible(false);
        clearPcmBtn.setVisible(false);
        pcmFileNameLabel.setVisible(false);
        loopCount.setVisibleWithLabel(false);
        loopCountButtons.setVisibles(false);
        pcmOffset.setVisibleWithLabel(false);
        pcmRatio.setVisibleWithLabel(false);
        loopPointEnable.setVisible(false);
        loopPointStart.setVisibleWithLabel(false);
        loopPointEnd.setVisibleWithLabel(false);
        loadWtBtn.setVisible(false);
        clearWtBtn.setVisible(false);
        wtFileNameLabel.setVisible(false);
        loadWt2Btn.setVisible(visible);
        clearWt2Btn.setVisible(visible);
        wt2FileNameLabel.setVisible(visible);
    }
    else if (selectedWs == Opzx7PrValue::pcmIndex)
    {
        // PCM関連だけ表示
        wsSeparator.setVisible(visible);
        speed.setVisibleWithLabel(visible);
        loadPcmBtn.setVisible(visible);
        clearPcmBtn.setVisible(visible);
        pcmFileNameLabel.setVisible(visible);
        loopCount.setVisibleWithLabel(visible);
        loopCountButtons.setVisibles(visible && loopCount.isVisibleNudge());
        pcmOffset.setVisibleWithLabel(visible);
        pcmRatio.setVisibleWithLabel(visible);
        loopPointEnable.setVisible(visible);
        loopPointStart.setVisibleWithLabel(visible);
        loopPointEnd.setVisibleWithLabel(visible);
        loadWtBtn.setVisible(false);
        clearWtBtn.setVisible(false);
        wtFileNameLabel.setVisible(false);
        loadWt2Btn.setVisible(false);
        clearWt2Btn.setVisible(false);
        wt2FileNameLabel.setVisible(false);
    }
    else {
        // 全関連非表示
        wsSeparator.setVisible(false);
        speed.setVisibleWithLabel(false);
        loadPcmBtn.setVisible(false);
        clearPcmBtn.setVisible(false);
        pcmFileNameLabel.setVisible(false);
        loopCount.setVisibleWithLabel(false);
        loopCountButtons.setVisibles(false);
        pcmOffset.setVisibleWithLabel(false);
        pcmRatio.setVisibleWithLabel(false);
        loopPointEnable.setVisible(false);
        loopPointStart.setVisibleWithLabel(false);
        loopPointEnd.setVisibleWithLabel(false);
        loadWtBtn.setVisible(false);
        clearWtBtn.setVisible(false);
        wtFileNameLabel.setVisible(false);
        loadWt2Btn.setVisible(false);
        clearWt2Btn.setVisible(false);
        wt2FileNameLabel.setVisible(false);
    }
}

void GuiOpzx7::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > Opzx7PrValue::Alg::max)
        return;

    // Coreのルーティング情報から FmAlgState を生成
    const auto& routing = Opzx7Core::routings[algIndex];
    FmAlgState s;
    s.numOps = Opzx7PrValue::ops;

    for (int i = 0; i < s.numOps; ++i) { // i = src
        s.isCarrier[i] = (routing.out[i] > 0.0f);
        for (int j = 0; j < s.numOps; ++j) { // j = dest
            s.mod[i][j] = (routing.mod[j][i] > 0.0f);
            s.fbMod[i][j] = (routing.fbMod[j][i] > 0.0f);
        }
    }

    // 生成したステートでグラフを描画
    algStaticGraphComp.updateState(s);

    // AlgRouting から出力に到達可能なオペレータを動的に判定
    auto activeOps = s.getActiveOperators();

    for (int i = 0; i < Opzx7PrValue::ops; ++i)
    {
        // 配列を使わず、到達可能性から判定したフラグをセット
        opActive[(size_t)i] = activeOps[i];

        updateOpEnable(i, activeOps[i]);
    }
}

void GuiOpzx7::updateAlgorithmMatrixDisplay()
{
    FmAlgState s = algMatrixComp.getState();
    auto activeOps = s.getActiveOperators();

    for (int i = 0; i < Opzx7PrValue::ops; ++i)
    {
        opActive[(size_t)i] = activeOps[i];

        updateOpEnable(i, activeOps[i]);
    }
}

void GuiOpzx7::updateRgDisplayAsOp(bool rgMode)
{
    rgAr.setVisibleWithLabel(rgMode);
    rgD1r.setVisibleWithLabel(rgMode);
    rgD1l.setVisibleWithLabel(rgMode);
    rgD2r.setVisibleWithLabel(rgMode);
    rgRr.setVisibleWithLabel(rgMode);
    rgTl.setVisibleWithLabel(rgMode);

    ar.setVisibleWithLabel(!rgMode);
    d1r.setVisibleWithLabel(!rgMode);
    d1l.setVisibleWithLabel(!rgMode);
    d2r.setVisibleWithLabel(!rgMode);
    rr.setVisibleWithLabel(!rgMode);
    tl.setVisibleWithLabel(!rgMode);
}

// TARGET が今どのオペレータを指しているか (0 から数える)。
int GuiOpzx7::currentOp() const
{
    return juce::jlimit(0, Opzx7PrValue::ops - 1, (int)targerOpSlider.getValue() - 1);
}

// TARGET が指すオペレータへ、設定の束縛を丸ごと移す。
//
// オペレータごとに部品を並べるのをやめたので、指し先はここで差し替える。
// 見た目は組み直さない。
void GuiOpzx7::rebind(int opIndex)
{
    const juce::String code = Opzx7PrKey::prefix + CPK::op + juce::String(opIndex);

    opNameLabel.setText(Opzx7GuiText::Group::opPrefix + juce::String(opIndex + 1), juce::dontSendNotification);

    mulDetune.rebind(code);

    rgEn.rebind(code + CPK::Fm::rgEn);

    rgAr.rebind(code + CPK::Fm::rgAr);
    rgD1r.rebind(code + CPK::Fm::rgD1r);
    rgD1l.rebind(code + CPK::Fm::rgD1l);
    rgD2r.rebind(code + CPK::Fm::rgD2r);
    rgRr.rebind(code + CPK::Fm::rgRr);
    rgTl.rebind(code + CPK::Fm::rgTl);

    ar.rebind(code + CPK::Fm::ar);
    d1r.rebind(code + CPK::Fm::d1r);
    d1l.rebind(code + CPK::Fm::d1l);
    d2r.rebind(code + CPK::Fm::d2r);
    rr.rebind(code + CPK::Fm::rr);
    tl.rebind(code + CPK::Fm::tl);

    sus.rebind(code + CPK::Fm::sus);
    xof.rebind(code + CPK::Fm::xof);
    kor.rebind(code + CPK::Fm::kor);

    ksEn.rebind(code + CPK::Fm::ksEn);
    ksMode.rebind(code + CPK::Fm::ksMode);
    ksrMA7.rebind(code + CPK::Fm::ksrMA7);
    kslMA7.rebind(code + CPK::Fm::kslMA7);
    ksrOPZ.rebind(code + CPK::Fm::ksrOPZ);
    kslOPZ.rebind(code + CPK::Fm::kslOPZ);
    ksBp.rebind(code + CPK::Fm::ksBp);
    ksLc.rebind(code + CPK::Fm::ksLc);
    ksRc.rebind(code + CPK::Fm::ksRc);
    ksLd.rebind(code + CPK::Fm::ksLd);
    ksRd.rebind(code + CPK::Fm::ksRd);
    ksRs.rebind(code + CPK::Fm::ksRs);

    opDelay.getSlider().rebind(code + CPK::delay);
    bypass.rebind(code + CPK::Fm::bypass);

    ws.rebind(code + CPK::Fm::ws);
    speed.rebind(code + CPK::speed);
    loopCount.getSlider().rebind(code + CPK::lpCount);
    pcmOffset.rebind(code + CPK::pcmOffset);
    pcmRatio.rebind(code + CPK::pcmRatio);
    loopPointEnable.rebind(code + CPK::lpEnable);
    loopPointStart.rebind(code + CPK::lpStart);
    loopPointEnd.rebind(code + CPK::lpEnd);

    se.rebind(code + CPK::Fm::se);
    seFreq.rebind(code + CPK::Fm::seFreq);

    mask.rebind(code + CPK::Fm::mask);

    pitchEnv.rebind(code);
    ssgSwEnv.rebind(code);
    ssgSwEnv11.rebind(code);
    ssgSwPEnv11.rebind(code);
    ssgHwPEnvOp.rebind(code);
    wtAmpModOp.rebind(code);
    ssgHwEnvOp.rebind(code);
    wtModOp.rebind(code);
    lfo.rebind(code);
    fix.rebind(code);

    // 読み込んである素材の名前は APVTS ではなく処理側が持っている。
    // 指し先が変わったら、名前もプレビューも引き直す。
    pcmFileNameLabel.setText(juce::File(ctx.audioProcessor.opzx7PcmFilePaths[opIndex]).getFileName(), juce::dontSendNotification);
    wtFileNameLabel.setText(juce::File(ctx.audioProcessor.opzx7WtFilePaths[opIndex]).getFileName(), juce::dontSendNotification);
    wt2FileNameLabel.setText(juce::File(ctx.audioProcessor.opzx7Wt2FilePaths[opIndex]).getFileName(), juce::dontSendNotification);

    updateSePreview();
    updateWsPreview();
}

// 指し先を切り替える。
//
// 設定の束縛を移し、枠線を付け替える。開き直したときに続きから
// 触れるよう、指し先は覚えておく。
void GuiOpzx7::applyOpTarget()
{
    const int op = currentOp();

    ctx.audioProcessor.apvts.state.setProperty(ProcessorStateKey::opzx7Target, op, nullptr);

    rebind(op);

    for (int i = 0; i < Opzx7PrValue::ops; ++i) cells[(size_t)i].setActive(i == op);

    // 鳴っていないオペレータは触れないようにしてある。指し先が変わったので
    // 入り切りを引き直す。
    if (algModeSelector.getSelectedItemIndex() == 0) updateAlgorithmDisplay();
    else updateAlgorithmMatrixDisplay();
}

// 指し先を一時的に動かして何かをする。
//
// 設定はひとそろいしか無いので、TARGET が指していないオペレータを
// 読み書きするには、いったんそこへ繋ぎ替えるしかない。
// 終わったら必ず元へ戻す。
void GuiOpzx7::withOp(int opIndex, const std::function<void()>& fn)
{
    const int keep = currentOp();

    if (opIndex != keep) rebind(opIndex);

    fn();

    if (opIndex != keep) rebind(keep);
}

// 絵をまとめて切り替える。
//
// どれを映すかは絵ごとではなく、全部そろえる。並べた絵を見比べる
// ためのものなので、一枚ずつ違うものを映しても比べようがない。
void GuiOpzx7::setGraphMode(GuiEnvGraphMode mode)
{
    currentGraphMode = mode;

    // 札は排他。押したものだけを入れる。
    graphBtnAmp.setToggleState(mode == GuiEnvGraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GuiEnvGraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GuiEnvGraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11.setToggleState(mode == GuiEnvGraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11.setToggleState(mode == GuiEnvGraphMode::SsgSwP11, juce::dontSendNotification);

    for (int i = 0; i < Opzx7PrValue::ops; ++i) updateOpGraph(i);
}

void GuiOpzx7::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

bool GuiOpzx7::keyPressed(const juce::KeyPress& key)
{
    // Ctrl を押しているときは TARGET を動かす。数字だけを押したときの
    // MML より先に見ること。あちらは修飾キーを見ていないので、
    // 後ろへ置くと Ctrl + 数字まで MML が食べてしまう。
    if (moveTargetByKey(targerOpSlider, key)) return true;

    int opIndex = -1;
    int code = key.getKeyCode();
    juce::ModifierKeys metaKeys = key.getModifiers();

    // 通常の 1〜8キー、または テンキーの 1〜8 を判定。
    // OPZX7 は 8 オペレータなので、7 と 8 まで受ける。
    if (code == '1' || code == juce::KeyPress::numberPad1) opIndex = 0;
    else if (code == '2' || code == juce::KeyPress::numberPad2) opIndex = 1;
    else if (code == '3' || code == juce::KeyPress::numberPad3) opIndex = 2;
    else if (code == '4' || code == juce::KeyPress::numberPad4) opIndex = 3;
    else if (code == '5' || code == juce::KeyPress::numberPad5) opIndex = 4;
    else if (code == '6' || code == juce::KeyPress::numberPad6) opIndex = 5;
    else if (code == '7' || code == juce::KeyPress::numberPad7) opIndex = 6;
    else if (code == '8' || code == juce::KeyPress::numberPad8) opIndex = 7;

    if (opIndex == -1) return false; // 他のキーなら無視（通常処理へ）

    // 数字はそのままオペレータ番号を指す。TARGET は動かさない。
    //
    // 設定はひとそろいしか無いので、ここで指し先を動かすと、MML を
    // 入れるつもりで押しただけで今見ているオペレータが入れ替わる。
    //
    // 鳴っていないオペレータには何もしない。以前はつまみが押せるかどうかで
    // 見ていたが、つまみは TARGET のぶんしか無くなったので、
    // アルゴリズムから出した表を見る。
    if (!opActive[(size_t)opIndex]) return true;

    const juce::String opPrefix = Opzx7PrKey::prefix + CPK::op + juce::String(opIndex);

    // Altキーを押しながら -> マスクのON/OFF、それ以外 -> MML の窓
    if (metaKeys.isAltDown()) {
        const juce::String id = opPrefix + CPK::Fm::mask;

        setParamValue(id, getParamValue(id) >= 0.5f ? 0.0f : 1.0f);
    }
    else {
        GuiMml::openDialog(this, opIndex, mmlHint,
            [this, opIndex](juce::String mml) {
                // 書き込む先は押された番号のオペレータ。TARGET は元へ戻す。
                withOp(opIndex, [this, mml] { this->applyMmlString(mml); });
            });
    }

    return true; // キー入力を消費したことをJUCEに伝える
}

void GuiOpzx7::copyFmParamsToString()
{
    // オペレータごとに部品を並べるのをやめたので、値はつまみからではなく
    // パラメータから直に採る。画面へ出ているのは TARGET の 1 本だけで、
    // MML には 4 本ぶんが要る。
    //
    // 選ぶ形のパラメータは、つまみの選択番号と同じ値を持っている。
    auto& apvts = ctx.audioProcessor.apvts;

    auto opVal = [&apvts](int index, const juce::String& key) {
        return (int)GuiGraphValues::value(apvts, Opzx7PrKey::prefix + CPK::op + juce::String(index) + key);
        };

    auto formatOpExt = [&opVal](int index) {
        // ' MUL AR DR SL RR TL KSR KSL
        return juce::String::formatted(
            u8"MUL%d DT1%+d DT2+%d AR%d D1R%d D1L%d D2R%d RR%d TL%d\n",
            opVal(index, CPK::mul),
            opVal(index, CPK::dt),
            opVal(index, CPK::dt2),
            opVal(index, CPK::Fm::rgAr),
            opVal(index, CPK::Fm::rgD1r),
            opVal(index, CPK::Fm::rgD1l),
            opVal(index, CPK::Fm::rgD2r),
            opVal(index, CPK::Fm::rgRr),
            opVal(index, CPK::Fm::rgTl)
        );
        };
    auto formatOpsExt = [&formatOpExt]() {
        return formatOpExt(0) + formatOpExt(1) + formatOpExt(2) + formatOpExt(3);
        };

    juce::String mml = juce::String("[OPZX7]\n")
        + FmMml::extMmlHeader
        + formatOpsExt();
    juce::SystemClipboard::copyTextToClipboard(mml);
}

void GuiOpzx7::copyFmParamsToObject()
{

}

void GuiOpzx7::pasteFmParamsFromObject()
{

}

void GuiOpzx7::initParams()
{
    this->ctx.audioProcessor.initParams("OPZX7_");

    for (int i = 0; i < Opzx7PrValue::ops; i++)
    {
        this->ctx.audioProcessor.unloadOpzx7PcmFile(i);
        updatePcmFileName(i, Io::empty);

        this->ctx.audioProcessor.unloadOpzx7WtFile(i);
        updateWtFileName(i, Io::empty);

        this->ctx.audioProcessor.unloadOpzx7Wt2File(i);
        updateWt2FileName(i, Io::empty);
    }
}

// 設定ひとそろいの置き方。
//
// 一番上が TARGET の帯、その下に区切り線、そこから下が区分。
// 区分は縦に積まず横へ並べ、あふれたぶんは横の送り棒で送る。
void GuiOpzx7::layoutOpPanel(juce::Rectangle<int> area)
{
    {
        auto row = area.removeFromTop(20);
        auto slot = row.removeFromLeft(Opzx7GuiValue::Fm::Op::Col::width);

        layoutMain({ .mainRect = slot, .label = &targerOpSlider.label, .component = &targerOpSlider });

        row.removeFromLeft(Opzx7GuiValue::Fm::Op::Col::gap);

        // TARGET のすぐ右が、いま指しているものの名前。
        opNameLabel.setBounds(row.removeFromLeft(Opzx7GuiValue::Fm::Op::Col::nameWidth).withHeight(18));

        // 名前と切り替えの間だけを空ける。
        row.removeFromLeft(Opzx7GuiValue::Fm::Op::Col::nameGap);

        // その右が、どのエンベロープを映すかの切り替え。
        {
            const int w = Opzx7GuiValue::ParamGroup::Graph::ModeButtonWidth;

            auto btnRow = row.removeFromLeft(w * 5).withHeight(Opzx7GuiValue::ParamGroup::Graph::ButtonHeight);

            graphBtnAmp.setBounds(btnRow.removeFromLeft(w));
            graphBtnPitch.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg11.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsgP11.setBounds(btnRow);
        }
    }

    opTargetSeparator.layoutComponent(area);

    stripViewport.setBounds(area);

    const int colW = Opzx7GuiValue::Fm::Op::Col::width;
    const int colGap = Opzx7GuiValue::Fm::Op::Col::gap;

    // 横の送り棒が出るぶんだけ、中身の丈を短くする。
    const int colH = juce::jmax(1, area.getHeight() - stripViewport.getScrollBarThickness());

    int x = 0;

    // 1 列を切り出して、中身を上から積む。
    // 簡易表示モードで隠す区分は、列ごと出さない。隠したぶんだけ
    // 右の列が左へ詰まる。
    auto layoutCol = [&](GuiScrollGroup& group, bool show, auto&& body)
        {
            if (!show) {
                group.setVisible(false);

                return;
            }

            group.setVisible(true);

            juce::Rectangle<int> colArea(x, 0, colW, colH);

            x += colW + colGap;

            group.setBounds(colArea);

            auto inner = colArea.reduced(Opzx7GuiValue::Group::Padding::width, Opzx7GuiValue::Group::Padding::height);

            inner.removeFromTop(Opzx7GuiValue::Group::TitlePaddingTop);

            group.setViewportCustomBounds(inner.translated(-colArea.getX(), -colArea.getY()));

            juce::Rectangle<int> rect(0, 0, group.getContentWidth(), 20000);

            body(rect);

            group.setContentHeight(rect.getY() + 20);
        };

    const auto shown = [this](SimpleView::Cat cat) { return ctx.audioProcessor.isSimpleShown(cat); };

    // レジスタの値で触るかどうかと、選んでいる波形。どちらも区分の中身を
    // 変えるので、先に引いておく。
    const bool rgMode = rgEn.getToggleState();
    const int selectedWs = ws.getSelectedItemIndex();

    layoutCol(colAmp, true, [&](juce::Rectangle<int>& rect) {
        layoutOpAmpCat(rect, rgMode);
        layoutOpOptionalCat(rect);
        layoutOpSsgEnvCat(rect);
        });

    layoutCol(colWs, true, [&](juce::Rectangle<int>& rect) { layoutOpWsCat(rect, selectedWs); });

    layoutCol(colSsgHwEnv, shown(SimpleView::SsgHwAmpEnv), [&](juce::Rectangle<int>& rect) {
        ssgHwEnvOp.setCategoryVisible(true);
        ssgHwEnvOp.layoutComponent(rect);
        });

    layoutCol(colSsgSwEnv, shown(SimpleView::SsgSwAmpEnv), [&](juce::Rectangle<int>& rect) {
        ssgSwEnv.setCategoryVisible(true);
        ssgSwEnv.layoutComponent(rect);
        });

    layoutCol(colSsgSwEnv11, shown(SimpleView::SsgSwAmpEnv11), [&](juce::Rectangle<int>& rect) {
        ssgSwEnv11.setCategoryVisible(true);
        ssgSwEnv11.layoutComponent(rect);
        });

    layoutCol(colAmpMod, shown(SimpleView::WtAmpMod), [&](juce::Rectangle<int>& rect) {
        wtAmpModOp.setCategoryVisible(true);
        wtAmpModOp.layoutComponent(rect);
        });

    layoutCol(colPitchEnv, shown(SimpleView::PitchEnv), [&](juce::Rectangle<int>& rect) {
        pitchEnv.setCategoryVisible(true);
        pitchEnv.layoutComponent(rect);
        });

    layoutCol(colSsgHwPEnv, shown(SimpleView::SsgHwPitchEnv), [&](juce::Rectangle<int>& rect) {
        ssgHwPEnvOp.setCategoryVisible(true);
        ssgHwPEnvOp.layoutComponent(rect);
        });

    layoutCol(colSsgSwPEnv11, shown(SimpleView::SsgSwPitchEnv11), [&](juce::Rectangle<int>& rect) {
        ssgSwPEnv11.setCategoryVisible(true);
        ssgSwPEnv11.layoutComponent(rect);
        });

    layoutCol(colMod, shown(SimpleView::WtPitchMod), [&](juce::Rectangle<int>& rect) {
        wtModOp.setCategoryVisible(true);
        wtModOp.layoutComponent(rect);
        });

    layoutCol(colKs, true, [&](juce::Rectangle<int>& rect) {
        layoutOpKsCat(rect, rgMode);

        mulDetune.setCategoryVisible(true);
        mulDetune.layoutComponent(rect);

        fix.setCategoryVisible(shown(SimpleView::Fix));
        fix.layoutComponent(rect);
        });

    layoutCol(colLfo, true, [&](juce::Rectangle<int>& rect) {
        lfo.setCategoryVisible(true);
        lfo.layoutComponent(rect);
        });

    // MASK と MML の札も 1 列へまとめてある。
    layoutCol(colMask, true, [&](juce::Rectangle<int>& rect) {
        layoutOpMaskCat(rect);

        mmlSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &mml, .paddingBottom = 0 });
        });

    stripCanvas.setSize(juce::jmax(1, x - colGap), colH);

    updateOnWsChange();
}

void GuiOpzx7::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    copyOpParamBtn.setVisible(visible);
    copyOpFromSlider.setVisibleWithLabel(visible);
    copyOpToSlider.setVisibleWithLabel(visible);
    uSep002.setVisible(visible);
    ieOpLfo.setVisible(visible);
    ieOpDetune.setVisible(visible);
    ieOpPitchEnv.setVisible(visible);
    ieOpSsgSwEnv.setVisible(visible);
    ieOpSsgSwEnv11.setVisible(visible);
    ieOpSsgSwPEnv11.setVisible(visible);
    ieOpSsgHwPEnv.setVisible(visible);
    ieOpWtAmpMod.setVisible(visible);
    ieOpSsgHwEnv.setVisible(visible);
    ieOpWtMod.setVisible(visible);
    ieOpPcmPlay.setVisible(visible);
    ieOpChParam.setVisible(visible);
    uSep003.setVisible(visible);
    ieAmpEnvG.setVisible(visible);
    ieSsgHwEnv.setVisible(visible);
    ieSsgHwPEnv.setVisible(visible);
    ieWtMod.setVisible(visible);
    ieWtAmpMod.setVisible(visible);
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11g.setVisible(visible);
    ieLfo.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);
    ieChParam.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyOpParamBtn });
        layoutMain({ .mainRect = rect, .label = &copyOpFromSlider.label, .component = &copyOpFromSlider });
        layoutMain({ .mainRect = rect, .label = &copyOpToSlider.label, .component = &copyOpToSlider });

        uSep002.layoutComponent(rect);

        ieOpLfo.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpDetune.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpPitchEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwPEnv11.layoutComponent(rect);
        ieOpSsgHwPEnv.layoutComponent(rect);
        ieOpWtAmpMod.layoutComponent(rect);
        ieOpSsgHwEnv.layoutComponent(rect);
        ieOpWtMod.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpPcmPlay.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpChParam.layoutComponent(rect);
        rect.removeFromTop(4);

        uSep003.layoutComponent(rect);

        ieAmpEnvG.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgHwEnv.layoutComponent(rect);
        ieSsgHwPEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieWtMod.layoutComponent(rect);
        ieWtAmpMod.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwPEnv11g.layoutComponent(rect);
        rect.removeFromTop(4);
        ieLfo.layoutComponent(rect);
        rect.removeFromTop(4);
        ieUnison.layoutComponent(rect);
        rect.removeFromTop(4);
        ieQuality.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpzx7::layoutOpMaskCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask });

    bool visibleMask = catMask.isDetailVisible();

    mask.setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }

    // MASK の後ろには分類の外の行 (セパレータや MML) が続くので、
    // 板をここで閉じないと 1 行ぶん下まで伸びてしまう。
    closeCategoryBackdrops(catMask.getParentComponent(), rect.getY());
}

void GuiOpzx7::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
}

void GuiOpzx7::layoutPanpotCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &panCat });

    bool visible = panCat.isDetailVisible();

    panpotEnableToggle.setVisible(visible);
    panpotSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &panpotEnableToggle });
        layoutMain({ .mainRect = rect, .label = &panpotSlider.label, .component = &panpotSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// オペレータの SSG HW ENV の形を描き直す。
//
// 選べる並びは先頭が「掛けない」で、そのあとに実機の 8 種類が続く。
// 実際の形の番号は 1 つずれるので、そのぶんを引いて渡す。
void GuiOpzx7::updateSePreview()
{
    int selected = se.getSelectedItemIndex();

    if (selected <= 0)
    {
        // 掛けないときは平らな線にする
        sePreview.setPoints(std::vector<float>(2, 1.0f), false);

        return;
    }

    sePreview.setPoints(
        WavePreviewSource::ssgHwEnv(selected - 1, 0.0f, 1.0f, false), false);
}

void GuiOpzx7::layoutOpSsgEnvCat(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catSsgEnv });

    bool visible = catSsgEnv.isDetailVisible();

    se.setVisibleWithLabel(visible);
    seFreq.setVisibleWithLabel(visible);
    sePreview.setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &se.label, .component = &se });
        layoutRow({ .rowRect = rect, .label = &seFreq.label, .component = &seFreq, });

        sePreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(3);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// ============================================================================
// チップ全体のエンベロープを見るグラフ
// ============================================================================
// オペレータごとのグラフと同じ作りで、対象がチップ全体の 3 つになる。
// 値が変わったときだけ描き直すので、常時の負荷は無い。
void GuiOpzx7::setupGlobalGraph()
{
    addAndMakeVisible(&gGraph);

    gGraphBtnAmp.setup({ .parent = *this, .title = "AMP", .isReset = false, .isResized = false });
    gGraphBtnAmp.setToggleState(true, juce::dontSendNotification);
    gGraphBtnAmp.onClick = [this] { setGlobalGraphMode(GlobalGraphMode::Amp); };

    gGraphBtnSsg11.setup({ .parent = *this, .title = "S11", .isReset = false, .isResized = false });
    gGraphBtnSsg11.onClick = [this] { setGlobalGraphMode(GlobalGraphMode::SsgSw11); };

    gGraphBtnSsgP11.setup({ .parent = *this, .title = "P11", .isReset = false, .isResized = false });
    gGraphBtnSsgP11.onClick = [this] { setGlobalGraphMode(GlobalGraphMode::SsgSwP11); };

    auto repaintGraph = [this]() {
        if (this->isUpdatingGraph) return;

        // 旗は必ず下ろす。途中で抜けたときに立ちっぱなしになると、
        // 以後グラフの更新が全部素通りしてしまうため。
        const juce::ScopedValueSetter<bool> guard(this->isUpdatingGraph, true);

        this->updateGlobalGraph();
        };

    ampEnvComponent.setupGraph(repaintGraph);
    ssgSwEnv11g.setupGraph(repaintGraph);
    ssgSwPEnv11g.setupGraph(repaintGraph);

    gGraphSeparator.setupComponent(*this);
}

void GuiOpzx7::layoutGlobalGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(Opzx7GuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    gGraphSeparator.layoutComponentBottom(mainArea);

    auto btnArea = mainArea.removeFromBottom(Opzx7GuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    gGraphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsg11.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsgP11.setBounds(btnArea);

    gGraph.setBounds(mainArea);
}

void GuiOpzx7::updateGlobalGraph()
{

    if (currentGlobalGraphMode == GlobalGraphMode::SsgSw11) {
        ssgSwEnv11g.updateGraph(gGraph);
    }
    else if (currentGlobalGraphMode == GlobalGraphMode::SsgSwP11) {
        ssgSwPEnv11g.updateGraph(gGraph);
    }
    else {
        ampEnvComponent.updateGraph(gGraph);
    }
}

void GuiOpzx7::setGlobalGraphMode(GlobalGraphMode mode)
{
    currentGlobalGraphMode = mode;

    // ラジオボタン的な排他制御
    gGraphBtnAmp.setToggleState(mode == GlobalGraphMode::Amp, juce::dontSendNotification);
    gGraphBtnSsg11.setToggleState(mode == GlobalGraphMode::SsgSw11, juce::dontSendNotification);
    gGraphBtnSsgP11.setToggleState(mode == GlobalGraphMode::SsgSwP11, juce::dontSendNotification);

    updateGlobalGraph();
}

// つまみが動いたら、上の絵を描き直してもらうためのつなぎ。
//
// グラフそのものはここには無い。オペレータの数だけ同時に出るので、
// 設定の側で持つわけにいかない。描き直すのは TARGET が指している
// 1 枚だけでよい。ほかのオペレータの値は変わっていない。
void GuiOpzx7::setupOpGraphWiring()
{
    auto repaintGraph = [this]() {
        if (this->isUpdatingGraph) return; // 既に更新中なら無視

        // 旗は必ず下ろす。途中で抜けたときに立ちっぱなしになると、
        // 以後グラフの更新が全部素通りしてしまうため。
        const juce::ScopedValueSetter<bool> guard(this->isUpdatingGraph, true);

        this->updateOpGraph(this->currentOp());
        };

    bypass.onStateChange = repaintGraph;
    xof.onStateChange = repaintGraph;
    kor.onStateChange = repaintGraph;
    sus.onStateChange = repaintGraph;

    // 生の値とレジスタの値、どちらを使っていてもグラフは描き直す。
    ar.onValueChange = repaintGraph;
    d1r.onValueChange = repaintGraph;
    d1l.onValueChange = repaintGraph;
    d2r.onValueChange = repaintGraph;
    rr.onValueChange = repaintGraph;
    tl.onValueChange = repaintGraph;

    rgAr.onValueChange = repaintGraph;
    rgD1r.onValueChange = repaintGraph;
    rgD1l.onValueChange = repaintGraph;
    rgD2r.onValueChange = repaintGraph;
    rgRr.onValueChange = repaintGraph;
    rgTl.onValueChange = repaintGraph;

    pitchEnv.setupGraph(repaintGraph);
    ssgSwEnv.setupGraph(repaintGraph);
    ssgSwEnv11.setupGraph(repaintGraph);
    ssgSwPEnv11.setupGraph(repaintGraph);
}

void GuiOpzx7::layoutOpKsCat(juce::Rectangle<int>& rect, bool rgMode) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat });

    bool visible = ksCat.isDetailVisible();
    Opzx7AdddrKeyScaleMode mode = (Opzx7AdddrKeyScaleMode)(ksMode.getSelectedItemIndex());

    ksEn.setVisible(visible && !rgMode);
    ksMode.setVisibleWithLabel(visible);
    ksrMA7.setVisible(visible && mode == Opzx7AdddrKeyScaleMode::MA7);
    kslMA7.setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::MA7);
    ksrOPZ.setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPZ);
    kslOPZ.setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPZ);
    ksBp.setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksBp.setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksLc.setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksRc.setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksLd.setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksRd.setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksRs.setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);

    if (visible) {
        if (!rgMode) {
            layoutRow({ .rowRect = rect, .component = &ksEn });
        }

        layoutRow({ .rowRect = rect, .label = &ksMode.label, .component = &ksMode });

        switch (mode) {
        case Opzx7AdddrKeyScaleMode::MA7:
            layoutRow({ .rowRect = rect, .component = &ksrMA7 });
            layoutRow({ .rowRect = rect, .label = &kslMA7.label, .component = &kslMA7 });
            break;
        case Opzx7AdddrKeyScaleMode::OPZ:
            layoutRow({ .rowRect = rect, .label = &ksrOPZ.label, .component = &ksrOPZ });
            layoutRow({ .rowRect = rect, .label = &kslOPZ.label, .component = &kslOPZ });
            break;
        case Opzx7AdddrKeyScaleMode::OPS:
            layoutRow({ .rowRect = rect, .label = &ksBp.label, .component = &ksBp });
            layoutRow({ .rowRect = rect, .label = &ksLc.label, .component = &ksLc });
            layoutRow({ .rowRect = rect, .label = &ksRc.label, .component = &ksRc });
            layoutRow({ .rowRect = rect, .label = &ksLd.label, .component = &ksLd });
            layoutRow({ .rowRect = rect, .label = &ksRd.label, .component = &ksRd });
            layoutRow({ .rowRect = rect, .label = &ksRs.label, .component = &ksRs });
            break;
        }

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpzx7::layoutOpAmpCat(juce::Rectangle<int>& rect, bool rgMode) {
    layoutRowCategory({ .rowRect = rect, .component = &catAmp });

    bool visible = catAmp.isDetailVisible();

    rgEn.setVisible(visible);
	rgSeparator.setVisible(visible);
    rgAr.setVisibleWithLabel(visible && rgMode);
    rgD1r.setVisibleWithLabel(visible && rgMode);
    rgD1l.setVisibleWithLabel(visible && rgMode);
    rgD2r.setVisibleWithLabel(visible && rgMode);
    rgRr.setVisibleWithLabel(visible && rgMode);
    rgTl.setVisibleWithLabel(visible && rgMode);
    ar.setVisibleWithLabel(visible && !rgMode);
    d1r.setVisibleWithLabel(visible && !rgMode);
    d1l.setVisibleWithLabel(visible && !rgMode);
    d2r.setVisibleWithLabel(visible && !rgMode);
    rr.setVisibleWithLabel(visible && !rgMode);
    tl.setVisibleWithLabel(visible && !rgMode);
	optionalSeparator.setVisible(visible);
    sus.setVisible(visible);
    xof.setVisible(visible);
    kor.setVisible(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .component = &rgEn });
		rgSeparator.layoutComponent(rect);
        updateRgDisplayAsOp(rgMode);
        if (rgMode)
        {
            layoutRow({ .rowRect = rect, .label = &rgAr.label, .component = &rgAr });
            layoutRow({ .rowRect = rect, .label = &rgD1r.label, .component = &rgD1r });
            layoutRow({ .rowRect = rect, .label = &rgD1l.label, .component = &rgD1l });
            layoutRow({ .rowRect = rect, .label = &rgD2r.label, .component = &rgD2r });
            layoutRow({ .rowRect = rect, .label = &rgRr.label, .component = &rgRr });
            layoutRow({ .rowRect = rect, .label = &rgTl.label, .component = &rgTl });
        }
        else
        {
            layoutRow({ .rowRect = rect, .label = &ar.label, .component = &ar });
            layoutRow({ .rowRect = rect, .label = &d1r.label, .component = &d1r });
            layoutRow({ .rowRect = rect, .label = &d1l.label, .component = &d1l });
            layoutRow({ .rowRect = rect, .label = &d2r.label, .component = &d2r });
            layoutRow({ .rowRect = rect, .label = &rr.label, .component = &rr });
            layoutRow({ .rowRect = rect, .label = &tl.label, .component = &tl });
        }

		optionalSeparator.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .component = &sus });
        layoutRow({ .rowRect = rect, .component = &xof });
        layoutRow({ .rowRect = rect, .component = &kor });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpzx7::layoutOpWsCat(juce::Rectangle<int>& rect, int selectedWs) {
    layoutRowCategory({ .rowRect = rect, .component = &catWaveShape });

    bool visible = catWaveShape.isDetailVisible();

    // 速さという概念があるのは、素材を回して鳴らす 3 つだけ
    const bool isSampleWs = selectedWs == Opzx7PrValue::pcmIndex
        || selectedWs == Opzx7PrValue::wtIndex
        || selectedWs == Opzx7PrValue::wt2Index;

    ws.setVisibleWithLabel(visible);
    wsPreview.setVisible(visible);
    speed.setVisibleWithLabel(visible && isSampleWs);
    wsSeparator.setVisible(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loadPcmBtn.setVisible(visible && selectedWs == Opzx7PrValue::pcmIndex);
    pcmFileNameLabel.setVisible(visible && selectedWs == Opzx7PrValue::pcmIndex);
    clearPcmBtn.setVisible(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loopCount.setVisibleWithLabel(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loopCountButtons.setVisibles((visible && selectedWs == Opzx7PrValue::pcmIndex) && loopCount.isVisibleNudge());
    pcmOffset.setVisibleWithLabel(visible && selectedWs == Opzx7PrValue::pcmIndex);
    pcmRatio.setVisibleWithLabel(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loopPointEnable.setVisible(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loopPointStart.setVisibleWithLabel(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loopPointEnd.setVisibleWithLabel(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loadWtBtn.setVisible(visible && selectedWs == Opzx7PrValue::wtIndex);
    wtFileNameLabel.setVisible(visible && selectedWs == Opzx7PrValue::wtIndex);
    clearWtBtn.setVisible(visible && selectedWs == Opzx7PrValue::wtIndex);
    loadWt2Btn.setVisible(visible && selectedWs == Opzx7PrValue::wt2Index);
    wt2FileNameLabel.setVisible(visible && selectedWs == Opzx7PrValue::wt2Index);
    clearWt2Btn.setVisible(visible && selectedWs == Opzx7PrValue::wt2Index);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &ws.label, .component = &ws });

        wsPreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        if (isSampleWs) {
            layoutRow({ .rowRect = rect, .label = &speed.label, .component = &speed });
        }

        if (selectedWs == Opzx7PrValue::pcmIndex)
        {
            wsSeparator.layoutComponent(rect);
            layoutRowOpzx7File({ .rect = rect, .loadPcmBtn = &loadPcmBtn, .pcmFileNameLabel = &pcmFileNameLabel, .clearPcmBtn = &clearPcmBtn });
            loopCount.layoutComponentRow(rect);
            if (loopCount.isVisibleNudge()) loopCountButtons.layoutComponentRow(rect);
            layoutRow({ .rowRect = rect, .label = &pcmOffset.label, .component = &pcmOffset });
            layoutRow({ .rowRect = rect, .label = &pcmRatio.label, .component = &pcmRatio });
            layoutRow({ .rowRect = rect, .component = &loopPointEnable });
            layoutRow({ .rowRect = rect, .label = &loopPointStart.label, .component = &loopPointStart, });
            layoutRow({ .rowRect = rect, .label = &loopPointEnd.label, .component = &loopPointEnd, });
        } 

        if (selectedWs == Opzx7PrValue::wtIndex)
        {
            wsSeparator.layoutComponent(rect);
            layoutRowOpzx7File({ .rect = rect, .loadPcmBtn = &loadWtBtn, .pcmFileNameLabel = &wtFileNameLabel, .clearPcmBtn = &clearWtBtn });
        }

        if (selectedWs == Opzx7PrValue::wt2Index)
        {
            wsSeparator.layoutComponent(rect);
            layoutRowOpzx7File({ .rect = rect, .loadPcmBtn = &loadWt2Btn, .pcmFileNameLabel = &wt2FileNameLabel, .clearPcmBtn = &clearWt2Btn });
        }

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// 枠に出すエンベロープを描き直す。
//
// 枠はオペレータの数だけ同時に出るので、値をつまみから読むわけには
// いかない。接頭辞を頼りにパラメータから直に採る。
void GuiOpzx7::updateOpGraph(int opIndex)
{
    auto& apvts = ctx.audioProcessor.apvts;
    auto& graph = cells[(size_t)opIndex].graph();

    const juce::String code = Opzx7PrKey::prefix + CPK::op + juce::String(opIndex);

    // -------------------------------------------------------------
    // Helper: 幅の計算 (Amp 用)
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return maxWidth;

        if (rateValue <= 0.0f) return maxWidth;

        float norm = 1.0f - (rateValue / maxRate);

        return maxWidth * norm;
        };

    // =============================================================
    // Pitch Env
    // =============================================================
    if (currentGraphMode == GuiEnvGraphMode::Pitch) {
        // オペレータの区分は「入れる」札なので、切ってあるときが素通し。
        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::PitchAdsr::enable));
        graph.setKeepLevels(GuiGraphValues::pitchEnvKeep(apvts, code));
        graph.updatePitchEnv(GuiGraphValues::pitchEnv(apvts, code));
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSw) {
        const auto v = GuiGraphValues::ssgSwEnv(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwEnv::enable));
        graph.updateSsgSwEnv(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    // =============================================================
    // SSG SW Env 11
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSw11) {
        const auto v = GuiGraphValues::ssgSwEnv11(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwEnv11::enable));
        graph.setKeepLevels(GuiGraphValues::ssgSwEnv11Keep(apvts, code));
        graph.updateSsgSwEnv11(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    // =============================================================
    // SSG SW PEnv 11
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSwP11) {
        const auto v = GuiGraphValues::ssgSwPEnv11(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwPEnv11::enable));
        graph.setKeepLevels(GuiGraphValues::ssgSwPEnv11Keep(apvts, code));
        graph.updateSsgSwPEnv11(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        bool isBypass = GuiGraphValues::flag(apvts, code + CPK::Fm::bypass);

        graph.updateBypass(isBypass);

        if (isBypass) {
            graph.repaint();

            return;
        }

        // OPZX7 は「生の値」と「レジスタの値」の二本立て。RG_EN で
        // どちらを使うかが変わるので、読む先の名前も変わる。
        bool isRg = GuiGraphValues::flag(apvts, code + CPK::Fm::rgEn);
        bool isSus = GuiGraphValues::flag(apvts, code + CPK::Fm::sus);
        bool isXof = GuiGraphValues::flag(apvts, code + CPK::Fm::xof);
        bool isKor = GuiGraphValues::flag(apvts, code + CPK::Fm::kor);

        auto key = [&code, isRg](const juce::String& rgKey, const juce::String& realKey) {
            return code + (isRg ? rgKey : realKey);
            };

        const juce::String arId = key(CPK::Fm::rgAr, CPK::Fm::ar);
        const juce::String d1rId = key(CPK::Fm::rgD1r, CPK::Fm::d1r);
        const juce::String d2rId = key(CPK::Fm::rgD2r, CPK::Fm::d2r);
        const juce::String d1lId = key(CPK::Fm::rgD1l, CPK::Fm::d1l);
        const juce::String rrId = key(CPK::Fm::rgRr, CPK::Fm::rr);
        const juce::String tlId = key(CPK::Fm::rgTl, CPK::Fm::tl);

        float arMax = GuiGraphValues::maxOf(apvts, arId);
        float d1rMax = GuiGraphValues::maxOf(apvts, d1rId);
        float d2rMax = GuiGraphValues::maxOf(apvts, d2rId);
        float d1lMax = GuiGraphValues::maxOf(apvts, d1lId);
        float rrMax = GuiGraphValues::maxOf(apvts, rrId);
        float tlMax = GuiGraphValues::maxOf(apvts, tlId);

        float arVal = GuiGraphValues::value(apvts, arId);
        float d1rVal = GuiGraphValues::value(apvts, d1rId);
        float d2rVal = GuiGraphValues::value(apvts, d2rId);
        float d1lVal = GuiGraphValues::value(apvts, d1lId);
        float rrVal = GuiGraphValues::value(apvts, rrId);
        float tlVal = GuiGraphValues::value(apvts, tlId);

        float sl = isRg ? (d1lMax - d1lVal) / d1lMax : d1lVal / d1lMax; // 15=0.0, 0=1.0
        float tlScale = isRg ? 1.0f - (tlVal / tlMax) : tlVal / tlMax; // TL=127で無音

        if (std::isnan(sl) || std::isinf(sl)) sl = 0.0f;
        if (std::isnan(tlScale) || std::isinf(tlScale)) tlScale = 1.0f;

        std::vector<GuiEnvelopeGraph::PhaseDef> phases;
        auto color = juce::Colours::cyan;

        float currentTotalWidth = 0.0f;

        // 1. Attack
        float attackWidth = rateToWidth(arVal, arMax);
        phases.push_back({
            .widthPx = attackWidth, .startLevel = 0.0f, .endLevel = 1.0f * tlScale, .color = color,
            .phaseLineColor = juce::Colours::red
            });
        currentTotalWidth += attackWidth;

        // 2. Decay 1
        float decayWidth = rateToWidth(d1rVal, d1rMax);
        phases.push_back({
            .widthPx = decayWidth, .startLevel = 1.0f * tlScale, .endLevel = sl * tlScale, .color = color,
            .phaseLineColor = juce::Colours::blue
            });
        currentTotalWidth += decayWidth;

        // 3. Sustain (D2R) の実線部分 (キーオン中)
        float releaseStartLevel = sl;
        float keyOnWidth = 60.0f;
        float sustainTotalWidth = 0.0f;

        if (d2rVal > 0.0f && d2rVal < d2rMax) {
            sustainTotalWidth = rateToWidth(d2rVal, d2rMax, 300.0f);

            phases.push_back({
                .widthPx = sustainTotalWidth, .startLevel = sl * tlScale, .endLevel = 0.0f, .color = color,
                .phaseLineColor = juce::Colours::green
                });

            currentTotalWidth += sustainTotalWidth * 0.5f;
        }
        else {
            phases.push_back({
                .widthPx = keyOnWidth,
                .startLevel = sl * tlScale,
                .endLevel = sl * tlScale,
                .color = color,
                .phaseLineColor = juce::Colours::green
                });

            currentTotalWidth += keyOnWidth;
        }

        float noteOffPositionX = currentTotalWidth;

        // 4. Release (通常時のみカーブを適用)
        if (isXof) {
            phases.push_back({
                .widthPx = 100.0f,
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = releaseStartLevel * tlScale,
                .isDashed = true,
                .color = juce::Colours::yellow,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX
                });
        }
        else if (isSus) {
            phases.push_back({
                .widthPx = rateToWidth(5.0f, rrMax, 200.0f),
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = 0.0f,
                .isDashed = true,
                .color = juce::Colours::yellow,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX
                });
        }
        else if (isKor) {
            phases.push_back({
                .widthPx = rateToWidth(rrVal, rrMax),
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = releaseStartLevel * tlScale,
                .color = juce::Colours::yellow,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX
                });
        }
        else {
            phases.push_back({
                .widthPx = rateToWidth(rrVal, rrMax),
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = 0.0f,
                .color = d2rVal > 0.0f ? juce::Colours::yellow : color,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX,
                .isMax = (rrVal == rrMax)
                });
        }

        graph.setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }

    graph.repaint();
}

void GuiOpzx7::layoutOpOptionalCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catOptional });

    bool visible = catOptional.isDetailVisible();

    opDelay.setVisibles(visible);
    opDelayNudge.setVisibles(visible && opDelay.isVisibleNudge());
    opDelaySeparator.setVisible(visible);
    bypass.setVisible(visible);

    if (visible)
    {
        opDelay.layoutComponentRow(rect);

        if (opDelay.isVisibleNudge()) opDelayNudge.layoutComponentRow(rect);

        opDelaySeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &bypass });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpzx7::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiOpzx7::copyParams(CopyOpzx7& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback1 = getFeedbackValue(0);
    copyObj.fmBase.feedback2 = getFeedbackValue(1);
    copyObj.fmBase.feedback3 = getFeedbackValue(2);
    copyObj.fmBase.feedback4 = getFeedbackValue(3);
    copyObj.fmBase.feedback5 = getFeedbackValue(4);
    copyObj.fmBase.feedback6 = getFeedbackValue(5);
    copyObj.fmBase.feedback7 = getFeedbackValue(6);
    copyObj.fmBase.feedback8 = getFeedbackValue(7);
    copyObj.panpot.enable = panpotEnableToggle.getToggleState();
    copyObj.panpot.panpot = panpotSlider.getValue();

    glLfo.copyParams(copyObj.lfo);
    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpzx7::copyOpParams(int p, CopyOpzx7Op& copyObj) {
    copyObj.detune.mul = mulDetune.getMul();
    copyObj.detune.mulRatio = mulDetune.getMulRatio();
    copyObj.detune.dt = mulDetune.getDt1();
    copyObj.detune.dt2 = mulDetune.getDt2();
    copyObj.detune.dt3 = mulDetune.getDt3();
    copyObj.aAdsr.rgEnable = rgEn.getToggleState();
    copyObj.aAdsr.rgAr = rgAr.getValue();
    copyObj.aAdsr.rgD1r = rgD1r.getValue();
    copyObj.aAdsr.rgD1l = rgD1l.getValue();
    copyObj.aAdsr.rgG2r = rgD2r.getValue();
    copyObj.aAdsr.rgRr = rgRr.getValue();
    copyObj.aAdsr.rgTl = rgTl.getValue();
    copyObj.aAdsr.ar = ar.getValue();
    copyObj.aAdsr.d1r = d1r.getValue();
    copyObj.aAdsr.d1l = d1l.getValue();
    copyObj.aAdsr.d2r = d2r.getValue();
    copyObj.aAdsr.rr = rr.getValue();
    copyObj.aAdsr.tl = tl.getValue();
    copyObj.aAdsr.bypass = bypass.getToggleState();
    copyObj.aAdsr.kor = kor.getToggleState();
    copyObj.aAdsr.sus = sus.getToggleState();
    copyObj.aAdsr.xof = xof.getToggleState();
    copyObj.aAdsr.mode = (Opzx7AdddrKeyScaleMode)ksMode.getSelectedId();
    copyObj.aAdsr.ksrMA7 = ksrMA7.getToggleState();
    copyObj.aAdsr.kslMA7 = kslMA7.getSelectedId();
    copyObj.aAdsr.ksrOPZ = ksrOPZ.getValue();
    copyObj.aAdsr.kslOPZ = kslOPZ.getValue();
    copyObj.aAdsr.ksBp = ksBp.getValue();
    copyObj.aAdsr.ksLc = ksLc.getSelectedId();
    copyObj.aAdsr.ksRc = ksRc.getSelectedId();
    copyObj.aAdsr.ksLd = ksLd.getValue();
    copyObj.aAdsr.ksRd = ksRd.getValue();
    copyObj.aAdsr.ksRs = ksRs.getValue();
    copyObj.waveSelect = ws.getSelectedId();
    copyObj.pcm.pcmOffset = pcmOffset.getValue();
    copyObj.pcm.speed = speed.getValue();
    copyObj.pcm.pcmRatio = pcmRatio.getValue();
    copyObj.ssgEg.ssgEg = se.getSelectedId();
    copyObj.ssgEg.fmSsgEgFreq = seFreq.getValue();
    copyObj.mask.mask = mask.getToggleState();

    fix.copyParams(copyObj.fix);
    lfo.copyParams(copyObj.lfo);
    pitchEnv.copyParams(copyObj.pAdsr);
    ssgSwEnv.copyParams(copyObj.aSsgSw);
}

void GuiOpzx7::pasteParams(CopyOpzx7& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    setFeedbackValue(0, copyObj.fmBase.feedback1);
    setFeedbackValue(1, copyObj.fmBase.feedback2);
    setFeedbackValue(2, copyObj.fmBase.feedback3);
    setFeedbackValue(3, copyObj.fmBase.feedback4);
    setFeedbackValue(4, copyObj.fmBase.feedback5);
    setFeedbackValue(5, copyObj.fmBase.feedback6);
    setFeedbackValue(6, copyObj.fmBase.feedback7);
    setFeedbackValue(7, copyObj.fmBase.feedback8);

    refreshFeedbackValues();
    panpotEnableToggle.setToggleState(copyObj.panpot.enable, juce::sendNotification);
    panpotSlider.setValue(copyObj.panpot.panpot, juce::sendNotification);
    glLfo.pasteParams(copyObj.lfo);
    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpzx7::pasteOpParams(int p, CopyOpzx7Op& copyObj) {
    mulDetune.setMul(copyObj.detune.mul);
    mulDetune.setMulRatio(copyObj.detune.mulRatio);
    mulDetune.setDt1(copyObj.detune.dt);
    mulDetune.setDt2(copyObj.detune.dt2);
    mulDetune.setDt3(copyObj.detune.dt3);
    rgEn.setToggleState(copyObj.aAdsr.rgEnable, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.rgAr, juce::sendNotification);
    rgD1r.setValue(copyObj.aAdsr.rgD1r, juce::sendNotification);
    rgD1l.setValue(copyObj.aAdsr.rgD1l, juce::sendNotification);
    rgD2r.setValue(copyObj.aAdsr.rgG2r, juce::sendNotification);
    rgRr.setValue(copyObj.aAdsr.rgRr, juce::sendNotification);
    rgTl.setValue(copyObj.aAdsr.rgTl, juce::sendNotification);
    ar.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    d1r.setValue(copyObj.aAdsr.d1r, juce::sendNotification);
    d1l.setValue(copyObj.aAdsr.d1l, juce::sendNotification);
    d2r.setValue(copyObj.aAdsr.d2r, juce::sendNotification);
    rr.setValue(copyObj.aAdsr.rr, juce::sendNotification);
    tl.setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass.setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor.setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    sus.setToggleState(copyObj.aAdsr.sus, juce::sendNotification);
    xof.setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ksMode.setSelectedId((int)copyObj.aAdsr.mode, juce::sendNotification);
    ksrMA7.setToggleState(copyObj.aAdsr.ksrMA7, juce::sendNotification);
    kslMA7.setSelectedId(copyObj.aAdsr.kslMA7, juce::sendNotification);
    ksrOPZ.setValue(copyObj.aAdsr.ksrOPZ, juce::sendNotification);
    kslOPZ.setValue(copyObj.aAdsr.kslOPZ, juce::sendNotification);
    ksBp.setValue(copyObj.aAdsr.ksBp, juce::sendNotification);
    ksLc.setSelectedId(copyObj.aAdsr.ksLc, juce::sendNotification);
    ksRc.setSelectedId(copyObj.aAdsr.ksRc, juce::sendNotification);
    ksLd.setValue(copyObj.aAdsr.ksLd, juce::sendNotification);
    ksRd.setValue(copyObj.aAdsr.ksRd, juce::sendNotification);
    ksRs.setValue(copyObj.aAdsr.ksRs, juce::sendNotification);
    ws.setSelectedId(copyObj.waveSelect, juce::sendNotification);
    pcmOffset.setValue(copyObj.pcm.pcmOffset, juce::sendNotification);
    speed.setValue(copyObj.pcm.speed, juce::sendNotification);
    pcmRatio.setValue(copyObj.pcm.pcmRatio, juce::sendNotification);
    se.setSelectedId(copyObj.ssgEg.ssgEg, juce::sendNotification);
    seFreq.setValue(copyObj.ssgEg.fmSsgEgFreq, juce::sendNotification);
    mask.setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix.pasteParams(copyObj.fix);
    lfo.pasteParams(copyObj.lfo);
    pitchEnv.pasteParams(copyObj.pAdsr);
    ssgSwEnv.pasteParams(copyObj.aSsgSw);
}

void GuiOpzx7::importOpLfoParam(int opIndex) {
    lfo.importParams();
}

void GuiOpzx7::exportOpLfoParam(int opIndex) {
    lfo.exportParams();
}

void GuiOpzx7::importDetuneParam(int opIndex) {
    mulDetune.importParams();
}

void GuiOpzx7::exportDetuneParam(int opIndex) {
    mulDetune.exportParams();
}

void GuiOpzx7::importPitchEnvParam(int opIndex) {
    pitchEnv.importParams();
}

void GuiOpzx7::exportPitchEnvParam(int opIndex) {
    pitchEnv.exportParams();
}

void GuiOpzx7::importSsgSwEnvParam(int opIndex) {
    ssgSwEnv.importParams();
}

void GuiOpzx7::exportSsgSwEnvParam(int opIndex) {
    ssgSwEnv.exportParams();
}

void GuiOpzx7::importQualityParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindQuality },
        [this](const juce::File& file) { applyQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpzx7::applyQualityParamFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingQualityParams(lines, index);
        }

        Io::ParamWriter writer(qualityFormat);

        writeQualityParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, qualityFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    qualityComponent.setBit(reader->getInt("bit", qualityComponent.getBit()));
    qualityComponent.setRate(reader->getInt("rate", qualityComponent.getRate()));
}

void GuiOpzx7::exportQualityParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindQuality }, Io::Extension::QualityParam,
        [this](const juce::File& file) { writeQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpzx7::writeQualityParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(qualityFormat);
    writeQualityParams(writer);

    writer.writeTo(file);
}

void GuiOpzx7::importOpPcmPlayParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultPcmPlayParamDir,
        { EditorGuiText::ParamBrowser::kindPcmPlay },
        [this, opIndex](const juce::File& file) { applyOpPcmPlayParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpzx7::applyOpPcmPlayParamFile(int opIndex, const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultPcmPlayParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingOpPcmPlayParams(opIndex, lines, index);
        }

        Io::ParamWriter writer(pcmPlayFormat);

        writeOpPcmPlayParams(opIndex, writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, pcmPlayFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    pcmOffset.setValue(reader->getFloat("pcmOffset", (float)pcmOffset.getValue()), juce::sendNotification);
    speed.setValue(reader->getFloat("speed", (float)speed.getValue()), juce::sendNotification);
    pcmRatio.setValue(reader->getFloat("pcmRatio", (float)pcmRatio.getValue()), juce::sendNotification);
    loopPointEnable.setToggleState(reader->getBool("loopPointEnable", loopPointEnable.getToggleState()), juce::sendNotification);
    loopPointStart.setValue(reader->getFloat("loopPointStart", (float)loopPointStart.getValue()), juce::sendNotification);
    loopPointEnd.setValue(reader->getFloat("loopPointEnd", (float)loopPointEnd.getValue()), juce::sendNotification);
    loopCount.setValue(reader->getFloat("loopCount", (float)loopCount.getValue()), juce::sendNotification);
}

void GuiOpzx7::exportOpPcmPlayParam(int opIndex)
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultPcmPlayParamDir,
        { EditorGuiText::ParamBrowser::kindPcmPlay }, Io::Extension::PcmPlayParam,
        [this, opIndex](const juce::File& file) { writeOpPcmPlayParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpzx7::writeOpPcmPlayParamFile(int opIndex, const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultPcmPlayParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(pcmPlayFormat);
    writeOpPcmPlayParams(opIndex, writer);

    writer.writeTo(file);
}

void GuiOpzx7::importSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11.importParams();
}

void GuiOpzx7::exportSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11.exportParams();
}

void GuiOpzx7::importSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11.importParams();
}

void GuiOpzx7::exportSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11.exportParams();
}

void GuiOpzx7::importOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp.importParams();
}

void GuiOpzx7::exportOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp.exportParams();
}

void GuiOpzx7::importOpWtAmpModParam(int opIndex) {
    wtAmpModOp.importParams();
}

void GuiOpzx7::exportOpWtAmpModParam(int opIndex) {
    wtAmpModOp.exportParams();
}

void GuiOpzx7::importOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp.importParams();
}

void GuiOpzx7::exportOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp.exportParams();
}

void GuiOpzx7::importOpWtModParam(int opIndex) {
    wtModOp.importParams();
}

void GuiOpzx7::exportOpWtModParam(int opIndex) {
    wtModOp.exportParams();
}

void GuiOpzx7::importChParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "OPZX7" },
        [this](const juce::File& file) { applyChParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiOpzx7::applyChParamFile(const juce::File& file) {
    if (!file.existsAsFile()) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingChParams(lines, index);
        }

        Io::ParamWriter writer(opzx7Format);

        writeChParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opzx7Format);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    readChParams(*reader);
}

void GuiOpzx7::exportChParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { "OPZX7" }, Io::Extension::opzx7sParam,
        [this](const juce::File& file) { writeChParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpzx7::writeChParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opzx7Format);
    writeChParams(writer);

    writer.writeTo(file);
}

void GuiOpzx7::importOpChParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpzx7Op },
        [this, opIndex](const juce::File& file) { applyOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpzx7::applyOpChParamFile(int opIndex, const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingOpChFileParams(opIndex, lines, index);
        }

        Io::ParamWriter writer(opzx7OpFormat);

        writeOpChFileParams(opIndex, writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opzx7OpFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    readOpParams(opIndex, *reader);
}

void GuiOpzx7::exportOpChParam(int opIndex)
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpzx7Op }, Io::Extension::opzx7sOpParam,
        [this, opIndex](const juce::File& file) { writeOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpzx7::writeOpChParamFile(int opIndex, const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opzx7OpFormat);
    writeOpChFileParams(opIndex, writer);

    writer.writeTo(file);
}


// チャンネル 1 つぶん。
void GuiOpzx7::readChParams(const Io::ParamReader& reader) {
    // Level
    levelComponent.readParams(reader, "level");

    // Algorithm & Feedback
    algModeSelector.setSelectedItemIndex(reader.getInt("algMode", algModeSelector.getSelectedItemIndex()), juce::sendNotification);
    algSelector.setSelectedId(reader.getInt("alg", algSelector.getSelectedId()), juce::sendNotification);

    algMatrixComp.readParams(reader, "algMatrixComp");
    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        const juce::String key = "feedback" + juce::String(i + 1);

        setFeedbackValue(i, reader.getFloat(key, getFeedbackValue(i)));
    }

    refreshFeedbackValues();

    int mode = algModeSelector.getSelectedItemIndex();
    if (mode == 0) {
        updateAlgorithmDisplay();
    }
    else {
        updateAlgorithmMatrixDisplay();
    }

    // Panpot
    panpotEnableToggle.setToggleState(reader.getBool("panpotEnable", panpotEnableToggle.getToggleState()), juce::sendNotification);
    panpotSlider.setValue(reader.getFloat("panpot", (float)panpotSlider.getValue()), juce::sendNotification);

    // Components (Global)
    ssgHwEnv.readParams(reader, "ssgHwEnv");
    ssgHwPEnv.readParams(reader, "ssgHwPEnv");
    ssgSwEnv11g.readParams(reader, "ssgSwEnv11");
    glLfo.readParams(reader, "glLfo");
    qualityComponent.readParams(reader, "quality");
    unisonComponent.readParams(reader, "unison");

    for (int i = 0; i < Opzx7PrValue::ops; i++) {
        withOp(i, [this, &reader, i] { readOpParams(i, reader.arrayItem(Io::ParamKey::ops, i)); });
    }

    ampEnvComponent.readParams(reader, "ampEnv");

    ssgSwPEnv11g.readParams(reader, "ssgSwPEnv11");

    modComponent.readParams(reader, "wtMod");
    ampModComponent.readParams(reader, "wtAmpMod");
}

// オペレータ 1 つぶん。並びの中のひとつを渡してもらう。
//
// 名前で引くので、他の音源のファイルを読ませても、こちらに無い
// 項目は勝手に読み飛ばされる。行数を数えて飛ばす細工が要らない。
void GuiOpzx7::readOpParams(int opIndex, const Io::ParamReader& r) {
    // Detune / Multiplier
    mulDetune.readParams(r, "mulDetune");

    // RG Env
    rgEn.setToggleState(r.getBool("rgEn", rgEn.getToggleState()), juce::sendNotification);
    rgAr.setValue(r.getFloat("rgAr", (float)rgAr.getValue()), juce::sendNotification);
    rgD1r.setValue(r.getFloat("rgD1r", (float)rgD1r.getValue()), juce::sendNotification);
    rgD1l.setValue(r.getFloat("rgD1l", (float)rgD1l.getValue()), juce::sendNotification);
    rgD2r.setValue(r.getFloat("rgD2r", (float)rgD2r.getValue()), juce::sendNotification);
    rgRr.setValue(r.getFloat("rgRr", (float)rgRr.getValue()), juce::sendNotification);
    rgTl.setValue(r.getFloat("rgTl", (float)rgTl.getValue()), juce::sendNotification);

    // Normal Env
    ar.setValue(r.getFloat("ar", (float)ar.getValue()), juce::sendNotification);
    d1r.setValue(r.getFloat("d1r", (float)d1r.getValue()), juce::sendNotification);
    d1l.setValue(r.getFloat("d1l", (float)d1l.getValue()), juce::sendNotification);
    d2r.setValue(r.getFloat("d2r", (float)d2r.getValue()), juce::sendNotification);
    rr.setValue(r.getFloat("rr", (float)rr.getValue()), juce::sendNotification);
    tl.setValue(r.getFloat("tl", (float)tl.getValue()), juce::sendNotification);

    // Key Scale
    ksEn.setToggleState(r.getBool("ksEn", ksEn.getToggleState()), juce::sendNotification);
    ksMode.setSelectedId(r.getInt("ksMode", ksMode.getSelectedId()), juce::sendNotification);
    ksrMA7.setToggleState(r.getBool("ksrMA7", ksrMA7.getToggleState()), juce::sendNotification);
    kslMA7.setSelectedId(r.getInt("kslMA7", kslMA7.getSelectedId()), juce::sendNotification);
    ksrOPZ.setValue(r.getFloat("ksrOPZ", (float)ksrOPZ.getValue()), juce::sendNotification);
    kslOPZ.setValue(r.getFloat("kslOPZ", (float)kslOPZ.getValue()), juce::sendNotification);
    ksBp.setValue(r.getFloat("ksBp", (float)ksBp.getValue()), juce::sendNotification);
    ksLc.setSelectedId(r.getInt("ksLc", ksLc.getSelectedId()), juce::sendNotification);
    ksRc.setSelectedId(r.getInt("ksRc", ksRc.getSelectedId()), juce::sendNotification);
    ksLd.setValue(r.getFloat("ksLd", (float)ksLd.getValue()), juce::sendNotification);
    ksRd.setValue(r.getFloat("ksRd", (float)ksRd.getValue()), juce::sendNotification);
    ksRs.setValue(r.getFloat("ksRs", (float)ksRs.getValue()), juce::sendNotification);

    // Optional
    bypass.setToggleState(r.getBool("bypass", bypass.getToggleState()), juce::sendNotification);
    sus.setToggleState(r.getBool("sus", sus.getToggleState()), juce::sendNotification);
    xof.setToggleState(r.getBool("xof", xof.getToggleState()), juce::sendNotification);
    kor.setToggleState(r.getBool("kor", kor.getToggleState()), juce::sendNotification);
    mask.setToggleState(r.getBool("mask", mask.getToggleState()), juce::sendNotification);

    // Wave Shape
    ws.setSelectedId(r.getInt("ws", ws.getSelectedId()), juce::sendNotification);

    ctx.audioProcessor.unloadOpzx7PcmFile(opIndex);
    ctx.audioProcessor.unloadOpzx7WtFile(opIndex);
    ctx.audioProcessor.unloadOpzx7Wt2File(opIndex);

    // 波形の場所は種類ごとに別の名前で持つ。以前は選んでいる種類のものしか
    // 書いていなかったので、切り替えて保存し直すと他の場所が失われていた。
    //
    // 場所はプロセッサが持っているものを使う。ラベルはファイル名だけを
    // 出しているので、そこから File を作ることはできない。
    auto pcmPath = Io::resolveSamplePath(r.getString("pcmFile", ctx.audioProcessor.opzx7PcmFilePaths[opIndex]), ctx.audioProcessor.defaultSampleDir);
    auto wtPath = Io::resolveSamplePath(r.getString("wtFile", ctx.audioProcessor.opzx7WtFilePaths[opIndex]), ctx.audioProcessor.defaultSampleDir);
    auto wt2Path = Io::resolveSamplePath(r.getString("wt2File", ctx.audioProcessor.opzx7Wt2FilePaths[opIndex]), ctx.audioProcessor.defaultSampleDir);

    auto showName = [](const juce::String& path) {
        return Io::isFilePath(path) ? juce::File(path).getFileName() : Io::empty;
        };

    // 名前は update...FileName を通す。ラベルだけを書き換えると、
    // WS のプレビューが前の波形を指したまま残るため。
    updatePcmFileName(opIndex, showName(pcmPath));
    updateWtFileName(opIndex, showName(wtPath));
    updateWt2FileName(opIndex, showName(wt2Path));

    // 実際に読み込むのは選んでいる種類のものだけ。他は場所を覚えておくに
    // とどめる。使わない波形まで読み込むと、そのぶん待たされるため。
    int wsIdx = ws.getSelectedItemIndex();

    if (wsIdx == Opzx7PrValue::pcmIndex && Io::isFilePath(pcmPath)) {
        ctx.audioProcessor.loadOpzx7PcmFile(opIndex, juce::File(pcmPath));
    }
    else if (wsIdx == Opzx7PrValue::wtIndex && Io::isFilePath(wtPath)) {
        ctx.audioProcessor.loadOpzx7WtFile(opIndex, juce::File(wtPath));
    }
    else if (wsIdx == Opzx7PrValue::wt2Index && Io::isFilePath(wt2Path)) {
        ctx.audioProcessor.loadOpzx7Wt2File(opIndex, juce::File(wt2Path));
    }

    // PCM Play / Loop Point
    pcmOffset.setValue(r.getFloat("pcmOffset", (float)pcmOffset.getValue()), juce::sendNotification);
    speed.setValue(r.getFloat("speed", (float)speed.getValue()), juce::sendNotification);
    pcmRatio.setValue(r.getFloat("pcmRatio", (float)pcmRatio.getValue()), juce::sendNotification);
    loopPointEnable.setToggleState(r.getBool("loopPointEnable", loopPointEnable.getToggleState()), juce::sendNotification);
    loopPointStart.setValue(r.getFloat("loopPointStart", (float)loopPointStart.getValue()), juce::sendNotification);
    loopPointEnd.setValue(r.getFloat("loopPointEnd", (float)loopPointEnd.getValue()), juce::sendNotification);
    loopCount.setValue(r.getFloat("loopCount", (float)loopCount.getValue()), juce::sendNotification);

    // SSG Env
    se.setSelectedId(r.getInt("se", se.getSelectedId()), juce::sendNotification);
    seFreq.setValue(r.getFloat("seFreq", (float)seFreq.getValue()), juce::sendNotification);

    // Components
    fix.readParams(r, "fix");
    lfo.readParams(r, "lfo");
    pitchEnv.readParams(r, "pitchEnv");
    ssgSwEnv.readParams(r, "ssgSwEnv");
    ssgSwEnv11.readParams(r, "ssgSwEnv11");
    ssgSwPEnv11.readParams(r, "ssgSwPEnv11");
    ssgHwPEnvOp.readParams(r, "ssgHwPEnv");
    wtAmpModOp.readParams(r, "wtAmpMod");
    ssgHwEnvOp.readParams(r, "ssgHwEnv");
    wtModOp.readParams(r, "wtMod");
}

void GuiOpzx7::writeOpParams(int opIndex, Io::ParamWriter& w) {
    // Detune / Multiplier
    mulDetune.writeParams(w, "mulDetune");

    // RG Env
    w.set("rgEn", rgEn.getToggleState());
    w.set("rgAr", (float)rgAr.getValue());
    w.set("rgD1r", (float)rgD1r.getValue());
    w.set("rgD1l", (float)rgD1l.getValue());
    w.set("rgD2r", (float)rgD2r.getValue());
    w.set("rgRr", (float)rgRr.getValue());
    w.set("rgTl", (float)rgTl.getValue());

    // Normal Env
    w.set("ar", (float)ar.getValue());
    w.set("d1r", (float)d1r.getValue());
    w.set("d1l", (float)d1l.getValue());
    w.set("d2r", (float)d2r.getValue());
    w.set("rr", (float)rr.getValue());
    w.set("tl", (float)tl.getValue());

    // Key Scale
    w.set("ksEn", ksEn.getToggleState());
    w.set("ksMode", ksMode.getSelectedId());
    w.set("ksrMA7", ksrMA7.getToggleState());
    w.set("kslMA7", kslMA7.getSelectedId());
    w.set("ksrOPZ", (float)ksrOPZ.getValue());
    w.set("kslOPZ", (float)kslOPZ.getValue());
    w.set("ksBp", (float)ksBp.getValue());
    w.set("ksLc", ksLc.getSelectedId());
    w.set("ksRc", ksRc.getSelectedId());
    w.set("ksLd", (float)ksLd.getValue());
    w.set("ksRd", (float)ksRd.getValue());
    w.set("ksRs", (float)ksRs.getValue());

    // Optional
    w.set("bypass", bypass.getToggleState());
    w.set("sus", sus.getToggleState());
    w.set("xof", xof.getToggleState());
    w.set("kor", kor.getToggleState());
    w.set("mask", mask.getToggleState());

    // Wave Shape
    w.set("ws", ws.getSelectedId());

    // 種類ごとに別の名前で持つので、切り替えて保存し直しても互いを壊さない
    w.set("pcmFile", ctx.audioProcessor.opzx7PcmFilePaths[opIndex]);
    w.set("wtFile", ctx.audioProcessor.opzx7WtFilePaths[opIndex]);
    w.set("wt2File", ctx.audioProcessor.opzx7Wt2FilePaths[opIndex]);

    // PCM Play / Loop Point
    w.set("pcmOffset", (float)pcmOffset.getValue());
    w.set("speed", (float)speed.getValue());
    w.set("pcmRatio", (float)pcmRatio.getValue());
    w.set("loopPointEnable", loopPointEnable.getToggleState());
    w.set("loopPointStart", (float)loopPointStart.getValue());
    w.set("loopPointEnd", (float)loopPointEnd.getValue());
    w.set("loopCount", (float)loopCount.getValue());

    // SSG Env
    w.set("se", se.getSelectedId());
    w.set("seFreq", (float)seFreq.getValue());

    // Components
    fix.writeParams(w, "fix");
    lfo.writeParams(w, "lfo");
    pitchEnv.writeParams(w, "pitchEnv");
    ssgSwEnv.writeParams(w, "ssgSwEnv");
    ssgSwEnv11.writeParams(w, "ssgSwEnv11");
    ssgSwPEnv11.writeParams(w, "ssgSwPEnv11");
    ssgHwPEnvOp.writeParams(w, "ssgHwPEnv");
    wtAmpModOp.writeParams(w, "wtAmpMod");
    ssgHwEnvOp.writeParams(w, "ssgHwEnv");
    wtModOp.writeParams(w, "wtMod");
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。並び順を写し直すより確実で、
// 当時の互換の工夫もそのまま残る。
void GuiOpzx7::setImportingChParams(juce::StringArray& lines, int& index) {
	// Level
	levelComponent.setImportingParams(lines, index);

	// Algorithm & Feedback
	algModeSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
	algSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

	algMatrixComp.setImportingParams(lines, index);
	for (int i = 0; i < Opzx7PrValue::ops; ++i) {
		setFeedbackValue(i, lines[index++].getFloatValue());
	}

	refreshFeedbackValues();

	int mode = algModeSelector.getSelectedItemIndex();
	if (mode == 0) {
	    updateAlgorithmDisplay();
	}
	else {
	    updateAlgorithmMatrixDisplay();
	}

	// Panpot
	panpotEnableToggle.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	panpotSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	// Components (Global)
	ssgHwEnv.setImportingParams(lines, index);
	ssgSwEnv11g.setImportingParams(lines, index);
	glLfo.setImportingParams(lines, index);
	qualityComponent.setImportingParams(lines, index);
	unisonComponent.setImportingParams(lines, index);

	for (int i = 0; i < Opzx7PrValue::ops; i++) {
	    getImportingOpParams(i, lines, index);
	}

	// AMP ENV は後から足したので、旧フォーマットとの互換のため
	// ファイル末尾から読む。行が無ければ既定のままにする。
	if (index < lines.size()) {
	    ampEnvComponent.setImportingParams(lines, index);
	}

	if (index < lines.size()) {
	    ssgSwPEnv11g.setImportingParams(lines, index);
	}

	if (index < lines.size()) {
	    modComponent.setImportingBaseParams(lines, index);
	    modComponent.setImportingShapeParam(lines, index);
	}

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpzx7::writeChParams(Io::ParamWriter& writer) {
	// Level
	levelComponent.writeParams(writer, "level");

	// Algorithm & Feedback
	writer.set("algMode", algModeSelector.getSelectedItemIndex());
	writer.set("alg", algSelector.getSelectedId());
	algMatrixComp.writeParams(writer, "algMatrixComp");
    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        writer.set("feedback" + juce::String(i + 1), getFeedbackValue(i));
    }

	// Panpot
	writer.set("panpotEnable", panpotEnableToggle.getToggleState());
	writer.set("panpot", (float)panpotSlider.getValue());

	// Components (Global)
	ssgHwEnv.writeParams(writer, "ssgHwEnv");
	ssgHwPEnv.writeParams(writer, "ssgHwPEnv");
	ssgSwEnv11g.writeParams(writer, "ssgSwEnv11");
	glLfo.writeParams(writer, "glLfo");
	qualityComponent.writeParams(writer, "quality");
	unisonComponent.writeParams(writer, "unison");

	for (int i = 0; i < Opzx7PrValue::ops; i++) {
	    auto op = writer.arrayItem(Io::ParamKey::ops, i);

	    writeOpParams(i, op);
	}

	// 名前で持つので、置き場所に意味は無い
	ampEnvComponent.writeParams(writer, "ampEnv");
	ssgSwPEnv11g.writeParams(writer, "ssgSwPEnv11");
	modComponent.writeParams(writer, "wtMod");
	ampModComponent.writeParams(writer, "wtAmpMod");

	
}

// 3.0.0 より前の形式を読むための補助。履歴から戻したもの。
void GuiOpzx7::getImportingOpParams(int opIndex, juce::StringArray& lines, int& index) {
    // Detune / Multiplier
    mulDetune.setImportingParams(lines, index);

    // RG Env
    rgEn.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    rgAr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD1r.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD1l.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD2r.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgRr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgTl.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Normal Env
    ar.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    d1r.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    d1l.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    d2r.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    tl.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Key Scale
    ksEn.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    ksMode.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ksrMA7.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    kslMA7.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ksrOPZ.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    kslOPZ.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    ksBp.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    ksLc.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ksRc.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ksLd.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    ksRd.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    ksRs.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Optional
    bypass.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    sus.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    xof.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    kor.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    mask.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

    // Wave Shape
    ws.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    ctx.audioProcessor.unloadOpzx7PcmFile(opIndex);
    ctx.audioProcessor.unloadOpzx7WtFile(opIndex);
    ctx.audioProcessor.unloadOpzx7Wt2File(opIndex);

    int wsIdx = ws.getSelectedItemIndex();

    if (wsIdx == Opzx7PrValue::pcmIndex) {
        pcmFileNameLabel.setText(lines[index++], juce::dontSendNotification);

        if (pcmFileNameLabel.getText().isNotEmpty()) {
            ctx.audioProcessor.loadOpzx7PcmFile(opIndex, pcmFileNameLabel.getText());
        }
    }
    else if (wsIdx == Opzx7PrValue::wtIndex) {
        wtFileNameLabel.setText(lines[index++], juce::dontSendNotification);

        if (wtFileNameLabel.getText().isNotEmpty()) {
            ctx.audioProcessor.loadOpzx7WtFile(opIndex, wtFileNameLabel.getText());
        }

    }
    else if (wsIdx == Opzx7PrValue::wt2Index) {
        wt2FileNameLabel.setText(lines[index++], juce::dontSendNotification);

        if (wt2FileNameLabel.getText().isNotEmpty()) {
            ctx.audioProcessor.loadOpzx7Wt2File(opIndex, wt2FileNameLabel.getText());
        }
    }

    // PCM Play / Loop Point
    pcmOffset.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    pcmRatio.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    loopPointEnable.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    loopPointStart.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    loopPointEnd.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // SSG Env
    se.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    seFreq.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Components
    fix.setImportingParams(lines, index);
    lfo.setImportingParams(lines, index);
    pitchEnv.setImportingParams(lines, index);
    ssgSwEnv.setImportingParams(lines, index);
    ssgSwEnv11.setImportingParams(lines, index);
    ssgSwPEnv11.setImportingParams(lines, index);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpzx7::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 2) return;

	qualityComponent.setBit(lines[0].getIntValue());
	qualityComponent.setRate(lines[1].getIntValue());

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpzx7::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("bit", qualityComponent.getBit());
	writer.set("rate", qualityComponent.getRate());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpzx7::setImportingOpPcmPlayParams(int opIndex, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 5) return;

	pcmOffset.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	pcmRatio.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	loopPointEnable.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	loopPointStart.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	loopPointEnd.setValue(lines[index++].getFloatValue(), juce::sendNotification);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpzx7::writeOpPcmPlayParams(int opIndex, Io::ParamWriter& writer) {
	writer.set("pcmOffset", (float)pcmOffset.getValue());
writer.set("speed", (float)speed.getValue());
	writer.set("pcmRatio", (float)pcmRatio.getValue());
	writer.set("loopPointEnable", loopPointEnable.getToggleState());
	writer.set("loopPointStart", (float)loopPointStart.getValue());
	writer.set("loopPointEnd", (float)loopPointEnd.getValue());
	writer.set("loopCount", (float)loopCount.getValue());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpzx7::setImportingOpChFileParams(int opIndex, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	getImportingOpParams(opIndex, lines, index);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpzx7::writeOpChFileParams(int opIndex, Io::ParamWriter& writer) {
	writeOpParams(opIndex, writer);

	
}

void GuiOpzx7::bypassHiddenCategories()
{
    // いま隠れている区分だけを切る。出したままの区分は触らない。
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv)) ampEnvComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)) ssgHwEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)) ssgSwEnv11g.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod)) ampModComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)) ssgHwPEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)) ssgSwPEnv11g.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod)) modComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::Unison)) unisonComponent.setCategoryBypassed(true);

    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)) ssgHwEnvOp.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv)) ssgSwEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)) ssgSwEnv11.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod)) wtAmpModOp.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv)) pitchEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)) ssgHwPEnvOp.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)) ssgSwPEnv11.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod)) wtModOp.setCategoryBypassed(true);
}

void GuiOpzx7::openEnabledCategories()
{
    // 効いている区分を開く。札を持たない区分は触らない。
    if (ampEnvComponent.hasBypassSwitch() && !ampEnvComponent.isCategoryBypassed()) ampEnvComponent.setCategoryOpen(true);
    if (ssgHwEnv.hasBypassSwitch() && !ssgHwEnv.isCategoryBypassed()) ssgHwEnv.setCategoryOpen(true);
    if (ssgSwEnv11g.hasBypassSwitch() && !ssgSwEnv11g.isCategoryBypassed()) ssgSwEnv11g.setCategoryOpen(true);
    if (ampModComponent.hasBypassSwitch() && !ampModComponent.isCategoryBypassed()) ampModComponent.setCategoryOpen(true);
    if (ssgHwPEnv.hasBypassSwitch() && !ssgHwPEnv.isCategoryBypassed()) ssgHwPEnv.setCategoryOpen(true);
    if (ssgSwPEnv11g.hasBypassSwitch() && !ssgSwPEnv11g.isCategoryBypassed()) ssgSwPEnv11g.setCategoryOpen(true);
    if (modComponent.hasBypassSwitch() && !modComponent.isCategoryBypassed()) modComponent.setCategoryOpen(true);
    if (unisonComponent.hasBypassSwitch() && !unisonComponent.isCategoryBypassed()) unisonComponent.setCategoryOpen(true);

    if (ssgHwEnvOp.hasBypassSwitch() && !ssgHwEnvOp.isCategoryBypassed()) ssgHwEnvOp.setCategoryOpen(true);
    if (ssgSwEnv.hasBypassSwitch() && !ssgSwEnv.isCategoryBypassed()) ssgSwEnv.setCategoryOpen(true);
    if (ssgSwEnv11.hasBypassSwitch() && !ssgSwEnv11.isCategoryBypassed()) ssgSwEnv11.setCategoryOpen(true);
    if (wtAmpModOp.hasBypassSwitch() && !wtAmpModOp.isCategoryBypassed()) wtAmpModOp.setCategoryOpen(true);
    if (pitchEnv.hasBypassSwitch() && !pitchEnv.isCategoryBypassed()) pitchEnv.setCategoryOpen(true);
    if (ssgHwPEnvOp.hasBypassSwitch() && !ssgHwPEnvOp.isCategoryBypassed()) ssgHwPEnvOp.setCategoryOpen(true);
    if (ssgSwPEnv11.hasBypassSwitch() && !ssgSwPEnv11.isCategoryBypassed()) ssgSwPEnv11.setCategoryOpen(true);
    if (wtModOp.hasBypassSwitch() && !wtModOp.isCategoryBypassed()) wtModOp.setCategoryOpen(true);

    // 大区分は中の区分に合わせる。効いている区分があれば開く。
    if (anyCategoryEnabled(ampEnvComponent, ssgHwEnv, ssgSwEnv11g, ampModComponent)) ampMajorCat.setDetailVisible(true);
    if (anyCategoryEnabled(ssgHwPEnv, ssgSwPEnv11g, modComponent)) pitchMajorCat.setDetailVisible(true);
}

void GuiOpzx7::closeBypassedCategories()
{
    // 切ってある区分を閉じる。札を持たない区分は触らない。
    if (ampEnvComponent.hasBypassSwitch() && ampEnvComponent.isCategoryBypassed()) ampEnvComponent.setCategoryOpen(false);
    if (ssgHwEnv.hasBypassSwitch() && ssgHwEnv.isCategoryBypassed()) ssgHwEnv.setCategoryOpen(false);
    if (ssgSwEnv11g.hasBypassSwitch() && ssgSwEnv11g.isCategoryBypassed()) ssgSwEnv11g.setCategoryOpen(false);
    if (ampModComponent.hasBypassSwitch() && ampModComponent.isCategoryBypassed()) ampModComponent.setCategoryOpen(false);
    if (ssgHwPEnv.hasBypassSwitch() && ssgHwPEnv.isCategoryBypassed()) ssgHwPEnv.setCategoryOpen(false);
    if (ssgSwPEnv11g.hasBypassSwitch() && ssgSwPEnv11g.isCategoryBypassed()) ssgSwPEnv11g.setCategoryOpen(false);
    if (modComponent.hasBypassSwitch() && modComponent.isCategoryBypassed()) modComponent.setCategoryOpen(false);
    if (unisonComponent.hasBypassSwitch() && unisonComponent.isCategoryBypassed()) unisonComponent.setCategoryOpen(false);

    if (ssgHwEnvOp.hasBypassSwitch() && ssgHwEnvOp.isCategoryBypassed()) ssgHwEnvOp.setCategoryOpen(false);
    if (ssgSwEnv.hasBypassSwitch() && ssgSwEnv.isCategoryBypassed()) ssgSwEnv.setCategoryOpen(false);
    if (ssgSwEnv11.hasBypassSwitch() && ssgSwEnv11.isCategoryBypassed()) ssgSwEnv11.setCategoryOpen(false);
    if (wtAmpModOp.hasBypassSwitch() && wtAmpModOp.isCategoryBypassed()) wtAmpModOp.setCategoryOpen(false);
    if (pitchEnv.hasBypassSwitch() && pitchEnv.isCategoryBypassed()) pitchEnv.setCategoryOpen(false);
    if (ssgHwPEnvOp.hasBypassSwitch() && ssgHwPEnvOp.isCategoryBypassed()) ssgHwPEnvOp.setCategoryOpen(false);
    if (ssgSwPEnv11.hasBypassSwitch() && ssgSwPEnv11.isCategoryBypassed()) ssgSwPEnv11.setCategoryOpen(false);
    if (wtModOp.hasBypassSwitch() && wtModOp.isCategoryBypassed()) wtModOp.setCategoryOpen(false);

    // 大区分は中の区分に合わせる。どれも切ってあれば閉じる。
    if (allCategoriesBypassed(ampEnvComponent, ssgHwEnv, ssgSwEnv11g, ampModComponent)) ampMajorCat.setDetailVisible(false);
    if (allCategoriesBypassed(ssgHwPEnv, ssgSwPEnv11g, modComponent)) pitchMajorCat.setDetailVisible(false);
}
