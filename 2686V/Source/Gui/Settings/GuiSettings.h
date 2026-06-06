#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"

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

    GuiLabel wavetableDirLabel;
    GuiLabel wavetableDirPathLabel;
    GuiTextButton wavetableDirBrowseBtn;

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

    // アンドゥ・リドゥ履歴消去
    GuiTextButton clearUndoHistoryBtn;
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
        wavetableDirLabel(context),
        wavetableDirPathLabel(context),
        wavetableDirBrowseBtn(context),
        virtualMidiKeyboardToggle(context),
        saveSettingsBtn(context),
        loadSettingsBtn(context),
        saveStartupSettingsBtn(context),
        tooltipToggle(context),
        useHeadroomToggle(context),
        headroomGainSlider(context),
        clearUndoHistoryBtn(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void setSettings(const juce::String& wallpaperPath, const juce::String& sampleDirPath, const juce::String& presetDirPath, const juce::String& wavetableDirPath);
	void setWallpaperPath(const juce::String& wallpaperPath);
};
