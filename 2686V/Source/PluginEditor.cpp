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
    std::initializer_list<std::pair<juce::Component*, int>> components)
{
    auto area = rect.removeFromTop(height);

    for (const auto& comp : components)
    {
        if (comp.first != nullptr) // 安全のためのチェック
        {
            comp.first->setBounds(area.removeFromLeft(comp.second));
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

    wtGui.page.addComponentListener(this);
    tabs.getTabbedButtonBar().addChangeListener(this);
    audioProcessor.apvts.addParameterListener("MODE", this);

    setupLogo();

    // ============================
    // FM Pages
    // ============================
    // OPNA (YM2608)
    setupOpnaGui(opnaGui);

    // OPN (YM2203)
    setupOpnGui(opnGui);

	// OPL (YM3812) : 2op
    setupOplGui(oplGui);

	// OPLL (YM2413) : 2op
	setupOpllGui(opllGui);

	// OPL3 (YM3833) : 2op
	setupOpl3Gui(opl3Gui);

	// OPM (YM2151) : 4op
	setupOpmGui(opmGui);

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
    // FX Page
    // ============================
    setupFxGui(fxGui);

    // ============================
    // Preset Page
    // ============================
    setupPresetGui(presetGui);

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

    int currentMode = (int)*audioProcessor.apvts.getRawParameterValue("MODE");
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

    // スライダーの設定
    masterVolLabel.setText("Master Volume", juce::NotificationType::dontSendNotification);
    masterVolLabel.setFont(juce::Font(24.0f));
    masterVolLabel.setSize(100, 30);
    masterVolLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    masterVolLabel.setColour(juce::Label::backgroundColourId, juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 0.4f));
    addAndMakeVisible(masterVolLabel);

    masterVolSlider.setSliderStyle(juce::Slider::LinearHorizontal); // または Rotary
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 30);
    masterVolSlider.setTextValueSuffix(" dB"); // 単位表示
    addAndMakeVisible(masterVolSlider);

    // パラメータと接続 (IDは "MASTER_VOL")
    // これを各タブのクラスで行えば、複数のスライダーが1つのパラメータを操作します
    masterVolAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MASTER_VOL", masterVolSlider);

    setSize(WindowWidth, WindowHeight);
}

AudioPlugin2686VEditor::~AudioPlugin2686VEditor()
{
    tabs.setLookAndFeel(nullptr);

    tabs.getTabbedButtonBar().removeChangeListener(this);
    wtGui.page.removeComponentListener(this);
    adpcmGui.loadButton.removeListener(this);
    audioProcessor.apvts.removeParameterListener("MODE", this);

    // Remove listeners for rhythm buttons
    for (auto& pad : rhythmGui.pads) {
        pad.loadButton.removeListener(this);
    }
}

void AudioPlugin2686VEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &tabs.getTabbedButtonBar())
    {
        // 0:OPNA, 1:OPN, 2:OPL, ...
        int targetMode = tabs.getCurrentTabIndex();

        if (targetMode >= 0 && targetMode <= (int)OscMode::ADPCM) // ADPCM is 9
        {
            auto* param = audioProcessor.apvts.getParameter("MODE");
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
    content.removeFromTop(tabs.getTabBarDepth());
    content.reduce(GroupPaddingWidth, GroupPaddingHeight); // 全体の余白
    auto inner = content.removeFromTop(content.getHeight() - 80);

    layoutOpnaPage(opnaGui, inner);
    layoutOpnPage(opnGui, inner);
    layoutOplPage(oplGui, inner);
	layoutOpllPage(opllGui, inner);
	layoutOpl3Page(opl3Gui, inner);
	layoutOpmPage(opmGui, inner);
	layoutSsgPage(ssgGui, inner);
    layoutWtPage(wtGui, inner);
	layoutRhythmPage(rhythmGui, inner);
	layoutAdpcmPage(adpcmGui, inner);
    layoutFxPage(fxGui, inner);
    layoutPresetPage(presetGui, inner);
    layoutSettingsPage(settingsGui, inner);
    layoutAboutPage(aboutGui, inner);

    // マスターボリュームは、タブの一番下に付ける
    auto masterVolArea = content.reduced(100, 10);
    masterVolLabel.setBounds(masterVolArea.removeFromLeft(200));
    masterVolSlider.setBounds(masterVolArea.removeFromLeft(400));
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

void AudioPlugin2686VEditor::attatchLabelToComponent(juce::Label& label, juce::Component& component)
{
    label.attachToComponent(&component, false);
    label.setJustificationType(juce::Justification::centred);
}

void AudioPlugin2686VEditor::setupLogo()
{
    logoLabel.setText(VstName, juce::dontSendNotification);

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
    tabs.addTab("OPNA", juce::Colours::transparentBlack, &opnaGui.page, true);
    tabs.addTab("OPN", juce::Colours::transparentBlack, &opnGui.page, true);
    tabs.addTab("OPL", juce::Colours::transparentBlack, &oplGui.page, true);
	tabs.addTab("OPLL", juce::Colours::transparentBlack, &opllGui.page, true);
    tabs.addTab("OPL3", juce::Colours::transparentBlack, &opl3Gui.page, true);
    tabs.addTab("OPM", juce::Colours::transparentBlack, &opmGui.page, true);
    tabs.addTab("SSG", juce::Colours::transparentBlack, &ssgGui.page, true);
    tabs.addTab("WAVETABLE", juce::Colours::transparentBlack, &wtGui.page, true);
    tabs.addTab("RHYTHM", juce::Colours::transparentBlack, &rhythmGui.page, true);
    tabs.addTab("ADPCM", juce::Colours::transparentBlack, &adpcmGui.page, true);
    tabs.addTab("FX", juce::Colours::transparentBlack, &fxGui.page, true);
    tabs.addTab("PRESET", juce::Colours::transparentBlack, &presetGui.page, true);
    tabs.addTab("SETTINGS", juce::Colours::transparentBlack, &settingsGui.page, true);
    tabs.addTab("ABOUT", juce::Colours::transparentBlack, &aboutGui.page, true);
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

    if (params.regType != RegisterType::None)
    {
        sliderRegMap[&params.slider] = params.regType;
        params.slider.addMouseListener(this, false);
    }
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
        
        SetupGroupParams groupParams = { .page = params.page, .group = params.groups[i], .title = "Operator " + juce::String(i + 1) };

        setupGroup(groupParams);
    }
}

void AudioPlugin2686VEditor::setupOpnaGui(Fm4GuiSet& gui)
{
    std::vector<SelectItem> pmsItems = createItems(8);
    std::vector<SelectItem> amsItems = createItems(4);
    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> seItems = {
        {.name = "Normal (OFF)", .value = 1 },
        {.name = "\\ \\ \\ \\ (Saw Dn)", .value = 2 },
        {.name = "\\_ \\_ (Dn Hold)", .value = 3 },
        {.name = "\\ / \\ / (Tri)", .value = 4 },
        {.name = "\\- \\- (Dn Hold Alt)", .value = 5 },
        {.name = "/ / / / (Saw Up)", .value = 6 },
        {.name = "/~ /~ (Up Hold)", .value = 7 },
        {.name = "/ \\ / \\ (Tri Inv)", .value = 8 },
        {.name = "/_ /_ (Up Hold Alt)", .value = 9 },
    };
    std::vector<SelectItem> ksItems = { {.name = "OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    std::vector<SelectItem> bdItems = {
        {.name = "4-bit (16 steps)",  .value = 1 },
        {.name = "5-bit (32 steps)",  .value = 2 },
        {.name = "6-bit (64 steps)",  .value = 3 },
        {.name = "8-bit (256 steps)", .value = 4 },
        {.name = "Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "96kHz",    .value = 1 },
        {.name = "55.5kHz",  .value = 2 },
        {.name = "48kHz",    .value = 3 },
        {.name = "44.1kHz",  .value = 4 },
        {.name = "22.05kHz", .value = 5 },
        {.name = "16kHz",    .value = 6 },
        {.name = "8kHz",     .value = 7 },
    };

	SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Algorithm / Feedback / LFO" };
    setupGroup(groupParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPNA_ALGORITHM", "Algorithm", algItems);
    setupCombo(algParams);
	attatchLabelToComponent(gui.algLabel, gui.algSelector);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPNA_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);
	attatchLabelToComponent(gui.feedbackLabel, gui.feedbackSlider);
    SetupSliderParams fqParams = SetupSliderParams::create(gui.page, gui.lfoFreqSlider, gui.lfoFreqLabel, gui.lfoFreqAtt, "OPNA_LFO_FREQ", "LFO FREQ");
    setupSlider(fqParams);
	attatchLabelToComponent(gui.lfoFreqLabel, gui.lfoFreqSlider);
    SetupComboParams pmsParams = SetupComboParams::create(gui.page, gui.lfoPmsSelector, gui.lfoPmsLabel, gui.lfoPmsAtt, "OPNA_LFO_PMS", "LFO PMS", pmsItems);
    setupCombo(pmsParams);
	attatchLabelToComponent(gui.lfoPmsLabel, gui.lfoPmsSelector);
    SetupComboParams amsParams = SetupComboParams::create(gui.page, gui.lfoAmsSelector, gui.lfoAmsLabel, gui.lfoAmsAtt, "OPNA_LFO_AMS", "LFO AMS", amsItems);
    setupCombo(amsParams);
	attatchLabelToComponent(gui.lfoAmsLabel, gui.lfoAmsSelector);

    SetupGroupParams qGroupParams = { .page = gui.page, .group = gui.qualityGroup, .title = "Quality" };
    setupGroup(qGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, "OPNA_BIT", "Bit Depth", bdItems, { 40, 15 });
    setupCombo(bParams);
    attatchLabelToComponent(gui.bitLabel, gui.bitSelector);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, "OPNA_RATE", "Rate", rateItems, { 40, 15 });
    setupCombo(rtParams);
    attatchLabelToComponent(gui.rateLabel, gui.rateCombo);

    // Operators
	SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPNA_OP" + juce::String(i) + "_";

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR31 DR0 SR0 RR15 SL0 TL0 MUL1 DT0",
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
        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + "DT", "DT", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + "KS", "KS", ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + "SR", "SR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(rrParams);
        SetupComboParams seParams = SetupComboParams::createOp(gui.page, gui.se[i], gui.seLabel[i], gui.seAtt[i], paramPrefix + "SE", "SE", seItems);
        setupCombo(seParams);
        SetupSliderParams seFreqParams = SetupSliderParams::createOp(gui.page, gui.seFreq[i], gui.seFreqLabel[i], gui.seFreqAtt[i], paramPrefix + "SEFREQ", "SSG Env Freq");
        setupSlider(seFreqParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixLabel[i], gui.fixAtt[i], paramPrefix + "FIX", "FIX");
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + "FREQ", "FRQ");
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + "FREQ_TO_0", "Freq -> 0Hz", { 64, 15 }, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + "FREQ_TO_440", "Freq -> 440Hz", { 64, 15 }, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        SetupToggleButtonParams amParams = SetupToggleButtonParams::createOp(gui.page, gui.am[i], gui.amLabel[i], gui.amAtt[i], paramPrefix + "AM", "AM");
        setupToggleButton(amParams);
    }
}

void AudioPlugin2686VEditor::setupOpnGui(Fm4GuiSet& gui)
{
    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> ksItems = { {.name = "OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    std::vector<SelectItem> bdItems = {
        {.name = "4-bit (16 steps)",  .value = 1 },
        {.name = "5-bit (32 steps)",  .value = 2 },
        {.name = "6-bit (64 steps)",  .value = 3 },
        {.name = "8-bit (256 steps)", .value = 4 },
        {.name = "Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "96kHz",    .value = 1 },
        {.name = "55.5kHz",  .value = 2 },
        {.name = "48kHz",    .value = 3 },
        {.name = "44.1kHz",  .value = 4 },
        {.name = "22.05kHz", .value = 5 },
        {.name = "16kHz",    .value = 6 },
        {.name = "8kHz",     .value = 7 },
    };

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Algorithm / Feedback" };
    setupGroup(groupParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPN_ALGORITHM", "Algorithm", algItems);
    setupCombo(algParams);
	attatchLabelToComponent(gui.algLabel, gui.algSelector);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPN_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);
	attatchLabelToComponent(gui.feedbackLabel, gui.feedbackSlider);

    SetupGroupParams qGroupParams = { .page = gui.page, .group = gui.qualityGroup, .title = "Quality" };
    setupGroup(qGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, "OPN_BIT", "Bit Depth", bdItems, { 40, 15 });
    setupCombo(bParams);
    attatchLabelToComponent(gui.bitLabel, gui.bitSelector);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, "OPN_RATE", "Rate", rateItems, { 40, 15 });
    setupCombo(rtParams);
    attatchLabelToComponent(gui.rateLabel, gui.rateCombo);

    // Operators
    SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPN_OP" + juce::String(i) + "_";

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR31 DR0 SR0 RR15 SL0 TL0 MUL1 DT0",
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

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + "DT", "DT", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + "KS", "KS", ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + "SR", "SR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(rrParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixLabel[i], gui.fixAtt[i], paramPrefix + "FIX", "FIX");
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + "FREQ", "FRQ");
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + "FREQ_TO_0", "Freq -> 0Hz", OpTextButtonSize, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + "FREQ_TO_440", "Freq -> 440Hz", OpTextButtonSize, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
    }
}

void AudioPlugin2686VEditor::setupOplGui(Fm2GuiSet& gui)
{
    std::vector<SelectItem> algItems = { {.name = "FM(Serial)", .value = 1}, {.name = "AM (Parallel)", .value = 2}, };
    std::vector<SelectItem> wsItems = { {.name = "Sine", .value = 1}, {.name = "Half", .value = 2}, {.name = "Abs", .value = 3}, {.name = "Pulse", .value = 4}, };

    std::vector<SelectItem> bdItems = {
        {.name = "4-bit (16 steps)",  .value = 1 },
        {.name = "5-bit (32 steps)",  .value = 2 },
        {.name = "6-bit (64 steps)",  .value = 3 },
        {.name = "8-bit (256 steps)", .value = 4 },
        {.name = "Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "96kHz",    .value = 1 },
        {.name = "55.5kHz",  .value = 2 },
        {.name = "48kHz",    .value = 3 },
        {.name = "44.1kHz",  .value = 4 },
        {.name = "22.05kHz", .value = 5 },
        {.name = "16kHz",    .value = 6 },
        {.name = "8kHz",     .value = 7 },
    };

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Algorithm / Feedback" };
    setupGroup(groupParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPL_ALGORITHM", "Algorithm", algItems);
    setupCombo(algParams);
	attatchLabelToComponent(gui.algLabel, gui.algSelector);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPL_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);
	attatchLabelToComponent(gui.feedbackLabel, gui.feedbackSlider);

    SetupGroupParams qGroupParams = { .page = gui.page, .group = gui.qualityGroup, .title = "Quality" };
    setupGroup(qGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, "OPL_BIT", "Bit Depth", bdItems, { 40, 15 });
    setupCombo(bParams);
    attatchLabelToComponent(gui.bitLabel, gui.bitSelector);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, "OPL_RATE", "Rate", rateItems, { 40, 15 });
    setupCombo(rtParams);
    attatchLabelToComponent(gui.rateLabel, gui.rateCombo);

    SetupOpGroupsParams<2> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 2; ++i)
    {
        juce::String paramPrefix = "OPL_OP" + juce::String(i) + "_";

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR31 DR0 SR0 RR15 SL0 TL0 MUL1 DT0",
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

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + "DT", "DT", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(rrParams);
        SetupComboParams wsParams = SetupComboParams::createOp(gui.page, gui.ws[i], gui.wsLabel[i], gui.wsAtt[i], paramPrefix + "WS", "WS", wsItems);
        setupCombo(wsParams);
    }
}

void AudioPlugin2686VEditor::setupOpllGui(OpllGuiSet& gui)
{
    std::vector<SelectItem> wsItems = { {.name = "Sine", .value = 1}, {.name = "Half", .value = 2}, };
    std::vector<SelectItem> kslItems = { {.name = "KSL: 0", .value = 1}, {.name = "KSL: 1", .value = 2}, {.name = "KSL: 2", .value = 3}, {.name = "KSL: 3", .value = 4}, };

    std::vector<SelectItem> bdItems = {
        {.name = "4-bit (16 steps)",  .value = 1 },
        {.name = "5-bit (32 steps)",  .value = 2 },
        {.name = "6-bit (64 steps)",  .value = 3 },
        {.name = "8-bit (256 steps)", .value = 4 },
        {.name = "Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "96kHz",    .value = 1 },
        {.name = "55.5kHz",  .value = 2 },
        {.name = "48kHz",    .value = 3 },
        {.name = "44.1kHz",  .value = 4 },
        {.name = "22.05kHz", .value = 5 },
        {.name = "16kHz",    .value = 6 },
        {.name = "8kHz",     .value = 7 },
    };

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Global" };
    setupGroup(groupParams);

    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPLL_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);
	attatchLabelToComponent(gui.feedbackLabel, gui.feedbackSlider);

    SetupGroupParams qGroupParams = { .page = gui.page, .group = gui.qualityGroup, .title = "Quality" };
    setupGroup(qGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, "OPLL_BIT", "Bit Depth", bdItems, { 40, 15 });
    setupCombo(bParams);
    attatchLabelToComponent(gui.bitLabel, gui.bitSelector);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, "OPLL_RATE", "Rate", rateItems, { 40, 15 });
    setupCombo(rtParams);
    attatchLabelToComponent(gui.rateLabel, gui.rateCombo);

    SetupOpGroupsParams<2> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 2; ++i)
    {
        juce::String paramPrefix = "OPLL_OP" + juce::String(i) + "_";

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR31 DR0 SR0 RR15 SL0 TL0 MUL1 DT0",
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

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(rrParams);
        SetupToggleButtonParams amParams = SetupToggleButtonParams::createOp(gui.page, gui.am[i], gui.amLabel[i], gui.amAtt[i], paramPrefix + "AM", "AM");
        setupToggleButton(amParams);
        SetupComboParams wsParams = SetupComboParams::createOp(gui.page, gui.ksl[i], gui.kslLabel[i], gui.kslAtt[i], paramPrefix + "KSL", "KSL", kslItems);
        setupCombo(wsParams);
        SetupToggleButtonParams vibParams = SetupToggleButtonParams::createOp(gui.page, gui.vib[i], gui.vibLabel[i], gui.vibAtt[i], paramPrefix + "VIB", "VIB");
        setupToggleButton(vibParams);
        SetupToggleButtonParams egtParams = SetupToggleButtonParams::createOp(gui.page, gui.egType[i], gui.egTypeLabel[i], gui.egTypeAtt[i], paramPrefix + "EG_TYP", "EGTYPE");
        setupToggleButton(egtParams);
        SetupToggleButtonParams ksrParams = SetupToggleButtonParams::createOp(gui.page, gui.ksr[i], gui.ksrLabel[i], gui.ksrAtt[i], paramPrefix + "KSR", "KSR");
        setupToggleButton(ksrParams);
    }
}

void AudioPlugin2686VEditor::setupOpl3Gui(Opl3GuiSet& gui)
{
    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> wsItems = {
        {.name = "Sine", .value = 1},
        {.name = "Half", .value = 2},
        {.name = "Abs", .value = 3},
        {.name = "Pulse", .value = 4},
        {.name = "Sine(Alt)", .value = 5},
        {.name = "Abs(Alt)", .value = 6},
        {.name = "Square", .value = 7},
        {.name = "Derived Sq", .value = 8},
    };

    std::vector<SelectItem> bdItems = {
        {.name = "4-bit (16 steps)",  .value = 1 },
        {.name = "5-bit (32 steps)",  .value = 2 },
        {.name = "6-bit (64 steps)",  .value = 3 },
        {.name = "8-bit (256 steps)", .value = 4 },
        {.name = "Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "96kHz",    .value = 1 },
        {.name = "55.5kHz",  .value = 2 },
        {.name = "48kHz",    .value = 3 },
        {.name = "44.1kHz",  .value = 4 },
        {.name = "22.05kHz", .value = 5 },
        {.name = "16kHz",    .value = 6 },
        {.name = "8kHz",     .value = 7 },
    };

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Algorithm / Feedback" };
    setupGroup(groupParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPL3_ALGORITHM", "Algorithm", algItems);
    setupCombo(algParams);
	attatchLabelToComponent(gui.algLabel, gui.algSelector);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPL3_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);
	attatchLabelToComponent(gui.feedbackLabel, gui.feedbackSlider);

    SetupGroupParams qGroupParams = { .page = gui.page, .group = gui.qualityGroup, .title = "Quality" };
    setupGroup(qGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, "OPL3_BIT", "Bit Depth", bdItems, { 40, 15 });
    setupCombo(bParams);
    attatchLabelToComponent(gui.bitLabel, gui.bitSelector);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, "OPL3_RATE", "Rate", rateItems, { 40, 15 });
    setupCombo(rtParams);
    attatchLabelToComponent(gui.rateLabel, gui.rateCombo);

    SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPL3_OP" + juce::String(i) + "_";

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR31 DR0 SR0 RR15 SL0 TL0 MUL1 DT0",
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

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(rrParams);
        SetupComboParams wsParams = SetupComboParams::createOp(gui.page, gui.ws[i], gui.wsLabel[i], gui.wsAtt[i], paramPrefix + "WS", "WS", wsItems);
        setupCombo(wsParams);
    }
}

void AudioPlugin2686VEditor::setupOpmGui(OpmGuiSet& gui)
{
    std::vector<SelectItem> pmsItems = createItems(8);
    std::vector<SelectItem> amsItems = createItems(4);
    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> ksItems = { {.name = "OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    std::vector<SelectItem> bdItems = {
        {.name = "4-bit (16 steps)",  .value = 1 },
        {.name = "5-bit (32 steps)",  .value = 2 },
        {.name = "6-bit (64 steps)",  .value = 3 },
        {.name = "8-bit (256 steps)", .value = 4 },
        {.name = "Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "96kHz",    .value = 1 },
        {.name = "55.5kHz",  .value = 2 },
        {.name = "48kHz",    .value = 3 },
        {.name = "44.1kHz",  .value = 4 },
        {.name = "22.05kHz", .value = 5 },
        {.name = "16kHz",    .value = 6 },
        {.name = "8kHz",     .value = 7 },
    };

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Algorithm / Feedback / LFO" };
    setupGroup(groupParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPM_ALGORITHM", "Algorithm", algItems);
    setupCombo(algParams);
	attatchLabelToComponent(gui.algLabel, gui.algSelector);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPM_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);
	attatchLabelToComponent(gui.feedbackLabel, gui.feedbackSlider);
    SetupSliderParams fqParams = SetupSliderParams::create(gui.page, gui.lfoFreqSlider, gui.lfoFreqLabel, gui.lfoFreqAtt, "OPM_LFO_FREQ", "LFO FREQ");
    setupSlider(fqParams);
	attatchLabelToComponent(gui.lfoFreqLabel, gui.lfoFreqSlider);
    SetupComboParams pmsParams = SetupComboParams::create(gui.page, gui.pmsSelector, gui.pmsLabel, gui.pmsAtt, "OPM_LFO_PMS", "LFO PMS", pmsItems);
    setupCombo(pmsParams);
	attatchLabelToComponent(gui.pmsLabel, gui.pmsSelector);
    SetupComboParams amsParams = SetupComboParams::create(gui.page, gui.amsSelector, gui.amsLabel, gui.amsAtt, "OPM_LFO_AMS", "LFO AMS", amsItems);
    setupCombo(amsParams);
	attatchLabelToComponent(gui.amsLabel, gui.amsSelector);

    SetupGroupParams qGroupParams = { .page = gui.page, .group = gui.qualityGroup, .title = "Quality" };
    setupGroup(qGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, "OPM_BIT", "Bit Depth", bdItems, { 40, 15 });
    setupCombo(bParams);
    attatchLabelToComponent(gui.bitLabel, gui.bitSelector);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, "OPM_RATE", "Rate", rateItems, { 40, 15 });
    setupCombo(rtParams);
    attatchLabelToComponent(gui.rateLabel, gui.rateCombo);

    // Operators
    SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPM_OP" + juce::String(i) + "_";

        gui.page.addAndMakeVisible(gui.mmlBtn[i]);
        gui.mmlBtn[i].setButtonText("MML");
        gui.mmlBtn[i].onClick = [this, &gui, i] {
            auto* w = new juce::AlertWindow("MML Input (Op " + juce::String(i + 1) + ")",
                "e.g. AR31 DR0 SR0 RR15 SL0 TL0 MUL1 DT0",
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

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmMul);
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt1[i], gui.dt1Label[i], gui.dt1Att[i], paramPrefix + "DT", "DT1", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt);
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmTl);
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::createOp(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + "KS", "KS", ksItems); // KS doesn't strictly need reg convert, it's 0-3
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "D1R", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "D1L", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmSl);
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + "SR", "D2R", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmRate);
        setupSlider(rrParams);
        SetupSliderParams dt2Params = SetupSliderParams::createOp(gui.page, gui.dt2[i], gui.dt2Label[i], gui.dt2Att[i], paramPrefix + "DT2", "DT2", OpSliderSize, OpSliderValueSize, OpLabelSize, OpSliderFlags, RegisterType::FmDt2);
        setupSlider(dt2Params);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixLabel[i], gui.fixAtt[i], paramPrefix + "FIX", "FIX");
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + "FREQ", "FRQ");
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + "FREQ_TO_0", "Freq -> 0Hz", OpTextButtonSize, { false, false });
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + "FREQ_TO_440", "Freq -> 440Hz", OpTextButtonSize, { false, false });
        setupTextButton(fto440Params);
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
    }
}

void AudioPlugin2686VEditor::setupSsgGui(SsgGuiSet& gui)
{
    std::vector<SelectItem> wsItems = {
        { .name = "Pulse(Rect)", .value = 1 },
        { .name = "Triangle / Saw", .value = 2 },
    };
    std::vector<SelectItem> dmItems = {
        {.name = "Preset Ratio", .value = 1 },
        {.name = "Variable (Slider)", .value = 2 },
    };
    std::vector<SelectItem> prItems = {
        {.name = "1:1 (50%)", .value = 1 },
        {.name = "3:5 (37.5%)", .value = 2 },
        {.name = "5:11 (31.25%)", .value = 3 },
        {.name = "1:3 (25%)", .value = 4 },
        {.name = "1:4 (20%)", .value = 5 },
        {.name = "3:13 (18.75%)", .value = 6 },
        {.name = "1:7 (12.5%)", .value = 7 },
        {.name = "1:15 (6.25%)", .value = 8 },
    };
    std::vector<SelectItem> envItems = {
        {.name = "08: \\\\\\\\ (Decay)", .value = 1 },
        {.name = "09: \\___ (Decay->0)", .value = 2 },
        {.name = "0A: \\/\\/ (Tri)", .value = 3 },
        {.name = "0B: \\``` (Decay->1)", .value = 4 },
        {.name = "0C: //// (Attack)", .value = 5 },
        {.name = "0D: /``` (Attack->1)", .value = 6 },
        {.name = "0E: /\\/\\ (Inv Tri)", .value = 7 },
        {.name = "0F: /___ (Attack->0)", .value = 8 },
    };

    std::vector<SelectItem> bdItems = {
        {.name = "4-bit (16 steps)",  .value = 1 },
        {.name = "5-bit (32 steps)",  .value = 2 },
        {.name = "6-bit (64 steps)",  .value = 3 },
        {.name = "8-bit (256 steps)", .value = 4 },
        {.name = "Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        { .name = "96kHz",    .value = 1 },
        { .name = "55.5kHz",  .value = 2 },
        { .name = "48kHz",    .value = 3 },
        { .name = "44.1kHz",  .value = 4 },
        { .name = "22.05kHz", .value = 5 },
        { .name = "16kHz",    .value = 6 },
        { .name = "8kHz",     .value = 7 },
    };

    SetupGroupParams vGroupParams = { .page = gui.page, .group = gui.voiceGroup, .title = "Voice" };
    setupGroup(vGroupParams);

    SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.waveSelector, gui.waveLabel, gui.waveAtt, "SSG_WAVEFORM", "Wave Form", wsItems, ComboBoxSize, LabelSize, {true, true});
    setupCombo(wsParams);
	attatchLabelToComponent(gui.waveLabel, gui.waveSelector);

    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "SSG_LEVEL", "Tone", SliderSize, SliderValueSize, LabelSize, SliderFlags, RegisterType::SsgVol);
    setupSlider(lvParams);
    attatchLabelToComponent(gui.levelLabel, gui.levelSlider);
    SetupSliderParams nsParams = SetupSliderParams::create(gui.page, gui.noiseSlider, gui.noiseLabel, gui.noiseAtt, "SSG_NOISE", "Noise", SliderSize, SliderValueSize, LabelSize, SliderFlags, RegisterType::SsgVol);
    setupSlider(nsParams);
    attatchLabelToComponent(gui.noiseLabel, gui.noiseSlider);
    SetupSliderParams nfParams = SetupSliderParams::create(gui.page, gui.noiseFreqSlider, gui.noiseFreqLabel, gui.noiseFreqAtt, "SSG_NOISE_FREQ", "Noise Freq");
    setupSlider(nfParams);
	attatchLabelToComponent(gui.noiseFreqLabel, gui.noiseFreqSlider);
    SetupSliderParams mxParams = SetupSliderParams::create(gui.page, gui.mixSlider, gui.mixLabel, gui.mixAtt, "SSG_MIX", "Mix (T<-M->N)");
    setupSlider(mxParams);
	attatchLabelToComponent(gui.mixLabel, gui.mixSlider);

    SetupTextButtonParams mstParams = SetupTextButtonParams::create(gui.page, gui.mixSetTone, gui.mixSetAtt, "Tone", "Tone", { 40, 20 }, { false, false });
    setupTextButton(mstParams);
	attatchLabelToComponent(gui.mixSetToneLabel, gui.mixSetTone);
    gui.mixSetTone.onClick = [this] { ssgGui.triPeakSlider.setValue(0.0, juce::sendNotification); };
    SetupTextButtonParams msmParams = SetupTextButtonParams::create(gui.page, gui.mixSetMix, gui.mixSetAtt, "Mix", "Mix", { 40, 20 }, { false, false });
    setupTextButton(msmParams);
	attatchLabelToComponent(gui.mixSetMixLabel, gui.mixSetMix);
    gui.mixSetMix.onClick = [this] { ssgGui.triPeakSlider.setValue(0.5, juce::sendNotification); };
    SetupTextButtonParams msnParams = SetupTextButtonParams::create(gui.page, gui.mixSetNoise, gui.mixSetAtt, "Noise", "Noise", { 40, 20 }, { false, false });
    setupTextButton(msnParams);
	attatchLabelToComponent(gui.mixSetNoiseLabel, gui.mixSetNoise);
    gui.mixSetNoise.onClick = [this] { ssgGui.triPeakSlider.setValue(1.0, juce::sendNotification); };

    SetupGroupParams qGroupParams = { .page = gui.page, .group = gui.qualityGroup, .title = "Quality" };
    setupGroup(qGroupParams);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, "SSG_BIT", "Bit Depth", bdItems, { 40, 15 });
    setupCombo(bParams);
    attatchLabelToComponent(gui.bitLabel, gui.bitSelector);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, "SSG_RATE", "Rate", rateItems, { 40, 15 });
    setupCombo(rtParams);
    attatchLabelToComponent(gui.rateLabel, gui.rateCombo);

    SetupGroupParams fGroupParams = { .page = gui.page, .group = gui.filterGroup, .title = "Filter (Envelope)" };
    setupGroup(fGroupParams);
    SetupToggleButtonParams bpParams = SetupToggleButtonParams::create(gui.page, gui.adsrBypassButton, gui.adsrBypassButtonLabel, gui.adsrBypassAtt, "SSG_ADSR_BYPASS", "Bypass ADSR (Gate)", { 160, 20 }, { true, true });
    setupToggleButton(bpParams);
	attatchLabelToComponent(gui.adsrBypassButtonLabel, gui.adsrBypassButton);
    SetupSliderParams arParams = SetupSliderParams::create(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, "SSG_AR", "Attack");
    setupSlider(arParams);
	attatchLabelToComponent(gui.attackLabel, gui.attackSlider);
    SetupSliderParams drParams = SetupSliderParams::create(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, "SSG_DR", "Decay");
    setupSlider(drParams);
	attatchLabelToComponent(gui.decayLabel, gui.decaySlider);
    SetupSliderParams slParams = SetupSliderParams::create(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, "SSG_SL", "Sustain");
    setupSlider(slParams);
	attatchLabelToComponent(gui.sustainLabel, gui.sustainSlider);
    SetupSliderParams rrParams = SetupSliderParams::create(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, "SSG_RR", "Release");
    setupSlider(rrParams);
	attatchLabelToComponent(gui.releaseLabel, gui.releaseSlider);

    // Duty Controls Setup
    SetupGroupParams dGroupParams = { .page = gui.page, .group = gui.dutyGroup, .title = "Pulse Width (Duty)" };
    setupGroup(dGroupParams);
    SetupComboParams dmParams = SetupComboParams::create(gui.page, gui.dutyModeSelector, gui.dutyModeLabel, gui.dutyModeAtt, "SSG_DUTY_MODE", "Duty Mode", dmItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(dmParams);
	attatchLabelToComponent(gui.dutyModeLabel, gui.dutyModeSelector);
    SetupComboParams dpParams = SetupComboParams::create(gui.page, gui.dutyPresetSelector, gui.dutyPresetLabel, gui.dutyPresetAtt, "SSG_DUTY_PRESET", "Duty Preset", prItems);
    setupCombo(dpParams);
	attatchLabelToComponent(gui.dutyPresetLabel, gui.dutyPresetSelector);
    SetupSliderParams dvParams = SetupSliderParams::create(gui.page, gui.dutyVarSlider, gui.dutyVarLabel, gui.dutyVarAtt, "SSG_DUTY_VAR", "Duty Ratio");
    setupSlider(dvParams);
	attatchLabelToComponent(gui.dutyVarLabel, gui.dutyVarSlider);
    SetupToggleButtonParams dtiParams = SetupToggleButtonParams::create(gui.page, gui.dutyInvertButton, gui.dutyInvertLabel, gui.dutyInvertAtt, "SSG_DUTY_INV", "Invert Phase", { 160, 20 });
    setupToggleButton(dtiParams);
	attatchLabelToComponent(gui.dutyInvertLabel, gui.dutyInvertButton);
    SetupGroupParams tpGroupParams = { .page = gui.page, .group = gui.triGroup, .title = "Triangle Property" };
    setupGroup(tpGroupParams);
	attatchLabelToComponent(gui.triKeyTrackLabel, gui.triKeyTrackButton);
    SetupToggleButtonParams tktParams = SetupToggleButtonParams::create(gui.page, gui.triKeyTrackButton, gui.triKeyTrackLabel, gui.triKeyTrackAtt, "SSG_TRI_KEYTRK", "Key Track (Pitch)", ToggleButtonSize, { true, true });
    setupToggleButton(tktParams);
	attatchLabelToComponent(gui.triFreqLabel, gui.triFreqSlider);
    SetupSliderParams tfParams = SetupSliderParams::create(gui.page, gui.triFreqSlider, gui.triFreqLabel, gui.triFreqAtt, "SSG_TRI_FREQ", "Manual Freq (Hz)");
    setupSlider(tfParams);
	attatchLabelToComponent(gui.triPeakLabel, gui.triPeakSlider);
    SetupSliderParams tpParams = SetupSliderParams::create(gui.page, gui.triPeakSlider, gui.triPeakLabel, gui.triPeakAtt, "SSG_TRI_PEAK", "Peak Position");
    setupSlider(tpParams);
	attatchLabelToComponent(gui.triPeakLabel, gui.triPeakSlider);

    SetupTextButtonParams tsdParams = SetupTextButtonParams::create(gui.page, gui.triSetSawDown, gui.triKeyTrackAtt, "0.0 (Down)", "0.0 (Down)", TextButtonSize, { false, false });
    setupTextButton(tsdParams);
	attatchLabelToComponent(gui.triSetSawDownLabel, gui.triSetSawDown);
    gui.triSetSawDown.onClick = [this] { ssgGui.triPeakSlider.setValue(0.0, juce::sendNotification); };
    SetupTextButtonParams tstParams = SetupTextButtonParams::create(gui.page, gui.triSetTri, gui.triKeyTrackAtt, "0.5 (Tri)0", "0.5 (Tri)", TextButtonSize, { false, false });
    setupTextButton(tstParams);
	attatchLabelToComponent(gui.triSetTriLabel, gui.triSetTri);
    gui.triSetTri.onClick = [this] { ssgGui.triPeakSlider.setValue(0.5, juce::sendNotification); };
    SetupTextButtonParams tsuParams = SetupTextButtonParams::create(gui.page, gui.triSetSawUp, gui.triKeyTrackAtt, "1.0 (Up)", "1.0 (Up)", TextButtonSize, { false, false });
    setupTextButton(tsuParams);
	attatchLabelToComponent(gui.triSetSawUpLabel, gui.triSetSawUp);
    gui.triSetSawUp.onClick = [this] { ssgGui.triPeakSlider.setValue(1.0, juce::sendNotification); };

    // HW Env Group
    SetupGroupParams groupParams = { .page = gui.page, .group = gui.envGroup, .title = "Hardware Envelope" };
    setupGroup(groupParams);
    SetupToggleButtonParams eeParams = SetupToggleButtonParams::create(gui.page, gui.envEnableButton, gui.envEnableLabel, gui.envEnableAtt, "SSG_ENV_ENABLE", "Enable HW Env", { 160, 20 }, { true, true });
    setupToggleButton(eeParams);
	attatchLabelToComponent(gui.envEnableLabel, gui.envEnableButton);
    SetupComboParams evParams = SetupComboParams::create(gui.page, gui.shapeSelector, gui.shapeLabel, gui.shapeAtt, "SSG_ENV_SHAPE", "Shape", envItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(evParams);
	attatchLabelToComponent(gui.shapeLabel, gui.shapeSelector);
    SetupSliderParams epParams = SetupSliderParams::create(gui.page, gui.periodSlider, gui.periodLabel, gui.periodAtt, "SSG_ENV_PERIOD", "Env Speed (Hz)", SliderSize, SliderValueSize, LabelSize, SliderFlags, RegisterType::SsgEnv);
    setupSlider(epParams);
    attatchLabelToComponent(gui.periodLabel, gui.periodSlider);
}

void AudioPlugin2686VEditor::setupWtGui(WtGuiSet& gui)
{
    std::vector<SelectItem> wsItems = {
        {.name = "Sine",          .value = 1 },
        {.name = "Triangle",      .value = 2 },
        {.name = "Saw Up",        .value = 3 },
        {.name = "Saw Down",      .value = 4 },
        {.name = "Square (50%)",  .value = 5 },
        {.name = "Pulse (25%)",   .value = 6 },
        {.name = "Pulse (12.5%)", .value = 7 },
        {.name = "Digital Noise", .value = 8 },
        {.name = "Custom(Draw)",  .value = 9 },
    };

    std::vector<SelectItem> tsItems = {
        {.name = "32 Samples",  .value = 1 },
        {.name = "64 Samples",  .value = 2 },
    };

    std::vector<SelectItem> bdItems = {
        {.name = "4-bit (16 steps)",  .value = 1 },
        {.name = "5-bit (32 steps)",  .value = 2 },
        {.name = "6-bit (64 steps)",  .value = 3 },
        {.name = "8-bit (256 steps)", .value = 4 },
        {.name = "Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "96kHz",    .value = 1 },
        {.name = "55.5kHz",  .value = 2 },
        {.name = "48kHz",    .value = 3 },
        {.name = "44.1kHz",  .value = 4 },
        {.name = "22.05kHz", .value = 5 },
        {.name = "16kHz",    .value = 6 },
        {.name = "8kHz",     .value = 7 },
    };

    // --- Filter Group (Left) ---
    SetupGroupParams fGroupParams = { .page = gui.page, .group = gui.filterGroup, .title = "Filter (ADSR)" };
    setupGroup(fGroupParams);
    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "WT_LEVEL", "Level");
    setupSlider(lvParams);
	attatchLabelToComponent(gui.levelLabel, gui.levelSlider);
    SetupSliderParams arParams = SetupSliderParams::create(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, "WT_AR", "Attack");
    setupSlider(arParams);
	attatchLabelToComponent(gui.attackLabel, gui.attackSlider);
    SetupSliderParams drParams = SetupSliderParams::create(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, "WT_DR", "Decay");
    setupSlider(drParams);
	attatchLabelToComponent(gui.decayLabel, gui.decaySlider);
    SetupSliderParams slParams = SetupSliderParams::create(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, "WT_SL", "Sustain");
    setupSlider(slParams);
	attatchLabelToComponent(gui.sustainLabel, gui.sustainSlider);
    SetupSliderParams rrParams = SetupSliderParams::create(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, "WT_RR", "Release");
    setupSlider(rrParams);
	attatchLabelToComponent(gui.releaseLabel, gui.releaseSlider);

    // --- Property Group (Right) ---
    SetupGroupParams wpGroupParams = { .page = gui.page, .group = gui.propGroup, .title = "Wavetable Properties" };
    setupGroup(wpGroupParams);

    // Waveform Selector Setup
    SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.waveSelector, gui.waveLabel, gui.waveAtt, "WT_WAVE", "Wave Form", wsItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(wsParams);
	attatchLabelToComponent(gui.waveLabel, gui.waveSelector);

    // Custom Sliders Setup
    SetupGroupParams cwGroupParams = { .page = gui.page, .group = gui.customGroup, .title = "Custom Wave" };
    setupGroup(cwGroupParams);
	SetupCustomWaveTableParams cw32Params = { gui.page, gui.customSliders32, gui.customAtts32, "WT_CUSTOM32_" };
    setupCustomWaveTable(cw32Params);
    SetupCustomWaveTableParams cw64Params = { gui.page, gui.customSliders64, gui.customAtts64, "WT_CUSTOM64_" };
    setupCustomWaveTable(cw64Params);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, "WT_BIT", "Bit Depth", bdItems);
    setupCombo(bParams);
	attatchLabelToComponent(gui.bitLabel, gui.bitSelector);
    SetupComboParams rtParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, "WT_RATE", "Rate", rateItems);
    setupCombo(rtParams);
    attatchLabelToComponent(gui.rateLabel, gui.rateCombo);
    SetupComboParams szParams = SetupComboParams::create(gui.page, gui.sizeSelector, gui.sizeLabel, gui.sizeAtt, "WT_SIZE", "Table Size", tsItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(szParams);
	attatchLabelToComponent(gui.sizeLabel, gui.sizeSelector);
    SetupToggleButtonParams meParams = SetupToggleButtonParams::create(gui.page, gui.modEnableButton, gui.modEnableLabel, gui.modEnableAtt, "WT_MOD_ENABLE", "Modulation ON", ToggleButtonSize, { true, true });
    setupToggleButton(meParams);
	attatchLabelToComponent(gui.modEnableLabel, gui.modEnableButton);
    SetupSliderParams mdParams = SetupSliderParams::create(gui.page, gui.modDepthSlider, gui.modDepthLabel, gui.modDepthAtt, "WT_MOD_DEPTH", "Mod Depth");
    setupSlider(mdParams);
	attatchLabelToComponent(gui.modDepthLabel, gui.modDepthSlider);
    SetupSliderParams msParams = SetupSliderParams::create(gui.page, gui.modSpeedSlider, gui.modSpeedLabel, gui.modSpeedAtt, "WT_MOD_SPEED", "Mod Speed");
    setupSlider(msParams);
	attatchLabelToComponent(gui.modSpeedLabel, gui.modSpeedSlider);
}

void AudioPlugin2686VEditor::setupRhythmGui(RhythmGuiSet& gui)
{
    // Prepare Items for ComboBoxes
    std::vector<SelectItem> qualityItems = {
        {.name = "Raw (32bit)", .value = 1 },
        {.name = "24-bit PCM",  .value = 2 },
        {.name = "16-bit PCM",  .value = 3 },
        {.name = "8-bit PCM",   .value = 4 },
        {.name = "5-bit PCM",   .value = 5 },
        {.name = "4-bit PCM",   .value = 6 },
        {.name = "4-bit ADPCM", .value = 7 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "96kHz",    .value = 1 },
        {.name = "55.5kHz",  .value = 2 },
        {.name = "48kHz",    .value = 3 },
        {.name = "44.1kHz",  .value = 4 },
        {.name = "22.05kHz", .value = 5 },
        {.name = "16kHz",    .value = 6 },
        {.name = "8kHz",     .value = 7 },
    };

    // Pad Definitions
    const char* padNames[] = { "BD", "SD", "HH Cl", "HH Op", "Tom L", "Tom H", "Crash", "Ride" };

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.group, .title = "Global" };
    setupGroup(groupParams);

    // Master Level
    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "RHYTHM_LEVEL", "Master Vol");
    setupFbSlider(lvParams);
    attatchLabelToComponent(gui.levelLabel, gui.levelSlider);

    // Setup 8 Pads
    for (int i = 0; i < 8; ++i)
    {
        auto& pad = gui.pads[i];
        juce::String padPrefix = "RHYTHM_PAD" + juce::String(i);

        // Construct Title: "Pad 1 (BD) [C1]"
        juce::String padTitle = "Pad " + juce::String(i + 1) + " (" + padNames[i] + ")";

        // Group
        SetupGroupParams groupParams = { .page = gui.page, .group = pad.group, .title = padTitle };
        setupGroup(groupParams);

        // Load Button
        gui.page.addAndMakeVisible(pad.loadButton);
        pad.loadButton.setButtonText("Load");
        pad.loadButton.addListener(this);

        gui.page.addAndMakeVisible(pad.fileNameLabel);
        pad.fileNameLabel.setText("(Empty)", juce::dontSendNotification);
        pad.fileNameLabel.setJustificationType(juce::Justification::centred);

        pad.clearButton.setButtonText("X"); // スペース節約のため "X" や "Clr"
        pad.group.addAndMakeVisible(pad.clearButton);

        pad.clearButton.onClick = [this, i, &pad]
            {
                // 1. 特定のパッドをアンロード
                audioProcessor.unloadRhythmFile(i);

                // 2. ファイル名表示を更新
                pad.fileNameLabel.setText("No File", juce::dontSendNotification);
            };

        // ADD: OneShot Button
        // Note: We use setupToggleButton helper but customized for small size
        SetupToggleButtonParams shotP = SetupToggleButtonParams::createOp(gui.page, pad.oneShotButton, pad.oneShotLabel, pad.oneShotAtt, padPrefix + "_ONESHOT", "One Shot");
        setupToggleButton(shotP);

        // Note
        SetupSliderParams noteP = SetupSliderParams::createOp(gui.page, pad.noteSlider, pad.noteLabel, pad.noteAtt, padPrefix + "_NOTE", "Note");
        setupSlider(noteP);
        pad.noteSlider.setRange(0, 127, 1);
        pad.noteSlider.textFromValueFunction = [](double value) {
            return getNoteName((int)value);
        };
        pad.noteSlider.updateText();

        // Quality
        SetupComboParams qualP = SetupComboParams::createOp(gui.page, pad.modeCombo, pad.modeLabel, pad.modeAtt, padPrefix + "_MODE", "Mode", qualityItems);
        setupCombo(qualP);

        // Rate
        SetupComboParams rateP = SetupComboParams::createOp(gui.page, pad.rateCombo, pad.rateLabel, pad.rateAtt, padPrefix + "_RATE", "Rate", rateItems);
        setupCombo(rateP);

        // Pan
        SetupSliderParams panP = SetupSliderParams::createOp(gui.page, pad.panSlider, pad.panLabel, pad.panAtt, padPrefix + "_PAN", "Pan");
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
        SetupSliderParams volP = SetupSliderParams::createOp(gui.page, pad.volSlider, pad.volLabel, pad.volAtt, padPrefix + "_VOL", "Vol");
        setupSlider(volP);

        // ★追加: RR Slider
                // 小さな横スライダー、またはロータリー
        pad.rrSlider.setSliderStyle(juce::Slider::LinearBar); // 数値の上にバーが出るタイプ（省スペース）
        // または RotaryHorizontalVerticalDrag
        pad.rrSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        pad.rrSlider.setTooltip("Release Time");
        pad.group.addAndMakeVisible(pad.rrSlider);

        pad.rrLabel.setText("RR", juce::dontSendNotification);
        pad.rrLabel.setFont(10.0f);
        pad.group.addAndMakeVisible(pad.rrLabel);

        // Attachment
        pad.rrAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, padPrefix + "_RR", pad.rrSlider);
    }
}

void AudioPlugin2686VEditor::setupAdpcmGui(AdpcmGuiSet& gui)
{
    // Prepare Items for ComboBoxes
    std::vector<SelectItem> qualityItems = {
        {.name = "Raw (32bit)", .value = 1 },
        {.name = "24-bit PCM",  .value = 2 },
        {.name = "16-bit PCM",  .value = 3 },
        {.name = "8-bit PCM",   .value = 4 },
        {.name = "5-bit PCM",   .value = 5 },
        {.name = "4-bit PCM",   .value = 6 },
        {.name = "4-bit ADPCM", .value = 7 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "96kHz",    .value = 1 },
        {.name = "55.5kHz",  .value = 2 },
        {.name = "48kHz",    .value = 3 },
        {.name = "44.1kHz",  .value = 4 },
        {.name = "22.05kHz", .value = 5 },
        {.name = "16kHz",    .value = 6 },
        {.name = "8kHz",     .value = 7 },
    };

    // Setup Main Group
    SetupGroupParams groupParams = { .page = gui.page, .group = gui.group, .title = "ADPCM Sampler" };
    setupGroup(groupParams);

    // Mode Combo
    SetupComboParams modeParams = SetupComboParams::create(gui.page, gui.modeCombo, gui.modeLabel, gui.modeAtt, "ADPCM_MODE", "Quality", qualityItems);
    setupCombo(modeParams);

    // Rate Combo
    SetupComboParams rateParams = SetupComboParams::create(gui.page, gui.rateCombo, gui.rateLabel, gui.rateAtt, "ADPCM_RATE", "Rate", rateItems);
    setupCombo(rateParams);

    gui.page.addAndMakeVisible(gui.loadButton);
    gui.loadButton.setButtonText("Load File");
    gui.loadButton.addListener(this);

    gui.page.addAndMakeVisible(gui.fileNameLabel);
    gui.fileNameLabel.setText("No File Loaded", juce::dontSendNotification);
    gui.fileNameLabel.setJustificationType(juce::Justification::centredLeft);
    gui.fileNameLabel.setColour(juce::Label::outlineColourId, juce::Colours::white.withAlpha(0.3f));

    gui.clearButton.setButtonText("Clear");
    gui.group.addAndMakeVisible(gui.clearButton);

    gui.clearButton.onClick = [this, &gui]
        {
            // 1. プロセッサーのアンロード関数を呼ぶ
            audioProcessor.unloadAdpcmFile();

            // 2. ラベル表示をクリア
            gui.fileNameLabel.setText("No File", juce::dontSendNotification);
        };

    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "ADPCM_LEVEL", "Master Vol");
    setupFbSlider(lvParams);
	attatchLabelToComponent(gui.levelLabel, gui.levelSlider);
    SetupSliderParams panParams = SetupSliderParams::create(gui.page, gui.panSlider, gui.panLabel, gui.panAtt, "ADPCM_PAN", "Pan");
    setupSlider(panParams);
    gui.panSlider.setRange(0.0f, 1.0f);
    attatchLabelToComponent(gui.panLabel, gui.panSlider);
    gui.page.addAndMakeVisible(gui.btnPanL); gui.btnPanL.setButtonText("L"); gui.btnPanL.addListener(this);
    gui.page.addAndMakeVisible(gui.btnPanC); gui.btnPanC.setButtonText("C"); gui.btnPanC.addListener(this);
    gui.page.addAndMakeVisible(gui.btnPanR); gui.btnPanR.setButtonText("R"); gui.btnPanR.addListener(this);
    SetupToggleButtonParams lpParams = SetupToggleButtonParams::create(gui.page, gui.loopButton, gui.loopLabel, gui.loopAtt, "ADPCM_LOOP", "One Shot");
    setupToggleButton(lpParams);
	attatchLabelToComponent(gui.loopLabel, gui.loopButton);
    SetupSliderParams arParams = SetupSliderParams::create(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, "ADPCM_AR", "Attack");
    setupSlider(arParams);
	attatchLabelToComponent(gui.attackLabel, gui.attackSlider);
    SetupSliderParams drParams = SetupSliderParams::create(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, "ADPCM_DR", "Decay");
    setupSlider(drParams);
	attatchLabelToComponent(gui.decayLabel, gui.decaySlider);
    SetupSliderParams slParams = SetupSliderParams::create(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, "ADPCM_SL", "Sustain");
    setupSlider(slParams);
	attatchLabelToComponent(gui.sustainLabel, gui.sustainSlider);
    SetupSliderParams rrParams = SetupSliderParams::create(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, "ADPCM_RR", "Release");
    setupSlider(rrParams);
	attatchLabelToComponent(gui.releaseLabel, gui.releaseSlider);
}

void AudioPlugin2686VEditor::setupFxGui(FxGuiSet& gui)
{
    auto setupS = [&](juce::Slider& s, juce::Label& l, juce::String name) {
        gui.page.addAndMakeVisible(s);

        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
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

    SetupGroupParams tGp = { .page = gui.page, .group = gui.tremGroup, .title = "Tremolo", .color = juce::Colour::fromFloatRGBA(0.5f, 0.5f, 0.5f, 0.3f) };
    setupGroup(tGp);

    setupS(gui.tRateSlider, gui.tRateLabel, "Rate");
    setupS(gui.tDepthSlider, gui.tDepthLabel, "Depth");
    setupS(gui.tMixSlider, gui.tMixLabel, "Mix");
    setupMixBtns(gui.tDryBtn, gui.tHalfBtn, gui.tWetBtn, gui.tMixSlider);

    SetupGroupParams vGp = { .page = gui.page, .group = gui.vibGroup, .title = "Vibrato", .color = juce::Colour::fromFloatRGBA(0.5f, 0.5f, 0.5f, 0.3f) };
    setupGroup(vGp);

    setupS(gui.vRateSlider, gui.vRateLabel, "Rate");
    setupS(gui.vDepthSlider, gui.vDepthLabel, "Depth");
    setupS(gui.vMixSlider, gui.vMixLabel, "Mix");
    setupMixBtns(gui.vDryBtn, gui.vHalfBtn, gui.vWetBtn, gui.vMixSlider);

    SetupGroupParams cGp = { .page = gui.page, .group = gui.crushGroup, .title = "Bit Crusher", .color = juce::Colour::fromFloatRGBA(0.5f, 0.5f, 0.5f, 0.3f) };
    setupGroup(cGp);

    setupS(gui.cRateSlider, gui.cRateLabel, "Downsample");
    setupS(gui.cBitsSlider, gui.cBitsLabel, "Bit Depth");
    setupS(gui.cMixSlider, gui.cMixLabel, "Mix");
    setupMixBtns(gui.cDryBtn, gui.cHalfBtn, gui.cWetBtn, gui.cMixSlider);

    // Delay Group
    SetupGroupParams dGp = { .page = gui.page, .group = gui.delayGroup, .title = "Delay", .color = juce::Colour::fromFloatRGBA(0.5f, 0.5f, 0.5f, 0.3f) };
    setupGroup(dGp);

    gui.page.addAndMakeVisible(gui.bypassToggle);
    gui.bypassToggle.setButtonText("Master Bypass");
    gui.bypassToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::orange);

    setupS(gui.dTimeSlider, gui.dTimeLabel, "Time (ms)");
    setupS(gui.dFbSlider, gui.dFbLabel, "Feedback");
    setupS(gui.dMixSlider, gui.dMixLabel, "Mix");
    setupMixBtns(gui.dDryBtn, gui.dHalfBtn, gui.dWetBtn, gui.dMixSlider);

    // Reverb Group
    SetupGroupParams rGp = { .page = gui.page, .group = gui.reverbGroup, .title = "Reverb", .color = juce::Colour::fromFloatRGBA(0.5f, 0.5f, 0.5f, 0.3f) };
    setupGroup(rGp);

    setupS(gui.rSizeSlider, gui.rSizeLabel, "Size");
    setupS(gui.rDampSlider, gui.rDampLabel, "Damp");
    setupS(gui.rMixSlider, gui.rMixLabel, "Mix");
    setupMixBtns(gui.rDryBtn, gui.rHalfBtn, gui.rWetBtn, gui.rMixSlider);

    // Attachments
    gui.fxBypassAtt = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "FX_BYPASS", gui.bypassToggle);
    gui.tRateAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_TRM_RATE", gui.tRateSlider);
    gui.tDepthAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_TRM_DEPTH", gui.tDepthSlider);
    gui.tMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_TRM_MIX", gui.tMixSlider);
    gui.vRateAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_VIB_RATE", gui.vRateSlider);
    gui.vDepthAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_VIB_DEPTH", gui.vDepthSlider);
    gui.vMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_VIB_MIX", gui.vMixSlider);
    gui.cRateAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_BIT_RATE", gui.cRateSlider);
    gui.cBitsAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_BIT_DEPTH", gui.cBitsSlider);
    gui.cMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_BIT_MIX", gui.cMixSlider);
    gui.dTimeAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_DLY_TIME", gui.dTimeSlider);
    gui.dFbAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_DLY_FB", gui.dFbSlider);
    gui.dMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_DLY_MIX", gui.dMixSlider);
    gui.rSizeAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_RVB_SIZE", gui.rSizeSlider);
    gui.rDampAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_RVB_DAMP", gui.rDampSlider);
    gui.rMixAtt = std::make_unique<SliderAttachment>(audioProcessor.apvts, "FX_RVB_MIX", gui.rMixSlider);
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

    gui.table.getHeader().addColumn("File Name", 1, 150);
    gui.table.getHeader().addColumn("Mode", 5, 80);
    gui.table.getHeader().addColumn("Preset Name", 2, 150);
    gui.table.getHeader().addColumn("Author", 3, 100);
    gui.table.getHeader().addColumn("Ver", 4, 50);
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

    /********************
    *
    * 5. Buttons
    *
    *********************/

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
                "Mode: " + item.modeName;

            juce::SystemClipboard::copyTextToClipboard(info);
        }
    };
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

    // --- ADPCM Dir ---
    setupRow(gui.adpcmDirLabel, "Sample Dir:", gui.adpcmDirPathLabel, gui.adpcmDirBrowseBtn);
    gui.adpcmDirPathLabel.setText(audioProcessor.defaultAdpcmDir, juce::dontSendNotification);

    gui.adpcmDirBrowseBtn.onClick = [this] {
        fileChooser = std::make_unique<juce::FileChooser>("Select Default Sample Directory",
            audioProcessor.defaultAdpcmDir.isEmpty() ? juce::File::getSpecialLocation(juce::File::userHomeDirectory) : juce::File(audioProcessor.defaultAdpcmDir));
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
            [this](const juce::FileChooser& fc) {
                auto file = fc.getResult();
                if (file.isDirectory()) {
                    audioProcessor.defaultAdpcmDir = file.getFullPathName();
                    audioProcessor.lastAdpcmDirectory = file;
                    settingsGui.adpcmDirPathLabel.setText(file.getFullPathName(), juce::dontSendNotification);
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
                    settingsGui.adpcmDirPathLabel.setText(audioProcessor.defaultAdpcmDir, juce::dontSendNotification);
                    settingsGui.presetDirPathLabel.setText(audioProcessor.defaultPresetDir, juce::dontSendNotification);

                    // 壁紙再描画
                    loadWallpaperImage();

                    // プリセットリスト更新
                    if (juce::File(audioProcessor.defaultPresetDir).isDirectory()) {
                        presetGui.currentFolder = juce::File(audioProcessor.defaultPresetDir);
                        presetGui.pathLabel.setText(presetGui.currentFolder.getFullPathName(), juce::dontSendNotification);
                        scanPresets(); // リスト更新関数を呼ぶ
                    }
                }
            }
        );
    };

    gui.saveStartupSettingsBtn.setButtonText("Save Current Settings as Default");
    gui.group.addAndMakeVisible(gui.saveStartupSettingsBtn);

    gui.saveStartupSettingsBtn.onClick = [this]
        {
            fileChooser = std::make_unique<juce::FileChooser>("Save Environment Settings as Default",
                juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("init_preset.xml"), "*.xml");

            fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
                [this](const juce::FileChooser& fc) {
                    auto file = fc.getResult();
                    if (file != juce::File()) {
                        audioProcessor.saveEnvironment(file);
                    }
                }
            );

            // 1. 保存先ファイルのパスを作成 (Documents/2686V/init_preset.xml)
            auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
            auto pluginDir = docDir.getChildFile("2686V");

            // フォルダがなければ作る
            if (!pluginDir.exists()) pluginDir.createDirectory();

            auto file = pluginDir.getChildFile("init_preset.xml");

            juce::XmlElement xml("PREF_2686V");

            xml.setAttribute("wallpaperPath", audioProcessor.wallpaperPath);
            xml.setAttribute("defaultAdpcmDir", audioProcessor.defaultAdpcmDir);
            xml.setAttribute("defaultPresetDir", audioProcessor.defaultPresetDir);
            xml.setAttribute("showTooltips", audioProcessor.showTooltips);

            xml.writeTo(file);

            // 3. 書き出し実行
            if (xml.writeTo(file))
            {
                // 成功メッセージ (ネイティブのアラートウィンドウを表示)
                juce::NativeMessageBox::showMessageBoxAsync(
                    juce::AlertWindow::InfoIcon,
                    "Success",
                    "Current parameters saved as default startup sound.\n" + file.getFullPathName(),
                    this
                );
            }
            else
            {
                // 失敗メッセージ
                juce::NativeMessageBox::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Error",
                    "Failed to save preset file.",
                    this
                );
            }
        };
}

void AudioPlugin2686VEditor::setupAboutGui(AboutGuiSet& gui)
{
    // 1. Plugin Name
    gui.page.addAndMakeVisible(gui.pluginNameLabel);
    gui.pluginNameLabel.setText(VstName, juce::dontSendNotification);
    gui.pluginNameLabel.setFont(juce::Font(FontFamily, 64.0f, juce::Font::bold | juce::Font::italic));
    gui.pluginNameLabel.setJustificationType(juce::Justification::centred);

    // 2. Version
    gui.page.addAndMakeVisible(gui.versionLabel);
    gui.versionLabel.setText("Version 0.0.1", juce::dontSendNotification);
    gui.versionLabel.setFont(juce::Font(24.0f));
    gui.versionLabel.setJustificationType(juce::Justification::centred);

    // 3. Copyright
    gui.page.addAndMakeVisible(gui.copyrightLabel);
    gui.copyrightLabel.setText("Copyright (C) 2026 CYROSS", juce::dontSendNotification);
    gui.copyrightLabel.setJustificationType(juce::Justification::centred);

    // 4. Logo (BinaryDataから読み込み)
    gui.page.addAndMakeVisible(gui.vst3Logo);

    // ★修正ポイント: ファイルパスではなく、メモリ(バイナリ)から読み込む
    // 名前空間: VstLogoForAboutData (CMakeで指定したもの)
    // 変数名: ファイル名のドットがアンダースコアになったもの (vst3_logo_png)
    auto logoImg = juce::ImageCache::getFromMemory(
        VstLogoForAboutData::vst3_logo_png,
        VstLogoForAboutData::vst3_logo_pngSize
    );
    gui.vst3Logo.setImage(logoImg);

    // 5. VST Trademark Notice (必須表記)
    gui.page.addAndMakeVisible(gui.vstGuidelineLabel);
    gui.vstGuidelineLabel.setText("VST is a registered trademark of Steinberg Media Technologies GmbH.", juce::dontSendNotification);
    gui.vstGuidelineLabel.setFont(juce::Font(12.0f)); // 小さめでOK
    gui.vstGuidelineLabel.setJustificationType(juce::Justification::centred);
    gui.vstGuidelineLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey); // 目立ちすぎない色に

    // 6. Link
    /*
    gui.page.addAndMakeVisible(gui.webLink);
    gui.webLink.setButtonText("Visit Website");
    gui.webLink.setURL(juce::URL("https://juce.com")); // 仮URL
    */
}

void AudioPlugin2686VEditor::layoutOpnaPage(Fm4GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // --- A. Global Section (Top) ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);
    gui.globalGroup.setBounds(topArea);
    auto globalRect = topArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    globalRect.removeFromTop(TitlePaddingTop); // タイトル回避

    // Globalエリアも少し余裕を持たせて配置
    gui.algSelector.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
    gui.feedbackSlider.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
	gui.lfoFreqSlider.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
    gui.lfoPmsSelector.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
    gui.lfoAmsSelector.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));

    // --- Quality Group ---
    auto qualityArea = pageArea.removeFromTop(QualityGroupHeight);
    gui.qualityGroup.setBounds(qualityArea);
    auto qRect = gui.qualityGroup.getBounds().withTrimmedTop(TitlePaddingTop).reduced(GroupPaddingWidth, GroupPaddingHeight);

    gui.bitSelector.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));
    gui.rateCombo.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        gui.opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);

        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsTtoB(innerRect, 15, 5, &gui.mulLabel[i], &gui.mul[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.dtLabel[i], &gui.dt[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.arLabel[i], &gui.ar[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.drLabel[i], &gui.dr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.srLabel[i], &gui.sr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.slLabel[i], &gui.sl[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.rrLabel[i], &gui.rr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.tlLabel[i], &gui.tl[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.ksLabel[i], &gui.ks[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.seLabel[i], &gui.se[i]);
        layoutComponentsTtoB(innerRect, 15, 0, &gui.seFreqLabel[i], &gui.seFreq[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.fixLabel[i], &gui.fix[i]);
        layoutComponentsTtoB(innerRect, 15, 2, &gui.freqLabel[i], &gui.freq[i]);

        auto btnRow = innerRect.removeFromTop(20);
        int btnW = btnRow.getWidth() / 2;

        gui.freqToZero[i].setBounds(btnRow.removeFromLeft(btnW));
        gui.freqTo440[i].setBounds(btnRow.removeFromLeft(btnW));

        layoutComponentsTtoB(innerRect, 15, 5, &gui.amLabel[i], &gui.am[i]);
        layoutComponentsTtoB(innerRect, 20, 5, &gui.mmlBtnLabel[i], &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutOpnPage(Fm4GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // --- A. Global Section (Top) ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);

    gui.globalGroup.setBounds(topArea);
    auto globalRect = topArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    globalRect.removeFromTop(TitlePaddingTop); // タイトル回避

    // Globalエリアも少し余裕を持たせて配置
    gui.algSelector.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
    gui.feedbackSlider.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));

    // --- Quality Group ---
    auto qualityArea = pageArea.removeFromTop(QualityGroupHeight);

    gui.qualityGroup.setBounds(qualityArea);

    auto qRect = gui.qualityGroup.getBounds().withTrimmedTop(TitlePaddingTop).reduced(GroupPaddingWidth, GroupPaddingHeight);

    gui.bitSelector.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));
    gui.rateCombo.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea.reduced(2));

        // 枠線の内側
        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop); // タイトル(Operator X)回避

        layoutComponentsTtoB(innerRect, 15, 5, &gui.mulLabel[i], &gui.mul[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.dtLabel[i], &gui.dt[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.arLabel[i], &gui.ar[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.drLabel[i], &gui.dr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.srLabel[i], &gui.sr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.slLabel[i], &gui.sl[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.rrLabel[i], &gui.rr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.tlLabel[i], &gui.tl[i]);
        layoutComponentsTtoB(innerRect, 15, 0, &gui.ksLabel[i], &gui.ks[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.fixLabel[i], &gui.fix[i]);
        layoutComponentsTtoB(innerRect, 15, 2, &gui.freqLabel[i], &gui.freq[i]);

        auto btnRow = innerRect.removeFromTop(20);
        int btnW = btnRow.getWidth() / 2;

        gui.freqToZero[i].setBounds(btnRow.removeFromLeft(btnW));
        gui.freqTo440[i].setBounds(btnRow.removeFromLeft(btnW));

        innerRect.removeFromTop(8);

        layoutComponentsTtoB(innerRect, 20, 5, &gui.mmlBtnLabel[i], &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutOplPage(Fm2GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // --- A. Global Section ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);
    gui.globalGroup.setBounds(topArea);

    auto globalRect = topArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    globalRect.removeFromTop(TitlePaddingTop);

    gui.algSelector.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
    gui.feedbackSlider.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));

    // --- Quality Group ---
    auto qualityArea = pageArea.removeFromTop(QualityGroupHeight);

    gui.qualityGroup.setBounds(qualityArea);

    auto qRect = gui.qualityGroup.getBounds().withTrimmedTop(TitlePaddingTop).reduced(GroupPaddingWidth, GroupPaddingHeight);

    gui.bitSelector.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));
    gui.rateCombo.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));

    // --- B. Operators Section ---
    for (int i = 0; i < 2; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);
        gui.opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        layoutComponentsTtoB(innerRect, 15, 5, &gui.mulLabel[i], &gui.mul[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.dtLabel[i], &gui.dt[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.arLabel[i], &gui.ar[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.drLabel[i], &gui.dr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.slLabel[i], &gui.sl[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.rrLabel[i], &gui.rr[i]);
        layoutComponentsTtoB(innerRect, 15, 8, &gui.wsLabel[i], &gui.ws[i]);
        layoutComponentsTtoB(innerRect, 20, 5, &gui.mmlBtnLabel[i], &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutOpllPage(OpllGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // --- Global & Preset Section ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);
    gui.globalGroup.setBounds(topArea);

    auto globalRect = topArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    globalRect.removeFromTop(TitlePaddingTop);

    // Feedback Center
    gui.feedbackSlider.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));

    // --- Quality Group ---
    auto qualityArea = pageArea.removeFromTop(QualityGroupHeight);

    gui.qualityGroup.setBounds(qualityArea);

    auto qRect = gui.qualityGroup.getBounds().withTrimmedTop(TitlePaddingTop).reduced(GroupPaddingWidth, GroupPaddingHeight);

    gui.bitSelector.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));
    gui.rateCombo.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));

    for (int i = 0; i < 2; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);
        gui.opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(20);

        layoutComponentsTtoB(innerRect, 15, 5, &gui.mulLabel[i], &gui.mul[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.arLabel[i], &gui.ar[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.drLabel[i], &gui.dr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.slLabel[i], &gui.sl[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.rrLabel[i], &gui.rr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.tlLabel[i], &gui.tl[i]);
        layoutComponentsTtoB(innerRect, 15, 0, &gui.kslLabel[i], &gui.ksl[i]);
        layoutComponentsTtoB(innerRect, 15, 0, &gui.ksrLabel[i], &gui.ksr[i]);
        layoutComponentsTtoB(innerRect, 15, 0, &gui.amLabel[i], &gui.am[i]);
        layoutComponentsTtoB(innerRect, 15, 0, &gui.vibLabel[i], &gui.vib[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.egTypeLabel[i], &gui.egType[i]);
        layoutComponentsTtoB(innerRect, 20, 5, &gui.mmlBtnLabel[i], &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutOpl3Page(Opl3GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // --- Global Section ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);
    gui.globalGroup.setBounds(topArea);

    auto globalRect = topArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    globalRect.removeFromTop(TitlePaddingTop);

    int globalW = globalRect.getWidth() / 2;
    gui.algSelector.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
    gui.feedbackSlider.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));

    // --- Quality Group ---
    auto qualityArea = pageArea.removeFromTop(QualityGroupHeight);

    gui.qualityGroup.setBounds(qualityArea);

    auto qRect = gui.qualityGroup.getBounds().withTrimmedTop(TitlePaddingTop).reduced(GroupPaddingWidth, GroupPaddingHeight);

    gui.bitSelector.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));
    gui.rateCombo.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));

    // --- Operators Section ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);
        gui.opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(TitlePaddingTop);

        auto place = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            c.setBounds(rect.removeFromTop(FmOpRowH).reduced(2, 5));
        };

        auto placeCombo = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            c.setBounds(rect.removeFromTop(FmOpRowH).withSizeKeepingCentre(rect.getWidth() - 4, 24));
        };

        layoutComponentsTtoB(innerRect, 15, 5, &gui.mulLabel[i], &gui.mul[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.arLabel[i], &gui.ar[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.drLabel[i], &gui.dr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.slLabel[i], &gui.sl[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.rrLabel[i], &gui.rr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.tlLabel[i], &gui.tl[i]);
        layoutComponentsTtoB(innerRect, 15, 8, &gui.wsLabel[i], &gui.ws[i]);
        layoutComponentsTtoB(innerRect, 20, 5, &gui.mmlBtnLabel[i], &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutOpmPage(OpmGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // --- A. Global Section (Top) ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);

    gui.globalGroup.setBounds(topArea);
    auto globalRect = topArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    globalRect.removeFromTop(TitlePaddingTop); // タイトル回避

    // Globalエリアも少し余裕を持たせて配置
    gui.algSelector.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
    gui.feedbackSlider.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
    gui.lfoFreqSlider.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
    gui.pmsSelector.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));
    gui.amsSelector.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));

    // --- Quality Group ---
    auto qualityArea = pageArea.removeFromTop(QualityGroupHeight);

    gui.qualityGroup.setBounds(qualityArea);

    auto qRect = gui.qualityGroup.getBounds().withTrimmedTop(TitlePaddingTop).reduced(GroupPaddingWidth, GroupPaddingHeight);

    gui.bitSelector.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));
    gui.rateCombo.setBounds(qRect.removeFromLeft(QualityParamWidth).reduced(5, 0));

    pageArea.removeFromTop(10); // Spacer

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(FmOpWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea.reduced(2));

        // 枠線の内側
        auto innerRect = opArea.reduced(OpGroupPaddingWidth, OpGroupPaddingHeight);
        innerRect.removeFromTop(20); // タイトル(Operator X)回避

        layoutComponentsTtoB(innerRect, 15, 5, &gui.mulLabel[i], &gui.mul[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.dt1Label[i], &gui.dt1[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.dt2Label[i], &gui.dt2[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.arLabel[i], &gui.ar[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.drLabel[i], &gui.dr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.srLabel[i], &gui.sr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.slLabel[i], &gui.sl[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.rrLabel[i], &gui.rr[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.tlLabel[i], &gui.tl[i]);
        layoutComponentsTtoB(innerRect, 15, 0, &gui.ksLabel[i], &gui.ks[i]);
        layoutComponentsTtoB(innerRect, 15, 5, &gui.fixLabel[i], &gui.fix[i]);
        layoutComponentsTtoB(innerRect, 15, 2, &gui.freqLabel[i], &gui.freq[i]);

        auto btnRow = innerRect.removeFromTop(20);
        int btnW = btnRow.getWidth() / 2;

        gui.freqToZero[i].setBounds(btnRow.removeFromLeft(btnW));
        gui.freqTo440[i].setBounds(btnRow.removeFromLeft(btnW));

        innerRect.removeFromTop(5);

        layoutComponentsTtoB(innerRect, 20, 5, &gui.mmlBtnLabel[i], &gui.mmlBtn[i]);
    }
}

void AudioPlugin2686VEditor::layoutSsgPage(SsgGuiSet& gui, juce::Rectangle<int> content)
{
    // ==================================
    // Layout for SSG Page (自動伸縮対応)
    // ==================================
    // 画面高さが伸びた分、各エリアも縦に伸びます
    auto pageArea = content.withZeroOrigin();
    auto leftArea = pageArea.removeFromLeft(480);
    auto rightArea = pageArea.removeFromLeft(480);

    auto voiceArea = leftArea.removeFromTop(320);

    // --- Voice Group ---
    gui.voiceGroup.setBounds(voiceArea.reduced(5));
    auto vRect = gui.voiceGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    vRect.removeFromTop(20);

    gui.waveSelector.setBounds(vRect.removeFromTop(24).withSizeKeepingCentre(160, 24));

    vRect.removeFromTop(20);

    auto pRect = vRect.removeFromTop(160);

    // Knobs Layout
    int knobW = pRect.getWidth() / 4;

    gui.levelSlider.setBounds(pRect.removeFromLeft(knobW).reduced(5));
    gui.noiseSlider.setBounds(pRect.removeFromLeft(knobW).reduced(5));
    gui.noiseFreqSlider.setBounds(pRect.removeFromLeft(knobW).reduced(5));

    auto mixArea = pRect;
    gui.mixSlider.setBounds(mixArea.reduced(5));

    int btnTotalW = 120;
    int btnW = btnTotalW / 3;
    int centerX = gui.mixSlider.getBounds().getCentreX();
    int startX = centerX - (btnTotalW / 2);
    int startY = gui.mixSlider.getBottom() + 0;

    juce::Rectangle<int> btnRect(startX, startY, btnTotalW, 20);
    gui.mixSetTone.setBounds(btnRect.removeFromLeft(btnW).reduced(2, 0));
    gui.mixSetMix.setBounds(btnRect.removeFromLeft(btnW).reduced(2, 0));
    gui.mixSetNoise.setBounds(btnRect.removeFromLeft(btnW).reduced(2, 0));

    auto filterArea = leftArea.removeFromTop(320);

    // --- Filter Group ---
    gui.filterGroup.setBounds(filterArea.reduced(5));
    auto fRect = gui.filterGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    fRect.removeFromTop(20);

    gui.adsrBypassButton.setBounds(fRect.removeFromTop(30).withSizeKeepingCentre(160, 30));
    fRect.removeFromTop(20);

    auto evRect = fRect.removeFromTop(160);

    // ADSR Sliders
    int adsrW = evRect.getWidth() / 4;

    gui.attackSlider.setBounds(evRect.removeFromLeft(adsrW).reduced(5));
    gui.decaySlider.setBounds(evRect.removeFromLeft(adsrW).reduced(5));
    gui.sustainSlider.setBounds(evRect.removeFromLeft(adsrW).reduced(5));
    gui.releaseSlider.setBounds(evRect.removeFromLeft(adsrW).reduced(5));

    // --- SSG Right Pane ---

    // --- Quality Group ---
    auto qualityArea = rightArea.removeFromTop(QualityGroupHeight);

    gui.qualityGroup.setBounds(qualityArea);

    auto qRect = gui.qualityGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);
    int qRowWidth = 160;
 
    qRect.removeFromTop(20);

    gui.bitSelector.setBounds(qRect.removeFromLeft(qRowWidth).reduced(5, 0));
    gui.rateCombo.setBounds(qRect.removeFromLeft(qRowWidth).reduced(5, 0));

    float waveParam = *audioProcessor.apvts.getRawParameterValue("SSG_WAVEFORM");
    int waveMode = (waveParam > 0.5f) ? 1 : 0;

    // Wave Group
    auto waveArea = rightArea.removeFromTop(240);

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
        gui.dutyVarSlider.setValue(true);
        gui.dutyInvertButton.setVisible(true);

        gui.dutyGroup.setBounds(waveArea);
        auto dRect = gui.dutyGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

        dRect.removeFromTop(20);

        gui.dutyModeSelector.setBounds(dRect.removeFromTop(30).reduced(5));

        dRect.removeFromTop(10);

        gui.dutyInvertButton.setBounds(dRect.removeFromTop(30).reduced(5));

        dRect.removeFromTop(10);

        float dutyModeVal = *audioProcessor.apvts.getRawParameterValue("SSG_DUTY_MODE");
        if (dutyModeVal < 0.5f) {
            gui.dutyPresetSelector.setVisible(true);
            gui.dutyVarSlider.setVisible(false);
            gui.dutyVarLabel.setVisible(false);
            gui.dutyPresetSelector.setBounds(dRect.removeFromTop(30).withSizeKeepingCentre(180, 24));
        }
        else {
            gui.dutyPresetSelector.setVisible(false);
            gui.dutyVarSlider.setVisible(true);
            gui.dutyVarLabel.setVisible(true);
            gui.dutyVarSlider.setBounds(dRect.removeFromTop(120).withSizeKeepingCentre(100, 120));
        }
    }
    else // Triangle
    {
        gui.dutyGroup.setVisible(false);
        gui.triGroup.setVisible(true);

        gui.dutyModeSelector.setVisible(false);
        gui.dutyInvertButton.setVisible(false);
        gui.dutyPresetSelector.setVisible(false);
        gui.dutyVarSlider.setVisible(false);
        gui.dutyVarLabel.setVisible(false);

        gui.triKeyTrackButton.setVisible(true);
        gui.triFreqSlider.setVisible(true);
        gui.triFreqLabel.setVisible(true);
        gui.triPeakSlider.setVisible(true);
        gui.triPeakLabel.setVisible(true);
        gui.triSetSawDown.setVisible(true);
        gui.triSetTri.setVisible(true); gui.triSetSawUp.setVisible(true);

        gui.triGroup.setBounds(waveArea);
        auto tRect = gui.triGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

        tRect.removeFromTop(20);

        gui.triKeyTrackButton.setBounds(tRect.removeFromTop(30).reduced(5));

        tRect.removeFromTop(10);

        bool isKeyTrack = gui.triKeyTrackButton.getToggleState();
        gui.triFreqSlider.setVisible(!isKeyTrack);
        gui.triFreqLabel.setVisible(!isKeyTrack);

        juce::Rectangle<int> peakSliderArea;

        if (!isKeyTrack) {
            auto sliderRow = tRect.removeFromTop(40);
            int w = sliderRow.getWidth() / 2;
            gui.triFreqSlider.setBounds(sliderRow.removeFromLeft(w).reduced(5));
            peakSliderArea = sliderRow;
            gui.triPeakSlider.setBounds(peakSliderArea.reduced(5));
        }
        else {
            gui.triPeakSlider.setBounds(tRect.removeFromTop(60).reduced(5));
        }

        auto buttonRow = tRect.removeFromTop(30);

        int btnW3 = 150 / 3;
        int cx = gui.triPeakSlider.getBounds().getCentreX();
        if (!isKeyTrack) cx = peakSliderArea.getCentreX();

        juce::Rectangle<int> bRect(cx - 75, buttonRow.getY(), 150, 24);
        gui.triSetSawDown.setBounds(bRect.removeFromLeft(btnW3).reduced(2, 0));
        gui.triSetTri.setBounds(bRect.removeFromLeft(btnW3).reduced(2, 0));
        gui.triSetSawUp.setBounds(bRect.removeFromLeft(btnW3).reduced(2, 0));
    }

    // HW Env Group
    auto envArea = rightArea.removeFromTop(240);
    gui.envGroup.setBounds(envArea);
    auto eRect = gui.envGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    eRect.removeFromTop(20);

    gui.envEnableButton.setBounds(eRect.removeFromTop(60).reduced(5));
    eRect.removeFromTop(10);
    gui.shapeSelector.setBounds(eRect.removeFromTop(60).reduced(5));
    eRect.removeFromTop(10);
    gui.periodSlider.setBounds(eRect.removeFromTop(60).reduced(5));
}

void AudioPlugin2686VEditor::layoutWtPage(WtGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();
    auto leftArea = pageArea.removeFromLeft(WtLeftWidth);

    gui.filterGroup.setBounds(leftArea.reduced(5));
    auto fRect = gui.filterGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    fRect.removeFromTop(10);

    // Level
    layoutComponentsTtoB(fRect, 20, 10, &gui.levelLabel, &gui.levelSlider);

    // ADSR
    layoutComponentsTtoB(fRect, 20, 5, &gui.attackLabel, &gui.attackSlider);
    layoutComponentsTtoB(fRect, 20, 5, &gui.decayLabel, &gui.decaySlider);
    layoutComponentsTtoB(fRect, 20, 5, &gui.sustainLabel, &gui.sustainSlider);
    layoutComponentsTtoB(fRect, 20, 5, &gui.releaseLabel, &gui.releaseSlider);

    // Right: Properties
    auto rightArea = pageArea.removeFromLeft(WtRightWidth);
    gui.propGroup.setBounds(rightArea.reduced(5));
    auto pRect = gui.propGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    pRect.removeFromTop(20);

    // Waveform Selector (Top)
    gui.waveSelector.setBounds(pRect.removeFromTop(30).withSizeKeepingCentre(200, 24));

    pRect.removeFromTop(20);

    // Bit & Size (Middle)
    auto configRow = pRect.removeFromTop(30);
    int configRowWidth = 160;
    gui.bitSelector.setBounds(configRow.removeFromLeft(configRowWidth).reduced(5, 0));
    gui.sizeSelector.setBounds(configRow.removeFromLeft(configRowWidth).reduced(5, 0));
    gui.rateCombo.setBounds(configRow.removeFromLeft(configRowWidth).reduced(5, 0));

    pRect.removeFromTop(20);

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

            int sliderW = sliderRect.getWidth() / 32;
            for (int i = 0; i < 32; ++i)
            {
                gui.customSliders32[i].setBounds(sliderRect.removeFromLeft(WtCustomSliderWidth));
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

        pRect.removeFromTop(10);
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

    pRect.removeFromTop(20);

    // Mod Sliders
    auto modRow = pRect.removeFromTop(100);
    gui.modDepthSlider.setBounds(modRow.removeFromLeft(modRow.getWidth() / 2).withSizeKeepingCentre(100, 100));
    gui.modSpeedSlider.setBounds(modRow.withSizeKeepingCentre(100, 100));
}

void AudioPlugin2686VEditor::layoutRhythmPage(RhythmGuiSet& gui, juce::Rectangle<int> content)
{
    auto applyPads = [&](juce::Rectangle<int>& area, int width, int start, int length)
        {
            for (int i = start; i < start + length; ++i)
            {
                auto& pad = gui.pads[i];
                auto padArea = area.removeFromLeft(width).reduced(2);

                pad.group.setBounds(padArea);

                // Indent inside the group
                auto area = padArea.reduced(5, 15);

                area.removeFromTop(10);

                // --- Layout Components Top to Bottom ---

                // 1. Load Button
                layoutComponentsTtoB(area, 20, 2, &pad.loadButton);

                // 2. File Name Label
                layoutComponentsTtoB(area, 15, 5, &pad.fileNameLabel);

                // 3. Clear Button
                layoutComponentsTtoB(area, 20, 5, &pad.clearButton);

                // 4. One Shot Toggle
                layoutComponentsTtoB(area, 20, 5, &pad.oneShotButton);

                // 5. Note
                layoutComponentsTtoB(area, 15, 5, &pad.noteLabel, &pad.noteSlider);

                // 6. Mode
                layoutComponentsTtoB(area, 15, 5, &pad.modeLabel, &pad.modeCombo);

                // 7. Rate
                layoutComponentsTtoB(area, 15, 5, &pad.rateLabel, &pad.rateCombo);

                // 8. Pan
                layoutComponentsTtoB(area, 15, 2, &pad.panLabel, &pad.panSlider);

                // 9. Pan Buttons
                auto btnRow = area.removeFromTop(15);
                int btnW = btnRow.getWidth() / 3;
                pad.btnPanL.setBounds(btnRow.removeFromLeft(btnW));
                pad.btnPanC.setBounds(btnRow.removeFromLeft(btnW));
                pad.btnPanR.setBounds(btnRow);

                area.removeFromTop(5);

                // 9. Vol
                layoutComponentsTtoB(area, 15, 5, &pad.volLabel, &pad.volSlider);

                // 10. RR
                layoutComponentsTtoB(area, 15, 5, &pad.rrLabel, &pad.rrSlider);
            }

        };

    // Top section for Master Volume
    auto pageArea = content.withZeroOrigin();

    // --- A. Global Section (Top) ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);

    gui.group.setBounds(topArea);
    auto globalRect = topArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    globalRect.removeFromTop(TitlePaddingTop); // タイトル回避

    gui.levelSlider.setBounds(globalRect.removeFromLeft(TopParamWidth).reduced(20, 10));

    auto topPadsArea = pageArea.removeFromTop(320);
    auto bottomPadsArea = pageArea.removeFromTop(320);

    // Remaining area for 8 pads
    int topPadWidth = topPadsArea.getWidth() / 4;

    applyPads(topPadsArea, topPadsArea.getWidth() / 4, 0, 4);
    applyPads(bottomPadsArea, bottomPadsArea.getWidth() / 4, 4, 4);
}

void AudioPlugin2686VEditor::layoutAdpcmPage(AdpcmGuiSet& gui, juce::Rectangle<int> content)
{
    auto area = content.withZeroOrigin();
    gui.group.setBounds(area);

    auto inner = area.reduced(GroupPaddingWidth, GroupPaddingHeight);

    inner.removeFromTop(20);

    auto fileRow = inner.removeFromTop(30);
    gui.loadButton.setBounds(fileRow.removeFromLeft(100));
    fileRow.removeFromLeft(10);
    gui.fileNameLabel.setBounds(fileRow);
    auto loadBounds = gui.loadButton.getBounds();
    gui.clearButton.setBounds(loadBounds.getRight() + 10, loadBounds.getY(), 50, 20);

    inner.removeFromTop(10);

    auto settingsRow = inner.removeFromTop(45);
    int w = settingsRow.getWidth() / 2;

    auto modeArea = settingsRow.removeFromLeft(w).reduced(5, 0);
    gui.modeLabel.setBounds(modeArea.removeFromTop(20)); // Attached labels need space reserved if we manually place them? 
    // Actually attatchLabelToComponent places label relative to component, 
    // but here we are setting bounds for the combo/slider. 
    // Since we attached them, we just need to place the component, the label follows.
    // However, to ensure vertical spacing, we reserve space implicitly.
    gui.modeCombo.setBounds(modeArea);

    auto rateArea = settingsRow.reduced(5, 0);
    gui.rateLabel.setBounds(rateArea.removeFromTop(20)); // Only needed if NOT attached. 
    // Note: In setupCombo we called setupLabel which ADDS label to page.
    // attatchLabelToComponent was NOT called for Mode/Rate in setupAdpcmGui above.
    // So we DO need to place labels manually for Mode/Rate.
    gui.rateCombo.setBounds(rateArea);

    inner.removeFromTop(20);
    // 3. Volume & Loop
    auto volRow = inner.removeFromTop(50);
    // Loop
    gui.loopButton.setBounds(volRow.removeFromRight(100).withSizeKeepingCentre(80, 25));
    // Vol
    gui.levelSlider.setBounds(volRow.reduced(10, 0)); // Attached label "Master Vol" appears above/left

    inner.removeFromTop(20);

    // 4. Pan Section
    auto panRow = inner.removeFromTop(60);
    gui.panSlider.setBounds(panRow.removeFromTop(25).withSizeKeepingCentre(200, 20));
    panRow.removeFromTop(5);

    auto btnRow = panRow.withSizeKeepingCentre(200, 20);
    int btnW = btnRow.getWidth() / 3;
    gui.btnPanL.setBounds(btnRow.removeFromLeft(btnW));
    gui.btnPanC.setBounds(btnRow.removeFromLeft(btnW));
    gui.btnPanR.setBounds(btnRow);

    inner.removeFromTop(20);

    // 5. ADSR Section
    int aw = inner.getWidth() / 4;
    auto adsrRow = inner.removeFromTop(50);

    // Sliders with attached labels
    gui.attackSlider.setBounds(adsrRow.removeFromLeft(aw).reduced(5));
    gui.decaySlider.setBounds(adsrRow.removeFromLeft(aw).reduced(5));
    gui.sustainSlider.setBounds(adsrRow.removeFromLeft(aw).reduced(5));
    gui.releaseSlider.setBounds(adsrRow.removeFromLeft(aw).reduced(5));
}

void AudioPlugin2686VEditor::layoutFxPage(FxGuiSet& gui, juce::Rectangle<int> content)
{
    auto layoutKnob = [&](juce::Rectangle<int> r, juce::Slider& s, juce::Label& l) {
        auto labelArea = r.removeFromTop(20);
        l.setBounds(labelArea);

        auto knobArea = r.removeFromTop(FxKnobHeight);
        s.setBounds(knobArea.withSizeKeepingCentre(FxKnobWidth, knobArea.getHeight()).reduced(0, 5));

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
        s.setBounds(knobArea.withSizeKeepingCentre(FxKnobWidth, knobArea.getHeight()).reduced(0, 5));

        r.removeFromTop(5);

        // 下部: ボタンエリア (高さ20px)
        auto btnArea = r.removeFromTop(20).reduced(5, 0); // 左右少し余白

        b1.setBounds(btnArea.removeFromLeft(FxMixButtonWidth + 40).withSizeKeepingCentre(FxMixButtonWidth, btnArea.getHeight())); // 左側
        b3.setBounds(btnArea.removeFromRight(FxMixButtonWidth + 40).withSizeKeepingCentre(FxMixButtonWidth, btnArea.getHeight())); // 右側
        b2.setBounds(btnArea.withSizeKeepingCentre(FxMixButtonWidth, btnArea.getHeight())); // 中央
    };

    auto area = content.withZeroOrigin().withSizeKeepingCentre(FxWidth, FxHeight);

    // 上部にバイパスボタンを配置
    auto headerArea = area.removeFromTop(30);
    gui.bypassToggle.setBounds(headerArea.removeFromRight(FxBypassHeight));

    area.removeFromTop(10);

    auto leftCol = area.removeFromLeft(area.getWidth() / 2 - 5);
    auto rightCol = area.removeFromRight(leftCol.getWidth()); // 残りの右側

    // Left

    // 1. Tremolo
    auto tremoloArea = leftCol.removeFromTop(FxGroupHeight);
    gui.tremGroup.setBounds(tremoloArea);
    tremoloArea.removeFromTop(20);

    layoutKnob(tremoloArea.removeFromLeft(FxKnobWidth + 20), gui.tRateSlider, gui.tRateLabel);
    layoutMixSection(tremoloArea.removeFromRight(FxKnobAreaWidth + 20), gui.tMixSlider, gui.tMixLabel, gui.tDryBtn, gui.tHalfBtn, gui.tWetBtn);
    layoutKnob(tremoloArea.withSizeKeepingCentre(FxKnobWidth, tremoloArea.getHeight()), gui.tDepthSlider, gui.tDepthLabel);

    // 2. Vibrato
    auto vibratoArea = leftCol.removeFromTop(FxGroupHeight);
    gui.vibGroup.setBounds(vibratoArea);
    vibratoArea.removeFromTop(20);

    layoutKnob(vibratoArea.removeFromLeft(FxKnobWidth + 20), gui.vRateSlider, gui.vRateLabel);
    layoutMixSection(vibratoArea.removeFromRight(FxKnobAreaWidth + 20), gui.vMixSlider, gui.vMixLabel, gui.vDryBtn, gui.vHalfBtn, gui.vWetBtn);
    layoutKnob(vibratoArea.withSizeKeepingCentre(FxKnobWidth, vibratoArea.getHeight()), gui.vDepthSlider, gui.vDepthLabel);

    // Crusher
    auto crushArea = leftCol.removeFromTop(FxGroupHeight);
    gui.crushGroup.setBounds(crushArea);
    crushArea.removeFromTop(20);

    // ... (layoutKnobで3つ配置) ...
    layoutKnob(crushArea.removeFromLeft(FxKnobWidth + 20), gui.cRateSlider, gui.cRateLabel);
    layoutMixSection(crushArea.removeFromRight(FxKnobAreaWidth + 20), gui.cMixSlider, gui.cMixLabel, gui.cDryBtn, gui.cHalfBtn, gui.cWetBtn);
    layoutKnob(crushArea.withSizeKeepingCentre(FxKnobWidth, crushArea.getHeight()), gui.cBitsSlider, gui.cBitsLabel);

    // Right

    // Delay
    auto delayArea = rightCol.removeFromTop(FxGroupHeight);
    gui.delayGroup.setBounds(delayArea);
    delayArea.removeFromTop(20);

    layoutKnob(delayArea.removeFromLeft(FxKnobWidth + 20), gui.dTimeSlider, gui.dTimeLabel);
    layoutMixSection(delayArea.removeFromRight(FxKnobAreaWidth + 20), gui.dMixSlider, gui.dMixLabel, gui.dDryBtn, gui.dHalfBtn, gui.dWetBtn);
    layoutKnob(delayArea.withSizeKeepingCentre(FxKnobWidth, delayArea.getHeight()), gui.dFbSlider, gui.dFbLabel);

    area.removeFromTop(10);

    // Lower: Reverb
    auto reverbArea = rightCol.removeFromTop(FxGroupHeight);
    gui.reverbGroup.setBounds(reverbArea);
    reverbArea.removeFromTop(20);

    layoutKnob(reverbArea.removeFromLeft(FxKnobWidth + 20), gui.rSizeSlider, gui.rSizeLabel);
    layoutMixSection(reverbArea.removeFromRight(FxKnobAreaWidth + 20), gui.rMixSlider, gui.rMixLabel, gui.rDryBtn, gui.rHalfBtn, gui.rWetBtn);
    layoutKnob(reverbArea.withSizeKeepingCentre(FxKnobWidth, reverbArea.getHeight()), gui.rDampSlider, gui.rDampLabel);
}

void AudioPlugin2686VEditor::layoutPresetPage(PresetGuiSet& gui, juce::Rectangle<int> content)
{
    auto area = content.withZeroOrigin().reduced(GlobalPaddingWidth, GlobalPaddingHeight);

    // Path Label (Top)
    gui.pathLabel.setBounds(area.removeFromTop(24));
    area.removeFromTop(5);

    // Left: List (60%)
    auto listArea = area.removeFromLeft(area.getWidth() * 0.6);
    gui.table.setBounds(listArea);

    area.removeFromLeft(10); // Gap

    // Right: Info & Buttons
    auto rightArea = area;

    // Metadata Group
    gui.metaGroup.setBounds(rightArea.removeFromTop(200));
    auto metaRect = gui.metaGroup.getBounds().reduced(15, 25);

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

    rightArea.removeFromTop(20);

    // Buttons
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

void AudioPlugin2686VEditor::layoutSettingsPage(SettingsGuiSet& gui, juce::Rectangle<int> content)
{
    auto area = content.withZeroOrigin().reduced(GlobalPaddingWidth, GlobalPaddingHeight);
    gui.group.setBounds(area);

    auto inner = area.reduced(20, 30);
    int rowH = 30;
    int gap = 15;

    // 1. Wallpaper
    auto row1 = inner.removeFromTop(rowH);
    gui.wallpaperLabel.setBounds(row1.removeFromLeft(80));
    gui.wallpaperClearBtn.setBounds(row1.removeFromRight(60).reduced(2));
    gui.wallpaperBrowseBtn.setBounds(row1.removeFromRight(80).reduced(2));
    gui.wallpaperPathLabel.setBounds(row1.reduced(5, 0));

    inner.removeFromTop(gap);

    // 2. ADPCM Dir
    auto row2 = inner.removeFromTop(rowH);
    gui.adpcmDirLabel.setBounds(row2.removeFromLeft(80));
    gui.adpcmDirBrowseBtn.setBounds(row2.removeFromRight(80).reduced(2));
    gui.adpcmDirPathLabel.setBounds(row2.reduced(5, 0));

    inner.removeFromTop(gap);

    // 3. Preset Dir
    auto row3 = inner.removeFromTop(rowH);
    gui.presetDirLabel.setBounds(row3.removeFromLeft(80));
    gui.presetDirBrowseBtn.setBounds(row3.removeFromRight(80).reduced(2));
    gui.presetDirPathLabel.setBounds(row3.reduced(5, 0));


    inner.removeFromTop(40); // Gap for Tooltip Button

    // 4. Tooltip Visible Row
    auto rowTooltip = inner.removeFromTop(rowH);
    gui.tooltipLabel.setBounds(rowTooltip.removeFromLeft(140));
    gui.tooltipToggle.setBounds(rowTooltip.removeFromLeft(60));

    inner.removeFromTop(40); // Gap for IO Buttons

    // 5. Config IO Buttons
    auto row4 = inner.removeFromTop(40);
    int btnW = 120;
    // Center two buttons
    auto btnArea = row4.withSizeKeepingCentre(btnW * 3 + 40, 30);
    gui.loadSettingsBtn.setBounds(btnArea.removeFromLeft(btnW));
    btnArea.removeFromLeft(20);
    gui.saveSettingsBtn.setBounds(btnArea.removeFromLeft(btnW));
    btnArea.removeFromLeft(20);
    gui.saveStartupSettingsBtn.setBounds(btnArea.removeFromLeft(btnW));
}

void AudioPlugin2686VEditor::layoutAboutPage(AboutGuiSet& gui, juce::Rectangle<int> content)
{
    auto area = content.withZeroOrigin().reduced(GlobalPaddingWidth, GlobalPaddingHeight);

    // Name & Ver
    gui.pluginNameLabel.setBounds(area.removeFromTop(80));
    gui.versionLabel.setBounds(area.removeFromTop(40));
    gui.copyrightLabel.setBounds(area.removeFromTop(30));

    area.removeFromTop(20);
    // Logo Row
        // ロゴの元サイズ比率を維持して中央配置
    auto logoArea = area.removeFromTop(80);
    gui.vst3Logo.setBounds(logoArea.withSizeKeepingCentre(150, 60)); // 枠内で最大150x60に収める等の調整

    area.removeFromTop(10);

    // Guideline Text
    gui.vstGuidelineLabel.setBounds(area.removeFromTop(20));
}

// ヘルパー: プリセットロード処理 (共通化)
void AudioPlugin2686VEditor::loadPresetFile(const juce::File& file)
{
    audioProcessor.loadPreset(file);

    presetGui.nameEditor.setText(audioProcessor.presetName);
    presetGui.authorEditor.setText(audioProcessor.presetAuthor);
    presetGui.versionEditor.setText(audioProcessor.presetVersion);
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
    auto files = presetGui.currentFolder.findChildFiles(juce::File::findFiles, false, "*.xml");

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
            item.name = xml->getStringAttribute("presetName", "(No Name)");
            item.author = xml->getStringAttribute("presetAuthor", "");
            item.version = xml->getStringAttribute("presetVersion", "");
            item.modeName = xml->getStringAttribute("activeModeName", "-");
        }
        else
        {
            item.name = "Invalid XML";
        }

        presetGui.items.push_back(item);
    }

    // リスト更新
    presetGui.table.updateContent();
    presetGui.table.repaint();
}

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
}

void AudioPlugin2686VEditor::buttonClicked(juce::Button* button)
{
    // ADPCM Load Buttons
    if (button == &adpcmGui.loadButton)
    {
        // ... (Existing ADPCM load logic) ...
        auto fileFilter = audioProcessor.formatManager.getWildcardForAllFormats();
        fileChooser = std::make_unique<juce::FileChooser>("Select an Audio file...", audioProcessor.lastAdpcmDirectory, fileFilter);
        auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    audioProcessor.loadAdpcmFile(file);
                    adpcmGui.fileNameLabel.setText(file.getFileName(), juce::dontSendNotification);
                    audioProcessor.lastAdpcmDirectory = file.getParentDirectory();
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
                fileChooser = std::make_unique<juce::FileChooser>("Load Sample for Pad " + juce::String(i + 1), audioProcessor.lastAdpcmDirectory, fileFilter);
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

                            audioProcessor.lastAdpcmDirectory = file.getParentDirectory();
                        }
                });

                return; // Found
            }
        }
    }
}

void AudioPlugin2686VEditor::mouseDown(const juce::MouseEvent& event)
{
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
                case RegisterType::FmRate: newVal = RegisterConverter::convertFmRate(regValue); break;
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

void AudioPlugin2686VEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "MODE")
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
    // AR
    val = getValue("AR", 31);
    if (val >= 0) gui.ar[opIndex].setValue(RegisterConverter::convertFmRate(val), juce::sendNotification);
    // DR
    val = getValue("DR", 31);
    if (val >= 0) gui.dr[opIndex].setValue(RegisterConverter::convertFmRate(val), juce::sendNotification);
    // SR (Check if struct has SR - OPL/OPLL/OPL3 usually don't, OPNA/OPN/OPM do)
    // using constexpr if or SFINAE is better, but here we assume T is compatible or we specialize.
    // Hack: Check if class has 'sr'. Fm4GuiSet(OPNA/OPN/OPM) has SR. Fm2GuiSet(OPL) doesn't (it has RR only? No, OPL has AR,DR,SL,RR).
    // OPL doesn't have SR. So we need specialization or runtime check?
    // Let's assume this template is for 4-Op (OPNA/OPN/OPM) mainly.
    // For 2-op, we can ignore SR.

    // RR
    val = getValue("RR", 15);
    if (val >= 0) gui.rr[opIndex].setValue(RegisterConverter::convertFmRate(val), juce::sendNotification);
    // SL
    val = getValue("SL", 15);
    if (val >= 0) gui.sl[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);
    // TL
    if constexpr (!std::is_same<T, Fm2GuiSet>::value) {
        val = getValue("TL", 127);
        if (val >= 0) gui.tl[opIndex].setValue(RegisterConverter::convertFmTl(val), juce::sendNotification);
    }
    // MUL
    val = getValue("MUL", 15);
    if (val >= 0) gui.mul[opIndex].setValue((double)RegisterConverter::convertFmMul(val), juce::sendNotification);

    // DT / DT1 / DT2
    if constexpr (std::is_same<T, OpmGuiSet>::value) {
        val = getValue("DT1", 7); // OPM uses DT1
        if (val >= 0) gui.dt1[opIndex].setValue((double)val, juce::sendNotification);

        val = getValue("DT2", 7); // OPM uses DT2
        if (val >= 0) gui.dt2[opIndex].setValue((double)val, juce::sendNotification);
    }
    else if constexpr (std::is_same<T, Fm4GuiSet>::value) {
        val = getValue("DT", 7);
        if (val >= 0) gui.dt[opIndex].setValue((double)RegisterConverter::convertFmDt(val), juce::sendNotification);
    }

    // SR (Only for 4-op types usually, logic specific)
    if constexpr (std::is_same<T, Fm4GuiSet>::value || std::is_same<T, OpmGuiSet>::value) {
        val = getValue("SR", 31);
        if (val >= 0) gui.sr[opIndex].setValue(RegisterConverter::convertFmRate(val), juce::sendNotification);
    }
}
