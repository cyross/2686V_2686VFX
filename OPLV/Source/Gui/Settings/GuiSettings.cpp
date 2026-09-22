#include <array>
#include <algorithm>
#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "Shared/Gui/Components/GenWave/GenWaveRender.h"
#include "./GuiSettings.h"

#include "../../Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiColor.h"

#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Shared/Core/Processor/ProcessorValues.h"
#include "./SettingsKeys.h"
#include "./SettingsValues.h"

#include "Shared/Core/Gui/GuiI18n.h"
#include "Shared/Core/Gui/GuiHelpers.h"
#include "./GuiSettingsValues.h"
#include "./GuiSettingsText.h"
#include "Shared/Core/Gui/GuiStructs.h"
#include "./GuiSettingsHelpers.h"
#include "../../Core/Gui/GuiPluginContext.h"

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
        {.name = SettingsGuiText::wpStretch, .value = 1 },
        {.name = SettingsGuiText::wpFill,  .value = 2 },
        {.name = SettingsGuiText::wpFit,  .value = 3 },
        {.name = SettingsGuiText::wpOriginal,   .value = 4 },
    };

    int tabOrder = 1;
    float separatorThick = 3.0f;

    mainGroup.setup(*this, SettingsGuiText::Group::settingEnv);

    // 画面に出す文字列の言語
    //
    // 選択肢の名前は、その言語自身の表記で固定にしてある。読めない言語へ
    // 間違えて切り替えても、ここを見れば戻せる。
    std::vector<SelectItem> languageItems = {
        {.name = juce::String(u8"日本語"), .value = 1 },
        {.name = "English", .value = 2 },
    };

    languageSelector.setup({
        .parent = *this,
        .id = "",
        .title = SettingsGuiText::language,
        .items = languageItems,
        .isReset = false,
        .labelColor = juce::Colours::yellow
        });
    languageSelector.setSelectedId(I18n::isJa() ? 1 : 2, juce::dontSendNotification);
    languageSelector.setWantsKeyboardFocus(true);
    languageSelector.setExplicitFocusOrder(++tabOrder);
    languageSelector.onChange = [this] {
        const auto lang = languageSelector.getSelectedItemIndex() == 0 ? I18n::Lang::ja : I18n::Lang::en;

        pluginOf(ctx).languageCode = I18n::toCode(lang);

        // ここを境に画面が組み直される。この選択そのものも作り直されるので、
        // 知らせは後回しで届くようにしてある (GuiI18n.cpp を参照)。
        I18n::setCurrent(lang);
        };

    // UI拡大率
    uiScaleSelector.setup({
        .parent = *this,
        .id = "",
        .title = SettingsGuiText::uiScale,
        .items = uiScaleItems,
        .isReset = false,
        .labelColor = juce::Colours::yellow
        });
    uiScaleSelector.setSelectedId(pluginOf(ctx).uiScaleIndex + 1, juce::dontSendNotification);
    uiScaleSelector.setWantsKeyboardFocus(true);
    uiScaleSelector.setExplicitFocusOrder(++tabOrder);
    uiScaleSelector.onChange = [this] {
        int index = uiScaleSelector.getSelectedItemIndex();

        pluginOf(ctx).uiScaleIndex = index;
        editorOf(ctx).updateUiScale(uiScaleLUT[index]);
        editorOf(ctx).resized();

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
        .title = SettingsGuiText::fileFormat,
        .items = fileFormatItems,
        .isReset = false,
        .labelColor = juce::Colours::yellow
        });
    fileFormatSelector.setSelectedId(pluginOf(ctx).fileFormatIndex + 1, juce::dontSendNotification);
    fileFormatSelector.setWantsKeyboardFocus(true);
    fileFormatSelector.setExplicitFocusOrder(++tabOrder);
    fileFormatSelector.onChange = [this] {
        pluginOf(ctx).fileFormatIndex = fileFormatSelector.getSelectedItemIndex();
        pluginOf(ctx).applyFileFormat();
        };

    separator1.setupComponent(*this);

    auto setupRow = [&](GuiLabel& lbl, juce::String title, GuiLabel& pathLbl, GuiTextButton& btn, juce::String btnText = SettingsGuiText::chooseFile) {
		lbl.setup({ .parent = *this, .title = title });
		pathLbl.setup({ .parent = *this, .title = Io::empty });
        pathLbl.setColour(juce::Label::outlineColourId, juce::Colours::white);
        pathLbl.setJustificationType(juce::Justification::centredLeft);
		btn.setup({ .parent = *this, .title = btnText, .isReset = false });
    };

    auto setupFolderRow = [&](GuiLabel& lbl, juce::String title, GuiLabel& pathLbl, GuiTextButton& btn, juce::String btnText = SettingsGuiText::chooseFolder) {
        lbl.setup({ .parent = *this, .title = title });
        pathLbl.setup({ .parent = *this, .title = Io::empty });
        pathLbl.setColour(juce::Label::outlineColourId, juce::Colours::white);
        pathLbl.setJustificationType(juce::Justification::centredLeft);
        btn.setup({ .parent = *this, .title = btnText, .isReset = false });
        };

    // --- Wallpaper Path ---
    setupRow(wallpaperLabel, SettingsGuiText::wallpaper, wallpaperPathLabel, wallpaperBrowseBtn);
    wallpaperPathLabel.setText(pluginOf(ctx).wallpaperPath, juce::dontSendNotification);
    wallpaperPathLabel.setWantsKeyboardFocus(false);
    wallpaperBrowseBtn.setWantsKeyboardFocus(true);
    wallpaperBrowseBtn.setExplicitFocusOrder(++tabOrder);
    wallpaperBrowseBtn.onClick = [this] {
        editorOf(ctx).openFileChooser(
            SettingsGuiText::wallpaperChoose,
            "*.png;*.jpg;*.jpeg",
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    pluginOf(ctx).wallpaperPath = file.getFullPathName();
                    wallpaperPathLabel.setText(file.getFileName(), juce::dontSendNotification);
                    editorOf(ctx).loadWallpaperImage();
                }
            }
        );
    };
    
	wallpaperClearBtn.setup({ .parent = *this, .title = SettingsGuiText::wallpaperClear, .textColor = juce::Colours::white, .bgColor = juce::Colours::red.withAlpha(0.5f), .isReset = false });
    wallpaperClearBtn.setWantsKeyboardFocus(true);
    wallpaperClearBtn.setExplicitFocusOrder(++tabOrder);
    wallpaperClearBtn.onClick = [this] {
        pluginOf(ctx).wallpaperPath = "";
        wallpaperPathLabel.setText(Io::empty, juce::dontSendNotification);
        editorOf(ctx).loadWallpaperImage();
    };

    // --- Wallpaper Mode ---
    wallpaperModeSelector.setup({ .parent = *this, .title = SettingsGuiText::wallpaperMode, .items = wpModeItems, .isReset = false });
    wallpaperModeSelector.setSelectedId(pluginOf(ctx).wallpaperMode + 1, juce::dontSendNotification);
    wallpaperModeSelector.setWantsKeyboardFocus(true);
    wallpaperModeSelector.setExplicitFocusOrder(++tabOrder);
    wallpaperModeSelector.onChange = [this] {
        pluginOf(ctx).wallpaperMode = wallpaperModeSelector.getSelectedId() - 1;
        editorOf(ctx).repaint(); // Editor全体の再描画を呼び出す
    };

    separator2.setupComponent(*this);

    // --- ADPCM Dir ---
    // フォルダ設定はまとめて畳めるようにする。板は敷かないので、
    // 見出しの下に中身が続くだけの形になる。
    dirCat.setupCategory({ .parent = *this, .title = SettingsGuiText::dirCat, .enableChangeDetailVisible = true }, GuiColor::Category::SettingsBg);
    
    setupFolderRow(sampleDirLabel, SettingsGuiText::Dir::sample, sampleDirPathLabel, sampleDirBrowseBtn);
    sampleDirPathLabel.setText(pluginOf(ctx).defaultSampleDir, juce::dontSendNotification);
    sampleDirPathLabel.setWantsKeyboardFocus(false);
    sampleDirBrowseBtn.setWantsKeyboardFocus(true);
    sampleDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    sampleDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::sampleChoose,
            pluginOf(ctx).defaultSampleDir.isEmpty() ? juce::File::getSpecialLocation(juce::File::userHomeDirectory) : juce::File(pluginOf(ctx).defaultSampleDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultSampleDir = file.getFullPathName();
                    pluginOf(ctx).lastSampleDirectory = file;
                    sampleDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
    };

    // --- Preset Dir ---
    setupFolderRow(presetDirLabel, SettingsGuiText::Dir::preset, presetDirPathLabel, presetDirBrowseBtn);
    presetDirPathLabel.setText(pluginOf(ctx).defaultPresetDir, juce::dontSendNotification);
    presetDirPathLabel.setWantsKeyboardFocus(false);

    presetDirBrowseBtn.setWantsKeyboardFocus(true);
    presetDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    presetDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::presetChoose,
            pluginOf(ctx).defaultPresetDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultPresetDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultPresetDir = file.getFullPathName();
                    presetDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);

                    editorOf(ctx).setPresetDir(file);
                    presetDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                    editorOf(ctx).scanPresets();
                }
            }
        );
    };

    // --- Wavetable Dir ---
    setupFolderRow(wavetableDirLabel, SettingsGuiText::Dir::wavetable, wavetableDirPathLabel, wavetableDirBrowseBtn);
    wavetableDirPathLabel.setText(pluginOf(ctx).defaultWavetableDir, juce::dontSendNotification);
    wavetableDirPathLabel.setWantsKeyboardFocus(false);

    wavetableDirBrowseBtn.setWantsKeyboardFocus(true);
    wavetableDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    wavetableDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::wavetableChoose,
            pluginOf(ctx).defaultWavetableDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultWavetableDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultWavetableDir = file.getFullPathName();
                    wavetableDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
    };

    // --- Fx Order Dir ---
    setupFolderRow(fxOrderDirLabel, SettingsGuiText::Dir::fxOrder, fxOrderDirPathLabel, fxOrderDirBrowseBtn);
    fxOrderDirPathLabel.setText(pluginOf(ctx).defaultFxOrderDir, juce::dontSendNotification);
    fxOrderDirPathLabel.setWantsKeyboardFocus(false);

    fxOrderDirBrowseBtn.setWantsKeyboardFocus(true);
    fxOrderDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    fxOrderDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::fxOrderChoose,
            pluginOf(ctx).defaultFxOrderDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultFxOrderDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultFxOrderDir = file.getFullPathName();
                    fxOrderDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Fx Param Dir ---
    setupFolderRow(fxParamDirLabel, SettingsGuiText::Dir::fxParam, fxParamDirPathLabel, fxParamDirBrowseBtn);
    fxParamDirPathLabel.setText(pluginOf(ctx).defaultFxParamDir, juce::dontSendNotification);
    fxParamDirPathLabel.setWantsKeyboardFocus(false);

    fxParamDirBrowseBtn.setWantsKeyboardFocus(true);
    fxParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    fxParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::fxParamChoose,
            pluginOf(ctx).defaultFxParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultFxParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultFxParamDir = file.getFullPathName();
                    fxParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Channel Param Dir ---
    setupFolderRow(channelParamDirLabel, SettingsGuiText::Dir::channelParam, channelParamDirPathLabel, channelParamDirBrowseBtn);
    channelParamDirPathLabel.setText(pluginOf(ctx).defaultChannelParamDir, juce::dontSendNotification);
    channelParamDirPathLabel.setWantsKeyboardFocus(false);

    channelParamDirBrowseBtn.setWantsKeyboardFocus(true);
    channelParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    channelParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::channelParamChoose,
            pluginOf(ctx).defaultChannelParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultChannelParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultChannelParamDir = file.getFullPathName();
                    channelParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Curve Param Dir ---
    setupFolderRow(curveParamDirLabel, SettingsGuiText::Dir::curveParam, curveParamDirPathLabel, curveParamDirBrowseBtn);
    curveParamDirPathLabel.setText(pluginOf(ctx).defaultCurveParamDir, juce::dontSendNotification);
    curveParamDirPathLabel.setWantsKeyboardFocus(false);

    curveParamDirBrowseBtn.setWantsKeyboardFocus(true);
    curveParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    curveParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::curveParamChoose,
            pluginOf(ctx).defaultCurveParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultCurveParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultCurveParamDir = file.getFullPathName();
                    curveParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- LFO Param Dir ---
    setupFolderRow(lfoParamDirLabel, SettingsGuiText::Dir::lfoParam, lfoParamDirPathLabel, lfoParamDirBrowseBtn);
    lfoParamDirPathLabel.setText(pluginOf(ctx).defaultLfoParamDir, juce::dontSendNotification);
    lfoParamDirPathLabel.setWantsKeyboardFocus(false);

    lfoParamDirBrowseBtn.setWantsKeyboardFocus(true);
    lfoParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    lfoParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::lfoParamChoose,
            pluginOf(ctx).defaultLfoParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultLfoParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultLfoParamDir = file.getFullPathName();
                    lfoParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Amp Env Param Dir ---
    setupFolderRow(ampEnvParamDirLabel, SettingsGuiText::Dir::ampEnvParam, ampEnvParamDirPathLabel, ampEnvParamDirBrowseBtn);
    ampEnvParamDirPathLabel.setText(pluginOf(ctx).defaultAmpEnvParamDir, juce::dontSendNotification);
    ampEnvParamDirPathLabel.setWantsKeyboardFocus(false);

    ampEnvParamDirBrowseBtn.setWantsKeyboardFocus(true);
    ampEnvParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    ampEnvParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::ampEnvParamChoose,
            pluginOf(ctx).defaultAmpEnvParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultAmpEnvParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultAmpEnvParamDir = file.getFullPathName();
                    ampEnvParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Pitch Env Param Dir ---
    setupFolderRow(pitchEnvParamDirLabel, SettingsGuiText::Dir::pitchEnvParam, pitchEnvParamDirPathLabel, pitchEnvParamDirBrowseBtn);
    pitchEnvParamDirPathLabel.setText(pluginOf(ctx).defaultPitchEnvParamDir, juce::dontSendNotification);
    pitchEnvParamDirPathLabel.setWantsKeyboardFocus(false);

    pitchEnvParamDirBrowseBtn.setWantsKeyboardFocus(true);
    pitchEnvParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    pitchEnvParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::pitchEnvParamChoose,
            pluginOf(ctx).defaultPitchEnvParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultPitchEnvParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultPitchEnvParamDir = file.getFullPathName();
                    pitchEnvParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- SSG SW Env Param Dir ---
    setupFolderRow(ssgSwEnvParamDirLabel, SettingsGuiText::Dir::ssgSwEnvParam, ssgSwEnvParamDirPathLabel, ssgSwEnvParamDirBrowseBtn);
    ssgSwEnvParamDirPathLabel.setText(pluginOf(ctx).defaultSsgSwEnvParamDir, juce::dontSendNotification);
    ssgSwEnvParamDirPathLabel.setWantsKeyboardFocus(false);

    ssgSwEnvParamDirBrowseBtn.setWantsKeyboardFocus(true);
    ssgSwEnvParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    ssgSwEnvParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::ssgSwEnvParamChoose,
            pluginOf(ctx).defaultSsgSwEnvParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultSsgSwEnvParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultSsgSwEnvParamDir = file.getFullPathName();
                    ssgSwEnvParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- SSG HW Env Param Dir ---
    setupFolderRow(ssgHwEnvParamDirLabel, SettingsGuiText::Dir::ssgHwEnvParam, ssgHwEnvParamDirPathLabel, ssgHwEnvParamDirBrowseBtn);
    ssgHwEnvParamDirPathLabel.setText(pluginOf(ctx).defaultSsgHwEnvParamDir, juce::dontSendNotification);
    ssgHwEnvParamDirPathLabel.setWantsKeyboardFocus(false);

    ssgHwEnvParamDirBrowseBtn.setWantsKeyboardFocus(true);
    ssgHwEnvParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    ssgHwEnvParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::ssgHwEnvParamChoose,
            pluginOf(ctx).defaultSsgHwEnvParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultSsgHwEnvParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultSsgHwEnvParamDir = file.getFullPathName();
                    ssgHwEnvParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Detune Param Dir ---
    setupFolderRow(detuneParamDirLabel, SettingsGuiText::Dir::detuneParam, detuneParamDirPathLabel, detuneParamDirBrowseBtn);
    detuneParamDirPathLabel.setText(pluginOf(ctx).defaultDetuneParamDir, juce::dontSendNotification);
    detuneParamDirPathLabel.setWantsKeyboardFocus(false);

    detuneParamDirBrowseBtn.setWantsKeyboardFocus(true);
    detuneParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    detuneParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::detuneParamChoose,
            pluginOf(ctx).defaultDetuneParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultDetuneParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultDetuneParamDir = file.getFullPathName();
                    detuneParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Unison Param Dir ---
    setupFolderRow(unisonParamDirLabel, SettingsGuiText::Dir::unisonParam, unisonParamDirPathLabel, unisonParamDirBrowseBtn);
    unisonParamDirPathLabel.setText(pluginOf(ctx).defaultUnisonParamDir, juce::dontSendNotification);
    unisonParamDirPathLabel.setWantsKeyboardFocus(false);

    unisonParamDirBrowseBtn.setWantsKeyboardFocus(true);
    unisonParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    unisonParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::unisonParamChoose,
            pluginOf(ctx).defaultUnisonParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultUnisonParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultUnisonParamDir = file.getFullPathName();
                    unisonParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Quality Param Dir ---
    setupFolderRow(qualityParamDirLabel, SettingsGuiText::Dir::qualityParam, qualityParamDirPathLabel, qualityParamDirBrowseBtn);
    qualityParamDirPathLabel.setText(pluginOf(ctx).defaultQualityParamDir, juce::dontSendNotification);
    qualityParamDirPathLabel.setWantsKeyboardFocus(false);

    qualityParamDirBrowseBtn.setWantsKeyboardFocus(true);
    qualityParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    qualityParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::qualityParamChoose,
            pluginOf(ctx).defaultQualityParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultQualityParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultQualityParamDir = file.getFullPathName();
                    qualityParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- PCM Play Param Dir ---
    setupFolderRow(pcmPlayParamDirLabel, SettingsGuiText::Dir::pcmPlayParam, pcmPlayParamDirPathLabel, pcmPlayParamDirBrowseBtn);
    pcmPlayParamDirPathLabel.setText(pluginOf(ctx).defaultPcmPlayParamDir, juce::dontSendNotification);
    pcmPlayParamDirPathLabel.setWantsKeyboardFocus(false);

    pcmPlayParamDirBrowseBtn.setWantsKeyboardFocus(true);
    pcmPlayParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    pcmPlayParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::pcmPlayParamChoose,
            pluginOf(ctx).defaultPcmPlayParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultPcmPlayParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultPcmPlayParamDir = file.getFullPathName();
                    pcmPlayParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Color Setting Dir ---
    setupFolderRow(colorSettingDirLabel, SettingsGuiText::Dir::colorSetting, colorSettingDirPathLabel, colorSettingDirBrowseBtn);
    colorSettingDirPathLabel.setText(pluginOf(ctx).defaultColorSettingDir, juce::dontSendNotification);
    colorSettingDirPathLabel.setWantsKeyboardFocus(false);

    colorSettingDirBrowseBtn.setWantsKeyboardFocus(true);
    colorSettingDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    colorSettingDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::colorSettingChoose,
            pluginOf(ctx).defaultColorSettingDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultColorSettingDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultColorSettingDir = file.getFullPathName();
                    colorSettingDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- Tone / Noise Param Dir ---
    setupFolderRow(toneNoiseParamDirLabel, SettingsGuiText::Dir::toneNoiseParam, toneNoiseParamDirPathLabel, toneNoiseParamDirBrowseBtn);
    toneNoiseParamDirPathLabel.setText(pluginOf(ctx).defaultToneNoiseParamDir, juce::dontSendNotification);
    toneNoiseParamDirPathLabel.setWantsKeyboardFocus(false);

    toneNoiseParamDirBrowseBtn.setWantsKeyboardFocus(true);
    toneNoiseParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    toneNoiseParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::toneNoiseParamChoose,
            pluginOf(ctx).defaultToneNoiseParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultToneNoiseParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultToneNoiseParamDir = file.getFullPathName();
                    toneNoiseParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    // --- WT MOD Param Dir ---
    setupFolderRow(wtModParamDirLabel, SettingsGuiText::Dir::wtModParam, wtModParamDirPathLabel, wtModParamDirBrowseBtn);
    wtModParamDirPathLabel.setText(pluginOf(ctx).defaultWtModParamDir, juce::dontSendNotification);
    wtModParamDirPathLabel.setWantsKeyboardFocus(false);

    wtModParamDirBrowseBtn.setWantsKeyboardFocus(true);
    wtModParamDirBrowseBtn.setExplicitFocusOrder(++tabOrder);
    wtModParamDirBrowseBtn.onClick = [this] {
        editorOf(ctx).openFolderChooser(
            SettingsGuiText::Dir::wtModParamChoose,
            pluginOf(ctx).defaultWtModParamDir.isEmpty() ? pluginOf(ctx).getPluginDirectory() : juce::File(pluginOf(ctx).defaultWtModParamDir),
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    pluginOf(ctx).defaultWtModParamDir = file.getFullPathName();
                    wtModParamDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
        };

    separator3.setupComponent(*this);


    // --- 簡易表示モード ---
    // 区分を隠すだけの切り替え。音には影響しない。
    // 切り替えたら画面を組み直して、その場で反映する。
    simpleViewToggle.setup({ .parent = *this, .title = SettingsGuiText::simpleView, .font = toggleFont, .isReset = false });
    simpleViewToggle.setToggleState(pluginOf(ctx).simpleView, juce::dontSendNotification);
    simpleViewToggle.setWantsKeyboardFocus(true);
    simpleViewToggle.setExplicitFocusOrder(++tabOrder);
    simpleViewToggle.onClick = [this] {
        pluginOf(ctx).simpleView = simpleViewToggle.getToggleState();

        bypassHiddenBtn.setEnabled(pluginOf(ctx).simpleView);

        editorOf(ctx).resized();
        };

    // 隠れている区分をまとめて切る。簡易表示モードのときだけ押せる。
    bypassHiddenBtn.setup({ .parent = *this, .title = SettingsGuiText::bypassHidden, .isReset = false });
    bypassHiddenBtn.setWantsKeyboardFocus(true);
    bypassHiddenBtn.setExplicitFocusOrder(++tabOrder);
    bypassHiddenBtn.setEnabled(pluginOf(ctx).simpleView);
    bypassHiddenBtn.onClick = [this] {
        editorOf(ctx).bypassHiddenCategories();
        };

    simpleViewCat.setupCategory({ .parent = *this, .title = SettingsGuiText::simpleViewCat, .enableChangeDetailVisible = true }, GuiColor::Category::SettingsBg);

    // 隠す対象のうち、出したままにするものを選ぶ。
    // 入れておくと簡易表示モードでもその区分が残る。
    for (int i = 0; i < SimpleView::Size; ++i)
    {
        auto& toggle = simpleViewShowToggles[(size_t)i];

        toggle.setup({ .parent = *this,
            .title = SettingsGuiText::showItem.get().replace("%s", SimpleView::items()[(size_t)i].title),
            .font = toggleFont, .isReset = false });
        toggle.setToggleState(pluginOf(ctx).simpleViewShow[(size_t)i], juce::dontSendNotification);
        toggle.setWantsKeyboardFocus(true);
        toggle.setExplicitFocusOrder(++tabOrder);
        toggle.onClick = [this, i] {
            pluginOf(ctx).simpleViewShow[(size_t)i] = simpleViewShowToggles[(size_t)i].getToggleState();

            editorOf(ctx).resized();
            };
    }

    separatorSimple.setupComponent(*this);

    // --- トグルボタンの並べ方 ---
    // 画面じゅうのトグルを、中央寄せ (従来) か左寄せかで描き分ける。
    // 置き場所は変えないので、切り替えたら描き直すだけでよい。
    std::vector<SelectItem> toggleAlignItems = {
        {.name = SettingsGuiText::toggleAlignCentred, .value = ToggleAlign::Centred + 1 },
        {.name = SettingsGuiText::toggleAlignLeft,   .value = ToggleAlign::Left + 1 },
    };

    toggleAlignSelector.setup({
        .parent = *this,
        .id = "",
        .title = SettingsGuiText::toggleAlign,
        .items = toggleAlignItems,
        .isReset = false
        });
    toggleAlignSelector.setSelectedId(pluginOf(ctx).toggleAlign + 1, juce::dontSendNotification);
    toggleAlignSelector.setWantsKeyboardFocus(true);
    toggleAlignSelector.setExplicitFocusOrder(++tabOrder);
    toggleAlignSelector.onChange = [this] {
        pluginOf(ctx).toggleAlign = toggleAlignSelector.getSelectedItemIndex();

        editorOf(ctx).repaint();
        };

    separatorToggleAlign.setupComponent(*this);

    // --- Toggle Tooltip Visible Toggle Button ---
    tooltipToggle.setup({ .parent = *this, .title = SettingsGuiText::showTooltips, .font = toggleFont, .isReset = false });
    tooltipToggle.setToggleState(pluginOf(ctx).showTooltips, juce::dontSendNotification);
    tooltipToggle.setWantsKeyboardFocus(true);
    tooltipToggle.setExplicitFocusOrder(++tabOrder);
    tooltipToggle.onClick = [this] {
        bool newState = tooltipToggle.getToggleState();
        pluginOf(ctx).showTooltips = newState;
        editorOf(ctx).setTooltipState(newState); // 即座に反映
        };

    separator4.setupComponent(*this);

    useHeadroomToggle.setup({ .parent = *this, .title = SettingsGuiText::useHeadroom, .font = toggleFont, .isReset = false });
    useHeadroomToggle.setToggleState(pluginOf(ctx).useHeadroom, juce::dontSendNotification);
    useHeadroomToggle.setWantsKeyboardFocus(true);
    useHeadroomToggle.setExplicitFocusOrder(++tabOrder);
    useHeadroomToggle.onClick = [this] {
        bool state = useHeadroomToggle.getToggleState();
        pluginOf(ctx).useHeadroom = state;
        headroomGainSlider.setEnabledWithLabel(state); // OFFならスライダーも無効化
        };

    // --- Headroom Gain Slider---
    headroomGainSlider.setup({ .parent = *this, .title = SettingsGuiText::headroomGain, .isReset = false });
    headroomGainSlider.setWantsKeyboardFocus(true);
    headroomGainSlider.setExplicitFocusOrder(++tabOrder);
    headroomGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    headroomGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    headroomGainSlider.setRange(0.0, 1.0, 0.01); // 0.0 ~ 1.0
    // プロセッサの値で初期化
    headroomGainSlider.setValue(pluginOf(ctx).headroomGain, juce::dontSendNotification);
    headroomGainSlider.setEnabledWithLabel(pluginOf(ctx).useHeadroom);

    headroomGainSlider.onValueChange = [this] {
        pluginOf(ctx).headroomGain = (float)headroomGainSlider.getValue();
        };

    separatorLiveDetune.setupComponent(*this);

    // MUL/DET・FIX を鳴らしている最中にも反映するか。切っていれば、
    // 押したときの値のまま鳴らす (3.6.1 までと同じ)。
    liveDetuneToggle.setup({ .parent = *this, .title = SettingsGuiText::liveDetune, .font = toggleFont, .isReset = false });
    liveDetuneToggle.setToggleState(pluginOf(ctx).liveDetune, juce::dontSendNotification);
    liveDetuneToggle.setWantsKeyboardFocus(true);
    liveDetuneToggle.setExplicitFocusOrder(++tabOrder);
    liveDetuneToggle.onClick = [this] {
        pluginOf(ctx).liveDetune = liveDetuneToggle.getToggleState();
        };

    separator5.setupComponent(*this);

    virtualMidiKeyboardToggle.setup({ .parent = *this, .title = SettingsGuiText::showVirtualKeyboard, .font = toggleFont , .isReset = false });
    virtualMidiKeyboardToggle.setWantsKeyboardFocus(true);
    virtualMidiKeyboardToggle.setExplicitFocusOrder(++tabOrder);
    virtualMidiKeyboardToggle.setToggleState(pluginOf(ctx).showVirtualKeyboard, juce::dontSendNotification);
    virtualMidiKeyboardToggle.onClick = [this] {
        pluginOf(ctx).showVirtualKeyboard = !pluginOf(ctx).showVirtualKeyboard;

        editorOf(ctx).updateKeyboardVisibility();
        };

    separator6.setupComponent(*this);

    // --- Save Preference Button ---
    saveSettingsBtn.setup({ .parent = *this, .title = SettingsGuiText::saveSettings, .isReset = false });
    saveSettingsBtn.setWantsKeyboardFocus(true);
    saveSettingsBtn.setExplicitFocusOrder(++tabOrder);
    saveSettingsBtn.onClick = [this] {
        editorOf(ctx).openWriteFileChooser(
            SettingsGuiText::chooseSettingsFile,
            pluginOf(ctx).getStartupSettingsFileToWrite(),
            SettingsValue::File::glob,
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file != juce::File()) {
                    pluginOf(ctx).saveEnvironment(file);
                }
            }
        );
        };

    // --- Load Preference Button ---
    loadSettingsBtn.setup({ .parent = *this, .title = SettingsGuiText::loadSettings, .isReset = false });
    loadSettingsBtn.setWantsKeyboardFocus(true);
    loadSettingsBtn.setExplicitFocusOrder(++tabOrder);
    loadSettingsBtn.onClick = [this] {
        editorOf(ctx).openFileChooser(
            SettingsGuiText::chooseSettingsFile,
            pluginOf(ctx).getPluginDirectory(),
            SettingsValue::File::glob,
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    pluginOf(ctx).loadEnvironment(file);

                    // 反映は 1 か所へ寄せる。ここに並べ直していたため、
                    // 足した項目が読み込みのときだけ画面に出なかった。
                    setSettings();

                    // 読んだ値を画面へ効かせる。簡易表示モードは組み直さないと出てこない。
                    editorOf(ctx).setTooltipState(pluginOf(ctx).showTooltips);
                    editorOf(ctx).updateKeyboardVisibility();
                    editorOf(ctx).resized();

                    // 壁紙再描画
                    editorOf(ctx).loadWallpaperImage();

                    // プリセットリスト更新
                    if (juce::File(pluginOf(ctx).defaultPresetDir).isDirectory()) {
                        editorOf(ctx).setPresetDir(juce::File(pluginOf(ctx).defaultPresetDir));
                        editorOf(ctx).updatePresetPath();
                        editorOf(ctx).scanPresets(); // リスト更新関数を呼ぶ
                    }

                    // UIスケール反映
                    editorOf(ctx).updateUiScale(getUiScale(pluginOf(ctx).uiScaleIndex));
                }
            }

        );
        };

    saveStartupSettingsBtn.setup({ .parent = *this, .title = SettingsGuiText::saveStartup, .textColor = juce::Colours::white, .bgColor = GuiColor::Settings::SaveAsDefaultBtnBg, .isReset = false });
    saveStartupSettingsBtn.setWantsKeyboardFocus(true);
    saveStartupSettingsBtn.setExplicitFocusOrder(++tabOrder);
    saveStartupSettingsBtn.onClick = [this]
        {
            // 保存は 1 か所へ寄せる。ここで項目を並べ直していたため、
            // 足した項目が標準設定にだけ入らないことが起きていた。
            auto file = pluginOf(ctx).getStartupSettingsFileToWrite();

            if (pluginOf(ctx).saveEnvironment(file))
            {
                // OS 標準のダイアログはテーマの色が当たらないので、
                // 他と同じ AlertWindow で出す。
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::InfoIcon,
                    SettingsGuiText::saveStartupOkTitle,
                    // 場所と名前を分けて出す。ダイアログの本文は折り返らないので、
                    // 長いパスを 1 行で置くと末尾が見切れる。
                    SettingsGuiText::saveStartupOkBody.get() + "\n\n"
                    + SettingsGuiText::dialogPlace.get() + file.getParentDirectory().getFullPathName() + "\n"
                    + SettingsGuiText::dialogFileName.get() + file.getFileName(),
                    juce::String(),
                    this
                );
            }
            else
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::WarningIcon,
                    SettingsGuiText::saveStartupNgTitle,
                    SettingsGuiText::saveStartupNgBody.get() + "\n\n"
                    + SettingsGuiText::dialogPlace.get() + file.getParentDirectory().getFullPathName() + "\n"
                    + SettingsGuiText::dialogFileName.get() + file.getFileName(),
                    juce::String(),
                    this
                );
            }
        };

    separator7.setupComponent(*this);

    // --- Clear Undo/Redo History Button ---
    clearUndoHistoryBtn.setup({ .parent = *this, .title = SettingsGuiText::clearUndoHistory, .textColor = juce::Colours::white, .bgColor = juce::Colours::blue.darker(0.3f).withAlpha(0.3f), .isReset = false});
    clearUndoHistoryBtn.setWantsKeyboardFocus(true);
    clearUndoHistoryBtn.setExplicitFocusOrder(++tabOrder);
    clearUndoHistoryBtn.onClick = [this] {
        pluginOf(ctx).undoManager.clearUndoHistory();
        };

    separator8.setupComponent(*this);

    // --- Clear All Wave Previews ---
    clearWavePreviewsBtn.setup({ .parent = *this, .title = SettingsGuiText::clearWavePreviews, .textColor = GuiColor::GenWave::DeleteText, .bgColor = GuiColor::GenWave::DeleteBg, .isReset = false });
    clearWavePreviewsBtn.setWantsKeyboardFocus(true);
    clearWavePreviewsBtn.setExplicitFocusOrder(++tabOrder);
    clearWavePreviewsBtn.onClick = [this] { clearWavePreviews(); };
}

// 作り置きした波形プレビューをまとめて捨てる。
//
// 消すのは自前の置き場の中だけで、ほかのファイルには触らない。
// 作り直せるものなので、消したあとは各画面の生成ボタンで作り直せる。
void GuiSettings::clearWavePreviews()
{
    const auto dir = GenWaveRender::cacheDirectory(pluginOf(ctx));
    const auto files = dir.findChildFiles(juce::File::findFiles, false,
        "*" + GenWaveRender::fileExtension);

    auto* window = new juce::AlertWindow(
        SettingsGuiText::clearWavePreviewsTitle,
        dir.getFullPathName() + "\n\n"
        + SettingsGuiText::clearWavePreviewsCount.get().replace("%d", juce::String(files.size())),
        juce::MessageBoxIconType::NoIcon);

    window->addButton(SettingsGuiText::clearWavePreviewsOk, 1);
    window->addButton(SettingsGuiText::clearWavePreviewsCancel, 0,
        juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

    GuiDialog::styleButtons(*window);

    window->enterModalState(true, juce::ModalCallbackFunction::create(
        [files](int result) {
            if (result != 1) return;

            for (const auto& file : files) file.deleteFile();
        }), true);
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

    // 1. Language / UI Scale
    auto rowUiScale = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    languageSelector.label.setBounds(rowUiScale.removeFromLeft(SettingsGuiValue::Settings::LanguageLabelWidth));
    languageSelector.setBounds(rowUiScale.removeFromLeft(SettingsGuiValue::Settings::LanguageSelectorWidth));

    rowUiScale.removeFromLeft(SettingsGuiValue::Settings::PaddingHeight);

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
    wtModParamDirLabel.setVisible(dirVisible);
    wtModParamDirPathLabel.setVisible(dirVisible);
    wtModParamDirBrowseBtn.setVisible(dirVisible);

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

        // 21. WT MOD Param Dir
        auto rowWtModParamDir = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
        wtModParamDirLabel.setBounds(rowWtModParamDir.removeFromLeft(SettingsGuiValue::Settings::LabelWidth));
        wtModParamDirBrowseBtn.setBounds(rowWtModParamDir.removeFromRight(SettingsGuiValue::Settings::BrowseButtonWidth));
        wtModParamDirPathLabel.setBounds(rowWtModParamDir);

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
    bool simpleOn = pluginOf(ctx).simpleView;

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

    // 20-1. トグルボタン配置
    auto rowToggleAlign = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    toggleAlignSelector.label.setBounds(rowToggleAlign.removeFromLeft(SettingsGuiValue::Settings::ToggleAlignLabelWidth));
    toggleAlignSelector.setBounds(rowToggleAlign.removeFromLeft(SettingsGuiValue::Settings::ToggleAlignSelectorWidth));

    separatorToggleAlign.layoutComponent(sRect);

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

    separatorLiveDetune.layoutComponent(sRect);

    // 23-1. MUL/DET・FIX を鳴らしながら反映
    auto rowLiveDetune = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    liveDetuneToggle.setBounds(rowLiveDetune.removeFromLeft(SettingsGuiValue::Settings::ToggleWidth));

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

    separator8.layoutComponent(sRect);

    // 27. Clear All Wave Previews
    auto rowClearPreviewBtns = sRect.removeFromTop(SettingsGuiValue::Settings::RowHeight);
    layoutRow({ .rowRect = rowClearPreviewBtns, .component = &clearWavePreviewsBtn, .rowHeight = SettingsGuiValue::Settings::RowHeight});
}

void GuiSettings::setSettings()
{
    // プロセッサから直に読む。引数を 18 個も並べていたときは、順番を
    // 間違えても、行を足し忘れても気づけなかった。
    languageSelector.setSelectedId(I18n::isJa() ? 1 : 2, juce::dontSendNotification);
    uiScaleSelector.setSelectedId(pluginOf(ctx).uiScaleIndex + 1, juce::dontSendNotification);
    fileFormatSelector.setSelectedId(pluginOf(ctx).fileFormatIndex + 1, juce::dontSendNotification);
    toggleAlignSelector.setSelectedId(pluginOf(ctx).toggleAlign + 1, juce::dontSendNotification);
    wallpaperModeSelector.setSelectedId(pluginOf(ctx).wallpaperMode + 1, juce::dontSendNotification);

    wallpaperPathLabel.setText(pluginOf(ctx).wallpaperPath.isEmpty()
        ? Io::empty : juce::File(pluginOf(ctx).wallpaperPath).getFileName(), juce::dontSendNotification);

    sampleDirPathLabel.setText(pluginOf(ctx).defaultSampleDir, juce::dontSendNotification);
    presetDirPathLabel.setText(pluginOf(ctx).defaultPresetDir, juce::dontSendNotification);
    wavetableDirPathLabel.setText(pluginOf(ctx).defaultWavetableDir, juce::dontSendNotification);
    fxOrderDirPathLabel.setText(pluginOf(ctx).defaultFxOrderDir, juce::dontSendNotification);
    fxParamDirPathLabel.setText(pluginOf(ctx).defaultFxParamDir, juce::dontSendNotification);
    channelParamDirPathLabel.setText(pluginOf(ctx).defaultChannelParamDir, juce::dontSendNotification);
    curveParamDirPathLabel.setText(pluginOf(ctx).defaultCurveParamDir, juce::dontSendNotification);
    lfoParamDirPathLabel.setText(pluginOf(ctx).defaultLfoParamDir, juce::dontSendNotification);
    ampEnvParamDirPathLabel.setText(pluginOf(ctx).defaultAmpEnvParamDir, juce::dontSendNotification);
    pitchEnvParamDirPathLabel.setText(pluginOf(ctx).defaultPitchEnvParamDir, juce::dontSendNotification);
    ssgSwEnvParamDirPathLabel.setText(pluginOf(ctx).defaultSsgSwEnvParamDir, juce::dontSendNotification);
    ssgHwEnvParamDirPathLabel.setText(pluginOf(ctx).defaultSsgHwEnvParamDir, juce::dontSendNotification);
    detuneParamDirPathLabel.setText(pluginOf(ctx).defaultDetuneParamDir, juce::dontSendNotification);
    unisonParamDirPathLabel.setText(pluginOf(ctx).defaultUnisonParamDir, juce::dontSendNotification);
    qualityParamDirPathLabel.setText(pluginOf(ctx).defaultQualityParamDir, juce::dontSendNotification);
    pcmPlayParamDirPathLabel.setText(pluginOf(ctx).defaultPcmPlayParamDir, juce::dontSendNotification);
    toneNoiseParamDirPathLabel.setText(pluginOf(ctx).defaultToneNoiseParamDir, juce::dontSendNotification);
    wtModParamDirPathLabel.setText(pluginOf(ctx).defaultWtModParamDir, juce::dontSendNotification);
    colorSettingDirPathLabel.setText(pluginOf(ctx).defaultColorSettingDir, juce::dontSendNotification);

    // 入り切りもプロセッサから読み直す。ここに無かったため、設定ファイルを
    // 読んでも画面のトグルだけが前の値のまま残っていた。
    simpleViewToggle.setToggleState(pluginOf(ctx).simpleView, juce::dontSendNotification);
    bypassHiddenBtn.setEnabled(pluginOf(ctx).simpleView);

    for (int i = 0; i < SimpleView::Size; ++i) {
        simpleViewShowToggles[(size_t)i].setToggleState(pluginOf(ctx).simpleViewShow[(size_t)i], juce::dontSendNotification);
    }

    tooltipToggle.setToggleState(pluginOf(ctx).showTooltips, juce::dontSendNotification);
    useHeadroomToggle.setToggleState(pluginOf(ctx).useHeadroom, juce::dontSendNotification);
    headroomGainSlider.setValue(pluginOf(ctx).headroomGain, juce::dontSendNotification);
    headroomGainSlider.setEnabledWithLabel(pluginOf(ctx).useHeadroom);
    liveDetuneToggle.setToggleState(pluginOf(ctx).liveDetune, juce::dontSendNotification);
    virtualMidiKeyboardToggle.setToggleState(pluginOf(ctx).showVirtualKeyboard, juce::dontSendNotification);
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
