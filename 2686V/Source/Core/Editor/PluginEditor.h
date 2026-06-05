#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>
#include <span>

#include "../Processor/PluginProcessor.h"
#include "../Fm/SliderRegMap.h"
#include "../Gui/GuiLF.h"
#include "./GuiText.h"

#include "../../Gui/Opna/Core.h"
#include "../../Gui/Opn/Core.h"
#include "../../Gui/Opl/Core.h"
#include "../../Gui/Opl3/Core.h"
#include "../../Gui/Opm/Core.h"
#include "../../Gui/Opzx7/Core.h"
#include "../../Gui/Ssg/Core.h"
#include "../../Gui/Wavetable/Core.h"
#include "../../Gui/Rhythm/Core.h"
#include "../../Gui/Adpcm/Core.h"
#include "../../Gui/Beep/Core.h"
#include "../../Gui/Preset/Core.h"
#include "../../Gui/Fx/Core.h"
#include "../../Gui/Settings/Core.h"
#include "../../Gui/About/Core.h"
#include "../../Gui/Curve/Core.h"

class SystemButtonLF : public juce::LookAndFeel_V4
{
public:
    juce::Font buttonFont{ 20.0f, 1 };

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
        juce::Colour baseColour = backgroundColour.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

        // マウスホバー時やクリック時は少し明るくする
        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.1f);

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
    void updateAdpcmFileName(const juce::String finename);
    void updateOpzx7FileNames(const juce::String finename);
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
    void updateTimerState();
    void updatePreviewVisibilityToProcessor();
    bool keyPressed(const juce::KeyPress& key) override;
    GuiCurve* getCurveGui();
private:
    AudioPlugin2686V& audioProcessor;

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
    GuiWaveformPreview realtimePreview{ juce::Colour(0xff0a3a1a), juce::Colours::lightgreen };
    bool isPreviewVisible = false;

    bool isMiniPlayerMode = false;
    juce::Label miniPresetLabel;
    juce::Label miniModeLabel;
    SystemButtonLF miniToggleBtnLF;
    juce::TextButton toggleMiniBtn{ "M" }; // 切り替えボタン
    juce::Label miniLogoLabel;

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

    inline juce::String getPreviewButtonText();
    inline juce::String getPreviewTooltipText();
    inline juce::String getUndoTooltipText();
    inline juce::String getRedoTooltipText();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686VEditor)
};
