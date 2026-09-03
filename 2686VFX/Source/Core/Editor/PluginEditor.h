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

#include "../../Gui/Fx/GuiFx.h"
#include "../../Gui/Settings/GuiSettings.h"
#include "../../Gui/About/GuiAbout.h"
#include "../../Gui/Colors/GuiColors.h"

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
        float cornerSize = 1.0f; // 角丸のサイズ（お好みで調整してください）

        // 背景の塗りつぶし
        juce::Colour baseColour = backgroundColour.darker(0.6f).withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.4f);

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
    void setupLogo();
    void setupMiniLogo();
    void setupTabs(juce::TabbedComponent& tabs);
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
    // フォルダを選ぶための開き方。
    //
    // ファイルを選ぶ開き方のままだと、Windows のダイアログはフォルダを
    // 開いても中へ入るだけで、そのフォルダ自体を決められない。
    void openFolderChooser(const juce::String title, const juce::File& file, const std::function<void(const FileChooser&)>& callback) {
        fileChooser = std::make_unique<juce::FileChooser>(title, file);
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories, callback);
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
    void loadWallpaperImage();
    void setTooltipState(bool enabled);

    // 区分の一括操作。SETTINGS のボタンと、画面右上の OP / CL から呼ぶ。
    // どのタブが対象になるかはタブ側が決める。
    void bypassHiddenCategories();
    void openEnabledCategories();
    void closeBypassedCategories();
    void assignTooltipsRecursive(juce::Component* parentComponent);
    void drawBg(juce::Graphics& g);
    void loadSettingsFile();

    // プリセット 1 件ぶんの見出しを読む。一覧を作るときと、履歴へ積む
    // ときの両方から使う。読めなければ false を返す。

    // 一覧の作り直し用の覚え書き。場所と大きさと更新日時が変わって
    // いなければ、そのファイルは開き直さない。
    std::map<juce::String, PresetItem> presetCache;

    // 仮想MIDIキーボード用
    void updateKeyboardVisibility();

    // 波形プレビュー用
    void timerCallback() override;
    void updateTimerState(bool start);
    void updatePreviewVisibilityToProcessor();
    bool keyPressed(const juce::KeyPress& key) override;
    void updateUiScale(float newScale);
    void resetMidiSettings();
    void updateFxOrder();
private:
    AudioPlugin2686V& audioProcessor;

    static inline constexpr int previewHz = 30;
    float uiScale = 1.0f;
    bool lastPlayingState = false; // 再生状態が変わったか判定するためのキャッシュ

    CustomTabLookAndFeel customTabLF;
    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };

    SystemButtonLF panicButtonLF;
    juce::TextButton panicButton;

    SystemButtonLF undoButtonLF;
    juce::TextButton undoButton;

    SystemButtonLF redoButtonLF;
    juce::TextButton redoButton;

    SystemButtonLF initParamsButtonLF;
    juce::TextButton initParamsButton;

    std::unique_ptr<juce::FileChooser> fileChooser;
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;

    SliderRegMap sliderRegMap;


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
    // 区分の一括開閉。簡易表示モードで隠す区分が対象。
    SystemButtonLF categoryToggleBtnLF;
    juce::TextButton openCategoriesBtn;
    juce::TextButton closeCategoriesBtn;

    // 開いているタブに限らず、持っているタブすべてへ配る。
    void forEachTabGui(const std::function<void(GuiBase&)>& fn);

    SystemButtonLF miniToggleBtnLF;
    juce::TextButton toggleMiniBtn;
    juce::ImageComponent mainIconImage;
    juce::ImageComponent miniIconImage;

    std::unique_ptr<GuiFx> fxGui; // FX
    std::unique_ptr<GuiSettings> settingsGui;
    std::unique_ptr<GuiAbout> aboutGui;
    std::unique_ptr<GuiColors> colorsGui;

    juce::Image backgroundImage; // Cache for wallpaper
    juce::Image blurredBackgroundImage; // ぼかし背景用のキャッシュ

    void updateUndoRedoButtons(); // アンドゥ・リドゥボタンの状態を更新する専用の関数
    void updateParameterInitializeButtons(); // パラメーター初期化ボタンの状態を更新する専用の関数

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
