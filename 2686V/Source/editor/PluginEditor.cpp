#include <cstdio>
#include <vector>
#include <initializer_list>
#include <utility>

#include "PluginEditor.h"
#include "../processor/PluginProcessor.h"
#include "../const/FileConstants.h"
#include "../gui/GuiColor.h"
#include "../gui/GuiContext.h"
#include "../fm/SliderRegMap.h"

AudioPlugin2686VEditor::AudioPlugin2686VEditor(AudioPlugin2686V& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    GuiContext context(audioProcessor, *this, audioProcessor.apvts, sliderRegMap);

#if !defined(BUILD_AS_FX_PLUGIN)
    opnaGui = std::make_unique<GuiOpna>(context);
	opnGui = std::make_unique<GuiOpn>(context);
	oplGui = std::make_unique<GuiOpl>(context);
	opl3Gui = std::make_unique<GuiOpl3>(context);
	opmGui = std::make_unique<GuiOpm>(context);
	opzx3Gui = std::make_unique<GuiOpzx3>(context);
	ssgGui = std::make_unique<GuiSsg>(context);
	wtGui = std::make_unique<GuiWt>(context);
	rhythmGui = std::make_unique<GuiRhythm>(context);
	adpcmGui = std::make_unique<GuiAdpcm>(context);
    presetGui = std::make_unique<GuiPreset>(context);
#endif
    fxGui = std::make_unique<GuiFx>(context);
	settingsGui = std::make_unique<GuiSettings>(context);
	aboutGui = std::make_unique<GuiAbout>(context);

#if !defined(BUILD_AS_FX_PLUGIN)
    wtGui->addComponentListener(this);
#endif
    tabs.getTabbedButtonBar().addChangeListener(this);
    audioProcessor.apvts.addParameterListener(codeMode, this);

    setupLogo();

#if !defined(BUILD_AS_FX_PLUGIN)
    opnaGui->setup();
    opnGui->setup();
    oplGui->setup();
    opl3Gui->setup();
    opmGui->setup();
    opzx3Gui->setup();
    ssgGui->setup();
    wtGui->setup();
    rhythmGui->setup();
    adpcmGui->setup();
    presetGui->setup();
#endif
    fxGui->setup();
    settingsGui->setup();
    aboutGui->setup();

    // Initial Wallpaper Load
    loadWallpaperImage();

    tabs.setLookAndFeel(&customTabLF);

    setupTabs(tabs);

    int currentMode = (int)*audioProcessor.apvts.getRawParameterValue(codeMode);
    tabs.setCurrentTabIndex(currentMode);

#if !defined(BUILD_AS_FX_PLUGIN)
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
#endif

    addAndMakeVisible(togglePreviewBtn);
    addChildComponent(staticPreview);
    addChildComponent(realtimePreview);

    togglePreviewBtn.onClick = [this] {
        isPreviewVisible = !isPreviewVisible;

        staticPreview.setVisible(isPreviewVisible);
        realtimePreview.setVisible(isPreviewVisible);
        togglePreviewBtn.setButtonText(isPreviewVisible ? "<<" : ">>");

        // ウィンドウの幅を動的に変更
        int newWidth = isPreviewVisible ? WindowWidth + PreviewExtraWidth : WindowWidth;
        setSize(newWidth, WindowHeight); // 高さは固定、幅を伸縮

        // タイマーのON/OFFを切り替え
        updateTimerState();
    };

    setSize(WindowWidth, WindowHeight);
}

AudioPlugin2686VEditor::~AudioPlugin2686VEditor()
{
    tabs.setLookAndFeel(nullptr);

    tabs.getTabbedButtonBar().removeChangeListener(this);
#if !defined(BUILD_AS_FX_PLUGIN)
    wtGui->removeComponentListener(this);
    adpcmGui->removeLoadButtonListener(this);

	rhythmGui->removeLoadButtonListener(this);
#endif
    audioProcessor.apvts.removeParameterListener(codeMode, this);

    stopTimer();
}

void AudioPlugin2686VEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
// 2686FXでは不要
#if !defined(BUILD_AS_FX_PLUGIN)
    if (source == &tabs.getTabbedButtonBar())
    {
        // 0:OPNA, 1:OPN, 2:OPL, ...
        int targetMode = tabs.getCurrentTabIndex();

        if (targetMode >= 0 && targetMode <= (int)OscMode::ADPCM) // ADPCM is 10
        {
            auto* param = audioProcessor.apvts.getParameter(codeMode);
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
    }
#endif
}

void AudioPlugin2686VEditor::paint(juce::Graphics& g)
{
	drawBg(g);
}

void AudioPlugin2686VEditor::resized()
{
    auto area = getLocalBounds();

    togglePreviewBtn.setBounds(getWidth() - 35, 5, 30, 20);

    if (isPreviewVisible)
    {
        auto rightArea = area.removeFromRight(PreviewExtraWidth);

        // 描画区画を 200 x 200 に設定 (タブの少し下あたりに配置)
        // Y座標はタブバーの高さなどに合わせて調整してください (ここでは30px下から)
        staticPreview.setBounds(rightArea.getX() + 4, 30, PreviewDrawSize, PreviewDrawSize);
        realtimePreview.setBounds(rightArea.getX() + 4, 330, PreviewDrawSize, PreviewDrawSize);
    }

    logoLabel.setBounds(area.reduced(GlobalPaddingWidth, GlobalPaddingHeight));

    tabs.setBounds(area);

    // タブの中身（コンテンツ領域）
    auto content = tabs.getLocalBounds();
    content.removeFromTop(tabs.getTabBarDepth()).reduce(GlobalPaddingWidth, GlobalPaddingHeight); // 全体の余白

#if !defined(BUILD_AS_FX_PLUGIN)
    opnaGui->layout(content);
    opnGui->layout(content);
    oplGui->layout(content);
    opl3Gui->layout(content);
    opmGui->layout(content);
    opzx3Gui->layout(content);
    ssgGui->layout(content);
    wtGui->layout(content);
    rhythmGui->layout(content);
    adpcmGui->layout(content);
#endif
    fxGui->layout(content);
#if !defined(BUILD_AS_FX_PLUGIN)
    presetGui->layout(content);
#endif
    settingsGui->layout(content);
    aboutGui->layout(content);
}

void AudioPlugin2686VEditor::drawBg(juce::Graphics& g)
{
    if (backgroundImage.isValid())
    {
        int mode = audioProcessor.wallpaperMode;

        // Fit または Original の場合、余白にぼかし背景を敷き詰める
        if (mode == 2 || mode == 3)
        {
            if (blurredBackgroundImage.isValid())
            {
                // ぼかし画像を画面全体に Fill (アスペクト比を維持して隙間なく拡大) で描画
                g.drawImage(blurredBackgroundImage, getLocalBounds().toFloat(), juce::RectanglePlacement::fillDestination);

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
        g.drawImage(backgroundImage, getLocalBounds().toFloat(), placement);

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
    logoLabel.setText(pluginName, juce::dontSendNotification);

    // フォント変更: Bold + Italic, サイズ 128.0f
    logoLabel.setFont(juce::Font(FontFamily, LogoFontSize, juce::Font::bold | juce::Font::italic));

    // 右下寄せ
    logoLabel.setJustificationType(juce::Justification::bottomRight);

    // 色設定 (背景になじむように少し透明度を入れると良いですが、ここでは白ではっきり表示)
    logoLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.3f)); // 0.3fで透かし風

    addAndMakeVisible(logoLabel);

    // 最背面へ移動 (これでタブの後ろに行きます)
    logoLabel.toBack();
}

void AudioPlugin2686VEditor::setupTabs(juce::TabbedComponent& tabs)
{
    addAndMakeVisible(tabs);
    // FXモードならシンセ系タブを追加しない
#if defined(BUILD_AS_FX_PLUGIN)
    tabs.addTab(fxTabName, juce::Colours::transparentBlack, fxGui.get(), true);
    tabs.addTab(settingsTabName, juce::Colours::transparentBlack, settingsGui.get(), true);
    tabs.addTab(aboutTabName, juce::Colours::transparentBlack, aboutGui.get(), true);
#else
    tabs.addTab(opnaTabName, juce::Colours::transparentBlack, opnaGui.get(), true);
    tabs.addTab(opnTabName, juce::Colours::transparentBlack, opnGui.get(), true);
    tabs.addTab(oplTabName, juce::Colours::transparentBlack, oplGui.get(), true);
    tabs.addTab(opl3TabName, juce::Colours::transparentBlack, opl3Gui.get(), true);
    tabs.addTab(opmTabName, juce::Colours::transparentBlack, opmGui.get(), true);
    tabs.addTab(opzx3TabName, juce::Colours::transparentBlack, opzx3Gui.get(), true);
    tabs.addTab(ssgTabName, juce::Colours::transparentBlack, ssgGui.get(), true);
    tabs.addTab(wtTabName, juce::Colours::transparentBlack, wtGui.get(), true);
    tabs.addTab(rhythmTabName, juce::Colours::transparentBlack, rhythmGui.get(), true);
    tabs.addTab(adpcmTabName, juce::Colours::transparentBlack, adpcmGui.get(), true);
    tabs.addTab(fxTabName, juce::Colours::transparentBlack, fxGui.get(), true);
    tabs.addTab(presetTabName, juce::Colours::transparentBlack, presetGui.get(), true);
    tabs.addTab(settingsTabName, juce::Colours::transparentBlack, settingsGui.get(), true);
    tabs.addTab(aboutTabName, juce::Colours::transparentBlack, aboutGui.get(), true);
#endif
}

#if !defined(BUILD_AS_FX_PLUGIN)
// ヘルパー: プリセットロード処理 (共通化)
void AudioPlugin2686VEditor::loadPresetFile(const juce::File& file)
{
    audioProcessor.loadPreset(file);

	presetGui->setMetaData(audioProcessor.presetName, audioProcessor.presetAuthor, audioProcessor.presetVersion, audioProcessor.presetComment);

    // リズム音源ファイル名エリアに反映
    updateRhythmFileNames(emptyFilename);

    // ADPCMファイル名エリアに反映
    if (audioProcessor.adpcmFilePath.isNotEmpty()) {
        adpcmGui->updateFileName(juce::File(audioProcessor.adpcmFilePath).getFileName());
    }
    else {
        adpcmGui->updateFileName(emptyFilename);
    }
}
#endif

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
                    audioProcessor.wallpaperPath.isEmpty() ? emptyFilename : juce::File(audioProcessor.wallpaperPath).getFileName(),
                    audioProcessor.defaultSampleDir,
                    audioProcessor.defaultPresetDir
                );

                // 壁紙再描画
                loadWallpaperImage();

#if !defined(BUILD_AS_FX_PLUGIN)
                // プリセットリスト更新
                if (juce::File(audioProcessor.defaultPresetDir).isDirectory()) {
                    presetGui->currentFolder = juce::File(audioProcessor.defaultPresetDir);
                    presetGui->updatePresetPath();
                    scanPresets(); // リスト更新関数を呼ぶ
                }
#endif
            }
        }
    );

}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686VEditor::scanPresets()
{
    presetGui->clearTable();

    // XMLファイルを探す
    auto files = presetGui->currentFolder.findChildFiles(juce::File::findFiles, false, globFiles);

    for (const auto& file : files)
    {
        PresetItem item;
        item.file = file;
        item.fileName = file.getFileName();

        // XMLをパースしてメタデータを取得
        juce::XmlDocument xmlDoc(file);
        auto xml = xmlDoc.getDocumentElement();
        if (xml != nullptr)
        {
            item.name = xml->getStringAttribute(settingPresetName, audioProcessor.presetName);
            item.author = xml->getStringAttribute(settingPresetAuthor, audioProcessor.presetAuthor);
            item.version = xml->getStringAttribute(settingPresetVersion, audioProcessor.presetVersion);
            item.comment = xml->getStringAttribute(settingPresetComment, audioProcessor.presetComment);
            item.modeName = xml->getStringAttribute(settingActiveModeName, "-");
        }
        else
        {
            item.name = invalidXmlNotice;
        }

        presetGui->items.push_back(item);
    }

    // リスト更新
    presetGui->updateTableContent();
    presetGui->repaintTable();
}

void AudioPlugin2686VEditor::saveCurrentPreset()
{
    juce::String filename = audioProcessor.presetName.trim();
    if (filename.isEmpty()) filename = "Untitled";
    filename = filename + ".xml";

    juce::File saveFile = presetGui->currentFolder.getChildFile(filename);

    if (saveFile.existsAsFile()) {
        // 上書き確認
        juce::AlertWindow::showAsync(juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::WarningIcon)
            .withTitle("Overwrite Preset")
            .withMessage("File exists. Overwrite?")
            .withButton("Overwrite")
            .withButton("Cancel"),
            [this, saveFile](int result) {
                if (result == 1) {
                    audioProcessor.savePreset(saveFile);
                    scanPresets(); // リスト更新
                }
            });
    }
    else {
        audioProcessor.savePreset(saveFile);
        scanPresets(); // リスト更新
    }
}
#endif

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
#if !defined(BUILD_AS_FX_PLUGIN)
    // wtPage のサイズが変わったときだけレイアウトを実行
    if (&component == wtGui.get() && wasResized)
    {
        auto content = tabs.getLocalBounds();
        content.removeFromTop(tabs.getTabBarDepth());
        content.reduce(GroupPaddingWidth, GroupPaddingHeight); // 全体の余白

        wtGui->layout(content);
    }
    // もし gui.page も登録したなら
    // if (&component == &gui.page && wasResized) layoutgui.page(); // (関数化していれば)
#endif
}

void AudioPlugin2686VEditor::buttonClicked(juce::Button* button)
{
#if !defined(BUILD_AS_FX_PLUGIN)
    // ADPCM Load Buttons
    if (adpcmGui->isThis(button))
    {
        // ... (Existing ADPCM load logic) ...
        auto fileFilter = audioProcessor.formatManager.getWildcardForAllFormats();
        openFileChooser(
            openAudioFileDialogTitle,
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
        fileChooser = std::make_unique<juce::FileChooser>(openAudioFileDialogTitle, audioProcessor.lastSampleDirectory, fileFilter);
        rhythmGui->buttonClicked(button, audioProcessor.formatManager, fileChooser);
    }
#endif
}

void AudioPlugin2686VEditor::mouseDown(const juce::MouseEvent& event)
{
#if !defined(BUILD_AS_FX_PLUGIN)
    if (event.mods.isRightButtonDown())
    {
        juce::Component* target = event.originalComponent;

        // Find if target (or its parent, as originalComponent might be the slider thumb) is in our map
        // JUCE sliders are complex components. We attached listener to the slider itself.
        // Event source should bubble up or be the slider if clicked on background.

        auto* slider = dynamic_cast<juce::Slider*>(event.eventComponent);
        if (slider && sliderRegMap.count(slider))
        {
            RegisterType type = sliderRegMap[slider];

            showRegisterInput(slider, [slider, type](int regValue) {
                float newVal = 0.0f;
                switch (type) {
                case RegisterType::FmAr: newVal = RegisterConverter::convertFmAr(regValue); break;
                case RegisterType::FmDr: newVal = RegisterConverter::convertFmDr(regValue); break;
                case RegisterType::FmSr: newVal = RegisterConverter::convertFmSr(regValue); break;
                case RegisterType::FmRr: newVal = RegisterConverter::convertFmRr(regValue); break;
                case RegisterType::FmSl:   newVal = RegisterConverter::convertFmSl(regValue); break;
                case RegisterType::FmTl:   newVal = RegisterConverter::convertFmTl(regValue); break;
                case RegisterType::FmMul:  newVal = (float)RegisterConverter::convertFmMul(regValue); break;
                case RegisterType::FmDt:   newVal = (float)RegisterConverter::convertFmDt(regValue); break;
                case RegisterType::FmDt2:  newVal = RegisterConverter::convertFmDt2(regValue); break;
                case RegisterType::SsgVol: newVal = RegisterConverter::convertSsgVol(regValue); break;
                case RegisterType::SsgEnv: newVal = RegisterConverter::convertSsgEnvPeriod(regValue); break;
                default: return;
                }
                slider->setValue(newVal, juce::sendNotification);
            });
        }
    }
#endif
}

#if !defined(BUILD_AS_FX_PLUGIN)
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
#endif

void AudioPlugin2686VEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == codeMode)
    {
        // UIスレッドで実行するために callAsync を使用
        juce::MessageManager::callAsync([this, idx = (int)newValue]() {
            // 現在のタブと違えば切り替える（ループ防止）
            if (tabs.getCurrentTabIndex() != idx) {
                tabs.setCurrentTabIndex(idx);
            }
            });
    }
}

#if !defined(BUILD_AS_FX_PLUGIN)
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
    if (filename == emptyFilename) {
        for (int i = 0; i < 8; ++i)
        {
            rhythmGui->updatePadFileName(i, filename);
        }

    }
    else {
        for (int i = 0; i < 8; ++i)
        {
            juce::String path = audioProcessor.rhythmFilePaths[i];
            juce::String text = emptyFilename;

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

void AudioPlugin2686VEditor::updateAdpcmFileName(const juce::String filename)
{
    if (filename == emptyFilename) {
        adpcmGui->updateFileName(filename);
    }
    else {
        juce::String path = audioProcessor.adpcmFilePath;
        juce::String text = emptyFilename;

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
#endif

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
    // 現在のタブがシンセ系かチェック (例: 0〜9 が OPNA〜ADPCM、それ以降がFXや設定など)
    // ※ ADPCM が 9 番目のタブであることを前提としています。構成に合わせて数字を調整してください。
    int currentTab = tabs.getCurrentTabIndex();
    bool isSynthTab = (currentTab <= 9);

    // プレビューが開いていて、かつシンセ画面の時だけタイマーを動かす
    if (isPreviewVisible && isSynthTab) {
        startTimerHz(15);
        timerCallback(); // ★タイマー開始時に即座に1回強制描画する！
    }
    else {
        stopTimer(); // 閉じてる時、または設定・About画面では負荷ゼロにする
    }
}
