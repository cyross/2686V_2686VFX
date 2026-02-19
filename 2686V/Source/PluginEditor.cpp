#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "RegisterConverter.h"
#include <cstdio>
#include <vector>
#include <initializer_list>
#include <utility>

// Helper function to get Note Name (Cubase Standard: C3 = 60)
juce::String getNoteName(int noteNumber)
{
    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = (noteNumber / 12) - 2; // MIDI 0 = C-2, MIDI 36 = C1, MIDI 60 = C3
    int noteIndex = noteNumber % 12;
    return juce::String(noteNames[noteIndex]) + juce::String(octave);
}

void layoutComponentsLtoR(juce::Rectangle<int>& rect,
    int height,
    int space,
    std::initializer_list<std::pair<juce::Component*, std::pair<int, int>>> components)
{
    auto area = rect.removeFromTop(height);

    for (const auto& comp : components)
    {
        if (comp.first != nullptr) // 安全のためのチェック
        {
            comp.first->setBounds(area.removeFromLeft(comp.second.first));
            area.removeFromLeft(comp.second.second);
        }
    }

    rect.removeFromTop(space);
}

AudioPlugin2686VEditor::AudioPlugin2686VEditor(AudioPlugin2686V& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

#if !defined(BUILD_AS_FX_PLUGIN)
    wtGui.page.addComponentListener(this);
#endif
    tabs.getTabbedButtonBar().addChangeListener(this);
    audioProcessor.apvts.addParameterListener(codeMode, this);

    setupLogo();

#if !defined(BUILD_AS_FX_PLUGIN)
    // ============================
    // FM Pages
    // ============================
    // OPNA (YM2608)
    setupOpnaGui(opnaGui);

    // OPN (YM2203)
    setupOpnGui(opnGui);

	// OPL (YM3812) : 2op
    setupOplGui(oplGui);

	// OPL3 (YM3833) : 2op
	setupOpl3Gui(opl3Gui);

	// OPM (YM2151) : 4op
	setupOpmGui(opmGui);

    // OPZX3 (CYROSS Original): 4op
    setupOpzx3Gui(opzx3Gui);

    // ============================
    // SSG Page
    // ============================
    setupSsgGui(ssgGui);

    // ============================
    // Wavetable Page
    // ============================
    setupWtGui(wtGui);

    // ============================
    // Rhythm Page
    // ============================
	setupRhythmGui(rhythmGui);

    // ============================
    // ADPCM Page
    // ============================
	setupAdpcmGui(adpcmGui);

    // ============================
    // Preset Page
    // ============================
    setupPresetGui(presetGui);
#endif
    // ============================
    // FX Page
    // ============================
    setupFxGui(fxGui);

    // ============================
    // Settings Page
    // ============================
    setupSettingsGui(settingsGui);

    // ============================
    // About Page
    // ============================
    setupAboutGui(aboutGui);

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
#endif

    setSize(WindowWidth, WindowHeight);
}

AudioPlugin2686VEditor::~AudioPlugin2686VEditor()
{
    tabs.setLookAndFeel(nullptr);

    tabs.getTabbedButtonBar().removeChangeListener(this);
#if !defined(BUILD_AS_FX_PLUGIN)
    wtGui.page.removeComponentListener(this);
    adpcmGui.loadButton.removeListener(this);

    // Remove listeners for rhythm buttons
    for (auto& pad : rhythmGui.pads) {
        pad.loadButton.removeListener(this);
    }
#endif
    audioProcessor.apvts.removeParameterListener(codeMode, this);
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
    }
#endif
}

void AudioPlugin2686VEditor::paint(juce::Graphics& g)
{
	drawBg(g);
}

void AudioPlugin2686VEditor::resized()
{
    logoLabel.setBounds(getLocalBounds().reduced(GlobalPaddingWidth, GlobalPaddingHeight));

    auto area = getLocalBounds();

    tabs.setBounds(area);

    // タブの中身（コンテンツ領域）
    auto content = tabs.getLocalBounds();
    content.removeFromTop(tabs.getTabBarDepth()).reduce(GlobalPaddingWidth, GlobalPaddingHeight); // 全体の余白

#if !defined(BUILD_AS_FX_PLUGIN)
    layoutOpnaPage(opnaGui, content);
    layoutOpnPage(opnGui, content);
    layoutOplPage(oplGui, content);
	layoutOpl3Page(opl3Gui, content);
	layoutOpmPage(opmGui, content);
    layoutOpzx3Page(opzx3Gui, content);
	layoutSsgPage(ssgGui, content);
    layoutWtPage(wtGui, content);
	layoutRhythmPage(rhythmGui, content);
	layoutAdpcmPage(adpcmGui, content);
#endif
    layoutFxPage(fxGui, content);
#if !defined(BUILD_AS_FX_PLUGIN)
    layoutPresetPage(presetGui, content);
#endif
    layoutSettingsPage(settingsGui, content);
    layoutAboutPage(aboutGui, content);
}

void AudioPlugin2686VEditor::drawBg(juce::Graphics& g)
{
    if (backgroundImage.isValid())
    {
        // Draw Image (Scale to fit)
        g.drawImage(backgroundImage, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);

        // Optional: Add a dark overlay to make controls readable
        g.fillAll(juce::Colours::black.withAlpha(0.2f));
    }
    else
    {
        // Default Solid Color
        g.fillAll(juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0f));
    }

    // Reset color for other drawings
    g.setColour(juce::Colours::white);
}

std::vector<SelectItem> AudioPlugin2686VEditor::createItems(int size, const juce::String& prefix)
{
    std::vector<SelectItem> items;

    for (int i = 0; i < size; ++i) items.push_back({ .name = prefix + juce::String(i), .value = i + 1 });

    return items;
}

std::vector<SelectItem> AudioPlugin2686VEditor::createAlgItems(int size)
{
    return createItems(size, "Alg ");
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
    tabs.addTab(fxTabName, juce::Colours::transparentBlack, &fxGui.page, true);
    tabs.addTab(settingsTabName, juce::Colours::transparentBlack, &settingsGui.page, true);
    tabs.addTab(aboutTabName, juce::Colours::transparentBlack, &aboutGui.page, true);
#else
    tabs.addTab(opnaTabName, juce::Colours::transparentBlack, &opnaGui.page, true);
    tabs.addTab(opnTabName, juce::Colours::transparentBlack, &opnGui.page, true);
    tabs.addTab(oplTabName, juce::Colours::transparentBlack, &oplGui.page, true);
    tabs.addTab(opl3TabName, juce::Colours::transparentBlack, &opl3Gui.page, true);
    tabs.addTab(opmTabName, juce::Colours::transparentBlack, &opmGui.page, true);
    tabs.addTab(opzx3TabName, juce::Colours::transparentBlack, &opzx3Gui.page, true);
    tabs.addTab(ssgTabName, juce::Colours::transparentBlack, &ssgGui.page, true);
    tabs.addTab(wtTabName, juce::Colours::transparentBlack, &wtGui.page, true);
    tabs.addTab(rhythmTabName, juce::Colours::transparentBlack, &rhythmGui.page, true);
    tabs.addTab(adpcmTabName, juce::Colours::transparentBlack, &adpcmGui.page, true);
    tabs.addTab(fxTabName, juce::Colours::transparentBlack, &fxGui.page, true);
    tabs.addTab(presetTabName, juce::Colours::transparentBlack, &presetGui.page, true);
    tabs.addTab(settingsTabName, juce::Colours::transparentBlack, &settingsGui.page, true);
    tabs.addTab(aboutTabName, juce::Colours::transparentBlack, &aboutGui.page, true);
#endif
}

void AudioPlugin2686VEditor::setupGroup(SetupGroupParams& params)
{
    params.page.addAndMakeVisible(params.group);
    params.group.setText(params.title);
    params.group.setTextLabelPosition(juce::Justification::centredLeft);

    // 背景色を設定
    if (!params.color.isTransparent())
    {
        params.group.setBackgroundColor(params.color);
    }

    params.group.toBack();
}

void AudioPlugin2686VEditor::setupLabel(SetupLabelParams& params)
{
    params.page.addAndMakeVisible(params.label);
    params.label.setText(params.title, juce::dontSendNotification);
    params.label.setSize(params.width, params.height);
//    params.label.attachToComponent(&params.pagent, false);
//    params.label.setJustificationType(juce::Justification::centred);
}

void AudioPlugin2686VEditor::setupSlider(SetupSliderParams& params)
{
    SetupLabelParams labelParams = SetupLabelParams::create(params.parent, params.slider, params.label, params.title, params.labelWidth, params.labelHeight);
    setupLabel(labelParams);

    params.parent.addAndMakeVisible(params.slider);
    params.slider.setSliderStyle(juce::Slider::LinearHorizontal);
    params.slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, params.valueWidth, params.valueHeight);

    if (params.isReset)
    {
        params.attr.reset(new SliderAttachment(audioProcessor.apvts, params.id, params.slider));
    }

#if !defined(BUILD_AS_FX_PLUGIN)
    if (params.regType != RegisterType::None)
    {
        sliderRegMap[&params.slider] = params.regType;
        params.slider.addMouseListener(this, false);
    }
#endif
}

void AudioPlugin2686VEditor::setupCombo(SetupComboParams& params)

{
    SetupLabelParams labelParams = SetupLabelParams::create(params.parent, params.combo, params.label, params.title, params.labelWidth, params.labelHeight);
    setupLabel(labelParams);

    params.parent.addAndMakeVisible(params.combo);

	for (SelectItem& item : params.items)
    {
        params.combo.addItem(item.name, item.value);
    }

    if (params.isReset)
    {
        params.attr.reset(new ComboBoxAttachment(audioProcessor.apvts, params.id, params.combo));
	}

    if (params.isResize)
    {
        params.combo.onChange = [this] { resized(); };
    }
}

void AudioPlugin2686VEditor::setupToggleButton(SetupToggleButtonParams& params)
{
    params.parent.addAndMakeVisible(params.btn);

    params.btn.setButtonText(params.title);
    params.btn.setSize(params.width, params.height);

    if (params.isReset) {
        params.attr.reset(new ButtonAttachment(audioProcessor.apvts, params.id, params.btn));
    }

    if (params.isResize) {
        params.btn.onClick = [this] { resized(); };
    }
}

void AudioPlugin2686VEditor::setupTextButton(SetupTextButtonParams& params)
{
    params.parent.addAndMakeVisible(params.btn);

    params.btn.setButtonText(params.title);
    params.btn.setSize(params.width, params.height);

    if (params.isReset)
    {
        params.attr.reset(new ButtonAttachment(audioProcessor.apvts, params.id, params.btn));
    }

    if (params.isResize) {
        params.btn.onClick = [this] { resized(); };
    }
}

void AudioPlugin2686VEditor::setupFbSlider(SetupSliderParams& params)

{
    setupSlider(params);
    params.slider.setRange(0.0, 7.0, 1.0);
}

#if !defined(BUILD_AS_FX_PLUGIN)
template <size_t tableSize>
void AudioPlugin2686VEditor::setupCustomWaveTable(SetupCustomWaveTableParams<tableSize>& params)
{
    for (int i = 0; i < tableSize; ++i)
    {
        juce::Slider& slider = params.wts[i];

        // バーグラフ風の見た目にする
        slider.setSliderStyle(juce::Slider::LinearBarVertical);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setRange(-1.0, 1.0);
        slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::white.withAlpha(0.5f));

        auto paramId = params.idPrefix + juce::String(i);
        params.attrs[i].reset(new SliderAttachment(audioProcessor.apvts, paramId, slider));
    }

}

template <size_t opCount>
void AudioPlugin2686VEditor::setupOpGroups(SetupOpGroupsParams<opCount>& params)
{
    for (int i = 0; i < opCount; ++i)
    {
        
        SetupGroupParams groupParams = { .page = params.page, .group = params.groups[i], .title = opGroupPrefix + juce::String(i + 1) };

        setupGroup(groupParams);
    }
}

void AudioPlugin2686VEditor::setupOpnaGui(OpnaGuiSet& gui)
{
    const juce::String code = codeOpna;

    std::vector<SelectItem> pmsItems = createItems(8);
    std::vector<SelectItem> amsItems = createItems(4);
    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> seItems = {
        {.name = "0: Normal", .value = 1 },
        {.name = "1: Saw Down", .value = 2 },
        {.name = "2: Saw Down & Hold", .value = 3 },
        {.name = "3: Triangle", .value = 4 },
        {.name = "4: Alternative Saw Down & Hold", .value = 5 },
        {.name = "5: Saw Up", .value = 6 },
        {.name = "6: Saw Up & Hold", .value = 7 },
        {.name = "7: Triangle Invert", .value = 8 },
        {.name = "8: Alternative Saw Up & Hold", .value = 9 },
    };
    std::vector<SelectItem> ksItems = { {.name = "0 OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    std::vector<SelectItem> bdItems = {
        {.name = "0: 4-bit (16 steps)",  .value = 1 },
        {.name = "1: 5-bit (32 steps)",  .value = 2 },
        {.name = "2: 6-bit (64 steps)",  .value = 3 },
        {.name = "3: 8-bit (256 steps)", .value = 4 },
        {.name = "4: Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    SetupGroupParams mGroupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(mGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, code + postBit, mBitTitle, bdItems);
    setupCombo(bParams);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, code + postRate, mRateTitle, rateItems);
    setupCombo(rtParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, code + postAlg, mAlgTitle, algItems);
    setupCombo(algParams);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, code + postFb0, mFb0Title);
    setupFbSlider(fbParams);
    SetupSliderParams fb2Params = SetupSliderParams::create(gui.page, gui.feedback2Slider, gui.feedback2Label, gui.fbAtt, code + postFb2, mFb2Title);
    setupFbSlider(fb2Params);
    SetupSliderParams fqParams = SetupSliderParams::create(gui.page, gui.lfoFreqSlider, gui.lfoFreqLabel, gui.lfoFreqAtt, code + postLFreq, mLfoFreq);
    setupSlider(fqParams);
    SetupComboParams pmsParams = SetupComboParams::create(gui.page, gui.lfoPmsSelector, gui.lfoPmsLabel, gui.lfoPmsAtt, code + postLPms, mLfoPms, pmsItems);
    setupCombo(pmsParams);
    SetupComboParams amsParams = SetupComboParams::create(gui.page, gui.lfoAmsSelector, gui.lfoAmsLabel, gui.lfoAmsAtt, code + postLAms, mLfoAms, amsItems);
    setupCombo(amsParams);

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    // Operators
	SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = code + codeOp + juce::String(i);

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR:31/RAR:0 DR:0 SL:0 SR:0 RR:15 TL:0 MUL:1 DT:0",
                juce::AlertWindow::QuestionIcon);

            w->addTextEditor("mmlInput", "", ""); // ID, 初期値, プレースホルダー
            w->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
            w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

            w->enterModalState(true, juce::ModalCallbackFunction::create([this, w, &gui, i](int result) {
                if (result == 1) {
                    // 入力文字列を取得して適用
                    juce::String mml = w->getTextEditorContents("mmlInput");
                    applyMmlString(mml, gui, i);
                }
            }), true);
        };

        // Pass RegisterType for sliders
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, opMulLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + postDt, opDtLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, opTlLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + postKs, opKSLabel, ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, opArLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postDr, opDrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postSl, opSlLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + postSr, opSrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSr);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, opRrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupComboParams seParams = SetupComboParams::createOp(gui.page, gui.se[i], gui.seLabel[i], gui.seAtt[i], paramPrefix + postSe, opSEnvLabel, seItems);
        setupCombo(seParams);
        SetupSliderParams seFreqParams = SetupSliderParams::createOp(gui.page, gui.seFreq[i], gui.seFreqLabel[i], gui.seFreqAtt[i], paramPrefix + postSeFreq, opSFreqLabel);
        setupSlider(seFreqParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + postFix, opFixLabel);
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + postFixFreq, opFFreqLabel);
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + postFixFreqTo0, "Freq -> 0Hz", { 64, 15 }, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + postFixFreqTo440, "Freq -> 440Hz", { 64, 15 }, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        SetupToggleButtonParams amParams = SetupToggleButtonParams::createOp(gui.page, gui.am[i], gui.amAtt[i], paramPrefix + postAm, opAmLabel);
        setupToggleButton(amParams);
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, opMaskLabel);
        setupToggleButton(maskParams);
    }
}

void AudioPlugin2686VEditor::setupOpnGui(OpnGuiSet& gui)
{
    const juce::String code = codeOpn;

    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> ksItems = { {.name = "0 OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    std::vector<SelectItem> bdItems = {
        {.name = "0: 4-bit (16 steps)",  .value = 1 },
        {.name = "1: 5-bit (32 steps)",  .value = 2 },
        {.name = "2: 6-bit (64 steps)",  .value = 3 },
        {.name = "3: 8-bit (256 steps)", .value = 4 },
        {.name = "4: Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    SetupGroupParams mGroupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(mGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, code + postBit, mBitTitle, bdItems);
    setupCombo(bParams);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, code + postRate, mRateTitle, rateItems);
    setupCombo(rtParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, code + postAlg, mAlgTitle, algItems);
    setupCombo(algParams);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, code + postFb0, mFb0Title);
    setupFbSlider(fbParams);
    SetupSliderParams fb2Params = SetupSliderParams::create(gui.page, gui.feedback2Slider, gui.feedback2Label, gui.fbAtt, code + postFb2, mFb2Title);
    setupFbSlider(fb2Params);

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    // Operators
    SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = code + codeOp + juce::String(i);

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR:31/RAR:0 DR:0 SR:0 SL:0 RR:15 TL:0 MUL:1 DT:0",
                juce::AlertWindow::QuestionIcon);

            w->addTextEditor("mmlInput", "", ""); // ID, 初期値, プレースホルダー
            w->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
            w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

            w->enterModalState(true, juce::ModalCallbackFunction::create([this, w, &gui, i](int result) {
                if (result == 1) {
                    // 入力文字列を取得して適用
                    juce::String mml = w->getTextEditorContents("mmlInput");
                    applyMmlString(mml, gui, i);
                }
            }), true);
        };



        // Pass RegisterType for sliders
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, opMulLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + postDt, opDtLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, opTlLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + postKs, opKSLabel, ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, opArLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postDr, opDrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postSl, opSlLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + postSr, opSrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSr);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, opRrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + postFix, opFixLabel);
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + postFixFreq, opFFreqLabel);
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + postFixFreqTo0, opFreqTo0Label, { 64, 15 }, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + postFixFreqTo440, opFreqTo440Label, { 64, 15 }, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, opMaskLabel);
        setupToggleButton(maskParams);
    }
}

void AudioPlugin2686VEditor::setupOplGui(OplGuiSet& gui)
{
    const juce::String code = codeOpl;

    std::vector<SelectItem> algItems = { {.name = "0: FM(Serial)", .value = 1}, {.name = "1: AM (Parallel)", .value = 2}, };
    std::vector<SelectItem> kslItems = { {.name = "KSL: 0", .value = 1}, {.name = "KSL: 1", .value = 2}, {.name = "KSL: 2", .value = 3}, {.name = "KSL: 3", .value = 4}, };
    std::vector<SelectItem> egItems = { {.name = "0: Sine", .value = 1}, {.name = "1: Half", .value = 2}, {.name = "2: Abs", .value = 3}, {.name = "3: Pulse", .value = 4}, };

    std::vector<SelectItem> bdItems = {
        {.name = "0: 4-bit (16 steps)",  .value = 1 },
        {.name = "1: 5-bit (32 steps)",  .value = 2 },
        {.name = "2: 6-bit (64 steps)",  .value = 3 },
        {.name = "3: 8-bit (256 steps)", .value = 4 },
        {.name = "4: Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    SetupGroupParams mGroupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(mGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, code + postBit, mBitTitle, bdItems);
    setupCombo(bParams);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, code + postRate, mRateTitle, rateItems);
    setupCombo(rtParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, code + postAlg, mAlgTitle, algItems);
    setupCombo(algParams);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, code + postFb0, mFb0Title);
    setupFbSlider(fbParams);

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    SetupOpGroupsParams<2> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 2; ++i)
    {
        juce::String paramPrefix = code + codeOp + juce::String(i);

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR:31/RAR:0 DR:0 SL:0 RR:15 MUL:1 DT:0",
                juce::AlertWindow::QuestionIcon);

            w->addTextEditor("mmlInput", "", ""); // ID, 初期値, プレースホルダー
            w->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
            w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

            w->enterModalState(true, juce::ModalCallbackFunction::create([this, w, &gui, i](int result) {
                if (result == 1) {
                    // 入力文字列を取得して適用
                    juce::String mml = w->getTextEditorContents("mmlInput");
                    applyMmlString(mml, gui, i);
                }
            }), true);
        };


        // Pass RegisterType for sliders
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, opMulLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + postDt, opDtLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, opTlLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, opArLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postDr, opDrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postSl, opSlLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, opRrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupToggleButtonParams amParams = SetupToggleButtonParams::createOp(gui.page, gui.am[i], gui.amAtt[i], paramPrefix + postAm, opAmLabel);
        setupToggleButton(amParams);
        SetupToggleButtonParams vibParams = SetupToggleButtonParams::createOp(gui.page, gui.vib[i], gui.vibAtt[i], paramPrefix + postVib, opVibLabel);
        setupToggleButton(vibParams);
        SetupToggleButtonParams egtParams = SetupToggleButtonParams::createOp(gui.page, gui.egType[i], gui.egTypeAtt[i], paramPrefix + postEgType, opEgTypeLabel);
        setupToggleButton(egtParams);
        SetupToggleButtonParams ksrParams = SetupToggleButtonParams::createOp(gui.page, gui.ksr[i], gui.ksrAtt[i], paramPrefix + postKsr, opKsrLabel);
        setupToggleButton(ksrParams);
        SetupComboParams kslParams = SetupComboParams::createOp(gui.page, gui.ksl[i], gui.kslLabel[i], gui.kslAtt[i], paramPrefix + postKsl, opKslLabel, kslItems);
        setupCombo(kslParams);
        SetupComboParams egParams = SetupComboParams::createOp(gui.page, gui.eg[i], gui.egLabel[i], gui.egAtt[i], paramPrefix + postEg, opEgLabel, egItems);
        setupCombo(egParams);
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, opMaskLabel);
        setupToggleButton(maskParams);
    }
}

void AudioPlugin2686VEditor::setupOpl3Gui(Opl3GuiSet& gui)
{
    const juce::String code = codeOpl3;

    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> kslItems = { {.name = "KSL: 0", .value = 1}, {.name = "KSL: 1", .value = 2}, {.name = "KSL: 2", .value = 3}, {.name = "KSL: 3", .value = 4}, };
    std::vector<SelectItem> egItems = {
        {.name = "0: Sine", .value = 1},
        {.name = "1: Half Sine", .value = 2},
        {.name = "2: Abs Sine", .value = 3},
        {.name = "3: Pulse", .value = 4},
        {.name = "4: Alternative Sine", .value = 5},
        {.name = "5: Alternative Abs Sine", .value = 6},
        {.name = "6: Square", .value = 7},
        {.name = "7: Derived Square", .value = 8},
    };

    std::vector<SelectItem> bdItems = {
        {.name = "0: 4-bit (16 steps)",  .value = 1 },
        {.name = "1: 5-bit (32 steps)",  .value = 2 },
        {.name = "2: 6-bit (64 steps)",  .value = 3 },
        {.name = "3: 8-bit (256 steps)", .value = 4 },
        {.name = "4: Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    SetupGroupParams mGroupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(mGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, code + postBit, mBitTitle, bdItems);
    setupCombo(bParams);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, code + postRate, mRateTitle, rateItems);
    setupCombo(rtParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, code + postAlg, mAlgTitle, algItems);
    setupCombo(algParams);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, code + postFb0, mFb0Title);
    setupFbSlider(fbParams);
    SetupSliderParams fb2Params = SetupSliderParams::create(gui.page, gui.feedback2Slider, gui.feedback2Label, gui.fbAtt, code + postFb2, mFb2Title);
    setupFbSlider(fb2Params);

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = code + codeOp + juce::String(i);

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR:31/RAR:0 DR:0 SL:0 RR:15 TL:0 MUL:1",
                juce::AlertWindow::QuestionIcon);

            w->addTextEditor("mmlInput", "", ""); // ID, 初期値, プレースホルダー
            w->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
            w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

            w->enterModalState(true, juce::ModalCallbackFunction::create([this, w, &gui, i](int result) {
                if (result == 1) {
                    // 入力文字列を取得して適用
                    juce::String mml = w->getTextEditorContents("mmlInput");
                    applyMmlString(mml, gui, i);
                }
            }), true);
        };


        // Pass RegisterType for sliders
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, opMulLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, opTlLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, opArLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postDr, opDrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postSl, opSlLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, opRrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupToggleButtonParams amParams = SetupToggleButtonParams::createOp(gui.page, gui.am[i], gui.amAtt[i], paramPrefix + postAm, opAmLabel);
        setupToggleButton(amParams);
        SetupToggleButtonParams vibParams = SetupToggleButtonParams::createOp(gui.page, gui.vib[i], gui.vibAtt[i], paramPrefix + postVib, opVibLabel);
        setupToggleButton(vibParams);
        SetupToggleButtonParams egtParams = SetupToggleButtonParams::createOp(gui.page, gui.egType[i], gui.egTypeAtt[i], paramPrefix + postEgType, opEgTypeLabel);
        setupToggleButton(egtParams);
        SetupToggleButtonParams ksrParams = SetupToggleButtonParams::createOp(gui.page, gui.ksr[i], gui.ksrAtt[i], paramPrefix + postKsr, opKsrLabel);
        setupToggleButton(ksrParams);
        SetupComboParams kslParams = SetupComboParams::createOp(gui.page, gui.ksl[i], gui.kslLabel[i], gui.kslAtt[i], paramPrefix + postKsl, opKslLabel, kslItems);
        setupCombo(kslParams);
        SetupComboParams egParams = SetupComboParams::createOp(gui.page, gui.eg[i], gui.egLabel[i], gui.egAtt[i], paramPrefix + postEg, opEgLabel, egItems);
        setupCombo(egParams);
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, opMaskLabel);
        setupToggleButton(maskParams);
    }
}

void AudioPlugin2686VEditor::setupOpmGui(OpmGuiSet& gui)
{
    const juce::String code = codeOpm;

    std::vector<SelectItem> pmsItems = createItems(8);
    std::vector<SelectItem> amsItems = createItems(4);
    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> ksItems = { {.name = "0 OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    std::vector<SelectItem> bdItems = {
        {.name = "0: 4-bit (16 steps)",  .value = 1 },
        {.name = "1: 5-bit (32 steps)",  .value = 2 },
        {.name = "2: 6-bit (64 steps)",  .value = 3 },
        {.name = "3: 8-bit (256 steps)", .value = 4 },
        {.name = "4: Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    SetupGroupParams mGroupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(mGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, code + postBit, mBitTitle, bdItems);
    setupCombo(bParams);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, code + postRate, mRateTitle, rateItems);
    setupCombo(rtParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, code + postAlg, mAlgTitle, algItems);
    setupCombo(algParams);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, code + postFb0, mFb0Title);
    setupFbSlider(fbParams);
    SetupSliderParams fb2Params = SetupSliderParams::create(gui.page, gui.feedback2Slider, gui.feedback2Label, gui.fbAtt, code + postFb2, mFb2Title);
    setupFbSlider(fb2Params);
    SetupSliderParams fqParams = SetupSliderParams::create(gui.page, gui.lfoFreqSlider, gui.lfoFreqLabel, gui.lfoFreqAtt, code + postLFreq, mLfoFreq);
    setupSlider(fqParams);
    SetupComboParams pmsParams = SetupComboParams::create(gui.page, gui.pmsSelector, gui.pmsLabel, gui.pmsAtt, code + postLPms, mLfoPms, pmsItems);
    setupCombo(pmsParams);
    SetupComboParams amsParams = SetupComboParams::create(gui.page, gui.amsSelector, gui.amsLabel, gui.amsAtt, code + postLAms, mLfoAms, amsItems);
    setupCombo(amsParams);

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    // Operators
    SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = code + codeOp + juce::String(i);

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR:31/RAR:0 D1R:0 D1L:0 D2R:0 RR:15 TL:0 MUL:1 DT:10 DT:20",
                juce::AlertWindow::QuestionIcon);

            w->addTextEditor("mmlInput", "", ""); // ID, 初期値, プレースホルダー
            w->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
            w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

            w->enterModalState(true, juce::ModalCallbackFunction::create([this, w, &gui, i](int result) {
                if (result == 1) {
                    // 入力文字列を取得して適用
                    juce::String mml = w->getTextEditorContents("mmlInput");
                    applyMmlString(mml, gui, i);
                }
            }), true);
        };



        // Pass RegisterType for sliders
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, opMulLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt1[i], gui.dt1Label[i], gui.dt1Att[i], paramPrefix + postDt, opDt1Label, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams dt2Params = SetupSliderParams::createOp(gui.page, gui.dt2[i], gui.dt2Label[i], gui.dt2Att[i], paramPrefix + postDt2, opDt2Label, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt2);
        setupSlider(dt2Params);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, opTlLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + postKs, opKSLabel, ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, opArLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postD1r, opD1rLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postD1l, opD1lLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + postD2r, opD2rLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSr);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, opRrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + postFix, opFixLabel);
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + postFixFreq, opFFreqLabel);
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + postFixFreqTo0, opFreqTo0Label, { 64, 15 }, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + postFixFreqTo440, opFreqTo440Label, { 64, 15 }, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, opMaskLabel);
        setupToggleButton(maskParams);
    }
}

void AudioPlugin2686VEditor::setupOpzx3Gui(Opzx3GuiSet& gui)
{
    const juce::String code = codeOpzx3;

    std::vector<SelectItem> pmsItems = createItems(8);
    std::vector<SelectItem> amsItems = createItems(4);
    std::vector<SelectItem> algItems = createAlgItems(27);
    std::vector<SelectItem> ksItems = { {.name = "0 OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };
    std::vector<SelectItem> bdItems = {
        {.name = "0: 4-bit (16 steps)",  .value = 1 },
        {.name = "1: 5-bit (32 steps)",  .value = 2 },
        {.name = "2: 6-bit (64 steps)",  .value = 3 },
        {.name = "3: 8-bit (256 steps)", .value = 4 },
        {.name = "4: Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };
    std::vector<SelectItem> wsItems = {
        {.name = "00: Sine", .value = 1},
        {.name = "01: Half Sine", .value = 2},
        {.name = "02: Abs Sine", .value = 3},
        {.name = "03: Alternating Half Sine", .value = 4},
        {.name = "04: Alternating Sine", .value = 5},
        {.name = "05: Alternating Abs Sine", .value = 6},
        {.name = "06: Square", .value = 7},
        {.name = "07: Derived Square", .value = 8},
        {.name = "08: Saw Down", .value = 9},
        {.name = "09: Saw Up", .value = 10},
        {.name = "10: Triangle", .value = 11},
        {.name = "11: Saw + Sine", .value = 12},
        {.name = "12: Log Saw", .value = 13},
        {.name = "13: Pulse 25%", .value = 14},
        {.name = "14: Pulse 12.5%", .value = 15},
        {.name = "15: Pulse 6.25%", .value = 16},
        {.name = "16: Round Square", .value = 17},
        {.name = "17: Impulse Train", .value = 18},
        {.name = "18: Comb / Multi-pulse", .value = 19},
        {.name = "19: Resonant Saw (Low)", .value = 20},
        {.name = "20: Resonant Saw (High)", .value = 21},
        {.name = "21: Resonant Triangle", .value = 22},
        {.name = "22: Bulb Sine", .value = 23},
        {.name = "23: Double Hump", .value = 24},
        {.name = "24: Pseudo Voice Formant 1", .value = 25},
        {.name = "25: Pseudo Voice Formant 2", .value = 26},
        {.name = "26: Metallic 1", .value = 27},
        {.name = "27: Metallic 2", .value = 28},
        {.name = "28: Noise-Like", .value = 29},
    };

    SetupGroupParams mGroupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(mGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, code + postBit, mBitTitle, bdItems);
    setupCombo(bParams);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, code + postRate, mRateTitle, rateItems);
    setupCombo(rtParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, code + postAlg, mAlgTitle, algItems);
    setupCombo(algParams);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, code + postFb0, mFb0Title);
    setupFbSlider(fbParams);
    SetupSliderParams fb2Params = SetupSliderParams::create(gui.page, gui.feedback2Slider, gui.feedback2Label, gui.fbAtt, code + postFb2, mFb2Title);
    setupFbSlider(fb2Params);
    SetupSliderParams fqParams = SetupSliderParams::create(gui.page, gui.lfoFreqSlider, gui.lfoFreqLabel, gui.lfoFreqAtt, code + postLFreq, mLfoFreq);
    setupSlider(fqParams);
    SetupComboParams pmsParams = SetupComboParams::create(gui.page, gui.pmsSelector, gui.pmsLabel, gui.pmsAtt, code + postLPms, mLfoPms, pmsItems);
    setupCombo(pmsParams);
    SetupComboParams amsParams = SetupComboParams::create(gui.page, gui.amsSelector, gui.amsLabel, gui.amsAtt, code + postLAms, mLfoAms, amsItems);
    setupCombo(amsParams);

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    // Operators
    SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = code + codeOp + juce::String(i);

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR:31/RAR:0 D1R:0 D1L:0 D2R:0 RR:15 TL:0 MUL:1 DT1:0 DT2:0",
                juce::AlertWindow::QuestionIcon);

            w->addTextEditor("mmlInput", "", ""); // ID, 初期値, プレースホルダー
            w->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
            w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

            w->enterModalState(true, juce::ModalCallbackFunction::create([this, w, &gui, i](int result) {
                if (result == 1) {
                    // 入力文字列を取得して適用
                    juce::String mml = w->getTextEditorContents("mmlInput");
                    applyMmlString(mml, gui, i);
                }
                }), true);
            };

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, opMulLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt1[i], gui.dt1Label[i], gui.dt1Att[i], paramPrefix + postDt, opDt1Label, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams dt2Params = SetupSliderParams::createOp(gui.page, gui.dt2[i], gui.dt2Label[i], gui.dt2Att[i], paramPrefix + postDt2, opDt2Label, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt2);
        setupSlider(dt2Params);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, opTlLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + postKs, opKSLabel, ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, opArLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postD1r, opD1rLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postD1l, opD1lLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + postD2r, opD2rLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSr);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, opRrLabel, OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + postFix, opFixLabel);
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + postFixFreq, opFFreqLabel);
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + postFixFreqTo0, opFreqTo0Label, { 64, 15 }, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + postFixFreqTo440, opFreqTo440Label, { 64, 15 }, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        SetupComboParams wsParams = SetupComboParams::createOp(gui.page, gui.ws[i], gui.wsLabel[i], gui.wsAtt[i], paramPrefix + postWs, opWsLabel, wsItems);
        setupCombo(wsParams);
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, opMaskLabel);
        setupToggleButton(maskParams);
    }
}

void AudioPlugin2686VEditor::setupSsgGui(SsgGuiSet& gui)
{
    const juce::String code = codeSsg;

    std::vector<SelectItem> wsItems = {
        { .name = "0: Pulse(Rect)", .value = 1 },
        { .name = "1: Triangle / Saw", .value = 2 },
    };

    std::vector<SelectItem> dmItems = {
        {.name = "0: Preset Ratio", .value = 1 },
        {.name = "1: Variable (Slider)", .value = 2 },
    };

    std::vector<SelectItem> prItems = {
        {.name = "0: 1:1 (50%)", .value = 1 },
        {.name = "1: 3:5 (37.5%)", .value = 2 },
        {.name = "2: 5:11 (31.25%)", .value = 3 },
        {.name = "3: 1:3 (25%)", .value = 4 },
        {.name = "4: 1:4 (20%)", .value = 5 },
        {.name = "5: 3:13 (18.75%)", .value = 6 },
        {.name = "6: 1:7 (12.5%)", .value = 7 },
        {.name = "7: 1:15 (6.25%)", .value = 8 },
    };

    std::vector<SelectItem> envItems = {
        {.name = "0: Saw Down", .value = 1 },
        {.name = "1: Saw Down & Hold", .value = 2 },
        {.name = "2: Triangle", .value = 3 },
        {.name = "3: Alternative Saw Down & Hold", .value = 4 },
        {.name = "4: Saw Up", .value = 5 },
        {.name = "5: Saw Up & Hold", .value = 6 },
        {.name = "6: Triangle Invert", .value = 7 },
        {.name = "7: Alternative Saw Up & Hold", .value = 8 },
    };

    std::vector<SelectItem> bdItems = {
        {.name = "0: 4-bit (16 steps)",  .value = 1 },
        {.name = "1: 5-bit (32 steps)",  .value = 2 },
        {.name = "2: 6-bit (64 steps)",  .value = 3 },
        {.name = "3: 8-bit (256 steps)", .value = 4 },
        {.name = "4: Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    SetupGroupParams mGroupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(mGroupParams);

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, code + postBit, mBitTitle, bdItems);
    setupCombo(bParams);

    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, code + postRate, mRateTitle, rateItems);
    setupCombo(rtParams);

    SetupGroupParams vGroupParams = { .page = gui.page, .group = gui.voiceGroup, .title = "Voice" };
    setupGroup(vGroupParams);

    SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.waveSelector, gui.waveLabel, gui.waveAtt, code + postWaveform, "Form", wsItems, ComboBoxSize, LabelSize, {true, true});
    setupCombo(wsParams);

    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, code + postLevel, "Tone", SliderSize, SliderValueSize, LabelSize, SliderFlags, RegisterType::SsgVol);
    setupSlider(lvParams);
    SetupSliderParams nsParams = SetupSliderParams::create(gui.page, gui.noiseSlider, gui.noiseLabel, gui.noiseAtt, code + postNoise, "Noise", SliderSize, SliderValueSize, LabelSize, SliderFlags, RegisterType::SsgVol);
    setupSlider(nsParams);
    SetupSliderParams nfParams = SetupSliderParams::create(gui.page, gui.noiseFreqSlider, gui.noiseFreqLabel, gui.noiseFreqAtt, code + postNoiseFreq, "Freq");
    setupSlider(nfParams);
    SetupToggleButtonParams nonParams = SetupToggleButtonParams::create(gui.page, gui.noiseOnNoteButton, gui.noiseOnNoteAtt, code + postNoiseOnNote, "Noise On Note", { 120, 20 }, { true, true });
    setupToggleButton(nonParams);

    // 初期状態反映
    SetupSliderParams mxParams = SetupSliderParams::create(gui.page, gui.mixSlider, gui.mixLabel, gui.mixAtt, code + postMix, "Mix");
    setupSlider(mxParams);

    SetupTextButtonParams mstParams = SetupTextButtonParams::create(gui.page, gui.mixSetTone, gui.mixSetAtt, "Tone", "Tone", { 40, 20 }, { false, false });
    setupTextButton(mstParams);
    gui.mixSetTone.onClick = [this] { ssgGui.mixSlider.setValue(0.0, juce::sendNotification); };
    SetupTextButtonParams msmParams = SetupTextButtonParams::create(gui.page, gui.mixSetMix, gui.mixSetAtt, "Mix", "Mix", { 40, 20 }, { false, false });
    setupTextButton(msmParams);
    gui.mixSetMix.onClick = [this] { ssgGui.mixSlider.setValue(0.5, juce::sendNotification); };
    SetupTextButtonParams msnParams = SetupTextButtonParams::create(gui.page, gui.mixSetNoise, gui.mixSetAtt, "Noise", "Noise", { 40, 20 }, { false, false });
    setupTextButton(msnParams);
    gui.mixSetNoise.onClick = [this] { ssgGui.mixSlider.setValue(1.0, juce::sendNotification); };
    SetupToggleButtonParams bpParams = SetupToggleButtonParams::create(gui.page, gui.adsrBypassButton, gui.adsrBypassAtt, code + codeAdsr + postBypass, "Bypass ADSR", { 160, 20 }, { true, true });
    setupToggleButton(bpParams);
    SetupSliderParams arParams = SetupSliderParams::create(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, code + postAr, mArLabel);
    setupSlider(arParams);
    SetupSliderParams drParams = SetupSliderParams::create(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, code + postDr, mDrLabel);
    setupSlider(drParams);
    SetupSliderParams slParams = SetupSliderParams::create(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, code + postSl, mSlLabel);
    setupSlider(slParams);
    SetupSliderParams rrParams = SetupSliderParams::create(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, code + postRr, mRrLabel);
    setupSlider(rrParams);

    // Duty Controls Setup
    SetupGroupParams dGroupParams = { .page = gui.page, .group = gui.dutyGroup, .title = "Pulse Width (Duty)" };
    setupGroup(dGroupParams);
    SetupComboParams dmParams = SetupComboParams::create(gui.page, gui.dutyModeSelector, gui.dutyModeLabel, gui.dutyModeAtt, code + postDutyMode, "Mode", dmItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(dmParams);
    SetupComboParams dpParams = SetupComboParams::create(gui.page, gui.dutyPresetSelector, gui.dutyPresetLabel, gui.dutyPresetAtt, code + postDutyPreset, "Preset", prItems);
    setupCombo(dpParams);
    SetupSliderParams dvParams = SetupSliderParams::create(gui.page, gui.dutyVarSlider, gui.dutyVarLabel, gui.dutyVarAtt, code + postDutyVar, "Ratio");
    setupSlider(dvParams);
    SetupToggleButtonParams dtiParams = SetupToggleButtonParams::create(gui.page, gui.dutyInvertButton, gui.dutyInvertAtt, code + postDutyInv, "Invert Phase", { 160, 20 });
    setupToggleButton(dtiParams);
    SetupGroupParams tpGroupParams = { .page = gui.page, .group = gui.triGroup, .title = "Triangle Property" };
    setupGroup(tpGroupParams);
    SetupToggleButtonParams tktParams = SetupToggleButtonParams::create(gui.page, gui.triKeyTrackButton, gui.triKeyTrackAtt, code + postTriKeyTrk, "Key Track (Pitch)", ToggleButtonSize, { true, true });
    setupToggleButton(tktParams);
    SetupSliderParams tfParams = SetupSliderParams::create(gui.page, gui.triFreqSlider, gui.triFreqLabel, gui.triFreqAtt, code + postTriFreq, "Freq");
    setupSlider(tfParams);
    SetupSliderParams tpParams = SetupSliderParams::create(gui.page, gui.triPeakSlider, gui.triPeakLabel, gui.triPeakAtt, code + postTriPeak, "Peak");
    setupSlider(tpParams);

    SetupTextButtonParams tsdParams = SetupTextButtonParams::create(gui.page, gui.triSetSawDown, gui.triKeyTrackAtt, "0.0 (Down)", "0.0 (Down)", TextButtonSize, { false, false });
    setupTextButton(tsdParams);
    gui.triSetSawDown.onClick = [this] { ssgGui.triPeakSlider.setValue(0.0, juce::sendNotification); };
    SetupTextButtonParams tstParams = SetupTextButtonParams::create(gui.page, gui.triSetTri, gui.triKeyTrackAtt, "0.5 (Tri)0", "0.5 (Tri)", TextButtonSize, { false, false });
    setupTextButton(tstParams);
    gui.triSetTri.onClick = [this] { ssgGui.triPeakSlider.setValue(0.5, juce::sendNotification); };
    SetupTextButtonParams tsuParams = SetupTextButtonParams::create(gui.page, gui.triSetSawUp, gui.triKeyTrackAtt, "1.0 (Up)", "1.0 (Up)", TextButtonSize, { false, false });
    setupTextButton(tsuParams);
    gui.triSetSawUp.onClick = [this] { ssgGui.triPeakSlider.setValue(1.0, juce::sendNotification); };

    // HW Env Group
    SetupGroupParams groupParams = { .page = gui.page, .group = gui.envGroup, .title = "Hardware Envelope" };
    setupGroup(groupParams);
    SetupToggleButtonParams eeParams = SetupToggleButtonParams::create(gui.page, gui.envEnableButton, gui.envEnableAtt, code + postEnvEnable, "Enable HW Env", { 160, 20 }, { true, true });
    setupToggleButton(eeParams);
    SetupComboParams evParams = SetupComboParams::create(gui.page, gui.shapeSelector, gui.shapeLabel, gui.shapeAtt, code + postEnvShape, "Shape", envItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(evParams);
    SetupSliderParams epParams = SetupSliderParams::create(gui.page, gui.periodSlider, gui.periodLabel, gui.periodAtt, code + postEnvPeriod, "Speed", SliderSize, SliderValueSize, LabelSize, SliderFlags, RegisterType::SsgEnv);
    setupSlider(epParams);
}

void AudioPlugin2686VEditor::setupWtGui(WtGuiSet& gui)
{
    const juce::String code = codeWt;

    std::vector<SelectItem> wsItems = {
        {.name = "0: Sine",          .value = 1 },
        {.name = "1: Triangle",      .value = 2 },
        {.name = "2: Saw Up",        .value = 3 },
        {.name = "3: Saw Down",      .value = 4 },
        {.name = "4: Square (50%)",  .value = 5 },
        {.name = "5: Pulse (25%)",   .value = 6 },
        {.name = "6: Pulse (12.5%)", .value = 7 },
        {.name = "7: Digital Noise", .value = 8 },
        {.name = "8: Custom(Draw)",  .value = 9 },
    };

    std::vector<SelectItem> tsItems = {
        {.name = "0: 32 Samples",  .value = 1 },
        {.name = "1: 64 Samples",  .value = 2 },
    };

    std::vector<SelectItem> bdItems = {
        {.name = "0: 4-bit (16 steps)",  .value = 1 },
        {.name = "1: 5-bit (32 steps)",  .value = 2 },
        {.name = "2: 6-bit (64 steps)",  .value = 3 },
        {.name = "3: 8-bit (256 steps)", .value = 4 },
        {.name = "4: Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    SetupGroupParams mGroupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(mGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, code + postBit, mBitTitle, bdItems);
    setupCombo(bParams);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, code + postRate, mRateTitle, rateItems);
    setupCombo(rtParams);
    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, code + postLevel, "Level");
    setupSlider(lvParams);
    SetupSliderParams arParams = SetupSliderParams::create(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, code + postAr, mArLabel);
    setupSlider(arParams);
    SetupSliderParams drParams = SetupSliderParams::create(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, code + postDr, mDrLabel);
    setupSlider(drParams);
    SetupSliderParams slParams = SetupSliderParams::create(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, code + postSl, mSlLabel);
    setupSlider(slParams);
    SetupSliderParams rrParams = SetupSliderParams::create(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, code + postRr, mRrLabel);
    setupSlider(rrParams);

    // Waveform
    SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.waveSelector, gui.waveLabel, gui.waveAtt, code + postWave, "Form", wsItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(wsParams);

    // Modulation
    SetupToggleButtonParams meParams = SetupToggleButtonParams::create(gui.page, gui.modEnableButton, gui.modEnableAtt, code + postModEnable, "Mod", ToggleButtonSize, { true, true });
    setupToggleButton(meParams);
    SetupSliderParams mdParams = SetupSliderParams::create(gui.page, gui.modDepthSlider, gui.modDepthLabel, gui.modDepthAtt, code + postModDepth, "Depth");
    setupSlider(mdParams);
    SetupSliderParams msParams = SetupSliderParams::create(gui.page, gui.modSpeedSlider, gui.modSpeedLabel, gui.modSpeedAtt, code + postModSpeed, "Speed");
    setupSlider(msParams);

    // Master Volume
    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    // Custom Wave Group
    SetupGroupParams cwGroupParams = { .page = gui.page, .group = gui.customWaveGroup, .title = "Custom Wave" };
    setupGroup(cwGroupParams);

    // Custom Wave Size
    SetupComboParams szParams = SetupComboParams::create(gui.page, gui.sizeSelector, gui.sizeLabel, gui.sizeAtt, code + postSize, "Wave Size", tsItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(szParams);

    gui.page.addAndMakeVisible(gui.waveContainer);

    // スライダーのセットアップはそのまま (APVTSへのアタッチが必要なため)
    // ただし、addAndMakeVisible(slider) は WaveformContainer::setTargetSliders 内で行われるため、
    // ここでの addAndMakeVisible は削除するか、containerに任せる形になります。
    // setupCustomWaveTable 内で params.page.addAndMakeVisible(slider) していますが、
    // 親が container に変わるので、そこだけ注意が必要です。

    // 既存のヘルパー setupCustomWaveTable は「ページに貼り付ける」処理を含んでいるため、
    // その部分だけ副作用が出ないように修正するか、
    // あるいは setupCustomWaveTable 実行後に container に移し替える処理を入れます。

    // Custom Wave Sliders
	SetupCustomWaveTableParams cw32Params = { gui.page, gui.customSliders32, gui.customAtts32, code + codeCustom32 };
    setupCustomWaveTable(cw32Params);
    SetupCustomWaveTableParams cw64Params = { gui.page, gui.customSliders64, gui.customAtts64, code + codeCustom64 };
    setupCustomWaveTable(cw64Params);

    SetupTextButtonParams cwrParams = SetupTextButtonParams::create(gui.page, gui.customWaveResetBtn, gui.customWaveResetAtt, "Reset", "Reset", TextButtonSize, { false, false });
    setupTextButton(cwrParams);
    gui.customWaveResetBtn.onClick = [this] { 
        for (auto& s : wtGui.customSliders32) {
            s.setValue(0.0, juce::sendNotification);
        }
        for (auto& s : wtGui.customSliders64) {
            s.setValue(0.0, juce::sendNotification);
        }
        resized();
    };
}

void AudioPlugin2686VEditor::setupRhythmGui(RhythmGuiSet& gui)
{
    const juce::String code = codeRhythm;

    // Prepare Items for ComboBoxes
    std::vector<SelectItem> qualityItems = {
        {.name = "0: Raw (32bit)", .value = 1 },
        {.name = "1: 24-bit PCM",  .value = 2 },
        {.name = "2: 16-bit PCM",  .value = 3 },
        {.name = "3: 8-bit PCM",   .value = 4 },
        {.name = "4: 5-bit PCM",   .value = 5 },
        {.name = "5: 4-bit PCM",   .value = 6 },
        {.name = "6: 4-bit ADPCM", .value = 7 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    // パッド名定義
    const char* padNames[] = { "BD", "SD", "HH Cl", "HH Op", "Tom L", "Tom H", "Crash", "Ride" };

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(groupParams);

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, code + postLevel, "Vol");
    setupSlider(lvParams);

    // Setup 8 Pads
    for (int i = 0; i < 8; ++i)
    {
        // パッドタイトル・オートメーションキー
        auto& pad = gui.pads[i];
        juce::String padPrefix = code + codePad + juce::String(i);
        juce::String padTitle = "Pad " + juce::String(i + 1) + " (" + padNames[i] + ")";

        // メイングループ
        SetupGroupParams groupParams = { .page = gui.page, .group = pad.group, .title = padTitle };
        setupGroup(groupParams);

        // 音声ファイルロードボタン
        gui.page.addAndMakeVisible(pad.loadButton);
        pad.loadButton.setButtonText("Load");
        pad.loadButton.addListener(this);

        // ロードしている音声ファイル名
        gui.page.addAndMakeVisible(pad.fileNameLabel);
        pad.fileNameLabel.setText("(Empty)", juce::dontSendNotification);
        pad.fileNameLabel.setJustificationType(juce::Justification::centred);

        // パッド音声アンロード
        gui.page.addAndMakeVisible(pad.clearButton);
        pad.clearButton.setButtonText("X");
        pad.clearButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred.withAlpha(0.7f));
        pad.clearButton.onClick = [this, i, &pad]
            {
                // 1. 特定のパッドをアンロード
                audioProcessor.unloadRhythmFile(i);

                // 2. ファイル名表示を更新
                pad.fileNameLabel.setText("(Empty)", juce::dontSendNotification);
            };

        // ワンショット機能トグル
        SetupToggleButtonParams shotP = SetupToggleButtonParams::createOp(gui.page, pad.oneShotButton, pad.oneShotAtt, padPrefix + postOneShot, "One Shot");
        setupToggleButton(shotP);

        // 割り当てキーノート番号
        SetupSliderParams noteP = SetupSliderParams::createOp(gui.page, pad.noteSlider, pad.noteLabel, pad.noteAtt, padPrefix + postNote, "Note");
        setupSlider(noteP);
        pad.noteSlider.setRange(0, 127, 1);
        pad.noteSlider.textFromValueFunction = [](double value) {
            return getNoteName((int)value);
        };
        pad.noteSlider.updateText();

        // 内部ビット深度
        SetupComboParams qualP = SetupComboParams::createOp(gui.page, pad.modeCombo, pad.modeLabel, pad.modeAtt, padPrefix + postMode, "Mode", qualityItems);
        setupCombo(qualP);

        // 内部サンプリングレート
        SetupComboParams rateP = SetupComboParams::createOp(gui.page, pad.rateCombo, pad.rateLabel, pad.rateAtt, padPrefix + postRate, mRateTitle, rateItems);
        setupCombo(rateP);

        // パンポット
        SetupSliderParams panP = SetupSliderParams::createOp(gui.page, pad.panSlider, pad.panLabel, pad.panAtt, padPrefix + postPan, "Pan");
        setupSlider(panP);
        pad.panSlider.setRange(0.0f, 1.0f);

        gui.page.addAndMakeVisible(pad.btnPanL);
        pad.btnPanL.setButtonText("L");
        pad.btnPanL.addListener(this);

        gui.page.addAndMakeVisible(pad.btnPanC);
        pad.btnPanC.setButtonText("C");
        pad.btnPanC.addListener(this);

        gui.page.addAndMakeVisible(pad.btnPanR);
        pad.btnPanR.setButtonText("R");
        pad.btnPanR.addListener(this);

        // Vol
        SetupSliderParams volP = SetupSliderParams::createOp(gui.page, pad.volSlider, pad.volLabel, pad.volAtt, padPrefix + postVolume, "Vol");
        setupSlider(volP);

        // RR
        SetupSliderParams rrP = SetupSliderParams::createOp(gui.page, pad.rrSlider, pad.rrLabel, pad.rrAtt, padPrefix + postRr, opRrLabel);
        setupSlider(rrP);
    }

    // ファイル名エリアに反映
    updateRhythmFileNames();
}

void AudioPlugin2686VEditor::setupAdpcmGui(AdpcmGuiSet& gui)
{
    const juce::String code = codeAdpcm;

    // Prepare Items for ComboBoxes
    std::vector<SelectItem> qualityItems = {
        {.name = "0: Raw (32bit)", .value = 1 },
        {.name = "1: 24-bit PCM",  .value = 2 },
        {.name = "2: 16-bit PCM",  .value = 3 },
        {.name = "3: 8-bit PCM",   .value = 4 },
        {.name = "4: 5-bit PCM",   .value = 5 },
        {.name = "5: 4-bit PCM",   .value = 6 },
        {.name = "6: 4-bit ADPCM", .value = 7 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    SetupGroupParams mGroupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(mGroupParams);

    SetupComboParams modeParams = SetupComboParams::create(gui.page, gui.modeCombo, gui.modeLabel, gui.modeAtt, code + postMode, "Quality", qualityItems);
    setupCombo(modeParams);
    SetupComboParams rateParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, code + postRate, mRateTitle, rateItems);
    setupCombo(rateParams);

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    // 出力レベル
    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, code + postLevel, "Vol");
    setupSlider(lvParams);

    // パンポット設定
    SetupSliderParams panParams = SetupSliderParams::create(gui.page, gui.panSlider, gui.panLabel, gui.panAtt, code + postPan, "Pan");
    setupSlider(panParams);
    gui.panSlider.setRange(0.0f, 1.0f);
    gui.page.addAndMakeVisible(gui.btnPanL); gui.btnPanL.setButtonText("L"); gui.btnPanL.addListener(this);
    gui.page.addAndMakeVisible(gui.btnPanC); gui.btnPanC.setButtonText("C"); gui.btnPanC.addListener(this);
    gui.page.addAndMakeVisible(gui.btnPanR); gui.btnPanR.setButtonText("R"); gui.btnPanR.addListener(this);

    // ループトグルボタン
    SetupToggleButtonParams lpParams = SetupToggleButtonParams::create(gui.page, gui.loopButton, gui.loopAtt, code + postLoop, "Loop");
    setupToggleButton(lpParams);

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.group, .title = "ADPCM Sampler" };
    setupGroup(groupParams);

    // 音声ファイル読み込みボタン
    gui.page.addAndMakeVisible(gui.loadButton);
    gui.loadButton.setButtonText("Load File");
    gui.loadButton.addListener(this);

    // ロードしているファイル名
    gui.page.addAndMakeVisible(gui.fileNameLabel);
    gui.fileNameLabel.setText("No File Loaded", juce::dontSendNotification);
    gui.fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    gui.fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    // 音声ファイルのアンロード
    gui.group.addAndMakeVisible(gui.clearButton);
    gui.clearButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred.withAlpha(0.7f));
    gui.clearButton.setButtonText("Clear");
    gui.clearButton.onClick = [this, &gui]
        {
            // 1. プロセッサーのアンロード関数を呼ぶ
            audioProcessor.unloadAdpcmFile();

            // 2. ラベル表示をクリア
            gui.fileNameLabel.setText("No File", juce::dontSendNotification);
        };

    // フィルターAR
    SetupSliderParams arParams = SetupSliderParams::create(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, code + postAr, mArLabel);
    setupSlider(arParams);

    // フィルターDR
    SetupSliderParams drParams = SetupSliderParams::create(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, code + postDr, mDrLabel);
    setupSlider(drParams);

    // フィルターSL
    SetupSliderParams slParams = SetupSliderParams::create(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, code + postSl, mSlLabel);
    setupSlider(slParams);

    // フィルターRR
    SetupSliderParams rrParams = SetupSliderParams::create(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, code + postRr, mRrLabel);
    setupSlider(rrParams);
}


void AudioPlugin2686VEditor::setupPresetGui(PresetGuiSet& gui)
{
    /********************
    *
    * 1. Folder
    *
    *********************/

    // defaultPresetDirから取ってくる
    auto defaultPath = audioProcessor.getDefaultPresetDir();
    gui.currentFolder = juce::File(defaultPath);
    if (!gui.currentFolder.exists()) {
        gui.currentFolder.createDirectory();
    }

    /********************
    *
    * 2. Path Label
    *
    *********************/

    gui.page.addAndMakeVisible(gui.pathLabel);
    gui.pathLabel.setText(gui.currentFolder.getFullPathName(), juce::dontSendNotification);
    gui.pathLabel.setColour(juce::Label::outlineColourId, juce::Colours::grey);
    gui.pathLabel.setJustificationType(juce::Justification::centredLeft);

    /********************
    *
    * 3. Preset Table
    *
    *********************/

    gui.table.getHeader().addColumn(presetTableFileNameColTitle, 1, PresetTableFileNameColTitleWidth);
    gui.table.getHeader().addColumn(presetTablePresetNameColTitle, 2, PresetTablePresetNameColTitleWidth);
    gui.table.getHeader().addColumn(presetTableAuthorColTitle, 3, PresetTableAuthorColTitleWidth);
    gui.table.getHeader().addColumn(presetTableVersionColTitle, 4, PresetTableVersionColTitleWidth);
    gui.table.getHeader().addColumn(presetTableModeColTitle, 5, PresetTableModeColTitleWidth);
    gui.table.setMultipleSelectionEnabled(false);

    gui.page.addAndMakeVisible(gui.table);

    // ダブルクリック時の動作
    gui.onDoubleClicked = [this](const juce::File& file) {
        loadPresetFile(file); // ロード処理を分離
        };

    // 初期スキャン
    scanPresets();

    /********************
    *
    * 4. Metadata Group
    *
    *********************/

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.metaGroup, .title = "Preset Info" };
    setupGroup(groupParams);

    // Name
    gui.page.addAndMakeVisible(gui.nameLabel);
    gui.nameLabel.setText(presetCbName, juce::dontSendNotification);
    gui.page.addAndMakeVisible(gui.nameEditor);
    gui.nameEditor.setText(audioProcessor.presetName);
    gui.nameEditor.onTextChange = [this] { audioProcessor.presetName = presetGui.nameEditor.getText(); };

    // Author
    gui.page.addAndMakeVisible(gui.authorLabel);
    gui.authorLabel.setText(presetCbAuther, juce::dontSendNotification);
    gui.page.addAndMakeVisible(gui.authorEditor);
    gui.authorEditor.setText(audioProcessor.presetAuthor);
    gui.authorEditor.onTextChange = [this] { audioProcessor.presetAuthor = presetGui.authorEditor.getText(); };

    // Version
    gui.page.addAndMakeVisible(gui.versionLabel);
    gui.versionLabel.setText(presetCbVersion, juce::dontSendNotification);
    gui.page.addAndMakeVisible(gui.versionEditor);
    gui.versionEditor.setText(audioProcessor.presetVersion);
    gui.versionEditor.onTextChange = [this] { audioProcessor.presetVersion = presetGui.versionEditor.getText(); };

    // Comment
    gui.page.addAndMakeVisible(gui.commentLabel);
    gui.commentLabel.setText(presetCbComment, juce::dontSendNotification);
    gui.page.addAndMakeVisible(gui.commentEditor);
    gui.commentEditor.setMultiLine(true); // 複数行OK
    gui.commentEditor.setReturnKeyStartsNewLine(true); // Enterで改行
    gui.commentEditor.setText(audioProcessor.presetComment);
    gui.commentEditor.onTextChange = [this] {
        // 入力時にもサニタイズ（文字数制限など）をかけるとより安全ですが
        // ここでは保存時にかける方針で、そのまま代入します
        audioProcessor.presetComment = presetGui.commentEditor.getText();
        };

    /********************
    *
    * 5. Buttons
    *
    *********************/

    // --- Init Preset Button ---
    gui.page.addAndMakeVisible(gui.initButton);
    gui.initButton.setButtonText(initPresetBtnLabel);
    gui.initButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue.withAlpha(0.7f));

    gui.initButton.onClick = [this] {
        // 確認ダイアログを表示
        juce::AlertWindow::showAsync(juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::WarningIcon)
            .withTitle("Initialize Preset")
            .withMessage("Are you sure you want to initialize all parameters and unload samples?")
            .withButton("Initialize")
            .withButton("Cancel"),
            [this](int result) {
                if (result == 1) { // Initializeボタンが押された
                    // 1. プロセッサ側の初期化実行
                    audioProcessor.initPreset();

                    // 2. エディタの表示更新
                    // テキストエディタへの反映
                    presetGui.nameEditor.setText(audioProcessor.presetName);
                    presetGui.authorEditor.setText(audioProcessor.presetAuthor);
                    presetGui.versionEditor.setText(audioProcessor.presetVersion);
                    presetGui.commentEditor.setText(audioProcessor.presetComment);

                    // ファイル名表示のクリア
                    updateRhythmFileNames();
                    adpcmGui.fileNameLabel.setText("No File Loaded", juce::dontSendNotification);
                }
            }
        );
        };

    // --- Load Preset Info Button ---
    gui.page.addAndMakeVisible(gui.loadButton);
    gui.loadButton.setButtonText("Load Preset");
    gui.loadButton.setEnabled(false);
    gui.loadButton.onClick = [this] {
        auto file = presetGui.getSelectedFile();

        if (file.existsAsFile()) loadPresetFile(file);
        };

    // --- Save Preset Button ---
    gui.page.addAndMakeVisible(gui.saveButton);
    gui.saveButton.setButtonText("Save Preset");
    gui.saveButton.onClick = [this] { saveCurrentPreset(); };

    // --- Delete Preset Button ---
    gui.page.addAndMakeVisible(gui.deleteButton);
    gui.deleteButton.setButtonText("Delete Preset");
    gui.deleteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred.withAlpha(0.7f));
    gui.deleteButton.setEnabled(false);
    gui.deleteButton.onClick = [this] {
        auto file = presetGui.getSelectedFile();

        if (file.existsAsFile()) {
            // 確認ダイアログ
            juce::AlertWindow::showAsync(juce::MessageBoxOptions()
                .withIconType(juce::MessageBoxIconType::WarningIcon)
                .withTitle("Delete Preset")
                .withMessage("Are you sure you want to delete " + file.getFileName() + "?")
                .withButton("Delete")
                .withButton("Cancel"),
                [this, file](int result) {
                    if (result == 1) { // Delete
                        file.deleteFile();
                        scanPresets(); // リスト更新
                    }
                }
            );
        }
        };

    // --- Reflesh Preset List Button ---
    gui.page.addAndMakeVisible(gui.refreshButton);
    gui.refreshButton.setButtonText("Refresh Preset List");
    gui.refreshButton.onClick = [this] { scanPresets(); };

    // --- Reflect Preset Info Button ---
    gui.page.addAndMakeVisible(gui.reflectButton);
    gui.reflectButton.setButtonText("Reflect Preset Info");
    gui.reflectButton.setEnabled(false);
    gui.reflectButton.setTooltip("Reflect selected preset info to text editors without loading");
    gui.reflectButton.onClick = [this] {
        int row = presetGui.table.getSelectedRow();

        if (row >= 0 && row < presetGui.items.size()) {
            const auto& item = presetGui.items[row];

            // TextEditorにセットすると onTextChange が発火し、AudioProcessorの変数も更新されます
            presetGui.nameEditor.setText(item.name);
            presetGui.authorEditor.setText(item.author);
            presetGui.versionEditor.setText(item.version);
            presetGui.commentEditor.setText(item.comment);
        }
        };

    // --- Copy Preset Info to Clipboard Button ---
    gui.page.addAndMakeVisible(gui.copyButton);
    gui.copyButton.setButtonText("Copy Preset Info to Clipboard");
    gui.copyButton.setEnabled(false);
    gui.copyButton.onClick = [this] {
        int row = presetGui.table.getSelectedRow();

        if (row >= 0 && row < presetGui.items.size()) {
            const auto& item = presetGui.items[row];

            // フォーマットはお好みで調整してください
            juce::String info = presetCbName + item.name + "\n" +
                presetCbAuther + item.author + "\n" +
                presetCbVersion + item.version + "\n" +
                presetCbComment + item.comment + "\n" +
                presetCbMode + item.modeName;

            juce::SystemClipboard::copyTextToClipboard(info);
        }
        };
}
#endif

void AudioPlugin2686VEditor::setupFxGui(FxGuiSet& gui)
{
    const juce::String code = codeFx;

    auto setupB = [&](juce::Slider& s, juce::Label& l, juce::String name) {
        gui.page.addAndMakeVisible(s);

        s.setSliderStyle(juce::Slider::LinearHorizontal);
        s.setTextBoxStyle(juce::Slider::TextBoxRight, false, SliderValueWidth, SliderValueHeight);

        gui.page.addAndMakeVisible(l);

        l.setText(name, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
    };

    auto setupMixBtns = [&](juce::TextButton& bDry, juce::TextButton& bHalf, juce::TextButton& bWet, juce::Slider& targetSlider)
    {
        gui.page.addAndMakeVisible(bDry);
        gui.page.addAndMakeVisible(bHalf);
        gui.page.addAndMakeVisible(bWet);

        bDry.setButtonText("Dry");
        bHalf.setButtonText("50%");
        bWet.setButtonText("Wet");

        // クリック時の動作 (スライダーを動かせば連動してパラメータも変わります)
        bDry.onClick = [&] { targetSlider.setValue(0.0f); };
        bHalf.onClick = [&] { targetSlider.setValue(0.5f); };
        bWet.onClick = [&] { targetSlider.setValue(1.0f); };
    };

    // バイパスボタンのセットアップ用ヘルパー
    auto setupBypass = [&](juce::ToggleButton& btn)
    {
        gui.page.addAndMakeVisible(btn);
        btn.setButtonText("Bypass");
    };

    std::vector<SelectItem> qualityItems = {
        {.name = "0: Raw (32bit)", .value = 1 },
        {.name = "1: 24-bit PCM",  .value = 2 },
        {.name = "2: 16-bit PCM",  .value = 3 },
        {.name = "3: 8-bit PCM",   .value = 4 },
        {.name = "4: 5-bit PCM",   .value = 5 },
        {.name = "5: 4-bit PCM",   .value = 6 },
        {.name = "6: 4-bit ADPCM", .value = 7 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    // MainGroup
    SetupGroupParams mGroupParams = { .page = gui.page, .group = gui.mainGroup, .title = mGroupTitle };
    setupGroup(mGroupParams);

    gui.page.addAndMakeVisible(gui.bypassToggle);
    gui.bypassToggle.setButtonText(masterBypassLabel);

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    // Tremolo Group
    SetupGroupParams tGp = { .page = gui.page, .group = gui.tremGroup, .title = "Tremolo" };
    setupGroup(tGp);

    setupBypass(gui.tBypassBtn);
    setupB(gui.tRateSlider, gui.tRateLabel, "Rate");
    setupB(gui.tDepthSlider, gui.tDepthLabel, "Depth");
    setupB(gui.tMixSlider, gui.tMixLabel, "Mix");
    setupMixBtns(gui.tDryBtn, gui.tHalfBtn, gui.tWetBtn, gui.tMixSlider);

    // Vibrato Group
    SetupGroupParams vGp = { .page = gui.page, .group = gui.vibGroup, .title = "Vibrato" };
    setupGroup(vGp);

    setupBypass(gui.vBypassBtn);
    setupB(gui.vRateSlider, gui.vRateLabel, "Rate");
    setupB(gui.vDepthSlider, gui.vDepthLabel, "Depth");
    setupB(gui.vMixSlider, gui.vMixLabel, "Mix");
    setupMixBtns(gui.vDryBtn, gui.vHalfBtn, gui.vWetBtn, gui.vMixSlider);

    // Modern Bit Crusher Group
    SetupGroupParams mbcGp = { .page = gui.page, .group = gui.mbcGroup, .title = "Modern Bit Crusher" };
    setupGroup(mbcGp);

    setupBypass(gui.mbcBypassBtn);
    setupB(gui.mbcRateSlider, gui.mbcRateLabel, "Rate");
    setupB(gui.mbcBitsSlider, gui.mbcBitsLabel, "Quality");
    setupB(gui.mbcMixSlider, gui.mbcMixLabel, "Mix");
    setupMixBtns(gui.mbcDryBtn, gui.mbcHalfBtn, gui.mbcWetBtn, gui.mbcMixSlider);

    // Delay Group
    SetupGroupParams dGp = { .page = gui.page, .group = gui.delayGroup, .title = "Delay" };
    setupGroup(dGp);

    setupBypass(gui.dBypassBtn);
    setupB(gui.dTimeSlider, gui.dTimeLabel, "Time (ms)");
    setupB(gui.dFbSlider, gui.dFbLabel, "Feedback");
    setupB(gui.dMixSlider, gui.dMixLabel, "Mix");
    setupMixBtns(gui.dDryBtn, gui.dHalfBtn, gui.dWetBtn, gui.dMixSlider);

    // Reverb Group
    SetupGroupParams rGp = { .page = gui.page, .group = gui.reverbGroup, .title = "Reverb" };
    setupGroup(rGp);

    setupBypass(gui.rBypassBtn);
    setupB(gui.rSizeSlider, gui.rSizeLabel, "Size");
    setupB(gui.rDampSlider, gui.rDampLabel, "Damp");
    setupB(gui.rMixSlider, gui.rMixLabel, "Mix");
    setupMixBtns(gui.rDryBtn, gui.rHalfBtn, gui.rWetBtn, gui.rMixSlider);

    // Retro Bit Crusher Group
    SetupGroupParams rbcGp = { .page = gui.page, .group = gui.rbcGroup, .title = "Retro Bit Crusher" };
    setupGroup(rbcGp);

    setupBypass(gui.rbcBypassBtn);

    SetupComboParams rbcRateP = SetupComboParams::create(gui.page, gui.rbcRateCombo, gui.rbcRateLabel, gui.rbcRateAtt, code + codeFxRbc + postRate, "Rate", rateItems);
    setupCombo(rbcRateP);
    gui.rbcRateLabel.setJustificationType(juce::Justification::centred);

    SetupComboParams rbcModeP = SetupComboParams::create(gui.page, gui.rbcBitsCombo, gui.rbcBitsLabel, gui.rbcBitsAtt, code + codeFxRbc + postFxBit, "Quality", qualityItems);
    setupCombo(rbcModeP);
    gui.rbcBitsLabel.setJustificationType(juce::Justification::centred);

    setupB(gui.rbcMixSlider, gui.rbcMixLabel, "Mix");
    setupMixBtns(gui.rbcDryBtn, gui.rbcHalfBtn, gui.rbcWetBtn, gui.rbcMixSlider);

    // Attachments
    gui.fxBypassAtt = std::make_unique<ButtonAttachment>(audioProcessor.apvts, code + postBypass, gui.bypassToggle);

    gui.tBypassAtt = std::make_unique<ButtonAttachment>(audioProcessor.apvts, code + codeFxTrm + postBypass, gui.tBypassBtn);
    gui.tRateAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxTrm + postRate, gui.tRateSlider);
    gui.tDepthAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxTrm + postDepth, gui.tDepthSlider);
    gui.tMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxTrm + postMix, gui.tMixSlider);

    gui.vBypassAtt = std::make_unique<ButtonAttachment>(audioProcessor.apvts, code + codeFxVib + postBypass, gui.vBypassBtn);
    gui.vRateAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxVib + postRate, gui.vRateSlider);
    gui.vDepthAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxVib + postDepth, gui.vDepthSlider);
    gui.vMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxVib + postMix, gui.vMixSlider);

    gui.mbcBypassAtt = std::make_unique<ButtonAttachment>(audioProcessor.apvts, code + codeFxMbc + postBypass, gui.mbcBypassBtn);
    gui.mbcRateAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxMbc + postRate, gui.mbcRateSlider);
    gui.mbcBitsAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxMbc + postFxBit, gui.mbcBitsSlider);
    gui.mbcMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxMbc + postMix, gui.mbcMixSlider);

    gui.dBypassAtt = std::make_unique<ButtonAttachment>(audioProcessor.apvts, code + codeFxDly + postBypass, gui.dBypassBtn);
    gui.dTimeAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxDly + postFxTime, gui.dTimeSlider);
    gui.dFbAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxDly + postFxFb, gui.dFbSlider);
    gui.dMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxDly + postMix, gui.dMixSlider);

    gui.rBypassAtt = std::make_unique<ButtonAttachment>(audioProcessor.apvts, code + codeFxRvb + postBypass, gui.rBypassBtn);
    gui.rSizeAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxRvb + postFxSize, gui.rSizeSlider);
    gui.rDampAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxRvb + postFxDamp, gui.rDampSlider);
    gui.rMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxRvb + postMix, gui.rMixSlider);

    gui.rbcBypassAtt = std::make_unique<ButtonAttachment>(audioProcessor.apvts, code + codeFxRbc + postBypass, gui.rbcBypassBtn);
    gui.rbcMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, code + codeFxRbc + postMix, gui.rbcMixSlider);
}

void AudioPlugin2686VEditor::setupSettingsGui(SettingsGuiSet& gui)
{
    SetupGroupParams gp = { gui.page, gui.group, "Environment Settings" };
    setupGroup(gp);

    auto setupRow = [&](juce::Label& lbl, juce::String title, juce::Label& pathLbl, juce::TextButton& btn, juce::String btnText = "Browse...") {
        gui.page.addAndMakeVisible(lbl);
        lbl.setText(title, juce::dontSendNotification);
        gui.page.addAndMakeVisible(pathLbl);
        pathLbl.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));
        pathLbl.setJustificationType(juce::Justification::centredLeft);
        gui.page.addAndMakeVisible(btn);
        btn.setButtonText(btnText);
    };

    // --- Wallpaper ---
    setupRow(gui.wallpaperLabel, "Wallpaper:", gui.wallpaperPathLabel, gui.wallpaperBrowseBtn);
    gui.wallpaperPathLabel.setText(audioProcessor.wallpaperPath, juce::dontSendNotification);

    gui.wallpaperBrowseBtn.onClick = [this] {
        auto fileFilter = "*.png;*.jpg;*.jpeg";
        fileChooser = std::make_unique<juce::FileChooser>("Select Wallpaper Image", juce::File(), fileFilter);
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    audioProcessor.wallpaperPath = file.getFullPathName();
                    settingsGui.wallpaperPathLabel.setText(file.getFileName(), juce::dontSendNotification);
                    loadWallpaperImage();
                }
            }
        );
    };

    gui.page.addAndMakeVisible(gui.wallpaperClearBtn);
    gui.wallpaperClearBtn.setButtonText("Clear");
    gui.wallpaperClearBtn.onClick = [this] {
        audioProcessor.wallpaperPath = "";
        settingsGui.wallpaperPathLabel.setText("(None)", juce::dontSendNotification);
        loadWallpaperImage();
    };

#if !defined(BUILD_AS_FX_PLUGIN)
    // --- ADPCM Dir ---
    setupRow(gui.sampleDirLabel, "Sample Dir:", gui.sampleDirPathLabel, gui.sampleDirBrowseBtn);
    gui.sampleDirPathLabel.setText(audioProcessor.defaultSampleDir, juce::dontSendNotification);

    gui.sampleDirBrowseBtn.onClick = [this] {
        fileChooser = std::make_unique<juce::FileChooser>("Select Default Sample Directory",
            audioProcessor.defaultSampleDir.isEmpty() ? juce::File::getSpecialLocation(juce::File::userHomeDirectory) : juce::File(audioProcessor.defaultSampleDir));
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    audioProcessor.defaultSampleDir = file.getFullPathName();
                    audioProcessor.lastSampleDirectory = file;
                    settingsGui.sampleDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                }
            }
        );
    };

    // --- Preset Dir ---
    setupRow(gui.presetDirLabel, "Preset Dir:", gui.presetDirPathLabel, gui.presetDirBrowseBtn);
    gui.presetDirPathLabel.setText(audioProcessor.defaultPresetDir, juce::dontSendNotification);

    gui.presetDirBrowseBtn.onClick = [this] {
        fileChooser = std::make_unique<juce::FileChooser>("Select Default Preset Directory",
            audioProcessor.defaultPresetDir.isEmpty() ? juce::File::getSpecialLocation(juce::File::userDocumentsDirectory) : juce::File(audioProcessor.defaultPresetDir));
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    audioProcessor.defaultPresetDir = file.getFullPathName();
                    settingsGui.presetDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);

                    presetGui.currentFolder = file;
                    presetGui.pathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
                    scanPresets();
                }
            }
        );
    };

    // --- Toggle Tooltip Visible Toggle Button ---
    gui.page.addAndMakeVisible(gui.tooltipLabel);
    gui.tooltipLabel.setText("Show Parameter Range:", juce::dontSendNotification);

    gui.page.addAndMakeVisible(gui.tooltipToggle);
    gui.tooltipToggle.setButtonText("ON");
    gui.tooltipToggle.setToggleState(audioProcessor.showTooltips, juce::dontSendNotification);

    gui.tooltipToggle.onClick = [this] {
        bool newState = settingsGui.tooltipToggle.getToggleState();
        audioProcessor.showTooltips = newState;
        setTooltipState(newState); // 即座に反映
    };

    // --- Use Headroom Toggle ---
    gui.page.addAndMakeVisible(gui.useHeadroomLabel);
    gui.useHeadroomLabel.setText("Internal Headroom:", juce::dontSendNotification);

    gui.page.addAndMakeVisible(gui.useHeadroomToggle);
    gui.useHeadroomToggle.setButtonText("Enable Headroom");
    gui.useHeadroomToggle.setToggleState(audioProcessor.useHeadroom, juce::dontSendNotification);
    gui.useHeadroomToggle.onClick = [this, &gui] {
        bool state = gui.useHeadroomToggle.getToggleState();
        audioProcessor.useHeadroom = state;
        gui.headroomGainSlider.setEnabled(state); // OFFならスライダーも無効化
        };

    // --- Headroom Gain Slider---
    gui.page.addAndMakeVisible(gui.headroomGainSlider);
    gui.headroomGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    gui.headroomGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    gui.headroomGainSlider.setRange(0.0, 1.0, 0.01); // 0.0 ~ 1.0
    // プロセッサの値で初期化
    gui.headroomGainSlider.setValue(audioProcessor.headroomGain, juce::dontSendNotification);
    gui.headroomGainSlider.setEnabled(audioProcessor.useHeadroom);

    gui.headroomGainSlider.onValueChange = [this, &gui] {
        audioProcessor.headroomGain = (float)gui.headroomGainSlider.getValue();
        };
#endif

    // --- Save Preference Button ---
    gui.page.addAndMakeVisible(gui.saveSettingsBtn);
    gui.saveSettingsBtn.setButtonText("Save Settings");
    gui.saveSettingsBtn.onClick = [this] {
        fileChooser = std::make_unique<juce::FileChooser>("Save Environment Settings",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("2686V_Config.xml"), "*.xml");

        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file != juce::File()) {
                    audioProcessor.saveEnvironment(file);
                }
            }
        );
    };

    // --- Load Preference Button ---
    gui.page.addAndMakeVisible(gui.loadSettingsBtn);
    gui.loadSettingsBtn.setButtonText("Load Settings");
    gui.loadSettingsBtn.onClick = [this] {
        fileChooser = std::make_unique<juce::FileChooser>("Load Environment Settings",
            juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "*.xml");

        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    audioProcessor.loadEnvironment(file);

                    // UI反映
                    settingsGui.wallpaperPathLabel.setText(audioProcessor.wallpaperPath.isEmpty() ? "(None)" : juce::File(audioProcessor.wallpaperPath).getFileName(), juce::dontSendNotification);
                    settingsGui.sampleDirPathLabel.setText(audioProcessor.defaultSampleDir, juce::dontSendNotification);
                    settingsGui.presetDirPathLabel.setText(audioProcessor.defaultPresetDir, juce::dontSendNotification);

                    // 壁紙再描画
                    loadWallpaperImage();

#if !defined(BUILD_AS_FX_PLUGIN)
                    // プリセットリスト更新
                    if (juce::File(audioProcessor.defaultPresetDir).isDirectory()) {
                        presetGui.currentFolder = juce::File(audioProcessor.defaultPresetDir);
                        presetGui.pathLabel.setText(presetGui.currentFolder.getFullPathName(), juce::dontSendNotification);
                        scanPresets(); // リスト更新関数を呼ぶ
                    }
#endif
                }
            }
        );
    };

    gui.group.addAndMakeVisible(gui.saveStartupSettingsBtn);
    gui.saveStartupSettingsBtn.setButtonText("Save Current Settings as Default");
    gui.saveStartupSettingsBtn.onClick = [this]
        {
            auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
            auto pluginDir = docDir.getChildFile(assetFolder);

            // フォルダがなければ作る
            if (!pluginDir.exists()) pluginDir.createDirectory();

            auto file = pluginDir.getChildFile(defaultSettingFilename);

            // 2. XMLデータの作成
            juce::XmlElement xml(envCode);

            xml.setAttribute(settingWallpaperPath, audioProcessor.wallpaperPath);
            xml.setAttribute(settingDefaultSampleDir, audioProcessor.defaultSampleDir);
            xml.setAttribute(settingDefaultPresetDir, audioProcessor.defaultPresetDir);
            xml.setAttribute(settingShowTooltips, audioProcessor.showTooltips);
            xml.setAttribute(settingUseHeadroom, audioProcessor.useHeadroom);
            xml.setAttribute(settingHeadroomGain, audioProcessor.headroomGain);

            // 3. 書き出し実行
            if (xml.writeTo(file))
            {
                // 成功メッセージ
                juce::NativeMessageBox::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "Success",
                    "Current settings have been saved as the default startup configuration.\n\nFile: " + file.getFullPathName(),
                    this
                );
            }
            else
            {
                // 失敗メッセージ
                juce::NativeMessageBox::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Error",
                    "Failed to save startup settings file.",
                    this
                );
            }
        };
}

void AudioPlugin2686VEditor::setupAboutGui(AboutGuiSet& gui)
{
    // 1. Plugin Name
    gui.page.addAndMakeVisible(gui.pluginNameLabel);
    gui.pluginNameLabel.setText(pluginName, juce::dontSendNotification);
    gui.pluginNameLabel.setFont(juce::Font(FontFamily, 64.0f, juce::Font::bold | juce::Font::italic));
    gui.pluginNameLabel.setJustificationType(juce::Justification::centred);

    // 2. Version
    gui.page.addAndMakeVisible(gui.versionLabel);
    gui.versionLabel.setText(pluginVersion, juce::dontSendNotification);
    gui.versionLabel.setFont(juce::Font(24.0f));
    gui.versionLabel.setJustificationType(juce::Justification::centred);

    // 3. Copyright
    gui.page.addAndMakeVisible(gui.copyrightLabel);
    gui.copyrightLabel.setText(pluginAuthor, juce::dontSendNotification);
    gui.copyrightLabel.setJustificationType(juce::Justification::centred);

    // 4. Logo (BinaryDataから読み込み)
    gui.page.addAndMakeVisible(gui.vst3Logo);

    // ファイルパスではなく、メモリ(バイナリ)から読み込む
    // 名前空間: VstLogoForAboutData (CMakeで指定したもの)
    // 変数名: ファイル名のドットがアンダースコアになったもの (vst3_logo_png)
    auto logoImg = juce::ImageCache::getFromMemory(
        VstLogoForAboutData::vst3_logo_png,
        VstLogoForAboutData::vst3_logo_pngSize
    );
    gui.vst3Logo.setImage(logoImg);

    // 5. VST Trademark Notice (必須表記)
    gui.page.addAndMakeVisible(gui.vstGuidelineLabel);
    gui.vstGuidelineLabel.setText(vstNotice, juce::dontSendNotification);
    gui.vstGuidelineLabel.setFont(juce::Font(12.0f)); // 小さめでOK
    gui.vstGuidelineLabel.setJustificationType(juce::Justification::centred);
    gui.vstGuidelineLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey); // 目立ちすぎない色に

    // --- GPLv3ライセンス表示 ---
    // 1. 通知テキスト
    gui.gplNoticeLabel.setText(gplv3Notice, juce::dontSendNotification);
    gui.gplNoticeLabel.setFont(juce::Font(14.0f));
    gui.gplNoticeLabel.setJustificationType(juce::Justification::centred);
    gui.page.addAndMakeVisible(gui.gplNoticeLabel);

    // 2. リンクボタン
    gui.gplLinkButton.setButtonText(gplv3Navigate);
    gui.gplLinkButton.setURL(juce::URL(gplLinkUrl));
    gui.gplLinkButton.setColour(juce::HyperlinkButton::textColourId, juce::Colours::lightblue); // 色設定（リンクっぽく青色にする例）
    gui.page.addAndMakeVisible(gui.gplLinkButton);
}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686VEditor::layoutOpnaPage(OpnaGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.rateCombo, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.algSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedbackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedback2Label, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedback2Slider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.lfoFreqLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.lfoFreqSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.lfoPmsLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.lfoPmsSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.lfoAmsLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.lfoAmsSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        gui.opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.mul[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dtLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dt[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ar[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.srLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.sr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.sl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.tl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.ksLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ks[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.seLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.se[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.seFreqLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.seFreq[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.fix[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.freq[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqToZero[i], { OpRegFreqChangeButtonWidth, OpRegPaddingRight} }, { &gui.freqTo440[i], { OpRegFreqChangeButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.am[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mask[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.mmlBtn[i], { OpRegButtonWidth, 0 } } });
    }
}

void AudioPlugin2686VEditor::layoutOpnPage(OpnGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.rateCombo, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.algSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedbackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedback2Label, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedback2Slider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea.reduced(2));

        // 枠線の内側
        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.mul[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dtLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dt[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ar[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.srLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.sr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.sl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.tl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.ksLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ks[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.fix[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.freq[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqToZero[i], { OpRegFreqChangeButtonWidth, OpRegPaddingRight} }, { &gui.freqTo440[i], { OpRegFreqChangeButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mask[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.mmlBtn[i], { OpRegButtonWidth, 0} } });
    }
}

void AudioPlugin2686VEditor::layoutOplPage(OplGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.rateCombo, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.algSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedbackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    // --- B. Operators Section ---
    for (int i = 0; i < 2; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);
        gui.opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.mul[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dtLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dt[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ar[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.sl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.tl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.am[i], { OpRegButtonWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.vib[i], { OpRegButtonWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.egType[i], { OpRegButtonWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.ksr[i], { OpRegButtonWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.kslLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ksl[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.egLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.eg[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mask[i], { OpRegButtonWidth, OpRegPaddingRight}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.mmlBtn[i], { OpRegButtonWidth, 0}} });
    }
}

void AudioPlugin2686VEditor::layoutOpl3Page(Opl3GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.rateCombo, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.algSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedbackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedback2Label, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedback2Slider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    // --- Operators Section ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);
        gui.opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.mul[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ar[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.sl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.tl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.am[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.vib[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.egType[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.ksr[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.kslLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ksl[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.egLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.eg[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mask[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.mmlBtn[i], { OpRegButtonWidth, 0} } });
    }
}

void AudioPlugin2686VEditor::layoutOpmPage(OpmGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.rateCombo, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.algSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedbackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedback2Label, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedback2Slider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.lfoFreqLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.lfoFreqSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.pmsLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.pmsSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.amsLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.amsSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.mul[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dt1Label[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dt1[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dt2Label[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dt2[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ar[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dr[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.srLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.sr[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.sl[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rr[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.tl[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.ksLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ks[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.fix[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.freq[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqToZero[i], { OpRegFreqChangeButtonWidth, OpRegPaddingRight} }, { &gui.freqTo440[i], { OpRegFreqChangeButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mask[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.mmlBtn[i], { OpRegButtonWidth, 0} } });
    }
}

void AudioPlugin2686VEditor::layoutOpzx3Page(Opzx3GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.rateCombo, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.algSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedbackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedback2Label, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.feedback2Slider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.lfoFreqLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.lfoFreqSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.pmsLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.pmsSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.amsLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.amsSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.mul[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dt1Label[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dt1[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dt2Label[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dt2[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ar[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.srLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.sr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.sl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rr[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.tl[i], { OpRegValueWidth, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.ksLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ks[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.fix[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.freq[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqToZero[i], { OpRegFreqChangeButtonWidth, OpRegPaddingRight} }, { &gui.freqTo440[i], { OpRegFreqChangeButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.wsLabel[i], { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.ws[i], { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mask[i], { OpRegButtonWidth, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.mmlBtn[i], { OpRegButtonWidth, 0} } });
    }
}

void AudioPlugin2686VEditor::layoutSsgPage(SsgGuiSet& gui, juce::Rectangle<int> content)
{
    juce::String code = codeSsg;
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.rateCombo, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.adsrBypassButton, { MainRegButtonWidth, MainRegPaddingRight} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.attackLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.attackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.decayLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.decaySlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.sustainLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.sustainSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.releaseLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.releaseSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    // --- Voice Group ---
    auto voiceArea = pageArea.removeFromLeft(FmOpWidth);

    gui.voiceGroup.setBounds(voiceArea);
    auto vRect = gui.voiceGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    vRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.levelLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.levelSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.noiseLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.noiseSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.noiseFreqLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.noiseFreqSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.noiseOnNoteButton, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mixLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.mixSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, {
        { &gui.mixSetTone, { OpRegMixChangeButtonWidth, 0} },
        { &gui.mixSetMix, { OpRegMixChangeButtonWidth, 0} },
        { &gui.mixSetNoise, { OpRegMixChangeButtonWidth, 0} }
    });
    layoutComponentsLtoR(vRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.waveLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.waveSelector, { OpRegValueWidth, 0} } });

    // Wave Group
    float waveParam = *audioProcessor.apvts.getRawParameterValue(code + postWaveform);
    int waveMode = (waveParam > 0.5f) ? 1 : 0;
    auto waveArea = pageArea.removeFromLeft(FmOpWidth);

    if (waveMode == 0) // Pulse
    {
        gui.dutyGroup.setVisible(true);
        gui.triGroup.setVisible(false);

        gui.triKeyTrackButton.setVisible(false);
        gui.triPeakSlider.setVisible(false);
        gui.triPeakLabel.setVisible(false);
        gui.triFreqSlider.setVisible(false);
        gui.triFreqLabel.setVisible(false);
        gui.triSetSawDown.setVisible(false);
        gui.triSetTri.setVisible(false);
        gui.triSetSawUp.setVisible(false);

        gui.dutyModeSelector.setVisible(true);
        gui.dutyModeLabel.setVisible(true);
        gui.dutyVarSlider.setValue(true);
        gui.dutyVarLabel.setVisible(true);
        gui.dutyInvertButton.setVisible(true);
        
        gui.dutyGroup.setBounds(waveArea);
        auto dRect = gui.dutyGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

        dRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(dRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dutyModeLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dutyModeSelector, { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(dRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dutyInvertButton, { OpRegButtonWidth, 0} } });

        float dutyModeVal = *audioProcessor.apvts.getRawParameterValue(code + postDutyMode);
        if (dutyModeVal < 0.5f) {
            gui.dutyPresetSelector.setVisible(true);
            gui.dutyPresetLabel.setVisible(true);
            gui.dutyVarSlider.setVisible(false);
            gui.dutyVarLabel.setVisible(false);
            layoutComponentsLtoR(dRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.dutyPresetLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dutyPresetSelector, { OpRegValueWidth, 0} } });
        }
        else {
            gui.dutyPresetSelector.setVisible(false);
            gui.dutyPresetLabel.setVisible(false);
            gui.dutyVarSlider.setVisible(true);
            gui.dutyVarLabel.setVisible(true);
            layoutComponentsLtoR(dRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.dutyVarLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dutyVarSlider, { OpRegValueWidth, 0} } });
        }
    }
    else // Triangle
    {
        gui.dutyGroup.setVisible(false);
        gui.triGroup.setVisible(true);

        gui.dutyModeSelector.setVisible(false);
        gui.dutyModeLabel.setVisible(false);
        gui.dutyInvertButton.setVisible(false);
        gui.dutyPresetSelector.setVisible(false);
        gui.dutyPresetLabel.setVisible(false);
        gui.dutyVarSlider.setVisible(false);
        gui.dutyVarLabel.setVisible(false);

        gui.triKeyTrackButton.setVisible(true);
        gui.triFreqSlider.setVisible(true);
        gui.triFreqLabel.setVisible(true);
        gui.triPeakSlider.setVisible(true);
        gui.triPeakLabel.setVisible(true);
        gui.triSetSawDown.setVisible(true);
        gui.triSetTri.setVisible(true);
        gui.triSetSawUp.setVisible(true);

        gui.triGroup.setBounds(waveArea);
        auto tRect = gui.triGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

        tRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &gui.triKeyTrackButton, { OpRegButtonWidth, 0} } });

        bool isKeyTrack = gui.triKeyTrackButton.getToggleState();
        gui.triFreqSlider.setVisible(!isKeyTrack);
        gui.triFreqLabel.setVisible(!isKeyTrack);

        if (!isKeyTrack) {
            layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &gui.triFreqLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.triFreqSlider, { OpRegValueWidth, 0} } });
            layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &gui.triPeakLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.triPeakSlider, { OpRegValueWidth, 0} } });
        }
        else {
            layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &gui.triPeakLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.triPeakSlider, { OpRegValueWidth, 0} } });
        }

        layoutComponentsLtoR(tRect, OpRowHeight, OpLastRowPaddingBottom, {
            { &gui.triSetSawDown, { OpRegSsgTriPeakButtonWidth, 0} },
            { &gui.triSetTri, { OpRegSsgTriPeakButtonWidth, 0} },
            { &gui.triSetSawUp, { OpRegSsgTriPeakButtonWidth, 0} }
        });
    }

    // HW Env Group
    auto envArea = pageArea.removeFromLeft(FmOpWidth);
    gui.envGroup.setBounds(envArea);
    auto eRect = gui.envGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    eRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(eRect, OpRowHeight, OpRowPaddingBottom, { { &gui.envEnableButton, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(eRect, OpRowHeight, OpRowPaddingBottom, { { &gui.shapeLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.shapeSelector, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(eRect, OpRowHeight, OpRowPaddingBottom, { { &gui.periodLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.periodSlider, { OpRegValueWidth, 0} } });
}

void AudioPlugin2686VEditor::layoutWtPage(WtGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.rateCombo, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.levelLabel, { MainRegLabelWidth, MainRegPaddingRight}}, { &gui.levelSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.attackLabel, { MainRegLabelWidth, MainRegPaddingRight}}, { &gui.attackSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.decayLabel, { MainRegLabelWidth, MainRegPaddingRight}}, { &gui.decaySlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.sustainLabel, { MainRegLabelWidth, MainRegPaddingRight}}, { &gui.sustainSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.releaseLabel, { MainRegLabelWidth, MainRegPaddingRight}}, { &gui.releaseSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.waveLabel, { MainRegLabelWidth, MainRegPaddingRight}}, { &gui.waveSelector, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.sizeLabel, { MainRegLabelWidth, MainRegPaddingRight}}, { &gui.sizeSelector, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.modEnableButton, { MainRegButtonWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.modDepthLabel, { MainRegLabelWidth, MainRegPaddingRight}}, { &gui.modDepthSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.modSpeedLabel, { MainRegLabelWidth, MainRegPaddingRight}}, { &gui.modSpeedSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    bool isMod = gui.modEnableButton.getToggleState();
    gui.modDepthSlider.setEnabled(isMod);
    gui.modDepthLabel.setEnabled(isMod);
    gui.modSpeedSlider.setEnabled(isMod);
    gui.modSpeedLabel.setEnabled(isMod);

    // 波形がカスタム以外の時は波形精度選択を Disabled に
    bool isCustomWave = (gui.waveSelector.getSelectedId() == 9);

    gui.sizeLabel.setEnabled(isCustomWave);
    gui.sizeSelector.setEnabled(isCustomWave);
    gui.waveContainer.setEnabled(isCustomWave);
    gui.customWaveResetBtn.setEnabled(isCustomWave);

    // Custom Wave
    // Custom Mode Layout
    // 中央に32/64本のスライダーを配置
    // 波形選択が Custom 以外の時は Disabled 表示
    auto rightArea = pageArea.removeFromLeft(WtRightWidth).removeFromTop(WtRightHeight);

    gui.customWaveGroup.setBounds(rightArea);

    auto cwRect = gui.customWaveGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);
    cwRect.removeFromTop(TitlePaddingTop);

    gui.waveContainer.setBounds(cwRect.removeFromTop(WtCustomSliderHeight));

    // Check if Custom Mode is selected (Index 8 -> ID 9 based on addItem?)
    int waveSize = gui.sizeSelector.getSelectedId() == 2 ? 64 : 32;

    gui.customWaveGroup.setEnabled(isCustomWave);
    gui.waveContainer.setEnabled(isCustomWave);

    switch (waveSize)
    {
    case 32:
        gui.waveContainer.setTargetSliders(gui.customSliders32);

        for (auto& s : gui.customSliders64) {
            s.setVisible(false);
            s.setEnabled(false);
        }

        break;
    case 64:
        gui.waveContainer.setTargetSliders(gui.customSliders64);

        for (auto& s : gui.customSliders32)
        {
            s.setVisible(false);
            s.setEnabled(false);
        }

        break;
    default:
        DBG("Illegal Custom Wave Size");
    }

    cwRect.removeFromTop(WtCustomSliderResetBtnPaddingTop);
    layoutComponentsLtoR(cwRect, WtCustomSliderResetBtnHeight, MainLastRowPaddingBottom, { { &gui.customWaveResetBtn, { MainRegButtonWidth, 0} } });
}

void AudioPlugin2686VEditor::layoutRhythmPage(RhythmGuiSet& gui, juce::Rectangle<int> content)
{
    auto applyPads = [&](juce::Rectangle<int>& area, int width, int start, int length)
        {
            for (int i = start; i < start + length; ++i)
            {
                auto& pad = gui.pads[i];

                auto padArea = area.removeFromLeft(width);
                pad.group.setBounds(padArea);

                auto padRect = padArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

                padRect.removeFromTop(TitlePaddingTop);

                layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &pad.loadButton, { OpRegLabelWidth, OpRegPaddingRight}}, { &pad.fileNameLabel, { 150, 0}},{ &pad.clearButton, { 30, 0}}, });
                layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &pad.oneShotButton, { OpRegButtonWidth, 0} } });
                layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &pad.noteLabel, { OpRegLabelWidth, OpRegPaddingRight}}, { &pad.noteSlider, { OpRegValueWidth, 0}} });
                layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &pad.modeLabel, { OpRegLabelWidth, OpRegPaddingRight}}, { &pad.modeCombo, { OpRegValueWidth, 0}} });
                layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &pad.rateLabel, { OpRegLabelWidth, OpRegPaddingRight}}, { &pad.rateCombo, { OpRegValueWidth, 0}} });
                layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &pad.panLabel, { OpRegLabelWidth, OpRegPaddingRight}}, { &pad.panSlider, { OpRegValueWidth, 0}} });
                layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, {
                    { &pad.btnPanL, { OpRegPanChangeButtonWidth, 0} },
                    { &pad.btnPanC, { OpRegPanChangeButtonWidth, 0} },
                    { &pad.btnPanR, { OpRegPanChangeButtonWidth, 0} }
                });
                layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &pad.volLabel, { OpRegLabelWidth, OpRegPaddingRight}}, { &pad.volSlider, { OpRegValueWidth, 0}} });
                layoutComponentsLtoR(padRect, OpRowHeight, OpRowPaddingBottom, { { &pad.rrLabel, { OpRegLabelWidth, OpRegPaddingRight}}, { &pad.rrSlider, { OpRegValueWidth, 0}} });
            }

        };

    // Top section for Master Volume
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.levelLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.levelSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    auto topPadsArea = pageArea.removeFromTop(RhythmPadsAreaHeight);
    auto bottomPadsArea = pageArea.removeFromTop(RhythmPadsAreaHeight);

    // Remaining area for 8 pads
    applyPads(topPadsArea, topPadsArea.getWidth() / 4, 0, 4);
    applyPads(bottomPadsArea, bottomPadsArea.getWidth() / 4, 4, 4);
}

void AudioPlugin2686VEditor::layoutAdpcmPage(AdpcmGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);
    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.modeLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.modeCombo, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.rateCombo, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.levelLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.levelSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.loopButton, { MainRegButtonWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.panLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.panSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, {
        { &gui.btnPanL, { MainRegPanChangeButtonWidth, MainRegPanPaddingHeight}},
        { &gui.btnPanC, { MainRegPanChangeButtonWidth, MainRegPanPaddingHeight}},
        { &gui.btnPanR, { MainRegPanChangeButtonWidth, 0}}
    });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.attackLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.attackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.decayLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.decaySlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.sustainLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.sustainSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.releaseLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.releaseSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    auto headerArea = pageArea.removeFromTop(HeaderHeight);

    gui.group.setBounds(headerArea);

    auto inner = headerArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    inner.removeFromTop(TitlePaddingTop);

    auto fileRow = inner.removeFromTop(30);
    // 左にロードボタン
    gui.loadButton.setBounds(fileRow.removeFromLeft(100));
    fileRow.removeFromLeft(10);
    // 右にクリアボタン
    gui.clearButton.setBounds(fileRow.removeFromRight(60));
    fileRow.removeFromRight(10);
    // 中間にファイル名ラベル
    gui.fileNameLabel.setBounds(fileRow);
}
#endif

void AudioPlugin2686VEditor::layoutFxPage(FxGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();
       
    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bypassToggle, { MainRegButtonWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { MainRegLabelWidth, MainRegPaddingRight} }, { &gui.masterVolSlider, { MainRegValueWidth, 0} } });

    auto topCol = pageArea.removeFromTop(FxAreaHeight);
    auto bottomCol = pageArea.removeFromTop(FxAreaHeight);

    // Left

    // 1. Tremolo
    auto trmArea = topCol.removeFromLeft(FmOpWidth);
    gui.tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    trmRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(trmRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.tBypassBtn, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(trmRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tRateLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.tRateSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(trmRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tDepthLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.tDepthSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(trmRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tMixLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.tMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(trmRect, OpRowHeight, OpRowPaddingBottom, {
        { &gui.tDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.tHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.tWetBtn, { OpRegPanChangeButtonWidth, 0} }
    });

    // 2. Vibrato
    auto vibArea = topCol.removeFromLeft(FmOpWidth);
    gui.vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    vibRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(vibRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.vBypassBtn, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(vibRect, OpRowHeight, OpRowPaddingBottom, { { &gui.vRateLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.vRateSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(vibRect, OpRowHeight, OpRowPaddingBottom, { { &gui.vDepthLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.vDepthSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(vibRect, OpRowHeight, OpRowPaddingBottom, { { &gui.vMixLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.vMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(vibRect, OpRowHeight, OpRowPaddingBottom, {
        { &gui.vDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.vHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.vWetBtn, { OpRegPanChangeButtonWidth, 0} }
    });

    // 3. Modern Bit Crusher
    auto mbcArea = topCol.removeFromLeft(FmOpWidth);
    gui.mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    mbcRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mbcRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.mbcBypassBtn, { OpRegButtonWidth, OpRegPaddingRight} } });
    layoutComponentsLtoR(mbcRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mbcRateLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.mbcRateSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(mbcRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mbcBitsLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.mbcBitsSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(mbcRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mbcMixLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.mbcMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(mbcRect, OpRowHeight, OpRowPaddingBottom, {
        { &gui.mbcDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.mbcHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.mbcWetBtn, { OpRegPanChangeButtonWidth, 0} }
    });

    // 4. Delay
    auto dlyArea = bottomCol.removeFromLeft(FmOpWidth);
    gui.delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    dlyRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(dlyRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.dBypassBtn, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(dlyRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dTimeLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dTimeSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(dlyRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dFbLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dFbSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(dlyRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dMixLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.dMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(dlyRect, OpRowHeight, OpRowPaddingBottom, {
        { &gui.dDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.dHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.dWetBtn, { OpRegPanChangeButtonWidth, 0} }
    });

    // 5. Reverb
    auto rvbArea = bottomCol.removeFromLeft(FmOpWidth);
    gui.reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    rvbRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(rvbRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.rBypassBtn, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(rvbRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rSizeLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rSizeSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(rvbRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rDampLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rDampSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(rvbRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rMixLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(rvbRect, OpRowHeight, OpRowPaddingBottom, {
        { &gui.rDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.rHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.rWetBtn, { OpRegPanChangeButtonWidth, 0} }
    });

    // 6. Retro Bit Crusher
    auto rbcArea = bottomCol.removeFromLeft(FmOpWidth);
    gui.rbcGroup.setBounds(rbcArea);

    auto rbcRect = rbcArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    rbcRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(rbcRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.rbcBypassBtn, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(rbcRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rbcBitsLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rbcBitsCombo, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(rbcRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rbcRateLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rbcRateCombo, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(rbcRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rbcMixLabel, { OpRegLabelWidth, OpRegPaddingRight} }, { &gui.rbcMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(rbcRect, OpRowHeight, OpRowPaddingBottom, {
        { &gui.rbcDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.rbcHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &gui.rbcWetBtn, { OpRegPanChangeButtonWidth, 0} }
    });
}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686VEditor::layoutPresetPage(PresetGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // Path Label (Top)
    gui.pathLabel.setBounds(pageArea.removeFromTop(PresetFileLabelHeight));

    // Left: List
    auto listArea = pageArea.removeFromLeft(pageArea.getWidth() * PresetTableWidthRate);
    gui.table.setBounds(listArea.reduced(PresetTablePaddingWidth, PresetTablePaddingHeight));

    // Right: Info & Buttons
    auto rightArea = pageArea;

    // Metadata Group
    auto metaArea = rightArea.removeFromTop(PresetMetaAreaHeight);

    gui.metaGroup.setBounds(metaArea);

    auto mRect = metaArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    mRect.removeFromTop(TitlePaddingTop);

    // Name
    auto row1 = mRect.removeFromTop(PresetMetaHeight).reduced(PresetMetaPaddingWidth, 0);
    gui.nameLabel.setBounds(row1.removeFromLeft(PresetMetaLabelWidth));
    gui.nameEditor.setBounds(row1);

    mRect.removeFromTop(PresetMetaPaddingHeight);

    // Author
    auto row2 = mRect.removeFromTop(PresetMetaHeight).reduced(PresetMetaPaddingWidth, 0);
    gui.authorLabel.setBounds(row2.removeFromLeft(PresetMetaLabelWidth));
    gui.authorEditor.setBounds(row2);

    mRect.removeFromTop(PresetMetaPaddingHeight);

    // Version
    auto row3 = mRect.removeFromTop(PresetMetaHeight).reduced(PresetMetaPaddingWidth, 0);
    gui.versionLabel.setBounds(row3.removeFromLeft(PresetMetaLabelWidth));
    gui.versionEditor.setBounds(row3);

    mRect.removeFromTop(PresetMetaPaddingHeight);

    // Comment
    auto row4 = mRect.removeFromTop(PresetMetaHeight).reduced(PresetMetaPaddingWidth, 0);

    gui.commentLabel.setBounds(row4);

    mRect.removeFromTop(PresetMetaPaddingHeight);

    auto row5 = mRect.reduced(PresetMetaPaddingWidth, 0);
    gui.commentEditor.setBounds(row5);

    rightArea.removeFromTop(PresetButtonsPaddingTop);

    // Buttons
    gui.initButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    gui.loadButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    gui.saveButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    gui.deleteButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    gui.refreshButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    gui.reflectButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    gui.copyButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
}
#endif

void AudioPlugin2686VEditor::layoutSettingsPage(SettingsGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    gui.group.setBounds(pageArea);

    auto sRect = pageArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    sRect.removeFromTop(TitlePaddingTop);

    // 1. Wallpaper
    auto row1 = sRect.removeFromTop(SettingsRowHeight);
    gui.wallpaperLabel.setBounds(row1.removeFromLeft(SettingsLabelWidth));
    gui.wallpaperClearBtn.setBounds(row1.removeFromRight(SettingsClearButtonWidth));
    gui.wallpaperBrowseBtn.setBounds(row1.removeFromRight(SettingsBrowseButtonWidth));
    gui.wallpaperPathLabel.setBounds(row1);

    sRect.removeFromTop(SettingsPaddingHeight);

    // 2. ADPCM Dir
    auto row2 = sRect.removeFromTop(SettingsRowHeight);
    gui.sampleDirLabel.setBounds(row2.removeFromLeft(SettingsLabelWidth));
    gui.sampleDirBrowseBtn.setBounds(row2.removeFromRight(SettingsBrowseButtonWidth));
    gui.sampleDirPathLabel.setBounds(row2);

    sRect.removeFromTop(SettingsPaddingHeight);

    // 3. Preset Dir
    auto row3 = sRect.removeFromTop(SettingsRowHeight);
    gui.presetDirLabel.setBounds(row3.removeFromLeft(SettingsLabelWidth));
    gui.presetDirBrowseBtn.setBounds(row3.removeFromRight(SettingsBrowseButtonWidth));
    gui.presetDirPathLabel.setBounds(row3);

    sRect.removeFromTop(SettingsPaddingHeight);

    // 4. Tooltip Visible Row
    auto rowTooltip = sRect.removeFromTop(SettingsRowHeight);
    gui.tooltipLabel.setBounds(rowTooltip.removeFromLeft(SettingsLongLabelWidth));
    gui.tooltipToggle.setBounds(rowTooltip.removeFromLeft(SettingsToggleWidth));

    sRect.removeFromTop(SettingsPaddingHeight);

    // Headroom Row
    auto rowHeadroom = sRect.removeFromTop(SettingsRowHeight);
    gui.useHeadroomLabel.setBounds(rowHeadroom.removeFromLeft(SettingsLongLabelWidth));
    gui.useHeadroomToggle.setBounds(rowHeadroom.removeFromLeft(SettingsToggleWidth));
    gui.headroomGainSlider.setBounds(rowHeadroom.removeFromLeft(SettingsHeadroomGainSliderWidth));

    sRect.removeFromTop(SettingsPaddingHeight);

    // 5. Config IO Buttons (Fixed Layout)
    auto row4 = sRect.removeFromTop(SettingsRowHeight);

    layoutComponentsLtoR(row4, OpRowHeight, OpLastRowPaddingBottom, {
        { &gui.loadSettingsBtn, { SettingsButtonWidth, SettingsButtonPaddingRight} },
        { &gui.saveSettingsBtn, { SettingsButtonWidth, SettingsButtonPaddingRight} },
        { &gui.saveStartupSettingsBtn, { SettingsButtonWidth, 0} }
    });
}

void AudioPlugin2686VEditor::layoutAboutPage(AboutGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();
    pageArea.removeFromTop(TitlePaddingTop);

    // Name & Ver
    gui.pluginNameLabel.setBounds(pageArea.removeFromTop(80));
    gui.versionLabel.setBounds(pageArea.removeFromTop(40));
    gui.copyrightLabel.setBounds(pageArea.removeFromTop(30));

    // Logo Row
    // ロゴの元サイズ比率を維持して中央配置
    auto logoArea = pageArea.removeFromTop(80);
    gui.vst3Logo.setBounds(logoArea.withSizeKeepingCentre(150, 60)); // 枠内で最大150x60に収める等の調整

    // VST Guideline Text
    gui.vstGuidelineLabel.setBounds(pageArea.removeFromTop(20));

    auto gplNoticeArea = pageArea.removeFromTop(40);

    gui.gplNoticeLabel.setBounds(gplNoticeArea);

    auto gplLinkArea = pageArea.removeFromTop(40);

    gui.gplLinkButton.setBounds(gplLinkArea);

}

#if !defined(BUILD_AS_FX_PLUGIN)
// ヘルパー: プリセットロード処理 (共通化)
void AudioPlugin2686VEditor::loadPresetFile(const juce::File& file)
{
    audioProcessor.loadPreset(file);

    presetGui.nameEditor.setText(audioProcessor.presetName);
    presetGui.authorEditor.setText(audioProcessor.presetAuthor);
    presetGui.versionEditor.setText(audioProcessor.presetVersion);

    // リズム音源ファイル名エリアに反映
    updateRhythmFileNames();

    // ADPCMファイル名エリアに反映
    if (audioProcessor.adpcmFilePath.isNotEmpty()) {
        adpcmGui.fileNameLabel.setText(juce::File(audioProcessor.adpcmFilePath).getFileName(), juce::dontSendNotification);
    }
    else {
        adpcmGui.fileNameLabel.setText("No File Loaded", juce::dontSendNotification);
    }
}

// ヘルパー: プリセット保存処理
void AudioPlugin2686VEditor::saveCurrentPreset()
{
    juce::String filename = audioProcessor.presetName.trim();
    if (filename.isEmpty()) filename = "Untitled";
    filename = filename + ".xml";

    juce::File saveFile = presetGui.currentFolder.getChildFile(filename);

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

void AudioPlugin2686VEditor::scanPresets()
{
    presetGui.items.clear();

    // XMLファイルを探す
    auto files = presetGui.currentFolder.findChildFiles(juce::File::findFiles, false, globFiles);

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

        presetGui.items.push_back(item);
    }

    // リスト更新
    presetGui.table.updateContent();
    presetGui.table.repaint();
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
            repaint(); // Trigger redraw
        }
    }
    else
    {
        backgroundImage = juce::Image(); // Null image
        repaint();
    }
}

void AudioPlugin2686VEditor::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
{
#if !defined(BUILD_AS_FX_PLUGIN)
    // wtPage のサイズが変わったときだけレイアウトを実行
    if (&component == &wtGui.page && wasResized)
    {
        auto content = tabs.getLocalBounds();
        content.removeFromTop(tabs.getTabBarDepth());
        content.reduce(GroupPaddingWidth, GroupPaddingHeight); // 全体の余白

        layoutWtPage(wtGui, content);
    }
    // もし gui.page も登録したなら
    // if (&component == &gui.page && wasResized) layoutgui.page(); // (関数化していれば)
#endif
}

void AudioPlugin2686VEditor::buttonClicked(juce::Button* button)
{
#if !defined(BUILD_AS_FX_PLUGIN)
    // ADPCM Load Buttons
    if (button == &adpcmGui.loadButton)
    {
        // ... (Existing ADPCM load logic) ...
        auto fileFilter = audioProcessor.formatManager.getWildcardForAllFormats();
        fileChooser = std::make_unique<juce::FileChooser>(openAudioFileDialogTitle, audioProcessor.lastSampleDirectory, fileFilter);
        auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    audioProcessor.loadAdpcmFile(file);
                    adpcmGui.fileNameLabel.setText(file.getFileName(), juce::dontSendNotification);
                    audioProcessor.lastSampleDirectory = file.getParentDirectory();
                }
            });
    }
    // ADPCM Pan Buttons
    else if (button == &adpcmGui.btnPanL) { adpcmGui.panSlider.setValue(0.0); }
    else if (button == &adpcmGui.btnPanC) { adpcmGui.panSlider.setValue(0.5); }
    else if (button == &adpcmGui.btnPanR) { adpcmGui.panSlider.setValue(1.0); }
    // Rhythm Pads Buttons
    else
    {
        for (int i = 0; i < 8; ++i)
        {
            auto& pad = rhythmGui.pads[i];

            if (button == &pad.loadButton)
            {
                auto fileFilter = audioProcessor.formatManager.getWildcardForAllFormats();
                fileChooser = std::make_unique<juce::FileChooser>(openAudioFileDialogTitle, audioProcessor.lastSampleDirectory, fileFilter);
                auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

                // Capture index 'i' by value
                fileChooser->launchAsync(folderChooserFlags, [this, i](const juce::FileChooser& fc)
                {
                        auto file = fc.getResult();
                        if (file.existsAsFile())
                        {
                            // Load to specific pad index
                            audioProcessor.loadRhythmFile(file, i);

                            // Update label
                            rhythmGui.pads[i].fileNameLabel.setText(file.getFileName(), juce::dontSendNotification);

                            audioProcessor.lastSampleDirectory = file.getParentDirectory();
                        }
                });

                return; // Found
            }
        }
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

void AudioPlugin2686VEditor::updateRhythmFileNames()
{
    for (int i = 0; i < 8; ++i)
    {
        juce::String path = audioProcessor.rhythmFilePaths[i];
        juce::String text = emptyFileName;

        if (path.isNotEmpty())
        {
            // パスが存在すればファイル名を取得
            // (resolvePathを使って絶対パス化してから名前を取得するのが確実です)
            juce::File f = audioProcessor.resolvePath(path);
            text = f.getFileName();
        }

        rhythmGui.pads[i].fileNameLabel.setText(text, juce::dontSendNotification);
    }
}

// ==============================================================================
// ADD: MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
template <typename T>
void AudioPlugin2686VEditor::applyMmlString(const juce::String& mml, T& gui, int opIndex)
{
    juce::String input = mml.toUpperCase();

    auto getValue = [&](const juce::String& key, int maxVal) -> int {
        int idx = input.indexOf(key);
        if (idx < 0) return -1;
        int valStart = idx + key.length();
        int valEnd = valStart;
        while (valEnd < input.length() && (juce::CharacterFunctions::isDigit(input[valEnd]))) {
            valEnd++;
        }
        if (valStart == valEnd) return -1;
        return input.substring(valStart, valEnd).getIntValue();
        };

    int val;

    // AR(Reverse)
    val = getValue(mmlPrefixRar, 31);
    if (val >= 0) gui.ar[opIndex].setValue(RegisterConverter::convertFmAr(31 - val), juce::sendNotification);
    // AR
    else {
        val = getValue(mmlPrefixAr, 31);
        if (val >= 0) gui.ar[opIndex].setValue(RegisterConverter::convertFmAr(val), juce::sendNotification);
    }

    // RR(Reverse)
    val = getValue(mmlPrefixRrr, 15);
    if (val >= 0) gui.rr[opIndex].setValue(RegisterConverter::convertFmRr(15 - val), juce::sendNotification);
    // RR
    else {
        val = getValue(mmlPrefixRr, 15);
        if (val >= 0) gui.rr[opIndex].setValue(RegisterConverter::convertFmRr(val), juce::sendNotification);
    }

    // Only for OPNA / OPN
    if constexpr (std::is_same<T, OpnaGuiSet>::value || std::is_same<T, OpnGuiSet>::value) {
        // DR
        val = getValue(mmlPrefixRdr, 31);
        if (val >= 0) gui.dr[opIndex].setValue(RegisterConverter::convertFmDr(31 - val), juce::sendNotification);
        // DR
        else
        {
            val = getValue(mmlPrefixDr, 31);
            if (val >= 0) gui.dr[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
        }

        val = getValue(mmlPrefixSl, 15);
        if (val >= 0) gui.sl[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

        // SR(Reverse)
        val = getValue(mmlPrefixRsr, 31);
        if (val >= 0) gui.sr[opIndex].setValue(RegisterConverter::convertFmSr(31 - val), juce::sendNotification);
        // SR
        else {
            val = getValue(mmlPrefixSr, 31);
            if (val >= 0) gui.sr[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
        }

        // DT
        val = getValue(mmlPrefixDt, 7);
        if (val >= 0) gui.dt[opIndex].setValue((double)val, juce::sendNotification);
    }
    // Only for OPM / OPZX3
    else if constexpr (std::is_same<T, OpmGuiSet>::value || std::is_same<T, Opzx3GuiSet>::value) {
        // D1R(Reverse)
        val = getValue(mmlPrefixRd1r, 31);
        if (val >= 0) gui.dr[opIndex].setValue(RegisterConverter::convertFmDr(31 - val), juce::sendNotification);
        // D1R
        else {
            val = getValue(mmlPrefixD1r, 31);
            if (val >= 0) gui.dr[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
        }

        // D1L
        val = getValue(mmlPrefixD1l, 15);
        if (val >= 0) gui.sl[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

        // D2R(Reverse)
        val = getValue(mmlPrefixRd2r, 15);
        if (val >= 0) gui.sr[opIndex].setValue(RegisterConverter::convertFmSr(15 - val), juce::sendNotification);
        // D2R
        else {
            val = getValue(mmlPrefixD2r, 15);
            if (val >= 0) gui.sr[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
        }

        // DT1
        val = getValue(mmlPrefixDt1, 7);
        if (val >= 0) gui.dt1[opIndex].setValue((double)val, juce::sendNotification);

        // DT2
        val = getValue(mmlPrefixDt2, 7);
        if (val >= 0) gui.dt2[opIndex].setValue((double)val, juce::sendNotification);
    }
    else {
        // DR
        val = getValue(mmlPrefixRdr, 31);
        if (val >= 0) gui.dr[opIndex].setValue(RegisterConverter::convertFmDr(31 - val), juce::sendNotification);
        // DR
        else
        {
            val = getValue(mmlPrefixDr, 31);
            if (val >= 0) gui.dr[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
        }

        val = getValue(mmlPrefixSl, 15);
        if (val >= 0) gui.sl[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);
    }

    // TL
    val = getValue(mmlPrefixTl, 127);
    if (val >= 0) gui.tl[opIndex].setValue(RegisterConverter::convertFmTl(val), juce::sendNotification);

    // MUL
    val = getValue(mmlPrefixMul, 15);
    if (val >= 0) gui.mul[opIndex].setValue((double)RegisterConverter::convertFmMul(val), juce::sendNotification);
}
#endif
