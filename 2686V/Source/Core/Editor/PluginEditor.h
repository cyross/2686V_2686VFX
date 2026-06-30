#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>
#include <span>

#include "../Processor/PluginProcessor.h"
#include "../Fm/FmSliderRegMap.h"
#include "../Gui/GuiLF.h"
#include "./EditorGuiText.h"
#include "./EditorGuiValues.h"

#include "../../Gui/Opna/GuiOpna.h"
#include "../../Gui/Opn/GuiOpn.h"
#include "../../Gui/Opl/GuiOpl.h"
#include "../../Gui/Opl3/GuiOpl3.h"
#include "../../Gui/Opm/GuiOpm.h"
#include "../../Gui/Opzx7/GuiOpzx7.h"
#include "../../Gui/Ssg/GuiSsg.h"
#include "../../Gui/Wavetable/GuiWt.h"
#include "../../Gui/Wt2/GuiWt2.h"
#include "../../Gui/Rhythm/GuiRhythm.h"
#include "../../Gui/Adpcm/GuiAdpcm.h"
#include "../../Gui/Beep/GuiBeep.h"
#include "../../Gui/Preset/GuiPreset.h"
#include "../../Gui/Fx/GuiFx.h"
#include "../../Gui/Settings/GuiSettings.h"
#include "../../Gui/About/GuiAbout.h"
#include "../../Gui/Curve/GuiCurve.h"

#include "../../Core/Gui/GuiCopyObj.h"

class SystemButtonLF : public juce::LookAndFeel_V4
{
public:
    juce::Font buttonFont = juce::Font(juce::FontOptions(20.0f, 1));

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        return buttonFont;
    }

    // =======================================================
    // ボタンの背景と枠線の描画を完全にコントロールする
    // =======================================================
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        float cornerSize = 2.0f; // 角丸のサイズ（お好みで調整してください）

        // 背景の塗りつぶし
        juce::Colour baseColour = backgroundColour.darker(0.6f).withMultipliedAlpha(button.isEnabled() ? 0.7f : 0.4f);

        // マウスホバー時やクリック時は少し明るくする
        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.5f);

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);
    }
};

class AudioPlugin2686VEditor :
    public juce::AudioProcessorEditor,
    public juce::ChangeListener,
    public juce::ComponentListener,
    public juce::Button::Listener,
    public juce::AudioProcessorValueTreeState::Listener,
    public juce::Timer
{
public:
    AudioPlugin2686VEditor(AudioPlugin2686V&);
    ~AudioPlugin2686VEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;
    void buttonClicked(juce::Button* button) override;
    void showRegisterInput(juce::Component* targetComp, std::function<void(int)> onValueEntered);
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void updateRhythmFileNames(const juce::String finename);
    void updateAdpcmFileNames(const juce::String finename);
    void updateOpzx7PcmFileNames(const juce::String finename);
    void updateOpzx7WtFileNames(const juce::String finename);
    void setupLogo();
    void setupMiniLogo();
    void setupTabs(juce::TabbedComponent& tabs);
    void drawBg(juce::Graphics& g);
    void loadSettingsFile();
    void loadPresetFile(const juce::File& file);
    void scanPresets();
    void saveCurrentPreset();
    void saveCurrentPresetAs();
    void updatePresetNameToTabs(const juce::String& pName);
    void loadWallpaperImage();
    void openFileChooser(const juce::String title, const juce::String filter, const std::function<void(const FileChooser&)>& callback) {
        fileChooser = std::make_unique<juce::FileChooser>(title, juce::File(), filter);
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, callback);
    }
    void openFileChooser(const juce::String title, const juce::File& file, const std::function<void(const FileChooser&)>& callback) {
        fileChooser = std::make_unique<juce::FileChooser>(title, file);
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, callback);
    }
    void openFileChooser(const juce::String title, const juce::File& file, const juce::String filter, const std::function<void(const FileChooser&)>& callback) {
        fileChooser = std::make_unique<juce::FileChooser>(title, file, filter);
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, callback);
    }
    void openWriteFileChooser(const juce::String title, const juce::String filter, const std::function<void(const FileChooser&)>& callback) {
        fileChooser = std::make_unique<juce::FileChooser>(title, juce::File(), filter);
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting, callback);
    }
    void openWriteFileChooser(const juce::String title, const juce::File& file, const std::function<void(const FileChooser&)>& callback) {
        fileChooser = std::make_unique<juce::FileChooser>(title, file);
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting, callback);
    }
    void openWriteFileChooser(const juce::String title, const juce::File& file, const juce::String filter, const std::function<void(const FileChooser&)>& callback) {
        fileChooser = std::make_unique<juce::FileChooser>(title, file, filter);
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting, callback);
    }
    void setTooltipState(bool enabled);
    void assignTooltipsRecursive(juce::Component* parentComponent);
    void setPresetDir(const juce::File& dir)
    {
		presetGui->currentFolder = dir;
    }
    void updatePresetPath()
    {
        presetGui->updatePresetPath();
	}

    // 仮想MIDIキーボード用
    void updateKeyboardVisibility();

    // 波形プレビュー用
    void timerCallback() override;
    void updateTimerState(bool start);
    void updatePreviewVisibilityToProcessor();
    bool keyPressed(const juce::KeyPress& key) override;
    GuiCurve* getCurveGui();
    void updateUiScale(float newScale);
    void resetMidiSettings();
    void breadcastLevel(float level);
    void copyRhythmPadParams(int from, int to);
    void copyOplOpParams(int from, int to);
    void copyOpl3OpParams(int from, int to);
    void copyOpmOpParams(int from, int to);
    void copyOpnOpParams(int from, int to);
    void copyOpnaOpParams(int from, int to);
    void copyOpzx7OpParams(int from, int to);
    void copyOplParamsToOpl3();
    void copyOplParamsToOpl312();
    void copyOplParamsToOpl334();
    void copyOpl3ParamsToOpl();
    void copyOpl312ParamsToOpl();
    void copyOpl334ParamsToOpl();
    void copyOpnParamsToOpna();
    void copyOpnaParamsToOpn();
    void copyOpnaParamsToOpm();
    void copyOpmParamsToOpna();
    void copyOpnParamsToOpm();
    void copyOpmParamsToOpn();
    void updateFxOrder();
private:
    AudioPlugin2686V& audioProcessor;

    static inline constexpr int previewHz = 30;
    float uiScale = 1.0f;
    bool lastPlayingState = false; // 再生状態が変わったか判定するためのキャッシュ

    CustomTabLookAndFeel customTabLF;
    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };
	juce::Label logoLabel;

    SystemButtonLF panicButtonLF;
    juce::TextButton panicButton;

    SystemButtonLF undoButtonLF;
    juce::TextButton undoButton;

    SystemButtonLF redoButtonLF;
    juce::TextButton redoButton;

    SystemButtonLF copyParamsButtonLF;
    juce::TextButton copyParamsButton;

    SystemButtonLF pasteParamsButtonLF;
    juce::TextButton pasteParamsButton;

    SystemButtonLF initParamsButtonLF;
    juce::TextButton initParamsButton;

    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;

    SliderRegMap sliderRegMap;

    std::unique_ptr<GuiOpna> opnaGui;  // OPNA
    std::unique_ptr<GuiOpn> opnGui; // OPN
    std::unique_ptr<GuiOpl> oplGui; // OPL
    std::unique_ptr<GuiOpl3> opl3Gui; // OPL3
    std::unique_ptr<GuiOpm> opmGui; // OPM
    std::unique_ptr<GuiOpzx7> opzx7Gui; // OPZX7
    std::unique_ptr<GuiSsg> ssgGui; // SSG
    std::unique_ptr<GuiWt> wtGui; // Wavetable
    std::unique_ptr<GuiWt2> wt2Gui; // Wt2
    std::unique_ptr<GuiRhythm> rhythmGui; // Rhythm
    std::unique_ptr<GuiAdpcm> adpcmGui; // ADPCM
    std::unique_ptr<GuiBeep> beepGui;
    std::unique_ptr<GuiPreset> presetGui;
    std::unique_ptr<GuiCurve> curveGui;

    // 仮想MIDIキーボード用
    std::unique_ptr<juce::MidiKeyboardComponent> midiKeyboard;

    // 波形プレビュー用
    SystemButtonLF togglePreviewButtonLF;
    juce::TextButton togglePreviewBtn{ EditorGuiText::Preview::show }; // 初期状態は閉じているので ">>"

    // 緑系のリアルタイムプレビュー
    juce::Label previewTitleLabel;
    juce::Label previewLabels[3];
    GuiWaveformPreview realtimePreviewL{ juce::Colours::white.darker(0.2f).withAlpha(0.5f), juce::Colours::blue.brighter(0.1f) };
    GuiWaveformPreview realtimePreviewMono{ juce::Colours::darkgreen.darker(0.8f).withAlpha(0.5f), juce::Colours::green.brighter(0.5f) };
    GuiWaveformPreview realtimePreviewR{ juce::Colours::white.darker(0.2f).withAlpha(0.5f), juce::Colours::red };

    // 状態コンポーネント
    GuiStateView playingState{ juce::Colours::yellow, juce::Colours::yellow.darker(0.9f).withAlpha(0.6f) };

    bool isPreviewVisible = false;

    enum class ViewMode { Full = 0, MiniPlayer = 1, Minimum = 2 };
    ViewMode viewMode = ViewMode::Full;
    juce::Label miniPresetLabel;
    juce::Label miniModeLabel;
    SystemButtonLF miniToggleBtnLF;
    juce::TextButton toggleMiniBtn;
    juce::Label miniLogoLabel;
    juce::ImageComponent mainIconImage;
    juce::ImageComponent miniIconImage;

    std::unique_ptr<GuiFx> fxGui; // FX
    std::unique_ptr<GuiSettings> settingsGui;
    std::unique_ptr<GuiAbout> aboutGui;

    juce::Image backgroundImage; // Cache for wallpaper
    juce::Image blurredBackgroundImage; // ぼかし背景用のキャッシュ

    void updateUndoRedoButtons(); // アンドゥ・リドゥボタンの状態を更新する専用の関数
    void updateParameterCopyPasteButtons(); // パラメーターコピー・ペーストボタンの状態を更新する専用の関数
    void updateParameterInitializeButtons(); // パラメーター初期化ボタンの状態を更新する専用の関数

    void copyFmParamsToString();
    void copyFmParamsToObject();
    void pasteFmParamsFromObject();
    void initParams();

    void showFullView();
    void showMiniPlayerView();
    void showMinimumView();
    void updateWindowSize();

    inline juce::String getPreviewButtonText();
    inline juce::String getPreviewTooltipText();
    inline juce::String getUndoTooltipText();
    inline juce::String getRedoTooltipText();

    void parentHierarchyChanged() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686VEditor)
};
