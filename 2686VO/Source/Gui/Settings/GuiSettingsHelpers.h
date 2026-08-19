#pragma once

#include <JuceHeader.h>
#include <vector>
#include "./GuiSettingsValues.h"

struct RowConfigSettingsIo {
    juce::Rectangle<int>& rect;
    juce::Component* loadSettingsBtn;
    juce::Component* saveSettingsBtn;
    juce::Component* saveStartupSettingsBtn;
    int rowHeight = SettingsGuiValue::ParamGroup::Row::height;
    int paddingTop = SettingsGuiValue::ParamGroup::Row::paddingTop;
    int paddingBottom = SettingsGuiValue::ParamGroup::Row::paddingBottom;
    int paddingRight = SettingsGuiValue::ParamGroup::Row::paddingRight;
    int loadSettingsBtnWidth = SettingsGuiValue::Settings::ButtonWidth;
    int saveSettingsBtnWidth = SettingsGuiValue::Settings::ButtonWidth;
    int saveStartupSettingsBtnWidth = SettingsGuiValue::Settings::ButtonWidth;
};

void layoutRowSettingsIo(const RowConfigSettingsIo& c);
