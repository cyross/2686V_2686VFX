#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"

class GuiSettings : public GuiBase
{
    juce::Font toggleFont = juce::Font(juce::FontOptions(16.0f));

    GuiGroup mainGroup;

    // 高解像度対応
    GuiComboBox uiScaleSelector;
    GuiComboBox fileFormatSelector;

    NormalSeparator separator1;

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

    NormalSeparator separator2;

    // Directories
    // 17 行あって画面を占めるので、まとめて畳めるようにしている。
    // 見出しはカテゴリラベルを流用するが、板は敷かない。
    GuiCategoryLabel dirCat;
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

    GuiLabel channelParamDirLabel;
    GuiLabel channelParamDirPathLabel;
    GuiTextButton channelParamDirBrowseBtn;

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

    GuiLabel ssgHwEnvParamDirLabel;
    GuiLabel ssgHwEnvParamDirPathLabel;
    GuiTextButton ssgHwEnvParamDirBrowseBtn;

    GuiLabel detuneParamDirLabel;
    GuiLabel detuneParamDirPathLabel;
    GuiTextButton detuneParamDirBrowseBtn;

    GuiLabel unisonParamDirLabel;
    GuiLabel unisonParamDirPathLabel;
    GuiTextButton unisonParamDirBrowseBtn;

    GuiLabel qualityParamDirLabel;
    GuiLabel qualityParamDirPathLabel;
    GuiTextButton qualityParamDirBrowseBtn;

    GuiLabel pcmPlayParamDirLabel;
    GuiLabel pcmPlayParamDirPathLabel;
    GuiTextButton pcmPlayParamDirBrowseBtn;

    GuiLabel toneNoiseParamDirLabel;
    GuiLabel toneNoiseParamDirPathLabel;
    GuiTextButton toneNoiseParamDirBrowseBtn;

    GuiLabel colorSettingDirLabel;
    GuiLabel colorSettingDirPathLabel;
    GuiTextButton colorSettingDirBrowseBtn;

    NormalSeparator separator3;

    // Tooltip Visible Switch
    GuiToggleButton tooltipToggle;

    NormalSeparator separator4;

    // For Headroom
    GuiToggleButton useHeadroomToggle;
    GuiSlider headroomGainSlider;

    NormalSeparator separator5;

    // 仮想MIDIキーボード表示制御
    GuiToggleButton virtualMidiKeyboardToggle;

    NormalSeparator separator6;

    // Global Settings I/O
    GuiTextButton saveSettingsBtn;
    GuiTextButton loadSettingsBtn;
    GuiTextButton saveStartupSettingsBtn;

    NormalSeparator separator7;

    // アンドゥ・リドゥ履歴消去
    GuiTextButton clearUndoHistoryBtn;
public:
    GuiSettings(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        uiScaleSelector(context),
        fileFormatSelector(context),
        separator1(context),
        wallpaperLabel(context),
        wallpaperPathLabel(context),
        wallpaperBrowseBtn(context),
        wallpaperClearBtn(context),
        wallpaperModeSelector(context),
        separator2(context),
        dirCat(context),
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
        channelParamDirLabel(context),
        channelParamDirPathLabel(context),
        channelParamDirBrowseBtn(context),
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
        ssgHwEnvParamDirLabel(context),
        ssgHwEnvParamDirPathLabel(context),
        ssgHwEnvParamDirBrowseBtn(context),
        detuneParamDirLabel(context),
        detuneParamDirPathLabel(context),
        detuneParamDirBrowseBtn(context),
        unisonParamDirLabel(context),
        unisonParamDirPathLabel(context),
        unisonParamDirBrowseBtn(context),
        qualityParamDirLabel(context),
        qualityParamDirPathLabel(context),
        qualityParamDirBrowseBtn(context),
        pcmPlayParamDirLabel(context),
        pcmPlayParamDirPathLabel(context),
        pcmPlayParamDirBrowseBtn(context),
        toneNoiseParamDirLabel(context),
        toneNoiseParamDirPathLabel(context),
        toneNoiseParamDirBrowseBtn(context),
        colorSettingDirLabel(context),
        colorSettingDirPathLabel(context),
        colorSettingDirBrowseBtn(context),
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
    // 設定を画面へ反映する。値はプロセッサから直に読む。
    void setSettings();
	void setWallpaperPath(const juce::String& wallpaperPath);
    float getUiScale(int index);
};
