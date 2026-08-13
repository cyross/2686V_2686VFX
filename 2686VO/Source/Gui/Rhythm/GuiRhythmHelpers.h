#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../../Core/Gui/GuiValues.h"
#include "./GuiRhythmValues.h"

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
