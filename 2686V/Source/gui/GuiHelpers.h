#pragma once

#include <JuceHeader.h>
#include <vector>
#include "GuiComponents.h"
#include "GuiBase.h"

std::vector<SelectItem> createItems(int size, const juce::String& prefix);
std::vector<SelectItem> createAlgItems(int size);
juce::String getNoteName(int noteNumber);

void layoutRowOneSide(
    juce::Rectangle<int>& rect,
    int height,
    int space,
    juce::Component *comp,
    int width,
    int paddingRight
);

void layoutRowBoth(
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

void layoutRow(
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

struct MainConfig {
    juce::Rectangle<int>& mainRect;
    GuiLabel *label = nullptr;
    juce::Component *component = nullptr;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingBottom = GuiValue::MainGroup::Row::paddingBottom;
    int labelWidth = -1;
    int labelPaddingRight = -1;
    int compWidth = -1;
    int compPaddingRight = 0;
};

struct RowConfig {
    juce::Rectangle<int>& rowRect;
    GuiLabel *label = nullptr;
    juce::Component *component = nullptr;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingBottom = GuiValue::ParamGroup::Row::paddingBottom;
    int labelWidth = -1;
    int labelPaddingRight = -1;
    int compWidth = -1;
    int compPaddingRight = 0;
};

void layoutRow(const RowConfig& c);

void layoutMain(const MainConfig& c);

struct RowConfigOpzx3Pcm {
    juce::Rectangle<int>& rect;
    juce::Component* loadPcmBtn;
    juce::Component* pcmFileNameLabel;
    juce::Component* clearPcmBtn;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingBottom = GuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = GuiValue::ParamGroup::Row::paddingRight;
    int loadPcmBtnWidth = GuiValue::ParamGroup::Opzx3Pcm::LoadBtn::width;
    int pcmFileNameLabelWidth = GuiValue::ParamGroup::Opzx3Pcm::FileLabel::width;
    int clearPcmBtnWidth = GuiValue::ParamGroup::Opzx3Pcm::ClearBtn::width;
};

void layoutRowOpzx3Pcm(const RowConfigOpzx3Pcm& c);

struct RowConfigWtWaveValueUpdate {
    juce::Rectangle<int>& rect;
    juce::Component* resetTo0Btn;
    juce::Component* resetTo1Btn;
    juce::Component* resetToM1Btn;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingBottom = GuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = GuiValue::ParamGroup::Row::paddingRight;
    int resetTo0BtnWidth = GuiValue::Wt::Custom::ResetBtn::to0Width;
    int resetTo1Width = GuiValue::Wt::Custom::ResetBtn::to1Width;
    int resetToM1Width = GuiValue::Wt::Custom::ResetBtn::toM1Width;
};

void layoutRowWtWaveValueUpdate(const RowConfigWtWaveValueUpdate& c);

struct RowConfigRhythmPadPcmFile {
    juce::Rectangle<int>& rect;
    juce::Component* loadBtn;
    juce::Component* filenameLabel;
    juce::Component* clearBtn;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingBottom = GuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = GuiValue::ParamGroup::Row::paddingRight;
    int loadBtnWidth = GuiValue::ParamGroup::RhythmPcm::LoadBtn::width;
    int filenameLabelWidth = GuiValue::ParamGroup::RhythmPcm::FileLabel::width;
    int clearBtnWidth = GuiValue::ParamGroup::RhythmPcm::ClearBtn::width;
};

void layoutRowRhythmPadPcmFile(const RowConfigRhythmPadPcmFile& c);

struct RowConfigSettingsIo {
    juce::Rectangle<int>& rect;
    juce::Component* loadSettingsBtn;
    juce::Component* saveSettingsBtn;
    juce::Component* saveStartupSettingsBtn;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingBottom = GuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = GuiValue::ParamGroup::Row::paddingRight;
    int loadSettingsBtnWidth = GuiValue::Settings::ButtonWidth;
    int saveSettingsBtnWidth = GuiValue::Settings::ButtonWidth;
    int saveStartupSettingsBtnWidth = GuiValue::Settings::ButtonWidth;
};

void layoutRowSettingsIo(const RowConfigSettingsIo& c);

struct RowConfigOneComp {
    juce::Rectangle<int>& rect;
    juce::Component* comp;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingBottom = GuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = GuiValue::ParamGroup::Comps::One::width;
    int compPaddingRight = GuiValue::ParamGroup::Comps::One::paddingRight;
};

void layoutRowOneComp(const RowConfigOneComp& c);

struct RowConfigTwoComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingBottom = GuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = GuiValue::ParamGroup::Comps::Two::width;
    int compPaddingRight = GuiValue::ParamGroup::Comps::Two::paddingRight;
};

void layoutRowTwoComps(const RowConfigTwoComps& c);

struct RowConfigThreeComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingBottom = GuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = GuiValue::ParamGroup::Comps::Three::width;
    int compPaddingRight = GuiValue::ParamGroup::Comps::Three::paddingRight;
};

void layoutRowThreeComps(const RowConfigThreeComps& c);

struct RowConfigFourComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    juce::Component* comp4;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingBottom = GuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = GuiValue::ParamGroup::Comps::Four::width;
    int compPaddingRight = GuiValue::ParamGroup::Comps::Four::paddingRight;
};

void layoutRowFourComps(const RowConfigFourComps& c);

struct RowConfigFiveComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    juce::Component* comp4;
    juce::Component* comp5;
    int rowHeight = GuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingBottom = GuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = GuiValue::ParamGroup::Comps::Five::width;
    int compPaddingRight = GuiValue::ParamGroup::Comps::Five::paddingRight;
};

void layoutRowFiveComps(const RowConfigFiveComps& c);

struct MainConfigOneComp {
    juce::Rectangle<int>& rect;
    juce::Component* comp;
    int rowHeight = GuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingBottom = GuiValue::MainGroup::Row::paddingBottom;
    int compWidth = GuiValue::MainGroup::Comps::One::width;
    int compPaddingRight = GuiValue::MainGroup::Comps::One::paddingRight;
};

void layoutMainOneComp(const MainConfigOneComp& c);

struct MainConfigTwoComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    int rowHeight = GuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingBottom = GuiValue::MainGroup::Row::paddingBottom;
    int compWidth = GuiValue::MainGroup::Comps::Two::width;
    int compPaddingRight = GuiValue::MainGroup::Comps::Two::paddingRight;
};

void layoutMainTwoComps(const MainConfigTwoComps& c);

struct MainConfigThreeComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    int rowHeight = GuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingBottom = GuiValue::MainGroup::Row::paddingBottom;
    int compWidth = GuiValue::MainGroup::Comps::Three::width;
    int compPaddingRight = GuiValue::MainGroup::Comps::Three::paddingRight;
};

void layoutMainThreeComps(const MainConfigThreeComps& c);
