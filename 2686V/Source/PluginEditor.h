#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>
#include "PluginProcessor.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

struct SelectItem
{
	const juce::String name;
    const int id;
};

struct Fm4GuiSet
{
    juce::Component page;

    juce::GroupComponent globalGroup;
    std::array<juce::GroupComponent, 4> opGroups;
    juce::GroupComponent lfoGroup;

    juce::ComboBox algSelector;
    juce::Label algLabel;
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;

    juce::Slider lfoFreqSlider;
    juce::Label lfoFreqLabel;
    juce::ComboBox lfoPmsSelector;
    juce::Label lfoPmsLabel;
    juce::ComboBox lfoAmsSelector;
    juce::Label lfoAmsLabel;

    std::array<juce::Label, 4> opLabels;

    // 9 columns
    std::array<juce::Slider, 4> mul;
    std::array<juce::Slider, 4> dt;
    std::array<juce::Slider, 4> ar;
    std::array<juce::Slider, 4> dr;
    std::array<juce::Slider, 4> sr;
    std::array<juce::Slider, 4> sl;
    std::array<juce::Slider, 4> rr;
    std::array<juce::Slider, 4> freq;
    std::array<juce::Slider, 4> tl;  // Total Level
    std::array<juce::ComboBox, 4> ks; // Key Scale (0-3)
    std::array<juce::ToggleButton, 4> am; // AM Enable
    std::array<juce::ComboBox, 4> se; // SSG-EG Shape Selector
    std::array<juce::ToggleButton, 4> fix;

    std::array<juce::Label, 4> mulLabel;
    std::array<juce::Label, 4> dtLabel;
    std::array<juce::Label, 4> arLabel;
    std::array<juce::Label, 4> drLabel;
    std::array<juce::Label, 4> srLabel;
    std::array<juce::Label, 4> slLabel;
    std::array<juce::Label, 4> rrLabel;
    std::array<juce::Label, 4> freqLabel;
    std::array<juce::Label, 4> tlLabel;  // Total Level
    std::array<juce::Label, 4> ksLabel; // Key Scale (0-3)
    std::array<juce::Label, 4> amLabel; // AM Enable
    std::array<juce::Label, 4> seLabel; // SSG-EG Shape Selector
    std::array<juce::Label, 4> fixLabel;

    // Attachments
    std::unique_ptr<ComboBoxAttachment> algAtt;
    std::unique_ptr<SliderAttachment> fbAtt;
    std::unique_ptr<SliderAttachment> lfoFreqAtt;
    std::unique_ptr<ComboBoxAttachment> lfoPmsAtt;
    std::unique_ptr<ComboBoxAttachment> lfoAmsAtt;

    std::array<std::unique_ptr<SliderAttachment>, 4> mulAtt;
    std::array<std::unique_ptr<SliderAttachment>, 4> dtAtt;
    std::array<std::unique_ptr<SliderAttachment>, 4> arAtt;
    std::array<std::unique_ptr<SliderAttachment>, 4> drAtt;
    std::array<std::unique_ptr<SliderAttachment>, 4> srAtt;
    std::array<std::unique_ptr<SliderAttachment>, 4> slAtt;
    std::array<std::unique_ptr<SliderAttachment>, 4> rrAtt;
    std::array<std::unique_ptr<SliderAttachment>, 4> freqAtt;
    std::array<std::unique_ptr<SliderAttachment>, 4> tlAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, 4> ksAtt;
    std::array<std::unique_ptr<ButtonAttachment>, 4> amAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, 4> seAtt;
    std::array<std::unique_ptr<ButtonAttachment>, 4> fixAtt;
};

struct Fm2GuiSet
{
    juce::Component page;

    juce::GroupComponent globalGroup;
    std::array<juce::GroupComponent, 2> opGroups;

    juce::ComboBox algSelector; juce::Label algLabel;
    juce::Slider feedbackSlider; juce::Label feedbackLabel;

    std::array<juce::Label, 2> opLabels;

    // Cols: MUL, DT, AR, DR, SL, RR, WS (7 cols)
    std::array<juce::Slider, 2> mul;
    std::array<juce::Slider, 2> dt;
    std::array<juce::Slider, 2> ar;
    std::array<juce::Slider, 2> dr;
    std::array<juce::Slider, 2> sl;
    std::array<juce::Slider, 2> rr;
    std::array<juce::ComboBox, 2> ws;

    std::array<juce::Label, 2> mulLabel;
    std::array<juce::Label, 2> dtLabel;
    std::array<juce::Label, 2> arLabel;
    std::array<juce::Label, 2> drLabel;
    std::array<juce::Label, 2> slLabel;
    std::array<juce::Label, 2> rrLabel;
    std::array<juce::Label, 2> wsLabel;

    std::unique_ptr<ComboBoxAttachment> algAtt;
    std::unique_ptr<SliderAttachment> fbAtt;

    std::array<std::unique_ptr<SliderAttachment>, 2> mulAtt;
    std::array<std::unique_ptr<SliderAttachment>, 2> dtAtt;
    std::array<std::unique_ptr<SliderAttachment>, 2> arAtt;
    std::array<std::unique_ptr<SliderAttachment>, 2> drAtt;
    std::array<std::unique_ptr<SliderAttachment>, 2> slAtt;
    std::array<std::unique_ptr<SliderAttachment>, 2> rrAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, 2> wsAtt;
};

struct SsgGuiSet
{
    // --- SSG Page ---
    juce::Component page;

    // Pane Groups
    juce::GroupComponent voiceGroup;
    juce::GroupComponent filterGroup;
    juce::GroupComponent dutyGroup;
    juce::GroupComponent triGroup;
    juce::GroupComponent envGroup;

    juce::Slider levelSlider;
    juce::Label levelLabel;
    juce::Slider noiseSlider;
    juce::Label noiseLabel;
    juce::Slider noiseFreqSlider;
    juce::Label noiseFreqLabel;
    juce::Slider mixSlider;
    juce::Label mixLabel; // Mix Slider
    juce::TextButton mixSetTone;  // 0.0
    juce::TextButton mixSetMix;   // 0.5
    juce::TextButton mixSetNoise; // 1.0
    juce::ComboBox waveSelector;
    juce::Label waveLabel; // Waveform Selector

    // SSG ADSR
    juce::ToggleButton adsrBypassButton; // ADSR Bypass Switch
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    // Duty Controls
    juce::ComboBox dutyModeSelector; // Preset / Variable
	juce::Label dutyModeLabel;
    juce::ComboBox dutyPresetSelector; // 1:1 ... 1:16
    juce::Label dutyPresetLabel;
    juce::Slider dutyVarSlider; // 0.0 - 0.5
    juce::Label dutyVarLabel;
    juce::ToggleButton dutyInvertButton; // Invert Switch
    juce::Label dutyInvertLabel;

    // Triangle Controls
    juce::ToggleButton triKeyTrackButton;
    juce::Slider triFreqSlider;
    juce::Label triFreqLabel;
    juce::Slider triPeakSlider;
    juce::Label triPeakLabel;

    // Peak Preset Buttons
    juce::TextButton triSetSawDown; // 0.0
    juce::TextButton triSetTri;     // 0.5
    juce::TextButton triSetSawUp;   // 1.0

    // HW Env Controls
    juce::ToggleButton envEnableButton;
    juce::ComboBox shapeSelector;
    juce::Label shapeLabel;
    juce::Slider periodSlider;
    juce::Label periodLabel;

    // SSG Attachments
    std::unique_ptr<SliderAttachment> levelAtt;
    std::unique_ptr<SliderAttachment> noiseAtt;
    std::unique_ptr<SliderAttachment> noiseFreqAtt;
    std::unique_ptr<SliderAttachment> mixAtt;
    std::unique_ptr<ComboBoxAttachment> waveAtt;
    std::unique_ptr<ButtonAttachment> adsrBypassAtt;
    std::unique_ptr<SliderAttachment> attackAtt;
    std::unique_ptr<SliderAttachment> decayAtt;
    std::unique_ptr<SliderAttachment> sustainAtt;
    std::unique_ptr<SliderAttachment> releaseAtt;
    std::unique_ptr<ComboBoxAttachment> dutyModeAtt;
    std::unique_ptr<ComboBoxAttachment> dutyPresetAtt;
    std::unique_ptr<SliderAttachment> dutyVarAtt;
    std::unique_ptr<ButtonAttachment> dutyInvertAtt;
    std::unique_ptr<ButtonAttachment> triKeyTrackAtt;
    std::unique_ptr<SliderAttachment> triPeriodAtt;
    std::unique_ptr<SliderAttachment> triPeakAtt;
    std::unique_ptr<SliderAttachment> triFreqAtt;
    std::unique_ptr<ButtonAttachment> envEnableAtt;
    std::unique_ptr<ComboBoxAttachment> shapeAtt;
    std::unique_ptr<SliderAttachment> periodAtt;
};

struct WtGuiSet
{
    // --- Wavetable Page ---
    juce::Component page;

    // Groups
    juce::GroupComponent filterGroup;
    juce::GroupComponent propGroup; // Wavetable Property

    // Custom Waveform Editors
    juce::GroupComponent customGroup;
    std::array<juce::Slider, 32> customSliders32;
    std::array<juce::Slider, 64> customSliders64;

    // Filter (ADSR)
    juce::Slider levelSlider;
    juce::Label levelLabel;
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    // Properties
    juce::ComboBox bitSelector;
    juce::Label bitLabel;
    juce::ComboBox sizeSelector;
    juce::Label sizeLabel;
    juce::ComboBox waveSelector;
    juce::Label waveLabel; // Waveform Selector

    // Modulation
    juce::ToggleButton modEnableButton;
    juce::Slider modDepthSlider;
    juce::Label modDepthLabel;
    juce::Slider modSpeedSlider;
    juce::Label modSpeedLabel;

    // Wavetable Attachments
    std::unique_ptr<SliderAttachment> levelAtt;
    std::unique_ptr<ComboBoxAttachment> bitAtt;
    std::unique_ptr<ComboBoxAttachment> sizeAtt;
    std::unique_ptr<ComboBoxAttachment> waveAtt;
    std::array<std::unique_ptr<SliderAttachment>, 32> customAtts32;
    std::array<std::unique_ptr<SliderAttachment>, 64> customAtts64;
    std::unique_ptr<SliderAttachment> attackAtt;
    std::unique_ptr<SliderAttachment> decayAtt;
    std::unique_ptr<SliderAttachment> sustainAtt;
    std::unique_ptr<SliderAttachment> releaseAtt;
    std::unique_ptr<ButtonAttachment> modEnableAtt;
    std::unique_ptr<SliderAttachment> modDepthAtt;
    std::unique_ptr<SliderAttachment> modSpeedAtt;
};

struct RhythmGuiSet
{
    // --- Rhythm Page ---
    juce::Component page;
    juce::Slider levelSlider;
    juce::Label levelLabel;

    // Rhythm Attachments
    std::unique_ptr<SliderAttachment> levelAtt;
};

struct AdpcmGuiSet
{
    // --- ADPCM Page ---
    juce::Component page;
    juce::Slider levelSlider;
    juce::Label levelLabel;

    // Loop Button
    juce::ToggleButton loopButton;

    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    // ADPCM Attachments
    std::unique_ptr<SliderAttachment> levelAtt;
    std::unique_ptr<ButtonAttachment> loopAtt;
    std::unique_ptr<SliderAttachment> attackAtt;
    std::unique_ptr<SliderAttachment> decayAtt;
    std::unique_ptr<SliderAttachment> sustainAtt;
    std::unique_ptr<SliderAttachment> releaseAtt;
};

class AudioPlugin2686VEditor : public juce::AudioProcessorEditor,
    public juce::ChangeListener,
    public juce::ComponentListener
{
public:
    AudioPlugin2686VEditor(AudioPlugin2686V&);
    ~AudioPlugin2686VEditor() override;

	static const int TopGroupHeight = 80;
    static const int TopGroupPaddingWidth = 10;
    static const int TopGroupPaddingHeight = 10;
    static const int SliderLabelWidth = 40;
    static const int SliderLabelHeight = 20;
    static const int ButtonWidth = 80;
    static const int ButtonHeight = 20;

    static const int OpValueWidth = 50;
    static const int OpValueHeight = 20;
    static const int OpLabelWidth = 50;
    static const int OpLabelHeight = 20;
    static const int OpButtonWidth = 80;
    static const int OpButtonHeight = 20;

    void paint(juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;
private:
    AudioPlugin2686V& audioProcessor;
    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };

    Fm4GuiSet opnaGui;  // OPNA
    Fm4GuiSet opnGui; // OPN
	Fm2GuiSet oplGui; // OPL
	SsgGuiSet ssgGui; // SSG
	WtGuiSet wtGui; // Wavetable
	RhythmGuiSet rhythmGui; // Rhythm
	AdpcmGuiSet adpcmGui; // ADPCM

    // Helper
    std::vector<SelectItem> createItems(int size, const juce::String& prefix = "");
    std::vector<SelectItem> createAlgItems(int size);
    void setupTabs(juce::TabbedComponent & tabs);
    void setupGroup(
        juce::Component& page,
        juce::GroupComponent& group,
        const juce::String& title
    );
    void setupLabel(
        juce::Component& page,
        juce::Component& parent,
        juce::Label& label,
        const juce::String& title,
        int width = SliderLabelWidth,
        int heigtht = SliderLabelHeight
    );
    void setupSlider(
        juce::Component& parent,
        juce::Slider& slider,
        juce::Label& label,
        const juce::String& title,
        int labelWidth = SliderLabelWidth,
        int labelHeigtht = SliderLabelHeight
    );
    void setupSliderAndReset(
        juce::Component& page,
        juce::Slider& s,
        juce::Label& label,
        std::unique_ptr<SliderAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        int labelWidth = SliderLabelWidth,
        int labelHeigtht = SliderLabelHeight
    );
    void setupCombo(
        juce::Component& parent,
        juce::ComboBox& combo,
        juce::Label& label,
        const juce::String& title,
        std::vector<SelectItem>& items,
        int labelWidth = SliderLabelWidth,
        int labelHeigtht = SliderLabelHeight,
		bool isResize = false
    );
    void setupComboAndReset(
        juce::Component& parent,
        juce::ComboBox& combo,
        juce::Label& label,
        std::unique_ptr<ComboBoxAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        std::vector<SelectItem>& items,
        int labelWidth = SliderLabelWidth,
        int labelHeigtht = SliderLabelHeight,
		bool isResize = false
    );
    void setupComboAndResetWithResize(
        juce::Component& parent,
        juce::ComboBox& combo,
        juce::Label& label,
        std::unique_ptr<ComboBoxAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        std::vector<SelectItem>& items,
        int labelWidth = SliderLabelWidth,
        int labelHeigtht = SliderLabelHeight
    );
    void setupButton(
        juce::Component& parent,
        juce::ToggleButton& btn,
        const juce::String& title,
		int width = ButtonWidth,
		int height = ButtonHeight,
        bool isResize = false
    );
    void setupButtonAndReset(
        juce::Component& parent,
        juce::ToggleButton& btn,
        std::unique_ptr<ButtonAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        int width = ButtonWidth,
        int height = ButtonHeight,
        bool isResize = false
    );
    void setupButtonAndResetWithResize(
        juce::Component& parent,
        juce::ToggleButton& btn,
        std::unique_ptr<ButtonAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        int width = ButtonWidth,
        int height = ButtonHeight
    );
    void setupFbSliderAndReset(
        juce::Component& page,
        juce::Slider& s,
        juce::Label& label,
        std::unique_ptr<SliderAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        int labelWidth = SliderLabelWidth,
        int labelHeight = SliderLabelHeight
    );
	template <size_t tableSize>
    void setupCustomWaveTable(
        juce::Component& page,
        std::array<juce::Slider, tableSize>& wts,
        std::array<std::unique_ptr<SliderAttachment>, tableSize>& attrs,
        const juce::String& idPrefix
    );
    template <size_t opCount>
    void setupOpGroups(
        juce::Component& page,
        std::array<juce::GroupComponent, opCount>& groups
    );
    void setupOpLabel(
        juce::Component& page,
        juce::Component& parent,
        juce::Label& label,
        const juce::String& title,
		int width = OpLabelWidth,
		int heigtht = OpLabelHeight
    );
    void setupOpSlider(
        juce::Component& page,
        juce::Slider& s,
        juce::Label& label,
        const juce::String& title,
        int labelWidth = OpLabelWidth,
        int labelHeigtht = OpLabelHeight
    );
    void setupOpSliderAndReset(
        juce::Component& page,
        juce::Slider& s,
        juce::Label& label,
        std::unique_ptr<SliderAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        int labelWidth = OpLabelWidth,
        int labelHeigtht = OpLabelHeight
    );
    void setupOpCombo(
        juce::Component& parent,
        juce::ComboBox& combo,
        juce::Label& label,
        const juce::String& title,
		std::vector<SelectItem>& items,
        int labelWidth = OpLabelWidth,
        int labelHeigtht = OpLabelHeight,
		bool isResize = false
	);
    void setupOpComboAndReset(
        juce::Component& parent,
        juce::ComboBox& combo,
        juce::Label& label,
        std::unique_ptr<ComboBoxAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        std::vector<SelectItem>& items,
        int labelWidth = OpLabelWidth,
        int labelHeigtht = OpLabelHeight,
		bool isResize = false
    );
    void setupOpComboAndResetWithResize(
        juce::Component& parent,
        juce::ComboBox& combo,
        juce::Label& label,
        std::unique_ptr<ComboBoxAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        std::vector<SelectItem>& items,
        int labelWidth = OpLabelWidth,
        int labelHeigtht = OpLabelHeight
    );
    void setupOpButton(
        juce::Component& parent,
        juce::ToggleButton& btn,
        const juce::String& title,
        int width = OpButtonWidth,
        int height = OpButtonHeight,
        bool isResize = false
    );
    void setupOpButtonAndReset(
        juce::Component& parent,
        juce::ToggleButton& btn,
        std::unique_ptr<ButtonAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        int width = OpButtonWidth,
        int height = OpButtonHeight,
        bool isResize = false
    );
    void setupOpButtonAndResetWithResize(
        juce::Component& parent,
        juce::ToggleButton& btn,
        std::unique_ptr<ButtonAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        int width = OpButtonWidth,
        int height = OpButtonHeight
    );
    void setupOpSsgEnvComboAndReset(
        juce::Component& parent,
        juce::ComboBox& combo,
        juce::Label& label,
        std::unique_ptr<ComboBoxAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        int labelWidth = OpLabelWidth,
        int labelHeigtht = OpLabelHeight
    );
    void setupSsgEnvComboAndReset(
        juce::Component& parent,
        juce::ComboBox& combo,
        juce::Label& label,
        std::unique_ptr<ComboBoxAttachment>& attr,
        const juce::String& id,
        const juce::String& title,
        int labelWidth = OpLabelWidth,
        int labelHeigtht = OpLabelHeight
    );
    void setupSsgMixBtn(juce::TextButton& btn, const juce::String& text, double val);
    void setupSsgPeakBtn(juce::TextButton& btn, const juce::String& text, double val);
    void setupOpnaGui(Fm4GuiSet& gui);
    void setupOpnGui(Fm4GuiSet& gui);
    void setupOplGui(Fm2GuiSet& gui);
    void setupSsgGui(SsgGuiSet& gui);
    void setupWtGui(WtGuiSet& gui);
    void setupRhythmGui(RhythmGuiSet& gui);
    void setupAdpcmGui(AdpcmGuiSet& gui);

    void layoutOpnaPage(Fm4GuiSet& gui, juce::Rectangle<int> content);
    void layoutOpnPage(Fm4GuiSet& gui, juce::Rectangle<int> content);
    void layoutOplPage(Fm2GuiSet& gui, juce::Rectangle<int> content);
    void layoutSsgPage(SsgGuiSet& gui, juce::Rectangle<int> content);
    void layoutWtPage(WtGuiSet& gui);
    void layoutRhythmPage(RhythmGuiSet& gui, juce::Rectangle<int> content);
    void layoutAdpcmPage(AdpcmGuiSet& gui, juce::Rectangle<int> content);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686VEditor)
};
