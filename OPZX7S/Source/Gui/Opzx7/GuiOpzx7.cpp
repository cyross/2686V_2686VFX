#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
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
    viewModeComp(context),
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
    opGroups{ GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context) },
    mulDetune{ GuiComponentMulDetune(context), GuiComponentMulDetune(context), GuiComponentMulDetune(context), GuiComponentMulDetune(context), GuiComponentMulDetune(context), GuiComponentMulDetune(context), GuiComponentMulDetune(context), GuiComponentMulDetune(context) },
    catAmp{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
    tl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    ar{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    d1r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    d1l{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    d2r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    rr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    ksCat{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
    ksEn{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    ksMode{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
    ksrMA7{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    kslMA7{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
    ksrOPZ{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    kslOPZ{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    ksBp{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    ksLc{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
    ksRc{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
    ksLd{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    ksRd{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    ksRs{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    catOptional{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
    bypass{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    fix{ GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context) },
    catWaveShape{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
    ws{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
    wsSeparator{ NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) },
    wsPreview{ GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context) },
    loadPcmBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
    clearPcmBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
    pcmFileNameLabel{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
    pcmOffset{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    pcmRatio{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    loopPointEnable{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    loopPointStart{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    loopPointEnd{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    loadWtBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
    clearWtBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
    wtFileNameLabel{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
    loadWt2Btn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
    clearWt2Btn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
    wt2FileNameLabel{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
    catSsgEnv{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
    se{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
    seFreq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) , GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    sePreview{ GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context) },
    lfo{ GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context) },
    pitchEnv{ GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context) },
    ssgSwEnv{ GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context) },
	ssgSwEnv11{ GuiComponentSsgSwEnv11(context),GuiComponentSsgSwEnv11(context),GuiComponentSsgSwEnv11(context),GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context) },
    ssgSwPEnv11{ GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context) },
        ssgHwPEnvOp{ GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context) },
        wtAmpModOp{ GuiComponentWtAmpMod(context), GuiComponentWtAmpMod(context), GuiComponentWtAmpMod(context), GuiComponentWtAmpMod(context), GuiComponentWtAmpMod(context), GuiComponentWtAmpMod(context), GuiComponentWtAmpMod(context), GuiComponentWtAmpMod(context) },
        ssgHwEnvOp{ GuiComponentSsgHwEnv(context), GuiComponentSsgHwEnv(context), GuiComponentSsgHwEnv(context), GuiComponentSsgHwEnv(context), GuiComponentSsgHwEnv(context), GuiComponentSsgHwEnv(context), GuiComponentSsgHwEnv(context), GuiComponentSsgHwEnv(context) },
        wtModOp{ GuiComponentWtMod(context), GuiComponentWtMod(context), GuiComponentWtMod(context), GuiComponentWtMod(context), GuiComponentWtMod(context), GuiComponentWtMod(context), GuiComponentWtMod(context), GuiComponentWtMod(context) },
    catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
    mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    mmlSeparator{ NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) },
    mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) },
    rgEn{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    rgSeparator{ NormalSeparator(context),NormalSeparator(context),NormalSeparator(context),NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) },
    rgAr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    rgD1r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    rgD2r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    rgD1l{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    rgRr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    rgTl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
    optionalSeparator{ NormalSeparator(context),NormalSeparator(context),NormalSeparator(context),NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) },
    sus{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    xof{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    kor{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    midiComponent(context),
    gGraph(),
    gGraphBtnAmp(context),
    gGraphBtnSsg11(context),
    gGraphBtnSsgP11(context),
    gGraphSeparator(context),
    graphBtnAmp{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    graphBtnPitch{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    graphBtnSsg{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    graphBtnSsg11{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    graphBtnSsgP11{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
    graphSeparator{ NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) }
{
    currentGraphMode.fill(GraphMode::Amp); // 初期状態はすべてAmp
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);

    int mode = context.audioProcessor.apvts.state.getProperty(ProcessorStateKey::rhythmViewMode, (int)GuiComponentViewModes::Twin);
    viewMode = (GuiComponentViewModes)mode;
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
    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

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
    feedbackTarget.setup({ .parent = mainGroup.contentCanvas, .title = "F.OP", .isReset = false });
    feedbackTarget.setRange(1.0, (double)Opzx7PrValue::ops, 1.0);
    feedbackTarget.setNumDecimalPlacesToDisplay(0);
    feedbackTarget.setWantsKeyboardFocus(true);
    feedbackTarget.setExplicitFocusOrder(++tabOrder);
    feedbackTarget.onValueChange = [this] { rebindFeedback(); };

    feedbackSlider.setupComponent(mainGroup.contentCanvas, "", Opzx7GuiText::Fm::fb, tabOrder, std::nullopt);
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

    targerOpSlider.setup({ .parent = mainGroup.contentCanvas, .title = "Op", .isReset = false });
    targerOpSlider.setRange(1.0, 6.0, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setValue(1, juce::sendNotification);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);

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

    viewModeComp.setupComponent(*this, tabOrder);
    viewModeComp.onChangeViewMode = [this](GuiComponentViewModes mode) {
        viewMode = mode;
        ctx.audioProcessor.apvts.state.setProperty(ProcessorStateKey::opzx7ViewMode, (int)viewMode, nullptr);
        ctx.editor.resized();
        };

    // Operators
    const juce::String opCode = code + CPK::op;

    for (int i = 0; i < Opzx7PrValue::ops; ++i)
    {
        opGroups[i].setup(*this, Opzx7GuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mulDetune[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, true);

        catAmp[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = Opzx7GuiText::Category::ampEnv, .detailVisible = true, .enableChangeDetailVisible = true });

        rgEn[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgEn, .title = Opzx7GuiText::Fm::Op::RgEn, .isReset = true });
        rgEn[i].setWantsKeyboardFocus(true);
        rgEn[i].setExplicitFocusOrder(++tabOrder);
        rgEn[i].onStateChange = [this, i] {
            updateOpGraph(i); // RG_ENを切り替えるときにアンプエンベロープの内容を切り替える
            ctx.editor.resized();
            };

		rgSeparator[i].setupComponent(opGroups[i].contentCanvas);

        rgAr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgAr, .title = Opzx7GuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgD1r[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgD1r, .title = Opzx7GuiText::Fm::Op::D1r, .isReset = true });
        rgD1r[i].setWantsKeyboardFocus(true);
        rgD1r[i].setExplicitFocusOrder(++tabOrder);

        rgD1l[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgD1l, .title = Opzx7GuiText::Fm::Op::D1l, .isReset = true });
        rgD1l[i].setWantsKeyboardFocus(true);
        rgD1l[i].setExplicitFocusOrder(++tabOrder);

        rgD2r[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgD2r, .title = Opzx7GuiText::Fm::Op::D2r, .isReset = true });
        rgD2r[i].setWantsKeyboardFocus(true);
        rgD2r[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgRr, .title = Opzx7GuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgTl, .title = Opzx7GuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ar[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ar, .title = Opzx7GuiText::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        ar[i].setWantsKeyboardFocus(true);
        ar[i].setExplicitFocusOrder(++tabOrder);

        d1r[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::d1r, .title = Opzx7GuiText::Fm::Op::D1r, .isReset = true, .regType = RegisterType::FmDr });
        d1r[i].setWantsKeyboardFocus(true);
        d1r[i].setExplicitFocusOrder(++tabOrder);

        d1l[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::d1l, .title = Opzx7GuiText::Fm::Op::D1l, .isReset = true, .regType = RegisterType::FmSl });
        d1l[i].setWantsKeyboardFocus(true);
        d1l[i].setExplicitFocusOrder(++tabOrder);

        d2r[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::d2r, .title = Opzx7GuiText::Fm::Op::D2r, .isReset = true, .regType = RegisterType::FmSr });
        d2r[i].setWantsKeyboardFocus(true);
        d2r[i].setExplicitFocusOrder(++tabOrder);

        rr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rr, .title = Opzx7GuiText::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });
        rr[i].setWantsKeyboardFocus(true);
        rr[i].setExplicitFocusOrder(++tabOrder);

        tl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::tl, .title = Opzx7GuiText::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });
        tl[i].setWantsKeyboardFocus(true);
        tl[i].setExplicitFocusOrder(++tabOrder);

        ksCat[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = Opzx7GuiText::Category::ks, .enableChangeDetailVisible = true });

        ksEn[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksEn, .title = Opzx7GuiText::Fm::Op::KsEn, .isReset = true });
        ksEn[i].setWantsKeyboardFocus(true);
        ksEn[i].setExplicitFocusOrder(++tabOrder);

        ksMode[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksMode, .title = "Mode", .items = ksModeItems, .isReset = true });
        ksMode[i].setWantsKeyboardFocus(true);
        ksMode[i].setExplicitFocusOrder(++tabOrder);
        ksMode[i].onChange = [this]() {
            ctx.editor.resized();
            };

        ksrMA7[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksrMA7, .title = Opzx7GuiText::Fm::Op::Ksr, .isReset = true });
        ksrMA7[i].setWantsKeyboardFocus(true);
        ksrMA7[i].setExplicitFocusOrder(++tabOrder);

        kslMA7[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::kslMA7, .title = Opzx7GuiText::Fm::Op::Ksl, .items = kslMA7Items, .isReset = true });
        kslMA7[i].setWantsKeyboardFocus(true);
        kslMA7[i].setExplicitFocusOrder(++tabOrder);

        ksrOPZ[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksrOPZ, .title = Opzx7GuiText::Fm::Op::Ksr, .isReset = true });
        ksrOPZ[i].setWantsKeyboardFocus(true);
        ksrOPZ[i].setExplicitFocusOrder(++tabOrder);

        kslOPZ[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::kslOPZ, .title = Opzx7GuiText::Fm::Op::Ksl, .isReset = true });
        kslOPZ[i].setWantsKeyboardFocus(true);
        kslOPZ[i].setExplicitFocusOrder(++tabOrder);

        ksBp[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksBp, .title = "B.P", .isReset = true});
        ksBp[i].setWantsKeyboardFocus(true);
        ksBp[i].setExplicitFocusOrder(++tabOrder);

        ksLc[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksLc, .title = "L.Cur", .items = ksCurveItems, .isReset = true});
        ksLc[i].setWantsKeyboardFocus(true);
        ksLc[i].setExplicitFocusOrder(++tabOrder);

        ksRc[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksRc, .title = "R.Cur", .items = ksCurveItems, .isReset = true });
        ksRc[i].setWantsKeyboardFocus(true);
        ksRc[i].setExplicitFocusOrder(++tabOrder);

        ksLd[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksLd, .title = "L.Dep", .isReset = true });
        ksLd[i].setWantsKeyboardFocus(true);
        ksLd[i].setExplicitFocusOrder(++tabOrder);

        ksRd[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksRd, .title = "R.Dep", .isReset = true });
        ksRd[i].setWantsKeyboardFocus(true);
        ksRd[i].setExplicitFocusOrder(++tabOrder);

        ksRs[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksRs, .title = "R.SC", .isReset = true});
        ksRs[i].setWantsKeyboardFocus(true);
        ksRs[i].setExplicitFocusOrder(++tabOrder);

        optionalSeparator[i].setupComponent(opGroups[i].contentCanvas);

        sus[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::sus, .title = Opzx7GuiText::Fm::Op::sus, .isReset = true });
        sus[i].setWantsKeyboardFocus(true);
        sus[i].setExplicitFocusOrder(++tabOrder);

        xof[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::xof, .title = Opzx7GuiText::Fm::Op::xof, .isReset = true });
        xof[i].setWantsKeyboardFocus(true);
        xof[i].setExplicitFocusOrder(++tabOrder);

        kor[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::kor, .title = Opzx7GuiText::Fm::Op::kor, .isReset = true });
        kor[i].setWantsKeyboardFocus(true);
        kor[i].setExplicitFocusOrder(++tabOrder);

        catWaveShape[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = Opzx7GuiText::Category::waveShape, .enableChangeDetailVisible = true });

        ws[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ws, .title = Opzx7GuiText::Fm::Op::Ws, .items = opzx7WsItems, .isReset = true });
        ws[i].setWantsKeyboardFocus(true);
        ws[i].setExplicitFocusOrder(++tabOrder);
        ws[i].onChange = [this, i] {
            updateOnWsChange(i);

            ctx.editor.resized();
            };

        wsSeparator[i].setupComponent(opGroups[i].contentCanvas);

        wsPreview[i].setup(opGroups[i].contentCanvas, GuiColor::WavePreview::WaveMemory);

        loadPcmBtn[i].setup({ .parent = opGroups[i].contentCanvas, .title = Opzx7GuiText::File::Pcm, .isReset = false, .isResized = true });
        loadPcmBtn[i].setWantsKeyboardFocus(true);
        loadPcmBtn[i].setExplicitFocusOrder(++tabOrder);
        loadPcmBtn[i].onClick = [this, i] {
            auto fileFilter = ctx.audioProcessor.formatManager.getWildcardForAllFormats();
            ctx.editor.openFileChooser(
                "Load PCM for OP" + juce::String(i + 1),
                ctx.audioProcessor.lastSampleDirectory,
                fileFilter,
                [this, i](const juce::FileChooser& fc) {
                    auto file = fc.getResult();
                    if (file.existsAsFile()) {
                        updatePcmFileName(i, "Loading...");

                        // 発火するころには画面が消えているかもしれないので、弱い参照で見張る。
                        juce::Component::SafePointer<std::remove_pointer_t<decltype(this)>> safe(this);

                        juce::Timer::callAfterDelay(50, [this, safe, i, file]()
                            {
                                // 画面が閉じられていたら何もしない。callAfterDelay は取り消せず、
                                // メッセージが詰まっていれば 50ms よりずっと遅れて発火する。
                                if (safe == nullptr) return;

                                ctx.audioProcessor.loadOpzx7PcmFile(i, file);
                                updatePcmFileName(i, file.getFileName());
                                ctx.audioProcessor.lastSampleDirectory = file.getParentDirectory();
                            });
                    }
                }
            );
            };

        clearPcmBtn[i].setup({ .parent = opGroups[i].contentCanvas, .title = Opzx7GuiText::File::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = true });
        clearPcmBtn[i].setWantsKeyboardFocus(true);
        clearPcmBtn[i].setExplicitFocusOrder(++tabOrder);
        clearPcmBtn[i].onClick = [this, i] {
            ctx.audioProcessor.unloadOpzx7PcmFile(i);
            updatePcmFileName(i, Io::empty);
            };

        pcmFileNameLabel[i].setup({ .parent = opGroups[i].contentCanvas, .title = Io::empty });
        if (ctx.audioProcessor.opzx7PcmFilePaths[i].isNotEmpty()) {
            updatePcmFileName(i, juce::File(ctx.audioProcessor.opzx7PcmFilePaths[i]).getFileName());
        }

        pcmOffset[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::pcmOffset, .title = Opzx7GuiText::Fm::Op::PcmOffset, .isReset = true });
        pcmOffset[i].setWantsKeyboardFocus(true);
        pcmOffset[i].setExplicitFocusOrder(++tabOrder);

        pcmRatio[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::pcmRatio, .title = Opzx7GuiText::Fm::Op::PcmRatio, .isReset = true });
        pcmRatio[i].setWantsKeyboardFocus(true);
        pcmRatio[i].setExplicitFocusOrder(++tabOrder);

        loopPointEnable[i].setup({ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::lpEnable, .title = Opzx7GuiText::Fm::Op::loopPointEnable, .isReset = true });
        loopPointEnable[i].setWantsKeyboardFocus(true);
        loopPointEnable[i].setExplicitFocusOrder(++tabOrder);

        loopPointStart[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::lpStart, .title = Opzx7GuiText::Fm::Op::loopPointStart, .isReset = true });
        loopPointStart[i].setWantsKeyboardFocus(true);
        loopPointStart[i].setExplicitFocusOrder(++tabOrder);

        loopPointEnd[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::lpEnd, .title = Opzx7GuiText::Fm::Op::loopPointEnd, .isReset = true });
        loopPointEnd[i].setWantsKeyboardFocus(true);
        loopPointEnd[i].setExplicitFocusOrder(++tabOrder);

        // 切り出しとループの設定が変わったら、プレビューも合わせる
        auto refreshWsPreview = [this, i]() { this->updateWsPreview(i); };

        pcmOffset[i].onValueChange = refreshWsPreview;
        pcmRatio[i].onValueChange = refreshWsPreview;
        loopPointStart[i].onValueChange = refreshWsPreview;
        loopPointEnd[i].onValueChange = refreshWsPreview;
        loopPointEnable[i].onStateChange = refreshWsPreview;

        loadWtBtn[i].setup({ .parent = opGroups[i].contentCanvas, .title = "WT", .isReset = false, .isResized = true });
        loadWtBtn[i].setWantsKeyboardFocus(true);
        loadWtBtn[i].setExplicitFocusOrder(++tabOrder);
        loadWtBtn[i].onClick = [this, i] {
            ctx.editor.openFileChooser(
                "Load Wavetable for OP" + juce::String(i + 1),
                ctx.audioProcessor.defaultWavetableDir,
                "*.wt",
                [this, i](const juce::FileChooser& fc) {
                    auto file = fc.getResult();
                    if (file.existsAsFile()) {
                        updateWtFileName(i, "Loading...");

                        // 発火するころには画面が消えているかもしれないので、弱い参照で見張る。
                        juce::Component::SafePointer<std::remove_pointer_t<decltype(this)>> safe(this);

                        juce::Timer::callAfterDelay(50, [this, safe, i, file]()
                            {
                                // 画面が閉じられていたら何もしない。callAfterDelay は取り消せず、
                                // メッセージが詰まっていれば 50ms よりずっと遅れて発火する。
                                if (safe == nullptr) return;

                                ctx.audioProcessor.loadOpzx7WtFile(i, file);
                                updateWtFileName(i, file.getFileName());
                                ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();
                            });
                    }
                }
            );
            };

        clearWtBtn[i].setup({ .parent = opGroups[i].contentCanvas, .title = Opzx7GuiText::File::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = true });
        clearWtBtn[i].setWantsKeyboardFocus(true);
        clearWtBtn[i].setExplicitFocusOrder(++tabOrder);
        clearWtBtn[i].onClick = [this, i] {
            ctx.audioProcessor.unloadOpzx7WtFile(i);
            updateWtFileName(i, Io::empty);
            };

        wtFileNameLabel[i].setup({ .parent = opGroups[i].contentCanvas, .title = Io::empty });
        if (ctx.audioProcessor.opzx7WtFilePaths[i].isNotEmpty()) {
            updateWtFileName(i, juce::File(ctx.audioProcessor.opzx7WtFilePaths[i]).getFileName());
        }

        loadWt2Btn[i].setup({ .parent = opGroups[i].contentCanvas, .title = "WT2", .isReset = false, .isResized = true });
        loadWt2Btn[i].setWantsKeyboardFocus(true);
        loadWt2Btn[i].setExplicitFocusOrder(++tabOrder);
        loadWt2Btn[i].onClick = [this, i] {
            ctx.editor.openFileChooser(
                "Load WT2for OP" + juce::String(i + 1),
                ctx.audioProcessor.defaultWavetableDir,
                "*.wt2",
                [this, i](const juce::FileChooser& fc) {
                    auto file = fc.getResult();
                    if (file.existsAsFile()) {
                        updateWt2FileName(i, "Loading...");

                        // 発火するころには画面が消えているかもしれないので、弱い参照で見張る。
                        juce::Component::SafePointer<std::remove_pointer_t<decltype(this)>> safe(this);

                        juce::Timer::callAfterDelay(50, [this, safe, i, file]()
                            {
                                // 画面が閉じられていたら何もしない。callAfterDelay は取り消せず、
                                // メッセージが詰まっていれば 50ms よりずっと遅れて発火する。
                                if (safe == nullptr) return;

                                ctx.audioProcessor.loadOpzx7Wt2File(i, file);
                                updateWt2FileName(i, file.getFileName());
                                ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();
                            });
                    }
                }
            );
            };

        clearWt2Btn[i].setup({ .parent = opGroups[i].contentCanvas, .title = Opzx7GuiText::File::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = true });
        clearWt2Btn[i].setWantsKeyboardFocus(true);
        clearWt2Btn[i].setExplicitFocusOrder(++tabOrder);
        clearWt2Btn[i].onClick = [this, i] {
            ctx.audioProcessor.unloadOpzx7Wt2File(i);
            updateWt2FileName(i, Io::empty);
            };

        wt2FileNameLabel[i].setup({ .parent = opGroups[i].contentCanvas, .title = Io::empty });
        if (ctx.audioProcessor.opzx7WtFilePaths[i].isNotEmpty()) {
            updateWt2FileName(i, juce::File(ctx.audioProcessor.opzx7Wt2FilePaths[i]).getFileName());
        }

        catOptional[i].setupSwCategory({ .parent = opGroups[i].contentCanvas, .title = Opzx7GuiText::Category::optional, .enableChangeDetailVisible = true });

        bypass[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::bypass, .title = Opzx7GuiText::Fm::Op::bypass, .isReset = true });
        bypass[i].setWantsKeyboardFocus(true);
        bypass[i].setExplicitFocusOrder(++tabOrder);

        catSsgEnv[i].setupSwCategory({ .parent = opGroups[i].contentCanvas, .title = Opzx7GuiText::Category::ssgEnv, .enableChangeDetailVisible = true });

        se[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::se, .title = Opzx7GuiText::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
        se[i].setWantsKeyboardFocus(true);
        se[i].setExplicitFocusOrder(++tabOrder);

        seFreq[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::seFreq, .title = Opzx7GuiText::Fm::Op::SFreq, .isReset = true });
        seFreq[i].setWantsKeyboardFocus(true);
        seFreq[i].setExplicitFocusOrder(++tabOrder);

        sePreview[i].setup(opGroups[i].contentCanvas, GuiColor::WavePreview::AmpEnv);

        // 形と周期のどちらが変わっても描き直す
        se[i].onChange = [this, i] { updateSePreview(i); };
        seFreq[i].onValueChange = [this, i] { updateSePreview(i); };

        updateSePreview(i);

        pitchEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::PitchAdsr::enable, Opzx7GuiText::PitchAdsr::enable, true);

        ssgSwEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv::enable, Opzx7GuiText::SsgSwEnv::enable, true);

        ssgSwEnv11[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv11::enable, Opzx7GuiText::SsgSwEnv11::enable, true);

        ssgSwPEnv11[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwPEnv11::enable, Opzx7GuiText::SsgSwPEnv11::enable, true);
        ssgHwPEnvOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);
        wtAmpModOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);
        ssgHwEnvOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);
        wtModOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);

        lfo[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);

        fix[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, Opzx7GuiText::Fm::Op::Opzx7FreqTo440, 440);

        catMask[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = Opzx7GuiText::Category::mask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::mask, .title = Opzx7GuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        mmlSeparator[i].setupComponent(opGroups[i].contentCanvas);

        mml[i].setup({ .parent = opGroups[i].contentCanvas, .title = juce::String("") + "MML風入力", .isReset = false, .isResized = false });
        mml[i].setWantsKeyboardFocus(true);
        mml[i].setExplicitFocusOrder(++tabOrder);
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = juce::String("") + "MML風にパラメータを入力してください。 例: AR:31 AR31 DT-1 等",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        setupGraph(i);
        updateOpGraph(i);
    }
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

    viewModeComp.layoutComponent(mmRect);

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

    ampEnvComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv));
    ampEnvComponent.layoutComponent(mRect);

    ssgHwEnv.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv));
    ssgHwEnv.layoutComponent(mRect);
    ssgSwEnv11g.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11));
    ssgSwEnv11g.layoutComponent(mRect);
    ampModComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));
    ampModComponent.layoutComponent(mRect);

    ssgHwPEnv.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv));
    ssgHwPEnv.layoutComponent(mRect);
    ssgSwPEnv11g.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11));
    ssgSwPEnv11g.layoutComponent(mRect);
    modComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));
    modComponent.layoutComponent(mRect);

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

    int opWidth = pageArea.getWidth() / 4;

    switch (viewMode) {
    case GuiComponentViewModes::Top:
    {
        for (int i = 4; i < Opzx7PrValue::ops; i++) {
            updateOpVisible(i, false);
        }

        for (int i = 0; i < 4; ++i)
        {
            updateOpVisible(i, true);
            layoutOp(i, opWidth, pageArea);
        }
        break;
    }
    case GuiComponentViewModes::Bottom:
    {
        for (int i = 0; i < 4; i++) {
            updateOpVisible(i, false);
        }

        for (int i = 4; i < Opzx7PrValue::ops; ++i)
        {
            updateOpVisible(i, true);
            layoutOp(i, opWidth, pageArea);
        }

        break;
    }
    case GuiComponentViewModes::Twin:
    {
        auto upperOpArea = pageArea.removeFromTop(pageArea.getHeight() / 2);

        for (int i = 0; i < 4; ++i)
        {
            updateOpVisible(i, true);
            layoutOp(i, opWidth, upperOpArea);
        }

        for (int i = 4; i < Opzx7PrValue::ops; ++i)
        {
            updateOpVisible(i, true);
            layoutOp(i, opWidth, pageArea);
        }

        break;

    }
    }

    int mode = algModeSelector.getSelectedItemIndex();

    if (mode == 0) {
        updateAlgorithmDisplay();
    }
    else {
        updateAlgorithmMatrixDisplay();
    }
}

void GuiOpzx7::layoutOp(int opIndex, int width, juce::Rectangle<int>& rect) {
    auto opArea = rect.removeFromLeft(width);

    // 枠線
    opGroups[opIndex].setBounds(opArea);

    // 枠線の内側
    auto iinnerRect = opArea.reduced(Opzx7GuiValue::Fm::Op::Padding::width, Opzx7GuiValue::Fm::Op::Padding::height);
    iinnerRect.removeFromTop(Opzx7GuiValue::Group::TitlePaddingTop);

    // グラフ用の区画を確保
    layoutOpGraph(opIndex, iinnerRect);
    updateOpGraph(opIndex);

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    opGroups[opIndex].setViewportCustomBounds(iinnerRect.translated(-opArea.getX(), -opArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> innerRect(0, 0, opGroups[opIndex].getContentWidth(), 2000);

    bool rgMode = rgEn[opIndex].getToggleState();
    int selectedWs = ws[opIndex].getSelectedItemIndex();

    layoutOpAmpCat(opIndex, innerRect, rgMode);

    layoutOpOptionalCat(opIndex, innerRect);

    layoutOpWsCat(opIndex, innerRect, selectedWs);

    layoutOpSsgEnvCat(opIndex, innerRect);
    ssgHwEnvOp[opIndex].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv));
    ssgHwEnvOp[opIndex].layoutComponentRow(innerRect);
    ssgSwEnv[opIndex].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv));
    ssgSwEnv[opIndex].layoutComponentRow(innerRect);
    ssgSwEnv11[opIndex].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11));
    ssgSwEnv11[opIndex].layoutComponentRow(innerRect);
    wtAmpModOp[opIndex].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));
    wtAmpModOp[opIndex].layoutComponent(innerRect);

    pitchEnv[opIndex].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv));
    pitchEnv[opIndex].layoutComponentRow(innerRect);
    ssgHwPEnvOp[opIndex].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv));
    ssgHwPEnvOp[opIndex].layoutComponentRow(innerRect);
    ssgSwPEnv11[opIndex].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11));
    ssgSwPEnv11[opIndex].layoutComponentRow(innerRect);
    wtModOp[opIndex].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));
    wtModOp[opIndex].layoutComponent(innerRect);

    layoutOpKsCat(opIndex, innerRect, rgMode);

    mulDetune[opIndex].layoutComponentRow(innerRect);

    lfo[opIndex].layoutComponentRow(innerRect);

    fix[opIndex].layoutComponentRow(innerRect);

    layoutOpMaskCat(opIndex, innerRect);

    mmlSeparator[opIndex].layoutComponent(innerRect);

    layoutRow({ .rowRect = innerRect, .component = &mml[opIndex], .paddingBottom = 0 });

    int usedHeight = 2000 - innerRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    opGroups[opIndex].setContentHeight(usedHeight + 20);

    updateOnWsChange(opIndex);
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpzx7::applyMmlString(const juce::String& mml, int opIndex)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);
    bool rgMode = rgEn[opIndex].getToggleState();

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mulDetune[opIndex].setMul(RegisterConverter::convertFmMulOpzx7(v)); } },
        { mmlPrefixMl,   [&](int v) { mulDetune[opIndex].setMul(RegisterConverter::convertFmMulOpzx7(v)); } },
        { mmlPrefixDt,   [&](int v) { mulDetune[opIndex].setDt1(RegisterConverter::convertFmDtOpzx7(v)); } },
        { mmlPrefixDt1,   [&](int v) { mulDetune[opIndex].setDt1(RegisterConverter::convertFmDtOpzx7(v)); } },
        { mmlPrefixDto,   [&](int v) { mulDetune[opIndex].setDt1(RegisterConverter::convertFmDtOpzx7(v)); } },
        { mmlPrefixDt2,  [&](int v) { mulDetune[opIndex].setDt2(RegisterConverter::convertMmlDt2ToReg(v)); } },
        { mmlPrefixDtt,  [&](int v) { mulDetune[opIndex].setDt2(RegisterConverter::convertMmlDt2ToReg(v)); } },
        { mmlPrefixMask, [&](int v) { mask[opIndex].setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },

        // --- TL系 (RGモードで分岐) ---
        { mmlPrefixTl,   [&](int v) {
            rgMode ? rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification)
                   : tl[opIndex].setValue(RegisterConverter::convertFmTl(v), juce::sendNotification);
        }},
        { mmlPrefixO,    [&](int v) {
            rgMode ? rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification)
                   : tl[opIndex].setValue(RegisterConverter::convertFmTl(v), juce::sendNotification);
        }},
        { mmlPrefixOl,   [&](int v) {
            rgMode ? rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification)
                   : tl[opIndex].setValue(RegisterConverter::convertFmTl(v), juce::sendNotification);
        }},

        // --- エンベロープ系 (RGモードで分岐) ---
        { mmlPrefixAr,   [&](int v) {
            rgMode ? rgAr[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification)
                   : ar[opIndex].setValue(RegisterConverter::convertFmAr(v), juce::sendNotification);
        }},
        { mmlPrefixD1r,   [&](int v) {
            rgMode ? rgD1r[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification)
                   : d1r[opIndex].setValue(RegisterConverter::convertFmDr(v), juce::sendNotification);
        }},
        { mmlPrefixD2r,   [&](int v) {
            rgMode ? rgD2r[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification)
                   : d2r[opIndex].setValue(RegisterConverter::convertFmSr(v), juce::sendNotification);
        }},
        { mmlPrefixD1l,   [&](int v) {
            rgMode ? rgD1l[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification)
                   : d1l[opIndex].setValue(RegisterConverter::convertFmSl(v), juce::sendNotification);
        }},
        { mmlPrefixRr,   [&](int v) {
            rgMode ? rgRr[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification)
                   : rr[opIndex].setValue(RegisterConverter::convertFmRr(v), juce::sendNotification);
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

void GuiOpzx7::updateOpVisible(int idx, bool visible) {
    opGroups[idx].setVisible(visible);

    mulDetune[idx].setVisible(visible);
    ar[idx].setVisibleWithLabel(visible);
    d1r[idx].setVisibleWithLabel(visible);
    d1l[idx].setVisibleWithLabel(visible);
    rr[idx].setVisibleWithLabel(visible);
    d2r[idx].setVisibleWithLabel(visible);
    tl[idx].setVisibleWithLabel(visible);
    ksEn[idx].setVisible(visible);
    ksrMA7[idx].setVisible(visible);
    kslMA7[idx].setVisibleWithLabel(visible);
    ksrOPZ[idx].setVisible(visible);
    kslOPZ[idx].setVisibleWithLabel(visible);
    ksBp[idx].setVisible(visible);
    ksBp[idx].setVisibleWithLabel(visible);
    ksLc[idx].setVisible(visible);
    ksLc[idx].setVisibleWithLabel(visible);
    ksRc[idx].setVisible(visible);
    ksRc[idx].setVisibleWithLabel(visible);
    ksLd[idx].setVisible(visible);
    ksLd[idx].setVisibleWithLabel(visible);
    ksRd[idx].setVisible(visible);
    ksRd[idx].setVisibleWithLabel(visible);
    ksRs[idx].setVisible(visible);
    ksRs[idx].setVisibleWithLabel(visible);
    se[idx].setVisibleWithLabel(visible);
    seFreq[idx].setVisibleWithLabel(visible);
    catOptional[idx].setVisible(visible);
    bypass[idx].setVisible(visible);
    fix[idx].setVisible(visible);
    lfo[idx].setVisible(visible);
    catWaveShape[idx].setVisible(visible);
    ws[idx].setVisibleWithLabel(visible);
    loadPcmBtn[idx].setVisible(visible);
    clearPcmBtn[idx].setVisible(visible);
    pcmFileNameLabel[idx].setVisible(visible);
    pcmOffset[idx].setVisibleWithLabel(visible);
    pcmRatio[idx].setVisibleWithLabel(visible);
    loadWtBtn[idx].setVisible(visible);
    clearWtBtn[idx].setVisible(visible);
    wtFileNameLabel[idx].setVisible(visible);
    loadWt2Btn[idx].setVisible(visible);
    clearWt2Btn[idx].setVisible(visible);
    wt2FileNameLabel[idx].setVisible(visible);
    catMask[idx].setVisible(visible);
    mask[idx].setVisible(visible);
    mmlSeparator[idx].setVisible(visible);
    mml[idx].setVisible(visible);
    rgEn[idx].setVisible(visible);
    rgSeparator[idx].setVisible(visible);
    rgAr[idx].setVisibleWithLabel(visible);
    rgD1r[idx].setVisibleWithLabel(visible);
    rgD1r[idx].setVisibleWithLabel(visible);
    rgD1l[idx].setVisibleWithLabel(visible);
    rgD1l[idx].setVisibleWithLabel(visible);
    rgD2r[idx].setVisibleWithLabel(visible);
    rgD2r[idx].setVisibleWithLabel(visible);
    rgRr[idx].setVisibleWithLabel(visible);
    rgRr[idx].setVisible(visible);
    rgTl[idx].setVisibleWithLabel(visible);
    rgTl[idx].setEnabled(visible);
	optionalSeparator[idx].setVisible(visible);
    sus[idx].setVisible(visible);
    xof[idx].setVisible(visible);
    kor[idx].setVisible(visible);
    pitchEnv[idx].setVisible(visible);
    ssgSwEnv[idx].setVisible(visible);
    ssgSwEnv11[idx].setVisible(visible);
    ssgSwPEnv11[idx].setVisible(visible);
    catSsgEnv[idx].setVisible(visible);
    se[idx].setVisible(visible);
    seFreq[idx].setVisible(visible);
    opGraphs[idx].setVisible(visible);
    graphBtnAmp[idx].setVisible(visible);
    graphBtnPitch[idx].setVisible(visible);
    graphBtnSsg[idx].setVisible(visible);
    graphBtnSsg11[idx].setVisible(visible);
    graphBtnSsgP11[idx].setVisible(visible);
    graphSeparator[idx].setVisible(visible);
}

void GuiOpzx7::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);

    mulDetune[idx].setEnables(enable);
    ar[idx].setEnabledWithLabel(enable);
    d1r[idx].setEnabledWithLabel(enable);
    d1l[idx].setEnabledWithLabel(enable);
    rr[idx].setEnabledWithLabel(enable);
    d2r[idx].setEnabledWithLabel(enable);
    tl[idx].setEnabledWithLabel(enable);
    ksEn[idx].setEnabled(enable);
    ksrMA7[idx].setEnabled(enable);
    kslMA7[idx].setEnabledWithLabel(enable);
    ksrOPZ[idx].setEnabled(enable);
    kslOPZ[idx].setEnabledWithLabel(enable);
    ksBp[idx].setEnabled(enable);
    ksBp[idx].setEnabledWithLabel(enable);
    ksLc[idx].setEnabled(enable);
    ksLc[idx].setEnabledWithLabel(enable);
    ksRc[idx].setEnabled(enable);
    ksRc[idx].setEnabledWithLabel(enable);
    ksLd[idx].setEnabled(enable);
    ksLd[idx].setEnabledWithLabel(enable);
    ksRd[idx].setEnabled(enable);
    ksRd[idx].setEnabledWithLabel(enable);
    ksRs[idx].setEnabled(enable);
    ksRs[idx].setEnabledWithLabel(enable);
    se[idx].setEnabledWithLabel(enable);
    seFreq[idx].setEnabledWithLabel(enable);
    catOptional[idx].setEnabled(enable);
    bypass[idx].setEnabled(enable);
    fix[idx].setEnables(enable);
    lfo[idx].setEnabled(enable);
    catWaveShape[idx].setEnabled(enable);
    ws[idx].setEnabledWithLabel(enable);
    loadPcmBtn[idx].setEnabled(enable);
    clearPcmBtn[idx].setEnabled(enable);
    pcmFileNameLabel[idx].setEnabled(enable);
    pcmOffset[idx].setEnabledWithLabel(enable);
    pcmRatio[idx].setEnabledWithLabel(enable);
    loadWtBtn[idx].setEnabled(enable);
    clearWtBtn[idx].setEnabled(enable);
    wtFileNameLabel[idx].setEnabled(enable);
    loadWt2Btn[idx].setEnabled(enable);
    clearWt2Btn[idx].setEnabled(enable);
    wt2FileNameLabel[idx].setEnabled(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    mmlSeparator[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
    rgEn[idx].setEnabled(enable);
	rgSeparator[idx].setEnabled(enable);
    rgAr[idx].setEnabledWithLabel(enable);
    rgD1r[idx].setEnabledWithLabel(enable);
    rgD1r[idx].setEnabledWithLabel(enable);
    rgD1l[idx].setEnabledWithLabel(enable);
    rgD1l[idx].setEnabledWithLabel(enable);
    rgD2r[idx].setEnabledWithLabel(enable);
    rgD2r[idx].setEnabledWithLabel(enable);
    rgRr[idx].setEnabledWithLabel(enable);
    rgRr[idx].setEnabled(enable);
    rgTl[idx].setEnabledWithLabel(enable);
    rgTl[idx].setEnabled(enable);
	optionalSeparator[idx].setEnabled(enable);
    sus[idx].setEnabled(enable);
    xof[idx].setEnabled(enable);
    kor[idx].setEnabled(enable);
    pitchEnv[idx].setEnabled(enable);
    ssgSwEnv[idx].setEnabled(enable);
    ssgSwEnv11[idx].setEnabled(enable);
    ssgSwPEnv11[idx].setEnabled(enable);
    catSsgEnv[idx].setEnabled(enable);
    se[idx].setEnabled(enable);
    seFreq[idx].setEnabled(enable);
    opGraphs[idx].setEnabled(enable);
    graphBtnAmp[idx].setEnabled(enable);
    graphBtnPitch[idx].setEnabled(enable);
    graphBtnSsg[idx].setEnabled(enable);
    graphBtnSsg11[idx].setEnabled(enable);
    graphBtnSsgP11[idx].setEnabled(enable);
    graphSeparator[idx].setEnabled(enable);
}

// 選んでいる WS の波形を折れ線にする。
// 描画のたびに計算すると重いので、形が変わったときだけここを通す。
void GuiOpzx7::updateWsPreview(int opIndex)
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す。
    // オペレータごとに別々に溜まるよう、番号を目印に混ぜる。
    if (GuiRefresh::defer((const char*)this + opIndex, [this, opIndex] { updateWsPreview(opIndex); })) return;

    if (opIndex < 0 || opIndex >= Opzx7PrValue::ops) return;

    // 波形メモリと PCM は未読込なら空。音源側と同じくサイン波になる。
    // PCM のときだけ P.OF / P.RT で切り出す範囲が変わるので、一緒に渡す。
    wsPreview[opIndex].setPoints(
        WavePreviewSource::opzx7Ws(
            ws[opIndex].getSelectedItemIndex(),
            ctx.audioProcessor.opzx7WtBuffers[opIndex],
            ctx.audioProcessor.opzx7Wt2Buffers[opIndex],
            ctx.audioProcessor.opzx7PcmBuffers[opIndex],
            (float)pcmOffset[opIndex].getValue(),
            (float)pcmRatio[opIndex].getValue(),
            ctx.audioProcessor.getSampleRate()),
        true);

    // 同じ枠が波形メモリとオーディオファイルの両方を映すので、
    // 何を出しているかに合わせて線の色も変える。
    bool isPcm = ws[opIndex].getSelectedItemIndex() == Opzx7PrValue::pcmIndex;

    wsPreview[opIndex].setLineColour(isPcm
        ? GuiColor::WavePreview::AudioFile
        : GuiColor::WavePreview::WaveMemory);

    // ループ位置は PCM のときだけ意味を持つ。切り出した範囲に対する 0.0〜1.0。
    std::vector<float> markers;

    if (isPcm && loopPointEnable[opIndex].getToggleState()) {
        markers.push_back((float)loopPointStart[opIndex].getValue());
        markers.push_back((float)loopPointEnd[opIndex].getValue());
    }

    wsPreview[opIndex].setMarkers(markers);
}

void GuiOpzx7::updateOnWsChange(int idx)
{
    updateWsPreview(idx);

    int selectedWs = ws[idx].getSelectedItemIndex();
    bool visible = catWaveShape[idx].isDetailVisible();
    if (selectedWs == Opzx7PrValue::wtIndex)
    {
        // WT関連だけ表示
        wsSeparator[idx].setVisible(visible);
        loadPcmBtn[idx].setVisible(false);
        clearPcmBtn[idx].setVisible(false);
        pcmFileNameLabel[idx].setVisible(false);
        pcmOffset[idx].setVisibleWithLabel(false);
        pcmRatio[idx].setVisibleWithLabel(false);
        loopPointEnable[idx].setVisible(false);
        loopPointStart[idx].setVisibleWithLabel(false);
        loopPointEnd[idx].setVisibleWithLabel(false);
        loadWtBtn[idx].setVisible(visible);
        clearWtBtn[idx].setVisible(visible);
        wtFileNameLabel[idx].setVisible(visible);
        loadWt2Btn[idx].setVisible(false);
        clearWt2Btn[idx].setVisible(false);
        wt2FileNameLabel[idx].setVisible(false);
    }
    else if (selectedWs == Opzx7PrValue::wt2Index)
    {
        // WT2関連だけ表示
        wsSeparator[idx].setVisible(visible);
        loadPcmBtn[idx].setVisible(false);
        clearPcmBtn[idx].setVisible(false);
        pcmFileNameLabel[idx].setVisible(false);
        pcmOffset[idx].setVisibleWithLabel(false);
        pcmRatio[idx].setVisibleWithLabel(false);
        loopPointEnable[idx].setVisible(false);
        loopPointStart[idx].setVisibleWithLabel(false);
        loopPointEnd[idx].setVisibleWithLabel(false);
        loadWtBtn[idx].setVisible(false);
        clearWtBtn[idx].setVisible(false);
        wtFileNameLabel[idx].setVisible(false);
        loadWt2Btn[idx].setVisible(visible);
        clearWt2Btn[idx].setVisible(visible);
        wt2FileNameLabel[idx].setVisible(visible);
    }
    else if (selectedWs == Opzx7PrValue::pcmIndex)
    {
        // PCM関連だけ表示
        wsSeparator[idx].setVisible(visible);
        loadPcmBtn[idx].setVisible(visible);
        clearPcmBtn[idx].setVisible(visible);
        pcmFileNameLabel[idx].setVisible(visible);
        pcmOffset[idx].setVisibleWithLabel(visible);
        pcmRatio[idx].setVisibleWithLabel(visible);
        loopPointEnable[idx].setVisible(visible);
        loopPointStart[idx].setVisibleWithLabel(visible);
        loopPointEnd[idx].setVisibleWithLabel(visible);
        loadWtBtn[idx].setVisible(false);
        clearWtBtn[idx].setVisible(false);
        wtFileNameLabel[idx].setVisible(false);
        loadWt2Btn[idx].setVisible(false);
        clearWt2Btn[idx].setVisible(false);
        wt2FileNameLabel[idx].setVisible(false);
    }
    else {
        // 全関連非表示
        wsSeparator[idx].setVisible(false);
        loadPcmBtn[idx].setVisible(false);
        clearPcmBtn[idx].setVisible(false);
        pcmFileNameLabel[idx].setVisible(false);
        pcmOffset[idx].setVisibleWithLabel(false);
        pcmRatio[idx].setVisibleWithLabel(false);
        loopPointEnable[idx].setVisible(false);
        loopPointStart[idx].setVisibleWithLabel(false);
        loopPointEnd[idx].setVisibleWithLabel(false);
        loadWtBtn[idx].setVisible(false);
        clearWtBtn[idx].setVisible(false);
        wtFileNameLabel[idx].setVisible(false);
        loadWt2Btn[idx].setVisible(false);
        clearWt2Btn[idx].setVisible(false);
        wt2FileNameLabel[idx].setVisible(false);
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
        updateOpEnable(i, activeOps[i]);
    }
}

void GuiOpzx7::updateAlgorithmMatrixDisplay()
{
    FmAlgState s = algMatrixComp.getState();
    auto activeOps = s.getActiveOperators();

    for (int i = 0; i < Opzx7PrValue::ops; ++i)
    {
        updateOpEnable(i, activeOps[i]);
    }
}

void GuiOpzx7::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].setVisibleWithLabel(rgMode);
    rgD1r[idx].setVisibleWithLabel(rgMode);
    rgD1l[idx].setVisibleWithLabel(rgMode);
    rgD2r[idx].setVisibleWithLabel(rgMode);
    rgRr[idx].setVisibleWithLabel(rgMode);
    rgTl[idx].setVisibleWithLabel(rgMode);

    ar[idx].setVisibleWithLabel(!rgMode);
    d1r[idx].setVisibleWithLabel(!rgMode);
    d1l[idx].setVisibleWithLabel(!rgMode);
    d2r[idx].setVisibleWithLabel(!rgMode);
    rr[idx].setVisibleWithLabel(!rgMode);
    tl[idx].setVisibleWithLabel(!rgMode);
}

void GuiOpzx7::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

// ==============================================================================
// Keyboard Shortcut Logic
// ==============================================================================
bool GuiOpzx7::keyPressed(const juce::KeyPress& key)
{
    int opIndex = -1;
    int code = key.getKeyCode();
    juce::ModifierKeys metaKeys = key.getModifiers();

    // 通常の 1〜4キー、または テンキーの 1〜4 を判定
    if (code == '1' || code == juce::KeyPress::numberPad1) opIndex = 0;
    else if (code == '2' || code == juce::KeyPress::numberPad2) opIndex = 1;
    else if (code == '3' || code == juce::KeyPress::numberPad3) opIndex = 2;
    else if (code == '4' || code == juce::KeyPress::numberPad4) opIndex = 3;
    else if (code == '5' || code == juce::KeyPress::numberPad5) opIndex = 4;
    else if (code == '6' || code == juce::KeyPress::numberPad6) opIndex = 5;

    // 対応するキーが押されていたら、該当する処理を実行
    if (opIndex != -1)
    {
        // Altキーを押しながら -> マスクのON/OFF、それ以外 -> MMLボタンクリック
        // 該当オペレータが有効(Enabled)な時のみ反応させる
        if (metaKeys.isAltDown() && mask[opIndex].isEnabled()) {
            mask[opIndex].setToggleState(!mask[opIndex].getToggleState(), juce::sendNotification);
        }
        else if (mml[opIndex].isEnabled()) {
            mml[opIndex].triggerClick();
        }

        return true; // キー入力を消費したことをJUCEに伝える
    }

    return false; // 他のキーなら無視（通常処理へ）
}

void GuiOpzx7::copyFmParamsToString()
{
    auto formatOpExt = [this](int index) {
        // ' MUL AR DR SL RR TL KSR KSL
        return juce::String::formatted(
            u8"MUL%d DT1%+d DT2+%d AR%d D1R%d D1L%d D2R%d RR%d TL%d\n",
            (int)this->mulDetune[index].getMul(),
            this->mulDetune[index].getDt1(),
            (int)this->mulDetune[index].getDt2(),
            (int)this->rgAr[index].getValue(),
            (int)this->rgD1r[index].getValue(),
            (int)this->rgD1l[index].getValue(),
            (int)this->rgD2r[index].getValue(),
            (int)this->rgRr[index].getValue(),
            (int)this->rgTl[index].getValue()
        );
        };
    auto formatOpsExt = [this, formatOpExt]() {
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
    targerOpSlider.setVisibleWithLabel(visible);
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
        layoutMain({ .mainRect = rect, .label = &targerOpSlider.label, .component = &targerOpSlider });

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

void GuiOpzx7::layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask[opIndex] });

    bool visibleMask = catMask[opIndex].isDetailVisible();

    mask[opIndex].setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask[opIndex] });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }

    // MASK の後ろには分類の外の行 (セパレータや MML) が続くので、
    // 板をここで閉じないと 1 行ぶん下まで伸びてしまう。
    closeCategoryBackdrops(catMask[opIndex].getParentComponent(), rect.getY());
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
void GuiOpzx7::updateSePreview(int opIndex)
{
    int selected = se[opIndex].getSelectedItemIndex();

    if (selected <= 0)
    {
        // 掛けないときは平らな線にする
        sePreview[opIndex].setPoints(std::vector<float>(2, 1.0f), false);

        return;
    }

    sePreview[opIndex].setPoints(
        WavePreviewSource::ssgHwEnv(selected - 1, 0.0f, 1.0f, false), false);
}

void GuiOpzx7::layoutOpSsgEnvCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catSsgEnv[opIndex] });

    bool visible = catSsgEnv[opIndex].isDetailVisible();

    se[opIndex].setVisibleWithLabel(visible);
    seFreq[opIndex].setVisibleWithLabel(visible);
    sePreview[opIndex].setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &se[opIndex].label, .component = &se[opIndex] });
        layoutRow({ .rowRect = rect, .label = &seFreq[opIndex].label, .component = &seFreq[opIndex], });

        sePreview[opIndex].setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
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
    // OPZX7S はカーブ編集が標準なので常に有効
    bool isCurveMode = true;
    if (currentGlobalGraphMode == GlobalGraphMode::SsgSw11) {
        ssgSwEnv11g.updateGraph(gGraph, p_curveCore, isCurveMode, 0);
    }
    else if (currentGlobalGraphMode == GlobalGraphMode::SsgSwP11) {
        ssgSwPEnv11g.updateGraph(gGraph, p_curveCore, isCurveMode, 0);
    }
    else {
        ampEnvComponent.updateGraph(gGraph, p_curveCore, isCurveMode, 0);
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

void GuiOpzx7::setupGraph(int opIndex)
{
    addAndMakeVisible(&opGraphs[opIndex]); // グラフを追加

    graphBtnAmp[opIndex].setup({ .parent = *this, .title = "AMP", .isReset = false, .isResized = false });
    graphBtnAmp[opIndex].setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::Amp); };

    graphBtnPitch[opIndex].setup({ .parent = *this, .title = "PIT", .isReset = false, .isResized = false });
    graphBtnPitch[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::Pitch); };

    graphBtnSsg[opIndex].setup({ .parent = *this, .title = "SSG", .isReset = false, .isResized = false });
    graphBtnSsg[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::SsgSw); };

    graphBtnSsg11[opIndex].setup({ .parent = *this, .title = "S11", .isReset = false, .isResized = false });
    graphBtnSsg11[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::SsgSw11); };

    graphBtnSsgP11[opIndex].setup({ .parent = *this, .title = "P11", .isReset = false, .isResized = false });
    graphBtnSsgP11[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::SsgSwP11); };

    auto repaintGraph = [this, opIndex]() {
        if (this->isUpdatingGraph) return; // 既に更新中なら無視

        // 旗は必ず下ろす。途中で抜けたときに立ちっぱなしになると、
        // 以後グラフの更新が全部素通りしてしまうため。
        const juce::ScopedValueSetter<bool> guard(this->isUpdatingGraph, true);

        this->updateOpGraph(opIndex);
        };

    bypass[opIndex].onStateChange = repaintGraph;

    ar[opIndex].onValueChange = repaintGraph;
    d1r[opIndex].onValueChange = repaintGraph;
    d2r[opIndex].onValueChange = repaintGraph;
    d1l[opIndex].onValueChange = repaintGraph;
    rr[opIndex].onValueChange = repaintGraph;
    tl[opIndex].onValueChange = repaintGraph;
    rgAr[opIndex].onValueChange = repaintGraph;
    rgD1r[opIndex].onValueChange = repaintGraph;
    rgD2r[opIndex].onValueChange = repaintGraph;
    rgD1l[opIndex].onValueChange = repaintGraph;
    rgRr[opIndex].onValueChange = repaintGraph;
    rgTl[opIndex].onValueChange = repaintGraph;
    sus[opIndex].onStateChange = repaintGraph;
    xof[opIndex].onStateChange = repaintGraph;
    kor[opIndex].onStateChange = repaintGraph;

    pitchEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv11[opIndex].setupGraph(repaintGraph);
    ssgSwPEnv11[opIndex].setupGraph(repaintGraph);

    graphSeparator[opIndex].setupComponent(*this);
}

void GuiOpzx7::layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect, bool rgMode) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat[opIndex] });

    bool visible = ksCat[opIndex].isDetailVisible();
    Opzx7AdddrKeyScaleMode mode = (Opzx7AdddrKeyScaleMode)(ksMode[opIndex].getSelectedItemIndex());

    ksEn[opIndex].setVisible(visible && !rgMode);
    ksMode[opIndex].setVisibleWithLabel(visible);
    ksrMA7[opIndex].setVisible(visible && mode == Opzx7AdddrKeyScaleMode::MA7);
    kslMA7[opIndex].setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::MA7);
    ksrOPZ[opIndex].setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPZ);
    kslOPZ[opIndex].setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPZ);
    ksBp[opIndex].setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksBp[opIndex].setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksLc[opIndex].setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksRc[opIndex].setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksLd[opIndex].setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksRd[opIndex].setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);
    ksRs[opIndex].setVisibleWithLabel(visible && mode == Opzx7AdddrKeyScaleMode::OPS);

    if (visible) {
        if (!rgMode) {
            layoutRow({ .rowRect = rect, .component = &ksEn[opIndex] });
        }

        layoutRow({ .rowRect = rect, .label = &ksMode[opIndex].label, .component = &ksMode[opIndex] });

        switch (mode) {
        case Opzx7AdddrKeyScaleMode::MA7:
            layoutRow({ .rowRect = rect, .component = &ksrMA7[opIndex] });
            layoutRow({ .rowRect = rect, .label = &kslMA7[opIndex].label, .component = &kslMA7[opIndex] });
            break;
        case Opzx7AdddrKeyScaleMode::OPZ:
            layoutRow({ .rowRect = rect, .label = &ksrOPZ[opIndex].label, .component = &ksrOPZ[opIndex] });
            layoutRow({ .rowRect = rect, .label = &kslOPZ[opIndex].label, .component = &kslOPZ[opIndex] });
            break;
        case Opzx7AdddrKeyScaleMode::OPS:
            layoutRow({ .rowRect = rect, .label = &ksBp[opIndex].label, .component = &ksBp[opIndex] });
            layoutRow({ .rowRect = rect, .label = &ksLc[opIndex].label, .component = &ksLc[opIndex] });
            layoutRow({ .rowRect = rect, .label = &ksRc[opIndex].label, .component = &ksRc[opIndex] });
            layoutRow({ .rowRect = rect, .label = &ksLd[opIndex].label, .component = &ksLd[opIndex] });
            layoutRow({ .rowRect = rect, .label = &ksRd[opIndex].label, .component = &ksRd[opIndex] });
            layoutRow({ .rowRect = rect, .label = &ksRs[opIndex].label, .component = &ksRs[opIndex] });
            break;
        }

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpzx7::layoutOpAmpCat(int opIndex, juce::Rectangle<int>& rect, bool rgMode) {
    layoutRowCategory({ .rowRect = rect, .component = &catAmp[opIndex] });

    bool visible = catAmp[opIndex].isDetailVisible();

    rgEn[opIndex].setVisible(visible);
	rgSeparator[opIndex].setVisible(visible);
    rgAr[opIndex].setVisibleWithLabel(visible && rgMode);
    rgD1r[opIndex].setVisibleWithLabel(visible && rgMode);
    rgD1l[opIndex].setVisibleWithLabel(visible && rgMode);
    rgD2r[opIndex].setVisibleWithLabel(visible && rgMode);
    rgRr[opIndex].setVisibleWithLabel(visible && rgMode);
    rgTl[opIndex].setVisibleWithLabel(visible && rgMode);
    ar[opIndex].setVisibleWithLabel(visible && !rgMode);
    d1r[opIndex].setVisibleWithLabel(visible && !rgMode);
    d1l[opIndex].setVisibleWithLabel(visible && !rgMode);
    d2r[opIndex].setVisibleWithLabel(visible && !rgMode);
    rr[opIndex].setVisibleWithLabel(visible && !rgMode);
    tl[opIndex].setVisibleWithLabel(visible && !rgMode);
	optionalSeparator[opIndex].setVisible(visible);
    sus[opIndex].setVisible(visible);
    xof[opIndex].setVisible(visible);
    kor[opIndex].setVisible(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .component = &rgEn[opIndex] });
		rgSeparator[opIndex].layoutComponent(rect);
        updateRgDisplayAsOp(opIndex, rgMode);
        if (rgMode)
        {
            layoutRow({ .rowRect = rect, .label = &rgAr[opIndex].label, .component = &rgAr[opIndex] });
            layoutRow({ .rowRect = rect, .label = &rgD1r[opIndex].label, .component = &rgD1r[opIndex] });
            layoutRow({ .rowRect = rect, .label = &rgD1l[opIndex].label, .component = &rgD1l[opIndex] });
            layoutRow({ .rowRect = rect, .label = &rgD2r[opIndex].label, .component = &rgD2r[opIndex] });
            layoutRow({ .rowRect = rect, .label = &rgRr[opIndex].label, .component = &rgRr[opIndex] });
            layoutRow({ .rowRect = rect, .label = &rgTl[opIndex].label, .component = &rgTl[opIndex] });
        }
        else
        {
            layoutRow({ .rowRect = rect, .label = &ar[opIndex].label, .component = &ar[opIndex] });
            layoutRow({ .rowRect = rect, .label = &d1r[opIndex].label, .component = &d1r[opIndex] });
            layoutRow({ .rowRect = rect, .label = &d1l[opIndex].label, .component = &d1l[opIndex] });
            layoutRow({ .rowRect = rect, .label = &d2r[opIndex].label, .component = &d2r[opIndex] });
            layoutRow({ .rowRect = rect, .label = &rr[opIndex].label, .component = &rr[opIndex] });
            layoutRow({ .rowRect = rect, .label = &tl[opIndex].label, .component = &tl[opIndex] });
        }

		optionalSeparator[opIndex].layoutComponent(rect);
        layoutRow({ .rowRect = rect, .component = &sus[opIndex] });
        layoutRow({ .rowRect = rect, .component = &xof[opIndex] });
        layoutRow({ .rowRect = rect, .component = &kor[opIndex] });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpzx7::layoutOpWsCat(int opIndex, juce::Rectangle<int>& rect, int selectedWs) {
    layoutRowCategory({ .rowRect = rect, .component = &catWaveShape[opIndex] });

    bool visible = catWaveShape[opIndex].isDetailVisible();

    ws[opIndex].setVisibleWithLabel(visible);
    wsPreview[opIndex].setVisible(visible);
    wsSeparator[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loadPcmBtn[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::pcmIndex);
    pcmFileNameLabel[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::pcmIndex);
    clearPcmBtn[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::pcmIndex);
    pcmOffset[opIndex].setVisibleWithLabel(visible && selectedWs == Opzx7PrValue::pcmIndex);
    pcmRatio[opIndex].setVisibleWithLabel(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loopPointEnable[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loopPointStart[opIndex].setVisibleWithLabel(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loopPointEnd[opIndex].setVisibleWithLabel(visible && selectedWs == Opzx7PrValue::pcmIndex);
    loadWtBtn[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::wtIndex);
    wtFileNameLabel[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::wtIndex);
    clearWtBtn[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::wtIndex);
    loadWt2Btn[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::wt2Index);
    wt2FileNameLabel[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::wt2Index);
    clearWt2Btn[opIndex].setVisible(visible && selectedWs == Opzx7PrValue::wt2Index);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &ws[opIndex].label, .component = &ws[opIndex] });

        wsPreview[opIndex].setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        if (selectedWs == Opzx7PrValue::pcmIndex)
        {
            wsSeparator[opIndex].layoutComponent(rect);
            layoutRowOpzx7File({ .rect = rect, .loadPcmBtn = &loadPcmBtn[opIndex], .pcmFileNameLabel = &pcmFileNameLabel[opIndex], .clearPcmBtn = &clearPcmBtn[opIndex] });
            layoutRow({ .rowRect = rect, .label = &pcmOffset[opIndex].label, .component = &pcmOffset[opIndex] });
            layoutRow({ .rowRect = rect, .label = &pcmRatio[opIndex].label, .component = &pcmRatio[opIndex] });
            layoutRow({ .rowRect = rect, .component = &loopPointEnable[opIndex] });
            layoutRow({ .rowRect = rect, .label = &loopPointStart[opIndex].label, .component = &loopPointStart[opIndex], });
            layoutRow({ .rowRect = rect, .label = &loopPointEnd[opIndex].label, .component = &loopPointEnd[opIndex], });
        } 

        if (selectedWs == Opzx7PrValue::wtIndex)
        {
            wsSeparator[opIndex].layoutComponent(rect);
            layoutRowOpzx7File({ .rect = rect, .loadPcmBtn = &loadWtBtn[opIndex], .pcmFileNameLabel = &wtFileNameLabel[opIndex], .clearPcmBtn = &clearWtBtn[opIndex] });
        }

        if (selectedWs == Opzx7PrValue::wt2Index)
        {
            wsSeparator[opIndex].layoutComponent(rect);
            layoutRowOpzx7File({ .rect = rect, .loadPcmBtn = &loadWt2Btn[opIndex], .pcmFileNameLabel = &wt2FileNameLabel[opIndex], .clearPcmBtn = &clearWt2Btn[opIndex] });
        }

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpzx7::setGraphMode(int opIndex, GraphMode mode)
{
    currentGraphMode[opIndex] = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp[opIndex].setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch[opIndex].setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg[opIndex].setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11[opIndex].setToggleState(mode == GraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11[opIndex].setToggleState(mode == GraphMode::SsgSwP11, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateOpGraph(opIndex);
}

void GuiOpzx7::layoutOpGraph(int opIndex, juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(Opzx7GuiValue::ParamGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator[opIndex].layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(Opzx7GuiValue::ParamGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 5;

    graphBtnAmp[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg11[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsgP11[opIndex].setBounds(btnArea);

    // 残りをグラフエリアにする
    opGraphs[opIndex].setBounds(mainArea);
}

// グラフを再計算して描画
void GuiOpzx7::updateOpGraph(int opIndex)
{
    GraphMode mode = currentGraphMode[opIndex];

    // カーブモードが有効かどうかを判定
    bool isCurveMode = true;

    int posIdx = opIndex; // Position::Op1 = 0, Op2 = 1 ... に合わせる

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
        pitchEnv[opIndex].updateGraph(opGraphs[opIndex], p_curveCore, isCurveMode, posIdx);
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        ssgSwEnv[opIndex].updateGraph(opGraphs[opIndex], p_curveCore, isCurveMode, posIdx);
    }
    // =============================================================
    // SSG SW Env 11
    // =============================================================
    else if (mode == GraphMode::SsgSw11) {
        ssgSwEnv11[opIndex].updateGraph(opGraphs[opIndex], p_curveCore, isCurveMode, posIdx);
    }
    // =============================================================
    // SSG SW PEnv 11
    // =============================================================
    else if (mode == GraphMode::SsgSwP11) {
        ssgSwPEnv11[opIndex].updateGraph(opGraphs[opIndex], p_curveCore, isCurveMode, posIdx);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        bool isBypass = bypass[opIndex].getToggleState();

        opGraphs[opIndex].updateBypass(isBypass);

        if (isBypass) {
            return;
        }

        // -------------------------------------------------------------
        // Helper: 幅の計算 (Amp 用)
        // -------------------------------------------------------------
        auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
            if (maxRate <= 0.0001f) return maxWidth;

            if (rateValue <= 0.0f) return maxWidth;

            float norm = 1.0f - (rateValue / maxRate);

            return maxWidth * norm;
            };

        // -------------------------------------------------------------
        // Helper: カーブ関数を生成する
        // -------------------------------------------------------------
        auto getCurveFunc = [this, isCurveMode](int posIdx, int targetIdx, int prmIdx) {
            return [this, isCurveMode, posIdx, targetIdx, prmIdx](float progress) -> float {
                if (!isCurveMode || p_curveCore == nullptr) return progress;
                return p_curveCore->process(posIdx, targetIdx, prmIdx, progress);
                };
            };

        bool isRg = rgEn[opIndex].getToggleState();
        bool isSus = sus[opIndex].getToggleState();
        bool isXof = xof[opIndex].getToggleState();
        bool isKor = kor[opIndex].getToggleState();

        auto getValue = [isRg](GuiSlider& rgSlider, GuiSlider& realSlider) -> float {
            return isRg ? (float)rgSlider.getValue() : (float)realSlider.getValue();
            };

        auto getMax = [isRg](GuiSlider& rgSlider, GuiSlider& realSlider) -> float {
            float m = isRg ? (float)rgSlider.getMaximum() : (float)realSlider.getMaximum();
            return m > 0.0f ? m : 1.0f;
            };

        float arMax = getMax(rgAr[opIndex], ar[opIndex]);
        float d1rMax = getMax(rgD1r[opIndex], d1r[opIndex]);
        float d2rMax = getMax(rgD2r[opIndex], d2r[opIndex]);
        float d1lMax = getMax(rgD1l[opIndex], d1l[opIndex]);
        float rrMax = getMax(rgRr[opIndex], rr[opIndex]);
        float tlMax = getMax(rgTl[opIndex], tl[opIndex]);

        float arVal = getValue(rgAr[opIndex], ar[opIndex]);
        float d1rVal = getValue(rgD1r[opIndex], d1r[opIndex]);
        float d2rVal = getValue(rgD2r[opIndex], d2r[opIndex]);
        float d1lVal = getValue(rgD1l[opIndex], d1l[opIndex]);
        float rrVal = getValue(rgRr[opIndex], rr[opIndex]);
        float tlVal = getValue(rgTl[opIndex], tl[opIndex]);

        float sl = isRg ? (d1lMax - d1lVal) / d1lMax : d1lVal / d1lMax; // 15=0.0, 0=1.0
        float tlScale = isRg ? 1.0f - (tlVal / tlMax) : tlVal / tlMax; // TL=127で無音

        if (std::isnan(sl) || std::isinf(sl)) sl = 0.0f;
        if (std::isnan(tlScale) || std::isinf(tlScale)) tlScale = 1.0f;

        std::vector<GuiEnvelopeGraph::PhaseDef> phases;
        auto color = juce::Colours::cyan;
        int targetIdx = (int)CurveParams::Target::AmpEnv; // または RegValue

        float currentTotalWidth = 0.0f;

        // 1. Attack
        float attackWidth = rateToWidth(arVal, arMax);
        phases.push_back({
            .widthPx = attackWidth, .startLevel = 0.0f, .endLevel = 1.0f * tlScale, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Ar),
            .phaseLineColor = juce::Colours::red
            });
        currentTotalWidth += attackWidth;

        // 2. Decay 1
        float decayWidth = rateToWidth(d1rVal, d1rMax);
        phases.push_back({
            .widthPx = decayWidth, .startLevel = 1.0f * tlScale, .endLevel = sl * tlScale, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Dr),
            .phaseLineColor = juce::Colours::blue
            });
        currentTotalWidth += decayWidth;

        // 3. Sustain (D2R) の実線部分 (キーオン中)
        float releaseStartLevel = sl;
        float keyOnWidth = 60.0f;
        float sustainTotalWidth = 0.0f;

        if (d2rVal > 0.0f && d2rVal < d2rMax) {
            sustainTotalWidth = rateToWidth(d2rVal, d2rMax, 300.0f);

            // カーブを加味したレベル計算
            float decayRatio = sustainTotalWidth / 300.0f;
            auto curveFunce = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Sr);
            float drCurvedRatio = curveFunce(1.0f);
            float rrCurvedRatio = curveFunce(0.5f);
            releaseStartLevel = sl - (sl * rrCurvedRatio);

            phases.push_back({
                .widthPx = sustainTotalWidth, .startLevel = sl * tlScale, .endLevel = 0.0f, .color = color,
                .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Sr),
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
                .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Rr),
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX,
                .isMax = (rrVal == rrMax)
                });
        }

        opGraphs[opIndex].setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }
}

void GuiOpzx7::layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catOptional[opIndex] });

    bool visible = catOptional[opIndex].isDetailVisible();

    bypass[opIndex].setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &bypass[opIndex] });

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
    copyObj.detune.mul = mulDetune[p].getMul();
    copyObj.detune.mulRatio = mulDetune[p].getMulRatio();
    copyObj.detune.dt = mulDetune[p].getDt1();
    copyObj.detune.dt2 = mulDetune[p].getDt2();
    copyObj.detune.dt3 = mulDetune[p].getDt3();
    copyObj.aAdsr.rgEnable = rgEn[p].getToggleState();
    copyObj.aAdsr.rgAr = rgAr[p].getValue();
    copyObj.aAdsr.rgD1r = rgD1r[p].getValue();
    copyObj.aAdsr.rgD1l = rgD1l[p].getValue();
    copyObj.aAdsr.rgG2r = rgD2r[p].getValue();
    copyObj.aAdsr.rgRr = rgRr[p].getValue();
    copyObj.aAdsr.rgTl = rgTl[p].getValue();
    copyObj.aAdsr.ar = ar[p].getValue();
    copyObj.aAdsr.d1r = d1r[p].getValue();
    copyObj.aAdsr.d1l = d1l[p].getValue();
    copyObj.aAdsr.d2r = d2r[p].getValue();
    copyObj.aAdsr.rr = rr[p].getValue();
    copyObj.aAdsr.tl = tl[p].getValue();
    copyObj.aAdsr.bypass = bypass[p].getToggleState();
    copyObj.aAdsr.kor = kor[p].getToggleState();
    copyObj.aAdsr.sus = sus[p].getToggleState();
    copyObj.aAdsr.xof = xof[p].getToggleState();
    copyObj.aAdsr.mode = (Opzx7AdddrKeyScaleMode)ksMode[p].getSelectedId();
    copyObj.aAdsr.ksrMA7 = ksrMA7[p].getToggleState();
    copyObj.aAdsr.kslMA7 = kslMA7[p].getSelectedId();
    copyObj.aAdsr.ksrOPZ = ksrOPZ[p].getValue();
    copyObj.aAdsr.kslOPZ = kslOPZ[p].getValue();
    copyObj.aAdsr.ksBp = ksBp[p].getValue();
    copyObj.aAdsr.ksLc = ksLc[p].getSelectedId();
    copyObj.aAdsr.ksRc = ksRc[p].getSelectedId();
    copyObj.aAdsr.ksLd = ksLd[p].getValue();
    copyObj.aAdsr.ksRd = ksRd[p].getValue();
    copyObj.aAdsr.ksRs = ksRs[p].getValue();
    copyObj.waveSelect = ws[p].getSelectedId();
    copyObj.pcm.pcmOffset = pcmOffset[p].getValue();
    copyObj.pcm.pcmRatio = pcmRatio[p].getValue();
    copyObj.ssgEg.ssgEg = se[p].getSelectedId();
    copyObj.ssgEg.fmSsgEgFreq = seFreq[p].getValue();
    copyObj.mask.mask = mask[p].getToggleState();

    fix[p].copyParams(copyObj.fix);
    lfo[p].copyParams(copyObj.lfo);
    pitchEnv[p].copyParams(copyObj.pAdsr);
    ssgSwEnv[p].copyParams(copyObj.aSsgSw);
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
    mulDetune[p].setMul(copyObj.detune.mul);
    mulDetune[p].setMulRatio(copyObj.detune.mulRatio);
    mulDetune[p].setDt1(copyObj.detune.dt);
    mulDetune[p].setDt2(copyObj.detune.dt2);
    mulDetune[p].setDt3(copyObj.detune.dt3);
    rgEn[p].setToggleState(copyObj.aAdsr.rgEnable, juce::sendNotification);
    rgAr[p].setValue(copyObj.aAdsr.rgAr, juce::sendNotification);
    rgD1r[p].setValue(copyObj.aAdsr.rgD1r, juce::sendNotification);
    rgD1l[p].setValue(copyObj.aAdsr.rgD1l, juce::sendNotification);
    rgD2r[p].setValue(copyObj.aAdsr.rgG2r, juce::sendNotification);
    rgRr[p].setValue(copyObj.aAdsr.rgRr, juce::sendNotification);
    rgTl[p].setValue(copyObj.aAdsr.rgTl, juce::sendNotification);
    ar[p].setValue(copyObj.aAdsr.ar, juce::sendNotification);
    d1r[p].setValue(copyObj.aAdsr.d1r, juce::sendNotification);
    d1l[p].setValue(copyObj.aAdsr.d1l, juce::sendNotification);
    d2r[p].setValue(copyObj.aAdsr.d2r, juce::sendNotification);
    rr[p].setValue(copyObj.aAdsr.rr, juce::sendNotification);
    tl[p].setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass[p].setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor[p].setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    sus[p].setToggleState(copyObj.aAdsr.sus, juce::sendNotification);
    xof[p].setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ksMode[p].setSelectedId((int)copyObj.aAdsr.mode, juce::sendNotification);
    ksrMA7[p].setToggleState(copyObj.aAdsr.ksrMA7, juce::sendNotification);
    kslMA7[p].setSelectedId(copyObj.aAdsr.kslMA7, juce::sendNotification);
    ksrOPZ[p].setValue(copyObj.aAdsr.ksrOPZ, juce::sendNotification);
    kslOPZ[p].setValue(copyObj.aAdsr.kslOPZ, juce::sendNotification);
    ksBp[p].setValue(copyObj.aAdsr.ksBp, juce::sendNotification);
    ksLc[p].setSelectedId(copyObj.aAdsr.ksLc, juce::sendNotification);
    ksRc[p].setSelectedId(copyObj.aAdsr.ksRc, juce::sendNotification);
    ksLd[p].setValue(copyObj.aAdsr.ksLd, juce::sendNotification);
    ksRd[p].setValue(copyObj.aAdsr.ksRd, juce::sendNotification);
    ksRs[p].setValue(copyObj.aAdsr.ksRs, juce::sendNotification);
    ws[p].setSelectedId(copyObj.waveSelect, juce::sendNotification);
    pcmOffset[p].setValue(copyObj.pcm.pcmOffset, juce::sendNotification);
    pcmRatio[p].setValue(copyObj.pcm.pcmRatio, juce::sendNotification);
    se[p].setSelectedId(copyObj.ssgEg.ssgEg, juce::sendNotification);
    seFreq[p].setValue(copyObj.ssgEg.fmSsgEgFreq, juce::sendNotification);
    mask[p].setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix[p].pasteParams(copyObj.fix);
    lfo[p].pasteParams(copyObj.lfo);
    pitchEnv[p].pasteParams(copyObj.pAdsr);
    ssgSwEnv[p].pasteParams(copyObj.aSsgSw);
}

void GuiOpzx7::importOpLfoParam(int opIndex) {
    lfo[opIndex].importParams();
}

void GuiOpzx7::exportOpLfoParam(int opIndex) {
    lfo[opIndex].exportParams();
}

void GuiOpzx7::importDetuneParam(int opIndex) {
    mulDetune[opIndex].importParams();
}

void GuiOpzx7::exportDetuneParam(int opIndex) {
    mulDetune[opIndex].exportParams();
}

void GuiOpzx7::importPitchEnvParam(int opIndex) {
    pitchEnv[opIndex].importParams();
}

void GuiOpzx7::exportPitchEnvParam(int opIndex) {
    pitchEnv[opIndex].exportParams();
}

void GuiOpzx7::importSsgSwEnvParam(int opIndex) {
    ssgSwEnv[opIndex].importParams();
}

void GuiOpzx7::exportSsgSwEnvParam(int opIndex) {
    ssgSwEnv[opIndex].exportParams();
}

void GuiOpzx7::importQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importQualityParamFile, defaultDir, Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

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
        });
}

void GuiOpzx7::exportQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::QualityParam)), Io::saveGlob(Io::Extension::QualityParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(qualityFormat);
                writeQualityParams(writer);

                writer.writeTo(file);
            }
        });
}

void GuiOpzx7::importOpPcmPlayParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultPcmPlayParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importPcmPlayParamFile, defaultDir, Io::ExtensionGlob::PcmPlayParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

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

                pcmOffset[opIndex].setValue(reader->getFloat("pcmOffset", (float)pcmOffset[opIndex].getValue()), juce::sendNotification);
                pcmRatio[opIndex].setValue(reader->getFloat("pcmRatio", (float)pcmRatio[opIndex].getValue()), juce::sendNotification);
                loopPointEnable[opIndex].setToggleState(reader->getBool("loopPointEnable", loopPointEnable[opIndex].getToggleState()), juce::sendNotification);
                loopPointStart[opIndex].setValue(reader->getFloat("loopPointStart", (float)loopPointStart[opIndex].getValue()), juce::sendNotification);
                loopPointEnd[opIndex].setValue(reader->getFloat("loopPointEnd", (float)loopPointEnd[opIndex].getValue()), juce::sendNotification);
            }
        });

}

void GuiOpzx7::exportOpPcmPlayParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultPcmPlayParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportPcmPlayParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::PcmPlayParam)), Io::saveGlob(Io::Extension::PcmPlayParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultPcmPlayParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(pcmPlayFormat);
                writeOpPcmPlayParams(opIndex, writer);

                writer.writeTo(file);
            }
        });
}

void GuiOpzx7::importSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11[opIndex].importParams();
}

void GuiOpzx7::exportSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11[opIndex].exportParams();
}

void GuiOpzx7::importSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11[opIndex].importParams();
}

void GuiOpzx7::exportSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11[opIndex].exportParams();
}

void GuiOpzx7::importOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp[opIndex].importParams();
}

void GuiOpzx7::exportOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp[opIndex].exportParams();
}

void GuiOpzx7::importOpWtAmpModParam(int opIndex) {
    wtAmpModOp[opIndex].importParams();
}

void GuiOpzx7::exportOpWtAmpModParam(int opIndex) {
    wtAmpModOp[opIndex].exportParams();
}

void GuiOpzx7::importOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp[opIndex].importParams();
}

void GuiOpzx7::exportOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp[opIndex].exportParams();
}

void GuiOpzx7::importOpWtModParam(int opIndex) {
    wtModOp[opIndex].importParams();
}

void GuiOpzx7::exportOpWtModParam(int opIndex) {
    wtModOp[opIndex].exportParams();
}

void GuiOpzx7::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::opzx7sParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

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
        });

}

void GuiOpzx7::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::opzx7sParam)), Io::saveGlob(Io::Extension::opzx7sParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(opzx7Format);
                writeChParams(writer);

                writer.writeTo(file);
            }
        });

}

void GuiOpzx7::importOpChParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::opzx7sOpParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

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
        });

}

void GuiOpzx7::exportOpChParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::opzx7sOpParam)), Io::saveGlob(Io::Extension::opzx7sOpParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(opzx7OpFormat);
                writeOpChFileParams(opIndex, writer);

                writer.writeTo(file);
            }
        });

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
        readOpParams(i, reader.arrayItem(Io::ParamKey::ops, i));
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
    mulDetune[opIndex].readParams(r, "mulDetune");

    // RG Env
    rgEn[opIndex].setToggleState(r.getBool("rgEn", rgEn[opIndex].getToggleState()), juce::sendNotification);
    rgAr[opIndex].setValue(r.getFloat("rgAr", (float)rgAr[opIndex].getValue()), juce::sendNotification);
    rgD1r[opIndex].setValue(r.getFloat("rgD1r", (float)rgD1r[opIndex].getValue()), juce::sendNotification);
    rgD1l[opIndex].setValue(r.getFloat("rgD1l", (float)rgD1l[opIndex].getValue()), juce::sendNotification);
    rgD2r[opIndex].setValue(r.getFloat("rgD2r", (float)rgD2r[opIndex].getValue()), juce::sendNotification);
    rgRr[opIndex].setValue(r.getFloat("rgRr", (float)rgRr[opIndex].getValue()), juce::sendNotification);
    rgTl[opIndex].setValue(r.getFloat("rgTl", (float)rgTl[opIndex].getValue()), juce::sendNotification);

    // Normal Env
    ar[opIndex].setValue(r.getFloat("ar", (float)ar[opIndex].getValue()), juce::sendNotification);
    d1r[opIndex].setValue(r.getFloat("d1r", (float)d1r[opIndex].getValue()), juce::sendNotification);
    d1l[opIndex].setValue(r.getFloat("d1l", (float)d1l[opIndex].getValue()), juce::sendNotification);
    d2r[opIndex].setValue(r.getFloat("d2r", (float)d2r[opIndex].getValue()), juce::sendNotification);
    rr[opIndex].setValue(r.getFloat("rr", (float)rr[opIndex].getValue()), juce::sendNotification);
    tl[opIndex].setValue(r.getFloat("tl", (float)tl[opIndex].getValue()), juce::sendNotification);

    // Key Scale
    ksEn[opIndex].setToggleState(r.getBool("ksEn", ksEn[opIndex].getToggleState()), juce::sendNotification);
    ksMode[opIndex].setSelectedId(r.getInt("ksMode", ksMode[opIndex].getSelectedId()), juce::sendNotification);
    ksrMA7[opIndex].setToggleState(r.getBool("ksrMA7", ksrMA7[opIndex].getToggleState()), juce::sendNotification);
    kslMA7[opIndex].setSelectedId(r.getInt("kslMA7", kslMA7[opIndex].getSelectedId()), juce::sendNotification);
    ksrOPZ[opIndex].setValue(r.getFloat("ksrOPZ", (float)ksrOPZ[opIndex].getValue()), juce::sendNotification);
    kslOPZ[opIndex].setValue(r.getFloat("kslOPZ", (float)kslOPZ[opIndex].getValue()), juce::sendNotification);
    ksBp[opIndex].setValue(r.getFloat("ksBp", (float)ksBp[opIndex].getValue()), juce::sendNotification);
    ksLc[opIndex].setSelectedId(r.getInt("ksLc", ksLc[opIndex].getSelectedId()), juce::sendNotification);
    ksRc[opIndex].setSelectedId(r.getInt("ksRc", ksRc[opIndex].getSelectedId()), juce::sendNotification);
    ksLd[opIndex].setValue(r.getFloat("ksLd", (float)ksLd[opIndex].getValue()), juce::sendNotification);
    ksRd[opIndex].setValue(r.getFloat("ksRd", (float)ksRd[opIndex].getValue()), juce::sendNotification);
    ksRs[opIndex].setValue(r.getFloat("ksRs", (float)ksRs[opIndex].getValue()), juce::sendNotification);

    // Optional
    bypass[opIndex].setToggleState(r.getBool("bypass", bypass[opIndex].getToggleState()), juce::sendNotification);
    sus[opIndex].setToggleState(r.getBool("sus", sus[opIndex].getToggleState()), juce::sendNotification);
    xof[opIndex].setToggleState(r.getBool("xof", xof[opIndex].getToggleState()), juce::sendNotification);
    kor[opIndex].setToggleState(r.getBool("kor", kor[opIndex].getToggleState()), juce::sendNotification);
    mask[opIndex].setToggleState(r.getBool("mask", mask[opIndex].getToggleState()), juce::sendNotification);

    // Wave Shape
    ws[opIndex].setSelectedId(r.getInt("ws", ws[opIndex].getSelectedId()), juce::sendNotification);

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
    int wsIdx = ws[opIndex].getSelectedItemIndex();

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
    pcmOffset[opIndex].setValue(r.getFloat("pcmOffset", (float)pcmOffset[opIndex].getValue()), juce::sendNotification);
    pcmRatio[opIndex].setValue(r.getFloat("pcmRatio", (float)pcmRatio[opIndex].getValue()), juce::sendNotification);
    loopPointEnable[opIndex].setToggleState(r.getBool("loopPointEnable", loopPointEnable[opIndex].getToggleState()), juce::sendNotification);
    loopPointStart[opIndex].setValue(r.getFloat("loopPointStart", (float)loopPointStart[opIndex].getValue()), juce::sendNotification);
    loopPointEnd[opIndex].setValue(r.getFloat("loopPointEnd", (float)loopPointEnd[opIndex].getValue()), juce::sendNotification);

    // SSG Env
    se[opIndex].setSelectedId(r.getInt("se", se[opIndex].getSelectedId()), juce::sendNotification);
    seFreq[opIndex].setValue(r.getFloat("seFreq", (float)seFreq[opIndex].getValue()), juce::sendNotification);

    // Components
    fix[opIndex].readParams(r, "fix");
    lfo[opIndex].readParams(r, "lfo");
    pitchEnv[opIndex].readParams(r, "pitchEnv");
    ssgSwEnv[opIndex].readParams(r, "ssgSwEnv");
    ssgSwEnv11[opIndex].readParams(r, "ssgSwEnv11");
    ssgSwPEnv11[opIndex].readParams(r, "ssgSwPEnv11");
    ssgHwPEnvOp[opIndex].readParams(r, "ssgHwPEnv");
    wtAmpModOp[opIndex].readParams(r, "wtAmpMod");
    ssgHwEnvOp[opIndex].readParams(r, "ssgHwEnv");
    wtModOp[opIndex].readParams(r, "wtMod");
}

void GuiOpzx7::writeOpParams(int opIndex, Io::ParamWriter& w) {
    // Detune / Multiplier
    mulDetune[opIndex].writeParams(w, "mulDetune");

    // RG Env
    w.set("rgEn", rgEn[opIndex].getToggleState());
    w.set("rgAr", (float)rgAr[opIndex].getValue());
    w.set("rgD1r", (float)rgD1r[opIndex].getValue());
    w.set("rgD1l", (float)rgD1l[opIndex].getValue());
    w.set("rgD2r", (float)rgD2r[opIndex].getValue());
    w.set("rgRr", (float)rgRr[opIndex].getValue());
    w.set("rgTl", (float)rgTl[opIndex].getValue());

    // Normal Env
    w.set("ar", (float)ar[opIndex].getValue());
    w.set("d1r", (float)d1r[opIndex].getValue());
    w.set("d1l", (float)d1l[opIndex].getValue());
    w.set("d2r", (float)d2r[opIndex].getValue());
    w.set("rr", (float)rr[opIndex].getValue());
    w.set("tl", (float)tl[opIndex].getValue());

    // Key Scale
    w.set("ksEn", ksEn[opIndex].getToggleState());
    w.set("ksMode", ksMode[opIndex].getSelectedId());
    w.set("ksrMA7", ksrMA7[opIndex].getToggleState());
    w.set("kslMA7", kslMA7[opIndex].getSelectedId());
    w.set("ksrOPZ", (float)ksrOPZ[opIndex].getValue());
    w.set("kslOPZ", (float)kslOPZ[opIndex].getValue());
    w.set("ksBp", (float)ksBp[opIndex].getValue());
    w.set("ksLc", ksLc[opIndex].getSelectedId());
    w.set("ksRc", ksRc[opIndex].getSelectedId());
    w.set("ksLd", (float)ksLd[opIndex].getValue());
    w.set("ksRd", (float)ksRd[opIndex].getValue());
    w.set("ksRs", (float)ksRs[opIndex].getValue());

    // Optional
    w.set("bypass", bypass[opIndex].getToggleState());
    w.set("sus", sus[opIndex].getToggleState());
    w.set("xof", xof[opIndex].getToggleState());
    w.set("kor", kor[opIndex].getToggleState());
    w.set("mask", mask[opIndex].getToggleState());

    // Wave Shape
    w.set("ws", ws[opIndex].getSelectedId());

    // 種類ごとに別の名前で持つので、切り替えて保存し直しても互いを壊さない
    w.set("pcmFile", ctx.audioProcessor.opzx7PcmFilePaths[opIndex]);
    w.set("wtFile", ctx.audioProcessor.opzx7WtFilePaths[opIndex]);
    w.set("wt2File", ctx.audioProcessor.opzx7Wt2FilePaths[opIndex]);

    // PCM Play / Loop Point
    w.set("pcmOffset", (float)pcmOffset[opIndex].getValue());
    w.set("pcmRatio", (float)pcmRatio[opIndex].getValue());
    w.set("loopPointEnable", loopPointEnable[opIndex].getToggleState());
    w.set("loopPointStart", (float)loopPointStart[opIndex].getValue());
    w.set("loopPointEnd", (float)loopPointEnd[opIndex].getValue());

    // SSG Env
    w.set("se", se[opIndex].getSelectedId());
    w.set("seFreq", (float)seFreq[opIndex].getValue());

    // Components
    fix[opIndex].writeParams(w, "fix");
    lfo[opIndex].writeParams(w, "lfo");
    pitchEnv[opIndex].writeParams(w, "pitchEnv");
    ssgSwEnv[opIndex].writeParams(w, "ssgSwEnv");
    ssgSwEnv11[opIndex].writeParams(w, "ssgSwEnv11");
    ssgSwPEnv11[opIndex].writeParams(w, "ssgSwPEnv11");
    ssgHwPEnvOp[opIndex].writeParams(w, "ssgHwPEnv");
    wtAmpModOp[opIndex].writeParams(w, "wtAmpMod");
    ssgHwEnvOp[opIndex].writeParams(w, "ssgHwEnv");
    wtModOp[opIndex].writeParams(w, "wtMod");
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
    mulDetune[opIndex].setImportingParams(lines, index);

    // RG Env
    rgEn[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    rgAr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD1r[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD1l[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD2r[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgRr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgTl[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Normal Env
    ar[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    d1r[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    d1l[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    d2r[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    tl[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Key Scale
    ksEn[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    ksMode[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ksrMA7[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    kslMA7[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ksrOPZ[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    kslOPZ[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    ksBp[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    ksLc[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ksRc[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ksLd[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    ksRd[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    ksRs[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Optional
    bypass[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    sus[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    xof[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    kor[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    mask[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

    // Wave Shape
    ws[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    ctx.audioProcessor.unloadOpzx7PcmFile(opIndex);
    ctx.audioProcessor.unloadOpzx7WtFile(opIndex);
    ctx.audioProcessor.unloadOpzx7Wt2File(opIndex);

    int wsIdx = ws[opIndex].getSelectedItemIndex();

    if (wsIdx == Opzx7PrValue::pcmIndex) {
        pcmFileNameLabel[opIndex].setText(lines[index++], juce::dontSendNotification);

        if (pcmFileNameLabel[opIndex].getText().isNotEmpty()) {
            ctx.audioProcessor.loadOpzx7PcmFile(opIndex, pcmFileNameLabel[opIndex].getText());
        }
    }
    else if (wsIdx == Opzx7PrValue::wtIndex) {
        wtFileNameLabel[opIndex].setText(lines[index++], juce::dontSendNotification);

        if (wtFileNameLabel[opIndex].getText().isNotEmpty()) {
            ctx.audioProcessor.loadOpzx7WtFile(opIndex, wtFileNameLabel[opIndex].getText());
        }

    }
    else if (wsIdx == Opzx7PrValue::wt2Index) {
        wt2FileNameLabel[opIndex].setText(lines[index++], juce::dontSendNotification);

        if (wt2FileNameLabel[opIndex].getText().isNotEmpty()) {
            ctx.audioProcessor.loadOpzx7Wt2File(opIndex, wt2FileNameLabel[opIndex].getText());
        }
    }

    // PCM Play / Loop Point
    pcmOffset[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    pcmRatio[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    loopPointEnable[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    loopPointStart[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    loopPointEnd[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // SSG Env
    se[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    seFreq[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Components
    fix[opIndex].setImportingParams(lines, index);
    lfo[opIndex].setImportingParams(lines, index);
    pitchEnv[opIndex].setImportingParams(lines, index);
    ssgSwEnv[opIndex].setImportingParams(lines, index);
    ssgSwEnv11[opIndex].setImportingParams(lines, index);
    ssgSwPEnv11[opIndex].setImportingParams(lines, index);
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

	pcmOffset[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
	pcmRatio[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
	loopPointEnable[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	loopPointStart[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
	loopPointEnd[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpzx7::writeOpPcmPlayParams(int opIndex, Io::ParamWriter& writer) {
	writer.set("pcmOffset", (float)pcmOffset[opIndex].getValue());
	writer.set("pcmRatio", (float)pcmRatio[opIndex].getValue());
	writer.set("loopPointEnable", loopPointEnable[opIndex].getToggleState());
	writer.set("loopPointStart", (float)loopPointStart[opIndex].getValue());
	writer.set("loopPointEnd", (float)loopPointEnd[opIndex].getValue());

	
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
