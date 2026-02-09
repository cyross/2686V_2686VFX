#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>
#include "PluginProcessor.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

// 構造体で使うので、グローバルに定義
static const juce::String VstName = "2686V";
static const juce::String FontFamily = "Times New Roman";
static const float LogoFontSize = 128.0f;

static const int WindowWidth = 1000;
static const int WindowHeight = 800;

static const int TabNumber = 9;

static const int Fm4Ops = 4;
static const int Fm2Ops = 2;

static const int WtSamples32 = 32;
static const int WtSamples64 = 64;

static const int TopGroupHeight = 80;
static const int TopGroupPaddingWidth = 10;
static const int TopGroupPaddingHeight = 10;
static const int LabelWidth = 40;
static const int LabelHeight = 20;
static const int SliderWidth = 50;
static const int SliderHeight = 20;
static const int SliderValueWidth = 50;
static const int SliderValueHeight = 20;
static const int ComboBoxWidth = 50;
static const int ComboBoxHeight = 20;
static const int ToggleButtonWidth = 80;
static const int ToggleButtonHeight = 20;
static const int TextButtonWidth = 80;
static const int TextButtonHeight = 20;

static const int OpLabelWidth = 40;
static const int OpLabelHeight = 15;
static const int OpSliderWidth = 50;
static const int OpSliderHeight = 15;
static const int OpSliderValueWidth = 50;
static const int OpSliderValueHeight = 15;
static const int OpComboBoxWidth = 80;
static const int OpComboBoxHeight = 15;
static const int OpToggleButtonWidth = 80;
static const int OpToggleButtonHeight = 15;
static const int OpTextButtonWidth = 80;
static const int OpTextButtonHeight = 15;

static const int FmOpRowH = 36;

struct SelectItem
{
	const juce::String name;
    const int value;
};

struct Size
{
    int width;
    int height;
};

struct Flags
{
    bool isReset;
    bool isResize;
};

static const Size LabelSize{ LabelWidth, LabelHeight };
static const Size SliderSize{ SliderWidth, SliderHeight };
static const Size SliderValueSize{ SliderValueWidth, SliderValueHeight };
static const Size ComboBoxSize{ ComboBoxWidth, ComboBoxHeight };
static const Size ToggleButtonSize{ ToggleButtonWidth, ToggleButtonHeight };
static const Size TextButtonSize{ TextButtonWidth, TextButtonHeight };
static const Size OpSliderSize{ OpSliderWidth, OpSliderHeight };
static const Size OpSliderValueSize{ OpSliderValueWidth, OpSliderValueHeight };
static const Size OpComboBoxSize{ OpComboBoxWidth, OpComboBoxHeight };
static const Size OpToggleButtonSize{ OpToggleButtonWidth, OpToggleButtonHeight };
static const Size OpTextButtonSize{ OpTextButtonWidth, OpTextButtonHeight };
static const Size OpLabelSize{ OpLabelWidth, OpLabelHeight };
static const Flags SliderFlags{ true, false };
static const Flags ComboBoxFlags{ true, false };
static const Flags ToggleButtonFlags{ true, false };
static const Flags TextButtonFlags{ true, false };
static const Flags OpSliderFlags{ true, false };
static const Flags OpComboBoxFlags{ true, false };
static const Flags OpToggleButtonFlags{ true, false };
static const Flags OpTextButtonFlags{ true, false };

struct Fm4GuiSet
{
    juce::Component page;

    juce::GroupComponent globalGroup;
    std::array<juce::GroupComponent, Fm4Ops> opGroups;
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

    std::array<juce::Label, Fm4Ops> opLabels;

    // 9 columns
    std::array<juce::Slider, Fm4Ops> mul;
    std::array<juce::Slider, Fm4Ops> dt;
    std::array<juce::Slider, Fm4Ops> ar;
    std::array<juce::Slider, Fm4Ops> dr;
    std::array<juce::Slider, Fm4Ops> sr;
    std::array<juce::Slider, Fm4Ops> sl;
    std::array<juce::Slider, Fm4Ops> rr;
    std::array<juce::ToggleButton, Fm4Ops> fix;
    std::array<juce::Slider, Fm4Ops> freq;
    std::array<juce::GroupComponent, Fm4Ops> freqBtnGroup;
    std::array<juce::TextButton, Fm4Ops> freqToZero;
    std::array<juce::TextButton, Fm4Ops> freqTo440;
    std::array<juce::Slider, Fm4Ops> tl;  // Total Level
    std::array<juce::ComboBox, Fm4Ops> ks; // Key Scale (0-3)
    std::array<juce::ToggleButton, Fm4Ops> am; // AM Enable
    std::array<juce::ComboBox, Fm4Ops> se; // SSG-EG Shape Selector

    std::array<juce::Label, Fm4Ops> mulLabel;
    std::array<juce::Label, Fm4Ops> dtLabel;
    std::array<juce::Label, Fm4Ops> arLabel;
    std::array<juce::Label, Fm4Ops> drLabel;
    std::array<juce::Label, Fm4Ops> srLabel;
    std::array<juce::Label, Fm4Ops> slLabel;
    std::array<juce::Label, Fm4Ops> rrLabel;
    std::array<juce::Label, Fm4Ops> fixLabel;
    std::array<juce::Label, Fm4Ops> freqLabel;
    std::array<juce::Label, Fm4Ops> freqBtnGroupLabel;
    std::array<juce::Label, Fm4Ops> freqToZeroLabel;
    std::array<juce::Label, Fm4Ops> freqTo440Label;
    std::array<juce::Label, Fm4Ops> tlLabel;  // Total Level
    std::array<juce::Label, Fm4Ops> ksLabel; // Key Scale (0-3)
    std::array<juce::Label, Fm4Ops> amLabel; // AM Enable
    std::array<juce::Label, Fm4Ops> seLabel; // SSG-EG Shape Selector

    // Attachments
    std::unique_ptr<ComboBoxAttachment> algAtt;
    std::unique_ptr<SliderAttachment> fbAtt;
    std::unique_ptr<SliderAttachment> lfoFreqAtt;
    std::unique_ptr<ComboBoxAttachment> lfoPmsAtt;
    std::unique_ptr<ComboBoxAttachment> lfoAmsAtt;

    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> mulAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> dtAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> arAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> drAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> srAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> slAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> rrAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> freqAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> freqToZeroAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> freqTo440Att;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> tlAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> ksAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> amAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> seAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> fixAtt;
};

struct Fm2GuiSet
{
    juce::Component page;

    juce::GroupComponent globalGroup;
    std::array<juce::GroupComponent, Fm2Ops> opGroups;

    juce::ComboBox algSelector; juce::Label algLabel;
    juce::Slider feedbackSlider; juce::Label feedbackLabel;

    std::array<juce::Label, Fm2Ops> opLabels;

    // Cols: MUL, DT, AR, DR, SL, RR, WS (7 cols)
    std::array<juce::Slider, Fm2Ops> mul;
    std::array<juce::Slider, Fm2Ops> dt;
    std::array<juce::Slider, Fm2Ops> ar;
    std::array<juce::Slider, Fm2Ops> dr;
    std::array<juce::Slider, Fm2Ops> sl;
    std::array<juce::Slider, Fm2Ops> rr;
    std::array<juce::ComboBox, Fm2Ops> ws;

    std::array<juce::Label, Fm2Ops> mulLabel;
    std::array<juce::Label, Fm2Ops> dtLabel;
    std::array<juce::Label, Fm2Ops> arLabel;
    std::array<juce::Label, Fm2Ops> drLabel;
    std::array<juce::Label, Fm2Ops> slLabel;
    std::array<juce::Label, Fm2Ops> rrLabel;
    std::array<juce::Label, Fm2Ops> wsLabel;

    std::unique_ptr<ComboBoxAttachment> algAtt;
    std::unique_ptr<SliderAttachment> fbAtt;

    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> mulAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> dtAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> arAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> drAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> slAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> rrAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm2Ops> wsAtt;
};

struct OpllGuiSet
{
    juce::Component page;

    juce::GroupComponent globalGroup;             // Preset & Feedback
    std::array<juce::GroupComponent, Fm2Ops> opGroups; // OP1, OP2

    // Feedback (User Patch)
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;

    std::array<juce::Label, Fm2Ops> opLabel;

    // Sliders (User Patch)
    std::array<juce::Slider, Fm2Ops> mul, tl, ar, dr, sl, rr;
    std::array<juce::Label, Fm2Ops> mulLabel, tlLabel, arLabel, drLabel, slLabel, rrLabel;
    std::array<juce::ComboBox, Fm2Ops> ksl; // Key Scale Level
    std::array<juce::Label, Fm2Ops> kslLabel;
    std::array<juce::ComboBox, Fm2Ops> ws; // Wave Select
    std::array<juce::Label, Fm2Ops> wsLabel;
    std::array<juce::ToggleButton, Fm2Ops> am, vib, egType, ksr;
    std::array<juce::Label, Fm2Ops> amLabel, vibLabel, egTypeLabel, ksrLabel;

    // Attachments
    std::unique_ptr<ComboBoxAttachment> presetAtt;
    std::unique_ptr<SliderAttachment> fbAtt;

    std::array<std::unique_ptr<SliderAttachment>, Fm2Ops> mulAtt, tlAtt, arAtt, drAtt, slAtt, rrAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm2Ops> wsAtt, kslAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm2Ops> amAtt, vibAtt, egTypeAtt, ksrAtt;
};

struct Opl3GuiSet
{
    juce::Component page;

    juce::GroupComponent globalGroup;
    std::array<juce::GroupComponent, Fm4Ops> opGroups; // 4 Operators

    juce::ComboBox algSelector; juce::Label algLabel;
    juce::Slider feedbackSlider; juce::Label feedbackLabel;

    // Sliders
    std::array<juce::Slider, Fm4Ops> mul, tl, ar, dr, sl, rr;
    std::array<juce::Label, Fm4Ops> mulLabel, tlLabel, arLabel, drLabel, slLabel, rrLabel;
    std::array<juce::ComboBox, Fm4Ops> ws; // Wave Select (8 types)
    std::array<juce::Label, Fm4Ops> wsLabel;

    // Attachments
    std::unique_ptr<ComboBoxAttachment> algAtt;
    std::unique_ptr<SliderAttachment> fbAtt;

    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> mulAtt, tlAtt, arAtt, drAtt, slAtt, rrAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> wsAtt;
};

struct OpmGuiSet
{
    juce::Component page;

    juce::GroupComponent globalGroup;
    juce::GroupComponent lfoGroup; // LFO��p�̘g��
    std::array<juce::GroupComponent, Fm4Ops> opGroups;

    // Global
    juce::ComboBox algSelector; juce::Label algLabel;
    juce::Slider feedbackSlider; juce::Label feedbackLabel;

    // OPM LFO
    juce::Slider lfoFreqSlider; juce::Label lfoFreqLabel;
    juce::ComboBox lfoWaveSelector; juce::Label lfoWaveLabel;
    juce::ComboBox pmsSelector; juce::Label pmsLabel;
    juce::ComboBox amsSelector; juce::Label amsLabel;

    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<juce::Slider, Fm4Ops> mul, dt1, dt2, tl, ar, dr, sl, sr, rr;
    std::array<juce::Label, Fm4Ops> mulLabel, dt1Label, dt2Label, tlLabel, arLabel, drLabel, slLabel, srLabel, rrLabel, ksLabel;
    std::array<juce::ComboBox, Fm4Ops> ks;

    std::array<juce::ToggleButton, Fm4Ops> fix;
    std::array<juce::Slider, Fm4Ops> freq;
    std::array<juce::TextButton, Fm4Ops> freqToZero;
    std::array<juce::TextButton, Fm4Ops> freqTo440;

    std::array<juce::Label, Fm4Ops> fixLabel;
    std::array<juce::Label, Fm4Ops> freqLabel;
    std::array<juce::Label, Fm4Ops> freqToZeroLabel;
    std::array<juce::Label, Fm4Ops> freqTo440Label;

    // Attachments
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> mulAtt, dt1Att, dt2Att, tlAtt, arAtt, drAtt, slAtt, srAtt, rrAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> fixAtt;
    std::array<std::unique_ptr<SliderAttachment>, Fm4Ops> freqAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> ksAtt;
    std::array<std::unique_ptr<ComboBoxAttachment>, Fm4Ops> seAtt;
    std::unique_ptr<ComboBoxAttachment> algAtt, lfoWaveAtt, pmsAtt, amsAtt;
    std::unique_ptr<SliderAttachment> fbAtt, lfoFreqAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> freqToZeroAtt;
    std::array<std::unique_ptr<ButtonAttachment>, Fm4Ops> freqTo440Att;

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
    juce::Label mixSetToneLabel;
    juce::TextButton mixSetMix;   // 0.5
    juce::Label mixSetMixLabel;
    juce::TextButton mixSetNoise; // 1.0
    juce::Label mixSetNoiseLabel;
    juce::ComboBox waveSelector;
    juce::Label waveLabel; // Waveform Selector

    // SSG ADSR
    juce::ToggleButton adsrBypassButton; // ADSR Bypass Switch
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Label adsrBypassButtonLabel;
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
    juce::Label triKeyTrackLabel;
    juce::Slider triFreqSlider;
    juce::Label triFreqLabel;
    juce::Slider triPeakSlider;
    juce::Label triPeakLabel;

    // Peak Preset Buttons
    juce::TextButton triSetSawDown; // 0.0
    juce::Label triSetSawDownLabel;
    juce::TextButton triSetTri;     // 0.5
    juce::Label triSetTriLabel;
    juce::TextButton triSetSawUp;   // 1.0
    juce::Label triSetSawUpLabel;

    // HW Env Controls
    juce::ToggleButton envEnableButton;
    juce::Label envEnableLabel;
    juce::ComboBox shapeSelector;
    juce::Label shapeLabel;
    juce::Slider periodSlider;
    juce::Label periodLabel;

    // SSG Attachments
    std::unique_ptr<SliderAttachment> levelAtt;
    std::unique_ptr<SliderAttachment> noiseAtt;
    std::unique_ptr<SliderAttachment> noiseFreqAtt;
    std::unique_ptr<SliderAttachment> mixAtt;
    std::unique_ptr<ButtonAttachment> mixSetAtt;
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
    juce::Label modEnableLabel;
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

    juce::ComboBox modeCombo;
    juce::Label modeLabel;

    juce::TextButton loadButton;
    juce::Label fileNameLabel;

    juce::Slider levelSlider;
    juce::Label levelLabel;

    // Loop Button
    juce::ToggleButton loopButton;
    juce::Label loopLabel;

    juce::Slider attackSlider;
    juce::Label attackLabel;

    juce::Slider decaySlider;
    juce::Label decayLabel;

    juce::Slider sustainSlider;
    juce::Label sustainLabel;

    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    juce::ComboBox rateCombo;
    juce::Label rateLabel;

    // ADPCM Attachments
    std::unique_ptr<ComboBoxAttachment> modeAtt;
    std::unique_ptr<SliderAttachment> levelAtt;
    std::unique_ptr<ButtonAttachment> loopAtt;
    std::unique_ptr<SliderAttachment> attackAtt;
    std::unique_ptr<SliderAttachment> decayAtt;
    std::unique_ptr<SliderAttachment> sustainAtt;
    std::unique_ptr<SliderAttachment> releaseAtt;
    std::unique_ptr<ComboBoxAttachment> rateAtt;
};

struct SetupGroupParams
{
    juce::Component& page;
    juce::GroupComponent& group;
    const juce::String title;
};

struct SetupLabelParams
{
    juce::Component& page;
    juce::Component& parent;
    juce::Label& label;
    const juce::String title;
    int width;
    int height;

    static SetupLabelParams create(
        juce::Component& p,
        juce::Component& par,
        juce::Label& l,
        const juce::String t, 
        int width = LabelWidth,
        int height = LabelHeight
    )
    {
		return SetupLabelParams(p, par, l, t, width, height);
    }

    static SetupLabelParams createOp(
        juce::Component& p,
        juce::Component& par,
        juce::Label& l,
        const juce::String t,
        int width = OpLabelWidth,
        int height = OpLabelHeight
    )
    {
        return SetupLabelParams(p, par, l, t, width, height);
    }
};

struct SetupSliderParams
{
    juce::Component& parent;
    juce::Slider& slider;
    juce::Label& label;
    std::unique_ptr<SliderAttachment>& attr;
    const juce::String id;
    const juce::String title;
    int width;
    int height;
    int valueWidth;
    int valueHeight;
    int labelWidth;
    int labelHeight;
    bool isReset;

    static SetupSliderParams create(
        juce::Component& p,
        juce::Slider& s,
        juce::Label& l,
        std::unique_ptr<SliderAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = SliderSize,
        Size valueSize = SliderValueSize,
        Size labelSize = LabelSize,
        Flags flags = SliderFlags
    )
    {
        return SetupSliderParams(p, s, l, a, i, t, size.width, size.height, valueSize.width, valueSize.height, labelSize.width, labelSize.height, flags.isReset);
    }

    static SetupSliderParams createOp(
        juce::Component& p,
        juce::Slider& s,
        juce::Label& l,
        std::unique_ptr<SliderAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = OpSliderSize,
        Size valueSize = OpSliderValueSize,
        Size labelSize = OpLabelSize,
        Flags flags = OpSliderFlags
    )
    {
        return SetupSliderParams(p, s, l, a, i, t, size.width, size.height, valueSize.width, valueSize.height, labelSize.width, labelSize.height, flags.isReset);
    }
};

struct SetupComboParams
{
    juce::Component& parent;
    juce::ComboBox& combo;
    juce::Label& label;
    std::unique_ptr<ComboBoxAttachment>& attr;
    const juce::String id;
    const juce::String title;
    std::vector<SelectItem>& items;
    int width;
    int height;
    int labelWidth;
    int labelHeight;
    bool isReset;
    bool isResize;

    static SetupComboParams create(
        juce::Component& p,
        juce::ComboBox& c,
        juce::Label& l,
        std::unique_ptr<ComboBoxAttachment>& a,
        const juce::String i,
        const juce::String t,
        std::vector<SelectItem>& items,
        Size size = ComboBoxSize,
        Size labelSize = LabelSize,
        Flags flags = ComboBoxFlags
    )
    {
        return SetupComboParams(p, c, l, a, i, t, items, size.width, size.height, labelSize.width, labelSize.height, flags.isReset, flags.isResize);
    }

    static SetupComboParams createOp(
        juce::Component& p,
        juce::ComboBox& c,
        juce::Label& l,
        std::unique_ptr<ComboBoxAttachment>& a,
        const juce::String i,
        const juce::String t,
        std::vector<SelectItem>& items,
        Size size = OpComboBoxSize,
        Size labelSize = OpLabelSize,
        Flags flags = OpComboBoxFlags
    )
    {
        return SetupComboParams(p, c, l, a, i, t, items, size.width, size.height, labelSize.width, labelSize.height, flags.isReset, flags.isResize);
    }
};

struct SetupToggleButtonParams
{
    juce::Component& parent;
    juce::ToggleButton& btn;
    juce::Label& label;
    std::unique_ptr<ButtonAttachment>& attr;
    const juce::String id;
    const juce::String title;
    int width;
    int height;
    bool isReset;
    bool isResize;

    static SetupToggleButtonParams create(
        juce::Component& p,
        juce::ToggleButton& b,
        juce::Label& l,
        std::unique_ptr<ButtonAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = ToggleButtonSize,
        Flags flags = ToggleButtonFlags
    )
    {
        return SetupToggleButtonParams(p, b, l, a, i, t, size.width, size.height, flags.isReset, flags.isResize);
    }

    static SetupToggleButtonParams createOp(
        juce::Component& p,
        juce::ToggleButton& b,
        juce::Label& l,
        std::unique_ptr<ButtonAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = OpToggleButtonSize,
        Flags flags = OpToggleButtonFlags
    )
    {
        return SetupToggleButtonParams(p, b, l, a, i, t, size.width, size.height, flags.isReset, flags.isResize);
    }
};

struct SetupTextButtonParams
{
    juce::Component& parent;
    juce::TextButton& btn;
    std::unique_ptr<ButtonAttachment>& attr;
    const juce::String id;
    const juce::String title;
    int width;
    int height;
    bool isReset;
    bool isResize;

    static SetupTextButtonParams create(
        juce::Component& p,
        juce::TextButton& b,
        std::unique_ptr<ButtonAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = TextButtonSize,
        Flags flags = TextButtonFlags
    )
    {
        return SetupTextButtonParams(p, b, a, i, t, size.width, size.height, flags.isReset, flags.isResize);
    }

    static SetupTextButtonParams createOp(
        juce::Component& p,
        juce::TextButton& b,
        std::unique_ptr<ButtonAttachment>& a,
        const juce::String i,
        const juce::String t,
        Size size = OpTextButtonSize,
        Flags flags = OpTextButtonFlags
    )
    {
        return SetupTextButtonParams(p, b, a, i, t, size.width, size.height, flags.isReset, flags.isResize);
    }
};

template <size_t tableSize>
struct SetupCustomWaveTableParams
{
    juce::Component& page;
    std::array<juce::Slider, tableSize>& wts;
    std::array<std::unique_ptr<SliderAttachment>, tableSize>& attrs;
    const juce::String idPrefix;
};

template <size_t opCount>
struct SetupOpGroupsParams
{
    juce::Component& page;
    std::array<juce::GroupComponent, opCount>& groups;
};

class AudioPlugin2686VEditor :
    public juce::AudioProcessorEditor,
    public juce::ChangeListener,
    public juce::ComponentListener,
    public juce::Button::Listener
{
public:
    AudioPlugin2686VEditor(AudioPlugin2686V&);
    ~AudioPlugin2686VEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;
    void buttonClicked(juce::Button* button) override; 
private:
    AudioPlugin2686V& audioProcessor;
    juce::TabbedComponent tabs{ juce::TabbedButtonBar::TabsAtTop };
	juce::Label logoLabel;
    std::unique_ptr<juce::FileChooser> fileChooser;

    Fm4GuiSet opnaGui;  // OPNA
    Fm4GuiSet opnGui; // OPN
	Fm2GuiSet oplGui; // OPL
	OpllGuiSet opllGui; // OPLL
    Opl3GuiSet opl3Gui; // OPL3
    OpmGuiSet opmGui; // OPM
    SsgGuiSet ssgGui; // SSG
	WtGuiSet wtGui; // Wavetable
	RhythmGuiSet rhythmGui; // Rhythm
	AdpcmGuiSet adpcmGui; // ADPCM

    // Helper
    // 背景描画
    void drawBg(juce::Graphics& g);
    std::vector<SelectItem> createItems(int size, const juce::String& prefix = "");
    std::vector<SelectItem> createAlgItems(int size);
    void attatchLabelToComponent(juce::Label& label, juce::Component& component);
    void setupLogo();
    void setupTabs(juce::TabbedComponent& tabs);
    void setupGroup(SetupGroupParams& params);
	void setupLabel(SetupLabelParams& params);
	void setupSlider(SetupSliderParams& params);
	void setupCombo(SetupComboParams& params);
	void setupToggleButton(SetupToggleButtonParams& params);
	void setupTextButton(SetupTextButtonParams& params);
	void setupFbSlider(SetupSliderParams& params);
	template <size_t tableSize>
	void setupCustomWaveTable(SetupCustomWaveTableParams<tableSize>& params);
    template <size_t opCount>
    void setupOpGroups(SetupOpGroupsParams<opCount>& params);
    void setupOpnaGui(Fm4GuiSet& gui);
    void setupOpnGui(Fm4GuiSet& gui);
    void setupOplGui(Fm2GuiSet& gui);
    void setupOpllGui(OpllGuiSet& gui);
    void setupOpl3Gui(Opl3GuiSet& gui);
    void setupOpmGui(OpmGuiSet& gui);
    void setupSsgGui(SsgGuiSet& gui);
    void setupWtGui(WtGuiSet& gui);
    void setupRhythmGui(RhythmGuiSet& gui);
    void setupAdpcmGui(AdpcmGuiSet& gui);

    void layoutOpnaPage(Fm4GuiSet& gui, juce::Rectangle<int> content);
    void layoutOpnPage(Fm4GuiSet& gui, juce::Rectangle<int> content);
    void layoutOplPage(Fm2GuiSet& gui, juce::Rectangle<int> content);
    void layoutOpllPage(OpllGuiSet& gui, juce::Rectangle<int> content);
    void layoutOpl3Page(Opl3GuiSet& gui, juce::Rectangle<int> content);
    void layoutOpmPage(OpmGuiSet& gui, juce::Rectangle<int> content);
    void layoutSsgPage(SsgGuiSet& gui, juce::Rectangle<int> content);
    void layoutWtPage(WtGuiSet& gui);
    void layoutRhythmPage(RhythmGuiSet& gui, juce::Rectangle<int> content);
    void layoutAdpcmPage(AdpcmGuiSet& gui, juce::Rectangle<int> content);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686VEditor)
};
