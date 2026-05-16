#pragma once

#include <JuceHeader.h>
#include <vector>
#include "./GuiValues.h"

struct RowConfigOpzx7File {
    juce::Rectangle<int>& rect;
    juce::Component* loadPcmBtn;
    juce::Component* pcmFileNameLabel;
    juce::Component* clearPcmBtn;
    int rowHeight = Opzx7GuiValue::ParamGroup::Row::height;
    int paddingTop = Opzx7GuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = Opzx7GuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = Opzx7GuiValue::ParamGroup::Row::paddingRight;
    int loadPcmBtnWidth = Opzx7GuiValue::ParamGroup::Opzx7File::LoadBtn::width;
    int pcmFileNameLabelWidth = Opzx7GuiValue::ParamGroup::Opzx7File::FileLabel::width;
    int clearPcmBtnWidth = Opzx7GuiValue::ParamGroup::Opzx7File::ClearBtn::width;
};

void layoutRowOpzx7File(const RowConfigOpzx7File& c);
