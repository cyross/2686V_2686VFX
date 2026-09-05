#include "../Gui/GuiRefresh.h"
#include <cstdio>
#include <vector>
#include <initializer_list>
#include <utility>

#include "./PluginEditor.h"
#include "../../Gui/Settings/SettingsValues.h"

#include "../Processor/PluginProcessor.h"

#include "../Processor/ProcessorKeys.h"
#include "../Const/ConstFileValues.h"
#include "../../Gui/Preset/PresetKeys.h"

#include "../Fm/FmSliderRegMap.h"
#include "../Fm/FmRegisterConverter.h"

#include "./EditorGuiValues.h"
#include "../Gui/GuiColor.h"
#include "../Gui/GuiContext.h"

// チャンネルごとのパラメータ名の頭。開いていないタブへ値を入れるときに使う。
#include "../../Processor/Opn/ProcessorOpnKeys.h"
#include "../../Processor/Ssg/ProcessorSsgKeys.h"

#include "AppIconForAbout.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat presetFormat{ "preset", 1 };
}

AudioPlugin2686VEditor::AudioPlugin2686VEditor(AudioPlugin2686V& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    int mode = audioProcessor.apvts.state.getProperty(ProcessorStateKey::windowMode, (int)ViewMode::Full);
    viewMode = (ViewMode)mode;

    GuiContext context(audioProcessor, *this, audioProcessor.apvts, sliderRegMap);

    // タブの中身は開かれるまで作らない。作り方だけ先に入れておく。
    setupLazyTabs();
    presetGui = std::make_unique<GuiPreset>(context);
    fxGui = std::make_unique<GuiFx>(context);
	settingsGui = std::make_unique<GuiSettings>(context);
	aboutGui = std::make_unique<GuiAbout>(context);
	colorsGui = std::make_unique<GuiColors>(context);

    tabs.getTabbedButtonBar().addChangeListener(this);

    // 色の差し替えを受けて描き直す
    GuiColor::changeBroadcaster().addChangeListener(this);

    audioProcessor.apvts.addParameterListener(CPK::mode, this);

    setupLogo();
    setupMiniLogo();

    presetGui->setup();
    fxGui->setup();
    settingsGui->setup();
    colorsGui->setup();
    aboutGui->setup();

    // Initial Wallpaper Load
    loadWallpaperImage();

    isPreviewVisible = audioProcessor.apvts.state.getProperty(ProcessorStateKey::isVisiblePreview);

    // ダイアログの色は共有の LookAndFeel に入るので、1 度だけ設定する
    GuiDialog::applyTheme();

    tabs.setLookAndFeel(&customTabLF);

    setupTabs(tabs);

    // 裏で溜めておいた配置を、そのタブが前へ出たときに 1 回だけ流す。
    tabs.onTabChanged = [this](int newIndex) {
        if (newIndex >= 0 && newIndex < tabCount && tabNeedsLayout[(size_t)newIndex]) {
            layoutTab(newIndex);
        }
    };

    int currentMode = (int)*audioProcessor.apvts.getRawParameterValue(CPK::mode);
    tabs.setCurrentTabIndex(currentMode);

    // 開いているタブだけは、ここで作っておく。
    // タブの切り替えの知らせは後回しで届くので、待つと一瞬空になる。
    materializeTab(currentMode);

    // 1. 全スライダーにツールチップ(範囲)を自動割り当て
    for (int i = 0; i < tabs.getNumTabs(); ++i)
    {
        // 各タブの中身（opnaGui.page, ssgGui.page など）を取得
        if (auto* contentComp = tabs.getTabContentComponent(i))
        {
            assignTooltipsRecursive(contentComp);
        }
    }

    // 2. 保存された設定に基づいてON/OFF初期化
    setTooltipState(audioProcessor.showTooltips);

    // お気に入りと履歴を読む。一覧を作る前に済ませておく。
    presetLibrary.openIn(audioProcessor.getPluginDirectory(), PresetValue::File::Name::library);

    if (presetGui->currentFolder.isDirectory()) {
        scanPresets();
    }

    addChildComponent(realtimePreviewL);
    addChildComponent(realtimePreviewMono);
    addChildComponent(realtimePreviewR);

    // 波形プレビューラベル
    addChildComponent(previewTitleLabel);
    previewTitleLabel.setText(EditorGuiText::Preview::label, juce::NotificationType::dontSendNotification);
    previewTitleLabel.setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
    previewTitleLabel.setColour(juce::Label::textColourId, juce::Colours::white);

    addChildComponent(previewLabels[0]);
    previewLabels[0].setText(EditorGuiText::Preview::labelL, juce::NotificationType::dontSendNotification);
    previewLabels[0].setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
    previewLabels[0].setColour(juce::Label::textColourId, juce::Colours::white);

    addChildComponent(previewLabels[1]);
    previewLabels[1].setText(EditorGuiText::Preview::labelMono, juce::NotificationType::dontSendNotification);
    previewLabels[1].setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
    previewLabels[1].setColour(juce::Label::textColourId, juce::Colours::white);

    addChildComponent(previewLabels[2]);
    previewLabels[2].setText(EditorGuiText::Preview::labelR, juce::NotificationType::dontSendNotification);
    previewLabels[2].setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
    previewLabels[2].setColour(juce::Label::textColourId, juce::Colours::white);

    addAndMakeVisible(playingState);

    // 再生ランプは画面が開いている間ずっと見る。中身は状態が変わったときだけ描く。
    startTimer(playingLampTimer, 1000 / playingLampHz);

    // プレビュー表示切替ボタン
    addAndMakeVisible(togglePreviewBtn);
    togglePreviewBtn.setButtonText(getPreviewButtonText());
    togglePreviewBtn.setTooltip(getPreviewTooltipText());
    togglePreviewBtn.setLookAndFeel(&togglePreviewButtonLF);
    togglePreviewBtn.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    togglePreviewBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::white.darker(0.5f));
    togglePreviewBtn.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    togglePreviewBtn.onClick = [this] {
        isPreviewVisible = !isPreviewVisible;

        // 再起動時にプレビュー表示を記録させる
        audioProcessor.apvts.state.setProperty(ProcessorStateKey::isVisiblePreview, isPreviewVisible, nullptr);

        updatePreviewVisibilityToProcessor();

        realtimePreviewL.setVisible(isPreviewVisible);
        previewLabels[0].setVisible(isPreviewVisible);
        realtimePreviewMono.setVisible(isPreviewVisible);
        previewLabels[1].setVisible(isPreviewVisible);
        realtimePreviewR.setVisible(isPreviewVisible);
        previewLabels[2].setVisible(isPreviewVisible);
        togglePreviewBtn.setButtonText(getPreviewButtonText());
        togglePreviewBtn.setTooltip(getPreviewTooltipText());

        updateUiScale(uiScale);

        // タイマーのON/OFFを切り替え
        updateTimerState();
    };

    // パニックボタン
    addAndMakeVisible(panicButton);
    panicButton.setVisible(true);
    panicButton.setButtonText(EditorGuiText::Panic::title);
    panicButton.setTooltip(EditorGuiText::Panic::tooltip);
    panicButton.setLookAndFeel(&panicButtonLF);
    panicButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    panicButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white.darker(0.2f));
    panicButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    panicButton.onClick = [this] {
        audioProcessor.panic();
        };


    // アンドゥボタンボタン
    addAndMakeVisible(undoButton);
    undoButton.setVisible(true);
    undoButton.setButtonText(EditorGuiText::Undo::title);
    undoButton.setTooltip(getUndoTooltipText());
    undoButton.setLookAndFeel(&undoButtonLF);
    undoButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    undoButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white.darker(0.2f));
    undoButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    undoButton.onClick = [this] {
        audioProcessor.undoManager.undo();
        };

    // リドゥボタン
    addAndMakeVisible(redoButton);
    redoButton.setVisible(true);
    redoButton.setButtonText(EditorGuiText::Redo::title);
    redoButton.setTooltip(getRedoTooltipText());
    redoButton.setLookAndFeel(&redoButtonLF);
    redoButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    redoButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white.darker(0.2f));
    redoButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    redoButton.onClick = [this] {
        audioProcessor.undoManager.redo();
        };

    audioProcessor.undoManager.addChangeListener(this);

    updateUndoRedoButtons();

    // パラメーター初期化ボタン
    addAndMakeVisible(initParamsButton);
    initParamsButton.setVisible(true);
    initParamsButton.setButtonText(EditorGuiText::Reset::title);
    initParamsButton.setTooltip(EditorGuiText::Reset::tooltip);
    initParamsButton.setLookAndFeel(&initParamsButtonLF);
    initParamsButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    initParamsButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black.brighter(0.5f));
    initParamsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    initParamsButton.onClick = [this] {
        initParams();
        };

    updateParameterInitializeButtons();

    addAndMakeVisible(miniModeLabel);
    miniModeLabel.setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::FontStyleFlags::bold)));
    miniModeLabel.setColour(juce::Label::textColourId, juce::Colours::green.brighter(0.6f));
    miniModeLabel.setVisible(false);

    addAndMakeVisible(miniPresetLabel);
    miniPresetLabel.setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::FontStyleFlags::bold)));
    miniPresetLabel.setColour(juce::Label::textColourId, juce::Colours::antiquewhite);
    miniPresetLabel.setVisible(false);

    // ミニプレイヤー表示切替ボタン
    // --- 区分の一括開閉 ---
    // 表示を整えるだけなので、他のシステムボタンとは色味を分けてある。
    for (auto* button : { &openCategoriesBtn, &closeCategoriesBtn })
    {
        addAndMakeVisible(button);
        button->setLookAndFeel(&categoryToggleBtnLF);
        button->setColour(juce::TextButton::textColourOnId, GuiColor::SystemBtn::CategoryToggleText);
        button->setColour(juce::TextButton::textColourOffId, GuiColor::SystemBtn::CategoryToggleText);
        button->setColour(juce::TextButton::buttonColourId, GuiColor::SystemBtn::CategoryToggleBg);
        button->setColour(juce::TextButton::buttonOnColourId, GuiColor::SystemBtn::CategoryToggleBg);
    }

    openCategoriesBtn.setButtonText(EditorGuiText::CategoryToggle::titleOpen);
    openCategoriesBtn.setTooltip(EditorGuiText::CategoryToggle::tooltipOpen);
    openCategoriesBtn.onClick = [this] { openEnabledCategories(); };

    closeCategoriesBtn.setButtonText(EditorGuiText::CategoryToggle::titleClose);
    closeCategoriesBtn.setTooltip(EditorGuiText::CategoryToggle::tooltipClose);
    closeCategoriesBtn.onClick = [this] { closeBypassedCategories(); };

    addAndMakeVisible(toggleMiniBtn);
    toggleMiniBtn.setVisible(true);
    toggleMiniBtn.setButtonText(EditorGuiText::MiniPlayer::titleToMini);
    toggleMiniBtn.setTooltip(EditorGuiText::MiniPlayer::tooltipToMini);
    toggleMiniBtn.setLookAndFeel(&miniToggleBtnLF);
    toggleMiniBtn.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    toggleMiniBtn.setColour(juce::TextButton::textColourOffId, juce::Colours::black.brighter(0.5f));
    toggleMiniBtn.setColour(juce::TextButton::buttonColourId, juce::Colours::cyan);
    toggleMiniBtn.setColour(juce::TextButton::buttonOnColourId, juce::Colours::cyan);
    toggleMiniBtn.onClick = [this] {
        switch (viewMode) {
        case ViewMode::Full: // -> MiniPlayer
            viewMode = ViewMode::MiniPlayer;
            audioProcessor.apvts.state.setProperty(ProcessorStateKey::windowMode, (int)viewMode, nullptr);

            updatePreviewVisibilityToProcessor();

            showMiniPlayerView();

            break;
        case ViewMode::MiniPlayer: // -> Minimum
            viewMode = ViewMode::Minimum;
            audioProcessor.apvts.state.setProperty(ProcessorStateKey::windowMode, (int)viewMode, nullptr);

            showMinimumView();

            break;
        case ViewMode::Minimum: // -> Full
            viewMode = ViewMode::Full;
            audioProcessor.apvts.state.setProperty(ProcessorStateKey::windowMode, (int)viewMode, nullptr);

            updatePreviewVisibilityToProcessor();

            showFullView();

            break;
        default: // -> Full
            viewMode = ViewMode::Full;
            audioProcessor.apvts.state.setProperty(ProcessorStateKey::windowMode, (int)viewMode, nullptr);

            showFullView();
        }
        };

    midiKeyboard = std::make_unique<juce::MidiKeyboardComponent>(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard);

    // 鍵盤を画面に追加し、PCキーボードのフォーカスを受け取れるようにする
    addAndMakeVisible(*midiKeyboard);
    midiKeyboard->setWantsKeyboardFocus(true);

    midiKeyboard->setVisible(audioProcessor.showVirtualKeyboard);

    addAndMakeVisible(*fxGui);
    fxGui->setVisible(true);

    // Processor から uiScale を取得
    uiScale = settingsGui->getUiScale(audioProcessor.uiScaleIndex);
    updateUiScale(uiScale);

    updateTimerState();
}

AudioPlugin2686VEditor::~AudioPlugin2686VEditor()
{
    tabs.setLookAndFeel(nullptr);
    tabs.getTabbedButtonBar().removeChangeListener(this);
    GuiColor::changeBroadcaster().removeChangeListener(this);

    audioProcessor.apvts.removeParameterListener(CPK::mode, this);

    audioProcessor.undoManager.removeChangeListener(this);

    stopTimer(previewTimer);
    stopTimer(playingLampTimer);
}

void AudioPlugin2686VEditor::updateWindowSize()
{
    // スケールを適用
    setScaleFactor(uiScale);

    // viewMode に応じたウィンドウサイズを計算して適用
    if (viewMode == ViewMode::MiniPlayer) {
        setSize(640, 300);
    }
    else if (viewMode == ViewMode::Minimum) {
        setSize(640, 100);
    }
    else {
        int targetWidth = isPreviewVisible ? EditorGuiValue::Window::width + EditorGuiValue::Preview::extraWidth : EditorGuiValue::Window::width;
        int targetHeight = audioProcessor.showVirtualKeyboard ? EditorGuiValue::Window::height + EditorGuiValue::KeyboardHeight : EditorGuiValue::Window::height;
        setSize(targetWidth, targetHeight);
    }
}

void AudioPlugin2686VEditor::showFullView() {
    updateWindowSize();
    updateTimerState();
}

void AudioPlugin2686VEditor::showMiniPlayerView() {
    updatePreviewVisibilityToProcessor();
    updateWindowSize();
    updateTimerState();
}

void AudioPlugin2686VEditor::showMinimumView() {
    updateWindowSize();
    updateTimerState();
}

void AudioPlugin2686VEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &GuiColor::changeBroadcaster())
    {
        // 色が差し替わった。どの部品が使っているかは追えないので、
        // 画面全体を描き直す。頻度は設定を触ったときだけ。
        //
        // ダイアログの配色は既定の LookAndFeel へ写してあるので、
        // 描き直しでは変わらない。ここで写し直す。
        GuiDialog::applyTheme();

        repaint();

        return;
    }

    if (source == &tabs.getTabbedButtonBar())
    {
        // 0:OPNA, 1:OPN, 2:OPL, ...
        int targetMode = tabs.getCurrentTabIndex();

        // 中身をまだ作っていないタブなら、ここで作る。
        materializeTab(targetMode);

        if (targetMode >= 0 && targetMode < (int)OscMode::Count) // BEEP is 11
        {
            // イベント発火に依存せず、タブが切り替わった瞬間に同期させる
            audioProcessor.lastActiveSynthMode = (OscMode)targetMode;

            auto* param = audioProcessor.apvts.getParameter(CPK::mode);
            if (param != nullptr)
            {
                float normalizedValue = param->getNormalisableRange().convertTo0to1((float)targetMode);

                // パラメータ値が既に同じなら更新しない（無駄なイベント発火防止）
                if (param->getValue() != normalizedValue)
                {
                    param->beginChangeGesture();
                    param->setValueNotifyingHost(normalizedValue);
                    param->endChangeGesture();
                }
            }

            playingState.setVisible(true);
        }
        else {
            playingState.setVisible(false);
        }

        updateTimerState();
        updateParameterInitializeButtons();
    }

    if (source == &audioProcessor.undoManager)
    {
        updateUndoRedoButtons();
    }
}

void AudioPlugin2686VEditor::paint(juce::Graphics& g)
{
	drawBg(g);

    switch (viewMode) {
    case ViewMode::Full:
        break;
    case ViewMode::MiniPlayer:
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillRect(5, 5, 630, 290);

        g.setColour(juce::Colours::blue.withAlpha(0.15f));
        g.fillRect(10, 5, 400, 40);
        g.fillRect(420, 5, 150, 40);

        break;
    case ViewMode::Minimum:
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillRect(5, 5, 630, 90);

        g.setColour(juce::Colours::blue.withAlpha(0.15f));
        g.fillRect(10, 5, 400, 40);
        g.fillRect(420, 5, 150, 40);

        break;
    }
}

void AudioPlugin2686VEditor::resized()
{
    // =========================================================================
    // 0. DAWによる不意なサイズ変更（キャッシュによる強制上書き）を防ぐガード
    // =========================================================================
    int expectedW = EditorGuiValue::MiniPreview::paddingLeft + EditorGuiValue::MiniPreview::drawWidth * 3 + EditorGuiValue::MiniPreview::paddingInnerX * 2 + EditorGuiValue::MiniPreview::paddingRight; // 期待する幅の初期値 (MiniPlayer)
    int expectedH = EditorGuiValue::MiniPreview::paddingTop
        + EditorGuiValue::MiniPreview::presetLabelHeight
        + EditorGuiValue::MiniPreview::paddingInnerY
        + EditorGuiValue::MiniPreview::labelHeight
        + EditorGuiValue::MiniPreview::paddingDrawSpaceY
        + EditorGuiValue::MiniPreview::drawHeight
        + EditorGuiValue::MiniPreview::paddingInnerY
        + EditorGuiValue::MinimumPreview::logoHeight
        + EditorGuiValue::MiniPreview::paddingBottom; // 期待する高さの初期値 (MiniPlayer)

    if (viewMode == ViewMode::Minimum) {
        expectedW = expectedW;
        expectedH = 100;
    }
    else if (viewMode == ViewMode::Full) {
        expectedW = isPreviewVisible ? EditorGuiValue::Window::width + EditorGuiValue::Preview::extraWidth : EditorGuiValue::Window::width;
        expectedH = audioProcessor.showVirtualKeyboard ? EditorGuiValue::Window::height + EditorGuiValue::KeyboardHeight : EditorGuiValue::Window::height;
    }

    // DAWが勝手に期待サイズ以外のサイズに変更してきた場合、強制的に正しいサイズに戻す
    if (getWidth() != expectedW || getHeight() != expectedH) {
        setSize(expectedW, expectedH);

        return; // setSize を呼ぶと再び resized() が走るため、ここで処理を中断して無限ループを防ぐ
    }

    // =========================================================================
    // 1. 現在の ViewMode に基づいて、全コンポーネントの表示/非表示(setVisible)と
    //    ラベルのスタイルを設定する
    // =========================================================================
    bool isFull = (viewMode == ViewMode::Full);
    bool isMini = (viewMode == ViewMode::MiniPlayer);
    bool isMin = (viewMode == ViewMode::Minimum);

    // Full View 専用のコンポーネント
    tabs.setVisible(isFull);
    mainIconImage.setVisible(isFull);
    fxGui->setVisible(isFull);
    undoButton.setVisible(isFull);
    redoButton.setVisible(isFull);
    initParamsButton.setVisible(isFull);
    togglePreviewBtn.setVisible(isFull);

    if (midiKeyboard != nullptr) {
        midiKeyboard->setVisible(audioProcessor.showVirtualKeyboard && isFull);
    }

    // Mini / Minimum 共通のコンポーネント
    miniIconImage.setVisible(isMini || isMin);
    miniPresetLabel.setVisible(isMini || isMin);
    miniModeLabel.setVisible(isMini || isMin);

    // プレビューの表示判定 (FullでトグルがON、またはMiniPlayerの時)
    bool showPreview = (isFull && isPreviewVisible) || isMini;
    previewTitleLabel.setVisible(showPreview);
    previewLabels[0].setVisible(showPreview);
    realtimePreviewL.setVisible(showPreview);
    previewLabels[1].setVisible(showPreview);
    realtimePreviewMono.setVisible(showPreview);
    previewLabels[2].setVisible(showPreview);
    realtimePreviewR.setVisible(showPreview);

    // モードに応じたラベルのテキストとスタイルの更新
    if (isFull) {
        toggleMiniBtn.setButtonText(EditorGuiText::MiniPlayer::titleToMini);
        toggleMiniBtn.setTooltip(EditorGuiText::MiniPlayer::tooltipToMini);
        previewLabels[0].setColour(juce::Label::textColourId, juce::Colours::white);
        previewLabels[0].setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
        previewLabels[1].setColour(juce::Label::textColourId, juce::Colours::white);
        previewLabels[1].setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
        previewLabels[2].setColour(juce::Label::textColourId, juce::Colours::white);
        previewLabels[2].setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
    }
    else if (isMini) {
        toggleMiniBtn.setButtonText(EditorGuiText::MiniPlayer::titleToMinimum);
        toggleMiniBtn.setTooltip(EditorGuiText::MiniPlayer::tooltipToMinimum);
        miniPresetLabel.setText(juce::String("") + (audioProcessor.presetName.length() == 0 ? "(EMPTY)" : audioProcessor.presetName), juce::NotificationType::dontSendNotification);
        miniModeLabel.setText(juce::String("") + getModeName(audioProcessor.getCurrentMode()), juce::NotificationType::dontSendNotification);
        previewLabels[0].setColour(juce::Label::textColourId, juce::Colours::white);
        previewLabels[0].setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
        previewLabels[1].setColour(juce::Label::textColourId, juce::Colours::white);
        previewLabels[1].setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
        previewLabels[2].setColour(juce::Label::textColourId, juce::Colours::white);
        previewLabels[2].setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
    }
    else {
        toggleMiniBtn.setButtonText(EditorGuiText::MiniPlayer::titleToFull);
        toggleMiniBtn.setTooltip(EditorGuiText::MiniPlayer::tooltipToFull);
        miniPresetLabel.setText(juce::String("") + (audioProcessor.presetName.length() == 0 ? "(EMPTY)" : audioProcessor.presetName), juce::NotificationType::dontSendNotification);
        miniModeLabel.setText(juce::String("") + getModeName(audioProcessor.getCurrentMode()), juce::NotificationType::dontSendNotification);
    }

    // =========================================================================
    // 2. 実際のレイアウト計算 (setBounds)
    // =========================================================================
    int textWidth = 0;
    int textHeight = 0;
    int iconSize = 0;
    int iconX = 0;
    int iconY = 0;

    // どのモードでも共通して使う「チャンネル表示中か」の判定
    bool isChannel = (OscMode)tabs.getCurrentTabIndex() < OscMode::Count;
    playingState.setVisible(isChannel);

    if (isMini || isMin) {
        int width = getWidth();
        int height = getHeight();

        int sx = width - EditorGuiValue::MiniPreview::paddingRight - EditorGuiValue::SystemBtns::miniButtonWidth;
        panicButton.setBounds(sx, EditorGuiValue::MiniPreview::paddingTop, EditorGuiValue::SystemBtns::miniButtonWidth, EditorGuiValue::SystemBtns::buttonHeight);

        sx -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::miniButtonWidth;
        toggleMiniBtn.setBounds(sx, EditorGuiValue::MiniPreview::paddingTop, EditorGuiValue::SystemBtns::miniButtonWidth, EditorGuiValue::SystemBtns::buttonHeight);

        if (isMini) {
            int lbx = EditorGuiValue::MiniPreview::paddingLeft;
            miniPresetLabel.setBounds(lbx, EditorGuiValue::MiniPreview::paddingTop, EditorGuiValue::MiniPreview::presetLabelWidth, EditorGuiValue::MiniPreview::presetLabelHeight);

            lbx += EditorGuiValue::MiniPreview::presetLabelWidth + EditorGuiValue::MiniPreview::paddingInnerX;
            miniModeLabel.setBounds(lbx, EditorGuiValue::MiniPreview::paddingTop, EditorGuiValue::MiniPreview::modeLabelWidth, EditorGuiValue::MiniPreview::modeLabelHeight);

            int iconWidth = EditorGuiValue::MiniPreview::logoWidth;
            int iconHeight = EditorGuiValue::MiniPreview::logoHeight;
            int iconX = width - EditorGuiValue::MiniPreview::paddingRight - iconWidth;
            int iconY = height - EditorGuiValue::MiniPreview::paddingBottom - iconHeight;
            miniIconImage.setBounds(iconX, iconY, iconWidth, iconHeight);

            int x = EditorGuiValue::MiniPreview::paddingLeft;
            int y = EditorGuiValue::MiniPreview::paddingTop + EditorGuiValue::MiniPreview::presetLabelHeight + EditorGuiValue::MiniPreview::paddingInnerY;
            int dy = y + EditorGuiValue::MiniPreview::labelHeight + EditorGuiValue::MiniPreview::paddingDrawSpaceY;
            previewLabels[0].setBounds(x, y, EditorGuiValue::MiniPreview::drawWidth, EditorGuiValue::MiniPreview::labelHeight);
            realtimePreviewL.setBounds(x, dy, EditorGuiValue::MiniPreview::drawWidth, EditorGuiValue::MiniPreview::drawHeight);

            x += EditorGuiValue::MiniPreview::drawWidth + EditorGuiValue::MiniPreview::paddingInnerX;
            previewLabels[1].setBounds(x, y, EditorGuiValue::MiniPreview::drawWidth, EditorGuiValue::MiniPreview::labelHeight);
            realtimePreviewMono.setBounds(x, dy, EditorGuiValue::MiniPreview::drawWidth, EditorGuiValue::MiniPreview::drawHeight);

            x += EditorGuiValue::MiniPreview::drawWidth + EditorGuiValue::MiniPreview::paddingInnerX;
            previewLabels[2].setBounds(x, y, EditorGuiValue::MiniPreview::drawWidth, EditorGuiValue::MiniPreview::labelHeight);
            realtimePreviewR.setBounds(x, dy, EditorGuiValue::MiniPreview::drawWidth, EditorGuiValue::MiniPreview::drawHeight);
        }
        else { // Minimum
            int lbx = EditorGuiValue::MinimumPreview::paddingLeft;
            miniPresetLabel.setBounds(lbx, EditorGuiValue::MinimumPreview::paddingTop, EditorGuiValue::MinimumPreview::presetLabelWidth, EditorGuiValue::MinimumPreview::presetLabelHeight);

            lbx += EditorGuiValue::MinimumPreview::presetLabelWidth + EditorGuiValue::MinimumPreview::paddingInnerX;
            miniModeLabel.setBounds(lbx, EditorGuiValue::MinimumPreview::paddingTop, EditorGuiValue::MinimumPreview::modeLabelWidth, EditorGuiValue::MinimumPreview::modeLabelHeight);

            int iconWidth = EditorGuiValue::MinimumPreview::logoWidth;
            int iconHeight = EditorGuiValue::MinimumPreview::logoHeight;
            int iconX = width - EditorGuiValue::MinimumPreview::paddingRight - iconWidth;
            int iconY = height - EditorGuiValue::MinimumPreview::paddingBottom - iconHeight;
            miniIconImage.setBounds(iconX, iconY, iconWidth, iconHeight);
        }

        // Mini / Minimum モードでのランプの配置
        if (isChannel) {
            playingState.setBounds(
                EditorGuiValue::StateBtns::paddingLeft,
                height - EditorGuiValue::StateBtns::paddingBottom - EditorGuiValue::StateBtns::height,
                EditorGuiValue::StateBtns::width,
                EditorGuiValue::StateBtns::height
            );
        }

        return; // Full Viewの計算は行わずに終了
    }

    // =========================================================================
    // 3. Full View のレイアウト計算
    // =========================================================================
    auto area = getLocalBounds();

    if (audioProcessor.showVirtualKeyboard && midiKeyboard != nullptr) {
        midiKeyboard->setBounds(area.removeFromBottom(EditorGuiValue::KeyboardHeight));
    }

    if (isPreviewVisible) {
        auto rightArea = area.removeFromRight(EditorGuiValue::Preview::extraWidth);

        int x = rightArea.getX() + EditorGuiValue::Preview::paddingLeft;
        int y = EditorGuiValue::Preview::paddingTop;
        previewTitleLabel.setBounds(x, y, EditorGuiValue::Preview::drawWidth, EditorGuiValue::Preview::labelHeight);

        y += EditorGuiValue::Preview::labelHeight + EditorGuiValue::Preview::paddingInnerY;
        previewLabels[0].setBounds(x, y, EditorGuiValue::Preview::drawWidth, EditorGuiValue::Preview::labelHeight);

        y += EditorGuiValue::Preview::labelHeight + EditorGuiValue::Preview::paddingDrawSpaceY;
        realtimePreviewL.setBounds(x, y, EditorGuiValue::Preview::drawWidth, EditorGuiValue::Preview::drawHeight);

        y += EditorGuiValue::Preview::drawHeight + EditorGuiValue::Preview::paddingInnerY;
        previewLabels[1].setBounds(x, y, EditorGuiValue::Preview::drawWidth, EditorGuiValue::Preview::labelHeight);

        y += EditorGuiValue::Preview::labelHeight + EditorGuiValue::Preview::paddingDrawSpaceY;
        realtimePreviewMono.setBounds(x, y, EditorGuiValue::Preview::drawWidth, EditorGuiValue::Preview::drawHeight);

        y += EditorGuiValue::Preview::drawHeight + EditorGuiValue::Preview::paddingInnerY;
        previewLabels[2].setBounds(x, y, EditorGuiValue::Preview::drawWidth, EditorGuiValue::Preview::labelHeight);

        y += EditorGuiValue::Preview::labelHeight + EditorGuiValue::Preview::paddingDrawSpaceY;
        realtimePreviewR.setBounds(x, y, EditorGuiValue::Preview::drawWidth, EditorGuiValue::Preview::drawHeight);
    }

    int x = getWidth() - ((isPreviewVisible ? EditorGuiValue::Preview::drawWidth : 0) + EditorGuiValue::SystemBtns::paddingRight + EditorGuiValue::SystemBtns::buttonWidth);
    togglePreviewBtn.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::buttonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::miniButtonWidth;
    panicButton.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::miniButtonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::buttonWidth;
    redoButton.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::buttonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::buttonWidth;
    undoButton.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::buttonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::initButtonWidth;
    initParamsButton.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::initButtonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::miniButtonWidth;
    toggleMiniBtn.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::miniButtonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::buttonWidth;
    closeCategoriesBtn.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::buttonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::buttonWidth;
    openCategoriesBtn.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::buttonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    auto reducedArea = area.reduced(EditorGuiValue::Group::Padding::width, EditorGuiValue::Group::Padding::height);
    int mainIconWidth = EditorGuiValue::Preview::logoWidth;
    int mainIconHeight = EditorGuiValue::Preview::logoHeight;
    int mainIconX = reducedArea.getRight() - mainIconWidth;
    int mainIconY = reducedArea.getBottom() - mainIconHeight;
    mainIconImage.setBounds(mainIconX, mainIconY, mainIconWidth, mainIconHeight);

    tabs.setBounds(area);

    auto content = tabs.getLocalBounds();
    auto tabContent = content.removeFromLeft(content.getWidth() - EditorGuiValue::Fx::width);
    tabContent.removeFromTop(tabs.getTabBarDepth()).reduce(EditorGuiValue::Group::Padding::width, EditorGuiValue::Group::Padding::height);

    // 後から作るタブが、resized() を待たずに位置を決められるようにする。
    lastTabContent = tabContent;

    // 配置は中でエンベロープのグラフまで作り直す。区分を 1 つ開け閉めしただけで
    // 裏に隠れているタブぶんまで走ると、そのぶん待たされる。表に出ているものだけ
    // 今やって、残りは印を立てておき、そのタブが前へ出たときに 1 回だけ流す。
    const int currentTab = tabs.getCurrentTabIndex();

    for (int i = 0; i < tabCount; ++i) tabNeedsLayout[(size_t)i] = true;

    layoutTab(currentTab);

    content.removeFromTop(tabs.getTabBarDepth());
    fxGui->setBounds(content);
    fxGui->layout(content);

    playingState.setVisible(isChannel);

    if (isChannel) {
        playingState.setBounds(
            EditorGuiValue::StateBtns::paddingLeft,
            EditorGuiValue::Window::height - EditorGuiValue::StateBtns::paddingBottom - EditorGuiValue::StateBtns::height,
            EditorGuiValue::StateBtns::width,
            EditorGuiValue::StateBtns::height
        );
    }
}

void AudioPlugin2686VEditor::drawBg(juce::Graphics& g)
{
    auto fullArea = getLocalBounds().toFloat();

    // 拡張パネルを含まない、本来のメイン画面の固定サイズ
    juce::Rectangle<float> baseArea(0.0f, 0.0f, (float)EditorGuiValue::Window::width, (float)EditorGuiValue::Window::height);

    if (backgroundImage.isValid())
    {
        int mode = audioProcessor.wallpaperMode;

        // Fit または Original の場合、余白にぼかし背景を敷き詰める
        if (mode == 2 || mode == 3)
        {
            if (blurredBackgroundImage.isValid())
            {
                g.drawImage(blurredBackgroundImage, fullArea, juce::RectanglePlacement::fillDestination);

                // ぼかし背景を少し暗く落とす (手前のメイン画像を目立たせるため)
                g.fillAll(GuiColor::Editor::blurWallpaperBg);
            }
        }

        int placement = juce::RectanglePlacement::stretchToFit; // デフォルト (0)

        switch (audioProcessor.wallpaperMode)
        {
        case 0:
            placement = juce::RectanglePlacement::stretchToFit;
            break;
        case 1:
            placement = juce::RectanglePlacement::fillDestination;
            break;
        case 2:
            placement = juce::RectanglePlacement::centred;
            break;
        case 3:
            placement = juce::RectanglePlacement::centred | juce::RectanglePlacement::doNotResize;
            break;
        }

        // Draw Image
        g.drawImage(backgroundImage, baseArea, placement);

        // Optional: Add a dark overlay to make controls readable
        g.fillAll(GuiColor::Editor::wallpaperBg);
    }
    else
    {
        // Default Solid Color
        g.fillAll(GuiColor::Editor::defaultBg);
    }

    // Reset color for other drawings
    g.setColour(juce::Colours::white);
}

void AudioPlugin2686VEditor::setupLogo()
{
    auto iconImg = juce::ImageCache::getFromMemory(
        AppIconAboutForAboutData::icon_png,
        AppIconAboutForAboutData::icon_pngSize
    );
    mainIconImage.setImage(iconImg);
    mainIconImage.setImagePlacement(juce::RectanglePlacement::centred);
    mainIconImage.setAlpha(EditorGuiValue::WaterMarkLogo::fontAlpha);
    addAndMakeVisible(mainIconImage);
    mainIconImage.toBack();
}

void AudioPlugin2686VEditor::setupMiniLogo()
{
    auto iconImg = juce::ImageCache::getFromMemory(
        AppIconAboutForAboutData::icon_png,
        AppIconAboutForAboutData::icon_pngSize
    );
    miniIconImage.setImage(iconImg);
    miniIconImage.setImagePlacement(juce::RectanglePlacement::centred);
    miniIconImage.setAlpha(EditorGuiValue::WaterMarkLogo::fontAlpha);
    addChildComponent(miniIconImage);
    miniIconImage.toBack();
}

void AudioPlugin2686VEditor::setupTabs(juce::TabbedComponent& tabs)
{
    // 中身を後から作るタブは、空の器だけ先に登録する。
    addAndMakeVisible(tabs);
    tabs.addTab(EditorGuiText::Tab::opn, juce::Colours::transparentBlack, &tabHosts[tabOpn], false);
    tabs.addTab(EditorGuiText::Tab::ssg, juce::Colours::transparentBlack, &tabHosts[tabSsg], false);
    // 画面は unique_ptr が持っているので、タブ側へは所有権を渡さない。
    // true にすると閉じるときに二重で消してしまう。
    tabs.addTab(EditorGuiText::Tab::preset, juce::Colours::transparentBlack, presetGui.get(), false);
    tabs.addTab(EditorGuiText::Tab::settings, juce::Colours::transparentBlack, settingsGui.get(), false);
    tabs.addTab(EditorGuiText::Tab::colors, juce::Colours::transparentBlack, colorsGui.get(), false);
    tabs.addTab(EditorGuiText::Tab::about, juce::Colours::transparentBlack, aboutGui.get(), false);
}

void AudioPlugin2686VEditor::loadPresetFile(const juce::File& file)
{
    audioProcessor.loadPreset(file);
    audioProcessor.presetFilePath = file.getFullPathName();

    presetGui->setMetaData(audioProcessor.presetName, audioProcessor.presetAuthor, audioProcessor.presetVersion, audioProcessor.presetComment, audioProcessor.presetGenre, audioProcessor.presetFilePath);

    // ロードされたプリセットのModeを読み取り、対応するタブへ強制移動させる
    int loadedMode = (int)*audioProcessor.apvts.getRawParameterValue(CPK::mode);
    if (loadedMode >= 0 && loadedMode <= (int)OscMode::SSG) {
        audioProcessor.lastActiveSynthMode = (OscMode)loadedMode;
        tabs.setCurrentTabIndex(loadedMode);
    }

    // 4. 各タブのプリセット名を更新
    updatePresetNameToTabs(audioProcessor.presetName);

    // 履歴へ積む。見出しも一緒に覚えるので、今見ているフォルダの外に
    // あるプリセットでも、開き直さずに一覧へ出せる。
    PresetItem item;

    readPresetMeta(file, item);

    presetLibrary.addHistory(item);

    presetGui->updateTableContent();
    presetGui->repaintTable();
}

void AudioPlugin2686VEditor::loadSettingsFile()
{
    fileChooser = std::make_unique<juce::FileChooser>(juce::String("") + "ファイルから環境設定を読み込み",
        audioProcessor.getPluginDirectory(), SettingsValue::File::glob);

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {
                // 読み終えてからまとめて描き直す。設定は画面全体に効くので、
                // 1 つずつ反映すると待たされる。
                GuiRefresh::Batch batch;

                audioProcessor.loadEnvironment(file);

                // UI反映
                settingsGui->setSettings();

                // 壁紙再描画
                loadWallpaperImage();

                // プリセットリスト更新
                if (juce::File(audioProcessor.defaultPresetDir).isDirectory()) {
                    presetGui->currentFolder = juce::File(audioProcessor.defaultPresetDir);
                    presetGui->updatePresetPath();
                    scanPresets(); // リスト更新関数を呼ぶ
                }
            }
        }
    );

}

// プリセット 1 件ぶんの見出しを読む。
//
// 一覧に出すのは見出しだけなので、中身は最後まで読まない。プリセット 1 つが
// 数百 KB あるため、全部読むと件数だけ時間がかかる。
//
// 3.0.0 より前の XML も読む。読み込みだけは残してあるため。
bool AudioPlugin2686VEditor::readPresetMeta(const juce::File& file, PresetItem& item)
{
    item.file = file;
    item.fileName = file.getFileName();
    item.fullPath = file.getFullPathName();
    item.lastModificationTime = file.getLastModificationTime();
    item.fileSize = file.getSize();
    item.format = file.getFileExtension().substring(1).toUpperCase();

    if (auto reader = Io::ParamReader::open(file, presetFormat, false))
    {
        auto meta = reader->child(Io::StateKey::meta);

        item.name = meta.getString(PresetKey::name, audioProcessor.presetName);
        item.author = meta.getString(PresetKey::author, audioProcessor.presetAuthor);
        item.version = meta.getString(PresetKey::version, audioProcessor.presetVersion);
        item.comment = meta.getString(PresetKey::comment, audioProcessor.presetComment);
        item.modeName = meta.getString(PresetKey::mode, PresetValue::MetaData::Initial::mode);
        item.genre = meta.getString(PresetKey::genre, PresetValue::MetaData::Initial::genre);

        return true;
    }

    if (auto xml = juce::XmlDocument(file).getDocumentElement(true))
    {
        item.name = xml->getStringAttribute(PresetKey::name, audioProcessor.presetName);
        item.author = xml->getStringAttribute(PresetKey::author, audioProcessor.presetAuthor);
        item.version = xml->getStringAttribute(PresetKey::version, audioProcessor.presetVersion);
        item.comment = xml->getStringAttribute(PresetKey::comment, audioProcessor.presetComment);
        item.modeName = xml->getStringAttribute(PresetKey::mode, PresetValue::MetaData::Initial::mode);
        item.genre = xml->getStringAttribute(PresetKey::genre, PresetValue::MetaData::Initial::genre);

        return true;
    }

    item.name = PresetValue::File::Message::invalidXmlNotice;

    return false;
}

void AudioPlugin2686VEditor::scanPresets()
{
    presetGui->clearTable();

    auto files = presetGui->currentFolder.findChildFiles(juce::File::findFiles, true, PresetValue::File::glob);

    // 前回読んだぶんを引き継ぐ。作り直しは保存や削除のたびに走るので、
    // 変わっていないファイルまで開き直すと、数が増えたときに待たされる。
    auto previous = std::move(presetCache);

    presetCache.clear();

    for (const auto& file : files)
    {
        PresetItem item;
        item.file = file;
        item.fileName = file.getFileName();
        item.fullPath = file.getFullPathName();
        item.lastModificationTime = file.getLastModificationTime();

        auto found = previous.find(item.fullPath);

        if (found != previous.end()
            && found->second.lastModificationTime == item.lastModificationTime
            && found->second.fileSize == file.getSize())
        {
            presetCache.emplace(item.fullPath, found->second);
            presetGui->items.push_back(found->second);

            continue;
        }

        readPresetMeta(file, item);

        presetCache.emplace(item.fullPath, item);
        presetGui->items.push_back(item);
    }

    // リスト更新
    presetGui->updateTableContent();
    presetGui->repaintTable();
}

void AudioPlugin2686VEditor::saveCurrentPreset()
{
    // パスが設定されているなら上書き
    if (audioProcessor.presetFilePath.isNotEmpty()) {
        juce::File saveFile(audioProcessor.presetFilePath);

        juce::AlertWindow::showAsync(juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::WarningIcon)
            .withTitle(juce::String("") + "プリセットの上書き")
            .withMessage(juce::String("") + "プリセットファイルの内容を指定したファイルに上書きしてもいいですか？\n\n" + saveFile.getFileName())
            .withButton(juce::String("") + "上書き")
            .withButton(juce::String("") + "キャンセル"),
            [this, saveFile](int result) {
                if (result == 1) {
                    audioProcessor.savePreset(saveFile);
                    scanPresets();
                }
            });
    }
    // パスが未設定（Init後や新規作成時）ならSave Asの挙動へ流す
    else {
        saveCurrentPresetAs();
    }
}

void AudioPlugin2686VEditor::saveCurrentPresetAs()
{
    juce::String filename = audioProcessor.presetName.trim();
    if (filename.isEmpty()) filename = PresetValue::File::def;

    // ファイル名として使えない文字を安全に置換
    filename = filename.replaceCharacter(':', '_').replaceCharacter('/', '_').replaceCharacter('\\', '_');
    filename = filename + PresetValue::File::extension();

    juce::File defaultFile = presetGui->currentFolder.getChildFile(filename);

    openWriteFileChooser(juce::String("") + "ファイルを指定してプリセットを保存", defaultFile, PresetValue::File::glob, [this](const juce::FileChooser& fc) {
        auto file = fc.getResult();
        if (file != juce::File{}) {
            // 保存したファイルパスを記録
            audioProcessor.presetFilePath = file.getFullPathName();
            presetGui->setMetaData(
                audioProcessor.presetName,
                audioProcessor.presetAuthor,
                audioProcessor.presetVersion,
                audioProcessor.presetComment,
                audioProcessor.presetGenre,
                audioProcessor.presetFilePath
            );

            audioProcessor.savePreset(file);
            scanPresets();
        }
        });
}

void AudioPlugin2686VEditor::updatePresetNameToTabs(const juce::String& pName) {
    // 4. 各タブのプリセット名を更新
    if (auto* gui = opnGui.peek()) gui->updatePresetName(pName);
    if (auto* gui = ssgGui.peek()) gui->updatePresetName(pName);
}

void AudioPlugin2686VEditor::loadWallpaperImage()
{
    if (audioProcessor.wallpaperPath.isNotEmpty())
    {
        juce::File imgFile(audioProcessor.wallpaperPath);
        if (imgFile.existsAsFile())
        {
            backgroundImage = juce::ImageFileFormat::loadFrom(imgFile);

            // ぼかし背景の生成
            if (backgroundImage.isValid())
            {
                // 1. 処理を軽くするため、画像を 1/16 サイズに縮小
                int smallW = juce::jmax(1, backgroundImage.getWidth() / 16);
                int smallH = juce::jmax(1, backgroundImage.getHeight() / 16);
                juce::Image smallImg = backgroundImage.rescaled(smallW, smallH, juce::Graphics::lowResamplingQuality);

                // 2. 5x5のガウシアンブラーをかける
                juce::ImageConvolutionKernel blurKernel(5);
                blurKernel.createGaussianBlur(2.0f); // ぼかしの強さ

                // ブラーを適用するための出力先画像を作成
                blurredBackgroundImage = juce::Image(smallImg.getFormat(), smallW, smallH, true);
                blurKernel.applyToImage(blurredBackgroundImage, smallImg, smallImg.getBounds());
            }

            repaint(); // Trigger redraw
        }
    }
    else
    {
        backgroundImage = juce::Image(); // Null image
        blurredBackgroundImage = juce::Image(); // Null image
        repaint();
    }
}

void AudioPlugin2686VEditor::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
{
    // もし gui.page も登録したなら
    // if (&component == &gui.page && wasResized) layoutgui.page(); // (関数化していれば)
}

void AudioPlugin2686VEditor::buttonClicked(juce::Button* button)
{
}

void AudioPlugin2686VEditor::showRegisterInput(juce::Component* targetComp, std::function<void(int)> onValueEntered)
{
    // AlertWindowをヒープに確保 (enterModalState(true) で自動的に削除されます)
    auto* w = new juce::AlertWindow(
        juce::String("") + "レジスタ値の設定",
        juce::String("") + "入力している値:",
        juce::AlertWindow::QuestionIcon);

    // テキストエディタを追加
    w->addTextEditor(
        "regInput",
        "",
        "0"
    );

    // ボタン設定
    w->addButton(juce::String("") + "設定", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
    w->addButton(juce::String("") + "キャンセル", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

    GuiDialog::styleButtons(*w);

    // モーダル表示
    w->enterModalState(true, juce::ModalCallbackFunction::create([onValueEntered, w](int result) {
        if (result == 1) { // OK clicked
            // 入力値を取得してコールバックを実行
            int val = w->getTextEditorContents("regInput").getIntValue();
            onValueEntered(val);
        }
        }), true);
}

// 再帰的に全ての子コンポーネントを探索し、スライダーなら範囲をツールチップにセット
void AudioPlugin2686VEditor::assignTooltipsRecursive(juce::Component* parentComponent)
{
    for (auto* child : parentComponent->getChildren())
    {
        // もしスライダーなら
        if (auto* slider = dynamic_cast<juce::Slider*>(child))
        {
            // 既にツールチップが設定されていない場合のみ自動生成
            if (slider->getTooltip().isEmpty())
            {
                juce::String tooltipText;

                // 範囲を取得 (Min - Max)
                // int型かfloat型かで表示を分けるなど微調整可能
                double min = slider->getMinimum();
                double max = slider->getMaximum();
                double interval = slider->getInterval();

                // 整数のみのスライダーか判定
                bool isInteger = (std::abs(interval - 1.0) < 0.001) || (interval > 0.9);

                if (isInteger) {
                    tooltipText = juce::String("") + "現在の値: " + juce::String((int)min) + " - " + juce::String((int)max);
                }
                else {
                    tooltipText = juce::String("") + "現在の値: " + juce::String(min, 1) + " - " + juce::String(max, 1);
                }

                slider->setTooltip(tooltipText);
            }
        }

        // さらにその子供（グループコンポーネントの中身など）も探索
        assignTooltipsRecursive(child);
    }
}

// ON/OFF 切り替え
void AudioPlugin2686VEditor::setTooltipState(bool enabled)
{
    if (enabled)
    {
        // ウィンドウが存在しなければ作成
        // 第2引数は表示までの遅延(ms)。700msくらいが邪魔にならず丁度よいです。
        if (tooltipWindow == nullptr) {
            tooltipWindow = std::make_unique<juce::TooltipWindow>(this, 700);
        }
    }
    else
    {
        // ウィンドウを削除することで無効化
        tooltipWindow = nullptr;
    }
}

void AudioPlugin2686VEditor::updateKeyboardVisibility()
{
    // 仮想キーボードが有効で、かつFull Viewの時のみ表示する
    if (midiKeyboard != nullptr) {
        midiKeyboard->setVisible(audioProcessor.showVirtualKeyboard && viewMode == ViewMode::Full);
    }
    updateWindowSize();
}

void AudioPlugin2686VEditor::timerCallback(int timerID)
{
    if (timerID == playingLampTimer)
    {
        // 再生ランプ。変わったときだけ描き直す。
        // 以前は毎回 repaint していて、しかも矩形を getHeight() から
        // 出していたのに配置は定数基準だったため、仮想キーボードを出すと
        // 実体と重ならず更新されなくなっていた。部品ごと描き直せばずれない。
        const bool playing = audioProcessor.isPlaying() || audioProcessor.isMidiProcessing();

        if (playing != playingState.state) {
            playingState.updateState(playing);
        }

        return;
    }

    if (isPreviewVisible || viewMode == ViewMode::MiniPlayer)
    {
        // ここで generatePreviewWaveform を呼んで staticData を埋めていたが、
        // 中身はどこからも読まれていなかった。下の描画はリングバッファから
        // 写すだけで完結している。
        //
        // 中では捨てるためだけに 40 ブロック (8000 サンプル) を空レンダリング
        // した上で 300 サンプルを組み立て、FX まで通していた。それを秒 30 回。
        // ユニゾン 8 ボイスの重い音色だと、波形を出しているだけで
        // メッセージスレッドが 1 コア近くを食っていた。

        // メモリ再確保を防ぐため static を付ける、もしくは std::array を使う
        static std::array<float, AudioPlugin2686V::previewBufferSize> localL;
        static std::array<float, AudioPlugin2686V::previewBufferSize> localMono;
        static std::array<float, AudioPlugin2686V::previewBufferSize> localR;

        // オーディオスレッドの最新の書き込み位置を取得
        int currentPos = audioProcessor.realTimeWritePos.load(std::memory_order_acquire);
        int ringSize = AudioPlugin2686V::ringBufferSize;
        int bufSize = AudioPlugin2686V::previewBufferSize;

        // 最新の位置から bufSize 分だけ過去に遡った位置を計算
        int readPos = currentPos - bufSize;
        if (readPos < 0) readPos += ringSize;

        // リングバッファから描画用ローカル配列にコピー
        for (int i = 0; i < bufSize; ++i) {
            localL[i] = audioProcessor.realTimeBufferL[readPos];
            localMono[i] = audioProcessor.realTimeBufferMono[readPos];
            localR[i] = audioProcessor.realTimeBufferR[readPos];

            readPos++;
            if (readPos >= ringSize) readPos = 0;
        }

        // 描画コンポーネントへ渡す
        realtimePreviewL.pushBuffer(localL.data(), bufSize);
        realtimePreviewMono.pushBuffer(localMono.data(), bufSize);
        realtimePreviewR.pushBuffer(localR.data(), bufSize);
    }

}

void AudioPlugin2686VEditor::updateTimerState()
{
    // 波形を出しているときだけ動かす。中の条件は timerCallback と同じなので、
    // 止まっている間にやることは元から無い。
    //
    // 以前は引数で入切を受けていたが、呼び出しが全て true だったため
    // 「設定・About 画面では負荷ゼロにする」というこの関数の目的は
    // 果たされていなかった。呼ぶ側に判断させず、ここで見て決める。
    if (isPreviewVisible || viewMode == ViewMode::MiniPlayer) {
        if (!isTimerRunning(previewTimer)) {
            startTimer(previewTimer, 1000 / previewHz);
        }

        timerCallback(previewTimer); // 開始時に即座に 1 回描く
    }
    else {
        stopTimer(previewTimer);
    }
}

void AudioPlugin2686VEditor::updatePreviewVisibilityToProcessor()
{
    audioProcessor.previewVisiblity = isPreviewVisible || viewMode == ViewMode::MiniPlayer;
}

void AudioPlugin2686VEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == CPK::mode)
    {
        int idx = (int)newValue;

        if (idx >= 0 && idx <= (int)OscMode::SSG) {
            // ホストがオートメーションを流している間、ここはオーディオスレッドで走る。
            // 番号だけ預けて、触るのはメッセージスレッド側に任せる。
            //
            // 以前はここから生の this を握った callAsync を積んでいて、積んだ後に
            // 画面が閉じられると解放済みの領域を触っていた。lastActiveSynthMode への
            // 書き込みも、保存側と同時に走る可能性があった。
            m_pendingModeTab.store(idx, std::memory_order_relaxed);
            triggerAsyncUpdate();
        }
    }
}

void AudioPlugin2686VEditor::handleAsyncUpdate()
{
    const int idx = m_pendingModeTab.exchange(-1, std::memory_order_relaxed);
    if (idx < 0) return;

    audioProcessor.lastActiveSynthMode = (OscMode)idx;

    // 現在のタブと違えば切り替える（ループ防止）
    if (tabs.getCurrentTabIndex() != idx) {
        tabs.setCurrentTabIndex(idx);
    }
}

bool AudioPlugin2686VEditor::keyPressed(const juce::KeyPress& key)
{
    // commandModifier は、WindowsではCtrl、MacではCmdキーを自動で判定します
    auto modifiers = key.getModifiers();

    if (modifiers.isCommandDown())
    {
        // Ctrl + Z (Undo)
        if (key.getKeyCode() == 'Z' || key.getKeyCode() == 'z')
        {
            // Macでは Cmd+Shift+Z が Redo の標準なので分岐
            if (modifiers.isShiftDown()) {
                audioProcessor.undoManager.redo();
            }
            else {
                audioProcessor.undoManager.undo();
            }
            return true; // イベントを消費
        }

        // Ctrl + Y (Redo / Windowsの標準)
        if (key.getKeyCode() == 'Y' || key.getKeyCode() == 'y')
        {
            audioProcessor.undoManager.redo();
            return true; // イベントを消費
        }

        // Ctrl + C (FM Parameter Copy)
        if (key.getKeyCode() == 'C' || key.getKeyCode() == 'c')
        {
            copyFmParamsToString();

            return true; // イベントを消費
        }

        // Ctrl + I (Initialize)
        if (key.getKeyCode() == 'I' || key.getKeyCode() == 'i')
        {
            initParams();

            return true; // イベントを消費
        }
    }
    else {
        // Q (Reset Midi Settings
        if (key.getKeyCode() == 'Q' || key.getKeyCode() == 'q')
        {
            resetMidiSettings();

            return true; // イベントを消費
        }
    }

    return false; // 他のキー入力は通常の処理へ
}

void AudioPlugin2686VEditor::updateUndoRedoButtons()
{
    // canUndo / canRedo は履歴の有無を bool で返してくれる
    undoButton.setEnabled(audioProcessor.undoManager.canUndo());
    redoButton.setEnabled(audioProcessor.undoManager.canRedo());
    undoButton.setTooltip(getUndoTooltipText());
    redoButton.setTooltip(getRedoTooltipText());
}

void AudioPlugin2686VEditor::updateParameterInitializeButtons()
{
    // 表示しているタブが音源のタブか
    // 0:OPNA, 1:OPN, 2:OPL, ...
    int targetMode = tabs.getCurrentTabIndex();
    bool isNotSystemTab = targetMode >= 0 && targetMode <= ((int)OscMode::SSG + 1); // OPNA ～ FX

    initParamsButton.setEnabled(isNotSystemTab);
}

void AudioPlugin2686VEditor::copyFmParamsToString()
{
    OscMode targetMode = (OscMode)tabs.getCurrentTabIndex();

    switch (targetMode)
    {
    case OscMode::OPN:
        opnGui->copyFmParamsToString();
        break;
    };
}

void AudioPlugin2686VEditor::initParams()
{
    int targetMode = tabs.getCurrentTabIndex();

    switch ((OscMode)targetMode)
    {
    case OscMode::OPN:
        opnGui->initParams();
        break;
    case OscMode::SSG:
        ssgGui->initParams();
        break;
    };
}

inline juce::String AudioPlugin2686VEditor::getPreviewButtonText()
{
    return isPreviewVisible ? EditorGuiText::Preview::hide : EditorGuiText::Preview::show;
}

inline juce::String AudioPlugin2686VEditor::getPreviewTooltipText()
{
    return isPreviewVisible ? EditorGuiText::Preview::tooltipHide : EditorGuiText::Preview::tooltipShow;
}

inline juce::String AudioPlugin2686VEditor::getUndoTooltipText()
{
    return audioProcessor.undoManager.canUndo() ? EditorGuiText::Undo::tooltip : EditorGuiText::Undo::tooltipNone;
}

inline juce::String AudioPlugin2686VEditor::getRedoTooltipText()
{
    return audioProcessor.undoManager.canRedo() ? EditorGuiText::Redo::tooltip : EditorGuiText::Redo::tooltipNone;
}

void AudioPlugin2686VEditor::parentHierarchyChanged()
{
    juce::AudioProcessorEditor::parentHierarchyChanged();

    // DAW(VST3/AU)がウィンドウをアタッチした際、ホストのデフォルトDPIで
    // スケールが強制上書きされてしまうため、ここでカスタムスケールを再適用する
    updateUiScale(uiScale);
}

void AudioPlugin2686VEditor::updateUiScale(float newScale) {
    uiScale = newScale;
    updateWindowSize();
};

void AudioPlugin2686VEditor::resetMidiSettings() {
    audioProcessor.resetMidiSettings();
}

// 現在のチャンネルのレベルを全チャンネルに伝播
void AudioPlugin2686VEditor::breadcastLevel(float level) {
    // 値を 1 つ入れるたびに波形を作り直すと、項目の数だけ待たされる。
    // 入れ終えてからまとめて 1 度だけ描き直す。
    GuiRefresh::Batch batch;

    // 開いていないタブは中身を作らず、値だけ入れる。
    // 配るためだけに全部のタブを作ると、そこで待たされてしまう。
    auto apply = [this, level](auto& slot, const juce::String& code) {
        if (auto* gui = slot.peek()) gui->setLevel(level);
        else setLevelParam(code, level);
        };

    apply(opnGui, OpnPrKey::prefix);
    apply(ssgGui, SsgPrKey::prefix);

}

void AudioPlugin2686VEditor::copyOpnOpParams(int from, int to) {
    // 値を 1 つ入れるたびに波形を作り直すと、項目の数だけ待たされる。
    // 入れ終えてからまとめて 1 度だけ描き直す。
    GuiRefresh::Batch batch;

    CopyOpnOp opParams;

    opnGui->copyOpParams(from, opParams);

    opnGui->pasteOpParams(to, opParams);
}

void AudioPlugin2686VEditor::updateFxOrder(){
    fxGui->updateFxOrder();
}

// タブは持っているものが プラグインごとに違うので、あるものだけ回す。
// 中身を持たないタブは、土台の空実装がそのまま呼ばれる。
GuiContext AudioPlugin2686VEditor::makeGuiContext()
{
    return GuiContext(audioProcessor, *this, audioProcessor.apvts, sliderRegMap);
}

juce::String AudioPlugin2686VEditor::currentPresetName() const
{
    return audioProcessor.presetName;
}

// ============================================================================
// タブの後作り
// ============================================================================
// 作り方だけを入れておく。実際に作られるのは、そのタブが開かれたときか、
// 中身を触る操作 (チャンネル間のコピーなど) が来たとき。
void AudioPlugin2686VEditor::setupLazyTabs()
{
    prepareLazyTab<GuiOpn>(opnGui, tabOpn);
    prepareLazyTab<GuiSsg>(ssgGui, tabSsg);
}

void AudioPlugin2686VEditor::materializeAllTabs()
{
    for (int i = 0; i < tabCount; ++i) materializeTab(i);
}

// 指定のタブ 1 枚だけ配置する。まだ作られていないタブは何もしない。
void AudioPlugin2686VEditor::layoutTab(int tabIndex)
{
    if (lastTabContent.isEmpty()) return;

    switch (tabIndex)
    {
    case tabOpn: if (auto* gui = opnGui.peek()) gui->layout(lastTabContent); break;
    case tabSsg: if (auto* gui = ssgGui.peek()) gui->layout(lastTabContent); break;
    case tabPreset: presetGui->layout(lastTabContent); break;
    case tabSettings: settingsGui->layout(lastTabContent); break;
    case tabColors: colorsGui->layout(lastTabContent); break;
    case tabAbout: aboutGui->layout(lastTabContent); break;
    default: break;
    }

    if (tabIndex >= 0 && tabIndex < tabCount) tabNeedsLayout[(size_t)tabIndex] = false;
}

void AudioPlugin2686VEditor::materializeTab(int tabIndex)
{
    switch (tabIndex)
    {
    case tabOpn: opnGui.ref(); break;
    case tabSsg: ssgGui.ref(); break;
    default: break; // 先に作ってあるタブ
    }
}

// 開いていないタブのレベルを、GUI を通さずに入れる。
//
// 画面のつまみは値を持たない (パラメータの写し) ので、ここへ入れておけば
// 後でそのタブを開いたときにそのまま出る。
void AudioPlugin2686VEditor::setLevelParam(const juce::String& prefix, float level)
{
    auto* param = audioProcessor.apvts.getParameter(prefix + CPK::level);

    if (param == nullptr) return;

    param->beginChangeGesture();
    param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(level));
    param->endChangeGesture();
}

void AudioPlugin2686VEditor::forEachTabGui(const std::function<void(GuiBase&)>& fn)
{
    if (auto* gui = opnGui.peek()) fn(*gui);
    if (auto* gui = ssgGui.peek()) fn(*gui);
    if (presetGui != nullptr) fn(*presetGui);
    if (fxGui != nullptr) fn(*fxGui);
    if (settingsGui != nullptr) fn(*settingsGui);
    if (aboutGui != nullptr) fn(*aboutGui);
    if (colorsGui != nullptr) fn(*colorsGui);
}

// 簡易表示モードで隠れている区分を、まとめて切る。
void AudioPlugin2686VEditor::bypassHiddenCategories()
{
    // これはパラメータを切る操作なので、開いていないタブにも効かせる。
    // 音を出しているのはタブではなくパラメータのほうなので、
    // 見ていないタブを飛ばすと、そこだけ鳴り続けてしまう。
    materializeAllTabs();

    forEachTabGui([](GuiBase& gui) { gui.bypassHiddenCategories(); });

    resized();
}

// 効いている区分をまとめて開く
void AudioPlugin2686VEditor::openEnabledCategories()
{
    lastCategoryBulk = CategoryBulk::open;

    forEachTabGui([](GuiBase& gui) { gui.openEnabledCategories(); });

    resized();
}

// 切ってある区分をまとめて閉じる
void AudioPlugin2686VEditor::closeBypassedCategories()
{
    lastCategoryBulk = CategoryBulk::close;

    forEachTabGui([](GuiBase& gui) { gui.closeBypassedCategories(); });

    resized();
}
