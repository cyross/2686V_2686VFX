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

    setSize(1000, 800);

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
        if (targetMode > tabNumber) targetMode = tabNumber;

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
    g.fillAll(juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0f));
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawFittedText("2686V", getLocalBounds().removeFromTop(30), juce::Justification::right, 1);
}

void AudioPlugin2686VEditor::resized()
{
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

void AudioPlugin2686VEditor::setupGroup(
    juce::Component& page,
    juce::GroupComponent& group,
    const juce::String& title
    )
{
    page.addAndMakeVisible(group);
    group.setText(title);
    group.setTextLabelPosition(juce::Justification::centredLeft);
    group.toBack(); // 最背面へ

}

void AudioPlugin2686VEditor::setupLabel(
    juce::Component& page,
    juce::Component& parent,
    juce::Label& label,
    const juce::String& title,
    int width,
    int height
)
{
    page.addAndMakeVisible(label);
    label.setText(title, juce::dontSendNotification);
    label.setSize(width, height);
    label.attachToComponent(&parent, false);
    label.setJustificationType(juce::Justification::centred);
}

void AudioPlugin2686VEditor::setupSlider(
    juce::Component& parent,
    juce::Slider& slider,
    juce::Label& label,
    const juce::String& title,
    int labelWidth,
    int labelHeight
)
{
	setupLabel(parent, slider, label, title, labelWidth, labelHeight);

    parent.addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
}

void AudioPlugin2686VEditor::setupSliderAndReset(
    juce::Component& page,
    juce::Slider& s,
    juce::Label& label,
    std::unique_ptr<SliderAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int labelWidth,
    int labelHeight
)
{
    setupSlider(page, s, label, title, labelWidth, labelHeight);

    attr.reset(new SliderAttachment(audioProcessor.apvts, id, s));
}

void AudioPlugin2686VEditor::setupCombo(
    juce::Component& parent,
    juce::ComboBox& combo,
    juce::Label& label,
    const juce::String& title,
    std::vector<SelectItem>& items,
    int labelWidth,
    int labelHeigtht,
    bool isResize
)
{
    setupLabel(parent, combo, label, title);

    parent.addAndMakeVisible(combo);

	for (SelectItem& item : items)
    {
        combo.addItem(item.name, item.value);
    }

    if (isResize)
    {
        combo.onChange = [this] { resized(); };
    }
}

void AudioPlugin2686VEditor::setupComboAndReset(
    juce::Component& parent,
    juce::ComboBox& combo,
    juce::Label& label,
    std::unique_ptr<ComboBoxAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    std::vector<SelectItem>& items,
    int labelWidth,
    int labelHeigtht,
    bool isResize
)
{
	setupCombo(parent, combo, label, title, items, labelWidth, labelHeigtht, isResize);
	attr.reset(new ComboBoxAttachment(audioProcessor.apvts, id, combo));
}

void AudioPlugin2686VEditor::setupComboAndResetWithResize(
    juce::Component& parent,
    juce::ComboBox& combo,
    juce::Label& label,
    std::unique_ptr<ComboBoxAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    std::vector<SelectItem>& items,
    int labelWidth,
    int labelHeigtht
)
{
    setupComboAndReset(parent, combo, label, attr, id, title, items, labelWidth, labelHeigtht, true);
}

void AudioPlugin2686VEditor::setupButton(
    juce::Component& parent,
    juce::ToggleButton& btn,
    const juce::String& title,
    int width,
    int height,
    bool isResize
)
{
    parent.addAndMakeVisible(btn);

    btn.setButtonText(title);
    btn.setSize(width, height);
    if (isResize) {
        btn.onClick = [this] { resized(); };
    }
}

void AudioPlugin2686VEditor::setupButtonAndReset(
    juce::Component& parent,
    juce::ToggleButton& btn,
    std::unique_ptr<ButtonAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int width,
    int height,
    bool isResize
)
{
    setupButton(parent, btn, title, width, height, isResize);

    attr.reset(new ButtonAttachment(audioProcessor.apvts, id, btn));
}

void AudioPlugin2686VEditor::setupButtonAndResetWithResize(
    juce::Component& parent,
    juce::ToggleButton& btn,
    std::unique_ptr<ButtonAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int width,
    int height
)
{
    setupButtonAndReset(parent, btn, attr, id, title, width, height, true);
}

void AudioPlugin2686VEditor::setupTextButton(
    juce::Component& parent,
    juce::TextButton& btn,
    const juce::String& title,
    int width,
    int height,
    bool isResize
)
{
    parent.addAndMakeVisible(btn);

    btn.setButtonText(title);
    btn.setSize(width, height);
    if (isResize) {
        btn.onClick = [this] { resized(); };
    }
}

void AudioPlugin2686VEditor::setupTextButtonAndReset(
    juce::Component& parent,
    juce::TextButton& btn,
    std::unique_ptr<ButtonAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int width,
    int height,
    bool isResize
)
{
    setupTextButton(parent, btn, title, width, height, isResize);

    attr.reset(new ButtonAttachment(audioProcessor.apvts, id, btn));
}

void AudioPlugin2686VEditor::setupTextButtonAndResetWithResize(
    juce::Component& parent,
    juce::TextButton& btn,
    std::unique_ptr<ButtonAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int width,
    int height
)
{
    setupTextButtonAndReset(parent, btn, attr, id, title, width, height, true);
}

void AudioPlugin2686VEditor::setupFbSliderAndReset(
    juce::Component& page,
    juce::Slider& s,
    juce::Label& label,
    std::unique_ptr<SliderAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int labelWidth,
    int labelHeight
)
{
    setupSliderAndReset(page, s, label, attr, id, title, labelWidth, labelHeight);
    s.setRange(0.0, 7.0, 1.0);
}

template <size_t tableSize>
void AudioPlugin2686VEditor::setupCustomWaveTable(
    juce::Component& page,
    std::array<juce::Slider, tableSize>& wts,
    std::array<std::unique_ptr<SliderAttachment>, tableSize>& attrs,
    const juce::String& idPrefix
)
{
    for (int i = 0; i < tableSize; ++i)
    {
        auto& slider = wts[i];
        page.addAndMakeVisible(slider);

        // バーグラフ風の見た目にする
        slider.setSliderStyle(juce::Slider::LinearBarVertical);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); // 数値なし
        slider.setRange(-1.0, 1.0);

        auto paramId = idPrefix + juce::String(i);
        attrs[i].reset(new SliderAttachment(audioProcessor.apvts, paramId, slider));
    }

}

template <size_t opCount>
void AudioPlugin2686VEditor::setupOpGroups(
    juce::Component& page,
    std::array<juce::GroupComponent, opCount>& groups
)
{
    for (int i = 0; i < opCount; ++i)
    {
        setupGroup(page, groups[i], "Operator " + juce::String(i + 1));
    }
}

void AudioPlugin2686VEditor::setupOpLabel(
    juce::Component& page,
    juce::Component& parent,
    juce::Label& label,
    const juce::String& title,
    int width,
    int height
)
{
    setupLabel(page, parent, label, title, width, height);
}

void AudioPlugin2686VEditor::setupOpSlider(
    juce::Component& page,
    juce::Slider& s,
    juce::Label& label,
    const juce::String& title,
    int labelWidth,
    int labelHeight
)
{
    setupSlider(page, s, label, title, labelWidth, labelHeight);
}

void AudioPlugin2686VEditor::setupOpSliderAndReset(
    juce::Component& page,
    juce::Slider& s,
    juce::Label& label,
    std::unique_ptr<SliderAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int labelWidth,
    int labelHeight
)
{
	setupSliderAndReset(page, s, label, attr, id, title, labelWidth, labelHeight);
}

void AudioPlugin2686VEditor::setupOpCombo(
    juce::Component& parent,
    juce::ComboBox& combo,
    juce::Label& label,
    const juce::String& title,
    std::vector<SelectItem>& items,
    int labelWidth,
    int labelHeigtht,
    bool isResize
)
{
	setupCombo(parent, combo, label, title, items, labelWidth, labelHeigtht, isResize);
}

void AudioPlugin2686VEditor::setupOpComboAndReset(
    juce::Component& parent,
    juce::ComboBox& combo,
    juce::Label& label,
    std::unique_ptr<ComboBoxAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    std::vector<SelectItem>& items,
    int labelWidth,
    int labelHeigtht,
    bool isResize
)
{
	setupComboAndReset(parent, combo, label, attr, id, title, items, labelWidth, labelHeigtht, isResize);
}

void AudioPlugin2686VEditor::setupOpComboAndResetWithResize(
    juce::Component& parent,
    juce::ComboBox& combo,
    juce::Label& label,
    std::unique_ptr<ComboBoxAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    std::vector<SelectItem>& items,
    int labelWidth,
    int labelHeigtht
)
{
    setupOpComboAndReset(parent, combo, label, attr, id, title, items, labelWidth, labelHeigtht, true);
}

void AudioPlugin2686VEditor::setupOpButton(
    juce::Component& parent,
    juce::ToggleButton& btn,
    const juce::String& title,
    int width,
    int height,
    bool isResize
)
{
    setupButton(parent, btn, title, width, height, isResize);
}

void AudioPlugin2686VEditor::setupOpButtonAndReset(
    juce::Component& parent,
    juce::ToggleButton& btn,
    std::unique_ptr<ButtonAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int width,
    int height,
    bool isResize
)
{
    setupButtonAndReset(parent, btn, attr, id, title, width, height, isResize);
}


void AudioPlugin2686VEditor::setupOpButtonAndResetWithResize(
    juce::Component& parent,
    juce::ToggleButton& btn,
    std::unique_ptr<ButtonAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int width,
    int height
)
{
    setupOpButtonAndReset(parent, btn, attr, id, title, width, height, true);
}

void AudioPlugin2686VEditor::setupOpTextButton(
    juce::Component& parent,
    juce::TextButton& btn,
    const juce::String& title,
    int width,
    int height,
    bool isResize
)
{
    setupTextButton(parent, btn, title, width, height, isResize);
}

void AudioPlugin2686VEditor::setupOpTextButtonAndReset(
    juce::Component& parent,
    juce::TextButton& btn,
    std::unique_ptr<ButtonAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int width,
    int height,
    bool isResize
)
{
    setupTextButtonAndReset(parent, btn, attr, id, title, width, height, isResize);
}


void AudioPlugin2686VEditor::setupOpTextButtonAndResetWithResize(
    juce::Component& parent,
    juce::TextButton& btn,
    std::unique_ptr<ButtonAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int width,
    int height
)
{
    setupOpTextButtonAndReset(parent, btn, attr, id, title, width, height, true);
}

void AudioPlugin2686VEditor::setupOpSsgEnvComboAndReset(
    juce::Component& parent,
    juce::ComboBox& combo,
    juce::Label& label,
    std::unique_ptr<ComboBoxAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int labelWidth,
    int labelHeigtht
)
{
    std::vector<SelectItem> items = {
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

    setupOpComboAndReset(parent, combo, label, attr, id, title, items, labelWidth, labelHeigtht);
}

void AudioPlugin2686VEditor::setupSsgEnvComboAndReset(
    juce::Component& parent,
    juce::ComboBox& combo,
    juce::Label& label,
    std::unique_ptr<ComboBoxAttachment>& attr,
    const juce::String& id,
    const juce::String& title,
    int labelWidth,
    int labelHeigtht
)
{
    std::vector<SelectItem> items = {
        {.name = "08: \\\\\\\\ (Decay)", .value = 1 },
        {.name = "09: \\___ (Decay->0)", .value = 2 },
        {.name = "0A: \\/\\/ (Tri)", .value = 3 },
        {.name = "0B: \\``` (Decay->1)", .value = 4 },
        {.name = "0C: //// (Attack)", .value = 5 },
        {.name = "0D: /``` (Attack->1)", .value = 6 },
        {.name = "0E: /\\/\\ (Inv Tri)", .value = 7 },
        {.name = "0F: /___ (Attack->0)", .value = 8 },
    };

    setupComboAndReset(parent, combo, label, attr, id, title, items, labelWidth, labelHeigtht);
}

void AudioPlugin2686VEditor::setupSsgMixBtn(juce::TextButton& btn, const juce::String& text, double val)
{
    setupTextButton(ssgGui.page, btn, text);
    btn.onClick = [this, val] { ssgGui.mixSlider.setValue(val, juce::sendNotification); };
};

void AudioPlugin2686VEditor::setupSsgPeakBtn(juce::TextButton& btn, const juce::String& text, double val)
{
    setupTextButton(ssgGui.page, btn, text);
    btn.onClick = [this, val] { ssgGui.triPeakSlider.setValue(val, juce::sendNotification); };
}

void AudioPlugin2686VEditor::setupOpnaGui(Fm4GuiSet& gui)
{
    std::vector<SelectItem> pmsItems = createItems(8);
    std::vector<SelectItem> amsItems = createItems(4);
    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> ksItems = { {.name = "OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    setupGroup(gui.page, gui.globalGroup, "Algorithm / Feedback / LFO");
    setupComboAndReset(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPNA_ALGORITHM", "Algorithm", algItems);
    setupFbSliderAndReset(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPNA_FEEDBACK", "Feedback");
    setupSliderAndReset(gui.page, gui.lfoFreqSlider, gui.lfoFreqLabel, gui.lfoFreqAtt, "OPNA_LFO_FREQ", "LFO FREQ");
    setupComboAndReset(gui.page, gui.lfoPmsSelector, gui.lfoPmsLabel, gui.lfoPmsAtt, "OPNA_LFO_PMS", "LFO PMS", pmsItems);
    setupComboAndReset(gui.page, gui.lfoAmsSelector, gui.lfoAmsLabel, gui.lfoAmsAtt, "OPNA_LFO_AMS", "LFO AMS", amsItems);

    // Operators
    setupOpGroups(gui.page, gui.opGroups);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPNA_OP" + juce::String(i) + "_";

        setupOpSliderAndReset(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupOpSliderAndReset(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + "DT", "DT");
        setupOpSliderAndReset(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupOpSliderAndReset(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR");
        setupOpSliderAndReset(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + "SR", "SR");
        setupOpSliderAndReset(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL");
        setupOpSliderAndReset(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
		setupOpSsgEnvComboAndReset(gui.page, gui.se[i], gui.seLabel[i], gui.seAtt[i], paramPrefix + "SE", "SE");
        setupOpButtonAndReset(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + "FIX", "FIX");
        setupOpSliderAndReset(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + "FREQ", "FRQ");
        setupOpTextButton(gui.page, gui.freqToZero[i], "Freq -> 0Hz");
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        setupOpTextButton(gui.page, gui.freqTo440[i], "Freq -> 440Hz");
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        setupOpSliderAndReset(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupOpComboAndReset(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + "KS", "KS", ksItems);
        setupOpButtonAndReset(gui.page, gui.am[i], gui.amAtt[i], paramPrefix + "AM", "AM");
    }
}

void AudioPlugin2686VEditor::setupOpnGui(Fm4GuiSet& gui)
{
    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> ksItems = { {.name = "OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    setupGroup(gui.page, gui.globalGroup, "Algorithm / Feedback");
    setupComboAndReset(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPN_ALGORITHM", "Algorithm", algItems);
    setupFbSliderAndReset(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPN_FEEDBACK", "Feedback");

    // Operators
    setupOpGroups(gui.page, gui.opGroups);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPN_OP" + juce::String(i) + "_";

        setupOpSliderAndReset(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupOpSliderAndReset(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + "DT", "DT");
        setupOpSliderAndReset(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupOpSliderAndReset(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR");
        setupOpSliderAndReset(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + "SR", "SR");
        setupOpSliderAndReset(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL");
        setupOpSliderAndReset(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
        setupOpButtonAndReset(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + "FIX", "FIX");
        setupOpSliderAndReset(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + "FREQ", "FRQ");
        setupOpTextButton(gui.page, gui.freqToZero[i], "Freq -> 0Hz");
        gui.freqToZero[i].onClick = [this, index = i] { opnGui.freq[index].setValue(0, juce::sendNotification); };
        setupOpTextButton(gui.page, gui.freqTo440[i], "Freq -> 440Hz");
        gui.freqTo440[i].onClick = [this, index = i] { opnGui.freq[index].setValue(440, juce::sendNotification); };
        setupOpSliderAndReset(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupOpComboAndReset(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + "KS", "KS", ksItems);
    }
}

void AudioPlugin2686VEditor::setupOplGui(Fm2GuiSet& gui)
{
    std::vector<SelectItem> algItems = { {.name = "FM(Serial)", .value = 1}, {.name = "AM (Parallel)", .value = 2}, };
    std::vector<SelectItem> wsItems = { {.name = "Sine", .value = 1}, {.name = "Half", .value = 2}, {.name = "Abs", .value = 3}, {.name = "Pulse", .value = 4}, };

    setupGroup(gui.page, gui.globalGroup, "Algorithm / Feedback");
    setupComboAndReset(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPL_ALGORITHM", "Algorithm", algItems);
    setupFbSliderAndReset(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPL_FEEDBACK", "Feedback");

    setupOpGroups(gui.page, gui.opGroups);
    for (int i = 0; i < 2; ++i)
    {
        juce::String paramPrefix = "OPL_OP" + juce::String(i) + "_";

        setupOpSliderAndReset(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupOpSliderAndReset(gui.page, gui.dt[i], gui.dtLabel[i], gui.dtAtt[i], paramPrefix + "DT", "DT");
        setupOpSliderAndReset(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupOpSliderAndReset(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR");
        setupOpSliderAndReset(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL");
        setupOpSliderAndReset(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
        setupOpComboAndReset(gui.page, gui.ws[i], gui.wsLabel[i], gui.wsAtt[i], paramPrefix + "WS", "WS", wsItems);
    }
}

void AudioPlugin2686VEditor::setupOpllGui(OpllGuiSet& gui)
{
    std::vector<SelectItem> wsItems = { {.name = "Sine", .value = 1}, {.name = "Half", .value = 2}, };
    std::vector<SelectItem> kslItems = { {.name = "KSL: 0", .value = 1}, {.name = "KSL: 1", .value = 2}, {.name = "KSL: 2", .value = 3}, {.name = "KSL: 3", .value = 4}, };

    setupGroup(gui.page, gui.globalGroup, "Preset / Global");

    setupFbSliderAndReset(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPLL_FEEDBACK", "Feedback");

    setupOpGroups(gui.page, gui.opGroups);
    for (int i = 0; i < 2; ++i)
    {
        juce::String paramPrefix = "OPLL_OP" + juce::String(i) + "_";

        setupOpSliderAndReset(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupOpSliderAndReset(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupOpSliderAndReset(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupOpSliderAndReset(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR");
        setupOpSliderAndReset(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL");
        setupOpSliderAndReset(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
		setupOpComboAndReset(gui.page, gui.ksl[i], gui.kslLabel[i], gui.kslAtt[i], paramPrefix + "KSL", "KSL", kslItems);
        setupOpComboAndReset(gui.page, gui.ws[i], gui.wsLabel[i], gui.wsAtt[i], paramPrefix + "WS", "WS", wsItems);
        setupOpButtonAndReset(gui.page, gui.am[i], gui.amAtt[i], paramPrefix + "AM", "AM");
        setupOpButtonAndReset(gui.page, gui.vib[i], gui.vibAtt[i], paramPrefix + "VIB", "VIB");
		setupOpButtonAndReset(gui.page, gui.egType[i], gui.egTypeAtt[i], paramPrefix + "EG_TYP", "EGTYP");
        setupOpButtonAndReset(gui.page, gui.ksr[i], gui.ksrAtt[i], paramPrefix + "KSR", "KSR");
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

    setupGroup(gui.page, gui.globalGroup, "Algorithm / Feedback");
    setupComboAndReset(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPL3_ALGORITHM", "Algorithm", algItems);
    setupFbSliderAndReset(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPL3_FEEDBACK", "Feedback");

    setupOpGroups(gui.page, gui.opGroups);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPL3_OP" + juce::String(i) + "_";

        setupOpSliderAndReset(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupOpSliderAndReset(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupOpSliderAndReset(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupOpSliderAndReset(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "DR");
        setupOpSliderAndReset(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "SL");
        setupOpSliderAndReset(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
        setupOpComboAndReset(gui.page, gui.ws[i], gui.wsLabel[i], gui.wsAtt[i], paramPrefix + "WS", "WS", wsItems);
    }
}

void AudioPlugin2686VEditor::setupOpmGui(OpmGuiSet& gui)
{
    std::vector<SelectItem> pmsItems = createItems(8);
    std::vector<SelectItem> amsItems = createItems(4);
    std::vector<SelectItem> algItems = createAlgItems(8);
    std::vector<SelectItem> ksItems = { {.name = "OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

    setupGroup(gui.page, gui.globalGroup, "Algorithm / Feedback / LFO");
    setupComboAndReset(gui.page, gui.algSelector, gui.algLabel, gui.algAtt, "OPM_ALGORITHM", "Algorithm", algItems);
    setupFbSliderAndReset(gui.page, gui.feedbackSlider, gui.feedbackLabel, gui.fbAtt, "OPM_FEEDBACK", "Feedback");
    setupSliderAndReset(gui.page, gui.lfoFreqSlider, gui.lfoFreqLabel, gui.lfoFreqAtt, "OPM_LFO_FREQ", "LFO FREQ");
    setupComboAndReset(gui.page, gui.pmsSelector, gui.pmsLabel, gui.pmsAtt, "OPM_LFO_PMS", "LFO PMS", pmsItems);
    setupComboAndReset(gui.page, gui.amsSelector, gui.amsLabel, gui.amsAtt, "OPM_LFO_AMS", "LFO AMS", amsItems);

    // Operators
    setupOpGroups(gui.page, gui.opGroups);
    for (int i = 0; i < 4; ++i)
    {
        juce::String paramPrefix = "OPM_OP" + juce::String(i) + "_";

        setupOpSliderAndReset(gui.page, gui.mul[i], gui.mulLabel[i], gui.mulAtt[i], paramPrefix + "MUL", "MUL");
        setupOpSliderAndReset(gui.page, gui.dt1[i], gui.dt1Label[i], gui.dt1Att[i], paramPrefix + "DT", "DT");
        setupOpSliderAndReset(gui.page, gui.dt2[i], gui.dt2Label[i], gui.dt2Att[i], paramPrefix + "DT2", "DT2");
        setupOpSliderAndReset(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupOpSliderAndReset(gui.page, gui.ar[i], gui.arLabel[i], gui.arAtt[i], paramPrefix + "AR", "AR");
        setupOpSliderAndReset(gui.page, gui.dr[i], gui.drLabel[i], gui.drAtt[i], paramPrefix + "DR", "D1R");
        setupOpSliderAndReset(gui.page, gui.sl[i], gui.slLabel[i], gui.slAtt[i], paramPrefix + "SL", "D2L");
        setupOpSliderAndReset(gui.page, gui.sr[i], gui.srLabel[i], gui.srAtt[i], paramPrefix + "SR", "D2R");
        setupOpSliderAndReset(gui.page, gui.rr[i], gui.rrLabel[i], gui.rrAtt[i], paramPrefix + "RR", "RR");
        setupOpButtonAndReset(gui.page, gui.fix[i], gui.fixAtt[i], paramPrefix + "FIX", "FIX");
        setupOpSliderAndReset(gui.page, gui.freq[i], gui.freqLabel[i], gui.freqAtt[i], paramPrefix + "FREQ", "FRQ");
        setupOpTextButton(gui.page, gui.freqToZero[i], "Freq -> 0Hz");
        gui.freqToZero[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(0, juce::sendNotification); };
        setupOpTextButton(gui.page, gui.freqTo440[i], "Freq -> 440Hz");
        gui.freqTo440[i].onClick = [this, index = i] { opnaGui.freq[index].setValue(440, juce::sendNotification); };
        setupOpSliderAndReset(gui.page, gui.tl[i], gui.tlLabel[i], gui.tlAtt[i], paramPrefix + "TL", "TL");
        setupOpComboAndReset(gui.page, gui.ks[i], gui.ksLabel[i], gui.ksAtt[i], paramPrefix + "KS", "KS", ksItems);
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

    setupGroup(gui.page, gui.voiceGroup, "Voice");
    setupGroup(gui.page, gui.filterGroup, "Filter (Envelope)");
    setupGroup(gui.page, gui.dutyGroup, "Pulse Width (Duty)");
    setupGroup(gui.page, gui.triGroup, "Triangle Property");
    setupGroup(gui.page, gui.envGroup, "Hardware Envelope");

    setupSliderAndReset(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "SSG_LEVEL", "Tone");
    setupSliderAndReset(gui.page, gui.noiseSlider, gui.noiseLabel, gui.noiseAtt, "SSG_NOISE", "Noise");
    setupSliderAndReset(gui.page, gui.noiseFreqSlider, gui.noiseFreqLabel, gui.noiseFreqAtt, "SSG_NOISE_FREQ", "Noise Freq");
    setupSliderAndReset(gui.page, gui.mixSlider, gui.mixLabel, gui.mixAtt, "SSG_MIX", "Mix (T<-M->N)");
    setupSsgMixBtn(gui.mixSetTone, "Tone", 0.0);
    setupSsgMixBtn(gui.mixSetMix, "Mix", 0.5);
    setupSsgMixBtn(gui.mixSetNoise, "Noise", 1.0);

    // Waveform
    setupComboAndResetWithResize(gui.page, gui.waveSelector, gui.waveLabel, gui.waveAtt, "SSG_WAVEFORM", "Wave Form", wsItems);

    // ADSR Bypass Switch
    setupButtonAndResetWithResize(gui.page, gui.adsrBypassButton, gui.adsrBypassAtt, "SSG_ADSR_BYPASS", "Bypass ADSR (Gate)");

    // SSG ADSR
    setupSliderAndReset(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, "SSG_AR", "Attack");
    setupSliderAndReset(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, "SSG_DR", "Decay");
    setupSliderAndReset(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, "SSG_SL", "Sustain");
    setupSliderAndReset(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, "SSG_RR", "Release");

    // Duty Controls Setup
    setupComboAndResetWithResize(gui.page, gui.dutyModeSelector, gui.dutyModeLabel, gui.dutyModeAtt, "SSG_DUTY_MODE", "Duty Mode", dmItems);
    setupComboAndReset(gui.page, gui.dutyPresetSelector, gui.dutyPresetLabel, gui.dutyPresetAtt, "SSG_DUTY_PRESET", "Duty Preset", prItems);
    setupSliderAndReset(gui.page, gui.dutyVarSlider, gui.dutyVarLabel, gui.dutyVarAtt, "SSG_DUTY_VAR", "Duty Ratio");
    setupButtonAndReset(gui.page, gui.dutyInvertButton, gui.dutyInvertAtt, "SSG_DUTY_INV", "Invert Phase");

    // Key Track Switch
    setupButtonAndResetWithResize(gui.page, gui.triKeyTrackButton, gui.triKeyTrackAtt, "SSG_TRI_KEYTRK", "Key Track (Pitch)");

    // Manual Frequency Slider Setup
    setupSliderAndReset(gui.page, gui.triFreqSlider, gui.triFreqLabel, gui.triFreqAtt, "SSG_TRI_FREQ", "Manual Freq (Hz)");

    // Peak Position
    setupSliderAndReset(gui.page, gui.triPeakSlider, gui.triPeakLabel, gui.triPeakAtt, "SSG_TRI_PEAK", "Peak Position");

    setupSsgPeakBtn(gui.triSetSawDown, "0.0 (Down)", 0.0);
    setupSsgPeakBtn(gui.triSetTri, "0.5 (Tri)", 0.5);
    setupSsgPeakBtn(gui.triSetSawUp, "1.0 (Up)", 1.0);

    // HW Env Group
    setupGroup(gui.page, gui.envGroup, "Hardware Envelope");
    setupButtonAndResetWithResize(gui.page, gui.envEnableButton, gui.envEnableAtt, "SSG_ENV_ENABLE", "Enable HW Env");

    // Shape ComboBox Setup
    setupSsgEnvComboAndReset(gui.page, gui.shapeSelector, gui.shapeLabel, gui.shapeAtt, "SSG_ENV_SHAPE", "Shape");

    // Period Slider
    setupSliderAndReset(gui.page, gui.periodSlider, gui.periodLabel, gui.periodAtt, "SSG_ENV_PERIOD", "Env Speed (Hz)");
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
    setupGroup(gui.page, gui.filterGroup, "Filter (ADSR)");
    setupSliderAndReset(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "WT_LEVEL", "Level");
    setupSliderAndReset(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, "WT_AR", "Attack");
    setupSliderAndReset(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, "WT_DR", "Decay");
    setupSliderAndReset(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, "WT_SL", "Sustain");
    setupSliderAndReset(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, "WT_RR", "Release");

    // --- Property Group (Right) ---
    setupGroup(gui.page, gui.propGroup, "Wavetable Properties");

    // Waveform Selector Setup
    setupComboAndResetWithResize(gui.page, gui.waveSelector, gui.waveLabel, gui.waveAtt, "WT_WAVE", "Waveform", wsItems);

    // Custom Sliders Setup
    setupGroup(gui.page, gui.customGroup, "Custom Wave");
    setupCustomWaveTable(gui.page, gui.customSliders32, gui.customAtts32, "WT_CUSTOM32_");
    setupCustomWaveTable(gui.page, gui.customSliders64, gui.customAtts64, "WT_CUSTOM64_");
    setupComboAndReset(gui.page, gui.bitSelector, gui.bitLabel, gui.bitAtt, "WT_BIT", "Bit Depth", bdItems);
    setupComboAndResetWithResize(gui.page, gui.sizeSelector, gui.sizeLabel, gui.sizeAtt, "WT_SIZE", "Table Size", tsItems);
    setupButtonAndResetWithResize(gui.page, gui.modEnableButton, gui.modEnableAtt, "WT_MOD_ENABLE", "Modulation ON");
    setupSliderAndReset(gui.page, gui.modDepthSlider, gui.modDepthLabel, gui.modDepthAtt, "WT_MOD_DEPTH", "Mod Depth");
    setupSliderAndReset(gui.page, gui.modSpeedSlider, gui.modSpeedLabel, gui.modSpeedAtt, "WT_MOD_SPEED", "Mod Speed");
}

void AudioPlugin2686VEditor::setupRhythmGui(RhythmGuiSet& gui)
{
    setupSliderAndReset(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "RHYTHM_LEVEL", "Master Vol");
}

void AudioPlugin2686VEditor::setupAdpcmGui(AdpcmGuiSet& gui)
{
    setupSliderAndReset(gui.page, gui.levelSlider, gui.levelLabel, gui.levelAtt, "ADPCM_LEVEL", "Master Vol");
    setupButtonAndReset(gui.page, gui.loopButton, gui.loopAtt, "ADPCM_LOOP", "Loop Playback");
    setupSliderAndReset(gui.page, gui.attackSlider, gui.attackLabel, gui.attackAtt, "ADPCM_AR", "Attack");
    setupSliderAndReset(gui.page, gui.decaySlider, gui.decayLabel, gui.decayAtt, "ADPCM_DR", "Decay");
    setupSliderAndReset(gui.page, gui.sustainSlider, gui.sustainLabel, gui.sustainAtt, "ADPCM_SL", "Sustain");
    setupSliderAndReset(gui.page, gui.releaseSlider, gui.releaseLabel, gui.releaseAtt, "ADPCM_RR", "Release");
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