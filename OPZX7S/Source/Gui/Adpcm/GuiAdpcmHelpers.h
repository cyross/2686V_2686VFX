#pragma once

#include <JuceHeader.h>
#include "../../Core/Gui/GuiValues.h"
#include "./GuiAdpcmValues.h"

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
