#pragma once
#include <JuceHeader.h>
#include "../../Gui/Preset/PresetLibrary.h"
#include <array>
#include <vector>
#include <span>

#include "../Processor/PluginProcessor.h"
#include "../Gui/GuiLF.h"
#include "../Gui/GuiLazy.h"
#include "../Gui/GuiContext.h"
#include "./EditorGuiText.h"
#include "./EditorGuiValues.h"

#include "../../Gui/Ssg/GuiSsg.h"
#include "../../Gui/Beep/GuiBeep.h"
#include "../../Gui/Preset/GuiPreset.h"
#include "../../Gui/Fx/GuiFx.h"
#include "../../Gui/Settings/GuiSettings.h"
#include "../../Gui/About/GuiAbout.h"
#include "../../Gui/Colors/GuiColors.h"
#include "../../Gui/Curve/GuiCurve.h"

#include "../../Gui/Components/Loading/GuiLoading.h"
#include "../../Gui/Components/GenWave/GenWave.h"
#include "../../Gui/Components/ParamBrowser/ParamBrowser.h"

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

// タブが切り替わったことを外へ知らせるだけの薄い被せもの。
// juce::TabbedComponent は切り替えを知らせる口を持たないので、ここで足す。
class GuiTabs : public juce::TabbedComponent
{
public:
    using juce::TabbedComponent::TabbedComponent;

    std::function<void(int)> onTabChanged;

    void currentTabChanged(int newIndex, const juce::String&) override
    {
        if (onTabChanged) onTabChanged(newIndex);
    }
};

class AudioPlugin2686VEditor :
    public juce::AudioProcessorEditor,
    public juce::ChangeListener,
    public juce::ComponentListener,
    public juce::Button::Listener,
    public juce::AudioProcessorValueTreeState::Listener,
    public juce::MultiTimer,
    public juce::AsyncUpdater
{
public:
    AudioPlugin2686VEditor(AudioPlugin2686V&);
    ~AudioPlugin2686VEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // 時間の掛かる処理の間、画面全体を覆って待たせる。
    //
    // 覆っている間は下の画面を触れない。処理そのものは別のスレッドで
    // 進めること。メッセージスレッドを塞ぐと棒が止まってしまう。
    // onCancel を渡すと中止ボタンが出る。押されたら呼ばれるだけなので、
    // 止めた側が hideLoading() まで面倒を見ること。
    void showLoading(const juce::String& message = {},
        std::function<void()> onCancel = nullptr);
    void updateLoading(const juce::String& message);
    void hideLoading();

    // パラメータファイルのブラウザを開く。
    //
    // allowed には、呼んだ側が読める区分の名前を渡す。それ以外の行は
    // 一覧には出るが選べない。選ばれたら onChoose が呼ばれる。
    void openParamBrowser(const juce::StringArray& allowed,
        std::function<void(const juce::File&)> onChoose);

    // 区分ごとに置き場が分かれているので、どこから始めるかを渡す形も
    // 用意しておく。根は settingsDir ごとに別に覚える。
    void openParamBrowser(const juce::String& settingsDir, const juce::StringArray& allowed,
        std::function<void(const juce::File&)> onChoose,
        const juce::String& nameMustContain = {});

    // 書き出す先を一覧から決める。base は "param.opna" のような種類の
    // 名前で、既定のファイル名と付ける拡張子はそこから組み立てる。
    void openParamBrowserToSave(const juce::String& settingsDir, const juce::StringArray& allowed,
        const juce::String& base, std::function<void(const juce::File&)> onChoose,
        const juce::String& nameMustContain = {});

    // 波形ファイルのブラウザ。読み込むときと、書き出す先を決めるとき。
    // 根はパラメータとは別に覚える。置き場が違うため。
    void openWaveBrowser(const juce::StringArray& allowed,
        std::function<void(const juce::File&)> onChoose);

    // 音声ファイルのブラウザ。PCM の素材を選ぶのに使う。
    void openAudioBrowser(std::function<void(const juce::File&)> onChoose);
    void openWaveBrowserToSave(const juce::StringArray& allowed,
        const juce::String& defaultName, const juce::String& extension,
        std::function<void(const juce::File&)> onChoose);

    // 区分に合うタブへ読ませる。合うタブが無ければ false。
    // そのファイルのための音源へ切り替えたうえで読む。
    bool applyChannelParamFile(const juce::File& file);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;
    void buttonClicked(juce::Button* button) override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // parameterChanged はホストのオートメーション中はオーディオスレッドで走る。
    // 番号だけここへ預けて、実際の切り替えはメッセージスレッド側で行う。
    std::atomic<int> m_pendingModeTab{ -1 };
    void handleAsyncUpdate() override;

    void setupLogo();
    void setupMiniLogo();
    void setupTabs(juce::TabbedComponent& tabs);
    void drawBg(juce::Graphics& g);
    void loadSettingsFile();
    void loadPresetFile(const juce::File& file);
    void scanPresets();

    // プリセット 1 件ぶんの見出しを読む。一覧を作るときと、履歴へ積む
    // ときの両方から使う。読めなければ false を返す。
    // 見出しが無いときに使う既定値。
    //
    // 裏のスレッドからプロセッサを触らずに済むよう、読み始める前に
    // ここへ写しておく。
    struct PresetMetaDefaults
    {
        juce::String name;
        juce::String author;
        juce::String version;
        juce::String comment;
        juce::String mode;
        juce::String genre;
    };

    PresetMetaDefaults presetMetaDefaults() const;

    // ファイルを開いて見出しだけ拾う。プロセッサを触らないので、
    // 裏のスレッドから呼んでよい。
    static bool readPresetMetaInto(const juce::File& file, PresetItem& item,
        const PresetMetaDefaults& defaults);

    bool readPresetMeta(const juce::File& file, PresetItem& item);

    // ------------------------------------------------------------------
    // 一覧の見出しの控え
    // ------------------------------------------------------------------
    // 名前や作者は、プリセットを開いて中身を見ないと分からない。ところが
    // 2686V のプリセットは 1 件で 9,000 近い値を持つので、開くだけで
    // 十数ミリ秒かかる。2,000 件あれば 30 秒を超え、画面が出るまで
    // ずっと待たされていた。
    //
    // 一度読んだものはファイルへ控えておき、次からは日付と大きさが
    // 変わっていないものを開き直さない。控えは作り直せるものしか
    // 入れていないので、消してしまってもかまわない。
    juce::File presetIndexFile() const;
    std::map<juce::String, PresetItem> readPresetIndex() const;
    void savePresetIndex() const;

    // まだ読んでいないぶんを裏で読む。読み終えるまで一覧には
    // ファイル名だけが並ぶ。
    void startPresetMetaScan(std::vector<juce::File> files);
    void finishPresetMetaScan(const std::vector<PresetItem>& done);
    void cancelPresetMetaScan();

    std::unique_ptr<juce::ThreadPool> presetScanPool;
    std::shared_ptr<std::atomic<bool>> presetScanCancel;

    // 一覧の作り直し用の覚え書き。場所と大きさと更新日時が変わって
    // いなければ、そのファイルは開き直さない。
    std::map<juce::String, PresetItem> presetCache;

    // お気に入りと履歴。プリセットの一覧とは別の情報なので、
    // プラグインのフォルダへ別のファイルとして持つ。
    PresetLibrary presetLibrary;
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
    void setTooltipState(bool enabled);

    // 区分の一括操作。SETTINGS のボタンと、画面右上の OP / CL から呼ぶ。
    // どのタブが対象になるかはタブ側が決める。
    void bypassHiddenCategories();
    void openEnabledCategories();
    void closeBypassedCategories();
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
    // タイマーは 2 つに分ける。波形はプレビューを出している間だけ、
    // 再生ランプは画面が開いている間ずっと。以前は 1 つで兼ねていたため、
    // 波形を止めるとランプも止まってしまい、結局止められなかった。
    enum TimerId { previewTimer = 0, playingLampTimer = 1 };

    void timerCallback(int timerID) override;

    // いまの表示状態を見て、波形用タイマーの入切を決める。
    void updateTimerState();
    void updatePreviewVisibilityToProcessor();
    bool keyPressed(const juce::KeyPress& key) override;
    void updateUiScale(float newScale);
    void resetMidiSettings();
    void breadcastLevel(float level);
    void updateFxOrder();
private:
    AudioPlugin2686V& audioProcessor;

    static inline constexpr int previewHz = 30;
    static inline constexpr int playingLampHz = 30;
    float uiScale = 1.0f;
    bool lastPlayingState = false; // 再生状態が変わったか判定するためのキャッシュ

    CustomTabLookAndFeel customTabLF;
    GuiTabs tabs{ juce::TabbedButtonBar::TabsAtTop };

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


    GuiLazy<GuiSsg> ssgGui; // SSG
    GuiLazy<GuiBeep> beepGui;
    std::unique_ptr<GuiPreset> presetGui;
    GuiLazy<GuiCurve> curveGui;

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

    // オシロの下に置く、押したときだけ作る波形プレビュー
    std::unique_ptr<GuiGenWave> genWaveGui;

    // パラメータファイルの一覧。開いている間だけ画面を覆う。
    std::unique_ptr<GuiParamBrowser> paramBrowser;

    // ブラウザを開く根。
    //
    // 読み込むたびに defaultChannelParamDir は「今読んだファイルのある
    // フォルダ」へ狭まる。ダイアログなら上へ辿れたが、ブラウザは根の下
    // しか見ないので、そのまま使うと次に別の区分を読もうとしたときに
    // 1 件も出てこない。ここは狭めず、「フォルダ…」でだけ動かす。
    juce::File paramBrowserRoot;
    juce::File waveBrowserRoot;

    // 区分ごとの根。置き場が分かれているので、設定の場所ごとに覚える。
    std::map<juce::String, juce::File> browserRoots;

    bool isPreviewVisible = false;

    // 待ち時間を知らせる覆い。出している間だけ動く。
    GuiLoading loadingScreen;

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

    // ------------------------------------------------------------------
    // タブの後作り
    // ------------------------------------------------------------------
    // タブの並び。setupTabs の登録順と揃えてある。
    enum TabIndex
    {
        tabSsg = 0, tabBeep, tabCurve, tabPreset, tabSettings, tabColors, tabAbout,

        tabCount
    };

    // 中身を入れる器。中身が出来るまでは空のまま置いておく。
    std::array<GuiTabHost, tabCount> tabHosts;

    // 配置し直しが要るタブの印。裏に隠れている間は溜めておく。
    std::array<bool, tabCount> tabNeedsLayout{};

    // 指定のタブ 1 枚だけ配置する。
    void layoutTab(int tabIndex);

    // 直近の resized() で決めたタブの中身の矩形。
    // 後から作ったタブは resized() を待たずにここへ合わせる。
    juce::Rectangle<int> lastTabContent;

    // 作り方を仕込む。ここで入れた手順が、そのタブが最初に要るときに走る。
    void setupLazyTabs();

    // 音源側の実体をここで触ると、取り込みの順によっては型が揃っていない。
    // 触るところは .cpp へ逃がしてある。
    GuiContext makeGuiContext();
    juce::String currentPresetName() const;

    // 1 つぶんの仕込み。extra には、そのタブだけに要る後始末を入れる。
    template <typename T>
    void prepareLazyTab(GuiLazy<T>& slot, int tabIndex, std::function<void(T&)> extra = {})
    {
        slot.onConstruct = [this] {
            return std::make_unique<T>(makeGuiContext());
            };

        slot.onSetup = [this, tabIndex, extra](T& gui) {
            finishTabGui(gui, tabIndex);

            if (extra) extra(gui);
            };
    }

    // そのタブの中身を作る。すでに出来ていれば何もしない。
    void materializeTab(int tabIndex);

    // タブ番号から中身を引く。まだ作っていなければここで作る。
    GuiBase* tabGuiFor(int tabIndex);

    // 区分の名前からタブ番号を引く。合うものが無ければ -1。
    static int tabForCategory(const juce::String& category);

    // すべてのタブの中身を作る。パラメータを書き換える一括操作は、
    // 開いていないタブにも効かないと困るので、そこだけはここを通す。
    void materializeAllTabs();

    // OP / CL の押し下げを覚えておく。
    //
    // これは見た目だけの操作なので、開いていないタブへ先回りして
    // 作りに行く必要はない。後でそのタブを開いたときに同じ形にする。
    enum class CategoryBulk { none, open, close };
    CategoryBulk lastCategoryBulk = CategoryBulk::none;

    // 開いていないタブのレベルを、GUI を作らずに入れる。
    void setLevelParam(const juce::String& prefix, float level);

    // 作った直後にやること。
    //
    // setup を済ませ、ツールチップを付け、器へ入れて位置を決める。
    // ここで resized() を呼ばないのは、まだ出来上がっていない自分自身を
    // 触りに来てしまうため。位置は覚えてある矩形から直に決める。
    template <typename T>
    void finishTabGui(T& gui, int tabIndex)
    {
        gui.setup();

        if constexpr (requires (T& g) { g.updatePresetName(juce::String()); })
        {
            gui.updatePresetName(currentPresetName());
        }

        assignTooltipsRecursive(&gui);

        // 一括で開け閉めした後に作られたタブも、同じ形で出す。
        if (lastCategoryBulk == CategoryBulk::open) gui.openEnabledCategories();
        else if (lastCategoryBulk == CategoryBulk::close) gui.closeBypassedCategories();

        tabHosts[(size_t)tabIndex].setContent(gui);

        if (!lastTabContent.isEmpty()) gui.layout(lastTabContent);
    }


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

    void copyFmParamsToString();
    void initParams();

    void showFullView();
    void showMiniPlayerView();
    void showMinimumView();
    void updateWindowSize();

    // いまの表示モードで本来あるべき窓の大きさ。
    // updateWindowSize と resized の両方がこれを見る。別々に書いていたころは
    // 値が食い違い、切り替えた直後に一度違う大きさで置いてから直していた。
    juce::Point<int> getExpectedSize() const;

    inline juce::String getPreviewButtonText();
    inline juce::String getPreviewTooltipText();
    inline juce::String getUndoTooltipText();
    inline juce::String getRedoTooltipText();

    void parentHierarchyChanged() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686VEditor)
};
