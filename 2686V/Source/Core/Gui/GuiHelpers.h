#pragma once

#include <JuceHeader.h>
#include <vector>
#include "./GuiComponents.h"
#include "./GuiBase.h"

std::vector<SelectItem> createItems(int size, const juce::String& prefix);
std::vector<SelectItem> createAlgItems(int size);
juce::String getNoteName(int noteNumber);

void layoutRowOneSide(
    juce::Rectangle<int>& rect,
    int height,
    int paddingTop,
    int paddingBottom,
    juce::Component *comp,
    int width,
    int paddingRight
);

void layoutRowBoth(
    juce::Rectangle<int>& rect,
    int height,
    int paddingTop,
    int paddingBottom,
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
    int paddingTop,
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
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int labelWidth = -1;
    int labelPaddingRight = -1;
    int compWidth = -1;
    int compPaddingRight = 0;
};

void layoutMain(const MainConfig& c);

struct RowConfig {
    juce::Rectangle<int>& rowRect;
    GuiLabel *label = nullptr;
    juce::Component *component = nullptr;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int labelWidth = -1;
    int labelPaddingRight = -1;
    int compWidth = -1;
    int compPaddingRight = 0;
};

void layoutRow(const RowConfig& c);

struct MainConfigCategory {
    juce::Rectangle<int>& mainRect;
    GuiLabel* label = nullptr;
    juce::Component* component = nullptr;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingTop = CoreGuiValue::Category::paddingTop;
    int paddingBottom = CoreGuiValue::Category::paddingBottom;
    int labelWidth = -1;
    int labelPaddingRight = -1;
    int compWidth = -1;
    int compPaddingRight = 0;
};

void layoutMainCategory(const MainConfigCategory& c);

struct RowConfigCategory {
    juce::Rectangle<int>& rowRect;
    GuiLabel* label = nullptr;
    juce::Component* component = nullptr;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingTop = CoreGuiValue::Category::paddingTop;
    int paddingBottom = CoreGuiValue::Category::paddingBottom;
    int labelWidth = -1;
    int labelPaddingRight = -1;
    int compWidth = -1;
    int compPaddingRight = 0;
};

void layoutRowCategory(const RowConfigCategory& c);

struct RowConfigOpzx7Pcm {
    juce::Rectangle<int>& rect;
    juce::Component* loadPcmBtn;
    juce::Component* pcmFileNameLabel;
    juce::Component* clearPcmBtn;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = CoreGuiValue::ParamGroup::Row::paddingRight;
    int loadPcmBtnWidth = CoreGuiValue::ParamGroup::Opzx7Pcm::LoadBtn::width;
    int pcmFileNameLabelWidth = CoreGuiValue::ParamGroup::Opzx7Pcm::FileLabel::width;
    int clearPcmBtnWidth = CoreGuiValue::ParamGroup::Opzx7Pcm::ClearBtn::width;
};

void layoutRowOpzx7Pcm(const RowConfigOpzx7Pcm& c);

struct RowConfigWtWaveValueUpdate {
    juce::Rectangle<int>& rect;
    juce::Component* resetTo0Btn;
    juce::Component* resetTo1Btn;
    juce::Component* resetToM1Btn;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = CoreGuiValue::ParamGroup::Row::paddingRight;
    int resetTo0BtnWidth = CoreGuiValue::Wt::Custom::ResetBtn::to0Width;
    int resetTo1Width = CoreGuiValue::Wt::Custom::ResetBtn::to1Width;
    int resetToM1Width = CoreGuiValue::Wt::Custom::ResetBtn::toM1Width;
};

void layoutRowWtWaveValueUpdate(const RowConfigWtWaveValueUpdate& c);

struct RowConfigRhythmPadPcmFile {
    juce::Rectangle<int>& rect;
    juce::Component* loadBtn;
    juce::Component* filenameLabel;
    juce::Component* clearBtn;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = CoreGuiValue::ParamGroup::Row::paddingRight;
    int loadBtnWidth = CoreGuiValue::ParamGroup::RhythmPcm::LoadBtn::width;
    int filenameLabelWidth = CoreGuiValue::ParamGroup::RhythmPcm::FileLabel::width;
    int clearBtnWidth = CoreGuiValue::ParamGroup::RhythmPcm::ClearBtn::width;
};

void layoutRowRhythmPadPcmFile(const RowConfigRhythmPadPcmFile& c);

struct RowConfigSettingsIo {
    juce::Rectangle<int>& rect;
    juce::Component* loadSettingsBtn;
    juce::Component* saveSettingsBtn;
    juce::Component* saveStartupSettingsBtn;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = CoreGuiValue::ParamGroup::Row::paddingRight;
    int loadSettingsBtnWidth = CoreGuiValue::Settings::ButtonWidth;
    int saveSettingsBtnWidth = CoreGuiValue::Settings::ButtonWidth;
    int saveStartupSettingsBtnWidth = CoreGuiValue::Settings::ButtonWidth;
};

void layoutRowSettingsIo(const RowConfigSettingsIo& c);

struct RowConfigOneComp {
    juce::Rectangle<int>& rect;
    juce::Component* comp;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::ParamGroup::Comps::One::width;
    int compPaddingRight = CoreGuiValue::ParamGroup::Comps::One::paddingRight;
};

void layoutRowOneComp(const RowConfigOneComp& c);

struct RowConfigTwoComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::ParamGroup::Comps::Two::width;
    int compPaddingRight = CoreGuiValue::ParamGroup::Comps::Two::paddingRight;
};

void layoutRowTwoComps(const RowConfigTwoComps& c);

struct RowConfigThreeComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::ParamGroup::Comps::Three::width;
    int compPaddingRight = CoreGuiValue::ParamGroup::Comps::Three::paddingRight;
};

void layoutRowThreeComps(const RowConfigThreeComps& c);

struct RowConfigFourComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    juce::Component* comp4;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::ParamGroup::Comps::Four::width;
    int compPaddingRight = CoreGuiValue::ParamGroup::Comps::Four::paddingRight;
};

void layoutRowFourComps(const RowConfigFourComps& c);

struct RowConfigFiveComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    juce::Component* comp4;
    juce::Component* comp5;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::ParamGroup::Comps::Five::width;
    int compPaddingRight = CoreGuiValue::ParamGroup::Comps::Five::paddingRight;
};

void layoutRowFiveComps(const RowConfigFiveComps& c);

struct MainConfigOneComp {
    juce::Rectangle<int>& rect;
    juce::Component* comp;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::MainGroup::Comps::One::width;
    int compPaddingRight = CoreGuiValue::MainGroup::Comps::One::paddingRight;
};

void layoutMainOneComp(const MainConfigOneComp& c);

struct MainConfigTwoComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::MainGroup::Comps::Two::width;
    int compPaddingRight = CoreGuiValue::MainGroup::Comps::Two::paddingRight;
};

void layoutMainTwoComps(const MainConfigTwoComps& c);

struct MainConfigThreeComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::MainGroup::Comps::Three::width;
    int compPaddingRight = CoreGuiValue::MainGroup::Comps::Three::paddingRight;
};

void layoutMainThreeComps(const MainConfigThreeComps& c);

struct MainConfigPcm {
    juce::Rectangle<int>& rect;
    juce::Component* loadPcmBtn;
    juce::Component* pcmFileNameLabel;
    juce::Component* clearPcmBtn;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int paddingRight = CoreGuiValue::MainGroup::Row::paddingRight;
    int loadPcmBtnWidth = CoreGuiValue::MainGroup::Row::Pcm::LoadBtn::width;
    int pcmFileNameLabelWidth = CoreGuiValue::MainGroup::Row::Pcm::FileLabel::width;
    int clearPcmBtnWidth = CoreGuiValue::MainGroup::Row::Pcm::ClearBtn::width;
};

void layoutMainPcm(const MainConfigPcm& c);
