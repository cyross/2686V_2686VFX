#pragma once

#include <JuceHeader.h>
#include <vector>
#include "./GuiWt2Values.h"

struct RowConfigWt2WaveValueUpdate {
    juce::Rectangle<int>& rect;
    juce::Component* resetTo0Btn;
    juce::Component* resetTo1Btn;
    juce::Component* resetToM1Btn;
    int rowHeight = Wt2GuiValue::ParamGroup::Row::height;
    int paddingTop = Wt2GuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = Wt2GuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = Wt2GuiValue::ParamGroup::Row::paddingRight;
    int resetTo0BtnWidth = Wt2GuiValue::Custom::ResetBtn::to0Width;
    int resetTo1Width = Wt2GuiValue::Custom::ResetBtn::to1Width;
    int resetToM1Width = Wt2GuiValue::Custom::ResetBtn::toM1Width;
};

void layoutRowWt2WaveValueUpdate(const RowConfigWt2WaveValueUpdate& c);
