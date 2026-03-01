#include "GuiSsg.h"
#include "../processor/PluginProcessor.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"

void GuiSsg::setup()
{
    const juce::String code = PrKey::Prefix::ssg;

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::bit, .title = GuiText::Group::Fm::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::rate, .title = GuiText::Group::Fm::alg, .items = rateItems, .isReset = true });

	voiceGroup.setup(*this, "Voice");
	waveSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::wveform, .title = "Form", .items = ssgWsItems, .isReset = true, .isResized = true });
	levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::tone, .title = "Tone", .isReset = true, .regType = RegisterType::SsgVol });
    noiseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::noise, .title = "Noise", .isReset = true, .regType = RegisterType::SsgVol });
	noiseFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::noiseFreq, .title = "Freq", .isReset = true });
	noiseOnNoteButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::noiseOnNote, .title = "Noise On Note", .isReset = true });

    // 初期状態反映
    mixSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::mix , .title = "Mix", .isReset = true });
	mixSetTone.setup({ .parent = *this, .title = "Tone", .isReset = false, .isResized = false });
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };
    mixSetMix.setup({ .parent = *this, .title = "Mix", .isReset = false, .isResized = false });
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };
    mixSetNoise.setup({ .parent = *this, .title = "Noise", .isReset = false, .isResized = false });
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

	adsrBypassButton.setup({ .parent = *this, .id = code + PrKey::Innder::adsr + PrKey::Post::bypass, .title = "Bypass ADSR", .isReset = true });

    attackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::ar, .title = GuiText::Group::Ssg::Post::Adsr::ar, .isReset = true });
    decaySlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::dr , .title = GuiText::Group::Ssg::Post::Adsr::dr, .isReset = true });
    sustainSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::sl, .title = GuiText::Group::Ssg::Post::Adsr::sl, .isReset = true });
    releaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::rr, .title = GuiText::Group::Ssg::Post::Adsr::rr, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    // Duty Controls Setup
	dutyGroup.setup(*this, "Pulse Width (Duty)");
	dutyModeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::mode, .title = "Mode", .items = ssgDmItems, .isReset = true, .isResized = true });
	dutyPresetSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::preset, .title = "Preset", .items = ssgPrItems, .isReset = true, .isResized = true });
	dutyVarSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::var, .title = "Ratio", .isReset = true });
	dutyInvertButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::inv, .title = "Invert Phase", .isReset = true, .isResized = true });

	triGroup.setup(*this, "Triangle Property");
	triKeyTrackButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Tri::keyTrk, .title = "Key Track (Pitch)", .isReset = true, .isResized = true });
	triFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Tri::freq, .title = "Freq", .isReset = true });
	triPeakSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Tri::peak, .title = "Peak", .isReset = true });
	triSetSawDown.setup({ .parent = *this, .title = "0.0 (Down)", .isReset = false, .isResized = false });
    triSetSawDown.onClick = [this] { triPeakSlider.setValue(0.0, juce::sendNotification); };
	triSetTri.setup({ .parent = *this, .title = "0.5 (Tri)", .isReset = false, .isResized = false });
    triSetTri.onClick = [this] { triPeakSlider.setValue(0.5, juce::sendNotification); };
	triSetSawUp.setup({ .parent = *this, .title = "1.0 (Up)", .isReset = false, .isResized = false });
    triSetSawUp.onClick = [this] { triPeakSlider.setValue(1.0, juce::sendNotification); };

    // HW Env Group
	envGroup.setup(*this, "Hardware Envelope");
	envEnableButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::HwEnv::enable, .title = "Enable HW Env", .isReset = true });
	shapeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::HwEnv::shape, .title = "Shape", .items = ssgEnvItems, .isReset = true });
	periodSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::HwEnv::period, .title = "Period", .isReset = true, .regType = RegisterType::SsgEnv });
}

void GuiSsg::layout(juce::Rectangle<int> content)
{
    juce::String code = PrKey::Prefix::ssg;
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &bitSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &bitSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &rateSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &rateSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &adsrBypassButton, { GuiValue::MainGroup::Button::width, GuiValue::MainGroup::Row::Padding::right} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &attackSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &attackSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &decaySlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &decaySlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &sustainSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &sustainSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &releaseSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &releaseSlider, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::MainVol::height, 0, { { &masterVolSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &masterVolSlider, { GuiValue::MainGroup::Value::width, 0} } });

    // --- Voice Group ---
    auto voiceArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

    voiceGroup.setBounds(voiceArea);
    auto vRect = voiceGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    vRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(vRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &levelSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &levelSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(vRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &noiseSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &noiseSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(vRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &noiseFreqSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &noiseFreqSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(vRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &noiseOnNoteButton, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(vRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &mixSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &mixSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(vRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, {
        { &mixSetTone, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} },
        { &mixSetMix, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} },
        { &mixSetNoise, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} }
        });
    layoutComponentsLtoR(vRect, GuiValue::Fm::Op::Row::height, 0, { { &waveSelector.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &waveSelector, { GuiValue::Fm::Op::Row::Value::width, 0} } });

    // Wave Group
    float waveParam = *ctx.audioProcessor.apvts.getRawParameterValue(code + PrKey::Post::Ssg::wveform);
    int waveMode = (waveParam > PrValue::boolThread) ? 1 : 0;
    auto waveArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

    if (waveMode == 0) // Pulse
    {
        dutyGroup.setVisible(true);
        triGroup.setVisible(false);

        triKeyTrackButton.setVisible(false);
        triPeakSlider.setVisible(false);
        triPeakSlider.label.setVisible(false);
        triFreqSlider.setVisible(false);
        triFreqSlider.label.setVisible(false);
        triSetSawDown.setVisible(false);
        triSetTri.setVisible(false);
        triSetSawUp.setVisible(false);

        dutyModeSelector.setVisible(true);
        dutyModeSelector.label.setVisible(true);
        dutyVarSlider.setValue(true);
        dutyVarSlider.label.setVisible(true);
        dutyInvertButton.setVisible(true);

        dutyGroup.setBounds(waveArea);
        auto dRect = dutyGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

        dRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutComponentsLtoR(dRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &dutyModeSelector.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &dutyModeSelector, { GuiValue::Fm::Op::Row::Value::width, 0} } });
        layoutComponentsLtoR(dRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &dutyInvertButton, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });

        float dutyModeVal = *ctx.audioProcessor.apvts.getRawParameterValue(code + PrKey::Post::Ssg::Duty::mode);
        if (dutyModeVal < 0.5f) {
            dutyPresetSelector.setVisible(true);
            dutyPresetSelector.label.setVisible(true);
            dutyVarSlider.setVisible(false);
            dutyVarSlider.label.setVisible(false);
            layoutComponentsLtoR(dRect, GuiValue::Fm::Op::Row::height, 0, { { &dutyPresetSelector.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &dutyPresetSelector, { GuiValue::Fm::Op::Row::Value::width, 0} } });
        }
        else {
            dutyPresetSelector.setVisible(false);
            dutyPresetSelector.label.setVisible(false);
            dutyVarSlider.setVisible(true);
            dutyVarSlider.label.setVisible(true);
            layoutComponentsLtoR(dRect, GuiValue::Fm::Op::Row::height, 0, { { &dutyVarSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &dutyVarSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
        }
    }
    else // Triangle
    {
        dutyGroup.setVisible(false);
        triGroup.setVisible(true);

        dutyModeSelector.setVisible(false);
        dutyModeSelector.label.setVisible(false);
        dutyInvertButton.setVisible(false);
        dutyPresetSelector.setVisible(false);
        dutyPresetSelector.label.setVisible(false);
        dutyVarSlider.setVisible(false);
        dutyVarSlider.label.setVisible(false);

        triKeyTrackButton.setVisible(true);
        triFreqSlider.setVisible(true);
        triFreqSlider.label.setVisible(true);
        triPeakSlider.setVisible(true);
        triPeakSlider.label.setVisible(true);
        triSetSawDown.setVisible(true);
        triSetTri.setVisible(true);
        triSetSawUp.setVisible(true);

        triGroup.setBounds(waveArea);
        auto tRect = triGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

        tRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutComponentsLtoR(tRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &triKeyTrackButton, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });

        bool isKeyTrack = triKeyTrackButton.getToggleState();
        triFreqSlider.setVisible(!isKeyTrack);
        triFreqSlider.label.setVisible(!isKeyTrack);

        if (!isKeyTrack) {
            layoutComponentsLtoR(tRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &triFreqSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &triFreqSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
            layoutComponentsLtoR(tRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &triPeakSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &triPeakSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
        }
        else {
            layoutComponentsLtoR(tRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &triPeakSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &triPeakSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
        }

        layoutComponentsLtoR(tRect, GuiValue::Fm::Op::Row::height, 0, {
            { &triSetSawDown, { GuiValue::Fm::Op::Row::Button::TriPeak::width, 0} },
            { &triSetTri, { GuiValue::Fm::Op::Row::Button::TriPeak::width, 0} },
            { &triSetSawUp, { GuiValue::Fm::Op::Row::Button::TriPeak::width, 0} }
            });
    }

    // HW Env Group
    auto envArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);
    envGroup.setBounds(envArea);
    auto eRect = envGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    eRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(eRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &envEnableButton, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(eRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &shapeSelector.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &shapeSelector, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(eRect, GuiValue::Fm::Op::Row::height, 0, { { &periodSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &periodSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
}
