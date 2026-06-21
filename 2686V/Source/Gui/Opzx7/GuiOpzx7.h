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
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"

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
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)"}},       // 00: <OPX-00>
        {{"([M->2])", "([M:FB1->3])", "([M->4])", "([C])", "(--)", "(--)"}},      // 01: <OPX-01>
        {{"([M:FB->3])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)"}},       // 02: <OPX-02>
        {{"([M:FB->4])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)"}},       // 03: <OPX-03>
        {{"([M:FB->2])", "([M->4])", "([M->4])", "([C])", "(--)", "(--)"}},       // 04: <OPX-04>
        {{"([M:FB->2])", "([M:FB1->4])", "([M->4])", "([C])", "(--)", "(--)"}},   // 05: <OPX-05>
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])", "(--)", "(--)"}},          // 06: <OPX-06>
        {{"([M->2])", "([C:FB1])", "([M->4])", "([C])", "(--)", "(--)"}},         // 07: <OPX-07>
        {{"([C:FB])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)"}},          // 08: <OPX-08>
        {{"([C:FB])", "([M->4])", "([M->4])", "([C])", "(--)", "(--)"}},          // 09: <OPX-09>
        {{"([M:FB->2])", "([C])", "([C])", "([C])", "(--)", "(--)"}},             // 10: <OPX-10>
        {{"([M->2])", "([C:FB1])", "([C])", "([C])", "(--)", "(--)"}},            // 11: <OPX-11>
        {{"([M:FB->2/3/4])", "([C])", "([C])", "([C])", "(--)", "(--)"}},         // 12: <OPX-12>
        {{"([C:FB])", "([M->3])", "([C])", "([C])", "(--)", "(--)"}},             // 13: <OPX-13>
        {{"([C:FB][M:FB->2])", "([C])", "([M->4])", "([C])", "(--)", "(--)"}},    // 14: <OPX-14>
        {{"([C:FB])", "([C])", "([C])", "([C])", "(--)", "(--)"}},                // 15: <OPX-15>
        {{"([M:FB->2])", "([M->3])", "([C])", "(--)", "(--)", "(--)"}},           // 16: <OPX-16>
        {{"([M->2])", "([M:FB1->3])", "([C])", "(--)", "(--)", "(--)"}},          // 17: <OPX-17>
        {{"([M:FB->3])", "([M->3])", "([C])", "(--)", "(--)", "(--)"}},           // 18: <OPX-18>
        {{"([C:FB])", "([M->3])", "([C])", "(--)", "(--)", "(--)"}},              // 19: <OPX-19>
        {{"([M:FB->2])", "([C])", "([C])", "(--)", "(--)", "(--)"}},              // 20: <OPX-20>
        {{"([M->2])", "([C:FB1])", "([C])", "(--)", "(--)", "(--)"}},             // 21: <OPX-21>
        {{"([C:FB])", "([C])", "([C])", "(--)", "(--)", "(--)"}},                 // 22: <OPX-22>
        {{"([C:FB][M:FB->2])", "([C])", "([C])", "(--)", "(--)", "(--)"}},        // 23: <OPX-23>
        {{"([M:FB->2])", "([C])", "(--)", "(--)", "(--)", "(--)"}},               // 24: <OPX-24>
        {{"([M->2])", "([C:FB1])", "(--)", "(--)", "(--)", "(--)"}},              // 25: <OPX-25>
        {{"([C:FB])", "([C])", "(--)", "(--)", "(--)", "(--)"}},                  // 26: <OPX-26>
        {{"([C:FB][M->2])", "([C])", "(--)", "(--)", "(--)", "(--)"}},            // 27: <OPX-27>
        {{"([M:FB->2])", "([C])", "(--)", "(--)", "(--)", "(--)"}},               // 28: <MA3-00>
        {{"([C:FB])", "([C])", "(--)", "(--)", "(--)", "(--)"}},                  // 29: <MA3-01>
        {{"([C:FB])", "([C])", "([C:FB])", "([C])", "(--)", "(--)"}},             // 30: <MA3-02>
        {{"([M:FB->4])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)"}},       // 31: <MA3-03>
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)"}},       // 32: <MA3-04>
        {{"([M:FB->2])", "([C])", "([M:FB->4])", "([C])", "(--)", "(--)"}},       // 33: <MA3-05>
        {{"([C:FB])", "([M->3])", "([M->4])", "([C])", "(--)", "(--)"}},          // 34: <MA3-06>
        {{"([C:FB])", "([M->3])", "([C])", "([C])", "(--)", "(--)"}},             // 35: <MA3-07>
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])", "([M->6])", "([C])"}},  // 36: <OPS-00>
        {{"([M->2])", "([M->3])", "([M->4])", "([C])", "([M:FB->6])", "([C])"}},  // 37: <OPS-01>
        {{"([M:FB->2])", "([M->3])", "([C])", "([M->5])", "([M->6])", "([C])"}},  // 38: <OPS-02>
        {{"([M->2])", "([M->3])", "([C:FB1])", "([M->5])", "([M->6])", "([C])"}}, // 39: <OPS-03>
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])", "([M->6])", "([C])"}},     // 40: <OPS-04>
        {{"([M->2])", "([C:FB1])", "([M->4])", "([C])", "([M->6])", "([C])"}},    // 41: <OPS-05>
        {{"([M:FB->2])", "([M->4])", "([M->4])", "([C])", "([M->6])", "([C])"}},  // 42: <OPS-06>
        {{"([M->2])", "([M->4])", "([M:FB->4])", "([C])", "([M->6])", "([C])"}},  // 43: <OPS-07>
        {{"([M->2])", "([M->4])", "([M->4])", "([C])", "([M:FB->6])", "([C])"}},  // 44: <OPS-08>
        {{"([M->3])", "([M->3])", "([C])", "([M:FB->5])", "([M->6])", "([C])"}},  // 45: <OPS-09>
        {{"([M:FB->3])", "([M->3])", "([C])", "([M->5])", "([M->6])", "([C])"}},  // 46: <OPS-10>
        {{"([M->4])", "([M->4])", "([M->4])", "([C])", "([M:FB->6])", "([C])"}},  // 47: <OPS-11>
        {{"([M:FB->4])", "([M->4])", "([M->4])", "([C])", "([M->6])", "([C])"}},  // 48: <OPS-12>
        {{"([M:FB->3])", "([M->3])", "([M->4])", "([C])", "([M->6])", "([C])"}},  // 49: <OPS-13>
        {{"([M->3])", "([M->3])", "([M->4])", "([C])", "([M:FB->6])", "([C])"}},  // 50: <OPS-14>
        {{"([M:FB->2])", "([C])", "([M->4])", "([M->6])", "([M->6])", "([C])"}},  // 51: <OPS-15>
        {{"([M->2])", "([C])", "([M->4])", "([M->6])", "([M:FB->6])", "([C])"}},  // 52: <OPS-16>
        {{"([M->2])", "([M->3])", "([C])", "([M:FB->6])", "([M->6])", "([C])"}},  // 53: <OPS-17>
        {{"([M:FB->2/3])", "([C])", "([C])", "([M->5])", "([M->6])", "([C])"}},   // 54: <OPS-18>
        {{"([M->2])", "([M->3])", "([C])", "([M:FB->5/6])", "([C])", "([C])"}},   // 55: <OPS-19>
        {{"([M->2/3])", "([C])", "([C])", "([M:FB->5/6])", "([C])", "([C])"}},    // 56: <OPS-20>
        {{"([M:FB->2/3/4])", "([C])", "([C])", "([C])", "([M->6])", "([C])"}},    // 57: <OPS-21>
        {{"([M:FB->2/3])", "([C])", "([C])", "([M->5])", "([C])", "([C])"}},      // 58: <OPS-22>
        {{"([M:FB->2/3/4/5/6])", "([C])", "([C])", "([C])", "([C])", "([C])"}},   // 59: <OPS-23>
        {{"([M:FB->2/3])", "([C])", "([C])", "([C])", "([C])", "([C])"}},         // 60: <OPS-24>
        {{"([M:FB->3])", "([M->3])", "([C])", "([M->5])", "([C])", "([C])"}},     // 61: <OPS-25>
        {{"([M->3])", "([M->3])", "([C])", "([M:FB->5])", "([C])", "([C])"}},     // 62: <OPS-26>
        {{"([C])", "([M:FB->3])", "([M->4])", "([C])", "([M->6])", "([C])"}},     // 63: <OPS-27>
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])", "([C])", "([C])"}},        // 64: <OPS-28>
        {{"([C])", "([M:FB->3])", "([M->4])", "([C])", "([C])", "([C])"}},        // 65: <OPS-29>
        {{"([M:FB->2])", "([C])", "([C])", "([C])", "([C])", "([C])"}},           // 66: <OPS-30>
        {{"([C:FB])", "([C])", "([C])", "([C])", "([C])", "([C])"}},              // 67: <OPS-31>
        {{"([C:FB])", "(--)", "(--)", "(--)", "(--)", "(--)"}},                   // 68: <OPZX7S-000>
        {{"([C:FB])", "([C:FB])", "(--)", "(--)", "(--)", "(--)"}},               // 69: <OPZX7S-001>
        {{"([M->2])", "([C:FB1])", "([M->4])", "([C:FB3])", "(--)", "(--)"}},     // 70: <OPZX7S-002>
        {{"([C:FB])", "([C:FB])", "([C:FB])", "([C:FB])", "(--)", "(--)"}},       // 71: <OPZX7S-003>
    }};

    // アルゴリズムごとに利用可能なオペレーターを制限
    static inline const std::array<std::array<bool, Opzx7PrValue::ops>, Opzx7PrValue::algorithms> opEnableOnAlg = { {
    {{true, true, true, true, false, false}},    // 00: <OPX-00>
    {{true, true, true, true, false, false}},    // 01: <OPX-01>
    {{true, true, true, true, false, false}},    // 02: <OPX-02>
    {{true, true, true, true, false, false}},    // 03: <OPX-03>
    {{true, true, true, true, false, false}},    // 04: <OPX-04>
    {{true, true, true, true, false, false}},    // 05: <OPX-05>
    {{true, true, true, true, false, false}},    // 06: <OPX-06>
    {{true, true, true, true, false, false}},    // 07: <OPX-07>
    {{true, true, true, true, false, false}},    // 08: <OPX-08>
    {{true, true, true, true, false, false}},    // 09: <OPX-09>
    {{true, true, true, true, false, false}},    // 10: <OPX-10>
    {{true, true, true, true, false, false}},    // 11: <OPX-11>
    {{true, true, true, true, false, false}},    // 12: <OPX-12>
    {{true, true, true, true, false, false}},    // 13: <OPX-13>
    {{true, true, true, true, false, false}},    // 14: <OPX-14>
    {{true, true, true, true, false, false}},    // 15: <OPX-15>
    {{true, true, true, false, false, false}},   // 16: <OPX-16>
    {{true, true, true, false, false, false}},   // 17: <OPX-17>
    {{true, true, true, false, false, false}},   // 18: <OPX-18>
    {{true, true, true, false, false, false}},   // 19: <OPX-19>
    {{true, true, true, false, false, false}},   // 20: <OPX-20>
    {{true, true, true, false, false, false}},   // 21: <OPX-21>
    {{true, true, true, false, false, false}},   // 22: <OPX-22>
    {{true, true, true, false, false, false}},   // 23: <OPX-23>
    {{true, true, false, false, false, false}},  // 24: <OPX-24>
    {{true, true, false, false, false, false}},  // 25: <OPX-25>
    {{true, true, false, false, false, false}},  // 26: <OPX-26>
    {{true, true, false, false, false, false}},  // 27: <OPX-27>
    {{true, true, false, false, false, false}},  // 28: <MA3-00>
    {{true, true, false, false, false, false}},  // 29: <MA3-01>
    {{true, true, true, true, false, false}},    // 30: <MA3-02>
    {{true, true, true, true, false, false}},    // 31: <MA3-03>
    {{true, true, true, true, false, false}},    // 32: <MA3-04>
    {{true, true, true, true, false, false}},    // 33: <MA3-05>
    {{true, true, true, true, false, false}},    // 34: <MA3-06>
    {{true, true, true, true, false, false}},    // 35: <MA3-07>
    {{true, true, true, true, true, true}},      // 36: <OPS-00>
    {{true, true, true, true, true, true}},      // 37: <OPS-01>
    {{true, true, true, true, true, true}},      // 38: <OPS-02>
    {{true, true, true, true, true, true}},      // 39: <OPS-03>
    {{true, true, true, true, true, true}},      // 40: <OPS-04>
    {{true, true, true, true, true, true}},      // 41: <OPS-05>
    {{true, true, true, true, true, true}},      // 42: <OPS-06>
    {{true, true, true, true, true, true}},      // 43: <OPS-07>
    {{true, true, true, true, true, true}},      // 44: <OPS-08>
    {{true, true, true, true, true, true}},      // 45: <OPS-09>
    {{true, true, true, true, true, true}},      // 46: <OPS-10>
    {{true, true, true, true, true, true}},      // 47: <OPS-11>
    {{true, true, true, true, true, true}},      // 48: <OPS-12>
    {{true, true, true, true, true, true}},      // 49: <OPS-13>
    {{true, true, true, true, true, true}},      // 50: <OPS-14>
    {{true, true, true, true, true, true}},      // 51: <OPS-15>
    {{true, true, true, true, true, true}},      // 52: <OPS-16>
    {{true, true, true, true, true, true}},      // 53: <OPS-17>
    {{true, true, true, true, true, true}},      // 54: <OPS-18>
    {{true, true, true, true, true, true}},      // 55: <OPS-19>
    {{true, true, true, true, true, true}},      // 56: <OPS-20>
    {{true, true, true, true, true, true}},      // 57: <OPS-21>
    {{true, true, true, true, true, true}},      // 58: <OPS-22>
    {{true, true, true, true, true, true}},      // 59: <OPS-23>
    {{true, true, true, true, true, true}},      // 60: <OPS-24>
    {{true, true, true, true, true, true}},      // 61: <OPS-25>
    {{true, true, true, true, true, true}},      // 62: <OPS-26>
    {{true, true, true, true, true, true}},      // 63: <OPS-27>
    {{true, true, true, true, true, true}},      // 64: <OPS-28>
    {{true, true, true, true, true, true}},      // 65: <OPS-29>
    {{true, true, true, true, true, true}},      // 66: <OPS-30>
    {{true, true, true, true, true, true}},      // 67: <OPS-31>
    {{true, false, false, false, false, false}}, // 68: <OPZX7S-000>
    {{true, true, false, false, false, false}},  // 69: <OPZX7S-001>
    {{true, true, true, true, false, false}},    // 70: <OPZX7S-002>
    {{true, true, true, true, false, false}},    // 71: <OPZX7S-003>
} };

    GuiScrollGroup mainGroup;

    GuiSlider levelSlider;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel algFbCat;

    // Global
    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

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

    // Preset Name Label
    GuiLabel presetNameLabel;
    GuiSeparator presetNameSeparator;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    GuiSeparator uSep001;
    GuiTextButton copyOpParamBtn;
    GuiSlider copyOpFromSlider;
    GuiSlider copyOpToSlider;
    GuiSeparator uSep002;
    GuiTextButton importLfoParamButton;
    GuiTextButton exportLfoParamButton;
    GuiTextButton importDetuneParamButton;
    GuiTextButton exportDetuneParamButton;
    GuiTextButton importPitchEnvParamButton;
    GuiTextButton exportPitchEnvParamButton;
    GuiTextButton importSsgSwEnvParamButton;
    GuiTextButton exportSsgSwEnvParamButton;
    GuiSlider targerOpSlider;
    GuiSeparator uSep003;
    GuiTextButton importUnisonParamButton;
    GuiTextButton exportUnisonParamButton;
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::ImageComponent algImageComp;
    std::array<juce::Image, Opzx7PrValue::algorithms> algImages;

    std::array<GuiScrollGroup, Opzx7PrValue::ops> opGroups;
    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<GuiComboBox, Opzx7PrValue::ops> mul;
    std::array<GuiSlider, Opzx7PrValue::ops> mulRatio;
    std::array<GuiSlider, Opzx7PrValue::ops> dt1;
    std::array<GuiSlider, Opzx7PrValue::ops> dt2;
    std::array<GuiSlider, Opzx7PrValue::ops> dt3;
    std::array<GuiComponentPitchButtons, Opzx7PrValue::ops> dt3Btns;
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
    std::array<GuiTextButton, Opzx7PrValue::ops> loadPcmBtn;
    std::array<GuiTextButton, Opzx7PrValue::ops> clearPcmBtn;
    std::array<GuiLabel, Opzx7PrValue::ops> pcmFileNameLabel;
    std::array<GuiSlider, Opzx7PrValue::ops> pcmOffset;
    std::array<GuiSlider, Opzx7PrValue::ops> pcmRatio;
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

    std::array<GuiCategoryLabel, Opzx7PrValue::ops> catMask;
    std::array<GuiToggleButton, Opzx7PrValue::ops> mask; // Mask
    std::array<GuiSeparator, Opzx7PrValue::ops> mmlSeparator;
    std::array<GuiMmlButton, Opzx7PrValue::ops> mml;

    std::array<GuiToggleButton, Opzx7PrValue::ops> rgEn;
    std::array<GuiSlider, Opzx7PrValue::ops> rgAr;
    std::array<GuiSlider, Opzx7PrValue::ops> rgD1r;
    std::array<GuiSlider, Opzx7PrValue::ops> rgD2r;
    std::array<GuiSlider, Opzx7PrValue::ops> rgD1l;
    std::array<GuiSlider, Opzx7PrValue::ops> rgRr;
    std::array<GuiSlider, Opzx7PrValue::ops> rgTl;

    std::array<GuiToggleButton, Opzx7PrValue::ops> sus; // Sus
    std::array<GuiToggleButton, Opzx7PrValue::ops> xof; // Xof
    std::array<GuiToggleButton, Opzx7PrValue::ops> kor;

    void applyMmlString(const juce::String& mml, int opIndex);

    std::array<GuiEnvelopeGraph, Opzx7PrValue::ops> opGraphs;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, Opzx7PrValue::ops> graphBtnSsg;
    std::array<GuiSeparator, Opzx7PrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw };
    std::array<GraphMode, Opzx7PrValue::ops> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
	GuiOpzx7(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        qualityCat(context),
        algFbCat(context),
        levelSlider(context),
        algSelector(context),
        feedbackSlider(context),
        bitSelector(context),
        rateSelector(context),
        panCat(context),
        panpotEnableToggle(context),
        panpotSlider(context),
        panToLBtn(context),
        panToCBtn(context),
        panToRBtn(context),
        glLfo(context),
		unisonComponent(context),
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        copyOpParamBtn(context),
        copyOpFromSlider(context),
        copyOpToSlider(context),
        uSep002(context),
        importLfoParamButton(context),
        exportLfoParamButton(context),
        importDetuneParamButton(context),
        exportDetuneParamButton(context),
        importPitchEnvParamButton(context),
        exportPitchEnvParamButton(context),
        importSsgSwEnvParamButton(context),
        exportSsgSwEnvParamButton(context),
        targerOpSlider(context),
        uSep003(context),
        importUnisonParamButton(context),
        exportUnisonParamButton(context),
        opGroups{ GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context) },
        mul{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        mulRatio{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt1{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt2{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt3{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt3Btns{ GuiComponentPitchButtons(context), GuiComponentPitchButtons(context), GuiComponentPitchButtons(context), GuiComponentPitchButtons(context), GuiComponentPitchButtons(context), GuiComponentPitchButtons(context) },
        tl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ar{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        d1r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        d1l{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        d2r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ksCat{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        ksEn{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        ksMode{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        ksrMA7{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        kslMA7{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        ksrOPZ{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        kslOPZ{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ksBp{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ksLc{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        ksRc{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        ksLd{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ksRd{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ksRs{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        catOptional{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        bypass{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        fix{ GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context) },
        catWaveShape{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        ws{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        loadPcmBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        clearPcmBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        pcmFileNameLabel{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
        pcmOffset{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pcmRatio{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        loadWtBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        clearWtBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        wtFileNameLabel{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
        loadWt2Btn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        clearWt2Btn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        wt2FileNameLabel{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
        catSsgEnv{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        se{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        seFreq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) , GuiSlider(context), GuiSlider(context) },
        lfo{ GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context), GuiComponentLfoOpzx7(context) },
        pitchEnv{ GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context) },
        ssgSwEnv{ GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        mmlSeparator{ GuiSeparator(context), GuiSeparator(context), GuiSeparator(context), GuiSeparator(context), GuiSeparator(context), GuiSeparator(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) },
        rgEn{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        rgAr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgD1r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgD2r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgD1l{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgRr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgTl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        sus{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        xof{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        kor{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        midiComponent(context),
        presetNameLabel(context),
        presetNameSeparator(context),
        graphBtnAmp{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnPitch{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnSsg{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphSeparator{ GuiSeparator(context), GuiSeparator(context), GuiSeparator(context), GuiSeparator(context), GuiSeparator(context), GuiSeparator(context) }
    {
        currentGraphMode.fill(GraphMode::Amp); // 初期状態はすべてAmp
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

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
    void updateOpEnable(int idx, bool enable);
	void updateOnWsChange(int idx);
    void updateAlgorithmDisplay();
    void updateRgDisplayAsOp(int idx, bool rgMode);
    void updatePresetName(const juce::String& presetName);
    bool keyPressed(const juce::KeyPress& key) override;
    void copyFmParamsToString();
    void copyFmParamsToObject();
    void pasteFmParamsFromObject();
    void initParams();
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanpotCat(juce::Rectangle<int>& rect);
    void layoutOpSsgEnvCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect, bool rgMode);
    void setupGraph(int opIndex);
    void layoutOpGraph(int opIndex, juce::Rectangle<int>& rect);
    void setLevel(float level);
    void copyParams(CopyOpzx7& copyObj);
    void copyOpParams(int p, CopyOpzx7Op& copyObj);
    void pasteParams(CopyOpzx7& copyObj);
    void pasteOpParams(int p, CopyOpzx7Op& copyObj);
    void importLfoParam(int opIndex);
    void exportLfoParam(int opIndex);
    void importPitchEnvParam(int opIndex);
    void exportPitchEnvParam(int opIndex);
    void importSsgSwEnvParam(int opIndex);
    void exportSsgSwEnvParam(int opIndex);
    void importDetuneParam(int opIndex);
    void exportDetuneParam(int opIndex);
    void importUnisonParam();
    void exportUnisonParam();
};
