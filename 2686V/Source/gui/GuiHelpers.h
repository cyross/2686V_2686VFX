#pragma once

#include <JuceHeader.h>
#include <vector>
#include "GuiComponents.h"
#include "GuiBase.h"

std::vector<SelectItem> createItems(int size, const juce::String& prefix);
std::vector<SelectItem> createAlgItems(int size);
juce::String getNoteName(int noteNumber);

void layoutComponentsOneSide(
    juce::Rectangle<int>& rect,
    int height,
    int space,
    juce::Component *comp,
    int width,
    int paddingRight
);

void layoutComponentsBoth(
    juce::Rectangle<int>& rect,
    int height,
    int space,
    GuiLabel *label,
    int labelWidth,
    int labelPaddingRight,
    juce::Component *comp,
    int compWidth,
    int compPaddingRight
);

void layoutComponentsLtoR(
    juce::Rectangle<int>& rect,
    GuiLabel *label,
    juce::Component *component,
    int rowHeight,
    int paddingBottom,
    int labelWidth,
    int labelPaddingRight,
    int compWidth,
    int compPaddingRight
);

struct LtoRConfigMain {
    juce::Rectangle<int>& mainRect;
    GuiLabel *label = nullptr;
    juce::Component *component = nullptr;
    int rowHeight = GuiValue::MainGroup::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::mainRow;
    int labelWidth = -1;
    int labelPaddingRight = -1;
    int compWidth = -1;
    int compPaddingRight = 0;
};

struct LtoRConfigRow {
    juce::Rectangle<int>& rowRect;
    GuiLabel *label = nullptr;
    juce::Component *component = nullptr;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int labelWidth = -1;
    int labelPaddingRight = -1;
    int compWidth = -1;
    int compPaddingRight = 0;
};

void layoutComponentsLtoRMain(const LtoRConfigMain& c);
void layoutComponentsLtoRRow(const LtoRConfigRow& c);

struct LtoRConfigFixFreqBtns {
    juce::Rectangle<int>& rect;
    juce::Component* to0Btn;
    juce::Component* to440Btn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int buttonWidth = GuiValue::Fm::Op::Row::Button::Freq::width;
};

void layoutComponentsLtoRFixFreqBtns(const LtoRConfigFixFreqBtns& c);

struct LtoRConfigOpzx3FixFreqBtns {
    juce::Rectangle<int>& rect;
    juce::Component* to0Btn;
    juce::Component* to05Btn;
    juce::Component* to1Btn;
    juce::Component* to2Btn;
    juce::Component* to440Btn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int buttonWidth = GuiValue::Fm::Op::Row::Button::Freq::opzx3Width;
};

void layoutComponentsLtoROpzx3FixFreqBtns(const LtoRConfigOpzx3FixFreqBtns& c);

struct LtoRConfigOpzx3PcmRow {
    juce::Rectangle<int>& rect;
    juce::Component* loadPcmBtn;
    juce::Component* pcmFileNameLabel;
    juce::Component* clearPcmBtn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int loadPcmBtnWidth = GuiValue::Fm::Op::Row::Button::LoadPcm::width;
    int pcmFileNameLabelWidth = GuiValue::Fm::Op::Row::Button::PcmFile::width;
    int clearPcmBtnWidth = GuiValue::Fm::Op::Row::Button::ClearPcm::width;
};

void layoutComponentsLtoROpzx3PcmRow(const LtoRConfigOpzx3PcmRow& c);

struct LtoRConfigSsgMixRow {
    juce::Rectangle<int>& rect;
    juce::Component* toneBtn;
    juce::Component* mixBtn;
    juce::Component* noizeBtn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int btnWidth = GuiValue::Fm::Op::Row::Button::Mix::width;
};

void layoutComponentsLtoRSsgMixRow(const LtoRConfigSsgMixRow& c);

struct LtoRConfigSsgTriPosRow {
    juce::Rectangle<int>& rect;
    juce::Component* sawDownBtn;
    juce::Component* triBtn;
    juce::Component* sawUpBtn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int btnWidth = GuiValue::Fm::Op::Row::Button::Mix::width;
};

void layoutComponentsLtoRSsgTriPosRow(const LtoRConfigSsgTriPosRow& c);

struct LtoRConfigWtWaveValueUpdateRow {
    juce::Rectangle<int>& rect;
    juce::Component* resetTo0Btn;
    juce::Component* resetTo1Btn;
    juce::Component* resetToM1Btn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int btnWidth = GuiValue::Fm::Op::Row::Button::Mix::width;
};

void layoutComponentsLtoRWtWaveValueUpdateRow(const LtoRConfigWtWaveValueUpdateRow& c);

struct LtoRConfigRhythmPadPcmFileRow {
    juce::Rectangle<int>& rect;
    juce::Component* loadBtn;
    juce::Component* filenameLabel;
    juce::Component* clearBtn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int btnWidth = GuiValue::Fm::Op::Row::Button::Mix::width;
};

void layoutComponentsLtoRRhythmPadPcmFileRow(const LtoRConfigRhythmPadPcmFileRow& c);

struct LtoRConfigRhythmPadPanRow {
    juce::Rectangle<int>& rect;
    juce::Component* lBtn;
    juce::Component* cBtn;
    juce::Component* rBtn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int btnWidth = GuiValue::Fm::Op::Row::Button::Mix::width;
};

void layoutComponentsLtoRRhythmPadPanRow(const LtoRConfigRhythmPadPanRow& c);

struct LtoRConfigAdpcmPanRow {
    juce::Rectangle<int>& rect;
    juce::Component* lBtn;
    juce::Component* cBtn;
    juce::Component* rBtn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int btnWidth = GuiValue::MainGroup::Button::Pan::width;
};

void layoutComponentsLtoRAdpcmPanRow(const LtoRConfigAdpcmPanRow& c);

struct LtoRConfigFxMixRow {
    juce::Rectangle<int>& rect;
    juce::Component* dryBtn;
    juce::Component* HalfBtn;
    juce::Component* wetBtn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int btnWidth = GuiValue::Fm::Op::Row::Button::Mix::width;
};

void layoutComponentsLtoRFxMixRow(const LtoRConfigFxMixRow& c);

struct LtoRConfigSettingsIoRow {
    juce::Rectangle<int>& rect;
    juce::Component* loadSettingsBtn;
    juce::Component* saveSettingsBtn;
    juce::Component* saveStartupSettingsBtn;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int btnWidth = GuiValue::Settings::ButtonWidth;
};

void layoutComponentsLtoRSettingsIoRow(const LtoRConfigSettingsIoRow& c);

struct LtoRConfigTwoBtnsRow {
    juce::Rectangle<int>& rect;
    juce::Component* btn1;
    juce::Component* btn2;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int btnWidth = GuiValue::Fm::Op::Row::Button::Two::width;
};

void layoutComponentsLtoRTwoBtnsRow(const LtoRConfigTwoBtnsRow& c);

struct LtoRConfigThreeBtnsRow {
    juce::Rectangle<int>& rect;
    juce::Component* btn1;
    juce::Component* btn2;
    juce::Component* btn3;
    int rowHeight = GuiValue::Fm::Op::Row::height;
    int paddingBottom = GuiValue::PaddingBottom::row;
    int btnWidth = GuiValue::Fm::Op::Row::Button::Three::width;
};

void layoutComponentsLtoRThreeBtnsRow(const LtoRConfigThreeBtnsRow& c);
