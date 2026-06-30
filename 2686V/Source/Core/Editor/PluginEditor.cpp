#include <cstdio>
#include <vector>
#include <initializer_list>
#include <utility>

#include "./PluginEditor.h"

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

AudioPlugin2686VEditor::AudioPlugin2686VEditor(AudioPlugin2686V& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    int mode = audioProcessor.apvts.state.getProperty(ProcessorStateKey::windowMode, (int)ViewMode::Full);
    viewMode = (ViewMode)mode;

    GuiContext context(audioProcessor, *this, audioProcessor.apvts, sliderRegMap);

    opnaGui = std::make_unique<GuiOpna>(context);
	opnGui = std::make_unique<GuiOpn>(context);
	oplGui = std::make_unique<GuiOpl>(context);
	opl3Gui = std::make_unique<GuiOpl3>(context);
	opmGui = std::make_unique<GuiOpm>(context);
	opzx7Gui = std::make_unique<GuiOpzx7>(context);
	ssgGui = std::make_unique<GuiSsg>(context);
	wtGui = std::make_unique<GuiWt>(context);
    wt2Gui = std::make_unique<GuiWt2>(context);
	rhythmGui = std::make_unique<GuiRhythm>(context);
	adpcmGui = std::make_unique<GuiAdpcm>(context);
    beepGui = std::make_unique<GuiBeep>(context);
    presetGui = std::make_unique<GuiPreset>(context);
    fxGui = std::make_unique<GuiFx>(context);
	settingsGui = std::make_unique<GuiSettings>(context);
	aboutGui = std::make_unique<GuiAbout>(context);
    curveGui = std::make_unique<GuiCurve>(context);

    wtGui->addComponentListener(this);
    wt2Gui->addComponentListener(this);

    tabs.getTabbedButtonBar().addChangeListener(this);

    audioProcessor.apvts.addParameterListener(CorePrKey::mode, this);

    setupLogo();
    setupMiniLogo();

    opnaGui->setup();
    opnGui->setup();
    oplGui->setup();
    opl3Gui->setup();
    opmGui->setup();
    opzx7Gui->setup();
    ssgGui->setup();
    wtGui->setup();
    wt2Gui->setup();
    rhythmGui->setup();
    adpcmGui->setup();
    beepGui->setup();
    presetGui->setup();
    fxGui->setup();
    settingsGui->setup();
    aboutGui->setup();
    curveGui->setup();

    // Initial Wallpaper Load
    loadWallpaperImage();

    isPreviewVisible = audioProcessor.apvts.state.getProperty(ProcessorStateKey::isVisiblePreview);

    tabs.setLookAndFeel(&customTabLF);

    setupTabs(tabs);

    int currentMode = (int)*audioProcessor.apvts.getRawParameterValue(CorePrKey::mode);
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

    // パラメーターコピーボタン
    addAndMakeVisible(copyParamsButton);
    copyParamsButton.setVisible(true);
    copyParamsButton.setButtonText(EditorGuiText::Copy::title);
    copyParamsButton.setTooltip(EditorGuiText::Copy::tooltip);
    copyParamsButton.setLookAndFeel(&copyParamsButtonLF);
    copyParamsButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    copyParamsButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black.brighter(0.5f));
    copyParamsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::yellow);
    copyParamsButton.onClick = [this] {
        copyFmParamsToString();
        copyFmParamsToObject();
        };

    // パラメーターペーストボタン
    addAndMakeVisible(pasteParamsButton);
    pasteParamsButton.setVisible(true);
    pasteParamsButton.setButtonText(EditorGuiText::Paste::title);
    pasteParamsButton.setTooltip(EditorGuiText::Paste::tooltip);
    pasteParamsButton.setLookAndFeel(&pasteParamsButtonLF);
    pasteParamsButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    pasteParamsButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black.brighter(0.5f));
    pasteParamsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::yellow);
    pasteParamsButton.onClick = [this] {
        pasteFmParamsFromObject();
        };

    updateParameterCopyPasteButtons();

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

    // Io::empty 以外の文字列を渡すことで、プロセッサ内に保持されたパスから再読み込みさせます
    updateRhythmFileNames("Reload");
    updateAdpcmFileNames("Reload");
    updateOpzx7PcmFileNames("Reload");
    updateOpzx7WtFileNames("Reload");

    updateTimerState(true);
}

AudioPlugin2686VEditor::~AudioPlugin2686VEditor()
{
    tabs.setLookAndFeel(nullptr);
    tabs.getTabbedButtonBar().removeChangeListener(this);

    wtGui->removeComponentListener(this);
    wt2Gui->removeComponentListener(this);

    adpcmGui->removeLoadButtonListener(this);

    rhythmGui->removeLoadButtonListener(this);

    audioProcessor.apvts.removeParameterListener(CorePrKey::mode, this);

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
    if (source == &tabs.getTabbedButtonBar())
    {
        // 0:OPNA, 1:OPN, 2:OPL, ...
        int targetMode = tabs.getCurrentTabIndex();

        if (targetMode >= 0 && targetMode < (int)OscMode::Count) // BEEP is 11
        {
            // イベント発火に依存せず、タブが切り替わった瞬間に同期させる
            audioProcessor.lastActiveSynthMode = (OscMode)targetMode;

            auto* param = audioProcessor.apvts.getParameter(CorePrKey::mode);
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
        updateParameterCopyPasteButtons();
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
    logoLabel.setVisible(isFull);
    mainIconImage.setVisible(isFull);
    fxGui->setVisible(isFull);
    undoButton.setVisible(isFull);
    redoButton.setVisible(isFull);
    copyParamsButton.setVisible(isFull);
    pasteParamsButton.setVisible(isFull);
    initParamsButton.setVisible(isFull);
    togglePreviewBtn.setVisible(isFull);

    if (midiKeyboard != nullptr) {
        midiKeyboard->setVisible(audioProcessor.showVirtualKeyboard && isFull);
    }

    // Mini / Minimum 共通のコンポーネント
    miniLogoLabel.setVisible(isMini || isMin);
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

            int lgx = width - EditorGuiValue::MiniPreview::paddingRight - EditorGuiValue::MiniPreview::logoWidth;
            int lgy = height - EditorGuiValue::MiniPreview::paddingBottom - EditorGuiValue::MiniPreview::logoHeight;
            miniLogoLabel.setBounds(lgx, lgy, EditorGuiValue::MiniPreview::logoWidth, EditorGuiValue::MiniPreview::logoHeight);

            textWidth = (int)juce::GlyphArrangement::getStringWidth(miniLogoLabel.getFont(), juce::StringRef(miniLogoLabel.getText()));
            textHeight = (int)miniLogoLabel.getFont().getHeight();
            iconSize = textHeight - 12;
            iconX = width - EditorGuiValue::MiniPreview::paddingInnerX - textWidth - iconSize - 8;
            iconY = lgy + (textHeight / 2) - 6;
            miniIconImage.setBounds(iconX, iconY, iconSize, iconSize);

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

            int lgx = width - EditorGuiValue::MinimumPreview::paddingRight - EditorGuiValue::MinimumPreview::logoWidth;
            int lgy = height - EditorGuiValue::MinimumPreview::paddingBottom - EditorGuiValue::MinimumPreview::logoHeight;
            miniLogoLabel.setBounds(lgx, lgy, EditorGuiValue::MinimumPreview::logoWidth, EditorGuiValue::MinimumPreview::logoHeight);

            textWidth = (int)juce::GlyphArrangement::getStringWidth(miniLogoLabel.getFont(), juce::StringRef(miniLogoLabel.getText()));
            textHeight = (int)miniLogoLabel.getFont().getHeight();
            iconSize = textHeight - 12;
            iconX = width - EditorGuiValue::MinimumPreview::paddingInnerX - textWidth - iconSize - 8;
            iconY = lgy + (textHeight / 2) - 6;
            miniIconImage.setBounds(iconX, iconY, iconSize, iconSize);
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

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::buttonWidth;
    copyParamsButton.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::buttonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::buttonWidth;
    pasteParamsButton.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::buttonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::initButtonWidth;
    initParamsButton.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::initButtonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    x -= EditorGuiValue::SystemBtns::paddingInnerX + EditorGuiValue::SystemBtns::miniButtonWidth;
    toggleMiniBtn.setBounds(x, EditorGuiValue::SystemBtns::paddingTop, EditorGuiValue::SystemBtns::miniButtonWidth, EditorGuiValue::SystemBtns::buttonHeight);

    auto reducedArea = area.reduced(EditorGuiValue::Group::Padding::width, EditorGuiValue::Group::Padding::height);
    logoLabel.setBounds(reducedArea);

    int mainTextWidth = (int)juce::GlyphArrangement::getStringWidth(logoLabel.getFont(), juce::StringRef(logoLabel.getText()));
    int mainTextHeight = (int)logoLabel.getFont().getHeight();
    int mainIconSize = mainTextHeight - 44;
    int mainIconX = reducedArea.getRight() - mainTextWidth - mainIconSize - 15;
    int mainIconY = reducedArea.getBottom() - ((mainTextHeight + mainIconSize) / 2);
    mainIconImage.setBounds(mainIconX, mainIconY, mainIconSize, mainIconSize);

    tabs.setBounds(area);

    auto content = tabs.getLocalBounds();
    auto tabContent = content.removeFromLeft(content.getWidth() - EditorGuiValue::Fx::width);
    tabContent.removeFromTop(tabs.getTabBarDepth()).reduce(EditorGuiValue::Group::Padding::width, EditorGuiValue::Group::Padding::height);

    opnaGui->layout(tabContent);
    opnGui->layout(tabContent);
    oplGui->layout(tabContent);
    opl3Gui->layout(tabContent);
    opmGui->layout(tabContent);
    opzx7Gui->layout(tabContent);
    ssgGui->layout(tabContent);
    wtGui->layout(tabContent);
    wt2Gui->layout(tabContent);
    rhythmGui->layout(tabContent);
    adpcmGui->layout(tabContent);
    beepGui->layout(tabContent);
    presetGui->layout(tabContent);
    settingsGui->layout(tabContent);
    aboutGui->layout(tabContent);
    curveGui->layout(tabContent);

    content.removeFromTop(tabs.getTabBarDepth());
    fxGui->setBounds(content);
    fxGui->layout(content);

    playingState.setVisible(isChannel);

    if (isChannel) {
        playingState.setBounds(
            EditorGuiValue::StateBtns::paddingLeft,
            getHeight() - EditorGuiValue::StateBtns::paddingBottom - EditorGuiValue::StateBtns::height,
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
    logoLabel.setText(Global::Plugin::name, juce::dontSendNotification);
    logoLabel.setFont(juce::Font(
        juce::FontOptions(
            EditorGuiValue::WaterMarkLogo::fontFamily,
            EditorGuiValue::WaterMarkLogo::fontSize,
            juce::Font::bold | juce::Font::italic
        )
    ));
    logoLabel.setJustificationType(juce::Justification::bottomRight);
    logoLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(EditorGuiValue::WaterMarkLogo::fontAlpha));
    addAndMakeVisible(logoLabel);
    logoLabel.toBack();

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
    miniLogoLabel.setText(Global::Plugin::name, juce::dontSendNotification);
    miniLogoLabel.setFont(
        juce::Font(
            juce::FontOptions(
                EditorGuiValue::WaterMarkLogo::fontFamily,
                40.0f,
                juce::Font::bold | juce::Font::italic
            )
        )
    );
    miniLogoLabel.setJustificationType(juce::Justification::bottomRight);
    miniLogoLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(EditorGuiValue::WaterMarkLogo::fontAlpha));
    addChildComponent(miniLogoLabel);
    miniLogoLabel.toBack();

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
    addAndMakeVisible(tabs);
    tabs.addTab(EditorGuiText::Tab::opna, juce::Colours::transparentBlack, opnaGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::opn, juce::Colours::transparentBlack, opnGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::opl, juce::Colours::transparentBlack, oplGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::opl3, juce::Colours::transparentBlack, opl3Gui.get(), true);
    tabs.addTab(EditorGuiText::Tab::opm, juce::Colours::transparentBlack, opmGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::opzx7, juce::Colours::transparentBlack, opzx7Gui.get(), true);
    tabs.addTab(EditorGuiText::Tab::ssg, juce::Colours::transparentBlack, ssgGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::wt, juce::Colours::transparentBlack, wtGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::wt2, juce::Colours::transparentBlack, wt2Gui.get(), true);
    tabs.addTab(EditorGuiText::Tab::rhythm, juce::Colours::transparentBlack, rhythmGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::adpcm, juce::Colours::transparentBlack, adpcmGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::beep, juce::Colours::transparentBlack, beepGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::advanced, juce::Colours::transparentBlack, curveGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::preset, juce::Colours::transparentBlack, presetGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::settings, juce::Colours::transparentBlack, settingsGui.get(), true);
    tabs.addTab(EditorGuiText::Tab::about, juce::Colours::transparentBlack, aboutGui.get(), true);
}

void AudioPlugin2686VEditor::loadPresetFile(const juce::File& file)
{
    audioProcessor.loadPreset(file);
    audioProcessor.presetFilePath = file.getFullPathName();

    presetGui->setMetaData(audioProcessor.presetName, audioProcessor.presetAuthor, audioProcessor.presetVersion, audioProcessor.presetComment, audioProcessor.presetGenre, audioProcessor.presetFilePath);

    // Io::empty 以外の文字列を渡すことで、プロセッサ内に保持されたパスから再読み込みさせます
    updateRhythmFileNames("Reload");
    updateAdpcmFileNames("Reload");
    updateOpzx7PcmFileNames("Reload");
    updateOpzx7WtFileNames("Reload");

    // ロードされたプリセットのModeを読み取り、対応するタブへ強制移動させる
    int loadedMode = (int)*audioProcessor.apvts.getRawParameterValue(CorePrKey::mode);
    if (loadedMode >= 0 && loadedMode <= (int)OscMode::BEEP) {
        audioProcessor.lastActiveSynthMode = (OscMode)loadedMode;
        tabs.setCurrentTabIndex(loadedMode);
    }

    // 4. 各タブのプリセット名を更新
    updatePresetNameToTabs(audioProcessor.presetName);
}

void AudioPlugin2686VEditor::loadSettingsFile()
{
    fileChooser = std::make_unique<juce::FileChooser>(juce::String("") + "ファイルから環境設定を読み込み",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "*.xml");

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {
                audioProcessor.loadEnvironment(file);

                // UI反映
                settingsGui->setSettings(
                    audioProcessor.uiScaleIndex,
                    audioProcessor.wallpaperPath.isEmpty() ? Io::empty : juce::File(audioProcessor.wallpaperPath).getFileName(),
                    audioProcessor.defaultSampleDir,
                    audioProcessor.defaultPresetDir,
                    audioProcessor.defaultWavetableDir,
                    audioProcessor.defaultFxOrderDir,
                    audioProcessor.defaultFxParamDir,
                    audioProcessor.defaultLfoParamDir,
                    audioProcessor.defaultAmpEnvParamDir,
                    audioProcessor.defaultPitchEnvParamDir,
                    audioProcessor.defaultSsgSwEnvParamDir,
                    audioProcessor.defaultDetuneParamDir,
                    audioProcessor.defaultUnisonParamDir,
                    audioProcessor.defaultQualityParamDir,
                    audioProcessor.defaultPcmPlayParamDir,
                    audioProcessor.defaultToneNoiseParamDir
                    );

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

void AudioPlugin2686VEditor::scanPresets()
{
    presetGui->clearTable();

    // XMLファイルを探す
    auto files = presetGui->currentFolder.findChildFiles(juce::File::findFiles, true, PresetValue::File::glob);

    for (const auto& file : files)
    {
        PresetItem item;
        item.file = file;
        item.fileName = file.getFileName();
        item.fullPath = file.getFullPathName();
        item.lastModificationTime = file.getLastModificationTime();

        // XMLをパースしてメタデータを取得
        juce::XmlDocument xmlDoc(file);
        auto xml = xmlDoc.getDocumentElement();
        if (xml != nullptr)
        {
            item.name = xml->getStringAttribute(PresetKey::name, audioProcessor.presetName);
            item.author = xml->getStringAttribute(PresetKey::author, audioProcessor.presetAuthor);
            item.version = xml->getStringAttribute(PresetKey::version, audioProcessor.presetVersion);
            item.comment = xml->getStringAttribute(PresetKey::comment, audioProcessor.presetComment);
            item.modeName = xml->getStringAttribute(PresetKey::mode, PresetValue::MetaData::Initial::mode);
            item.genre = xml->getStringAttribute(PresetKey::genre, PresetValue::MetaData::Initial::genre);
        }
        else
        {
            item.name = PresetValue::File::Message::invalidXmlNotice;
        }

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
    filename = filename + PresetValue::File::ext;

    juce::File defaultFile = presetGui->currentFolder.getChildFile(filename);

    openWriteFileChooser(juce::String("") + "ファイルを指定してプリセットを保存", defaultFile, "*.xml", [this](const juce::FileChooser& fc) {
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
    opnaGui->updatePresetName(pName);
    opnGui->updatePresetName(pName);
    oplGui->updatePresetName(pName);
    opl3Gui->updatePresetName(pName);
    opmGui->updatePresetName(pName);
    opzx7Gui->updatePresetName(pName);
    ssgGui->updatePresetName(pName);
    wtGui->updatePresetName(pName);
    wt2Gui->updatePresetName(pName);
    rhythmGui->updatePresetName(pName);
    adpcmGui->updatePresetName(pName);
    beepGui->updatePresetName(pName);
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
    // wtPage のサイズが変わったときだけレイアウトを実行
    if (&component == wtGui.get() && wasResized)
    {
        auto content = tabs.getLocalBounds();
        content.removeFromTop(tabs.getTabBarDepth());
        content.reduce(EditorGuiValue::Group::Padding::width, EditorGuiValue::Group::Padding::height); // 全体の余白

        wtGui->layout(content);
    }
    // もし gui.page も登録したなら
    // if (&component == &gui.page && wasResized) layoutgui.page(); // (関数化していれば)
}

void AudioPlugin2686VEditor::buttonClicked(juce::Button* button)
{
    // ADPCM Load Buttons
    if (adpcmGui->isThis(button))
    {
        // ... (Existing ADPCM load logic) ...
        auto fileFilter = audioProcessor.formatManager.getWildcardForAllFormats();
        openFileChooser(
            Io::Dialog::Title::openAudioFile,
            audioProcessor.lastSampleDirectory,
            fileFilter,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    adpcmGui->updateFileName("Loading...");

                    juce::Timer::callAfterDelay(50, [this, file]()
                        {
                            audioProcessor.loadAdpcmFile(file);
                            adpcmGui->updateFileName(file.getFileName());
                            audioProcessor.lastSampleDirectory = file.getParentDirectory();
                        });
                }
            }
        );
    }
    // ADPCM Pan Buttons
    else if (adpcmGui->isBtnPanL(button)) { adpcmGui->setPan(0.0); }
    else if (adpcmGui->isBtnPanC(button)) { adpcmGui->setPan(0.5); }
    else if (adpcmGui->isBtnPanR(button)) { adpcmGui->setPan(1.0); }
    // Rhythm Pads Buttons
    else
    {
        auto fileFilter = audioProcessor.formatManager.getWildcardForAllFormats();
        fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::openAudioFile, audioProcessor.lastSampleDirectory, fileFilter);
        rhythmGui->buttonClicked(button, audioProcessor.formatManager, fileChooser);
    }
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

void AudioPlugin2686VEditor::updateRhythmFileNames(const juce::String filename)
{
    if (filename == Io::empty) {
        for (int i = 0; i < 8; ++i)
        {
            rhythmGui->updatePadFileName(i, filename);
        }

    }
    else {
        for (int i = 0; i < 8; ++i)
        {
            juce::String path = audioProcessor.rhythmFilePaths[i];
            juce::String text = Io::empty;

            if (path.isNotEmpty())
            {
                // パスが存在すればファイル名を取得
                // (resolvePathを使って絶対パス化してから名前を取得するのが確実です)
                juce::File f = audioProcessor.resolvePath(path);
                text = f.getFileName();
            }

            rhythmGui->updatePadFileName(i, text);
        }
    }
}

void AudioPlugin2686VEditor::updateOpzx7PcmFileNames(const juce::String filename)
{
    if (filename == Io::empty) {
        for (int i = 0; i < 4; ++i)
        {
            opzx7Gui->updatePcmFileName(i, filename);
        }

    }
    else {
        for (int i = 0; i < 4; ++i)
        {
            juce::String path = audioProcessor.opzx7PcmFilePaths[i];
            juce::String text = Io::empty;

            if (path.isNotEmpty())
            {
                // パスが存在すればファイル名を取得
                // (resolvePathを使って絶対パス化してから名前を取得するのが確実です)
                juce::File f = audioProcessor.resolvePath(path);
                text = f.getFileName();
            }

            opzx7Gui->updatePcmFileName(i, text);
        }
    }
}

void AudioPlugin2686VEditor::updateOpzx7WtFileNames(const juce::String filename)
{
    if (filename == Io::empty) {
        for (int i = 0; i < 4; ++i)
        {
            opzx7Gui->updateWtFileName(i, filename);
        }

    }
    else {
        for (int i = 0; i < 4; ++i)
        {
            juce::String path = audioProcessor.opzx7WtFilePaths[i];
            juce::String text = Io::empty;

            if (path.isNotEmpty())
            {
                // パスが存在すればファイル名を取得
                // (resolvePathを使って絶対パス化してから名前を取得するのが確実です)
                juce::File f = audioProcessor.resolvePath(path);
                text = f.getFileName();
            }

            opzx7Gui->updateWtFileName(i, text);
        }
    }
}

void AudioPlugin2686VEditor::updateAdpcmFileNames(const juce::String filename)
{
    if (filename == Io::empty) {
        adpcmGui->updateFileName(filename);
    }
    else {
        juce::String path = audioProcessor.adpcmFilePath;
        juce::String text = Io::empty;

        if (path.isNotEmpty())
        {
            // パスが存在すればファイル名を取得
            // (resolvePathを使って絶対パス化してから名前を取得するのが確実です)
            juce::File f = audioProcessor.resolvePath(path);
            text = f.getFileName();
        }

        adpcmGui->updateFileName(text);
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
        std::vector<float> staticData;
        audioProcessor.generatePreviewWaveform(&staticData);

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
    if (parameterID == CorePrKey::mode)
    {
        int idx = (int)newValue;

        if (idx >= 0 && idx <= (int)OscMode::BEEP) {
            audioProcessor.lastActiveSynthMode = (OscMode)idx;
        }

        // UIスレッドで実行するために callAsync を使用
        juce::MessageManager::callAsync([this, idx]() {
            // 現在のタブと違えば切り替える（ループ防止）
            if (tabs.getCurrentTabIndex() != idx) {
                tabs.setCurrentTabIndex(idx);
            }
            });
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

        // Ctrl + C (Copy)
        if (key.getKeyCode() == 'C' || key.getKeyCode() == 'c')
        {
            copyFmParamsToString();
            copyFmParamsToObject();

            return true; // イベントを消費
        }

        // 今後やる
#if false
        // Ctrl + V (Paste)
        if (key.getKeyCode() == 'V' || key.getKeyCode() == 'v')
        {
            copyFmParamsToObject();

            return true; // イベントを消費
        }
#endif

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

void AudioPlugin2686VEditor::updateParameterCopyPasteButtons()
{
    // 表示しているタブがFM音源のタブか
    // 0:OPNA, 1:OPN, 2:OPL, ...
    int targetMode = tabs.getCurrentTabIndex();
    bool isFmTab = targetMode >= 0 && targetMode <= (int)OscMode::OPZX7;

    copyParamsButton.setEnabled(isFmTab);
    pasteParamsButton.setEnabled(isFmTab);
}

void AudioPlugin2686VEditor::updateParameterInitializeButtons()
{
    // 表示しているタブが音源のタブか
    // 0:OPNA, 1:OPN, 2:OPL, ...
    int targetMode = tabs.getCurrentTabIndex();
    bool isNotSystemTab = targetMode >= 0 && targetMode <= ((int)OscMode::BEEP + 1); // OPNA ～ FX

    initParamsButton.setEnabled(isNotSystemTab);
}

void AudioPlugin2686VEditor::copyFmParamsToString()
{
    OscMode targetMode = (OscMode)tabs.getCurrentTabIndex();

    switch (targetMode)
    {
    case OscMode::OPNA:
        opnaGui->copyFmParamsToString();
        break;
    case OscMode::OPN:
        opnGui->copyFmParamsToString();
        break;
    case OscMode::OPL:
        oplGui->copyFmParamsToString();
        break;
    case OscMode::OPL3:
        opl3Gui->copyFmParamsToString();
        break;
    case OscMode::OPM:
        opmGui->copyFmParamsToString();
        break;
    case OscMode::OPZX7:
        opzx7Gui->copyFmParamsToString();
        break;
    };
}

void AudioPlugin2686VEditor::copyFmParamsToObject()
{
    OscMode targetMode = (OscMode)tabs.getCurrentTabIndex();

    switch (targetMode)
    {
    case OscMode::OPNA:
        opnaGui->copyFmParamsToObject();
        break;
    case OscMode::OPN:
        opnGui->copyFmParamsToObject();
        break;
    case OscMode::OPL:
        oplGui->copyFmParamsToObject();
        break;
    case OscMode::OPL3:
        opl3Gui->copyFmParamsToObject();
        break;
    case OscMode::OPM:
        opmGui->copyFmParamsToObject();
        break;
    case OscMode::OPZX7:
        opzx7Gui->copyFmParamsToObject();
        break;
    };
}

void AudioPlugin2686VEditor::pasteFmParamsFromObject()
{
    OscMode targetMode = (OscMode)tabs.getCurrentTabIndex();

    switch (targetMode)
    {
    case OscMode::OPNA:
        opnaGui->pasteFmParamsFromObject();
        break;
    case OscMode::OPN:
        opnGui->pasteFmParamsFromObject();
        break;
    case OscMode::OPL:
        oplGui->pasteFmParamsFromObject();
        break;
    case OscMode::OPL3:
        opl3Gui->pasteFmParamsFromObject();
        break;
    case OscMode::OPM:
        opmGui->pasteFmParamsFromObject();
        break;
    case OscMode::OPZX7:
        opzx7Gui->pasteFmParamsFromObject();
        break;
    };
}

void AudioPlugin2686VEditor::initParams()
{
    int targetMode = tabs.getCurrentTabIndex();

    if (targetMode == (int)OscMode::BEEP + 1) { // Curve
        curveGui->initParams();

        return;
    }

    switch ((OscMode)targetMode)
    {
    case OscMode::OPNA:
        opnaGui->initParams();
        break;
    case OscMode::OPN:
        opnGui->initParams();
        break;
    case OscMode::OPL:
        oplGui->initParams();
        break;
    case OscMode::OPL3:
        opl3Gui->initParams();
        break;
    case OscMode::OPM:
        opmGui->initParams();
        break;
    case OscMode::OPZX7:
        opzx7Gui->initParams();
        break;
    case OscMode::SSG:
        ssgGui->initParams();
        break;
    case OscMode::WAVETABLE:
        wtGui->initParams();
        break;
    case OscMode::WT2:
        wt2Gui->initParams();
        break;
    case OscMode::RHYTHM:
        rhythmGui->initParams();
        break;
    case OscMode::ADPCM:
        adpcmGui->initParams();
        break;
    case OscMode::BEEP:
        beepGui->initParams();
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

GuiCurve* AudioPlugin2686VEditor::getCurveGui() {
    return curveGui.get();
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
    opnaGui->setLevel(level);
    opnGui->setLevel(level);
    oplGui->setLevel(level);
    opl3Gui->setLevel(level);
    opmGui->setLevel(level);
    opzx7Gui->setLevel(level);
    ssgGui->setLevel(level);
    wtGui->setLevel(level);
    wt2Gui->setLevel(level);
    rhythmGui->setLevel(level);
    adpcmGui->setLevel(level);
    beepGui->setLevel(level);
}

void AudioPlugin2686VEditor::copyRhythmPadParams(int from, int to) {
    CopyRhythmPad padParams;

    rhythmGui->copyPadParams(from, padParams);

    rhythmGui->pastePadParams(to, padParams);
}

void AudioPlugin2686VEditor::copyOplOpParams(int from, int to) {
    CopyOplOp opParams;

    oplGui->copyOpParams(from, opParams);

    oplGui->pasteOpParams(to, opParams);
}


void AudioPlugin2686VEditor::copyOpl3OpParams(int from, int to) {
    CopyOpl3Op opParams;

    opl3Gui->copyOpParams(from, opParams);

    opl3Gui->pasteOpParams(to, opParams);
}


void AudioPlugin2686VEditor::copyOpmOpParams(int from, int to) {
    CopyOpmOp opParams;

    opmGui->copyOpParams(from, opParams);

    opmGui->pasteOpParams(to, opParams);
}


void AudioPlugin2686VEditor::copyOpnOpParams(int from, int to) {
    CopyOpnOp opParams;

    opnGui->copyOpParams(from, opParams);

    opnGui->pasteOpParams(to, opParams);
}


void AudioPlugin2686VEditor::copyOpnaOpParams(int from, int to) {
    CopyOpnaOp opParams;

    opnaGui->copyOpParams(from, opParams);

    opnaGui->pasteOpParams(to, opParams);
}


void AudioPlugin2686VEditor::copyOpzx7OpParams(int from, int to) {
    CopyOpzx7Op opParams;

    opzx7Gui->copyOpParams(from, opParams);

    opzx7Gui->pasteOpParams(to, opParams);
}

void AudioPlugin2686VEditor::copyOplParamsToOpl3() {
    CopyOpl oplParams;
    CopyOpl3 opl3Params;

    oplGui->copyParams(oplParams);

    // アルゴリズムは母数が違うのでコピーしない
    opl3Params.fmBase.level = oplParams.fmBase.level;
    opl3Params.fmBase.feedback = oplParams.fmBase.feedback;
    opl3Params.quality = oplParams.quality;
    opl3Params.unison = oplParams.unison;

    opl3Gui->pasteParams(opl3Params);

    for (int i = 0; i < 2; i++) {
        CopyOplOp oplOpParams;
        CopyOpl3Op opl3OpParams;

        oplGui->copyOpParams(i, oplOpParams);

        opl3OpParams.detune = oplOpParams.detune;
        opl3OpParams.aAdsr = oplOpParams.aAdsr;
        // waveSelectは、母数が違うためコピーしない
        opl3OpParams.lfo = oplOpParams.lfo;
        opl3OpParams.pAdsr = oplOpParams.pAdsr;
        opl3OpParams.aSsgSw = oplOpParams.aSsgSw;
        opl3OpParams.mask = oplOpParams.mask;

        opl3Gui->pasteOpParams(i, opl3OpParams); // OP1 -> OP1 / OP2 -> OP2
        opl3Gui->pasteOpParams(i + 2, opl3OpParams); // OP1 -> OP3 / OP2 -> OP4
    }
}

void AudioPlugin2686VEditor::copyOplParamsToOpl312() {
    for (int i = 0; i < 2; i++) {
        CopyOplOp oplOpParams;
        CopyOpl3Op opl3OpParams;

        oplGui->copyOpParams(i, oplOpParams);

        opl3OpParams.detune = oplOpParams.detune;
        opl3OpParams.aAdsr = oplOpParams.aAdsr;
        // waveSelectは、母数が違うためコピーしない
        opl3OpParams.lfo = oplOpParams.lfo;
        opl3OpParams.pAdsr = oplOpParams.pAdsr;
        opl3OpParams.aSsgSw = oplOpParams.aSsgSw;
        opl3OpParams.mask = oplOpParams.mask;

        opl3Gui->pasteOpParams(i, opl3OpParams); // OP1 -> OP1 / OP2 -> OP2
    }
}

void AudioPlugin2686VEditor::copyOplParamsToOpl334() {
    for (int i = 0; i < 2; i++) {
        CopyOplOp oplOpParams;
        CopyOpl3Op opl3OpParams;

        oplGui->copyOpParams(i, oplOpParams);

        opl3OpParams.detune = oplOpParams.detune;
        opl3OpParams.aAdsr = oplOpParams.aAdsr;
        // waveSelectは、母数が違うためコピーしない
        opl3OpParams.lfo = oplOpParams.lfo;
        opl3OpParams.pAdsr = oplOpParams.pAdsr;
        opl3OpParams.aSsgSw = oplOpParams.aSsgSw;
        opl3OpParams.mask = oplOpParams.mask;

        opl3Gui->pasteOpParams(i + 2, opl3OpParams); // OP1 -> OP3 / OP2 -> OP4
    }
}

void AudioPlugin2686VEditor::copyOpl3ParamsToOpl() {
    CopyOpl3 opl3Params;
    CopyOpl oplParams;

    opl3Gui->copyParams(opl3Params);

    // アルゴリズムは母数が違うのでコピーしない
    oplParams.fmBase.level = opl3Params.fmBase.level;
    oplParams.fmBase.feedback = opl3Params.fmBase.feedback;
    oplParams.quality = opl3Params.quality;
    oplParams.unison = opl3Params.unison;

    oplGui->pasteParams(oplParams);

    // OPL3 の OP1, OP2 のパラメータを OPL にコピー
    for (int i = 0; i < 2; i++) {
        CopyOpl3Op opl3OpParams;
        CopyOplOp oplOpParams;

        opl3Gui->copyOpParams(i, opl3OpParams);

        oplOpParams.detune = opl3OpParams.detune;
        oplOpParams.aAdsr = opl3OpParams.aAdsr;
        oplOpParams.aAdsr.sus = false;
        // waveSelectは、母数が違うためコピーしない
        oplOpParams.lfo = opl3OpParams.lfo;
        oplOpParams.pAdsr = opl3OpParams.pAdsr;
        oplOpParams.aSsgSw = opl3OpParams.aSsgSw;
        oplOpParams.mask = opl3OpParams.mask;

        oplGui->pasteOpParams(i, oplOpParams); // OP1 -> OP1 / OP2 -> OP2
    }
}

void AudioPlugin2686VEditor::copyOpl312ParamsToOpl() {
    // OPL3 の OP1, OP2 のパラメータを OPL にコピー
    for (int i = 0; i < 2; i++) {
        CopyOpl3Op opl3OpParams;
        CopyOplOp oplOpParams;

        opl3Gui->copyOpParams(i, opl3OpParams);

        oplOpParams.detune = opl3OpParams.detune;
        oplOpParams.aAdsr = opl3OpParams.aAdsr;
        oplOpParams.aAdsr.sus = false;
        // waveSelectは、母数が違うためコピーしない
        oplOpParams.lfo = opl3OpParams.lfo;
        oplOpParams.pAdsr = opl3OpParams.pAdsr;
        oplOpParams.aSsgSw = opl3OpParams.aSsgSw;
        oplOpParams.mask = opl3OpParams.mask;

        oplGui->pasteOpParams(i, oplOpParams); // OP1 -> OP1 / OP2 -> OP2
    }
}

void AudioPlugin2686VEditor::copyOpl334ParamsToOpl() {
    // OPL3 の OP3, OP4 のパラメータを OPL にコピー
    for (int i = 0; i < 2; i++) {
        CopyOpl3Op opl3OpParams;
        CopyOplOp oplOpParams;

        opl3Gui->copyOpParams(i + 2, opl3OpParams);

        oplOpParams.detune = opl3OpParams.detune;
        oplOpParams.aAdsr = opl3OpParams.aAdsr;
        oplOpParams.aAdsr.sus = false;
        // waveSelectは、母数が違うためコピーしない
        oplOpParams.lfo = opl3OpParams.lfo;
        oplOpParams.pAdsr = opl3OpParams.pAdsr;
        oplOpParams.aSsgSw = opl3OpParams.aSsgSw;
        oplOpParams.mask = opl3OpParams.mask;

        oplGui->pasteOpParams(i, oplOpParams); // OP3 -> OP1 / OP4 -> OP2
    }
}

void AudioPlugin2686VEditor::copyOpnParamsToOpna() {
    CopyOpn opnParams;
    CopyOpna opnaParams;

    opnGui->copyParams(opnParams);

    opnaParams.fmBase = opnParams.fmBase;
    // OPNAへの pan は必ずC
    opnaParams.pan.pan = 0;
    opnaParams.n88Lfo = opnParams.n88Lfo;
    opnaParams.quality = opnParams.quality;
    opnaParams.unison = opnParams.unison;

    opnaGui->pasteParams(opnaParams);

    for (int i = 0; i < 4; i++) {
        CopyOpnOp opnOpParams;
        CopyOpnaOp opnaOpParams;

        opnGui->copyOpParams(i, opnOpParams);

        opnaOpParams.detune = opnOpParams.detune;
        opnaOpParams.aAdsr = opnOpParams.aAdsr;
        opnaOpParams.fix = opnOpParams.fix;
        opnaOpParams.n88Lfo = opnOpParams.n88Lfo;
        opnaOpParams.pAdsr = opnOpParams.pAdsr;
        opnaOpParams.aSsgSw = opnOpParams.aSsgSw;
        opnaOpParams.mask = opnOpParams.mask;
        opnaOpParams.waveSelect = 0;
        opnaOpParams.ssgEg.ssgEg = 0;
        opnaOpParams.ssgEg.fmSsgEgFreq = 0.0f;
        opnaOpParams.opnaLfo.freqsIndex = 0;
        opnaOpParams.opnaLfo.syncDelay = 0;
        opnaOpParams.opnaLfo.am = false;
        opnaOpParams.opnaLfo.amSmoothRate = 0.0f;
        opnaOpParams.opnaLfo.ams = 0.0f;
        opnaOpParams.opnaLfo.pm = false;
        opnaOpParams.opnaLfo.pms = 0.0f;

        opnaGui->pasteOpParams(i, opnaOpParams); // OP1 -> OP1 / OP2 -> OP2
    }
}

void AudioPlugin2686VEditor::copyOpnaParamsToOpn() {
    CopyOpna opnaParams;
    CopyOpn opnParams;

    opnaGui->copyParams(opnaParams);

    opnParams.fmBase = opnaParams.fmBase;
    opnParams.n88Lfo = opnaParams.n88Lfo;
    opnParams.quality = opnaParams.quality;
    opnParams.unison = opnaParams.unison;

    opnGui->pasteParams(opnParams);

    for (int i = 0; i < 4; i++) {
        CopyOpnaOp opnaOpParams;
        CopyOpnOp opnOpParams;

        opnaGui->copyOpParams(i, opnaOpParams);

        opnOpParams.detune = opnaOpParams.detune;
        opnOpParams.aAdsr = opnaOpParams.aAdsr;
        opnOpParams.fix = opnaOpParams.fix;
        opnOpParams.n88Lfo = opnaOpParams.n88Lfo;
        opnOpParams.pAdsr = opnaOpParams.pAdsr;
        opnOpParams.aSsgSw = opnaOpParams.aSsgSw;
        opnOpParams.mask = opnaOpParams.mask;

        opnGui->pasteOpParams(i, opnOpParams); // OP1 -> OP1 / OP2 -> OP2
    }
}

void AudioPlugin2686VEditor::copyOpnaParamsToOpm() {
    CopyOpnaOpnOpm params;

    opnaGui->copyParamsOpnOpm(params);

    opmGui->pasteParamsOpnaOpn(params);

    for (int i = 0; i < 4; i++) {
        CopyOpnaOpnOpmOp opParams;

        opnaGui->copyOpParamsOpnOpm(i, opParams);

        opmGui->pasteOpParamsOpnaOpn(i, opParams);
    }
}

void AudioPlugin2686VEditor::copyOpmParamsToOpna() {
    CopyOpnaOpnOpm params;

    opmGui->copyParamsOpnaOpn(params);

    opnaGui->pasteParamsOpnOpm(params);

    for (int i = 0; i < 4; i++) {
        CopyOpnaOpnOpmOp opParams;

        opmGui->copyOpParamsOpnaOpn(i, opParams);

        opnaGui->pasteOpParamsOpnOpm(i, opParams);
    }
}

void AudioPlugin2686VEditor::copyOpnParamsToOpm() {
    CopyOpnaOpnOpm params;

    opnGui->copyParamsOpnaOpm(params);

    opmGui->pasteParamsOpnaOpn(params);

    for (int i = 0; i < 4; i++) {
        CopyOpnaOpnOpmOp opParams;

        opnGui->copyOpParamsOpnaOpm(i, opParams);

        opmGui->pasteOpParamsOpnaOpn(i, opParams);
    }
}

void AudioPlugin2686VEditor::copyOpmParamsToOpn() {
    CopyOpnaOpnOpm params;

    opmGui->copyParamsOpnaOpn(params);

    opnGui->pasteParamsOpnaOpm(params);

    for (int i = 0; i < 4; i++) {
        CopyOpnaOpnOpmOp opParams;

        opmGui->copyOpParamsOpnaOpn(i, opParams);

        opnGui->pasteOpParamsOpnaOpm(i, opParams);
    }
}

void AudioPlugin2686VEditor::updateFxOrder(){
    fxGui->updateFxOrder();
}
