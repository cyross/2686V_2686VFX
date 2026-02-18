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

template<typename... Components>
void layoutComponentsTtoB(juce::Rectangle<int>& rect, int height, int space, Components*... components) {
    (components->setBounds(rect.removeFromTop(height)), ...);

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
        params.page.addAndMakeVisible(slider);

        // バーグラフ風の見た目にする
        slider.setSliderStyle(juce::Slider::LinearBarVertical);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setRange(-1.0, 1.0);
        slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::white.withAlpha(0.3f));

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

void AudioPlugin2686VEditor::setupOpnaGui(Fm4GuiSet& gui)
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
    setupFbSlider(mvParams);
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
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + postDt, "DT", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + postKs, "KS", ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postDr, "DR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postSl, "SL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + postSr, "SR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSr);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupComboParams seParams = SetupComboParams::createOp(gui.page, gui.se[i], gui.seLabel[i], gui.seAtt[i], paramPrefix + postSe, "SE", seItems);
        setupCombo(seParams);
        SetupSliderParams seFreqParams = SetupSliderParams::createOp(gui.page, gui.seFreq[i], gui.seFreqLabel[i], gui.seFreqAtt[i], paramPrefix + postSeFreq, "SSG Env Freq");
        setupSlider(seFreqParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + postFix, "FIX");
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + postFixFreq, "FRQ");
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + postFixFreqTo0, "Freq -> 0Hz", { 64, 15 }, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + postFixFreqTo440, "Freq -> 440Hz", { 64, 15 }, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        SetupToggleButtonParams amParams = SetupToggleButtonParams::createOp(gui.page, gui.am[i], gui.amAtt[i], paramPrefix + postAm, "AM");
        setupToggleButton(amParams);
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, "MASK");
        setupToggleButton(maskParams);
    }
}

void AudioPlugin2686VEditor::setupOpnGui(Fm4GuiSet& gui)
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
    setupFbSlider(mvParams);
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
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + postDt, "DT", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + postKs, "KS", ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postDr, "DR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postSl, "SL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + postSr, "SR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSr);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + postFix, "FIX");
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + postFixFreq, "FRQ");
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + postFixFreqTo0, "Freq -> 0Hz", { 64, 15 }, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + postFixFreqTo440, "Freq -> 440Hz", { 64, 15 }, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, "MASK");
        setupToggleButton(maskParams);
    }
}

void AudioPlugin2686VEditor::setupOplGui(Fm2GuiSet& gui)
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
    setupFbSlider(mvParams);
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
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + postDt, "DT", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postDr, "DR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postSl, "SL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupToggleButtonParams amParams = SetupToggleButtonParams::createOp(gui.page, gui.am[i], gui.amAtt[i], paramPrefix + postAm, "AM");
        setupToggleButton(amParams);
        SetupToggleButtonParams vibParams = SetupToggleButtonParams::createOp(gui.page, gui.vib[i], gui.vibAtt[i], paramPrefix + postVib, "VIB");
        setupToggleButton(vibParams);
        SetupToggleButtonParams egtParams = SetupToggleButtonParams::createOp(gui.page, gui.egType[i], gui.egTypeAtt[i], paramPrefix + postEgType, "EGTYPE");
        setupToggleButton(egtParams);
        SetupToggleButtonParams ksrParams = SetupToggleButtonParams::createOp(gui.page, gui.ksr[i], gui.ksrAtt[i], paramPrefix + postKsr, "KSR");
        setupToggleButton(ksrParams);
        SetupComboParams kslParams = SetupComboParams::createOp(gui.page, gui.ksl[i], gui.kslLabel[i], gui.kslAtt[i], paramPrefix + postKsl, "KSL", kslItems);
        setupCombo(kslParams);
        SetupComboParams egParams = SetupComboParams::createOp(gui.page, gui.eg[i], gui.egLabel[i], gui.egAtt[i], paramPrefix + postEg, "EG", egItems);
        setupCombo(egParams);
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, "MASK");
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
    setupFbSlider(mvParams);
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
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postDr, "DR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postSl, "SL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupToggleButtonParams amParams = SetupToggleButtonParams::createOp(gui.page, gui.am[i], gui.amAtt[i], paramPrefix + postAm, "AM");
        setupToggleButton(amParams);
        SetupToggleButtonParams vibParams = SetupToggleButtonParams::createOp(gui.page, gui.vib[i], gui.vibAtt[i], paramPrefix + postVib, "VIB");
        setupToggleButton(vibParams);
        SetupToggleButtonParams egtParams = SetupToggleButtonParams::createOp(gui.page, gui.egType[i], gui.egTypeAtt[i], paramPrefix + postEgType, "EGTYPE");
        setupToggleButton(egtParams);
        SetupToggleButtonParams ksrParams = SetupToggleButtonParams::createOp(gui.page, gui.ksr[i], gui.ksrAtt[i], paramPrefix + postKsr, "KSR");
        setupToggleButton(ksrParams);
        SetupComboParams kslParams = SetupComboParams::createOp(gui.page, gui.ksl[i], gui.kslLabel[i], gui.kslAtt[i], paramPrefix + postKsl, "KSL", kslItems);
        setupCombo(kslParams);
        SetupComboParams egParams = SetupComboParams::createOp(gui.page, gui.eg[i], gui.egLabel[i], gui.egAtt[i], paramPrefix + postEg, "EG", egItems);
        setupCombo(egParams);
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, "MASK");
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
    setupFbSlider(mvParams);
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
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt1[i], gui.dt1Label[i], gui.dt1Att[i], paramPrefix + postDt, "DT1", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams dt2Params = SetupSliderParams::createOp(gui.page, gui.dt2[i], gui.dt2Label[i], gui.dt2Att[i], paramPrefix + postDt2, "DT2", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt2);
        setupSlider(dt2Params);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + postKs, "KS", ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postD1r, "D1R", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postD1l, "D1L", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + postD2r, "D2R", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSr);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + postFix, "FIX");
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + postFixFreq, "FRQ");
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + postFixFreqTo0, "Freq -> 0Hz", { 64, 15 }, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + postFixFreqTo440, "Freq -> 440Hz", { 64, 15 }, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, "MASK");
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
    setupFbSlider(mvParams);
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

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + postMul, "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt1[i], gui.dt1Label[i], gui.dt1Att[i], paramPrefix + postDt, "DT1", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams dt2Params = SetupSliderParams::createOp(gui.page, gui.dt2[i], gui.dt2Label[i], gui.dt2Att[i], paramPrefix + postDt2, "DT2", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt2);
        setupSlider(dt2Params);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + postTl, "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + postKs, "KS", ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + postAr, "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmAr);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + postD1r, "D1R", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDr);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + postD1l, "D1L", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + postD2r, "D2R", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSr);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + postRr, "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRr);
        setupSlider(rrParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + postFix, "FIX");
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + postFixFreq, "FRQ");
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + postFixFreqTo0, "Freq -> 0Hz", { 64, 15 }, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + postFixFreqTo440, "Freq -> 440Hz", { 64, 15 }, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        SetupToggleButtonParams maskParams = SetupToggleButtonParams::createOp(gui.page, gui.mask[i], gui.maskAtt[i], paramPrefix + postMask, "MASK");
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
    setupFbSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, code + postBit, mBitTitle, bdItems);
    setupCombo(bParams);

    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, code + postRate, mRateTitle, rateItems);
    setupCombo(rtParams);

    SetupGroupParams vGroupParams = { .page = gui.page, .group = gui.voiceGroup, .title = "Voice" };
    setupGroup(vGroupParams);

    SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.waveSelector, gui.waveLabel, gui.waveAtt, code + postWaveform, "Wave Form", wsItems, ComboBoxSize, LabelSize, {true, true});
    setupCombo(wsParams);

    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, code + postLevel, "Tone", SliderSize, SliderValueSize, LabelSize, SliderFlags, RegisterType::SsgVol);
    setupSlider(lvParams);
    SetupSliderParams nsParams = SetupSliderParams::create(gui.page, gui.noiseSlider, gui.noiseLabel, gui.noiseAtt, code + postNoise, "Noise", SliderSize, SliderValueSize, LabelSize, SliderFlags, RegisterType::SsgVol);
    setupSlider(nsParams);
    SetupSliderParams nfParams = SetupSliderParams::create(gui.page, gui.noiseFreqSlider, gui.noiseFreqLabel, gui.noiseFreqAtt, code + postNoiseFreq, "Noise Freq");
    setupSlider(nfParams);
    SetupToggleButtonParams nonParams = SetupToggleButtonParams::create(gui.page, gui.noiseOnNoteButton, gui.noiseOnNoteAtt, code + postNoiseOnNote, "Noise On Note", { 120, 20 }, { true, true });
    setupToggleButton(nonParams);

    // KeyTrackがONのときは、Freqスライダーを無効化してあげると親切です
    gui.noiseOnNoteButton.onClick = [this, &gui] {
        bool isTrack = gui.noiseOnNoteButton.getToggleState();
        gui.noiseFreqSlider.setEnabled(!isTrack);
        gui.noiseFreqLabel.setEnabled(!isTrack);
    };

    // 初期状態反映
    bool isTrack = (bool)*audioProcessor.apvts.getRawParameterValue(code + postNoiseOnNote);
    gui.noiseFreqSlider.setEnabled(!isTrack);
    gui.noiseFreqLabel.setEnabled(!isTrack);
    SetupSliderParams mxParams = SetupSliderParams::create(gui.page, gui.mixSlider, gui.mixLabel, gui.mixAtt, code + postMix, "Mix (T<-M->N)");
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
    SetupToggleButtonParams bpParams = SetupToggleButtonParams::create(gui.page, gui.adsrBypassButton, gui.adsrBypassAtt, code + codeAdsr + postBypass, "Bypass ADSR (Gate)", { 160, 20 }, { true, true });
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
    SetupComboParams dmParams = SetupComboParams::create(gui.page, gui.dutyModeSelector, gui.dutyModeLabel, gui.dutyModeAtt, code + postDutyMode, "Duty Mode", dmItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(dmParams);
    SetupComboParams dpParams = SetupComboParams::create(gui.page, gui.dutyPresetSelector, gui.dutyPresetLabel, gui.dutyPresetAtt, code + postDutyPreset, "Duty Preset", prItems);
    setupCombo(dpParams);
    SetupSliderParams dvParams = SetupSliderParams::create(gui.page, gui.dutyVarSlider, gui.dutyVarLabel, gui.dutyVarAtt, code + postDutyVar, "Duty Ratio");
    setupSlider(dvParams);
    SetupToggleButtonParams dtiParams = SetupToggleButtonParams::create(gui.page, gui.dutyInvertButton, gui.dutyInvertAtt, code + postDutyInv, "Invert Phase", { 160, 20 });
    setupToggleButton(dtiParams);
    SetupGroupParams tpGroupParams = { .page = gui.page, .group = gui.triGroup, .title = "Triangle Property" };
    setupGroup(tpGroupParams);
    SetupToggleButtonParams tktParams = SetupToggleButtonParams::create(gui.page, gui.triKeyTrackButton, gui.triKeyTrackAtt, code + postTriKeyTrk, "Key Track (Pitch)", ToggleButtonSize, { true, true });
    setupToggleButton(tktParams);
    SetupSliderParams tfParams = SetupSliderParams::create(gui.page, gui.triFreqSlider, gui.triFreqLabel, gui.triFreqAtt, code + postTriFreq, "Manual Freq (Hz)");
    setupSlider(tfParams);
    SetupSliderParams tpParams = SetupSliderParams::create(gui.page, gui.triPeakSlider, gui.triPeakLabel, gui.triPeakAtt, code + postTriPeak, "Peak Position");
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
    SetupSliderParams epParams = SetupSliderParams::create(gui.page, gui.periodSlider, gui.periodLabel, gui.periodAtt, code + postEnvPeriod, "Env Speed (Hz)", SliderSize, SliderValueSize, LabelSize, SliderFlags, RegisterType::SsgEnv);
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

    SetupSliderParams mvParams = SetupSliderParams::create(gui.page, gui.masterVolSlider, gui.masterVolLabel, gui.masterVolAttachment, codeMasterVol, masterVolumeLabel);
    setupFbSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    // --- Property Group (Right) ---
    SetupGroupParams wpGroupParams = { .page = gui.page, .group = gui.propGroup, .title = "Wavetable Properties" };
    setupGroup(wpGroupParams);

    // Waveform Selector Setup
    SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.waveSelector, gui.waveLabel, gui.waveAtt, code + postWave, "Wave Form", wsItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(wsParams);

    // Custom Sliders Setup
    SetupGroupParams cwGroupParams = { .page = gui.page, .group = gui.customGroup, .title = "Custom Wave" };
    setupGroup(cwGroupParams);
	SetupCustomWaveTableParams cw32Params = { gui.page, gui.customSliders32, gui.customAtts32, code + codeCustom32 };
    setupCustomWaveTable(cw32Params);
    SetupCustomWaveTableParams cw64Params = { gui.page, gui.customSliders64, gui.customAtts64, code + codeCustom64 };
    setupCustomWaveTable(cw64Params);
    SetupComboParams szParams = SetupComboParams::create(gui.page, gui.sizeSelector, gui.sizeLabel, gui.sizeAtt, code + postSize, "Table Size", tsItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(szParams);
    SetupToggleButtonParams meParams = SetupToggleButtonParams::create(gui.page, gui.modEnableButton, gui.modEnableAtt, code + postModEnable, "Modulation ON", ToggleButtonSize, { true, true });
    setupToggleButton(meParams);
    SetupSliderParams mdParams = SetupSliderParams::create(gui.page, gui.modDepthSlider, gui.modDepthLabel, gui.modDepthAtt, code + postModDepth, "Mod Depth");
    setupSlider(mdParams);
    SetupSliderParams msParams = SetupSliderParams::create(gui.page, gui.modSpeedSlider, gui.modSpeedLabel, gui.modSpeedAtt, code + postModSpeed, "Mod Speed");
    setupSlider(msParams);
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
    setupFbSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, code + postLevel, "Vol");
    setupFbSlider(lvParams);

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
        SetupSliderParams rrP = SetupSliderParams::createOp(gui.page, pad.rrSlider, pad.rrLabel, pad.rrAtt, padPrefix + postRr, "RR");
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
    setupFbSlider(mvParams);
    gui.masterVolSlider.setTextValueSuffix(masterVolumeUnit); // 単位表示

    // 出力レベル
    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, code + postLevel, "Vol");
    setupFbSlider(lvParams);

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

    gui.table.getHeader().addColumn(presetTableFileNameColTitle, 1, 150);
    gui.table.getHeader().addColumn(presetTableModeColTitle, 5, 80);
    gui.table.getHeader().addColumn(presetTablePresetNameColTitle, 2, 150);
    gui.table.getHeader().addColumn(presetTableAuthorColTitle, 3, 100);
    gui.table.getHeader().addColumn(presetTableVersionColTitle, 4, 50);
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
    gui.nameLabel.setText("Name:", juce::dontSendNotification);
    gui.page.addAndMakeVisible(gui.nameEditor);
    gui.nameEditor.setText(audioProcessor.presetName);
    gui.nameEditor.onTextChange = [this] { audioProcessor.presetName = presetGui.nameEditor.getText(); };

    // Author
    gui.page.addAndMakeVisible(gui.authorLabel);
    gui.authorLabel.setText("Author:", juce::dontSendNotification);
    gui.page.addAndMakeVisible(gui.authorEditor);
    gui.authorEditor.setText(audioProcessor.presetAuthor);
    gui.authorEditor.onTextChange = [this] { audioProcessor.presetAuthor = presetGui.authorEditor.getText(); };

    // Version
    gui.page.addAndMakeVisible(gui.versionLabel);
    gui.versionLabel.setText("Ver:", juce::dontSendNotification);
    gui.page.addAndMakeVisible(gui.versionEditor);
    gui.versionEditor.setText(audioProcessor.presetVersion);
    gui.versionEditor.onTextChange = [this] { audioProcessor.presetVersion = presetGui.versionEditor.getText(); };

    // Comment
    gui.page.addAndMakeVisible(gui.commentLabel);
    gui.commentLabel.setText("Comment:", juce::dontSendNotification);
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
            juce::String info = "Preset Name: " + item.name + "\n" +
                "Author: " + item.author + "\n" +
                "Version: " + item.version + "\n" +
                "Comment: " + item.comment + "\n" +
                "Mode: " + item.modeName;

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
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);

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

        // ボタンを小さくするので文字サイズも調整
        juce::Font f(10.0f);
        for (auto* b : { &bDry, &bHalf, &bWet }) {
            b->setConnectedEdges(juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight); // 連結した見た目に
        }

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
    setupFbSlider(mvParams);
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
void AudioPlugin2686VEditor::layoutOpnaPage(Fm4GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { 50, 0} }, { &gui.bitSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { 50, 0} }, { &gui.rateCombo, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { 50, 0} }, { &gui.algSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { 50, 0} }, { &gui.feedbackSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedback2Label, { 50, 0} }, { &gui.feedback2Slider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.lfoFreqLabel, { 50, 0} }, { &gui.lfoFreqSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.lfoPmsLabel, { 50, 0} }, { &gui.lfoPmsSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.lfoAmsLabel, { 50, 0} }, { &gui.lfoAmsSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        gui.opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { 50, 0} }, { &gui.mul[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dtLabel[i], { 50, 0} }, { &gui.dt[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { 50, 0} }, { &gui.ar[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { 50, 0} }, { &gui.dr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.srLabel[i], { 50, 0} }, { &gui.sr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { 50, 0} }, { &gui.sl[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { 50, 0} }, { &gui.rr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { 50, 0} }, { &gui.tl[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.ksLabel[i], { 50, 0} }, { &gui.ks[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.seLabel[i], { 50, 0} }, { &gui.se[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.seFreqLabel[i], { 50, 0} }, { &gui.seFreq[i], { 190, 0} } });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.fix[i]);
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqLabel[i], { 50, 0} }, { &gui.freq[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqToZero[i], { 120, 0} }, { &gui.freqTo440[i], { 120, 0} } });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.mask[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpLastRowPaddingBottom, &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutOpnPage(Fm4GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { 50, 0} }, { &gui.bitSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { 50, 0} }, { &gui.rateCombo, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { 50, 0} }, { &gui.algSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { 50, 0} }, { &gui.feedbackSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedback2Label, { 50, 0} }, { &gui.feedback2Slider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea.reduced(2));

        // 枠線の内側
        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { 50, 0} }, { &gui.mul[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dtLabel[i], { 50, 0} }, { &gui.dt[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { 50, 0} }, { &gui.ar[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { 50, 0} }, { &gui.dr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.srLabel[i], { 50, 0} }, { &gui.sr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { 50, 0} }, { &gui.sl[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { 50, 0} }, { &gui.rr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { 50, 0} }, { &gui.tl[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.ksLabel[i], { 50, 0} }, { &gui.ks[i], { 190, 0} } });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.fix[i]);
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqLabel[i], { 50, 0} }, { &gui.freq[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqToZero[i], { 120, 0} }, { &gui.freqTo440[i], { 120, 0} } });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.mask[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpLastRowPaddingBottom, &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutOplPage(Fm2GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { 50, 0} }, { &gui.bitSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { 50, 0} }, { &gui.rateCombo, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { 50, 0} }, { &gui.algSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { 50, 0} }, { &gui.feedbackSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

    // --- B. Operators Section ---
    for (int i = 0; i < 2; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);
        gui.opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { 50, 0} }, { &gui.mul[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dtLabel[i], { 50, 0} }, { &gui.dt[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { 50, 0} }, { &gui.ar[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { 50, 0} }, { &gui.dr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { 50, 0} }, { &gui.sl[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { 50, 0} }, { &gui.rr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { 50, 0} }, { &gui.tl[i], { 190, 0}} });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.am[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.vib[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.egType[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.ksr[i]);
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.kslLabel[i], { 50, 0} }, { &gui.ksl[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.egLabel[i], { 50, 0} }, { &gui.eg[i], { 190, 0} } });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.mask[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpLastRowPaddingBottom, &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutOpl3Page(Opl3GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { 50, 0} }, { &gui.bitSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { 50, 0} }, { &gui.rateCombo, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { 50, 0} }, { &gui.algSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { 50, 0} }, { &gui.feedbackSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedback2Label, { 50, 0} }, { &gui.feedback2Slider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

    // --- Operators Section ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);
        gui.opGroups[i].setBounds(opArea);

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { 50, 0} }, { &gui.mul[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { 50, 0} }, { &gui.ar[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { 50, 0} }, { &gui.dr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { 50, 0} }, { &gui.sl[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { 50, 0} }, { &gui.rr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { 50, 0} }, { &gui.tl[i], { 190, 0}} });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.am[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.vib[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.egType[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.ksr[i]);
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.kslLabel[i], { 50, 0} }, { &gui.ksl[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.egLabel[i], { 50, 0} }, { &gui.eg[i], { 190, 0} } });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.mask[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpLastRowPaddingBottom, &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutOpmPage(OpmGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { 50, 0} }, { &gui.bitSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { 50, 0} }, { &gui.rateCombo, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { 50, 0} }, { &gui.algSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { 50, 0} }, { &gui.feedbackSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedback2Label, { 50, 0} }, { &gui.feedback2Slider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.lfoFreqLabel, { 50, 0} }, { &gui.lfoFreqSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.pmsLabel, { 50, 0} }, { &gui.pmsSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.amsLabel, { 50, 0} }, { &gui.amsSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { 50, 0} }, { &gui.mul[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dt1Label[i], { 50, 0} }, { &gui.dt1[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dt2Label[i], { 50, 0} }, { &gui.dt2[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { 50, 0} }, { &gui.ar[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { 50, 0} }, { &gui.dr[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.srLabel[i], { 50, 0} }, { &gui.sr[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { 50, 0} }, { &gui.sl[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { 50, 0} }, { &gui.rr[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { 50, 0} }, { &gui.tl[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.ksLabel[i], { 50, 0} }, { &gui.ks[i], { 190, 0} } });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.fix[i]);
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqLabel[i], { 50, 0} }, { &gui.freq[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqToZero[i], { 120, 0} }, { &gui.freqTo440[i], { 120, 0} } });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.mask[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpLastRowPaddingBottom, &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutOpzx3Page(Opzx3GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { 50, 0} }, { &gui.bitSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { 50, 0} }, { &gui.rateCombo, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.algLabel, { 50, 0} }, { &gui.algSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedbackLabel, { 50, 0} }, { &gui.feedbackSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.feedback2Label, { 50, 0} }, { &gui.feedback2Slider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.lfoFreqLabel, { 50, 0} }, { &gui.lfoFreqSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.pmsLabel, { 50, 0} }, { &gui.pmsSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.amsLabel, { 50, 0} }, { &gui.amsSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mulLabel[i], { 50, 0} }, { &gui.mul[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dt1Label[i], { 50, 0} }, { &gui.dt1[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dt2Label[i], { 50, 0} }, { &gui.dt2[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.arLabel[i], { 50, 0} }, { &gui.ar[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.drLabel[i], { 50, 0} }, { &gui.dr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.srLabel[i], { 50, 0} }, { &gui.sr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.slLabel[i], { 50, 0} }, { &gui.sl[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.rrLabel[i], { 50, 0} }, { &gui.rr[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.tlLabel[i], { 50, 0} }, { &gui.tl[i], { 190, 0}} });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.ksLabel[i], { 50, 0} }, { &gui.ks[i], { 190, 0} } });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.fix[i]);
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqLabel[i], { 50, 0} }, { &gui.freq[i], { 190, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.freqToZero[i], { 120, 0} }, { &gui.freqTo440[i], { 120, 0} } });
        layoutComponentsLtoR(innerRect, OpRowHeight, OpRowPaddingBottom, { { &gui.wsLabel[i], { 50, 0} }, { &gui.ws[i], { 190, 0} } });
        layoutComponentsTtoB(innerRect, OpRowHeight, OpRowPaddingBottom, &gui.mask[i]);
        layoutComponentsTtoB(innerRect, OpRowHeight, OpLastRowPaddingBottom, &gui.mmlBtn[i]);
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

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { 50, 0} }, { &gui.bitSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { 50, 0} }, { &gui.rateCombo, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.adsrBypassButton, { 240, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.attackLabel, { 50, 0} }, { &gui.attackSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.decayLabel, { 50, 0} }, { &gui.decaySlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.sustainLabel, { 50, 0} }, { &gui.sustainSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.releaseLabel, { 50, 0} }, { &gui.releaseSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

    // --- Voice Group ---
    auto voiceArea = pageArea.removeFromLeft(FmOpWidth);

    gui.voiceGroup.setBounds(voiceArea);
    auto vRect = gui.voiceGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    vRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.levelLabel, { 50, 0} }, { &gui.levelSlider, { 190, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.noiseLabel, { 50, 0} }, { &gui.noiseSlider, { 190, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.noiseFreqLabel, { 50, 0} }, { &gui.noiseFreqSlider, { 190, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.noiseOnNoteButton, { 240, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mixLabel, { 50, 0} }, { &gui.mixSlider, { 190, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &gui.mixSetTone, { 80, 0} }, { &gui.mixSetMix, { 80, 0} }, { &gui.mixSetNoise, { 80, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.waveLabel, { 50, 0} }, { &gui.waveSelector, { 190, 0} } });

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

        layoutComponentsLtoR(dRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dutyModeLabel, { 50, 0} }, { &gui.dutyModeSelector, { 190, 0} } });
        layoutComponentsLtoR(dRect, OpRowHeight, OpRowPaddingBottom, { { &gui.dutyInvertButton, { 240, 0} } });

        float dutyModeVal = *audioProcessor.apvts.getRawParameterValue(code + postDutyMode);
        if (dutyModeVal < 0.5f) {
            gui.dutyPresetSelector.setVisible(true);
            gui.dutyPresetLabel.setVisible(true);
            gui.dutyVarSlider.setVisible(false);
            gui.dutyVarLabel.setVisible(false);
            layoutComponentsLtoR(dRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.dutyPresetLabel, { 50, 0} }, { &gui.dutyPresetSelector, { 240, 0} } });
        }
        else {
            gui.dutyPresetSelector.setVisible(false);
            gui.dutyPresetLabel.setVisible(false);
            gui.dutyVarSlider.setVisible(true);
            gui.dutyVarLabel.setVisible(true);
            layoutComponentsLtoR(dRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.dutyVarLabel, { 50, 0} }, { &gui.dutyVarSlider, { 240, 0} } });
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

        layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &gui.triKeyTrackButton, { 240, 0} } });

        bool isKeyTrack = gui.triKeyTrackButton.getToggleState();
        gui.triFreqSlider.setVisible(!isKeyTrack);
        gui.triFreqLabel.setVisible(!isKeyTrack);

        if (!isKeyTrack) {
            layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &gui.triFreqLabel, { 50, 0} }, { &gui.triFreqSlider, { 190, 0} } });
            layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &gui.triPeakLabel, { 50, 0} }, { &gui.triPeakSlider, { 190, 0} } });
        }
        else {
            layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &gui.triPeakLabel, { 50, 0} }, { &gui.triPeakSlider, { 190, 0} } });
        }

        layoutComponentsLtoR(tRect, OpRowHeight, OpLastRowPaddingBottom, { { &gui.triSetSawDown, { 80, 0} }, { &gui.triSetTri, { 80, 0} }, { &gui.triSetSawUp, { 80, 0} } });
    }

    // HW Env Group
    auto envArea = pageArea.removeFromLeft(FmOpWidth);
    gui.envGroup.setBounds(envArea);
    auto eRect = gui.envGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    eRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(eRect, OpRowHeight, OpRowPaddingBottom, { { &gui.envEnableButton, { 240, 0} } });
    layoutComponentsLtoR(eRect, OpRowHeight, OpRowPaddingBottom, { { &gui.shapeLabel, { 50, 0} }, { &gui.shapeSelector, { 190, 0} } });
    layoutComponentsLtoR(eRect, OpRowHeight, OpRowPaddingBottom, { { &gui.periodLabel, { 50, 0} }, { &gui.periodSlider, { 190, 0} } });
}

void AudioPlugin2686VEditor::layoutWtPage(WtGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bitLabel, { 50, 0} }, { &gui.bitSelector, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { 50, 0} }, { &gui.rateCombo, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.levelLabel, { 50, 0}}, { &gui.levelSlider, { 190, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.attackLabel, { 50, 0}}, { &gui.attackSlider, { 190, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.decayLabel, { 50, 0}}, { &gui.decaySlider, { 190, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.sustainLabel, { 50, 0}}, { &gui.sustainSlider, { 190, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.releaseLabel, { 50, 0}}, { &gui.releaseSlider, { 190, 0}} });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

    // Right: Properties
    auto rightArea = pageArea.removeFromLeft(WtRightWidth);
    gui.propGroup.setBounds(rightArea);
    auto pRect = gui.propGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);
    pRect.removeFromTop(TitlePaddingTop);

    // Waveform Selector (Top)
    gui.waveSelector.setBounds(pRect.removeFromTop(30).withSizeKeepingCentre(200, 24));

    // Bit & Size (Middle)
    auto configRow = pRect.removeFromTop(30);
    int configRowWidth = 160;
    gui.sizeSelector.setBounds(configRow.removeFromLeft(configRowWidth));

    // Check if Custom Mode is selected (Index 8 -> ID 9 based on addItem?)
    // 以前のコードで: addItem("Custom (Draw)", 9); としたので ID=9 です。
    // getSelectedId() を使うのが確実です。
    bool isCustom = (gui.waveSelector.getSelectedId() == 9);
    int waveSize = 32;
    if (gui.sizeSelector.getSelectedId() == 2) waveSize = 64;

    gui.customGroup.setVisible(isCustom);
    for (auto& s : gui.customSliders32) s.setVisible(false);
    for (auto& s : gui.customSliders64) s.setVisible(false);

    if (isCustom)
    {
        // Custom Mode Layout
        // 中央に32本のスライダーを配置
        auto customArea = pRect.removeFromTop(120); // 高さ確保
        gui.customGroup.setBounds(customArea);

        auto sliderRect = customArea.reduced(10, 20);

        if (waveSize == 32)
        {
            for (auto& s : gui.customSliders32) s.setVisible(true);

            int sliderW = WtCustomSliderWidth;
            for (int i = 0; i < 32; ++i)
            {
                gui.customSliders32[i].setBounds(sliderRect.removeFromLeft(sliderW));
            }

        }
        else if (waveSize == 64)
        {
            for (auto& s : gui.customSliders64) s.setVisible(true);

            int sliderW = WtCustomSliderWidth / 2;
            for (int i = 0; i < 64; ++i)
            {
                gui.customSliders64[i].setBounds(sliderRect.removeFromLeft(sliderW));
            }

        }
    }
    else
    {
        // Custom以外ならスペースを空けない（詰める）
        // 何もしない
    }

    // Modulation
    gui.modEnableButton.setBounds(pRect.removeFromTop(30).withSizeKeepingCentre(150, 30));

    bool isMod = gui.modEnableButton.getToggleState();
    gui.modDepthSlider.setEnabled(isMod); gui.modDepthLabel.setEnabled(isMod);
    gui.modSpeedSlider.setEnabled(isMod); gui.modSpeedLabel.setEnabled(isMod);

    // Mod Sliders
    auto modRow = pRect.removeFromTop(100);
    gui.modDepthSlider.setBounds(modRow.removeFromLeft(modRow.getWidth() / 2).withSizeKeepingCentre(100, 100));
    gui.modSpeedSlider.setBounds(modRow.withSizeKeepingCentre(100, 100));
}

void AudioPlugin2686VEditor::layoutRhythmPage(RhythmGuiSet& gui, juce::Rectangle<int> content)
{
    auto applyPads = [&](juce::Rectangle<int>& area, int width, int start, int length)
        {
            area.removeFromTop(TitlePaddingTop);

            for (int i = start; i < start + length; ++i)
            {
                auto& pad = gui.pads[i];
                auto padArea = area.removeFromLeft(width);

                pad.group.setBounds(padArea);

                layoutComponentsLtoR(padArea, OpRowHeight, OpRowPaddingBottom, { { &pad.loadButton, { 50, 0}}, { &pad.fileNameLabel, { 150, 0}},{ &pad.clearButton, { 30, 0}}, });
                layoutComponentsTtoB(padArea, OpRowHeight, OpRowPaddingBottom, &pad.oneShotButton);
                layoutComponentsLtoR(padArea, OpRowHeight, OpRowPaddingBottom, { { &pad.noteLabel, { 50, 0}}, { &pad.noteSlider, { 190, 0}} });
                layoutComponentsLtoR(padArea, OpRowHeight, OpRowPaddingBottom, { { &pad.modeLabel, { 50, 0}}, { &pad.modeCombo, { 190, 0}} });
                layoutComponentsLtoR(padArea, OpRowHeight, OpRowPaddingBottom, { { &pad.rateLabel, { 50, 0}}, { &pad.rateCombo, { 190, 0}} });
                layoutComponentsLtoR(padArea, OpRowHeight, OpRowPaddingBottom, { { &pad.panLabel, { 50, 0}}, { &pad.panSlider, { 190, 0}} });
                layoutComponentsLtoR(padArea, OpRowHeight, OpRowPaddingBottom, { { &pad.btnPanL, { 80, 0}}, { &pad.btnPanC, { 80, 0}}, { &pad.btnPanR, { 80, 0}} });
                layoutComponentsTtoB(padArea, OpRowHeight, OpRowPaddingBottom, &pad.volLabel, &pad.volSlider);
                layoutComponentsTtoB(padArea, OpRowHeight, OpRowPaddingBottom, &pad.rrLabel, &pad.rrSlider);
            }

        };

    // Top section for Master Volume
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.levelLabel, { 50, 0} }, { &gui.levelSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

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

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.modeLabel, { 50, 0} }, { &gui.modeCombo, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.rateLabel, { 50, 0} }, { &gui.rateCombo, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.levelLabel, { 50, 0} }, { &gui.levelSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.loopButton, { 240, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.panLabel, { 50, 0} }, { &gui.panSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.btnPanL, { 80, 0}}, { &gui.btnPanC, { 80, 0}}, { &gui.btnPanR, { 80, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.attackLabel, { 50, 0} }, { &gui.attackSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.decayLabel, { 50, 0} }, { &gui.decaySlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.sustainLabel, { 50, 0} }, { &gui.sustainSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.releaseLabel, { 50, 0} }, { &gui.releaseSlider, { 190, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

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
    auto layoutFooterArea = [&](juce::Rectangle<int>& area, juce::ToggleButton& bypassBtn)
    {
        auto header = area.removeFromTop(TitlePaddingTop);

        bypassBtn.setBounds(header);
    };

    auto layoutKnob = [&](juce::Rectangle<int> r, juce::Slider& s, juce::Label& l) {
        auto labelArea = r.removeFromTop(20);
        l.setBounds(labelArea);

        auto knobArea = r.removeFromTop(FxKnobHeight);
        s.setBounds(knobArea.withSizeKeepingCentre(FxKnobWidth, knobArea.getHeight()));

        // Mixボタンの空白を開けるため
        r.removeFromTop(5);
        r.removeFromTop(20);
    };

    auto layoutMixSection = [&](juce::Rectangle<int> r, juce::Slider& s, juce::Label& l,
        juce::TextButton& b1, juce::TextButton& b2, juce::TextButton& b3)
    {
        // 上部: ラベル
        auto labelArea = r.removeFromTop(20);
        l.setBounds(labelArea);

        // 中間: スライダー
        auto knobArea = r.removeFromTop(FxKnobHeight);
        s.setBounds(knobArea.withSizeKeepingCentre(FxKnobWidth, knobArea.getHeight()));

        r.removeFromTop(5);

        // 下部: ボタンエリア (高さ20px)
        auto btnArea = r.removeFromTop(20);

        b1.setBounds(btnArea.removeFromLeft(FxMixButtonWidth).withSizeKeepingCentre(FxMixButtonWidth, btnArea.getHeight())); // 左側
        b3.setBounds(btnArea.removeFromRight(FxMixButtonWidth).withSizeKeepingCentre(FxMixButtonWidth, btnArea.getHeight())); // 右側
        b2.setBounds(btnArea.withSizeKeepingCentre(FxMixButtonWidth, btnArea.getHeight())); // 中央
    };

    auto layoutCombo = [&](juce::Rectangle<int> r, juce::ComboBox& c, juce::Label& l) {
        auto labelArea = r.removeFromTop(20);
        l.setBounds(labelArea);

        auto comboArea = r.removeFromTop(FxKnobHeight);
        c.setBounds(comboArea.withSizeKeepingCentre(100, 24).translated(0, (FxKnobHeight - 24) / 2));

        r.removeFromTop(5);
        r.removeFromTop(20);
    };

    auto pageArea = content.withZeroOrigin();
       
    auto mainArea = pageArea.removeFromLeft(MainWidth);

    gui.mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &gui.bypassToggle, { 240, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &gui.masterVolLabel, { 50, 0} }, { &gui.masterVolSlider, { 190, 0} } });

    auto fxArea = pageArea.withSizeKeepingCentre(FxWidth, FxHeight);

    auto leftCol = fxArea.removeFromLeft(fxArea.getWidth() / 2 - 5);
    auto rightCol = fxArea.removeFromRight(leftCol.getWidth()); // 残りの右側

    // Left

    // 1. Tremolo
    auto tremoloArea = leftCol.removeFromTop(FxGroupHeight);
    gui.tremGroup.setBounds(tremoloArea);

    layoutKnob(tremoloArea.removeFromLeft(FxKnobWidth), gui.tRateSlider, gui.tRateLabel);
    layoutMixSection(tremoloArea.removeFromRight(FxKnobAreaWidth), gui.tMixSlider, gui.tMixLabel, gui.tDryBtn, gui.tHalfBtn, gui.tWetBtn);
    auto tremoloFooterArea = tremoloArea.removeFromBottom(30);
    layoutKnob(tremoloArea.withSizeKeepingCentre(FxKnobWidth, tremoloArea.getHeight()), gui.tDepthSlider, gui.tDepthLabel);
    layoutFooterArea(tremoloFooterArea, gui.tBypassBtn);

    // 2. Vibrato
    auto vibratoArea = leftCol.removeFromTop(FxGroupHeight);
    gui.vibGroup.setBounds(vibratoArea);

    layoutKnob(vibratoArea.removeFromLeft(FxKnobWidth + 20), gui.vRateSlider, gui.vRateLabel);
    layoutMixSection(vibratoArea.removeFromRight(FxKnobAreaWidth), gui.vMixSlider, gui.vMixLabel, gui.vDryBtn, gui.vHalfBtn, gui.vWetBtn);
    layoutKnob(vibratoArea.withSizeKeepingCentre(FxKnobWidth, vibratoArea.getHeight()), gui.vDepthSlider, gui.vDepthLabel);
    auto vibratoFooterArea = vibratoArea.removeFromBottom(30);
    layoutFooterArea(vibratoFooterArea, gui.vBypassBtn);

    // 3. Modern Bit Crusher
    auto mbcArea = leftCol.removeFromTop(FxGroupHeight);
    gui.mbcGroup.setBounds(mbcArea);

    layoutKnob(mbcArea.removeFromLeft(FxKnobWidth + 20), gui.mbcRateSlider, gui.mbcRateLabel);
    layoutMixSection(mbcArea.removeFromRight(FxKnobAreaWidth), gui.mbcMixSlider, gui.mbcMixLabel, gui.mbcDryBtn, gui.mbcHalfBtn, gui.mbcWetBtn);
    layoutKnob(mbcArea.withSizeKeepingCentre(FxKnobWidth, mbcArea.getHeight()), gui.mbcBitsSlider, gui.mbcBitsLabel);
    auto mbcFooterArea = mbcArea.removeFromBottom(30);
    layoutFooterArea(mbcFooterArea, gui.mbcBypassBtn);

    // Right

    // 4. Delay
    auto delayArea = rightCol.removeFromTop(FxGroupHeight);
    gui.delayGroup.setBounds(delayArea);

    layoutKnob(delayArea.removeFromLeft(FxKnobWidth + 20), gui.dTimeSlider, gui.dTimeLabel);
    layoutMixSection(delayArea.removeFromRight(FxKnobAreaWidth), gui.dMixSlider, gui.dMixLabel, gui.dDryBtn, gui.dHalfBtn, gui.dWetBtn);
    layoutKnob(delayArea.withSizeKeepingCentre(FxKnobWidth, delayArea.getHeight()), gui.dFbSlider, gui.dFbLabel);
    auto delayFooterArea = delayArea.removeFromBottom(30);
    layoutFooterArea(delayFooterArea, gui.dBypassBtn);

    // 5. Reverb
    auto reverbArea = rightCol.removeFromTop(FxGroupHeight);
    gui.reverbGroup.setBounds(reverbArea);

    layoutKnob(reverbArea.removeFromLeft(FxKnobWidth + 20), gui.rSizeSlider, gui.rSizeLabel);
    layoutMixSection(reverbArea.removeFromRight(FxKnobAreaWidth), gui.rMixSlider, gui.rMixLabel, gui.rDryBtn, gui.rHalfBtn, gui.rWetBtn);
    layoutKnob(reverbArea.withSizeKeepingCentre(FxKnobWidth, reverbArea.getHeight() - 30), gui.rDampSlider, gui.rDampLabel);
    auto reverbFooterArea = reverbArea.removeFromBottom(30);
    layoutFooterArea(reverbFooterArea, gui.rBypassBtn);

    // 6. Retro Bit Crusher
    auto rbcArea = rightCol.removeFromTop(FxGroupHeight);
    gui.rbcGroup.setBounds(rbcArea);

    layoutCombo(rbcArea.removeFromLeft(FxKnobWidth + 20), gui.rbcBitsCombo, gui.rbcBitsLabel);
    layoutMixSection(rbcArea.removeFromRight(FxKnobAreaWidth), gui.rbcMixSlider, gui.rbcMixLabel, gui.rbcDryBtn, gui.rbcHalfBtn, gui.rbcWetBtn);
    layoutCombo(rbcArea.withSizeKeepingCentre(FxKnobWidth, rbcArea.getHeight()), gui.rbcRateCombo, gui.rbcRateLabel);
    auto rbcFooterArea = rbcArea.removeFromBottom(30);
    layoutFooterArea(rbcFooterArea, gui.rbcBypassBtn);
}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686VEditor::layoutPresetPage(PresetGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // Path Label (Top)
    gui.pathLabel.setBounds(pageArea.removeFromTop(24));

    // Left: List (60%)
    auto listArea = pageArea.removeFromLeft(pageArea.getWidth() * 0.6);
    gui.table.setBounds(listArea);

    // Right: Info & Buttons
    auto rightArea = pageArea;

    // Metadata Group
    gui.metaGroup.setBounds(rightArea.removeFromTop(320));
    auto metaRect = gui.metaGroup.getBounds();
    metaRect.removeFromTop(TitlePaddingTop);

    int rowH = 30;

    // Name
    auto row1 = metaRect.removeFromTop(rowH);
    gui.nameLabel.setBounds(row1.removeFromLeft(50));
    gui.nameEditor.setBounds(row1);
    metaRect.removeFromTop(10);

    // Author
    auto row2 = metaRect.removeFromTop(rowH);
    gui.authorLabel.setBounds(row2.removeFromLeft(50));
    gui.authorEditor.setBounds(row2);
    metaRect.removeFromTop(10);

    // Version
    auto row3 = metaRect.removeFromTop(rowH);
    gui.versionLabel.setBounds(row3.removeFromLeft(50));
    gui.versionEditor.setBounds(row3);

    // Comment
    auto row4 = metaRect.removeFromTop(rowH);
    gui.commentLabel.setBounds(row4);
    gui.commentEditor.setBounds(metaRect);

    rightArea.removeFromTop(20);

    // Buttons
    gui.initButton.setBounds(rightArea.removeFromTop(40));
    rightArea.removeFromTop(10);
    gui.loadButton.setBounds(rightArea.removeFromTop(40));
    rightArea.removeFromTop(10);
    gui.saveButton.setBounds(rightArea.removeFromTop(40));
    rightArea.removeFromTop(10);
    gui.deleteButton.setBounds(rightArea.removeFromTop(40));
    rightArea.removeFromTop(10);
    gui.refreshButton.setBounds(rightArea.removeFromTop(40));
    rightArea.removeFromTop(10);
    gui.reflectButton.setBounds(rightArea.removeFromTop(40));
    rightArea.removeFromTop(10);
    gui.copyButton.setBounds(rightArea.removeFromTop(40));
}
#endif

void AudioPlugin2686VEditor::layoutSettingsPage(SettingsGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    gui.group.setBounds(pageArea);
    pageArea.removeFromTop(TitlePaddingTop);

    int rowH = 30;
    int gap = 15;

    // 1. Wallpaper
    auto row1 = pageArea.removeFromTop(rowH);
    gui.wallpaperLabel.setBounds(row1.removeFromLeft(80));
    gui.wallpaperClearBtn.setBounds(row1.removeFromRight(60).reduced(2));
    gui.wallpaperBrowseBtn.setBounds(row1.removeFromRight(80).reduced(2));
    gui.wallpaperPathLabel.setBounds(row1.reduced(5, 0));

    // 2. ADPCM Dir
    auto row2 = pageArea.removeFromTop(rowH);
    gui.sampleDirLabel.setBounds(row2.removeFromLeft(80));
    gui.sampleDirBrowseBtn.setBounds(row2.removeFromRight(80).reduced(2));
    gui.sampleDirPathLabel.setBounds(row2.reduced(5, 0));

    // 3. Preset Dir
    auto row3 = pageArea.removeFromTop(rowH);
    gui.presetDirLabel.setBounds(row3.removeFromLeft(80));
    gui.presetDirBrowseBtn.setBounds(row3.removeFromRight(80).reduced(2));
    gui.presetDirPathLabel.setBounds(row3.reduced(5, 0));

    // 4. Tooltip Visible Row
    auto rowTooltip = pageArea.removeFromTop(rowH);
    gui.tooltipLabel.setBounds(rowTooltip.removeFromLeft(140));
    gui.tooltipToggle.setBounds(rowTooltip.removeFromLeft(60));

    // Headroom Row
    auto rowHeadroom = pageArea.removeFromTop(30);
    gui.useHeadroomLabel.setBounds(rowHeadroom.removeFromLeft(140));
    gui.useHeadroomToggle.setBounds(rowHeadroom.removeFromLeft(80)); // Enableスイッチ
    // スライダー
    gui.headroomGainSlider.setBounds(rowHeadroom.removeFromLeft(200));

    // 5. Config IO Buttons (Fixed Layout)
    auto row4 = pageArea.removeFromTop(40);

    int btnW = 120;     // Load / Save 幅
    int defBtnW = 240;  // Default Save 幅
    int btnGap = 20;    // ボタン間の隙間

    int totalBtnWidth = 480;

    // 行の中央に配置するためのエリアを作成 (高さ30pxで統一)
    auto btnArea = row4.withSizeKeepingCentre(totalBtnWidth, row4.getHeight());

    // 左から順に切り取って配置
    gui.loadSettingsBtn.setBounds(btnArea.removeFromLeft(btnW));
    btnArea.removeFromLeft(btnGap);
    gui.saveSettingsBtn.setBounds(btnArea.removeFromRight(btnW));

    auto row5 = pageArea.removeFromTop(40);
    auto btn2Area = row5.withSizeKeepingCentre(totalBtnWidth, row5.getHeight());

    btn2Area.removeFromLeft((totalBtnWidth - defBtnW) / 2);

    gui.saveStartupSettingsBtn.setBounds(btn2Area.removeFromLeft(defBtnW));
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
    if constexpr (std::is_same<T, Fm4GuiSet>::value) {
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
