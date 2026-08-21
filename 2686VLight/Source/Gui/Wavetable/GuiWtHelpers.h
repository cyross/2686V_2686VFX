#pragma once

#include <JuceHeader.h>
#include <vector>
#include "./GuiWtValues.h"

struct RowConfigWtWaveValueUpdate {
    juce::Rectangle<int>& rect;
    juce::Component* resetTo0Btn;
    juce::Component* resetTo1Btn;
    juce::Component* resetToM1Btn;
    int rowHeight = WtGuiValue::ParamGroup::Row::height;
    int paddingTop = WtGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = WtGuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = WtGuiValue::ParamGroup::Row::paddingRight;
    int resetTo0BtnWidth = WtGuiValue::Custom::ResetBtn::to0Width;
    int resetTo1Width = WtGuiValue::Custom::ResetBtn::to1Width;
    int resetToM1Width = WtGuiValue::Custom::ResetBtn::toM1Width;
};

void layoutRowWtWaveValueUpdate(const RowConfigWtWaveValueUpdate& c);
