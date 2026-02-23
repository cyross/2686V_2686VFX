#pragma once

#include <JuceHeader.h>
#include <array>
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiSettings : public GuiBase
{
    GuiGroup mainGroup;

    // Wallpaper
    GuiLabel wallpaperLabel;
    GuiLabel wallpaperPathLabel;
    GuiTextButton wallpaperBrowseBtn;
    GuiTextButton wallpaperClearBtn;

    // Directories
    GuiLabel sampleDirLabel;
    GuiLabel sampleDirPathLabel;
    GuiTextButton sampleDirBrowseBtn;

    GuiLabel presetDirLabel;
    GuiLabel presetDirPathLabel;
    GuiTextButton presetDirBrowseBtn;

    // Global Settings I/O
    GuiTextButton saveSettingsBtn;
    GuiTextButton loadSettingsBtn;
    GuiTextButton saveStartupSettingsBtn;

    // Tooltip Visible Switch
    GuiToggleButton tooltipToggle;

    // For Headroom
    GuiToggleButton useHeadroomToggle;
    GuiSlider headroomGainSlider;
public:
    GuiSettings(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        wallpaperLabel(context),
        wallpaperPathLabel(context),
        wallpaperBrowseBtn(context),
        wallpaperClearBtn(context),
        sampleDirLabel(context),
        sampleDirPathLabel(context),
        sampleDirBrowseBtn(context),
        presetDirLabel(context),
        presetDirPathLabel(context),
        presetDirBrowseBtn(context),
        saveSettingsBtn(context),
        loadSettingsBtn(context),
        saveStartupSettingsBtn(context),
        tooltipToggle(context),
        useHeadroomToggle(context),
        headroomGainSlider(context)
    {
	}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void setSettings(const juce::String& wallpaperPath, const juce::String& sampleDirPath, const juce::String& presetDirPath);
	void setWallpaperPath(const juce::String& wallpaperPath);
};
