#pragma once

#include <JuceHeader.h>

#include "../gui/GuiStructs.h"

// Defined globally as it's used in structs
static const juce::String FontFamily = "Times New Roman";
static inline const float LogoFontSize = 128.0f;

static inline const int WindowWidth = 1280;
static inline const int WindowHeight = 660;
static inline const int PreviewExtraWidth = 288;
static inline const int PreviewDrawSize = 280;

#if defined(BUILD_AS_FX_PLUGIN)
static inline const int TabNumber = 3;
#else
static inline const int TabNumber = 13;

// 鍵盤UIの高さ
const inline int KeyboardHeight = 60;
#endif

static inline const int MainWidth = 280;
static inline const int MainRowHeight = 20;
static inline const int MainVolHeight = 28;
static inline const int MainRowPaddingTop = 2;
static inline const int MainRowPaddingBottom = 2;
static inline const int MainLastRowPaddingBottom = 0;
static inline const int MainRegLabelWidth = 50;
static inline const int MainRegValueWidth = 190;
static inline const int MainRegButtonWidth = 240;
static inline const int MainRegPaddingRight = 0;
static inline const int MainOpRegFreqChangeButtonWidth = 120;
static inline const int MainRegMixChangeButtonWidth = 64;
static inline const int MainRegPanChangeButtonWidth = 64;
static inline const int MainRegPanPaddingHeight = 2;

static inline const int HeaderWidth = 960;
static inline const int HeaderHeight = 80;
static inline const int FooterWidth = 960;
static inline const int FooterHeight = 80;

static inline const int LabelWidth = 40;
static inline const int LabelHeight = 20;
static inline const int SliderWidth = 50;
static inline const int SliderHeight = 20;
static inline const int SliderValueWidth = 50;
static inline const int SliderValueHeight = 20;
static inline const int ComboBoxWidth = 50;
static inline const int ComboBoxHeight = 20;
static inline const int ToggleButtonWidth = 80;
static inline const int ToggleButtonHeight = 20;
static inline const int TextButtonWidth = 80;
static inline const int TextButtonHeight = 20;

static inline const int GlobalPaddingWidth = 20;
static inline const int GlobalPaddingHeight = 20;
static inline const int GroupPaddingWidth = 10;
static inline const int GroupPaddingHeight = 10;
static inline const int TitlePaddingTop = 10;
static inline const int TopGroupHeight = 80;

static inline const int FmOpRowH = 36;
static inline const int FmOpWidth = 250;
static inline const int FmOpHeight = 640;

static const int Fm4Ops = 4;
static const int Fm2Ops = 2;

static const int WtSamples32 = 32;
static const int WtSamples64 = 64;

static const int TopParamWidth = 160;
static const int QualityGroupHeight = 60;
static const int QualityParamWidth = 160;

static const int OpRowHeight = 20;
static const int OpRowPaddingBottom = 2;
static const int OpLastRowPaddingBottom = 0;
static const int OpRegLabelWidth = 50;
static const int OpRegValueWidth = 190;
static const int OpRegButtonWidth = 240;
static const int OpRegPaddingRight = 0;
static const int OpRegFreqChangeButtonWidth = 120;
static const int OpRegMixChangeButtonWidth = 64;
static const int OpRegPanChangeButtonWidth = 64;
static const int OpRegPanPaddingHeight = 2;
static const int OpRegSsgTriPeakButtonWidth = 80;
static const int OpRegPcmFilePathWidth = 155;
static const int OpRegClearPcmFileWidth = 20;
static const int OpLabelWidth = 50;
static const int OpLabelHeight = 15;
static const int OpSliderWidth = 50;
static const int OpSliderHeight = 15;
static const int OpSliderValueWidth = 50;
static const int OpSliderValueHeight = 15;
static const int OpComboBoxWidth = 80;
static const int OpComboBoxHeight = 15;
static const int OpToggleButtonWidth = 80;
static const int OpToggleButtonHeight = 15;
static const int OpTextButtonWidth = 80;
static const int OpTextButtonHeight = 15;
static const int OpGroupPaddingWidth = 12;
static const int OpGroupPaddingHeight = 8;

static const int SsgLeftWidth = 240;
static const int SsgRightWidth = 800;
static const int SsgHeight = 640;
static const int WtRightWidth = 1000;
static const int WtRightHeight = 560;
static const int WtHeight = 640;
static const int WtCustomSliderWidth = 30;
static const int WtCustomSliderHeight = 480;
static const int WtCustomSliderResetBtnPaddingTop = 8;
static const int WtCustomSliderResetBtnPaddingRight = 4;
static const int WtCustomSliderResetBtnHeight = 36;
static const int WtCustomResetTo0ButtonWidth = 120;
static const int WtCustomResetTo1ButtonWidth = 120;
static const int WtCustomResetToM1ButtonWidth = 120;

static const int RhythmPadsAreaWidth = FmOpWidth;
static const int RhythmPadsAreaHeight = 240;

static const int PresetFileLabelHeight = 24;
static const int PresetMetaAreaHeight = 320;
static const int PresetMetaHeight = 20;
static const int PresetMetaLabelWidth = 50;
static const int PresetMetaPaddingWidth = 4;
static const int PresetMetaPaddingHeight = 4;
static const float PresetTableWidthRate = 0.6f;
static const int PresetTablePaddingWidth = 4;
static const int PresetTablePaddingHeight = 4;
static const int PresetTableFileNameColTitleWidth = 150;
static const int PresetTableModeColTitleWidth = 80;
static const int PresetTablePresetNameColTitleWidth = 150;
static const int PresetTableAuthorColTitleWidth = 100;
static const int PresetTableVersionColTitleWidth = 50;

static const int PresetButtonsPaddingTop = 10;
static const int PresetButtonHeight = 20;
static const int PresetButtonPaddingHeight = 4;

static const int FxWidth = 1000;
static const int FxHeight = 720;
static const int FxAreaHeight = 160;
static const int FxGlobalBypassHeight = 120;
static const int FxBypassWidth = 120;
static const int FxGroupHeight = 200;
static const int FxKnobAreaWidth = 200;
static const int FxKnobWidth = 120;
static const int FxKnobHeight = 60;
static const int FxButtonWidth = 240;

static const int SettingsPaddingHeight = 4;
static const int SettingsRowHeight = 30;
static const int SettingsLabelWidth = 80;
static const int SettingsLongLabelWidth = 140;
static const int SettingsModeSelectorWidth = 320;
static const int SettingsBrowseButtonWidth = 80;
static const int SettingsClearButtonWidth = 60;
static const int SettingsHeadroomGainSliderWidth = 200;
static const int SettingsToggleWidth = 400;
static const int SettingsButtonWidth = 200;
static const int SettingsButtonPaddingRight = 4;

static std::vector<SelectItem> bdItems = {
    {.name = "1: 4-bit (16 steps)",  .value = 1 },
    {.name = "2: 5-bit (32 steps)",  .value = 2 },
    {.name = "3: 6-bit (64 steps)",  .value = 3 },
    {.name = "4: 8-bit (256 steps)", .value = 4 },
    {.name = "5: Raw",               .value = 5 },
};

static std::vector<SelectItem> qualityItems = {
    {.name = "1: Raw (32bit)", .value = 1 },
    {.name = "2: 24-bit PCM",  .value = 2 },
    {.name = "3: 16-bit PCM",  .value = 3 },
    {.name = "4: 8-bit PCM",   .value = 4 },
    {.name = "5: 5-bit PCM",   .value = 5 },
    {.name = "6: 4-bit PCM",   .value = 6 },
    {.name = "7: 4-bit ADPCM", .value = 7 },
};

static std::vector<SelectItem> rateItems = {
    {.name = "1: 96kHz",    .value = 1 },
    {.name = "2: 55.5kHz",  .value = 2 },
    {.name = "3: 48kHz",    .value = 3 },
    {.name = "4: 44.1kHz",  .value = 4 },
    {.name = "5: 22.05kHz", .value = 5 },
    {.name = "6: 16kHz",    .value = 6 },
    {.name = "7: 8kHz",     .value = 7 },
};

static std::vector<SelectItem> pmsItems = {
    {.name = "1: Pms 0", .value = 1 },
    {.name = "2: Pms 1", .value = 2 },
    {.name = "3: Pms 2", .value = 3 },
    {.name = "4: Pms 3", .value = 4 },
    {.name = "5: Pms 4", .value = 5 },
    {.name = "6: Pms 5", .value = 6 },
    {.name = "7: Pms 6", .value = 7 },
    {.name = "8: Pms 7", .value = 8 },
};

static std::vector<SelectItem> amsItems = {
    {.name = "1: Ams 0", .value = 1 },
    {.name = "2: Ams 1", .value = 2 },
    {.name = "3: Ams 2", .value = 3 },
    {.name = "4: Ams 3", .value = 4 },
};

static std::vector<SelectItem> ksItems = { {.name = "0 OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

static std::vector<SelectItem> kslItems = { {.name = "KSL: 0", .value = 1}, {.name = "KSL: 1", .value = 2}, {.name = "KSL: 2", .value = 3}, {.name = "KSL: 3", .value = 4}, };

static std::vector<SelectItem> opnaAlgItems = {
    {.name = "0: [OP0]:FB -> [OP1] -> [OP2] -> [OP3]", .value = 1 },
    {.name = "1: ([OP0]:FB + [OP1]) -> [OP2] -> [OP3]", .value = 2 },
    {.name = "2: ([OP0] -> [OP1] + [OP2]:FB) -> [OP3]", .value = 3 },
    {.name = "3: ([OP0]:FB -> [OP1]) + [OP2] -> [OP3]", .value = 4 },
    {.name = "4: ([OP0]:FB -> [OP1]) + ([OP2] -> [OP3])", .value = 5 },
    {.name = "5: ([OP0]:FB -> [OP1]) + ([OP0]:FB -> [OP2]) + ([OP0]:FB -> [OP3])", .value = 6 },
    {.name = "6: ([OP0]:FB -> [OP1]) + [OP2] + [OP3]", .value = 7 },
    {.name = "7: [OP0]:FB + [OP1] + [OP2] + [OP3]", .value = 8 },
};

static std::vector<SelectItem> opnaSeItems = {
    {.name = "0: Normal", .value = 1 },
    {.name = "1: Saw Down", .value = 2 },
    {.name = "2: Saw Down & Hold", .value = 3 },
    {.name = "3: Triangle", .value = 4 },
    {.name = "4: Alternative Saw Down & Hold", .value = 5 },
    {.name = "5: Saw Up", .value = 6 },
    {.name = "6: Saw Up & Hold", .value = 7 },
    {.name = "7: Triangle Invert", .value = 8 },
    {.name = "8: Alternative Saw Up & Hold", .value = 9 },
};

static std::vector<SelectItem> oplAlgItems = {
    {.name = "0: [OP0]:FB -> [OP1]", .value = 1 },
    {.name = "1: [OP0]:FB + [OP1]", .value = 2 },
};

static std::vector<SelectItem> oplEgItems = { {.name = "0: Sine", .value = 1}, {.name = "1: Half", .value = 2}, {.name = "2: Abs", .value = 3}, {.name = "3: Pulse", .value = 4}, };

static std::vector<SelectItem> opl3AlgItems = {
    {.name = "0: [OP0]:FB -> [OP1] -> [OP2] -> [OP3]", .value = 1 },
    {.name = "1: [OP0]:FB + ([OP1] -> [OP2] -> [OP3])", .value = 2 },
    {.name = "2: ([OP0]:FB -> [OP1]) + ([OP2] -> [OP3])", .value = 3 },
    {.name = "3: [OP0]:FB + ([OP1] + [OP2]) + [OP3]", .value = 4 },
};

static std::vector<SelectItem> opl3EgItems = {
    {.name = "0: Sine", .value = 1},
    {.name = "1: Half Sine", .value = 2},
    {.name = "2: Abs Sine", .value = 3},
    {.name = "3: Pulse", .value = 4},
    {.name = "4: Alternative Sine", .value = 5},
    {.name = "5: Alternative Abs Sine", .value = 6},
    {.name = "6: Square", .value = 7},
    {.name = "7: Derived Square", .value = 8},
};

static std::vector<SelectItem> opmAlgItems = {
    {.name = "0: [OP0]:FB -> [OP1] -> [OP2] -> [OP3]", .value = 1 },
    {.name = "1: ([OP0]:FB + [OP1]) -> [OP2] -> [OP3]", .value = 2 },
    {.name = "2: [OP0]:FB + ([OP1] -> [OP2]) -> [OP3]", .value = 3 },
    {.name = "3: ([OP0]:FB -> [OP1]) + [OP2] -> [OP3]", .value = 4 },
    {.name = "4: ([OP0]:FB -> [OP1]) + ([OP2] -> [OP3])", .value = 5 },
    {.name = "5: ([OP0]:FB -> [OP1]) + ([OP0]:FB -> [OP2]) + ([OP0]:FB -> [OP3])", .value = 6 },
    {.name = "6: ([OP0]:FB -> [OP1]) + [OP2] + [OP3]", .value = 7 },
    {.name = "7: [OP0]:FB + [OP1] + [OP2] + [OP3]", .value = 8 },
};

static std::vector<SelectItem> opzx3AlgItems = {
    {.name = "00: [OP0]:FB -> [OP1] -> [OP2] -> [OP3]", .value = 1 },
    {.name = "01: ([OP0] -> [OP1]):FB -> [OP2] -> [OP3]", .value = 2 },
    {.name = "02: ([OP0]:FB + [OP1]) -> [OP2] -> [OP3]", .value = 3 },
    {.name = "03: ([OP0]:FB -> [OP3]) + ([OP1] -> [OP2] -> [OP3])", .value = 4 },
    {.name = "04: ([OP0]:FB -> [OP1]) + ([OP0]:FB -> [OP2]) + ([OP0]:FB -> [OP3])", .value = 5 },
    {.name = "05: ([OP0] -> [OP1]):FB + ([OP0] -> [OP2]) + ([OP0] -> [OP3])", .value = 6 },
    {.name = "06: ([OP0]:FB -> [OP1]) + ([OP2] -> [O32])", .value = 7 },
    {.name = "07: ([OP0] -> [OP1]):FB + ([OP2] -> [O32])", .value = 8 },
    {.name = "08: [OP0]:FB + ([OP1] -> [OP2] -> [OP3])", .value = 9 },
    {.name = "09: [OP0]:FB + ([OP1] -> [OP2]) -> [OP3]", .value = 10 },
    {.name = "10: ([OP0]:FB -> [OP1]) + [OP2] + [OP3]", .value = 11 },
    {.name = "11: ([OP0] -> [OP1]):FB + [OP2] + [OP3]", .value = 12 },
    {.name = "12: ([OP0]:FB -> [OP1]) + ([OP0]:FB -> [OP2]) + ([OP0]:FB -> [OP3])", .value = 13 },
    {.name = "13: [OP0]:FB + ([OP1] -> [OP2]) + [OP3]", .value = 14 },
    {.name = "14: [OP0]:FB + ([OP0]:FB -> [OP1]) + ([OP2] -> [OP3])", .value = 15 },
    {.name = "15: [OP0]:FB + [OP1] + [OP2] + [OP3]", .value = 16 },
    {.name = "16 [OP0]:FB -> [OP1] -> [OP2]", .value = 17 },
    {.name = "17 ([OP0] -> [OP1]):FB -> [OP2]", .value = 18 },
    {.name = "18 ([OP0]:FB + [OP2]) -> [OP1]", .value = 19 },
    {.name = "19: [OP0]:FB + [OP2] -> [OP1]", .value = 20 },
    {.name = "20: ([OP0]:FB -> [OP2]) + [OP1]", .value = 21 },
    {.name = "21: ([OP0]:FB -> [OP1]):FB + [OP2]", .value = 22 },
    {.name = "22: [OP0]:FB + ([OP1] -> [OP2]) + [OP1]", .value = 23 },
    {.name = "23: [OP0]:FB + ([OP0] -> [OP2]) + [OP1]", .value = 24 },
    {.name = "24: [OP0]:FB -> [OP1]", .value = 25 },
    {.name = "25: ([OP0] -> [OP1]):FB", .value = 26 },
    {.name = "26: [OP0]:FB + [OP1]", .value = 27 },
    {.name = "27: [OP0]:FB + ([OP0] -> [OP1])", .value = 28 },
};

static std::vector<SelectItem> opzx3WsItems = {
    {.name = "00: Sine", .value = 1},
    {.name = "01: Half Sine", .value = 2},
    {.name = "02: Abs Sine", .value = 3},
    {.name = "03: Alternating Half Sine", .value = 4},
    {.name = "04: Alternating Sine", .value = 5},
    {.name = "05: Alternating Abs Sine", .value = 6},
    {.name = "06: Square", .value = 7},
    {.name = "07: Derived Square", .value = 8},
    {.name = "08: Saw Down", .value = 9},
    {.name = "09: Saw Up", .value = 10},
    {.name = "10: Triangle", .value = 11},
    {.name = "11: Saw + Sine", .value = 12},
    {.name = "12: Log Saw", .value = 13},
    {.name = "13: Pulse 25%", .value = 14},
    {.name = "14: Pulse 12.5%", .value = 15},
    {.name = "15: Pulse 6.25%", .value = 16},
    {.name = "16: Round Square", .value = 17},
    {.name = "17: Impulse Train", .value = 18},
    {.name = "18: Comb / Multi-pulse", .value = 19},
    {.name = "19: Resonant Saw (Low)", .value = 20},
    {.name = "20: Resonant Saw (High)", .value = 21},
    {.name = "21: Resonant Triangle", .value = 22},
    {.name = "22: Bulb Sine", .value = 23},
    {.name = "23: Double Hump", .value = 24},
    {.name = "24: Pseudo Voice Formant 1", .value = 25},
    {.name = "25: Pseudo Voice Formant 2", .value = 26},
    {.name = "26: Metallic 1", .value = 27},
    {.name = "27: Metallic 2", .value = 28},
    {.name = "28: Noise-Like", .value = 29},
    {.name = "29: PCM(Audio) File", .value = 30},
};

static std::vector<SelectItem> ssgWsItems = {
    {.name = "0: Pulse(Rect)", .value = 1 },
    {.name = "1: Triangle / Saw", .value = 2 },
};

static std::vector<SelectItem> ssgDmItems = {
    {.name = "0: Preset Ratio", .value = 1 },
    {.name = "1: Variable (Slider)", .value = 2 },
};

static std::vector<SelectItem> ssgPrItems = {
    {.name = "0: 1:1 (50%)", .value = 1 },
    {.name = "1: 3:5 (37.5%)", .value = 2 },
    {.name = "2: 5:11 (31.25%)", .value = 3 },
    {.name = "3: 1:3 (25%)", .value = 4 },
    {.name = "4: 1:4 (20%)", .value = 5 },
    {.name = "5: 3:13 (18.75%)", .value = 6 },
    {.name = "6: 1:7 (12.5%)", .value = 7 },
    {.name = "7: 1:15 (6.25%)", .value = 8 },
};

static std::vector<SelectItem> ssgEnvItems = {
    {.name = "0: Saw Down", .value = 1 },
    {.name = "1: Saw Down & Hold", .value = 2 },
    {.name = "2: Triangle", .value = 3 },
    {.name = "3: Alternative Saw Down & Hold", .value = 4 },
    {.name = "4: Saw Up", .value = 5 },
    {.name = "5: Saw Up & Hold", .value = 6 },
    {.name = "6: Triangle Invert", .value = 7 },
    {.name = "7: Alternative Saw Up & Hold", .value = 8 },
};

static std::vector<SelectItem> wtWsItems = {
    {.name = "0: Sine",          .value = 1 },
    {.name = "1: Triangle",      .value = 2 },
    {.name = "2: Saw Up",        .value = 3 },
    {.name = "3: Saw Down",      .value = 4 },
    {.name = "4: Square (50%)",  .value = 5 },
    {.name = "5: Pulse (25%)",   .value = 6 },
    {.name = "6: Pulse (12.5%)", .value = 7 },
    {.name = "7: Digital Noise", .value = 8 },
    {.name = "8: Custom(Draw)",  .value = 9 },
};

static std::vector<SelectItem> wtTsItems = {
    {.name = "0: 32 Samples",  .value = 1 },
    {.name = "1: 64 Samples",  .value = 2 },
};

static std::vector<SelectItem> flTypeItems = {
    {.name = "LPF (Low Pass)", .value = 1 },
    {.name = "HPF (High Pass)", .value = 2 },
    {.name = "BPF (Band Pass)", .value = 3 }
};
