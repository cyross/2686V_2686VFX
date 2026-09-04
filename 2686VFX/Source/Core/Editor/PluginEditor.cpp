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

    int currentMode = (int)*audioProcessor.apvts.getRawParameterValue(CPK::mode);
    tabs.setCurrentTabIndex(currentMode);

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


    // プリセットの一覧はこのプラグインには無い

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
        updateTimerState(true);
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


    // Processor から uiScale を取得
    uiScale = settingsGui->getUiScale(audioProcessor.uiScaleIndex);
    updateUiScale(uiScale);


    updateTimerState(true);
}

AudioPlugin2686VEditor::~AudioPlugin2686VEditor()
{
    tabs.setLookAndFeel(nullptr);
    tabs.getTabbedButtonBar().removeChangeListener(this);
    GuiColor::changeBroadcaster().removeChangeListener(this);



    audioProcessor.apvts.removeParameterListener(CPK::mode, this);

    audioProcessor.undoManager.removeChangeListener(this);

    stopTimer();
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
    updateTimerState(true);
}

void AudioPlugin2686VEditor::showMiniPlayerView() {
    updatePreviewVisibilityToProcessor();
    updateWindowSize();
    updateTimerState(true);
}

void AudioPlugin2686VEditor::showMinimumView() {
    updateWindowSize();
    updateTimerState(true);
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

        updateTimerState(true);
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

    // FX はタブになったので、他のタブと同じ広さを使う。
    // 以前は右側の帯を占めていたぶんだけ狭かった。
    auto tabContent = tabs.getLocalBounds();
    tabContent.removeFromTop(tabs.getTabBarDepth()).reduce(EditorGuiValue::Group::Padding::width, EditorGuiValue::Group::Padding::height);

    fxGui->layout(tabContent);
    settingsGui->layout(tabContent);
    colorsGui->layout(tabContent);
    aboutGui->layout(tabContent);

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
                    // プリセットの一覧はこのプラグインには無い
                }
            }
        }
    );

}


void AudioPlugin2686VEditor::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
{
}

// 音源のタブが持っていた読み込みボタンを受けるための口。
// このプラグインには音源が無いので、今は何もしない。
void AudioPlugin2686VEditor::buttonClicked(juce::Button* button)
{
    juce::ignoreUnused(button);
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

void AudioPlugin2686VEditor::updateKeyboardVisibility()
{
    // 仮想キーボードが有効で、かつFull Viewの時のみ表示する
    if (midiKeyboard != nullptr) {
        midiKeyboard->setVisible(audioProcessor.showVirtualKeyboard && viewMode == ViewMode::Full);
    }
    updateWindowSize();
}

void AudioPlugin2686VEditor::timerCallback()
{
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

        // 加工前のぶん
        static std::array<float, AudioPlugin2686V::previewBufferSize> dryL;
        static std::array<float, AudioPlugin2686V::previewBufferSize> dryMono;
        static std::array<float, AudioPlugin2686V::previewBufferSize> dryR;

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

            dryL[i] = audioProcessor.dryBufferL[readPos];
            dryMono[i] = audioProcessor.dryBufferMono[readPos];
            dryR[i] = audioProcessor.dryBufferR[readPos];

            readPos++;
            if (readPos >= ringSize) readPos = 0;
        }

        // 描画コンポーネントへ渡す。
        //
        // 加工前を先に渡す。加工後を渡した側が描き直しを起こすので、
        // 順番を逆にすると 1 回ぶん古い加工前が描かれてしまう。
        realtimePreviewL.pushDryBuffer(dryL.data(), bufSize);
        realtimePreviewMono.pushDryBuffer(dryMono.data(), bufSize);
        realtimePreviewR.pushDryBuffer(dryR.data(), bufSize);

        realtimePreviewL.pushBuffer(localL.data(), bufSize);
        realtimePreviewMono.pushBuffer(localMono.data(), bufSize);
        realtimePreviewR.pushBuffer(localR.data(), bufSize);
    }

    playingState.state = audioProcessor.isPlaying() || audioProcessor.isMidiProcessing();

    repaint(
        EditorGuiValue::StateBtns::paddingLeft,
        getHeight() - EditorGuiValue::StateBtns::paddingBottom - EditorGuiValue::StateBtns::height,
        EditorGuiValue::StateBtns::width,
        EditorGuiValue::StateBtns::height
    );
}

void AudioPlugin2686VEditor::updateTimerState(bool start = false)
{
    // プレビューが開いていている時だけタイマーを動かす
    if (start) {
        startTimerHz(previewHz);
        timerCallback(); // タイマー開始時に即座に1回強制描画する！
    }
    else {
        stopTimer(); // 閉じてる時、または設定・About画面では負荷ゼロにする
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

        if (idx >= 0 && idx <= (int)OscMode::ADPCM) {
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
        // FM のレジスタ書き出しは音源のプラグインの機能
        if (false)
        {

            return true; // イベントを消費
        }

        // Ctrl + I (Initialize
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

// INIT が効くのは、パラメータを持つタブを見ているときだけ。
// このプラグインでは FX だけなので、先頭のタブかどうかを見る。
void AudioPlugin2686VEditor::updateParameterInitializeButtons()
{
    initParamsButton.setEnabled(tabs.getCurrentTabIndex() == 0);
}


void AudioPlugin2686VEditor::initParams()
{
    // 触れるのは FX だけ
    fxGui->initParams();
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


void AudioPlugin2686VEditor::updateFxOrder(){
    fxGui->updateFxOrder();
}


void AudioPlugin2686VEditor::setupTabs(juce::TabbedComponent& tabs)
{
    addAndMakeVisible(tabs);
    // FX はもともと画面の下へ敷いていたが、このプラグインでは主役なので
    // 独立したタブにする。
    tabs.addTab(EditorGuiText::Tab::fx, juce::Colours::transparentBlack, fxGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::settings, juce::Colours::transparentBlack, settingsGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::colors, juce::Colours::transparentBlack, colorsGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::about, juce::Colours::transparentBlack, aboutGui.get(), true);
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

// タブは持っているものが プラグインごとに違うので、あるものだけ回す。
// 中身を持たないタブは、土台の空実装がそのまま呼ばれる。
void AudioPlugin2686VEditor::forEachTabGui(const std::function<void(GuiBase&)>& fn)
{
    if (fxGui != nullptr) fn(*fxGui);
    if (settingsGui != nullptr) fn(*settingsGui);
    if (aboutGui != nullptr) fn(*aboutGui);
    if (colorsGui != nullptr) fn(*colorsGui);
}

// 簡易表示モードで隠れている区分を、まとめて切る。
void AudioPlugin2686VEditor::bypassHiddenCategories()
{
    forEachTabGui([](GuiBase& gui) { gui.bypassHiddenCategories(); });

    resized();
}

// 効いている区分をまとめて開く
void AudioPlugin2686VEditor::openEnabledCategories()
{
    forEachTabGui([](GuiBase& gui) { gui.openEnabledCategories(); });

    resized();
}

// 切ってある区分をまとめて閉じる
void AudioPlugin2686VEditor::closeBypassedCategories()
{
    forEachTabGui([](GuiBase& gui) { gui.closeBypassedCategories(); });

    resized();
}
