#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>
#include <span>
#include "../processor/PluginProcessor.h"
#include "../fm/SliderRegMap.h"
#include "../gui/GuiLF.h"
#if !defined(BUILD_AS_FX_PLUGIN)
#include "../OPNA/GuiOpna.h"
#include "../OPN/GuiOpn.h"
#include "../OPL/GuiOpl.h"
#include "../OPL3/GuiOpl3.h"
#include "../OPM/GuiOpm.h"
#include "../OPZX3/GuiOpzx3.h"
#include "../SSG/GuiSsg.h"
#include "../WT/GuiWt.h"
#include "../RHYTHM/GuiRhythm.h"
#include "../ADPCM/GuiAdpcm.h"
#include "../BEEP/GuiBeep.h"
#include "../PRESET/GuiPreset.h"
#endif
#include "../FX/GuiFx.h"
#include "../SETTINGS/GuiSettings.h"
#include "../ABOUT/GuiAbout.h"

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
    void mouseDown(const juce::MouseEvent& event) override;
    void showRegisterInput(juce::Component* targetComp, std::function<void(int)> onValueEntered);
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void updateRhythmFileNames(const juce::String finename);
    void updateAdpcmFileName(const juce::String finename);
    void updateOpzx3FileNames(const juce::String finename);
    void setupLogo();
    void setupTabs(juce::TabbedComponent& tabs);
    void drawBg(juce::Graphics& g);
    void loadSettingsFile();
#if !defined(BUILD_AS_FX_PLUGIN)
    void loadPresetFile(const juce::File& file);
    void scanPresets();
    void saveCurrentPreset();
    void saveCurrentPresetAs();
    void updatePresetNameToTabs(const juce::String& pName);
#endif
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
#if !defined(BUILD_AS_FX_PLUGIN)
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
#endif

    // 波形プレビュー用
    void timerCallback() override;
    void updateTimerState();
    void updatePreviewVisibilityToProcessor();
private:
    AudioPlugin2686V& audioProcessor;

    CustomTabLookAndFeel customTabLF;

    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };
	juce::Label logoLabel;
    juce::TextButton panicButton;
    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;

    SliderRegMap sliderRegMap;

#if !defined(BUILD_AS_FX_PLUGIN)
    std::unique_ptr<GuiOpna> opnaGui;  // OPNA
    std::unique_ptr<GuiOpn> opnGui; // OPN
    std::unique_ptr<GuiOpl> oplGui; // OPL
    std::unique_ptr<GuiOpl3> opl3Gui; // OPL3
    std::unique_ptr<GuiOpm> opmGui; // OPM
    std::unique_ptr<GuiOpzx3> opzx3Gui; // OPZX3
    std::unique_ptr<GuiSsg> ssgGui; // SSG
    std::unique_ptr<GuiWt> wtGui; // Wavetable
    std::unique_ptr<GuiRhythm> rhythmGui; // Rhythm
    std::unique_ptr<GuiAdpcm> adpcmGui; // ADPCM
    std::unique_ptr<GuiBeep> beepGui;
    std::unique_ptr<GuiPreset> presetGui;

    // 仮想MIDIキーボード用
    std::unique_ptr<juce::MidiKeyboardComponent> midiKeyboard;
#endif
    // 波形プレビュー用
    juce::TextButton togglePreviewBtn{ ">>" }; // 初期状態は閉じているので ">>"
    // 青系のリアルタイムプレビュー
    GuiWaveformPreview staticPreview{ juce::Colour(0xff0a1a3a), juce::Colours::cyan };
    // 緑系のリアルタイムプレビュー
    GuiWaveformPreview realtimePreview{ juce::Colour(0xff0a3a1a), juce::Colours::lightgreen };
    bool isPreviewVisible = false;

    std::unique_ptr<GuiFx> fxGui; // FX
    std::unique_ptr<GuiSettings> settingsGui;
    std::unique_ptr<GuiAbout> aboutGui;

    juce::Image backgroundImage; // Cache for wallpaper
    juce::Image blurredBackgroundImage; // ぼかし背景用のキャッシュ

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686VEditor)
};
