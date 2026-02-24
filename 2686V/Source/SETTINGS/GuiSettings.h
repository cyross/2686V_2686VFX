#pragma once

#include <JuceHeader.h>
#include <array>
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiSettings : public GuiBase
{
    GuiGroup mainGroup;

    // 壁紙のファイルパス
    GuiLabel wallpaperLabel;
    GuiLabel wallpaperPathLabel;
    GuiTextButton wallpaperBrowseBtn;
    GuiTextButton wallpaperClearBtn;

    // 壁紙表示モード
    // 0: Stretch
    // 1: Fill
    // 2: Fit
    // 3: Original
    GuiComboBox wallpaperModeSelector;

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

    // 仮想MIDIキーボード表示制御
    GuiToggleButton virtualMidiKeyboardToggle;
public:
    GuiSettings(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        wallpaperLabel(context),
        wallpaperPathLabel(context),
        wallpaperBrowseBtn(context),
        wallpaperClearBtn(context),
        wallpaperModeSelector(context),
        sampleDirLabel(context),
        sampleDirPathLabel(context),
        sampleDirBrowseBtn(context),
        presetDirLabel(context),
        presetDirPathLabel(context),
        presetDirBrowseBtn(context),
        virtualMidiKeyboardToggle(context),
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
