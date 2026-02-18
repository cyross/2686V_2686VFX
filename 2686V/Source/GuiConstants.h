#pragma once

#include <JuceHeader.h>

// Defined globally as it's used in structs
static const juce::String FontFamily = "Times New Roman";
static const float LogoFontSize = 128.0f;

static const int WindowWidth = 1280;
static const int WindowHeight = 880;

#if defined(BUILD_AS_FX_PLUGIN)
static const int TabNumber = 3;
#else
static const int TabNumber = 13;
#endif

static const int MainWidth = 280;
static const int MainRowHeight = 20;
static const int MainVolHeight = 28;
static const int MainRowPaddingBottom = 2;
static const int MainLastRowPaddingBottom = 0;

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

#if !defined(BUILD_AS_FX_PLUGIN)
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
static const int OpLabelWidth = 40;
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

static const int FmOpRowH = 36;
static const int FmOpWidth = 250;
static const int FmOpHeight = 640;

static const int SsgLeftWidth = 240;
static const int SsgRightWidth = 800;
static const int SsgHeight = 640;
static const int WtLeftWidth = 240;
static const int WtRightWidth = 800;
static const int WtHeight = 640;
static const int WtCustomSliderWidth = 22;
static const int RhythmPadsAreaWidth = FmOpWidth;
static const int RhythmPadsAreaHeight = 320;
#endif

static const int FxWidth = 1000;
static const int FxHeight = 720;
static const int FxGlobalBypassHeight = 120;
static const int FxBypassWidth = 120;
static const int FxGroupHeight = 200;
static const int FxKnobAreaWidth = 200;
static const int FxKnobWidth = 120;
static const int FxKnobHeight = 60;
static const int FxMixButtonWidth = 40;

