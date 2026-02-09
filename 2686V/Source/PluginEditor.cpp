#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cstdio>

AudioPlugin2686VEditor::AudioPlugin2686VEditor(AudioPlugin2686V& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    wtGui.page.addComponentListener(this);
    tabs.getTabbedButtonBar().addChangeListener(this);

    setupLogo();

    setSize(WindowWidth, WindowHeight);

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

	setupTabs(tabs);

    int currentMode = (int)*audioProcessor.apvts.getRawParameterValue("MODE");
    tabs.setCurrentTabIndex(currentMode);
}

AudioPlugin2686VEditor::~AudioPlugin2686VEditor()
{
    tabs.getTabbedButtonBar().removeChangeListener(this);
    wtGui.page.removeComponentListener(this);
}

void AudioPlugin2686VEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &tabs.getTabbedButtonBar())
    {
        // 0:OPNA, 1:OPN, 2:OPL, ...
        int targetMode = tabs.getCurrentTabIndex();

        if (targetMode < 0) targetMode = 0;
        if (targetMode > TabNumber) targetMode = TabNumber;

        auto* param = audioProcessor.apvts.getParameter("MODE");

        if (param != nullptr)
        {
            float normalizedValue = param->getNormalisableRange().convertTo0to1((float)targetMode);

            param->beginChangeGesture();
            param->setValueNotifyingHost(normalizedValue);
            param->endChangeGesture();
        }
    }
}

void AudioPlugin2686VEditor::paint(juce::Graphics& g)
{
	drawBg(g);
}

void AudioPlugin2686VEditor::resized()
{
    logoLabel.setBounds(getLocalBounds().reduced(20));

    auto area = getLocalBounds();

    tabs.setBounds(area);

    // タブの中身（コンテンツ領域）
    auto content = tabs.getLocalBounds();
    content.removeFromTop(tabs.getTabBarDepth());
    content.reduce(TopGroupPaddingWidth, TopGroupPaddingHeight); // 全体の余白

    layoutOpnaPage(opnaGui, content);
    layoutOpnPage(opnGui, content);
    layoutOplPage(oplGui, content);
	layoutOpllPage(opllGui, content);
	layoutOpl3Page(opl3Gui, content);
	layoutOpmPage(opmGui, content);
	layoutSsgPage(ssgGui, content);
    layoutWtPage(wtGui);
	layoutRhythmPage(rhythmGui, content);
	layoutAdpcmPage(adpcmGui, content);
}

void AudioPlugin2686VEditor::drawBg(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0f));
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
    logoLabel.setText(VstName, juce::dontSendNotification);

    // ★フォント変更: Bold + Italic, サイズ 128.0f
    logoLabel.setFont(juce::Font(FontFamily, LogoFontSize, juce::Font::bold | juce::Font::italic));

    // ★右下寄せ
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
}

void AudioPlugin2686VEditor::setupGroup(SetupGroupParams& params)
{
    params.page.addAndMakeVisible(params.group);
    params.group.setText(params.title);
    params.group.setTextLabelPosition(juce::Justification::centredLeft);
    params.group.toBack(); // 最背面へ

}

void AudioPlugin2686VEditor::setupLabel(SetupLabelParams& params)
{
    params.page.addAndMakeVisible(params.label);
    params.label.setText(params.title, juce::dontSendNotification);
    params.label.setSize(params.width, params.height);
    params.label.attachToComponent(&params.parent, false);
    params.label.setJustificationType(juce::Justification::centred);
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
        auto& slider = params.wts[i];
        params.page.addAndMakeVisible(slider);

        // バーグラフ風の見た目にする
        slider.setSliderStyle(juce::Slider::LinearBarVertical);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setRange(-1.0, 1.0);

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

	SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Algorithm / Feedback / LFO" };
    setupGroup(groupParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPNA_ALGORITHM", "Algorithm", algItems);
    setupCombo(algParams);
	SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPNA_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);
    SetupSliderParams fqParams = SetupSliderParams::create(gui.page, gui.lfoFreqSlider, gui.lfoFreqLabel, gui.lfoFreqAtt, "OPNA_LFO_FREQ", "LFO FREQ");
    setupSlider(fqParams);
    SetupComboParams pmsParams = SetupComboParams::create(gui.page, gui.lfoPmsSelector, gui.lfoPmsLabel, gui.lfoPmsAtt, "OPNA_LFO_PMS", "LFO PMS", pmsItems);
    setupCombo(pmsParams);
    SetupComboParams amsParams = SetupComboParams::create(gui.page, gui.lfoAmsSelector, gui.lfoAmsLabel, gui.lfoAmsAtt, "OPNA_LFO_AMS", "LFO AMS", amsItems);
    setupCombo(amsParams);

    // Operators
	SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPNA_OP" + juce::String(i) + "_";

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + "DT", "DT");
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::create(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + "KS", "KS", ksItems);
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR");
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL");
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + "SR", "SR");
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
        setupSlider(rrParams);
        SetupComboParams seParams = SetupComboParams::create(gui.page, gui.se[i], gui.seLabel[i], gui.seAtt[i], paramPrefix + "SE", "SE", seItems);
        setupCombo(seParams);
        SetupToggleButtonParams fixParams = SetupToggleButtonParams::createOp(gui.page, gui.fix[i], gui.fixLabel[i], gui.fixAtt[i], paramPrefix + "FIX", "FIX");
        setupToggleButton(fixParams);
        SetupSliderParams freqParams = SetupSliderParams::createOp(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + "FREQ", "FRQ");
        setupSlider(freqParams);
        SetupTextButtonParams fto0Params = SetupTextButtonParams::createOp(gui.page, gui.freqToZero[i], gui.freqToZeroAtt[i], paramPrefix + "FREQ_TO_0", "Freq -> 0Hz", OpTextButtonSize, {false, false});
        setupTextButton(fto0Params);
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        SetupTextButtonParams fto440Params = SetupTextButtonParams::createOp(gui.page, gui.freqTo440[i], gui.freqTo440Att[i], paramPrefix + "FREQ_TO_440", "Freq -> 440Hz", OpTextButtonSize, { false, false });
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

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Algorithm / Feedback" };
    setupGroup(groupParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPN_ALGORITHM", "Algorithm", algItems);
    setupCombo(algParams);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPN_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);

    // Operators
    SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPN_OP" + juce::String(i) + "_";

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + "DT", "DT");
        setupSlider(dtParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::create(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + "KS", "KS", ksItems);
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR");
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL");
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + "SR", "SR");
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
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

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Algorithm / Feedback" };
    setupGroup(groupParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPL_ALGORITHM", "Algorithm", algItems);
    setupCombo(algParams);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPL_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);

    SetupOpGroupsParams<2> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 2; ++i)
    {
        juce::String paramPrefix = "OPL_OP" + juce::String(i) + "_";

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupSlider(mulParams);
        SetupSliderParams dtParams = SetupSliderParams::createOp(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + "DT", "DT");
        setupSlider(dtParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR");
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL");
        setupSlider(slParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
        setupSlider(rrParams);
        SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.ws[i], gui.wsLabel[i], gui.wsAtt[i], paramPrefix + "WS", "WS", wsItems);
        setupCombo(wsParams);
    }
}

void AudioPlugin2686VEditor::setupOpllGui(OpllGuiSet& gui)
{
    std::vector<SelectItem> wsItems = { {.name = "Sine", .value = 1}, {.name = "Half", .value = 2}, };
    std::vector<SelectItem> kslItems = { {.name = "KSL: 0", .value = 1}, {.name = "KSL: 1", .value = 2}, {.name = "KSL: 2", .value = 3}, {.name = "KSL: 3", .value = 4}, };

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Global" };
    setupGroup(groupParams);

    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPLL_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);

    SetupOpGroupsParams<2> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 2; ++i)
    {
        juce::String paramPrefix = "OPLL_OP" + juce::String(i) + "_";

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupSlider(mulParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupSlider(tlParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR");
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL");
        setupSlider(slParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
        setupSlider(rrParams);
        SetupToggleButtonParams amParams = SetupToggleButtonParams::createOp(gui.page, gui.am[i], gui.amLabel[i], gui.amAtt[i], paramPrefix + "AM", "AM");
        setupToggleButton(amParams);
        SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.ksl[i], gui.kslLabel[i], gui.kslAtt[i], paramPrefix + "KSL", "KSL", kslItems);
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

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Algorithm / Feedback" };
    setupGroup(groupParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPL3_ALGORITHM", "Algorithm", algItems);
    setupCombo(algParams);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPL3_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);

    SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPL3_OP" + juce::String(i) + "_";

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupSlider(mulParams);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupSlider(tlParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR");
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL");
        setupSlider(slParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
        setupSlider(rrParams);
        SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.ws[i], gui.wsLabel[i], gui.wsAtt[i], paramPrefix + "WS", "WS", wsItems);
        setupCombo(wsParams);
    }
}

void AudioPlugin2686VEditor::setupOpmGui(OpmGuiSet& gui)
{
    std::vector<SelectItem> pmsItems = createItems(8);
    std::vector<SelectItem> amsItems = createItems(4);
    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> ksItems = { {.name = "OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    SetupGroupParams groupParams = { .page = gui.page, .group = gui.globalGroup, .title = "Algorithm / Feedback / LFO" };
    setupGroup(groupParams);
    SetupComboParams algParams = SetupComboParams::create(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPM_ALGORITHM", "Algorithm", algItems);
    setupCombo(algParams);
    SetupSliderParams fbParams = SetupSliderParams::create(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPM_FEEDBACK", "Feedback");
    setupFbSlider(fbParams);
    SetupSliderParams fqParams = SetupSliderParams::create(gui.page, gui.lfoFreqSlider, gui.lfoFreqLabel, gui.lfoFreqAtt, "OPM_LFO_FREQ", "LFO FREQ");
    setupSlider(fqParams);
    SetupComboParams pmsParams = SetupComboParams::create(gui.page, gui.pmsSelector, gui.pmsLabel, gui.pmsAtt, "OPM_LFO_PMS", "LFO PMS", pmsItems);
    setupCombo(pmsParams);
    SetupComboParams amsParams = SetupComboParams::create(gui.page, gui.amsSelector, gui.amsLabel, gui.amsAtt, "OPM_LFO_AMS", "LFO AMS", amsItems);
    setupCombo(amsParams);

    // Operators
    SetupOpGroupsParams<4> opGroupsParams = { .page = gui.page, .groups = gui.opGroups };
    setupOpGroups(opGroupsParams);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPM_OP" + juce::String(i) + "_";

        SetupSliderParams mulParams = SetupSliderParams::createOp(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupSlider(mulParams);
        SetupSliderParams dt1Params = SetupSliderParams::createOp(gui.page, gui.dt1[i], gui.dt1Label[i], gui.dt1Att[i], paramPrefix + "DT", "DT1");
        setupSlider(dt1Params);
        SetupSliderParams dt2Params = SetupSliderParams::createOp(gui.page, gui.dt2[i], gui.dt2Label[i], gui.dt2Att[i], paramPrefix + "DT2", "DT2");
        setupSlider(dt2Params);
        SetupSliderParams tlParams = SetupSliderParams::createOp(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupSlider(tlParams);
        SetupComboParams ksParams = SetupComboParams::create(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + "KS", "KS", ksItems);
        setupCombo(ksParams);
        SetupSliderParams arParams = SetupSliderParams::createOp(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupSlider(arParams);
        SetupSliderParams drParams = SetupSliderParams::createOp(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "D1R");
        setupSlider(drParams);
        SetupSliderParams slParams = SetupSliderParams::createOp(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "D1L");
        setupSlider(slParams);
        SetupSliderParams srParams = SetupSliderParams::createOp(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + "SR", "D2R");
        setupSlider(srParams);
        SetupSliderParams rrParams = SetupSliderParams::createOp(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
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

    SetupGroupParams vGroupParams = { .page = gui.page, .group = gui.voiceGroup, .title = "Voice" };
    setupGroup(vGroupParams);

    SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.waveSelector, gui.waveLabel, gui.waveAtt, "SSG_WAVEFORM", "Wave Form", wsItems, ComboBoxSize, LabelSize, {true, true});
    setupCombo(wsParams);
    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "SSG_LEVEL", "Tone");
    setupSlider(lvParams);
    SetupSliderParams nsParams = SetupSliderParams::create(gui.page, gui.noiseSlider, gui.noiseLabel, gui.noiseAtt, "SSG_NOISE", "Noise");
    setupSlider(nsParams);
    SetupSliderParams nfParams = SetupSliderParams::create(gui.page, gui.noiseFreqSlider, gui.noiseFreqLabel, gui.noiseFreqAtt, "SSG_NOISE_FREQ", "Noise Freq");
    setupSlider(nfParams);
    SetupSliderParams mxParams = SetupSliderParams::create(gui.page, gui.mixSlider, gui.mixLabel, gui.mixAtt, "SSG_MIX", "Mix (T<-M->N)");
    setupSlider(mxParams);

    SetupTextButtonParams mstParams = SetupTextButtonParams::create(gui.page, gui.mixSetTone, gui.mixSetAtt, "Tone", "Tone", TextButtonSize, { false, false });
    setupTextButton(mstParams);
    gui.mixSetTone.onClick = [this] { ssgGui.triPeakSlider.setValue(0.0, juce::sendNotification); };
    SetupTextButtonParams msmParams = SetupTextButtonParams::create(gui.page, gui.mixSetMix, gui.mixSetAtt, "Mix", "Mix", TextButtonSize, { false, false });
    setupTextButton(msmParams);
    gui.mixSetMix.onClick = [this] { ssgGui.triPeakSlider.setValue(0.5, juce::sendNotification); };
    SetupTextButtonParams msnParams = SetupTextButtonParams::create(gui.page, gui.mixSetNoise, gui.mixSetAtt, "Noise", "Noise", TextButtonSize, { false, false });
    setupTextButton(msnParams);
    gui.mixSetNoise.onClick = [this] { ssgGui.triPeakSlider.setValue(1.0, juce::sendNotification); };

    SetupGroupParams fGroupParams = { .page = gui.page, .group = gui.filterGroup, .title = "Filter (Envelope)" };
    setupGroup(fGroupParams);
    SetupToggleButtonParams bpParams = SetupToggleButtonParams::create(gui.page, gui.adsrBypassButton, gui.adsrBypassButtonLabel, gui.adsrBypassAtt, "SSG_ADSR_BYPASS", "Bypass ADSR (Gate)", ToggleButtonSize, {true, true});
    setupToggleButton(bpParams);
    SetupSliderParams arParams = SetupSliderParams::create(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, "SSG_AR", "Attack");
    setupSlider(arParams);
    SetupSliderParams drParams = SetupSliderParams::create(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, "SSG_DR", "Decay");
    setupSlider(drParams);
    SetupSliderParams slParams = SetupSliderParams::create(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, "SSG_SL", "Sustain");
    setupSlider(slParams);
    SetupSliderParams rrParams = SetupSliderParams::create(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, "SSG_RR", "Release");
    setupSlider(rrParams);

    // Duty Controls Setup
    SetupGroupParams dGroupParams = { .page = gui.page, .group = gui.dutyGroup, .title = "Pulse Width (Duty)" };
    setupGroup(dGroupParams);
    SetupComboParams dmParams = SetupComboParams::create(gui.page, gui.dutyModeSelector, gui.dutyModeLabel, gui.dutyModeAtt, "SSG_DUTY_MODE", "Duty Mode", dmItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(dmParams);
    SetupComboParams dpParams = SetupComboParams::create(gui.page, gui.dutyPresetSelector, gui.dutyPresetLabel, gui.dutyPresetAtt, "SSG_DUTY_PRESET", "Duty Preset", prItems);
    setupCombo(dpParams);
    SetupSliderParams dvParams = SetupSliderParams::create(gui.page, gui.dutyVarSlider, gui.dutyVarLabel, gui.dutyVarAtt, "SSG_DUTY_VAR", "Duty Ratio");
    setupSlider(dvParams);
    SetupToggleButtonParams dtiParams = SetupToggleButtonParams::create(gui.page, gui.dutyInvertButton, gui.dutyInvertLabel, gui.dutyInvertAtt, "SSG_DUTY_INV", "Invert Phase");
    setupToggleButton(dtiParams);

    SetupGroupParams tpGroupParams = { .page = gui.page, .group = gui.triGroup, .title = "Triangle Property" };
    setupGroup(tpGroupParams);
    SetupToggleButtonParams tktParams = SetupToggleButtonParams::create(gui.page, gui.triKeyTrackButton, gui.triKeyTrackLabel, gui.triKeyTrackAtt, "SSG_TRI_KEYTRK", "Key Track (Pitch)", ToggleButtonSize, { true, true });
    setupToggleButton(tktParams);
    SetupSliderParams tfParams = SetupSliderParams::create(gui.page, gui.triFreqSlider, gui.triFreqLabel, gui.triFreqAtt, "SSG_TRI_FREQ", "Manual Freq (Hz)");
    setupSlider(tfParams);
    SetupSliderParams tpParams = SetupSliderParams::create(gui.page, gui.triPeakSlider, gui.triPeakLabel, gui.triPeakAtt, "SSG_TRI_PEAK", "Peak Position");
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
    SetupToggleButtonParams eeParams = SetupToggleButtonParams::create(gui.page, gui.envEnableButton, gui.envEnableLabel, gui.envEnableAtt, "SSG_ENV_ENABLE", "Enable HW Env", ToggleButtonSize, { true, true });
    setupToggleButton(eeParams);
    SetupComboParams evParams = SetupComboParams::create(gui.page, gui.shapeSelector, gui.shapeLabel, gui.shapeAtt, "SSG_ENV_SHAPE", "Shape", envItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(evParams);
    SetupSliderParams epParams = SetupSliderParams::create(gui.page, gui.periodSlider, gui.periodLabel, gui.periodAtt, "SSG_ENV_PERIOD", "Env Speed (Hz)");
    setupSlider(epParams);
}

void AudioPlugin2686VEditor::setupWtGui(WtGuiSet& gui)
{
    std::vector<SelectItem> wsItems = {
        {.name = "Sine", .value = 1 },
        {.name = "Triangle", .value = 2 },
        {.name = "Saw Up", .value = 3 },
        {.name = "Saw Down", .value = 4 },
        {.name = "Square (50%)", .value = 5 },
        {.name = "Pulse (25%)", .value = 6 },
        {.name = "Pulse (12.5%)", .value = 7 },
        {.name = "Digital Noise", .value = 8 },
        {.name = "Custom (Draw)", .value = 9 },
    };
    std::vector<SelectItem> bdItems = {
        {.name = "4-bit (16 steps)", .value = 1 },
        {.name = "5-bit (32 steps)", .value = 2 },
        {.name = "6-bit (64 steps)", .value = 3 },
        {.name = "8-bit (256 steps)", .value = 4 },
    };
    std::vector<SelectItem> tsItems = {
        {.name = "32 Samples", .value = 1 },
        {.name = "64 Samples", .value = 2 },
    };

    // --- Filter Group (Left) ---
    SetupGroupParams fGroupParams = { .page = gui.page, .group = gui.filterGroup, .title = "Filter (ADSR)" };
    setupGroup(fGroupParams);
    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "WT_LEVEL", "Level");
    setupSlider(lvParams);
    SetupSliderParams arParams = SetupSliderParams::create(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, "WT_AR", "Attack");
    setupSlider(arParams);
    SetupSliderParams drParams = SetupSliderParams::create(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, "WT_DR", "Decay");
    setupSlider(drParams);
    SetupSliderParams slParams = SetupSliderParams::create(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, "WT_SL", "Sustain");
    setupSlider(slParams);
    SetupSliderParams rrParams = SetupSliderParams::create(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, "WT_RR", "Release");
    setupSlider(rrParams);

    // --- Property Group (Right) ---
    SetupGroupParams wpGroupParams = { .page = gui.page, .group = gui.propGroup, .title = "Wavetable Properties" };
    setupGroup(wpGroupParams);

    // Waveform Selector Setup
    SetupComboParams wsParams = SetupComboParams::create(gui.page, gui.waveSelector, gui.waveLabel, gui.waveAtt, "WT_WAVE", "Wave Form", wsItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(wsParams);

    // Custom Sliders Setup
    SetupGroupParams cwGroupParams = { .page = gui.page, .group = gui.customGroup, .title = "Custom Wave" };
    setupGroup(cwGroupParams);
	SetupCustomWaveTableParams cw32Params = { gui.page, gui.customSliders32, gui.customAtts32, "WT_CUSTOM32_" };
    setupCustomWaveTable(cw32Params);
    SetupCustomWaveTableParams cw64Params = { gui.page, gui.customSliders64, gui.customAtts64, "WT_CUSTOM64_" };
    setupCustomWaveTable(cw64Params);
    SetupComboParams bParams = SetupComboParams::create(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, "WT_BIT", "Bit Depth", bdItems);
    setupCombo(bParams);
    SetupComboParams szParams = SetupComboParams::create(gui.page, gui.sizeSelector, gui.sizeLabel, gui.sizeAtt, "WT_SIZE", "Table Size", tsItems, ComboBoxSize, LabelSize, { true, true });
    setupCombo(szParams);
    SetupToggleButtonParams meParams = SetupToggleButtonParams::create(gui.page, gui.modEnableButton, gui.modEnableLabel, gui.modEnableAtt, "WT_MOD_ENABLE", "Modulation ON", ToggleButtonSize, { true, true });
    setupToggleButton(meParams);
    SetupSliderParams mdParams = SetupSliderParams::create(gui.page, gui.modDepthSlider, gui.modDepthLabel, gui.modDepthAtt, "WT_MOD_DEPTH", "Mod Depth");
    setupSlider(mdParams);
    SetupSliderParams msParams = SetupSliderParams::create(gui.page, gui.modSpeedSlider, gui.modSpeedLabel, gui.modSpeedAtt, "WT_MOD_SPEED", "Mod Speed");
    setupSlider(msParams);
}

void AudioPlugin2686VEditor::setupRhythmGui(RhythmGuiSet& gui)
{
    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "RHYTHM_LEVEL", "Master Vol");
    setupFbSlider(lvParams);
}

void AudioPlugin2686VEditor::setupAdpcmGui(AdpcmGuiSet& gui)
{
    SetupSliderParams lvParams = SetupSliderParams::create(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "ADPCM_LEVEL", "Master Vol");
    setupFbSlider(lvParams);
    SetupToggleButtonParams lpParams = SetupToggleButtonParams::create(gui.page, gui.loopButton, gui.loopLabel, gui.loopAtt, "ADPCM_LOOP", "Loop Playback");
    setupToggleButton(lpParams);
    SetupSliderParams arParams = SetupSliderParams::create(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, "ADPCM_AR", "Attack");
    setupSlider(arParams);
    SetupSliderParams drParams = SetupSliderParams::create(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, "ADPCM_DR", "Decay");
    setupSlider(drParams);
    SetupSliderParams slParams = SetupSliderParams::create(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, "ADPCM_SL", "Sustain");
    setupSlider(slParams);
    SetupSliderParams rrParams = SetupSliderParams::create(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, "ADPCM_RR", "Release");
    setupSlider(rrParams);
}

void AudioPlugin2686VEditor::layoutOpnaPage(Fm4GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin().reduced(5);

    // --- A. Global Section (Top) ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);

    gui.globalGroup.setBounds(topArea);
    auto globalRect = topArea.reduced(10);
    globalRect.removeFromTop(20); // タイトル回避

    int globalW = globalRect.getWidth() / 5;
    // Globalエリアも少し余裕を持たせて配置
    gui.algSelector.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 10));
    gui.feedbackSlider.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 5));
	gui.lfoFreqSlider.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 10));
    gui.lfoPmsSelector.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 10));
    gui.lfoAmsSelector.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 10));

    pageArea.removeFromTop(10); // Spacer

    // --- B. Operators Section (Bottom) ---
    // 残りのエリアを4等分
    int opWidth = pageArea.getWidth() / 4;

    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(opWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea.reduced(2));

        // 枠線の内側
        auto innerRect = opArea.reduced(8);
        innerRect.removeFromTop(20); // タイトル(Operator X)回避

        int colWidth = innerRect.getWidth();
        auto leftCol = innerRect.removeFromLeft(60);
        auto rightCol = innerRect;

        // 5行に分割 (高さ800pxなら1行あたり十分な高さが確保されます)
        int rowH = innerRect.getHeight() / 15;

        // 配置ヘルパー (上下に少し余白 reduced(0, 5) を入れて数値を見やすくする)
        auto place = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            c.setBounds(rect.removeFromTop(rowH).reduced(2, 5));
        };

        // ★追加: コンボボックス配置ヘルパー (少し高さを小さく調整)
        auto placeCombo = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            // コンボボックスは縦幅がありすぎると間延びするので、中央寄せで配置
            c.setBounds(rect.removeFromTop(rowH).withSizeKeepingCentre(rect.getWidth() - 4, 24));
        };

        place(gui.mulLabel[i], leftCol);
        place(gui.mul[i], rightCol);
        place(gui.dtLabel[i], leftCol);
        place(gui.dt[i], rightCol);
        place(gui.arLabel[i], leftCol);
        place(gui.ar[i], rightCol);
        place(gui.drLabel[i], leftCol);
        place(gui.dr[i], rightCol);
        place(gui.srLabel[i], leftCol);
        place(gui.sr[i], rightCol);
        place(gui.slLabel[i], leftCol);
        place(gui.sl[i], rightCol);
        place(gui.rrLabel[i], leftCol);
        place(gui.rr[i], rightCol);
        place(gui.tlLabel[i], leftCol);
        place(gui.tl[i], rightCol);
        place(gui.ksLabel[i], leftCol);
        placeCombo(gui.ks[i], rightCol);
        place(gui.seLabel[i], leftCol);
        placeCombo(gui.se[i], rightCol);
        place(gui.fixLabel[i], leftCol);
        place(gui.fix[i], rightCol);
        place(gui.freqLabel[i], leftCol);
        place(gui.freq[i], rightCol);
        place(gui.freqToZeroLabel[i], leftCol);
        place(gui.freqToZero[i], rightCol);
        place(gui.freqTo440Label[i], leftCol);
        place(gui.freqTo440[i], rightCol);
        place(gui.amLabel[i], leftCol);
        place(gui.am[i], rightCol); // Button    
    }
}

void AudioPlugin2686VEditor::layoutOpnPage(Fm4GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin().reduced(5);

    // --- A. Global Section (Top) ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);

    gui.globalGroup.setBounds(topArea);
    auto globalRect = topArea.reduced(10);
    globalRect.removeFromTop(20); // タイトル回避

    int globalW = globalRect.getWidth() / 2;
    // Globalエリアも少し余裕を持たせて配置
    gui.algSelector.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 10));
    gui.feedbackSlider.setBounds(globalRect.reduced(20, 5));

    pageArea.removeFromTop(10); // Spacer

    // --- B. Operators Section (Bottom) ---
    // 残りのエリアを4等分
    int opWidth = pageArea.getWidth() / 4;

    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(opWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea.reduced(2));

        // 枠線の内側
        auto innerRect = opArea.reduced(8);
        innerRect.removeFromTop(20); // タイトル(Operator X)回避

        int colWidth = innerRect.getWidth();
        auto leftCol = innerRect.removeFromLeft(60);
        auto rightCol = innerRect;

        int rowH = innerRect.getHeight() / 13;

        // 配置ヘルパー (上下に少し余白 reduced(0, 5) を入れて数値を見やすくする)
        auto place = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            c.setBounds(rect.removeFromTop(rowH).reduced(2, 5));
        };

        // ★追加: コンボボックス配置ヘルパー (少し高さを小さく調整)
        auto placeCombo = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            // コンボボックスは縦幅がありすぎると間延びするので、中央寄せで配置
            c.setBounds(rect.removeFromTop(rowH).withSizeKeepingCentre(rect.getWidth() - 4, 24));
        };

        place(gui.mulLabel[i], leftCol);
        place(gui.mul[i], rightCol);
        place(gui.dtLabel[i], leftCol);
        place(gui.dt[i], rightCol);
        place(gui.arLabel[i], leftCol);
        place(gui.ar[i], rightCol);
        place(gui.drLabel[i], leftCol);
        place(gui.dr[i], rightCol);
        place(gui.srLabel[i], leftCol);
        place(gui.sr[i], rightCol);
        place(gui.slLabel[i], leftCol);
        place(gui.sl[i], rightCol);
        place(gui.rrLabel[i], leftCol);
        place(gui.rr[i], rightCol);
        place(gui.tlLabel[i], leftCol);
        place(gui.tl[i], rightCol);
        place(gui.ksLabel[i], leftCol);
        placeCombo(gui.ks[i], rightCol);
        place(gui.fixLabel[i], leftCol);
        place(gui.fix[i], rightCol);
        place(gui.freqLabel[i], leftCol);
        place(gui.freq[i], rightCol);
        place(gui.freqToZeroLabel[i], leftCol);
        place(gui.freqToZero[i], rightCol);
        place(gui.freqTo440Label[i], leftCol);
        place(gui.freqTo440[i], rightCol);
    }
}

void AudioPlugin2686VEditor::layoutOplPage(Fm2GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin().reduced(5);

    // --- A. Global Section ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);
    gui.globalGroup.setBounds(topArea);

    auto globalRect = topArea.reduced(10);
    globalRect.removeFromTop(20);

    int globalW = globalRect.getWidth() / 2;
    gui.algSelector.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 10));
    gui.feedbackSlider.setBounds(globalRect.reduced(20, 5));

    pageArea.removeFromTop(10);

    // --- B. Operators Section ---
    int opWidth = pageArea.getWidth() / 2;

    for (int i = 0; i < 2; ++i)
    {
        auto opArea = pageArea.removeFromLeft(opWidth);
        gui.opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(8);
        innerRect.removeFromTop(20);

        int colWidth = innerRect.getWidth();
        auto leftCol = innerRect.removeFromLeft(60);
        auto rightCol = innerRect;

        // 4行
        int rowH = innerRect.getHeight() / 7;

        auto place = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            c.setBounds(rect.removeFromTop(rowH).reduced(2, 5));
        };

        place(gui.mulLabel[i], leftCol);
        place(gui.mul[i], rightCol);
        place(gui.dtLabel[i], leftCol);
        place(gui.dt[i], rightCol);
        place(gui.arLabel[i], leftCol);
        place(gui.ar[i], rightCol);
        place(gui.drLabel[i], leftCol);
        place(gui.dr[i], rightCol);
        place(gui.slLabel[i], leftCol);
        place(gui.sl[i], rightCol);
        place(gui.rrLabel[i], leftCol);
        place(gui.rr[i], rightCol);
        place(gui.wsLabel[i], leftCol);
        place(gui.ws[i], rightCol);
    }
}

void AudioPlugin2686VEditor::layoutOpllPage(OpllGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin().reduced(5);

    // --- Global & Preset Section ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);
    gui.globalGroup.setBounds(topArea);

    auto globalRect = topArea.reduced(10);
    globalRect.removeFromTop(15);

    // Feedback Center
    gui.feedbackSlider.setBounds(globalRect.withSizeKeepingCentre(100, 50));

    pageArea.removeFromTop(10);

    // --- Operators Section ---
    int opWidth = pageArea.getWidth() / 2;

    for (int i = 0; i < 2; ++i)
    {
        auto opArea = pageArea.removeFromLeft(opWidth);
        gui.opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(8);
        innerRect.removeFromTop(20);

        int colWidth = innerRect.getWidth() / 2;
        auto leftCol = innerRect.removeFromLeft(colWidth);
        auto rightCol = innerRect;

        // 4行
        int rowH = innerRect.getHeight() / 12;
        auto place = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            c.setBounds(rect.removeFromTop(rowH).reduced(2, 5));
        };

        place(gui.mulLabel[i], leftCol);
        place(gui.mul[i], rightCol);
        place(gui.tlLabel[i], leftCol);
        place(gui.tl[i], rightCol);
        place(gui.arLabel[i], leftCol);
        place(gui.ar[i], rightCol);
        place(gui.drLabel[i], leftCol);
        place(gui.dr[i], rightCol);
        place(gui.slLabel[i], leftCol);
        place(gui.sl[i], rightCol);
        place(gui.rrLabel[i], leftCol);
        place(gui.rr[i], rightCol);
        place(gui.wsLabel[i], leftCol);
        place(gui.ws[i], rightCol);
        place(gui.kslLabel[i], leftCol);
        place(gui.ksl[i], rightCol);
        place(gui.ksrLabel[i], leftCol);
        place(gui.ksr[i], rightCol);
        place(gui.amLabel[i], leftCol);
        place(gui.am[i], rightCol);
        place(gui.vibLabel[i], leftCol);
        place(gui.vib[i], rightCol);
        place(gui.egTypeLabel[i], leftCol);
        place(gui.egType[i], rightCol);
    }
}

void AudioPlugin2686VEditor::layoutOpl3Page(Opl3GuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin().reduced(5);

    // --- Global Section ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);
    gui.globalGroup.setBounds(topArea);

    auto globalRect = topArea.reduced(10);
    globalRect.removeFromTop(20);

    int globalW = globalRect.getWidth() / 2;
    gui.algSelector.setBounds(globalRect.removeFromLeft(globalW).withSizeKeepingCentre(200, 24));
    gui.feedbackSlider.setBounds(globalRect.withSizeKeepingCentre(100, 110));

    pageArea.removeFromTop(10);

    // --- Operators Section ---
    int opWidth = pageArea.getWidth() / 4;

    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(opWidth);
        gui.opGroups[i].setBounds(opArea.reduced(2));

        auto innerRect = opArea.reduced(8);
        innerRect.removeFromTop(20);

        int colWidth = innerRect.getWidth() / 2;
        auto leftCol = innerRect.removeFromLeft(colWidth);
        auto rightCol = innerRect;

        int rowH = innerRect.getHeight() / 7;

        auto place = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            c.setBounds(rect.removeFromTop(rowH).reduced(2, 5));
        };

        auto placeCombo = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            c.setBounds(rect.removeFromTop(rowH).withSizeKeepingCentre(rect.getWidth() - 4, 24));
        };

		place(gui.mulLabel[i], leftCol);
        place(gui.mul[i], rightCol);
		place(gui.tlLabel[i], leftCol);
        place(gui.tl[i], rightCol);
		place(gui.arLabel[i], leftCol);
        place(gui.ar[i], rightCol);
		place(gui.drLabel[i], leftCol);
        place(gui.dr[i], rightCol);
		place(gui.slLabel[i], leftCol);
        place(gui.sl[i], rightCol);
		place(gui.rrLabel[i], leftCol);
        place(gui.rr[i], rightCol);
		place(gui.wsLabel[i], leftCol);
        placeCombo(gui.ws[i], rightCol);
    }
}

void AudioPlugin2686VEditor::layoutOpmPage(OpmGuiSet& gui, juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin().reduced(5);

    // --- A. Global Section (Top) ---
    auto topArea = pageArea.removeFromTop(TopGroupHeight);

    gui.globalGroup.setBounds(topArea);
    auto globalRect = topArea.reduced(10);
    globalRect.removeFromTop(20); // タイトル回避

    int globalW = globalRect.getWidth() / 5;
    // Globalエリアも少し余裕を持たせて配置
    gui.algSelector.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 10));
    gui.feedbackSlider.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 5));
    gui.lfoFreqSlider.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 10));
    gui.pmsSelector.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 10));
    gui.amsSelector.setBounds(globalRect.removeFromLeft(globalW).reduced(20, 10));

    pageArea.removeFromTop(10); // Spacer

    // --- B. Operators Section (Bottom) ---
    // 残りのエリアを4等分
    int opWidth = pageArea.getWidth() / 4;

    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(opWidth);

        // 枠線
        gui.opGroups[i].setBounds(opArea.reduced(2));

        // 枠線の内側
        auto innerRect = opArea.reduced(8);
        innerRect.removeFromTop(20); // タイトル(Operator X)回避

        int colWidth = innerRect.getWidth();
        auto leftCol = innerRect.removeFromLeft(60);
        auto rightCol = innerRect;

        // 5行に分割 (高さ800pxなら1行あたり十分な高さが確保されます)
        int rowH = innerRect.getHeight() / 14;

        // 配置ヘルパー (上下に少し余白 reduced(0, 5) を入れて数値を見やすくする)
        auto place = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            c.setBounds(rect.removeFromTop(rowH).reduced(2, 5));
            };

        // ★追加: コンボボックス配置ヘルパー (少し高さを小さく調整)
        auto placeCombo = [&](juce::Component& c, juce::Rectangle<int>& rect) {
            // コンボボックスは縦幅がありすぎると間延びするので、中央寄せで配置
            c.setBounds(rect.removeFromTop(rowH).withSizeKeepingCentre(rect.getWidth() - 4, 24));
            };

        place(gui.mulLabel[i], leftCol);
        place(gui.mul[i], rightCol);
        place(gui.dt1Label[i], leftCol);
        place(gui.dt1[i], rightCol);
        place(gui.dt2Label[i], leftCol);
        place(gui.dt2[i], rightCol);
        place(gui.arLabel[i], leftCol);
        place(gui.ar[i], rightCol);
        place(gui.drLabel[i], leftCol);
        place(gui.dr[i], rightCol);
        place(gui.srLabel[i], leftCol);
        place(gui.sr[i], rightCol);
        place(gui.slLabel[i], leftCol);
        place(gui.sl[i], rightCol);
        place(gui.rrLabel[i], leftCol);
        place(gui.rr[i], rightCol);
        place(gui.tlLabel[i], leftCol);
        place(gui.tl[i], rightCol);
        place(gui.ksLabel[i], leftCol);
        placeCombo(gui.ks[i], rightCol);
        place(gui.fixLabel[i], leftCol);
        place(gui.fix[i], rightCol);
        place(gui.freqLabel[i], leftCol);
        place(gui.freq[i], rightCol);
        place(gui.freqToZeroLabel[i], leftCol);
        place(gui.freqToZero[i], rightCol);
        place(gui.freqTo440Label[i], leftCol);
        place(gui.freqTo440[i], rightCol);
    }
}

void AudioPlugin2686VEditor::layoutSsgPage(SsgGuiSet& gui, juce::Rectangle<int> content)
{
    // ==================================
    // Layout for SSG Page (自動伸縮対応)
    // ==================================
    // 画面高さが伸びた分、各エリアも縦に伸びます
    auto ssg = content.withZeroOrigin();
    auto left = ssg.removeFromLeft(600);
    auto right = ssg;

    int leftHeight = left.getHeight();
    auto voiceArea = left.removeFromTop(leftHeight / 2 - 20);
    left.removeFromTop(10);
    auto filterArea = left;

    // --- 1. Voice Group ---
    gui.voiceGroup.setBounds(voiceArea.reduced(5));
    auto vRect = gui.voiceGroup.getBounds().reduced(20, 25);

    gui.waveSelector.setBounds(vRect.removeFromTop(24).withSizeKeepingCentre(160, 24));
    vRect.removeFromTop(10);

    // Knobs Layout
    int knobW = vRect.getWidth() / 4;

    // 縦に余裕ができたので、ノブのサイズ制限を少し緩めます
    // withSizeKeepingCentre(80, 90) -> (90, 110) くらいに拡大
    gui.levelSlider.setBounds(vRect.removeFromLeft(knobW).withSizeKeepingCentre(90, 110));
    gui.noiseSlider.setBounds(vRect.removeFromLeft(knobW).withSizeKeepingCentre(90, 110));

    gui.noiseFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    gui.noiseFreqSlider.setBounds(vRect.removeFromLeft(knobW).withSizeKeepingCentre(90, 110));

    auto mixArea = vRect;
    gui.mixSlider.setBounds(mixArea.withSizeKeepingCentre(90, 110));

    int btnTotalW = 120;
    int btnW = btnTotalW / 3;
    int centerX = gui.mixSlider.getBounds().getCentreX();
    int startX = centerX - (btnTotalW / 2);
    int startY = gui.mixSlider.getBottom() + 0;

    juce::Rectangle<int> btnRect(startX, startY, btnTotalW, 20);
    gui.mixSetTone.setBounds(btnRect.removeFromLeft(btnW).reduced(2, 0));
    gui.mixSetMix.setBounds(btnRect.removeFromLeft(btnW).reduced(2, 0));
    gui.mixSetNoise.setBounds(btnRect.removeFromLeft(btnW).reduced(2, 0));

    // --- 2. Filter Group ---
    gui.filterGroup.setBounds(filterArea.reduced(5));
    auto fRect = gui.filterGroup.getBounds().reduced(20, 25);

    gui.adsrBypassButton.setBounds(fRect.removeFromTop(30).withSizeKeepingCentre(160, 30));
    fRect.removeFromTop(20);

    // ADSR Sliders
    int adsrW = fRect.getWidth() / 4;
    // 縦長になったので高さを有効活用
    gui.attackSlider.setBounds(fRect.removeFromLeft(adsrW).reduced(5));
    gui.decaySlider.setBounds(fRect.removeFromLeft(adsrW).reduced(5));
    gui.sustainSlider.setBounds(fRect.removeFromLeft(adsrW).reduced(5));
    gui.releaseSlider.setBounds(fRect.removeFromLeft(adsrW).reduced(5));

    // --- SSG Right Pane ---
    auto rightArea = right.reduced(5);
    int groupHeight = rightArea.getHeight() / 2;

    float waveParam = *audioProcessor.apvts.getRawParameterValue("SSG_WAVEFORM");
    int waveMode = (waveParam > 0.5f) ? 1 : 0;

    auto topArea = rightArea.removeFromTop(groupHeight);

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
        gui.dutyInvertButton.setVisible(true);

        gui.dutyGroup.setBounds(topArea);
        auto dRect = gui.dutyGroup.getBounds().reduced(20, 25);

        gui.dutyModeSelector.setBounds(dRect.removeFromTop(30).withSizeKeepingCentre(180, 24));
        dRect.removeFromTop(10);
        gui.dutyInvertButton.setBounds(dRect.removeFromTop(30).withSizeKeepingCentre(120, 30));
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

        gui.triGroup.setBounds(topArea);
        auto tRect = gui.triGroup.getBounds().reduced(20, 25);

        gui.triKeyTrackButton.setBounds(tRect.removeFromTop(30).withSizeKeepingCentre(160, 30));
        tRect.removeFromTop(10);

        bool isKeyTrack = gui.triKeyTrackButton.getToggleState();
        gui.triFreqSlider.setVisible(!isKeyTrack);
        gui.triFreqLabel.setVisible(!isKeyTrack);

        juce::Rectangle<int> peakSliderArea;

        if (!isKeyTrack) {
            auto sliderRow = tRect.removeFromTop(120);
            int w = sliderRow.getWidth() / 2;
            gui.triFreqSlider.setBounds(sliderRow.removeFromLeft(w).withSizeKeepingCentre(100, 120));
            peakSliderArea = sliderRow;
            gui.triPeakSlider.setBounds(peakSliderArea.withSizeKeepingCentre(100, 120));
        }
        else {
            gui.triPeakSlider.setBounds(tRect.removeFromTop(120).withSizeKeepingCentre(100, 120));
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

    // HW Env
    auto envArea = rightArea;
    gui.envGroup.setBounds(envArea);
    auto eRect = gui.envGroup.getBounds().reduced(20, 25);

    gui.envEnableButton.setBounds(eRect.removeFromTop(30).withSizeKeepingCentre(140, 30));
    eRect.removeFromTop(10);
    gui.shapeSelector.setBounds(eRect.removeFromTop(30).withSizeKeepingCentre(180, 24));
    eRect.removeFromTop(30);
    gui.periodSlider.setBounds(eRect.removeFromTop(120).withSizeKeepingCentre(100, 120));
}

void AudioPlugin2686VEditor::layoutWtPage(WtGuiSet& gui)
{
    auto content = tabs.getLocalBounds();
    content.removeFromTop(tabs.getTabBarDepth());
    content.reduce(10, 10); // 全体の余白
    auto area = content.withZeroOrigin();

    // Left: Filter (40%)
    auto leftArea = area.removeFromLeft(area.getWidth() * 0.4);
    gui.filterGroup.setBounds(leftArea.reduced(5));
    auto fRect = gui.filterGroup.getBounds().reduced(20, 25);

    // Level
    gui.levelSlider.setBounds(fRect.removeFromTop(80).withSizeKeepingCentre(100, 80));
    fRect.removeFromTop(10);

    // ADSR
    int h = fRect.getHeight() / 4;
    auto placeAdsr = [&](juce::Slider& s) { s.setBounds(fRect.removeFromTop(h).withSizeKeepingCentre(200, 40)); s.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20); };

    placeAdsr(gui.attackSlider);
    placeAdsr(gui.decaySlider);
    placeAdsr(gui.sustainSlider);
    placeAdsr(gui.releaseSlider);

    // Right: Properties (60%)
    auto rightArea = area;
    gui.propGroup.setBounds(rightArea.reduced(5));
    auto pRect = gui.propGroup.getBounds().reduced(20, 25);

    // Waveform Selector (Top)
    gui.waveSelector.setBounds(pRect.removeFromTop(30).withSizeKeepingCentre(200, 24));

    pRect.removeFromTop(20);

    // Bit & Size (Middle)
    auto configRow = pRect.removeFromTop(30);
    gui.bitSelector.setBounds(configRow.removeFromLeft(configRow.getWidth() / 2).reduced(5, 0));
    gui.sizeSelector.setBounds(configRow.reduced(5, 0));

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
                gui.customSliders32[i].setBounds(sliderRect.removeFromLeft(sliderW));
            }

        }
        else if (waveSize == 64)
        {
            for (auto& s : gui.customSliders64) s.setVisible(true);

            int sliderW = sliderRect.getWidth() / 64;
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
    auto rhythm = content.withZeroOrigin();
    gui.levelSlider.setBounds(rhythm.withSizeKeepingCentre(100, 100));
}

void AudioPlugin2686VEditor::layoutAdpcmPage(AdpcmGuiSet& gui, juce::Rectangle<int> content)
{

    auto adpcm = content.withZeroOrigin();
    auto adpcmTop = adpcm.removeFromTop(120);
    gui.levelSlider.setBounds(adpcmTop.removeFromLeft(adpcmTop.getWidth() / 2).withSizeKeepingCentre(100, 100));
    gui.loopButton.setBounds(adpcmTop.withSizeKeepingCentre(120, 30));

    adpcm.removeFromTop(20);
    int aw = adpcm.getWidth() / 4;
    gui.attackSlider.setBounds(adpcm.removeFromLeft(aw).reduced(5));
    gui.decaySlider.setBounds(adpcm.removeFromLeft(aw).reduced(5));
    gui.sustainSlider.setBounds(adpcm.removeFromLeft(aw).reduced(5));
    gui.releaseSlider.setBounds(adpcm.removeFromLeft(aw).reduced(5));
}

void AudioPlugin2686VEditor::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
{
    // wtPage のサイズが変わったときだけレイアウトを実行
    if (&component == &wtGui.page && wasResized)
    {
        layoutWtPage(wtGui);
    }

    // もし gui.page も登録したなら
    // if (&component == &gui.page && wasResized) layoutgui.page(); // (関数化していれば)
}