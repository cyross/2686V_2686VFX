#pragma once

#include <JuceHeader.h>

// Defined globally as it's used in structs
static const juce::String FontFamily = "Times New Roman";
static const float LogoFontSize = 128.0f;

static const int WindowWidth = 1280;
static const int WindowHeight = 600;

#if defined(BUILD_AS_FX_PLUGIN)
static const int TabNumber = 3;
#else
static const int TabNumber = 13;
#endif

static const int MainWidth = 280;
static const int MainRowHeight = 20;
static const int MainVolHeight = 28;
static const int MainRowPaddingTop = 2;
static const int MainRowPaddingBottom = 2;
static const int MainLastRowPaddingBottom = 0;
static const int MainRegLabelWidth = 50;
static const int MainRegValueWidth = 190;
static const int MainRegButtonWidth = 240;
static const int MainRegPaddingRight = 0;
static const int MainOpRegFreqChangeButtonWidth = 120;
static const int MainRegMixChangeButtonWidth = 64;
static const int MainRegPanChangeButtonWidth = 64;
static const int MainRegPanPaddingHeight = 2;

static const int HeaderWidth = 960;
static const int HeaderHeight = 80;
static const int FooterWidth = 960;
static const int FooterHeight = 80;

static const int LabelWidth = 40;
static const int LabelHeight = 20;
static const int SliderWidth = 50;
static const int SliderHeight = 20;
static const int SliderValueWidth = 50;
static const int SliderValueHeight = 20;
static const int ComboBoxWidth = 50;
static const int ComboBoxHeight = 20;
static const int ToggleButtonWidth = 80;
static const int ToggleButtonHeight = 20;
static const int TextButtonWidth = 80;
static const int TextButtonHeight = 20;

static const int GlobalPaddingWidth = 20;
static const int GlobalPaddingHeight = 20;
static const int GroupPaddingWidth = 10;
static const int GroupPaddingHeight = 10;
static const int TitlePaddingTop = 10;
static const int TopGroupHeight = 80;

static const int FmOpRowH = 36;
static const int FmOpWidth = 250;
static const int FmOpHeight = 640;

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
static const int WtCustomSliderResetBtnHeight = 36;
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
static const int SettingsBrowseButtonWidth = 80;
static const int SettingsClearButtonWidth = 60;
static const int SettingsHeadroomGainSliderWidth = 200;
static const int SettingsToggleWidth = 80;
static const int SettingsButtonWidth = 200;
static const int SettingsButtonPaddingRight = 4;

