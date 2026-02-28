#include "GuiSettings.h"
#include "../editor/PluginEditor.h"
#include "../gui/GuiColor.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/FileValues.h"
#include "../core/PrKeys.h"
#include "../core/SettingsKeys.h"
#include "../core/SettingsValues.h"

void GuiSettings::setup()
{
    std::vector<SelectItem> wpModeItems = {
        {.name = "Stretch", .value = 1 },
        {.name = "Fill",  .value = 2 },
        {.name = "Fit",  .value = 3 },
        {.name = "Original",   .value = 4 },
    };

    mainGroup.setup(*this, "Environment Settings");

    auto setupRow = [&](GuiLabel& lbl, juce::String title, GuiLabel& pathLbl, GuiTextButton& btn, juce::String btnText = "Browse...") {
		lbl.setup({ .parent = *this, .title = title });
		pathLbl.setup({ .parent = *this, .title = Io::empty });
        pathLbl.setColour(juce::Label::outlineColourId, juce::Colours::white);
        pathLbl.setJustificationType(juce::Justification::centredLeft);
		btn.setup({ .parent = *this, .title = btnText, .isReset = false });
    };

    // --- Wallpaper Path ---
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
    
	wallpaperClearBtn.setup({ .parent = *this, .title = "Clear", .isReset = false });
    wallpaperClearBtn.onClick = [this] {
        ctx.audioProcessor.wallpaperPath = "";
        wallpaperPathLabel.setText(Io::empty, juce::dontSendNotification);
        ctx.editor.loadWallpaperImage();
    };

    // --- Wallpaper Mode ---
    wallpaperModeSelector.setup({ .parent = *this, .title = "Mode:", .items = wpModeItems, .isReset = false });
    wallpaperModeSelector.setSelectedId(ctx.audioProcessor.wallpaperMode + 1, juce::dontSendNotification);
    wallpaperModeSelector.onChange = [this] {
        ctx.audioProcessor.wallpaperMode = wallpaperModeSelector.getSelectedId() - 1;
        ctx.editor.repaint(); // Editor全体の再描画を呼び出す
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
    headroomGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    headroomGainSlider.setRange(0.0, 1.0, 0.01); // 0.0 ~ 1.0
    // プロセッサの値で初期化
    headroomGainSlider.setValue(ctx.audioProcessor.headroomGain, juce::dontSendNotification);
    headroomGainSlider.setEnabled(ctx.audioProcessor.useHeadroom);

    headroomGainSlider.onValueChange = [this] {
        ctx.audioProcessor.headroomGain = (float)headroomGainSlider.getValue();
    };

    virtualMidiKeyboardToggle.setup({ .parent = *this, .title = "Show Virtual Midi Keyboard", .isReset = false });
    virtualMidiKeyboardToggle.setToggleState(ctx.audioProcessor.showVirtualKeyboard, juce::dontSendNotification);
    virtualMidiKeyboardToggle.onClick = [this] {
        ctx.audioProcessor.showVirtualKeyboard = !ctx.audioProcessor.showVirtualKeyboard;

        ctx.editor.updateKeyboardVisibility();
    };
#endif

    // --- Save Preference Button ---
	saveSettingsBtn.setup({ .parent = *this, .title = "Save Settings", .isReset = false });
    saveSettingsBtn.onClick = [this] {
        ctx.editor.openWriteFileChooser(
            "Save Environment Settings",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile(SettingsValue::File::Name::initial),
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
                    wallpaperPathLabel.setText(ctx.audioProcessor.wallpaperPath.isEmpty() ? Io::empty : juce::File(ctx.audioProcessor.wallpaperPath).getFileName(), juce::dontSendNotification);
                    wallpaperModeSelector.setSelectedId(ctx.audioProcessor.wallpaperMode + 1);
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

    saveStartupSettingsBtn.setup({ .parent = *this, .title = "Save Current Settings as Default", .bgColor = GuiColor::Settings::SaveAsDefaultBtnBg, .isReset = false });
    saveStartupSettingsBtn.onClick = [this]
        {
            auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
            auto pluginDir = docDir.getChildFile(Io::Folder::asset);

            // フォルダがなければ作る
            if (!pluginDir.exists()) pluginDir.createDirectory();

            // 初期設定ファイル名を指定
            auto file = pluginDir.getChildFile(SettingsValue::File::Name::def);

            // 2. XMLデータの作成
            juce::XmlElement xml(SettingsKey::envCode);

            xml.setAttribute(SettingsKey::wallpaperPath, ctx.audioProcessor.wallpaperPath);
            xml.setAttribute(SettingsKey::wallpaperMode, ctx.audioProcessor.wallpaperMode);
            xml.setAttribute(SettingsKey::defaultSampleDir, ctx.audioProcessor.defaultSampleDir);
            xml.setAttribute(SettingsKey::defaultPresetDir, ctx.audioProcessor.defaultPresetDir);
            xml.setAttribute(SettingsKey::showTooltips, ctx.audioProcessor.showTooltips);
            xml.setAttribute(SettingsKey::useHeadroom , ctx.audioProcessor.useHeadroom);
            xml.setAttribute(SettingsKey::headroomGain, ctx.audioProcessor.headroomGain);
            xml.setAttribute(SettingsKey::showVirtualKeyboard, ctx.audioProcessor.showVirtualKeyboard);

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

    auto sRect = pageArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    sRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    // 1. WallpaperPath
    auto rowWpPath = sRect.removeFromTop(GuiValue::Settings::RowHeight);
    wallpaperLabel.setBounds(rowWpPath.removeFromLeft(GuiValue::Settings::LabelWidth));
    wallpaperClearBtn.setBounds(rowWpPath.removeFromRight(GuiValue::Settings::ClearButtonWidth));
    wallpaperBrowseBtn.setBounds(rowWpPath.removeFromRight(GuiValue::Settings::BrowseButtonWidth));
    wallpaperPathLabel.setBounds(rowWpPath);

    sRect.removeFromTop(GuiValue::Settings::PaddingHeight);

    // 2. WallpaperMode
    auto rowWpMode = sRect.removeFromTop(GuiValue::Settings::RowHeight);
    wallpaperModeSelector.label.setBounds(rowWpMode.removeFromLeft(GuiValue::Settings::LabelWidth));
    wallpaperModeSelector.setBounds(rowWpMode.removeFromLeft(GuiValue::Settings::ModeSelectorWidth));

    sRect.removeFromTop(GuiValue::Settings::PaddingHeight);

    // 3. ADPCM Dir
    auto rowAdpcmDir = sRect.removeFromTop(GuiValue::Settings::RowHeight);
    sampleDirLabel.setBounds(rowAdpcmDir.removeFromLeft(GuiValue::Settings::LabelWidth));
    sampleDirBrowseBtn.setBounds(rowAdpcmDir.removeFromRight(GuiValue::Settings::BrowseButtonWidth));
    sampleDirPathLabel.setBounds(rowAdpcmDir);

    sRect.removeFromTop(GuiValue::Settings::PaddingHeight);

    // 4. Preset Dir
    auto rowPresetDir = sRect.removeFromTop(GuiValue::Settings::RowHeight);
    presetDirLabel.setBounds(rowPresetDir.removeFromLeft(GuiValue::Settings::LabelWidth));
    presetDirBrowseBtn.setBounds(rowPresetDir.removeFromRight(GuiValue::Settings::BrowseButtonWidth));
    presetDirPathLabel.setBounds(rowPresetDir);

    sRect.removeFromTop(GuiValue::Settings::PaddingHeight);

    // 5. Tooltip Visible Row
    auto rowTooltip = sRect.removeFromTop(GuiValue::Settings::RowHeight);
    tooltipToggle.setBounds(rowTooltip.removeFromLeft(GuiValue::Settings::ToggleWidth));

    sRect.removeFromTop(GuiValue::Settings::PaddingHeight);

    // 6. Headroom Row
    auto rowHeadroom = sRect.removeFromTop(GuiValue::Settings::RowHeight);
    useHeadroomToggle.setBounds(rowHeadroom.removeFromLeft(GuiValue::Settings::ToggleWidth));

    sRect.removeFromTop(GuiValue::Settings::PaddingHeight);

    // 7. Headroom Row
    auto rowHeadroomGain = sRect.removeFromTop(GuiValue::Settings::RowHeight);
    headroomGainSlider.label.setBounds(rowHeadroomGain.removeFromLeft(GuiValue::Settings::LabelWidth));
    headroomGainSlider.setBounds(rowHeadroomGain.removeFromLeft(GuiValue::Settings::HeadroomGainSliderWidth));

    sRect.removeFromTop(GuiValue::Settings::PaddingHeight);

    // 8. Virtual Keyboard Row
    auto rowVirtualMidiKeyboard = sRect.removeFromTop(GuiValue::Settings::RowHeight);
    virtualMidiKeyboardToggle.setBounds(rowVirtualMidiKeyboard.removeFromLeft(GuiValue::Settings::ToggleWidth));

    sRect.removeFromTop(GuiValue::Settings::PaddingHeight);

    // 9. Config IO Buttons (Fixed Layout)
    auto rowIoBtns = sRect.removeFromTop(GuiValue::Settings::RowHeight);

    layoutComponentsLtoR(rowIoBtns, GuiValue::Fm::Op::Row::height, 0, {
        { &loadSettingsBtn, { GuiValue::Settings::ButtonWidth, GuiValue::Settings::ButtonPaddingRight} },
        { &saveSettingsBtn, { GuiValue::Settings::ButtonWidth, GuiValue::Settings::ButtonPaddingRight} },
        { &saveStartupSettingsBtn, { GuiValue::Settings::ButtonWidth, 0} }
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