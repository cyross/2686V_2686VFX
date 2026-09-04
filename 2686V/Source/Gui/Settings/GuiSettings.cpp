#include <array>
#include <algorithm>
#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
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
    {.name = "30%",  .value = 2 },
    {.name = "40%",  .value = 3 },
    {.name = "50%",  .value = 4 },
    {.name = "60%",  .value = 5 },
    {.name = "70%",  .value = 6 },
    {.name = "75%",  .value = 7 },
    {.name = "80%",  .value = 8 },
    {.name = "90%",  .value = 9 },
    {.name = "100%", .value = 10 },
    {.name = "125%", .value = 11 },
    {.name = "150%", .value = 12 },
    {.name = "175%", .value = 13 },
    {.name = "200%", .value = 14 },
    {.name = "250%", .value = 15 },
    {.name = "300%", .value = 16 }
};

static std::array<float, 16> uiScaleLUT = {
    0.25f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.75f, 0.8f, 0.9f, 1.00f, 1.25f, 1.50f, 1.75f, 2.00f, 2.50f, 3.00f
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

    // パラメータファイルの形
    //
    // 読み込みは中身を見て振り分けるので、ここを変えても今までに
    // 書き出したファイルはそのまま読める。変わるのは書き出す形と
    // 拡張子だけ。
    std::vector<SelectItem> fileFormatItems = {
        {.name = "JSON", .value = 1 },
        {.name = "YAML", .value = 2 },
    };

    fileFormatSelector.setup({
        .parent = *this,
        .id = "",
        .title = juce::String("") + "ファイル形式",
        .items = fileFormatItems,
        .isReset = false,
        .labelColor = juce::Colours::yellow
        });
    fileFormatSelector.setSelectedId(ctx.audioProcessor.fileFormatIndex + 1, juce::dontSendNotification);
    fileFormatSelector.setWantsKeyboardFocus(true);
    fileFormatSelector.setExplicitFocusOrder(++tabOrder);
    fileFormatSelector.onChange = [this] {
        ctx.audioProcessor.fileFormatIndex = fileFormatSelector.getSelectedItemIndex();
        ctx.audioProcessor.applyFileFormat();
        };

    separator1.setupComponent(*this);

    auto setupRow = [&](GuiLabel& lbl, juce::String title, GuiLabel& pathLbl, GuiTextButton& btn, juce::String btnText = juce::String("") + "ファイル選択") {
		lbl.setup({ .parent = *this, .title = title });
		pathLbl.setup({ .parent = *this, .title = Io::empty });
        pathLbl.setColour(juce::Label::outlineColourId, juce::Colours::white);
        pathLbl.setJustificationType(juce::Justification::centredLeft);
		btn.setup({ .parent = *this, .title = btnText, .isReset = false });
    };

    auto setupFolderRow = [&](GuiLabel& lbl, juce::String title, GuiLabel& pathLbl, GuiTextButton& btn, juce::String btnText = juce::String("") + "フォルダ選択") {
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
    
	wallpaperClearBtn.setup({ .parent = *this, .title = juce::String("") + "解除", .textColor = juce::Colours::white, .bgColor = juce::Colours::red.withAlpha(0.5f), .isReset = false });
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

    separator2.setupComponent(*this);

    // --- ADPCM Dir ---
    // フォルダ設定はまとめて畳めるようにする。板は敷かないので、
    // 見出しの下に中身が続くだけの形になる。
    dirCat.setupCategory({ .parent = *this, .title = juce::String("") + "フォルダ設定(開閉)", .enableChangeDetailVisible = true }, GuiColor::Category::SettingsBg);
    
    setupFolderRow(sampleDirLabel, juce::String("") + "サンプルファイルディレクトリ:", sampleDirPathLabel, sampleDirBrowseBtn);
    sampleDirPathLabel.setText(ctx.audioProcessor.defaultSampleDir, juce::dontSendNotification);
    sampleDirPathLabel.setWantsKeyboardFocus(false);
    sampleDirBrowseBtn.setWantsKeyboardFocus(true);
    sampleDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    sampleDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
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
    setupFolderRow(presetDirLabel, juce::String("") + "プリセットファイルディレクトリ:", presetDirPathLabel, presetDirBrowseBtn);
    presetDirPathLabel.setText(ctx.audioProcessor.defaultPresetDir, juce::dontSendNotification);
    presetDirPathLabel.setWantsKeyboardFocus(false);

    presetDirBrowseBtn.setWantsKeyboardFocus(true);
    presetDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    presetDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "プリセットファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultPresetDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultPresetDir),
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
    setupFolderRow(wavetableDirLabel, juce::String("") + "波形メモリファイルディレクトリ:", wavetableDirPathLabel, wavetableDirBrowseBtn);
    wavetableDirPathLabel.setText(ctx.audioProcessor.defaultWavetableDir, juce::dontSendNotification);
    wavetableDirPathLabel.setWantsKeyboardFocus(false);

    wavetableDirBrowseBtn.setWantsKeyboardFocus(true);
    wavetableDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    wavetableDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "波形メモリファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultWavetableDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultWavetableDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultWavetableDir = file.getFullPathName();
                    wavetableDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
    };

    // --- Fx Order Dir ---
    setupFolderRow(fxOrderDirLabel, juce::String("") + "FX順番ファイルディレクトリ:", fxOrderDirPathLabel, fxOrderDirBrowseBtn);
    fxOrderDirPathLabel.setText(ctx.audioProcessor.defaultFxOrderDir, juce::dontSendNotification);
    fxOrderDirPathLabel.setWantsKeyboardFocus(false);

    fxOrderDirBrowseBtn.setWantsKeyboardFocus(true);
    fxOrderDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    fxOrderDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "エフェクトオーダーファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultFxOrderDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultFxOrderDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultFxOrderDir = file.getFullPathName();
                    fxOrderDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Fx Param Dir ---
    setupFolderRow(fxParamDirLabel, juce::String("") + "FXファイルディレクトリ:", fxParamDirPathLabel, fxParamDirBrowseBtn);
    fxParamDirPathLabel.setText(ctx.audioProcessor.defaultFxParamDir, juce::dontSendNotification);
    fxParamDirPathLabel.setWantsKeyboardFocus(false);

    fxParamDirBrowseBtn.setWantsKeyboardFocus(true);
    fxParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    fxParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "FXファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultFxParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultFxParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultFxParamDir = file.getFullPathName();
                    fxParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Channel Param Dir ---
    setupFolderRow(channelParamDirLabel, juce::String("") + "CHパラメータファイルディレクトリ:", channelParamDirPathLabel, channelParamDirBrowseBtn);
    channelParamDirPathLabel.setText(ctx.audioProcessor.defaultChannelParamDir, juce::dontSendNotification);
    channelParamDirPathLabel.setWantsKeyboardFocus(false);

    channelParamDirBrowseBtn.setWantsKeyboardFocus(true);
    channelParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    channelParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "CHパラメータファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultChannelParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultChannelParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultChannelParamDir = file.getFullPathName();
                    channelParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Curve Param Dir ---
    setupFolderRow(curveParamDirLabel, juce::String("") + "カーブ編集パラメータファイルディレクトリ:", curveParamDirPathLabel, curveParamDirBrowseBtn);
    curveParamDirPathLabel.setText(ctx.audioProcessor.defaultCurveParamDir, juce::dontSendNotification);
    curveParamDirPathLabel.setWantsKeyboardFocus(false);

    curveParamDirBrowseBtn.setWantsKeyboardFocus(true);
    curveParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    curveParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "カーブ編集パラメータファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultCurveParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultCurveParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultCurveParamDir = file.getFullPathName();
                    curveParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- LFO Param Dir ---
    setupFolderRow(lfoParamDirLabel, juce::String("") + "LFOファイルディレクトリ:", lfoParamDirPathLabel, lfoParamDirBrowseBtn);
    lfoParamDirPathLabel.setText(ctx.audioProcessor.defaultLfoParamDir, juce::dontSendNotification);
    lfoParamDirPathLabel.setWantsKeyboardFocus(false);

    lfoParamDirBrowseBtn.setWantsKeyboardFocus(true);
    lfoParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    lfoParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "LFOファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultLfoParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultLfoParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultLfoParamDir = file.getFullPathName();
                    lfoParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Amp Env Param Dir ---
    setupFolderRow(ampEnvParamDirLabel, juce::String("") + "AMP ENVファイルディレクトリ:", ampEnvParamDirPathLabel, ampEnvParamDirBrowseBtn);
    ampEnvParamDirPathLabel.setText(ctx.audioProcessor.defaultAmpEnvParamDir, juce::dontSendNotification);
    ampEnvParamDirPathLabel.setWantsKeyboardFocus(false);

    ampEnvParamDirBrowseBtn.setWantsKeyboardFocus(true);
    ampEnvParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    ampEnvParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "AMP ENVファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultAmpEnvParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultAmpEnvParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultAmpEnvParamDir = file.getFullPathName();
                    ampEnvParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Pitch Env Param Dir ---
    setupFolderRow(pitchEnvParamDirLabel, juce::String("") + "PITCH ENVファイルディレクトリ:", pitchEnvParamDirPathLabel, pitchEnvParamDirBrowseBtn);
    pitchEnvParamDirPathLabel.setText(ctx.audioProcessor.defaultPitchEnvParamDir, juce::dontSendNotification);
    pitchEnvParamDirPathLabel.setWantsKeyboardFocus(false);

    pitchEnvParamDirBrowseBtn.setWantsKeyboardFocus(true);
    pitchEnvParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    pitchEnvParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "PITCH ENVファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultPitchEnvParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultPitchEnvParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultPitchEnvParamDir = file.getFullPathName();
                    pitchEnvParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- SSG SW Env Param Dir ---
    setupFolderRow(ssgSwEnvParamDirLabel, juce::String("") + "SSG SW ENVファイルディレクトリ:", ssgSwEnvParamDirPathLabel, ssgSwEnvParamDirBrowseBtn);
    ssgSwEnvParamDirPathLabel.setText(ctx.audioProcessor.defaultSsgSwEnvParamDir, juce::dontSendNotification);
    ssgSwEnvParamDirPathLabel.setWantsKeyboardFocus(false);

    ssgSwEnvParamDirBrowseBtn.setWantsKeyboardFocus(true);
    ssgSwEnvParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    ssgSwEnvParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "SSG SW ENVファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultSsgSwEnvParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultSsgSwEnvParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getFullPathName();
                    ssgSwEnvParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- SSG HW Env Param Dir ---
    setupFolderRow(ssgHwEnvParamDirLabel, juce::String("") + "SSG HW ENVファイルディレクトリ:", ssgHwEnvParamDirPathLabel, ssgHwEnvParamDirBrowseBtn);
    ssgHwEnvParamDirPathLabel.setText(ctx.audioProcessor.defaultSsgHwEnvParamDir, juce::dontSendNotification);
    ssgHwEnvParamDirPathLabel.setWantsKeyboardFocus(false);

    ssgHwEnvParamDirBrowseBtn.setWantsKeyboardFocus(true);
    ssgHwEnvParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    ssgHwEnvParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "SSG HW ENVファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultSsgHwEnvParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultSsgHwEnvParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultSsgHwEnvParamDir = file.getFullPathName();
                    ssgHwEnvParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Detune Param Dir ---
    setupFolderRow(detuneParamDirLabel, juce::String("") + "DETUNE ファイルディレクトリ:", detuneParamDirPathLabel, detuneParamDirBrowseBtn);
    detuneParamDirPathLabel.setText(ctx.audioProcessor.defaultDetuneParamDir, juce::dontSendNotification);
    detuneParamDirPathLabel.setWantsKeyboardFocus(false);

    detuneParamDirBrowseBtn.setWantsKeyboardFocus(true);
    detuneParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    detuneParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "DETUNE ファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultDetuneParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultDetuneParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultDetuneParamDir = file.getFullPathName();
                    detuneParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Unison Param Dir ---
    setupFolderRow(unisonParamDirLabel, juce::String("") + "UNISON ファイルディレクトリ:", unisonParamDirPathLabel, unisonParamDirBrowseBtn);
    unisonParamDirPathLabel.setText(ctx.audioProcessor.defaultUnisonParamDir, juce::dontSendNotification);
    unisonParamDirPathLabel.setWantsKeyboardFocus(false);

    unisonParamDirBrowseBtn.setWantsKeyboardFocus(true);
    unisonParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    unisonParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "UNISON ファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultUnisonParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultUnisonParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultUnisonParamDir = file.getFullPathName();
                    unisonParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Quyality Param Dir ---
    setupFolderRow(qualityParamDirLabel, juce::String("") + "音質ファイルディレクトリ:", qualityParamDirPathLabel, qualityParamDirBrowseBtn);
    qualityParamDirPathLabel.setText(ctx.audioProcessor.defaultQualityParamDir, juce::dontSendNotification);
    qualityParamDirPathLabel.setWantsKeyboardFocus(false);

    qualityParamDirBrowseBtn.setWantsKeyboardFocus(true);
    qualityParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    qualityParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "音質ファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultQualityParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultQualityParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultQualityParamDir = file.getFullPathName();
                    qualityParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- PCM Play Param Dir ---
    setupFolderRow(pcmPlayParamDirLabel, juce::String("") + "PCM再生ファイルディレクトリ:", pcmPlayParamDirPathLabel, pcmPlayParamDirBrowseBtn);
    pcmPlayParamDirPathLabel.setText(ctx.audioProcessor.defaultPcmPlayParamDir, juce::dontSendNotification);
    pcmPlayParamDirPathLabel.setWantsKeyboardFocus(false);

    pcmPlayParamDirBrowseBtn.setWantsKeyboardFocus(true);
    pcmPlayParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    pcmPlayParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "PCM再生ファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultPcmPlayParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultPcmPlayParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultPcmPlayParamDir = file.getFullPathName();
                    pcmPlayParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Tone / Noise Param Dir ---
    setupFolderRow(colorSettingDirLabel, juce::String("") + "色の設定ファイルディレクトリ:", colorSettingDirPathLabel, colorSettingDirBrowseBtn);
    colorSettingDirPathLabel.setText(ctx.audioProcessor.defaultColorSettingDir, juce::dontSendNotification);
    colorSettingDirPathLabel.setWantsKeyboardFocus(false);

    colorSettingDirBrowseBtn.setWantsKeyboardFocus(true);
    colorSettingDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    colorSettingDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "色の設定ファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultColorSettingDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultColorSettingDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultColorSettingDir = file.getFullPathName();
                    colorSettingDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Tone / Noise Param Dir ---
    setupFolderRow(toneNoiseParamDirLabel, juce::String("") + "トーン/ノイズファイルディレクトリ:", toneNoiseParamDirPathLabel, toneNoiseParamDirBrowseBtn);
    toneNoiseParamDirPathLabel.setText(ctx.audioProcessor.defaultToneNoiseParamDir, juce::dontSendNotification);
    toneNoiseParamDirPathLabel.setWantsKeyboardFocus(false);

    toneNoiseParamDirBrowseBtn.setWantsKeyboardFocus(true);
    toneNoiseParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    toneNoiseParamDirBrowseBtn.onClick = [this] {
        ctx.editor.openFolderChooser(
            juce::String("") + "トーン/ノイズファイルディレクトリを選択してください",
            ctx.audioProcessor.defaultToneNoiseParamDir.isEmpty() ? ctx.audioProcessor.getPluginDirectory() : juce::File(ctx.audioProcessor.defaultToneNoiseParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    ctx.audioProcessor.defaultToneNoiseParamDir = file.getFullPathName();
                    toneNoiseParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    separator3.setupComponent(*this);


    // --- 簡易表示モード ---
    // 区分を隠すだけの切り替え。音には影響しない。
    // 切り替えたら画面を組み直して、その場で反映する。
    simpleViewToggle.setup({ .parent = *this, .title = juce::String("") + "簡易表示モード", .font = toggleFont, .isReset = false });
    simpleViewToggle.setToggleState(ctx.audioProcessor.simpleView, juce::dontSendNotification);
    simpleViewToggle.setWantsKeyboardFocus(true);
    simpleViewToggle.setExplicitFocusOrder(++tabOrder);
    simpleViewToggle.onClick = [this] {
        ctx.audioProcessor.simpleView = simpleViewToggle.getToggleState();

        bypassHiddenBtn.setEnabled(ctx.audioProcessor.simpleView);

        ctx.editor.resized();
        };

    // 隠れている区分をまとめて切る。簡易表示モードのときだけ押せる。
    bypassHiddenBtn.setup({ .parent = *this, .title = juce::String("") + "非表示中の区分をバイパス", .isReset = false });
    bypassHiddenBtn.setWantsKeyboardFocus(true);
    bypassHiddenBtn.setExplicitFocusOrder(++tabOrder);
    bypassHiddenBtn.setEnabled(ctx.audioProcessor.simpleView);
    bypassHiddenBtn.onClick = [this] {
        ctx.editor.bypassHiddenCategories();
        };

    simpleViewCat.setupCategory({ .parent = *this, .title = juce::String("") + "簡易表示モードカスタマイズ(開閉)", .enableChangeDetailVisible = true }, GuiColor::Category::SettingsBg);

    // 隠す対象のうち、出したままにするものを選ぶ。
    // 入れておくと簡易表示モードでもその区分が残る。
    for (int i = 0; i < SimpleView::Size; ++i)
    {
        auto& toggle = simpleViewShowToggles[(size_t)i];

        toggle.setup({ .parent = *this,
            .title = juce::String(SimpleView::items()[(size_t)i].title) + juce::String("") + " を表示",
            .font = toggleFont, .isReset = false });
        toggle.setToggleState(ctx.audioProcessor.simpleViewShow[(size_t)i], juce::dontSendNotification);
        toggle.setWantsKeyboardFocus(true);
        toggle.setExplicitFocusOrder(++tabOrder);
        toggle.onClick = [this, i] {
            ctx.audioProcessor.simpleViewShow[(size_t)i] = simpleViewShowToggles[(size_t)i].getToggleState();

            ctx.editor.resized();
            };
    }

    separatorSimple.setupComponent(*this);

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

    separator4.setupComponent(*this);

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

    separator5.setupComponent(*this);

    virtualMidiKeyboardToggle.setup({ .parent = *this, .title = juce::String("") + "仮想MIDIキーボード表示", .font = toggleFont , .isReset = false });
    virtualMidiKeyboardToggle.setWantsKeyboardFocus(true);
    virtualMidiKeyboardToggle.setExplicitFocusOrder(++tabOrder);
    virtualMidiKeyboardToggle.setToggleState(ctx.audioProcessor.showVirtualKeyboard, juce::dontSendNotification);
    virtualMidiKeyboardToggle.onClick = [this] {
        ctx.audioProcessor.showVirtualKeyboard = !ctx.audioProcessor.showVirtualKeyboard;

        ctx.editor.updateKeyboardVisibility();
        };

    separator6.setupComponent(*this);

    // --- Save Preference Button ---
    saveSettingsBtn.setup({ .parent = *this, .title = juce::String("") + "設定ファイルに保存", .isReset = false });
    saveSettingsBtn.setWantsKeyboardFocus(true);
    saveSettingsBtn.setExplicitFocusOrder(++tabOrder);
    saveSettingsBtn.onClick = [this] {
        ctx.editor.openWriteFileChooser(
            juce::String("") + "設定ファイルを選択してください",
            ctx.audioProcessor.getStartupSettingsFileToWrite(),
            SettingsValue::File::glob,
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
            ctx.audioProcessor.getPluginDirectory(),
            SettingsValue::File::glob,
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    ctx.audioProcessor.loadEnvironment(file);

                    // 反映は 1 か所へ寄せる。ここに並べ直していたため、
                    // 足した項目が読み込みのときだけ画面に出なかった。
                    setSettings();

                    // 壁紙再描画
                    ctx.editor.loadWallpaperImage();

                    // プリセットリスト更新
                    if (juce::File(ctx.audioProcessor.defaultPresetDir).isDirectory()) {
                        ctx.editor.setPresetDir(juce::File(ctx.audioProcessor.defaultPresetDir));
                        ctx.editor.updatePresetPath();
                        ctx.editor.scanPresets(); // リスト更新関数を呼ぶ
                    }

                    // UIスケール反映
                    ctx.editor.updateUiScale(getUiScale(ctx.audioProcessor.uiScaleIndex));
                }
            }

        );
        };

    saveStartupSettingsBtn.setup({ .parent = *this, .title = juce::String("") + "標準設定として保存", .textColor = juce::Colours::white, .bgColor = GuiColor::Settings::SaveAsDefaultBtnBg, .isReset = false });
    saveStartupSettingsBtn.setWantsKeyboardFocus(true);
    saveStartupSettingsBtn.setExplicitFocusOrder(++tabOrder);
    saveStartupSettingsBtn.onClick = [this]
        {
            // 保存は 1 か所へ寄せる。ここで項目を並べ直していたため、
            // 足した項目が標準設定にだけ入らないことが起きていた。
            auto file = ctx.audioProcessor.getStartupSettingsFileToWrite();

            if (ctx.audioProcessor.saveEnvironment(file))
            {
                // OS 標準のダイアログはテーマの色が当たらないので、
                // 他と同じ AlertWindow で出す。
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::InfoIcon,
                    juce::String("") + "成功",
                    // 場所と名前を分けて出す。ダイアログの本文は折り返らないので、
                    // 長いパスを 1 行で置くと末尾が見切れる。
                    juce::String("") + "現在の設定を標準設定として保存しました。\n\n"
                    + "場所: " + file.getParentDirectory().getFullPathName() + "\n"
                    + "ファイル名: " + file.getFileName(),
                    juce::String(),
                    this
                );
            }
            else
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::WarningIcon,
                    juce::String("") + "失敗",
                    juce::String("") + "標準設定を保存できませんでした。\n\n"
                    + "場所: " + file.getParentDirectory().getFullPathName() + "\n"
                    + "ファイル名: " + file.getFileName(),
                    juce::String(),
                    this
                );
            }
        };

    separator7.setupComponent(*this);

    // --- Clear Undo/Redo History Button ---
    clearUndoHistoryBtn.setup({ .parent = *this, .title = juce::String("") + "アンドゥ・リドゥ履歴の初期化", .textColor = juce::Colours::white, .bgColor = juce::Colours::blue.darker(0.3f).withAlpha(0.3f), .isReset = false});
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

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    mainGroup.setBounds(pageArea);

    auto sRect = pageArea.reduced(SettingsGuiValue::Group::Padding::width, SettingsGuiValue::Group::Padding::height);
    sRect.removeFromTop(SettingsGuiValue::Group::TitlePaddingTop);

    // 1. UI Scale
    auto rowUiScale = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    uiScaleSelector.label.setBounds(rowUiScale.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
    uiScaleSelector.setBounds(rowUiScale.removeFromLeft(SettingsGuiValue::Settings::UiScaleSelectorWidth));

    // 同じ行の余白へ置く。行を増やすと下がすべてずれるため。
    rowUiScale.removeFromLeft(SettingsGuiValue::Settings::PaddingHeight);
    fileFormatSelector.label.setBounds(rowUiScale.removeFromLeft(SettingsGuiValue::Settings::FileFormatLabelWidth));
    fileFormatSelector.setBounds(rowUiScale.removeFromLeft(SettingsGuiValue::Settings::FileFormatSelectorWidth));

    separator1.layoutComponent(sRect);

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

    separator2.layoutComponent(sRect);

    // ---------------- フォルダ設定 ----------------
    // 17 行あるので、まとめて畳めるようにしてある。閉じているときは
    // 隠すだけでなく、場所も取らないようにする。
    auto dirCatRow = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);

    // 見出しだけラベル幅では窮屈なので、行の中で広めに取る
    dirCat.setBounds(dirCatRow.removeFromLeft(SettingsGuiValue::Settings::LabelWidth * 3));

    sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

    bool dirVisible = dirCat.isDetailVisible();

    sampleDirLabel.setVisible(dirVisible);
    sampleDirPathLabel.setVisible(dirVisible);
    sampleDirBrowseBtn.setVisible(dirVisible);
    presetDirLabel.setVisible(dirVisible);
    presetDirPathLabel.setVisible(dirVisible);
    presetDirBrowseBtn.setVisible(dirVisible);
    wavetableDirLabel.setVisible(dirVisible);
    wavetableDirPathLabel.setVisible(dirVisible);
    wavetableDirBrowseBtn.setVisible(dirVisible);
    fxOrderDirLabel.setVisible(dirVisible);
    fxOrderDirPathLabel.setVisible(dirVisible);
    fxOrderDirBrowseBtn.setVisible(dirVisible);
    fxParamDirLabel.setVisible(dirVisible);
    fxParamDirPathLabel.setVisible(dirVisible);
    fxParamDirBrowseBtn.setVisible(dirVisible);
    channelParamDirLabel.setVisible(dirVisible);
    channelParamDirPathLabel.setVisible(dirVisible);
    channelParamDirBrowseBtn.setVisible(dirVisible);
    curveParamDirLabel.setVisible(dirVisible);
    curveParamDirPathLabel.setVisible(dirVisible);
    curveParamDirBrowseBtn.setVisible(dirVisible);
    lfoParamDirLabel.setVisible(dirVisible);
    lfoParamDirPathLabel.setVisible(dirVisible);
    lfoParamDirBrowseBtn.setVisible(dirVisible);
    ampEnvParamDirLabel.setVisible(dirVisible);
    ampEnvParamDirPathLabel.setVisible(dirVisible);
    ampEnvParamDirBrowseBtn.setVisible(dirVisible);
    pitchEnvParamDirLabel.setVisible(dirVisible);
    pitchEnvParamDirPathLabel.setVisible(dirVisible);
    pitchEnvParamDirBrowseBtn.setVisible(dirVisible);
    ssgSwEnvParamDirLabel.setVisible(dirVisible);
    ssgSwEnvParamDirPathLabel.setVisible(dirVisible);
    ssgSwEnvParamDirBrowseBtn.setVisible(dirVisible);
    ssgHwEnvParamDirLabel.setVisible(dirVisible);
    ssgHwEnvParamDirPathLabel.setVisible(dirVisible);
    ssgHwEnvParamDirBrowseBtn.setVisible(dirVisible);
    detuneParamDirLabel.setVisible(dirVisible);
    detuneParamDirPathLabel.setVisible(dirVisible);
    detuneParamDirBrowseBtn.setVisible(dirVisible);
    unisonParamDirLabel.setVisible(dirVisible);
    unisonParamDirPathLabel.setVisible(dirVisible);
    unisonParamDirBrowseBtn.setVisible(dirVisible);
    qualityParamDirLabel.setVisible(dirVisible);
    qualityParamDirPathLabel.setVisible(dirVisible);
    qualityParamDirBrowseBtn.setVisible(dirVisible);
    pcmPlayParamDirLabel.setVisible(dirVisible);
    pcmPlayParamDirPathLabel.setVisible(dirVisible);
    pcmPlayParamDirBrowseBtn.setVisible(dirVisible);
    toneNoiseParamDirLabel.setVisible(dirVisible);
    toneNoiseParamDirPathLabel.setVisible(dirVisible);
    toneNoiseParamDirBrowseBtn.setVisible(dirVisible);

    colorSettingDirLabel.setVisible(dirVisible);
    colorSettingDirPathLabel.setVisible(dirVisible);
    colorSettingDirBrowseBtn.setVisible(dirVisible);

    if (dirVisible)
    {
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

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 7. FX Order Dir
        auto rowFxOrderDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        fxOrderDirLabel.setBounds(rowFxOrderDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        fxOrderDirBrowseBtn.setBounds(rowFxOrderDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        fxOrderDirPathLabel.setBounds(rowFxOrderDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 8. FX Param Dir
        auto rowFxParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        fxParamDirLabel.setBounds(rowFxParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        fxParamDirBrowseBtn.setBounds(rowFxParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        fxParamDirPathLabel.setBounds(rowFxParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 9. Channel Param Dir
        auto rowChannelParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        channelParamDirLabel.setBounds(rowChannelParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        channelParamDirBrowseBtn.setBounds(rowChannelParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        channelParamDirPathLabel.setBounds(rowChannelParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 10. Curve Param Dir
        auto rowCurveParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        curveParamDirLabel.setBounds(rowCurveParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        curveParamDirBrowseBtn.setBounds(rowCurveParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        curveParamDirPathLabel.setBounds(rowCurveParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 11. LFO Param Dir
        auto rowLfoParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        lfoParamDirLabel.setBounds(rowLfoParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        lfoParamDirBrowseBtn.setBounds(rowLfoParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        lfoParamDirPathLabel.setBounds(rowLfoParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 12. Amp Env Param Dir
        auto rowAmpEnvParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        ampEnvParamDirLabel.setBounds(rowAmpEnvParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        ampEnvParamDirBrowseBtn.setBounds(rowAmpEnvParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        ampEnvParamDirPathLabel.setBounds(rowAmpEnvParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 13. Pitch Env Param Dir
        auto rowPitchEnvParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        pitchEnvParamDirLabel.setBounds(rowPitchEnvParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        pitchEnvParamDirBrowseBtn.setBounds(rowPitchEnvParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        pitchEnvParamDirPathLabel.setBounds(rowPitchEnvParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 14. Ssg Sw Env Param Dir
        auto rowSsgSwEnvParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        ssgSwEnvParamDirLabel.setBounds(rowSsgSwEnvParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        ssgSwEnvParamDirBrowseBtn.setBounds(rowSsgSwEnvParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        ssgSwEnvParamDirPathLabel.setBounds(rowSsgSwEnvParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 15. Ssg Hw Env Param Dir
        auto rowSsgHwEnvParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        ssgHwEnvParamDirLabel.setBounds(rowSsgHwEnvParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        ssgHwEnvParamDirBrowseBtn.setBounds(rowSsgHwEnvParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        ssgHwEnvParamDirPathLabel.setBounds(rowSsgHwEnvParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 16. Detune Param Dir
        auto rowDetuneParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        detuneParamDirLabel.setBounds(rowDetuneParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        detuneParamDirBrowseBtn.setBounds(rowDetuneParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        detuneParamDirPathLabel.setBounds(rowDetuneParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 17. Unison Param Dir
        auto rowUnisonParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        unisonParamDirLabel.setBounds(rowUnisonParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        unisonParamDirBrowseBtn.setBounds(rowUnisonParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        unisonParamDirPathLabel.setBounds(rowUnisonParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 18. Quality Param Dir
        auto rowQualityParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        qualityParamDirLabel.setBounds(rowQualityParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        qualityParamDirBrowseBtn.setBounds(rowQualityParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        qualityParamDirPathLabel.setBounds(rowQualityParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 19. PCM Play Param Dir
        auto rowPcmPlayParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        pcmPlayParamDirLabel.setBounds(rowPcmPlayParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        pcmPlayParamDirBrowseBtn.setBounds(rowPcmPlayParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        pcmPlayParamDirPathLabel.setBounds(rowPcmPlayParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 20. Tone / Noise Param Dir
        auto rowToneNoiseParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        toneNoiseParamDirLabel.setBounds(rowToneNoiseParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        toneNoiseParamDirBrowseBtn.setBounds(rowToneNoiseParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        toneNoiseParamDirPathLabel.setBounds(rowToneNoiseParamDir);

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        // 色の設定ファイルディレクトリ
        auto rowColorSettingDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        colorSettingDirLabel.setBounds(rowColorSettingDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        colorSettingDirBrowseBtn.setBounds(rowColorSettingDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        colorSettingDirPathLabel.setBounds(rowColorSettingDir);

    }

    // 区切り線はフォルダ設定の外。畳んでも下の設定との境目は残す。
    separator3.layoutComponent(sRect);


    // ---------------- 簡易表示モード ----------------
    auto rowSimpleView = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    simpleViewToggle.setBounds(rowSimpleView.removeFromLeft(SettingsGuiValue::Settings::ToggleWidth));

    rowSimpleView.removeFromLeft(SettingsGuiValue::Settings::PaddingWidth);

    bypassHiddenBtn.setBounds(rowSimpleView.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));

    // カスタマイズは、簡易表示モードを入れているときだけ出す。
    bool simpleOn = ctx.audioProcessor.simpleView;

    simpleViewCat.setVisible(simpleOn);

    bool simpleCustomVisible = false;

    if (simpleOn)
    {
        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        auto simpleCatRow = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);

        // 見出しだけラベル幅では窮屈なので、行の中で広めに取る
        simpleViewCat.setBounds(simpleCatRow.removeFromLeft(SettingsGuiValue::Settings::LabelWidth * 3));

        sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

        simpleCustomVisible = simpleViewCat.isDetailVisible();
    }

    for (int i = 0; i < SimpleView::Size; ++i)
    {
        auto& toggle = simpleViewShowToggles[(size_t)i];

        toggle.setVisible(simpleCustomVisible);

        if (!simpleCustomVisible) continue;

        auto row = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        toggle.setBounds(row.removeFromLeft(SettingsGuiValue::Settings::ToggleWidth));
    }

    separatorSimple.layoutComponent(sRect);

    // 21. Tooltip Visible Row
    auto rowTooltip = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    tooltipToggle.setBounds(rowTooltip.removeFromLeft(SettingsGuiValue::Settings::ToggleWidth));

    separator4.layoutComponent(sRect);

    // 22. Headroom Row
    auto rowHeadroom = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    useHeadroomToggle.setBounds(rowHeadroom.removeFromLeft(SettingsGuiValue::Settings::ToggleWidth));

    sRect.removeFromTop(SettingsGuiValue::Settings::PaddingHeight);

    // 23. Headroom Gain Row
    auto rowHeadroomGain = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    headroomGainSlider.label.setBounds(rowHeadroomGain.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
    headroomGainSlider.setBounds(rowHeadroomGain.removeFromLeft(SettingsGuiValue::Settings::HeadroomGainSliderWidth));

    separator5.layoutComponent(sRect);

    // 24. Virtual Keyboard Row
    auto rowVirtualMidiKeyboard = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    virtualMidiKeyboardToggle.setBounds(rowVirtualMidiKeyboard.removeFromLeft(SettingsGuiValue::Settings::ToggleWidth));

    separator6.layoutComponent(sRect);

    // 25. Config IO Buttons (Fixed Layout)
    auto rowIoBtns = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);

    layoutRowSettingsIo({ .rect = rowIoBtns, .loadSettingsBtn = &loadSettingsBtn, .saveSettingsBtn = &saveSettingsBtn, .saveStartupSettingsBtn = &saveStartupSettingsBtn, .rowHeight = SettingsGuiValue::Settings::RowHeight });

    separator7.layoutComponent(sRect);

    // 26. Clear Undo/Redo History Button
    auto rowClearHistoryBtns = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    layoutRow({ .rowRect = rowClearHistoryBtns, .component = &clearUndoHistoryBtn, .rowHeight = SettingsGuiValue::Settings::RowHeight});
}

void GuiSettings::setSettings()
{
    // プロセッサから直に読む。引数を 18 個も並べていたときは、順番を
    // 間違えても、行を足し忘れても気づけなかった。
    uiScaleSelector.setSelectedId(ctx.audioProcessor.uiScaleIndex + 1, juce::dontSendNotification);
    fileFormatSelector.setSelectedId(ctx.audioProcessor.fileFormatIndex + 1, juce::dontSendNotification);
    wallpaperModeSelector.setSelectedId(ctx.audioProcessor.wallpaperMode + 1, juce::dontSendNotification);

    wallpaperPathLabel.setText(ctx.audioProcessor.wallpaperPath.isEmpty()
        ? Io::empty : juce::File(ctx.audioProcessor.wallpaperPath).getFileName(), juce::dontSendNotification);

    sampleDirPathLabel.setText(ctx.audioProcessor.defaultSampleDir, juce::dontSendNotification);
    presetDirPathLabel.setText(ctx.audioProcessor.defaultPresetDir, juce::dontSendNotification);
    wavetableDirPathLabel.setText(ctx.audioProcessor.defaultWavetableDir, juce::dontSendNotification);
    fxOrderDirPathLabel.setText(ctx.audioProcessor.defaultFxOrderDir, juce::dontSendNotification);
    fxParamDirPathLabel.setText(ctx.audioProcessor.defaultFxParamDir, juce::dontSendNotification);
    channelParamDirPathLabel.setText(ctx.audioProcessor.defaultChannelParamDir, juce::dontSendNotification);
    curveParamDirPathLabel.setText(ctx.audioProcessor.defaultCurveParamDir, juce::dontSendNotification);
    lfoParamDirPathLabel.setText(ctx.audioProcessor.defaultLfoParamDir, juce::dontSendNotification);
    ampEnvParamDirPathLabel.setText(ctx.audioProcessor.defaultAmpEnvParamDir, juce::dontSendNotification);
    pitchEnvParamDirPathLabel.setText(ctx.audioProcessor.defaultPitchEnvParamDir, juce::dontSendNotification);
    ssgSwEnvParamDirPathLabel.setText(ctx.audioProcessor.defaultSsgSwEnvParamDir, juce::dontSendNotification);
    ssgHwEnvParamDirPathLabel.setText(ctx.audioProcessor.defaultSsgHwEnvParamDir, juce::dontSendNotification);
    detuneParamDirPathLabel.setText(ctx.audioProcessor.defaultDetuneParamDir, juce::dontSendNotification);
    unisonParamDirPathLabel.setText(ctx.audioProcessor.defaultUnisonParamDir, juce::dontSendNotification);
    qualityParamDirPathLabel.setText(ctx.audioProcessor.defaultQualityParamDir, juce::dontSendNotification);
    pcmPlayParamDirPathLabel.setText(ctx.audioProcessor.defaultPcmPlayParamDir, juce::dontSendNotification);
    toneNoiseParamDirPathLabel.setText(ctx.audioProcessor.defaultToneNoiseParamDir, juce::dontSendNotification);
    colorSettingDirPathLabel.setText(ctx.audioProcessor.defaultColorSettingDir, juce::dontSendNotification);
}

void GuiSettings::setWallpaperPath(const juce::String& wallpaperPath)
{
    wallpaperPathLabel.setText(wallpaperPath, juce::dontSendNotification);
}

float GuiSettings::getUiScale(int index) {
    // 番号は設定ファイルから来ることがあり、表の範囲内とは限らない。
    // std::array の [] は範囲を見ないので、ここで丸めておく。
    // 壊れた設定ファイルを一度読むと、以後画面を開くたびに落ちていた。
    const int last = (int)uiScaleLUT.size() - 1;
    return uiScaleLUT[(size_t)std::clamp(index, 0, last)];
}
