#include <cstdio>
#include <vector>
#include <initializer_list>
#include <utility>

#include "PluginEditor.h"

#include "../Processor/PluginProcessor.h"

#include "../Processor/Keys.h"
#include "../Const/FileValues.h"
#include "../../Gui/Preset/Keys.h"

#include "../Fm/SliderRegMap.h"
#include "../Fm/RegisterConverter.h"

#include "../Gui/GuiText.h"
#include "../Gui/GuiValues.h"
#include "../Gui/GuiColor.h"
#include "../Gui/GuiContext.h"

AudioPlugin2686VEditor::AudioPlugin2686VEditor(AudioPlugin2686V& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    GuiContext context(audioProcessor, *this, audioProcessor.apvts, sliderRegMap);

    opnaGui = std::make_unique<GuiOpna>(context);
	opnGui = std::make_unique<GuiOpn>(context);
	oplGui = std::make_unique<GuiOpl>(context);
	opl3Gui = std::make_unique<GuiOpl3>(context);
	opmGui = std::make_unique<GuiOpm>(context);
	opzx7Gui = std::make_unique<GuiOpzx7>(context);
	ssgGui = std::make_unique<GuiSsg>(context);
	wtGui = std::make_unique<GuiWt>(context);
	rhythmGui = std::make_unique<GuiRhythm>(context);
	adpcmGui = std::make_unique<GuiAdpcm>(context);
    beepGui = std::make_unique<GuiBeep>(context);
    presetGui = std::make_unique<GuiPreset>(context);
    fxGui = std::make_unique<GuiFx>(context);
	settingsGui = std::make_unique<GuiSettings>(context);
	aboutGui = std::make_unique<GuiAbout>(context);

    wtGui->addComponentListener(this);

    tabs.getTabbedButtonBar().addChangeListener(this);

    audioProcessor.apvts.addParameterListener(CorePrKey::mode, this);

    setupLogo();

    opnaGui->setup();
    opnGui->setup();
    oplGui->setup();
    opl3Gui->setup();
    opmGui->setup();
    opzx7Gui->setup();
    ssgGui->setup();
    wtGui->setup();
    rhythmGui->setup();
    adpcmGui->setup();
    beepGui->setup();
    presetGui->setup();
    fxGui->setup();
    settingsGui->setup();
    aboutGui->setup();

    // Initial Wallpaper Load
    loadWallpaperImage();

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

    addChildComponent(staticPreview);
    addChildComponent(realtimePreview);

    // プレビュー表示切替ボタン
    addAndMakeVisible(togglePreviewBtn);
    togglePreviewBtn.setButtonText(isPreviewVisible ? u8"<<" : u8">>");
    togglePreviewBtn.setTooltip(isPreviewVisible ? u8"Hide Preview" : u8"Show Preview");
    togglePreviewBtn.setLookAndFeel(&togglePreviewButtonLF);
    togglePreviewBtn.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    togglePreviewBtn.setColour(juce::TextButton::buttonColourId, juce::Colours::green.withAlpha(0.9f));
    togglePreviewBtn.onClick = [this] {
        isPreviewVisible = !isPreviewVisible;
        updatePreviewVisibilityToProcessor();

        staticPreview.setVisible(isPreviewVisible);
        realtimePreview.setVisible(isPreviewVisible);
        togglePreviewBtn.setButtonText(isPreviewVisible ? u8"<<" : u8">>");
        togglePreviewBtn.setTooltip(isPreviewVisible ? u8"Hide Preview" : u8"Show Preview");

        // ウィンドウの幅を動的に変更
        int newWidth = isPreviewVisible ? CoreGuiValue::Window::width + CoreGuiValue::Preview::extraWidth : CoreGuiValue::Window::width;
        int height = audioProcessor.showVirtualKeyboard ? CoreGuiValue::Window::height + CoreGuiValue::KeyboardHeight : CoreGuiValue::Window::height;
        setSize(newWidth, height); // 高さは固定、幅を伸縮

        // タイマーのON/OFFを切り替え
        updateTimerState();
    };

    // パニックボタン
    addAndMakeVisible(panicButton);
    panicButton.setVisible(true);
    panicButton.setButtonText("!");
    panicButton.setTooltip(u8"Reset Audio Engine");
    panicButton.setLookAndFeel(&panicButtonLF);
    panicButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    panicButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red.withAlpha(0.9f));
    panicButton.onClick = [this] {
        audioProcessor.panic();
        };


    // アンドゥボタンボタン
    addAndMakeVisible(undoButton);
    undoButton.setVisible(true);
    undoButton.setButtonText(u8"Un");
    undoButton.setTooltip("Nothing to undo");
    undoButton.setLookAndFeel(&undoButtonLF);
    undoButton.setColour(juce::TextButton::textColourOnId, juce::Colours::yellow);
    undoButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue.withAlpha(0.9f));
    undoButton.onClick = [this] {
        audioProcessor.undoManager.undo();
        };

    // リドゥボタン
    addAndMakeVisible(redoButton);
    redoButton.setVisible(true);
    redoButton.setButtonText(u8"Re");
    redoButton.setTooltip("Nothing to Redo");
    redoButton.setLookAndFeel(&redoButtonLF);
    redoButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    redoButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue.withAlpha(0.9f));
    redoButton.onClick = [this] {
        audioProcessor.undoManager.redo();
        };

    audioProcessor.undoManager.addChangeListener(this);

    updateUndoRedoButtons();

    // パラメーターコピーボタン
    addAndMakeVisible(copyParamsButton);
    copyParamsButton.setVisible(true);
    copyParamsButton.setButtonText(u8"C");
    copyParamsButton.setTooltip("Copy FM Parameters");
    copyParamsButton.setLookAndFeel(&copyParamsButtonLF);
    copyParamsButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    copyParamsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::yellow.darker(0.6f).withAlpha(0.9f));
    copyParamsButton.onClick = [this] {
        copyFmParamsToString();
        copyFmParamsToObject();
        };

    // パラメーターペーストボタン
    addAndMakeVisible(pasteParamsButton);
    pasteParamsButton.setVisible(true);
    pasteParamsButton.setButtonText(u8"P");
    pasteParamsButton.setTooltip("Paste FM Parameters");
    pasteParamsButton.setLookAndFeel(&pasteParamsButtonLF);
    pasteParamsButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    pasteParamsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::yellow.darker(0.6f).withAlpha(0.9f));
    pasteParamsButton.onClick = [this] {
        pasteFmParamsFromObject();
        };

    updateParameterCopyPasteButtons();

    // パラメーター初期化ボタン
    addAndMakeVisible(initParamsButton);
    initParamsButton.setVisible(true);
    initParamsButton.setButtonText(u8"Reset");
    initParamsButton.setTooltip("Reset Parameters");
    initParamsButton.setLookAndFeel(&initParamsButtonLF);
    initParamsButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white.darker(0.8f));
    initParamsButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    initParamsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white.darker(0.2f).withAlpha(0.9f));
    initParamsButton.onClick = [this] {
        initParams();
        };

    updateParameterInitializeButtons();

    midiKeyboard = std::make_unique<juce::MidiKeyboardComponent>(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard);

    // 鍵盤を画面に追加し、PCキーボードのフォーカスを受け取れるようにする
    addAndMakeVisible(*midiKeyboard);
    midiKeyboard->setWantsKeyboardFocus(true);

    midiKeyboard->setVisible(audioProcessor.showVirtualKeyboard);

    addAndMakeVisible(*fxGui);
    fxGui->setVisible(true);

    int initialHeight = audioProcessor.showVirtualKeyboard ? CoreGuiValue::Window::height + CoreGuiValue::KeyboardHeight : CoreGuiValue::Window::height;
    setSize(CoreGuiValue::Window::width, initialHeight);
}

AudioPlugin2686VEditor::~AudioPlugin2686VEditor()
{
    tabs.setLookAndFeel(nullptr);
    tabs.getTabbedButtonBar().removeChangeListener(this);

    wtGui->removeComponentListener(this);

    adpcmGui->removeLoadButtonListener(this);

    rhythmGui->removeLoadButtonListener(this);

    audioProcessor.apvts.removeParameterListener(CorePrKey::mode, this);

    audioProcessor.undoManager.removeChangeListener(this);

    stopTimer();
}

void AudioPlugin2686VEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &tabs.getTabbedButtonBar())
    {
        // 0:OPNA, 1:OPN, 2:OPL, ...
        int targetMode = tabs.getCurrentTabIndex();

        if (targetMode >= 0 && targetMode <= (int)OscMode::BEEP) // BEEP is 11
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
        }

        updateTimerState();
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
}

void AudioPlugin2686VEditor::resized()
{
    auto area = getLocalBounds();

    // 画面の一番下を鍵盤UIに割り当てる
    if (audioProcessor.showVirtualKeyboard && midiKeyboard != nullptr)
    {
        midiKeyboard->setBounds(area.removeFromBottom(CoreGuiValue::KeyboardHeight));
    }

    if (isPreviewVisible)
    {
        auto rightArea = area.removeFromRight(CoreGuiValue::Preview::extraWidth);
        // 上下に並べる
        staticPreview.setBounds(rightArea.getX() + 4, 30, CoreGuiValue::Preview::drawSize, CoreGuiValue::Preview::drawSize);
        realtimePreview.setBounds(rightArea.getX() + 4, 330, CoreGuiValue::Preview::drawSize, CoreGuiValue::Preview::drawSize);
    }

    int previewPaddingRight = isPreviewVisible ? CoreGuiValue::Preview::drawSize + 8: 0;
    togglePreviewBtn.setBounds(getWidth() - 45, 5, 40, 20);
    panicButton.setBounds(getWidth() - 80 - previewPaddingRight, 5, 30, 20);
    redoButton.setBounds(getWidth() - 125 - previewPaddingRight, 5, 40, 20);
    undoButton.setBounds(getWidth() - 170 - previewPaddingRight, 5, 40, 20);
#if true
    copyParamsButton.setBounds(getWidth() - 215 - previewPaddingRight, 5, 40, 20);
    initParamsButton.setBounds(getWidth() - 280 - previewPaddingRight, 5, 60, 20);
#else
    pasteParamsButton.setBounds(getWidth() - 215 - previewPaddingRight, 5, 40, 20);
    copyParamsButton.setBounds(getWidth() - 260 - previewPaddingRight, 5, 40, 20);
    initParamsButton.setBounds(getWidth() - 325 - previewPaddingRight, 5, 60, 20);
#endif

    logoLabel.setBounds(area.reduced(CoreGuiValue::Group::Padding::width, CoreGuiValue::Group::Padding::height));

    tabs.setBounds(area);

    // タブの中身（コンテンツ領域）
    auto content = tabs.getLocalBounds();
    auto tabContent = content.removeFromLeft(content.getWidth() - 300);
    tabContent.removeFromTop(tabs.getTabBarDepth()).reduce(CoreGuiValue::Group::Padding::width, CoreGuiValue::Group::Padding::height); // 全体の余白

    opnaGui->layout(tabContent);
    opnGui->layout(tabContent);
    oplGui->layout(tabContent);
    opl3Gui->layout(tabContent);
    opmGui->layout(tabContent);
    opzx7Gui->layout(tabContent);
    ssgGui->layout(tabContent);
    wtGui->layout(tabContent);
    rhythmGui->layout(tabContent);
    adpcmGui->layout(tabContent);
    beepGui->layout(tabContent);
    presetGui->layout(tabContent);
    settingsGui->layout(tabContent);
    aboutGui->layout(tabContent);
    
    content.removeFromTop(tabs.getTabBarDepth());
    fxGui->setBounds(content);
    fxGui->layout(content);
}

void AudioPlugin2686VEditor::drawBg(juce::Graphics& g)
{
    auto fullArea = getLocalBounds().toFloat();

    // 拡張パネルを含まない、本来のメイン画面の固定サイズ
    juce::Rectangle<float> baseArea(0.0f, 0.0f, (float)CoreGuiValue::Window::width, (float)CoreGuiValue::Window::height);

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

    // フォント変更: Bold + Italic, サイズ 128.0f
    logoLabel.setFont(juce::Font(CoreGuiValue::WaterMarkLogo::fontFamily, CoreGuiValue::WaterMarkLogo::fontSize, juce::Font::bold | juce::Font::italic));

    // 右下寄せ
    logoLabel.setJustificationType(juce::Justification::bottomRight);

    // 色設定 (背景になじむように少し透明度を入れると良いですが、ここでは白ではっきり表示)
    logoLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(CoreGuiValue::WaterMarkLogo::fontAlpha));

    addAndMakeVisible(logoLabel);

    // 最背面へ移動 (これでタブの後ろに行きます)
    logoLabel.toBack();
}

void AudioPlugin2686VEditor::setupTabs(juce::TabbedComponent& tabs)
{
    addAndMakeVisible(tabs);
    tabs.addTab(CoreGuiText::Tab::opna, juce::Colours::transparentBlack, opnaGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::opn, juce::Colours::transparentBlack, opnGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::opl, juce::Colours::transparentBlack, oplGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::opl3, juce::Colours::transparentBlack, opl3Gui.get(), true);
    tabs.addTab(CoreGuiText::Tab::opm, juce::Colours::transparentBlack, opmGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::opzx7, juce::Colours::transparentBlack, opzx7Gui.get(), true);
    tabs.addTab(CoreGuiText::Tab::ssg, juce::Colours::transparentBlack, ssgGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::wt, juce::Colours::transparentBlack, wtGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::rhythm, juce::Colours::transparentBlack, rhythmGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::adpcm, juce::Colours::transparentBlack, adpcmGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::beep, juce::Colours::transparentBlack, beepGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::preset, juce::Colours::transparentBlack, presetGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::settings, juce::Colours::transparentBlack, settingsGui.get(), true);
    tabs.addTab(CoreGuiText::Tab::about, juce::Colours::transparentBlack, aboutGui.get(), true);
}

void AudioPlugin2686VEditor::loadPresetFile(const juce::File& file)
{
    audioProcessor.loadPreset(file);
    audioProcessor.presetFilePath = file.getFullPathName();

    presetGui->setMetaData(audioProcessor.presetName, audioProcessor.presetAuthor, audioProcessor.presetVersion, audioProcessor.presetComment, audioProcessor.presetGenre, audioProcessor.presetFilePath);

    // 1. リズム音源のファイル名を復元
    // Io::empty 以外の文字列を渡すことで、プロセッサ内に保持されたパスから再読み込みさせます
    updateRhythmFileNames("Reload");

    // 2. ADPCMのファイル名を復元
    updateAdpcmFileName("Reload");

    // 3. OPZX7のPCMファイル名を復元
    for (int i = 0; i < 4; ++i) {
        juce::String path = audioProcessor.opzx7PcmFilePaths[i];
        juce::String text = Io::empty;

        if (path.isNotEmpty()) {
            // 相対パスを絶対パスに復元してからファイル名を取得する
            juce::File f = audioProcessor.resolvePath(path);
            text = f.getFileName();
        }

        opzx7Gui->updatePcmFileName(i, text);
    }

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
    fileChooser = std::make_unique<juce::FileChooser>("Load Environment Settings",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "*.xml");

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {
                audioProcessor.loadEnvironment(file);

                // UI反映
                settingsGui->setSettings(
                    audioProcessor.wallpaperPath.isEmpty() ? Io::empty : juce::File(audioProcessor.wallpaperPath).getFileName(),
                    audioProcessor.defaultSampleDir,
                    audioProcessor.defaultPresetDir,
                    audioProcessor.defaultWavetableDir
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
            .withTitle("Overwrite Preset")
            .withMessage("Overwrite existing preset file?\n\n" + saveFile.getFileName())
            .withButton("Overwrite")
            .withButton("Cancel"),
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

    openWriteFileChooser("Save Preset As", defaultFile, "*.xml", [this](const juce::FileChooser& fc) {
        auto file = fc.getResult();
        if (file != juce::File{}) {
            // 保存したファイルパスを記録
            audioProcessor.presetFilePath = file.getFullPathName();
            presetGui->setMetaData(audioProcessor.presetName, audioProcessor.presetAuthor, audioProcessor.presetVersion, audioProcessor.presetComment, audioProcessor.presetGenre, audioProcessor.presetFilePath);

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
        content.reduce(CoreGuiValue::Group::Padding::width, CoreGuiValue::Group::Padding::height); // 全体の余白

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
                    audioProcessor.loadAdpcmFile(file);
                    adpcmGui->updateFileName(file.getFileName());
                    audioProcessor.lastSampleDirectory = file.getParentDirectory();
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
    auto* w = new juce::AlertWindow("Set Register Value", "Enter integer value:", juce::AlertWindow::QuestionIcon);

    // テキストエディタを追加
    w->addTextEditor("regInput", "", "0");

    // ボタン設定
    w->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
    w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

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
                    tooltipText = juce::String((int)min) + " - " + juce::String((int)max);
                }
                else {
                    tooltipText = juce::String(min, 1) + " - " + juce::String(max, 1);
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

void AudioPlugin2686VEditor::updateOpzx7FileNames(const juce::String filename)
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

void AudioPlugin2686VEditor::updateAdpcmFileName(const juce::String filename)
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
    // コンポーネントの表示/非表示を切り替え
    if (midiKeyboard != nullptr) {
        midiKeyboard->setVisible(audioProcessor.showVirtualKeyboard);
    }

    // プレビューの開閉状態（幅）と、キーボードの開閉状態（高さ）を両方考慮してリサイズ
    int targetWidth = isPreviewVisible ? CoreGuiValue::Window::width + CoreGuiValue::Preview::extraWidth : CoreGuiValue::Window::width;
    int targetHeight = audioProcessor.showVirtualKeyboard ? CoreGuiValue::Window::height + CoreGuiValue::KeyboardHeight : CoreGuiValue::Window::height;

    setSize(targetWidth, targetHeight);
}

void AudioPlugin2686VEditor::timerCallback()
{
    if (isPreviewVisible)
    {
        // 1. 静的波形（完成波形）の更新
        std::vector<float> staticData;

        audioProcessor.generatePreviewWaveform(staticData);
        staticPreview.pushBuffer(staticData.data(), (int)staticData.size());
        // 2. リアルタイム波形の更新
        std::vector<float> realTimeData(512);

        for (int i = 0; i < 512; ++i) {
            realTimeData[i] = audioProcessor.realTimeBuffer[i].load(std::memory_order_relaxed);
        }
        realtimePreview.pushBuffer(realTimeData.data(), 512);
    }
}

void AudioPlugin2686VEditor::updateTimerState()
{
    // プレビューが開いていている時だけタイマーを動かす
    if (isPreviewVisible) {
        startTimerHz(15);
        timerCallback(); // タイマー開始時に即座に1回強制描画する！
    }
    else {
        stopTimer(); // 閉じてる時、または設定・About画面では負荷ゼロにする
    }
}

void AudioPlugin2686VEditor::updatePreviewVisibilityToProcessor()
{
    audioProcessor.previewVisiblity = isPreviewVisible;
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

    return false; // 他のキー入力は通常の処理へ
}

void AudioPlugin2686VEditor::updateUndoRedoButtons()
{
    // canUndo / canRedo は履歴の有無を bool で返してくれる
    undoButton.setEnabled(audioProcessor.undoManager.canUndo());
    redoButton.setEnabled(audioProcessor.undoManager.canRedo());

    if (audioProcessor.undoManager.canUndo())
        undoButton.setTooltip("Undo");
    else
        undoButton.setTooltip("Nothing to undo");

    if (audioProcessor.undoManager.canRedo())
        redoButton.setTooltip("Redo");
    else
        redoButton.setTooltip("Nothing to redo");
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
