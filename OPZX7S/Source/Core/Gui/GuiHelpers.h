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

struct MainConfigParamName {
    juce::Rectangle<int>& mainRect;
    GuiLabel* label = nullptr;
    juce::Component* component = nullptr;
    int rowHeight = CoreGuiValue::ParamNameRow::height;
    int paddingTop = CoreGuiValue::ParamNameRow::paddingTop;
    int paddingBottom = CoreGuiValue::ParamNameRow::paddingBottom;
    int labelWidth = -1;
    int labelPaddingRight = -1;
    int compWidth = -1;
    int compPaddingRight = 0;
};

void layoutMainParamName(const MainConfigParamName& c);

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

struct RowConfigOneComp {
    juce::Rectangle<int>& rect;
    juce::Component* comp;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::ParamGroup::Comps::One::width;
    int compPaddingRight = CoreGuiValue::ParamGroup::Comps::One::paddingRight;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::One::paddingLeft;
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
    int compPaddingInner = CoreGuiValue::ParamGroup::Comps::Two::paddingInner;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::Two::paddingLeft;
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
    int compPaddingInner = CoreGuiValue::ParamGroup::Comps::Three::paddingInner;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::Three::paddingLeft;
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
    int compPaddingInner = CoreGuiValue::ParamGroup::Comps::Four::paddingInner;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::Four::paddingLeft;
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
    int compPaddingInner = CoreGuiValue::ParamGroup::Comps::Five::paddingInner;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::Five::paddingLeft;
};

void layoutRowFiveComps(const RowConfigFiveComps& c);

struct RowConfigSixComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    juce::Component* comp4;
    juce::Component* comp5;
    juce::Component* comp6;
    int rowHeight = CoreGuiValue::ParamGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::ParamGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::ParamGroup::Comps::Six::width;
    int compPaddingRight = CoreGuiValue::ParamGroup::Comps::Six::paddingRight;
    int compPaddingInner = CoreGuiValue::ParamGroup::Comps::Six::paddingInner;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::Six::paddingLeft;
};

void layoutRowSixComps(const RowConfigSixComps& c);

struct MainConfigOneComp {
    juce::Rectangle<int>& rect;
    juce::Component* comp;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::MainGroup::Comps::One::width;
    int compPaddingRight = CoreGuiValue::MainGroup::Comps::One::paddingRight;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::One::paddingLeft;
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
    int compPaddingInner = CoreGuiValue::ParamGroup::Comps::Two::paddingInner;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::Two::paddingLeft;
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
    int compPaddingInner = CoreGuiValue::ParamGroup::Comps::Three::paddingInner;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::Three::paddingLeft;
};

void layoutMainThreeComps(const MainConfigThreeComps& c);

struct MainConfigFourComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
	juce::Component* comp4;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::MainGroup::Comps::Four::width;
    int compPaddingRight = CoreGuiValue::MainGroup::Comps::Four::paddingRight;
    int compPaddingInner = CoreGuiValue::ParamGroup::Comps::Four::paddingInner;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::Four::paddingLeft;
};

void layoutMainFourComps(const MainConfigFourComps& c);

struct MainConfigFiveComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
	juce::Component* comp4;
	juce::Component* comp5;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::MainGroup::Comps::Five::width;
    int compPaddingRight = CoreGuiValue::MainGroup::Comps::Five::paddingRight;
    int compPaddingInner = CoreGuiValue::ParamGroup::Comps::Five::paddingInner;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::Five::paddingLeft;
};

void layoutMainFiveComps(const MainConfigFiveComps& c);

struct MainConfigSixComps {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    juce::Component* comp4;
    juce::Component* comp5;
    juce::Component* comp6;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int compWidth = CoreGuiValue::MainGroup::Comps::Six::width;
    int compPaddingRight = CoreGuiValue::MainGroup::Comps::Six::paddingRight;
    int compPaddingInner = CoreGuiValue::ParamGroup::Comps::Six::paddingInner;
    int compPaddingLeft = CoreGuiValue::ParamGroup::Comps::Six::paddingLeft;
};

void layoutMainSixComps(const MainConfigSixComps& c);

struct MainConfigFxOrder {
    juce::Rectangle<int>& rect;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int labelWidth = 200;
    int buttonWidth = 30;
    int compPaddingRight = CoreGuiValue::MainGroup::Comps::Three::paddingRight;
};

void layoutMainFxOrder(const MainConfigFxOrder& c);

struct MainConfigViewMode {
    juce::Rectangle<int>& rect;
    GuiLabel& label;
    juce::Component* comp1;
    juce::Component* comp2;
    juce::Component* comp3;
    int rowHeight = CoreGuiValue::MainGroup::Row::height;
    int paddingLeft = 0;
    int paddingTop = CoreGuiValue::MainGroup::Row::paddingTop;
    int paddingBottom = CoreGuiValue::MainGroup::Row::paddingBottom;
    int labelWidth = 120;
    int compWidth = 30;
    int compPaddingRight = CoreGuiValue::MainGroup::Comps::Four::paddingRight;
};

void layoutMainViewMode(const MainConfigViewMode& c);
