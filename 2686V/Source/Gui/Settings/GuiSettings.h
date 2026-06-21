#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"

class GuiSettings : public GuiBase
{
    juce::Font toggleFont = juce::Font(juce::FontOptions(16.0f));

    GuiGroup mainGroup;

    // 高解像度対応
    GuiComboBox uiScaleSelector;

    GuiSeparator separator1;

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

    GuiSeparator separator2;

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

    GuiLabel fxOrderDirLabel;
    GuiLabel fxOrderDirPathLabel;
    GuiTextButton fxOrderDirBrowseBtn;

    GuiLabel fxParamDirLabel;
    GuiLabel fxParamDirPathLabel;
    GuiTextButton fxParamDirBrowseBtn;

    GuiLabel lfoParamDirLabel;
    GuiLabel lfoParamDirPathLabel;
    GuiTextButton lfoParamDirBrowseBtn;

    GuiLabel ampEnvParamDirLabel;
    GuiLabel ampEnvParamDirPathLabel;
    GuiTextButton ampEnvParamDirBrowseBtn;

    GuiLabel pitchEnvParamDirLabel;
    GuiLabel pitchEnvParamDirPathLabel;
    GuiTextButton pitchEnvParamDirBrowseBtn;

    GuiLabel ssgSwEnvParamDirLabel;
    GuiLabel ssgSwEnvParamDirPathLabel;
    GuiTextButton ssgSwEnvParamDirBrowseBtn;

    GuiLabel detuneParamDirLabel;
    GuiLabel detuneParamDirPathLabel;
    GuiTextButton detuneParamDirBrowseBtn;

    GuiLabel unisonParamDirLabel;
    GuiLabel unisonParamDirPathLabel;
    GuiTextButton unisonParamDirBrowseBtn;

    GuiSeparator separator3;

    // Tooltip Visible Switch
    GuiToggleButton tooltipToggle;

    GuiSeparator separator4;

    // For Headroom
    GuiToggleButton useHeadroomToggle;
    GuiSlider headroomGainSlider;

    GuiSeparator separator5;

    // 仮想MIDIキーボード表示制御
    GuiToggleButton virtualMidiKeyboardToggle;

    GuiSeparator separator6;

    // Global Settings I/O
    GuiTextButton saveSettingsBtn;
    GuiTextButton loadSettingsBtn;
    GuiTextButton saveStartupSettingsBtn;

    GuiSeparator separator7;

    // アンドゥ・リドゥ履歴消去
    GuiTextButton clearUndoHistoryBtn;
public:
    GuiSettings(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        uiScaleSelector(context),
        separator1(context),
        wallpaperLabel(context),
        wallpaperPathLabel(context),
        wallpaperBrowseBtn(context),
        wallpaperClearBtn(context),
        wallpaperModeSelector(context),
        separator2(context),
        sampleDirLabel(context),
        sampleDirPathLabel(context),
        sampleDirBrowseBtn(context),
        presetDirLabel(context),
        presetDirPathLabel(context),
        presetDirBrowseBtn(context),
        wavetableDirLabel(context),
        wavetableDirPathLabel(context),
        wavetableDirBrowseBtn(context),
        fxOrderDirLabel(context),
        fxOrderDirPathLabel(context),
        fxOrderDirBrowseBtn(context),
        fxParamDirLabel(context),
        fxParamDirPathLabel(context),
        fxParamDirBrowseBtn(context),
        lfoParamDirLabel(context),
        lfoParamDirPathLabel(context),
        lfoParamDirBrowseBtn(context),
        ampEnvParamDirLabel(context),
        ampEnvParamDirPathLabel(context),
        ampEnvParamDirBrowseBtn(context),
        pitchEnvParamDirLabel(context),
        pitchEnvParamDirPathLabel(context),
        pitchEnvParamDirBrowseBtn(context),
        ssgSwEnvParamDirLabel(context),
        ssgSwEnvParamDirPathLabel(context),
        ssgSwEnvParamDirBrowseBtn(context),
        detuneParamDirLabel(context),
        detuneParamDirPathLabel(context),
        detuneParamDirBrowseBtn(context),
        unisonParamDirLabel(context),
        unisonParamDirPathLabel(context),
        unisonParamDirBrowseBtn(context),
        separator3(context),
        tooltipToggle(context),
        separator4(context),
        useHeadroomToggle(context),
        headroomGainSlider(context),
        separator5(context),
        virtualMidiKeyboardToggle(context),
        separator6(context),
        saveSettingsBtn(context),
        loadSettingsBtn(context),
        saveStartupSettingsBtn(context),
        separator7(context),
        clearUndoHistoryBtn(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void setSettings(
        int uiScaleIndex,
        const juce::String& wallpaperPath,
        const juce::String& sampleDirPath,
        const juce::String& presetDirPath,
        const juce::String& wavetableDirPath,
        const juce::String& fxOrderDirPath,
        const juce::String& fxParamDirPath,
        const juce::String& lfoParamDirPath,
        const juce::String& ampEnvParamDirPath,
        const juce::String& pitchEnvParamDirPath,
        const juce::String& ssgSwEnvParamDirPath,
        const juce::String& detuneParamDirPath,
        const juce::String& unisonParamDirPath
        );
	void setWallpaperPath(const juce::String& wallpaperPath);
    float getUiScale(int index);
};
