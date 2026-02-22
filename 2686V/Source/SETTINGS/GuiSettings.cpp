#include "GuiSettings.h"
#include "../editor/PluginEditor.h"
#include "../gui/GuiConstants.h"
#include "../gui/LabelConstants.h"
#include "../fm/OpConstants.h"

void GuiSettings::setup()
{
    mainGroup.setup(*this, "Environment Settings");

    auto setupRow = [&](GuiLabel& lbl, juce::String title, GuiLabel& pathLbl, GuiTextButton& btn, juce::String btnText = "Browse...") {
		lbl.setup({ .parent = *this, .title = title });
		pathLbl.setup({ .parent = *this, .title = emptyFilename });
        pathLbl.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));
        pathLbl.setJustificationType(juce::Justification::centredLeft);
		btn.setup({ .parent = *this, .title = btnText, .isReset = false });
    };

    // --- Wallpaper ---
    setupRow(wallpaperLabel, "Wallpaper:", wallpaperPathLabel, wallpaperBrowseBtn);
    wallpaperPathLabel.setText(ctx.audioProcessor.wallpaperPath, juce::dontSendNotification);
    wallpaperBrowseBtn.onClick = [this] {
        ctx.editor.openFileChooser(
            "Select Wallpaper Image",
            "*.png;*.jpg;*.jpeg",
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    ctx.audioProcessor.wallpaperPath = file.getFullPathName();
                    wallpaperPathLabel.setText(file.getFileName(), juce::dontSendNotification);
                    ctx.editor.loadWallpaperImage();
                }
            }
        );
    };

	wallpaperBrowseBtn.setup({ .parent = *this, .title = "Clear", .isReset = false });
    wallpaperClearBtn.onClick = [this] {
        ctx.audioProcessor.wallpaperPath = "";
        wallpaperPathLabel.setText("(None)", juce::dontSendNotification);
        ctx.editor.loadWallpaperImage();
    };

#if !defined(BUILD_AS_FX_PLUGIN)
    // --- ADPCM Dir ---
    setupRow(sampleDirLabel, "Sample Dir:", sampleDirPathLabel, sampleDirBrowseBtn);
    sampleDirPathLabel.setText(ctx.audioProcessor.defaultSampleDir, juce::dontSendNotification);
    sampleDirBrowseBtn.onClick = [this] {
        ctx.editor.openFileChooser(
            "Select Default Sample Directory",
            ctx.audioProcessor.defaultSampleDir.isEmpty() ? juce::File::getSpecialLocation(juce::File::userHomeDirectory) : juce::File(ctx.audioProcessor.defaultSampleDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultSampleDir = file.getFullPathName();
                    ctx.audioProcessor.lastSampleDirectory = file;
                    sampleDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
    };

    // --- Preset Dir ---
    setupRow(presetDirLabel, "Preset Dir:", presetDirPathLabel, presetDirBrowseBtn);
    presetDirPathLabel.setText(ctx.audioProcessor.defaultPresetDir, juce::dontSendNotification);

    presetDirBrowseBtn.onClick = [this] {
        ctx.editor.openFileChooser(
            "Select Default Preset Directory",
            ctx.audioProcessor.defaultPresetDir.isEmpty() ? juce::File::getSpecialLocation(juce::File::userDocumentsDirectory) : juce::File(ctx.audioProcessor.defaultPresetDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultPresetDir = file.getFullPathName();
                    presetDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);

                    ctx.editor.setPresetDir(file);
                    presetDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                    ctx.editor.scanPresets();
                }
            }
        );
    };

    // --- Toggle Tooltip Visible Toggle Button ---
	tooltipToggle.setup({ .parent = *this, .title = "Show Parameter Range", .isReset = false });
    tooltipToggle.setToggleState(ctx.audioProcessor.showTooltips, juce::dontSendNotification);
    tooltipToggle.onClick = [this] {
        bool newState = tooltipToggle.getToggleState();
        ctx.audioProcessor.showTooltips = newState;
        ctx.editor.setTooltipState(newState); // 即座に反映
        };

	useHeadroomToggle.setup({ .parent = *this, .title = "Use Headroom Settings", .isReset = false });
    useHeadroomToggle.setToggleState(ctx.audioProcessor.useHeadroom, juce::dontSendNotification);
    useHeadroomToggle.onClick = [this] {
        bool state = useHeadroomToggle.getToggleState();
        ctx.audioProcessor.useHeadroom = state;
        headroomGainSlider.setEnabled(state); // OFFならスライダーも無効化
        };

    // --- Headroom Gain Slider---
	headroomGainSlider.setup({ .parent = *this, .title = "Headroom Gain", .isReset = false });
    headroomGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    headroomGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    headroomGainSlider.setRange(0.0, 1.0, 0.01); // 0.0 ~ 1.0
    // プロセッサの値で初期化
    headroomGainSlider.setValue(ctx.audioProcessor.headroomGain, juce::dontSendNotification);
    headroomGainSlider.setEnabled(ctx.audioProcessor.useHeadroom);

    headroomGainSlider.onValueChange = [this] {
        ctx.audioProcessor.headroomGain = (float)headroomGainSlider.getValue();
    };
#endif

    // --- Save Preference Button ---
	saveSettingsBtn.setup({ .parent = *this, .title = "Save Settings", .isReset = false });
    saveSettingsBtn.onClick = [this] {
        ctx.editor.openWriteFileChooser(
            "Save Environment Settings",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("2686V_Config.xml"),
            "*.xml",
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file != juce::File()) {
                    ctx.audioProcessor.saveEnvironment(file);
                }
            }
        );
    };

    // --- Load Preference Button ---
	loadSettingsBtn.setup({ .parent = *this, .title = "Load Settings", .isReset = false });
    loadSettingsBtn.onClick = [this] {
        ctx.editor.openFileChooser(
            "Load Environment Settings",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
            "*.xml",
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    ctx.audioProcessor.loadEnvironment(file);

                    // UI反映
                    wallpaperPathLabel.setText(ctx.audioProcessor.wallpaperPath.isEmpty() ? emptyFilename : juce::File(ctx.audioProcessor.wallpaperPath).getFileName(), juce::dontSendNotification);
                    sampleDirPathLabel.setText(ctx.audioProcessor.defaultSampleDir, juce::dontSendNotification);
                    presetDirPathLabel.setText(ctx.audioProcessor.defaultPresetDir, juce::dontSendNotification);

                    // 壁紙再描画
                    ctx.editor.loadWallpaperImage();

#if !defined(BUILD_AS_FX_PLUGIN)
                    // プリセットリスト更新
                    if (juce::File(ctx.audioProcessor.defaultPresetDir).isDirectory()) {
                        ctx.editor.setPresetDir(juce::File(ctx.audioProcessor.defaultPresetDir));
                        ctx.editor.updatePresetPath();
                        ctx.editor.scanPresets(); // リスト更新関数を呼ぶ
                    }
#endif
                }
            }

        );
    };

    saveStartupSettingsBtn.setup({ .parent = *this, .title = "Save Current Settings as Default", .isReset = false });
    saveStartupSettingsBtn.onClick = [this]
        {
            auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
            auto pluginDir = docDir.getChildFile(assetFolder);

            // フォルダがなければ作る
            if (!pluginDir.exists()) pluginDir.createDirectory();

            auto file = pluginDir.getChildFile(defaultSettingFilename);

            // 2. XMLデータの作成
            juce::XmlElement xml(envCode);

            xml.setAttribute(settingWallpaperPath, ctx.audioProcessor.wallpaperPath);
            xml.setAttribute(settingDefaultSampleDir, ctx.audioProcessor.defaultSampleDir);
            xml.setAttribute(settingDefaultPresetDir, ctx.audioProcessor.defaultPresetDir);
            xml.setAttribute(settingShowTooltips, ctx.audioProcessor.showTooltips);
            xml.setAttribute(settingUseHeadroom, ctx.audioProcessor.useHeadroom);
            xml.setAttribute(settingHeadroomGain, ctx.audioProcessor.headroomGain);

            // 3. 書き出し実行
            if (xml.writeTo(file))
            {
                // 成功メッセージ
                juce::NativeMessageBox::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "Success",
                    "Current settings have been saved as the default startup configuration.\n\nFile: " + file.getFullPathName(),
                    this
                );
            }
            else
            {
                // 失敗メッセージ
                juce::NativeMessageBox::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Error",
                    "Failed to save startup settings file.",
                    this
                );
            }
        };
}

void GuiSettings::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    mainGroup.setBounds(pageArea);

    auto sRect = pageArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    sRect.removeFromTop(TitlePaddingTop);

    // 1. Wallpaper
    auto row1 = sRect.removeFromTop(SettingsRowHeight);
    wallpaperLabel.setBounds(row1.removeFromLeft(SettingsLabelWidth));
    wallpaperClearBtn.setBounds(row1.removeFromRight(SettingsClearButtonWidth));
    wallpaperBrowseBtn.setBounds(row1.removeFromRight(SettingsBrowseButtonWidth));
    wallpaperPathLabel.setBounds(row1);

    sRect.removeFromTop(SettingsPaddingHeight);

    // 2. ADPCM Dir
    auto row2 = sRect.removeFromTop(SettingsRowHeight);
    sampleDirLabel.setBounds(row2.removeFromLeft(SettingsLabelWidth));
    sampleDirBrowseBtn.setBounds(row2.removeFromRight(SettingsBrowseButtonWidth));
    sampleDirPathLabel.setBounds(row2);

    sRect.removeFromTop(SettingsPaddingHeight);

    // 3. Preset Dir
    auto row3 = sRect.removeFromTop(SettingsRowHeight);
    presetDirLabel.setBounds(row3.removeFromLeft(SettingsLabelWidth));
    presetDirBrowseBtn.setBounds(row3.removeFromRight(SettingsBrowseButtonWidth));
    presetDirPathLabel.setBounds(row3);

    sRect.removeFromTop(SettingsPaddingHeight);

    // 4. Tooltip Visible Row
    auto rowTooltip = sRect.removeFromTop(SettingsRowHeight);
    tooltipToggle.setBounds(rowTooltip.removeFromLeft(SettingsToggleWidth));

    sRect.removeFromTop(SettingsPaddingHeight);

    // Headroom Row
    auto rowHeadroom = sRect.removeFromTop(SettingsRowHeight);
    useHeadroomToggle.setBounds(rowHeadroom.removeFromLeft(SettingsToggleWidth));
    headroomGainSlider.setBounds(rowHeadroom.removeFromLeft(SettingsHeadroomGainSliderWidth));

    sRect.removeFromTop(SettingsPaddingHeight);

    // 5. Config IO Buttons (Fixed Layout)
    auto row4 = sRect.removeFromTop(SettingsRowHeight);

    layoutComponentsLtoR(row4, OpRowHeight, OpLastRowPaddingBottom, {
        { &loadSettingsBtn, { SettingsButtonWidth, SettingsButtonPaddingRight} },
        { &saveSettingsBtn, { SettingsButtonWidth, SettingsButtonPaddingRight} },
        { &saveStartupSettingsBtn, { SettingsButtonWidth, 0} }
        });
}

void GuiSettings::setSettings(const juce::String& wallpaperPath, const juce::String& sampleDirPath, const juce::String& presetDirPath)
{
    wallpaperPathLabel.setText(wallpaperPath, juce::dontSendNotification);
    sampleDirPathLabel.setText(sampleDirPath, juce::dontSendNotification);
    presetDirPathLabel.setText(presetDirPath, juce::dontSendNotification);
}

void GuiSettings::setWallpaperPath(const juce::String& wallpaperPath)
{
    wallpaperPathLabel.setText(wallpaperPath, juce::dontSendNotification);
}