#include <array>
#include <vector>

#include "./GuiSettings.h"

#include "../../Core/Editor/PluginEditor.h"
#include "../../Core/Gui/GuiColor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "./SettingsKeys.h"
#include "./SettingsValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiSettingsValues.h"
#include "./GuiSettingsText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiSettingsHelpers.h"

static std::vector<SelectItem> uiScaleItems = {
    {.name = "25%",  .value = 1 },
    {.name = "50%",  .value = 2 },
    {.name = "75%",  .value = 3 },
    {.name = "100%", .value = 4 },
    {.name = "125%", .value = 5 },
    {.name = "150%", .value = 6 },
    {.name = "175%", .value = 7 },
    {.name = "200%", .value = 8 },
    {.name = "250%", .value = 9 },
    {.name = "300%", .value = 10 }
};

static std::array<float, 10> uiScaleLUT = {
    0.25f, 0.5f, 0.75f, 1.00f, 1.25f, 1.50f, 1.75f, 2.00f, 2.50f, 3.00f
};

void GuiSettings::setup()
{
    std::vector<SelectItem> wpModeItems = {
        {.name = juce::String("") + "Stretch(画面アスペクト比維持・画面全体カバー)", .value = 1 },
        {.name = juce::String("") + "Fill(画像アスペクト比維持・画面全体カバー)",  .value = 2 },
        {.name = juce::String("") + "Fit(画像アスペクト比準拠・見切りあり)",  .value = 3 },
        {.name = juce::String("") + "Original(入力画像そのまま・センタリング)",   .value = 4 },
    };

    int tabOrder = 1;
    float separatorThick = 3.0f;

    mainGroup.setup(*this, SettingsGuiText::Group::settingEnv);

    // UI拡大率
    uiScaleSelector.setup({
        .parent = *this,
        .id = "",
        .title = juce::String("") + "UIスケール",
        .items = uiScaleItems,
        .isReset = false,
        .labelColor = juce::Colours::yellow
        });
    uiScaleSelector.setSelectedId(ctx.audioProcessor.uiScaleIndex + 1, juce::dontSendNotification);
    uiScaleSelector.setWantsKeyboardFocus(true);
    uiScaleSelector.setExplicitFocusOrder(++tabOrder);
    uiScaleSelector.onChange = [this] {
        int index = uiScaleSelector.getSelectedItemIndex();

        ctx.audioProcessor.uiScaleIndex = index;
        ctx.editor.updateUiScale(uiScaleLUT[index]);
        ctx.editor.resized();

        };

    addAndMakeVisible(separator1);
    separator1.setup({ .lineThick = separatorThick, .lineColour = juce::Colours::grey });

    auto setupRow = [&](GuiLabel& lbl, juce::String title, GuiLabel& pathLbl, GuiTextButton& btn, juce::String btnText = juce::String("") + "ファイル選択") {
		lbl.setup({ .parent = *this, .title = title });
		pathLbl.setup({ .parent = *this, .title = Io::empty });
        pathLbl.setColour(juce::Label::outlineColourId, juce::Colours::white);
        pathLbl.setJustificationType(juce::Justification::centredLeft);
		btn.setup({ .parent = *this, .title = btnText, .isReset = false });
    };

    // --- Wallpaper Path ---
    setupRow(wallpaperLabel, juce::String("") + "壁紙:", wallpaperPathLabel, wallpaperBrowseBtn);
    wallpaperPathLabel.setText(ctx.audioProcessor.wallpaperPath, juce::dontSendNotification);
    wallpaperPathLabel.setWantsKeyboardFocus(false);
    wallpaperBrowseBtn.setWantsKeyboardFocus(true);
    wallpaperBrowseBtn.setExplicitFocusOrder(++tabOrder);
    wallpaperBrowseBtn.onClick = [this] {
        ctx.editor.openFileChooser(
            juce::String("") + "壁紙画像ファイルを選択してください",
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
    
	wallpaperClearBtn.setup({ .parent = *this, .title = juce::String("") + "解除", .bgColor = juce::Colours::red.withAlpha(0.5f), .isReset = false });
    wallpaperClearBtn.setWantsKeyboardFocus(true);
    wallpaperClearBtn.setExplicitFocusOrder(++tabOrder);
    wallpaperClearBtn.onClick = [this] {
        ctx.audioProcessor.wallpaperPath = "";
        wallpaperPathLabel.setText(Io::empty, juce::dontSendNotification);
        ctx.editor.loadWallpaperImage();
    };

    // --- Wallpaper Mode ---
    wallpaperModeSelector.setup({ .parent = *this, .title = juce::String("") + "壁紙表示スケール:", .items = wpModeItems, .isReset = false });
    wallpaperModeSelector.setSelectedId(ctx.audioProcessor.wallpaperMode + 1, juce::dontSendNotification);
    wallpaperModeSelector.setWantsKeyboardFocus(true);
    wallpaperModeSelector.setExplicitFocusOrder(++tabOrder);
    wallpaperModeSelector.onChange = [this] {
        ctx.audioProcessor.wallpaperMode = wallpaperModeSelector.getSelectedId() - 1;
        ctx.editor.repaint(); // Editor全体の再描画を呼び出す
    };

    addAndMakeVisible(separator2);
    separator2.setup({ .lineThick = separatorThick, .lineColour = juce::Colours::grey });

    // --- ADPCM Dir ---
    setupRow(sampleDirLabel, juce::String("") + "サンプルファイルディレクトリ:", sampleDirPathLabel, sampleDirBrowseBtn);
    sampleDirPathLabel.setText(ctx.audioProcessor.defaultSampleDir, juce::dontSendNotification);
    sampleDirPathLabel.setWantsKeyboardFocus(false);
    sampleDirBrowseBtn.setWantsKeyboardFocus(true);
    sampleDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    sampleDirBrowseBtn.onClick = [this] {
        ctx.editor.openFileChooser(
            juce::String("") + "サンプルファイルディレクトリを選択してください",
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
    setupRow(presetDirLabel, juce::String("") + "プリセットファイルディレクトリ:", presetDirPathLabel, presetDirBrowseBtn);
    presetDirPathLabel.setText(ctx.audioProcessor.defaultPresetDir, juce::dontSendNotification);
    presetDirPathLabel.setWantsKeyboardFocus(false);

    presetDirBrowseBtn.setWantsKeyboardFocus(true);
    presetDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    presetDirBrowseBtn.onClick = [this] {
        ctx.editor.openFileChooser(
            juce::String("") + "プリセットファイルディレクトリを選択してください",
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

    // --- Wavetable Dir ---
    setupRow(wavetableDirLabel, juce::String("") + "波形メモリファイルディレクトリ:", wavetableDirPathLabel, wavetableDirBrowseBtn);
    wavetableDirPathLabel.setText(ctx.audioProcessor.defaultWavetableDir, juce::dontSendNotification);
    wavetableDirPathLabel.setWantsKeyboardFocus(false);

    wavetableDirBrowseBtn.setWantsKeyboardFocus(true);
    wavetableDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    wavetableDirBrowseBtn.onClick = [this] {
        ctx.editor.openFileChooser(
            juce::String("") + "波形メモリファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultWavetableDir.isEmpty() ? juce::File::getSpecialLocation(juce::File::userDocumentsDirectory) : juce::File(ctx.audioProcessor.defaultWavetableDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultWavetableDir = file.getFullPathName();
                    wavetableDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
    };

    addAndMakeVisible(separator3);
    separator3.setup({ .lineThick = separatorThick, .lineColour = juce::Colours::grey });

    // --- Toggle Tooltip Visible Toggle Button ---
    tooltipToggle.setup({ .parent = *this, .title = juce::String("") + "ツールチップを表示", .font = toggleFont, .isReset = false });
    tooltipToggle.setToggleState(ctx.audioProcessor.showTooltips, juce::dontSendNotification);
    tooltipToggle.setWantsKeyboardFocus(true);
    tooltipToggle.setExplicitFocusOrder(++tabOrder);
    tooltipToggle.onClick = [this] {
        bool newState = tooltipToggle.getToggleState();
        ctx.audioProcessor.showTooltips = newState;
        ctx.editor.setTooltipState(newState); // 即座に反映
        };

    addAndMakeVisible(separator4);
    separator4.setup({ .lineThick = separatorThick, .lineColour = juce::Colours::grey });

    useHeadroomToggle.setup({ .parent = *this, .title = juce::String("") + "ヘッドルームを確保", .font = toggleFont, .isReset = false });
    useHeadroomToggle.setToggleState(ctx.audioProcessor.useHeadroom, juce::dontSendNotification);
    useHeadroomToggle.setWantsKeyboardFocus(true);
    useHeadroomToggle.setExplicitFocusOrder(++tabOrder);
    useHeadroomToggle.onClick = [this] {
        bool state = useHeadroomToggle.getToggleState();
        ctx.audioProcessor.useHeadroom = state;
        headroomGainSlider.setEnabledWithLabel(state); // OFFならスライダーも無効化
        };

    // --- Headroom Gain Slider---
    headroomGainSlider.setup({ .parent = *this, .title = juce::String("") + "ヘッドルームゲイン", .isReset = false });
    headroomGainSlider.setWantsKeyboardFocus(true);
    headroomGainSlider.setExplicitFocusOrder(++tabOrder);
    headroomGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    headroomGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    headroomGainSlider.setRange(0.0, 1.0, 0.01); // 0.0 ~ 1.0
    // プロセッサの値で初期化
    headroomGainSlider.setValue(ctx.audioProcessor.headroomGain, juce::dontSendNotification);
    headroomGainSlider.setEnabledWithLabel(ctx.audioProcessor.useHeadroom);

    headroomGainSlider.onValueChange = [this] {
        ctx.audioProcessor.headroomGain = (float)headroomGainSlider.getValue();
        };

    addAndMakeVisible(separator5);
    separator5.setup({ .lineThick = separatorThick, .lineColour = juce::Colours::grey });

    virtualMidiKeyboardToggle.setup({ .parent = *this, .title = juce::String("") + "仮想MIDIキーボード表示", .font = toggleFont , .isReset = false });
    virtualMidiKeyboardToggle.setWantsKeyboardFocus(true);
    virtualMidiKeyboardToggle.setExplicitFocusOrder(++tabOrder);
    virtualMidiKeyboardToggle.setToggleState(ctx.audioProcessor.showVirtualKeyboard, juce::dontSendNotification);
    virtualMidiKeyboardToggle.onClick = [this] {
        ctx.audioProcessor.showVirtualKeyboard = !ctx.audioProcessor.showVirtualKeyboard;

        ctx.editor.updateKeyboardVisibility();
        };

    addAndMakeVisible(separator6);
    separator6.setup({ .lineThick = separatorThick, .lineColour = juce::Colours::grey });

    // --- Save Preference Button ---
    saveSettingsBtn.setup({ .parent = *this, .title = juce::String("") + "設定ファイルに保存", .isReset = false });
    saveSettingsBtn.setWantsKeyboardFocus(true);
    saveSettingsBtn.setExplicitFocusOrder(++tabOrder);
    saveSettingsBtn.onClick = [this] {
        ctx.editor.openWriteFileChooser(
            juce::String("") + "設定ファイルを選択してください",
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
    loadSettingsBtn.setup({ .parent = *this, .title = juce::String("") + "設定ファイルから読み込み", .isReset = false });
    loadSettingsBtn.setWantsKeyboardFocus(true);
    loadSettingsBtn.setExplicitFocusOrder(++tabOrder);
    loadSettingsBtn.onClick = [this] {
        ctx.editor.openFileChooser(
            juce::String("") + "設定ファイルを選択してください",
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

                    // プリセットリスト更新
                    if (juce::File(ctx.audioProcessor.defaultPresetDir).isDirectory()) {
                        ctx.editor.setPresetDir(juce::File(ctx.audioProcessor.defaultPresetDir));
                        ctx.editor.updatePresetPath();
                        ctx.editor.scanPresets(); // リスト更新関数を呼ぶ
                    }

                    // UIスケール反映
                    ctx.editor.updateUiScale(ctx.audioProcessor.uiScaleIndex);
                }
            }

        );
        };

    saveStartupSettingsBtn.setup({ .parent = *this, .title = juce::String("") + "標準設定として保存", .bgColor = GuiColor::Settings::SaveAsDefaultBtnBg, .isReset = false });
    saveStartupSettingsBtn.setWantsKeyboardFocus(true);
    saveStartupSettingsBtn.setExplicitFocusOrder(++tabOrder);
    saveStartupSettingsBtn.onClick = [this]
        {
            auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
            auto pluginDir = docDir.getChildFile(Io::Folder::asset);

            // フォルダがなければ作る
            if (!pluginDir.exists()) pluginDir.createDirectory();

            // 初期設定ファイル名を指定
            auto file = pluginDir.getChildFile(SettingsValue::File::Name::initial);

            // 2. XMLデータの作成
            juce::XmlElement xml(SettingsKey::envCode);

            xml.setAttribute(SettingsKey::uiScaleIndex, ctx.audioProcessor.uiScaleIndex);
            xml.setAttribute(SettingsKey::wallpaperPath, ctx.audioProcessor.wallpaperPath);
            xml.setAttribute(SettingsKey::wallpaperMode, ctx.audioProcessor.wallpaperMode);
            xml.setAttribute(SettingsKey::defaultSampleDir, ctx.audioProcessor.defaultSampleDir);
            xml.setAttribute(SettingsKey::defaultPresetDir, ctx.audioProcessor.defaultPresetDir);
            xml.setAttribute(SettingsKey::showTooltips, ctx.audioProcessor.showTooltips);
            xml.setAttribute(SettingsKey::useHeadroom, ctx.audioProcessor.useHeadroom);
            xml.setAttribute(SettingsKey::headroomGain, ctx.audioProcessor.headroomGain);
            xml.setAttribute(SettingsKey::showVirtualKeyboard, ctx.audioProcessor.showVirtualKeyboard);

            // 3. 書き出し実行
            if (xml.writeTo(file))
            {
                // 成功メッセージ
                juce::NativeMessageBox::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    juce::String("") + "成功",
                    juce::String("") + "現在の設定は標準設定として以下のファイルに保存されました。\n\nファイル名: " + file.getFullPathName(),
                    this
                );
            }
            else
            {
                // 失敗メッセージ
                juce::NativeMessageBox::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    juce::String("") + "失敗",
                    juce::String("") + "標準設定の保存に失敗しました。",
                    this
                );
            }
        };

    addAndMakeVisible(separator7);
    separator7.setup({ .lineThick = separatorThick, .lineColour = juce::Colours::grey });

    // --- Clear Undo/Redo History Button ---
    clearUndoHistoryBtn.setup({ .parent = *this, .title = juce::String("") + "アンドゥ・リドゥ履歴の初期化", .bgColor = juce::Colours::blue.darker(0.3f).withAlpha(0.3f), .isReset = false});
    clearUndoHistoryBtn.setWantsKeyboardFocus(true);
    clearUndoHistoryBtn.setExplicitFocusOrder(++tabOrder);
    clearUndoHistoryBtn.onClick = [this] {
        ctx.audioProcessor.undoManager.clearUndoHistory();
        };
}

void GuiSettings::layout(juce::Rectangle<int> content)
{
    int separatorHeight = 20;
    auto pageArea = content.withZeroOrigin();

    mainGroup.setBounds(pageArea);

    auto sRect = pageArea.reduced(SettingsGuiValue::Group::Padding::width, SettingsGuiValue::Group::Padding::height);
    sRect.removeFromTop(SettingsGuiValue::Group::TitlePaddingTop);

    // 1. UI Scale
    auto rowUiScale = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    uiScaleSelector.label.setBounds(rowUiScale.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
    uiScaleSelector.setBounds(rowUiScale.removeFromLeft(SettingsGuiValue::Settings::UiScaleSelectorWidth));

    auto sp1Rect = sRect.removeFromTop(separatorHeight);
    separator1.setBounds(sp1Rect);

    // 2. WallpaperPath
    auto rowWpPath = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    wallpaperLabel.setBounds(rowWpPath.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
    wallpaperClearBtn.setBounds(rowWpPath.removeFromRight(SettingsGuiValue::Settings::ClearButtonWidth));
    wallpaperBrowseBtn.setBounds(rowWpPath.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
    wallpaperPathLabel.setBounds(rowWpPath);

    sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

    // 3. WallpaperMode
    auto rowWpMode = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    wallpaperModeSelector.label.setBounds(rowWpMode.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
    wallpaperModeSelector.setBounds(rowWpMode.removeFromLeft(SettingsGuiValue::Settings::ModeSelectorWidth));

    auto sp2Rect = sRect.removeFromTop(separatorHeight);
    separator2.setBounds(sp2Rect);

    // 4. ADPCM Dir
    auto rowAdpcmDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    sampleDirLabel.setBounds(rowAdpcmDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
    sampleDirBrowseBtn.setBounds(rowAdpcmDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
    sampleDirPathLabel.setBounds(rowAdpcmDir);

    sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

    // 5. Preset Dir
    auto rowPresetDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    presetDirLabel.setBounds(rowPresetDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
    presetDirBrowseBtn.setBounds(rowPresetDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
    presetDirPathLabel.setBounds(rowPresetDir);

    sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

    // 6. Wavetable Dir
    auto rowWavetableDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    wavetableDirLabel.setBounds(rowWavetableDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
    wavetableDirBrowseBtn.setBounds(rowWavetableDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
    wavetableDirPathLabel.setBounds(rowWavetableDir);

    auto sp3Rect = sRect.removeFromTop(separatorHeight);
    separator3.setBounds(sp3Rect);

    // 7. Tooltip Visible Row
    auto rowTooltip = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    tooltipToggle.setBounds(rowTooltip.removeFromLeft(SettingsGuiValue::Settings::ToggleWidth));

    auto sp4Rect = sRect.removeFromTop(separatorHeight);
    separator4.setBounds(sp4Rect);

    // 8. Headroom Row
    auto rowHeadroom = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    useHeadroomToggle.setBounds(rowHeadroom.removeFromLeft(SettingsGuiValue::Settings::ToggleWidth));

    sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

    // 9. Headroom Gain Row
    auto rowHeadroomGain = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    headroomGainSlider.label.setBounds(rowHeadroomGain.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
    headroomGainSlider.setBounds(rowHeadroomGain.removeFromLeft(SettingsGuiValue::Settings::HeadroomGainSliderWidth));

    auto sp5Rect = sRect.removeFromTop(separatorHeight);
    separator5.setBounds(sp5Rect);

    // 10. Virtual Keyboard Row
    auto rowVirtualMidiKeyboard = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    virtualMidiKeyboardToggle.setBounds(rowVirtualMidiKeyboard.removeFromLeft(SettingsGuiValue::Settings::ToggleWidth));

    auto sp6Rect = sRect.removeFromTop(separatorHeight);
    separator6.setBounds(sp6Rect);

    // 11. Config IO Buttons (Fixed Layout)
    auto rowIoBtns = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);

    layoutRowSettingsIo({ .rect = rowIoBtns, .loadSettingsBtn = &loadSettingsBtn, .saveSettingsBtn = &saveSettingsBtn, .saveStartupSettingsBtn = &saveStartupSettingsBtn, .rowHeight = SettingsGuiValue::Settings::RowHeight });

    auto sp7Rect = sRect.removeFromTop(separatorHeight);
    separator7.setBounds(sp7Rect);

    // 12. Clear Undo/Redo History Button
    auto rowClearHistoryBtns = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    layoutRow({ .rowRect = rowClearHistoryBtns, .component = &clearUndoHistoryBtn, .rowHeight = SettingsGuiValue::Settings::RowHeight});
}

void GuiSettings::setSettings(int uiScaleIndex, const juce::String& wallpaperPath, const juce::String& sampleDirPath, const juce::String& presetDirPath, const juce::String& wavetableDirPath)
{
    uiScaleSelector.setSelectedId(uiScaleIndex + 1, juce::dontSendNotification);
    wallpaperPathLabel.setText(wallpaperPath, juce::dontSendNotification);
    sampleDirPathLabel.setText(sampleDirPath, juce::dontSendNotification);
    presetDirPathLabel.setText(presetDirPath, juce::dontSendNotification);
	wavetableDirLabel.setText(wavetableDirPath, juce::dontSendNotification);
}

void GuiSettings::setWallpaperPath(const juce::String& wallpaperPath)
{
    wallpaperPathLabel.setText(wallpaperPath, juce::dontSendNotification);
}

float GuiSettings::getUiScale(int index) {
    return uiScaleLUT[index];
}
