#include "GuiHelpers.h"

#include "../core/MmlKeys.h"

#include "../fm/RegisterType.h"
#include "../fm/RegisterConverter.h"

#include "../gui/GuiValues.h"

std::vector<SelectItem> createItems(int size, const juce::String& prefix)
{
    std::vector<SelectItem> items;

    for (int i = 0; i < size; ++i) items.push_back({ .name = prefix + juce::String(i), .value = i + 1 });

    return items;
}

std::vector<SelectItem> createAlgItems(int size)
{
    return createItems(size, "Alg ");
}

// Helper function to get Note Name (Cubase Standard: C3 = 60)
juce::String getNoteName(int noteNumber)
{
    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = (noteNumber / 12) - 2; // MIDI 0 = C-2, MIDI 36 = C1, MIDI 60 = C3
    int noteIndex = noteNumber % 12;
    return juce::String(noteNames[noteIndex]) + juce::String(octave);
}

void layoutComponentsOneSide(
    juce::Rectangle<int>& rect,
    int height,
    int paddingBottom,
    juce::Component *comp,
    int width,
    int paddingRight
) 
{
    auto area = rect.removeFromTop(height);

    comp->setBounds(area.removeFromLeft(width));

    area.removeFromLeft(paddingRight);

    rect.removeFromTop(paddingBottom);
}

void layoutComponentsBoth(
    juce::Rectangle<int>& rect,
    int height,
    int paddingBottom,
    GuiLabel *label,
    int labelWidth,
    int labelPaddingRight,
    juce::Component *comp,
    int compWidth,
    int compPaddingRight
)
{
    auto area = rect.removeFromTop(height);

    label->setBounds(area.removeFromLeft(labelWidth));

    area.removeFromLeft(labelPaddingRight);

    comp->setBounds(area.removeFromLeft(compWidth));

    area.removeFromLeft(compPaddingRight);

    rect.removeFromTop(paddingBottom);
}

void layoutComponentsLtoR(
    juce::Rectangle<int>& rect,
    GuiLabel *label,
    juce::Component *component,
    int rowHeight,
    int paddingBottom,
    int labelWidth,
    int labelPaddingRight,
    int compWidth,
    int compPaddingRight
)
{
    if (label == nullptr && component == nullptr)
    {
        return;
    }

    // ラベルのみ配置
    if (component == nullptr)
    {
        int newLabelWidth = labelWidth == -1 ? GuiValue::MainGroup::LabelOnly::width : labelWidth;
        int newLabelPaddingRight = labelPaddingRight == -1 ? 0 : labelPaddingRight;

        layoutComponentsOneSide(rect, rowHeight, paddingBottom, label, newLabelWidth, newLabelPaddingRight);
    }
    // コンポーネントのみ配置
    else if (label == nullptr)
    {
        int newCompWidth = compWidth == -1 ? GuiValue::MainGroup::Button::width : compWidth;
        int newCompPaddingRight = compPaddingRight == -1 ? 0 : compPaddingRight;

        layoutComponentsOneSide(rect, rowHeight, paddingBottom, component, newCompWidth, newCompPaddingRight);
    }
    // 両方配置
    else
    {
        int newLabelWidth = labelWidth == -1 ? GuiValue::MainGroup::Label::width : labelWidth;
        int newLabelPaddingRight = labelPaddingRight == -1 ? GuiValue::MainGroup::Row::Padding::right : labelPaddingRight;
        int newCompWidth = compWidth == -1 ? GuiValue::MainGroup::Value::width : compWidth;

        layoutComponentsBoth(rect, rowHeight, paddingBottom, label, newLabelWidth, newLabelPaddingRight, component, newCompWidth, compPaddingRight);
    }

}

void layoutComponentsLtoRMain(const LtoRConfigMain& c)
{
    layoutComponentsLtoR(c.mainRect, c.label, c.component, c.rowHeight, c.paddingBottom, c.labelWidth, c.labelPaddingRight, c.compWidth, c.compPaddingRight);
}

void layoutComponentsLtoRRow(const LtoRConfigRow& c)
{
    layoutComponentsLtoR(c.rowRect, c.label, c.component, c.rowHeight, c.paddingBottom, c.labelWidth, c.labelPaddingRight, c.compWidth, c.compPaddingRight);
}

void layoutComponentsLtoRFixFreqBtns(const LtoRConfigFixFreqBtns& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.to0Btn->setBounds(area.removeFromLeft(c.buttonWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.to440Btn->setBounds(area.removeFromLeft(c.buttonWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutComponentsLtoROpzx3FixFreqBtns(const LtoRConfigOpzx3FixFreqBtns& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.to0Btn->setBounds(area.removeFromLeft(c.buttonWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.to05Btn->setBounds(area.removeFromLeft(c.buttonWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.to1Btn->setBounds(area.removeFromLeft(c.buttonWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.to2Btn->setBounds(area.removeFromLeft(c.buttonWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.to440Btn->setBounds(area.removeFromLeft(c.buttonWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutComponentsLtoROpzx3PcmRow(const LtoRConfigOpzx3PcmRow& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.loadPcmBtn->setBounds(area.removeFromLeft(c.loadPcmBtnWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.pcmFileNameLabel->setBounds(area.removeFromLeft(c.pcmFileNameLabelWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.clearPcmBtn->setBounds(area.removeFromLeft(c.clearPcmBtnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutComponentsLtoRSsgMixRow(const LtoRConfigSsgMixRow& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.toneBtn->setBounds(area.removeFromLeft(c.btnWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.mixBtn->setBounds(area.removeFromLeft(c.btnWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.noizeBtn->setBounds(area.removeFromLeft(c.btnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutComponentsLtoRSsgTriPosRow(const LtoRConfigSsgTriPosRow& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.sawDownBtn->setBounds(area.removeFromLeft(c.btnWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.triBtn->setBounds(area.removeFromLeft(c.btnWidth));

    area.removeFromLeft(GuiValue::Fm::Op::Row::Padding::right);

    c.sawUpBtn->setBounds(area.removeFromLeft(c.btnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutComponentsLtoRWtWaveValueUpdateRow(const LtoRConfigWtWaveValueUpdateRow& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.resetTo0Btn->setBounds(area.removeFromLeft(GuiValue::Wt::Custom::ResetBtn::To0Width));

    area.removeFromLeft(GuiValue::Wt::Custom::ResetBtn::Padding::Right);

    c.resetTo1Btn->setBounds(area.removeFromLeft(GuiValue::Wt::Custom::ResetBtn::To1Width));

    area.removeFromLeft(GuiValue::Wt::Custom::ResetBtn::Padding::Right);

    c.resetToM1Btn->setBounds(area.removeFromLeft(GuiValue::Wt::Custom::ResetBtn::ToM1Width));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutComponentsLtoRRhythmPadPcmFileRow(const LtoRConfigRhythmPadPcmFileRow& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.loadBtn->setBounds(area.removeFromLeft(50));

    area.removeFromLeft(0);

    c.filenameLabel->setBounds(area.removeFromLeft(150));

    area.removeFromLeft(0);

    c.clearBtn->setBounds(area.removeFromLeft(30));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutComponentsLtoRRhythmPadPanRow(const LtoRConfigRhythmPadPanRow& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.lBtn->setBounds(area.removeFromLeft(GuiValue::Fm::Op::Row::Button::Pan::width));

    area.removeFromLeft(0);

    c.cBtn->setBounds(area.removeFromLeft(GuiValue::Fm::Op::Row::Button::Pan::width));

    area.removeFromLeft(0);

    c.rBtn->setBounds(area.removeFromLeft(GuiValue::Fm::Op::Row::Button::Pan::width));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutComponentsLtoRAdpcmPanRow(const LtoRConfigAdpcmPanRow& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.lBtn->setBounds(area.removeFromLeft(c.btnWidth));

    area.removeFromLeft(GuiValue::MainGroup::Button::Pan::paddingHeight);

    c.cBtn->setBounds(area.removeFromLeft(c.btnWidth));

    area.removeFromLeft(GuiValue::MainGroup::Button::Pan::paddingHeight);

    c.rBtn->setBounds(area.removeFromLeft(c.btnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutComponentsLtoRFxMixRow(const LtoRConfigFxMixRow& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.dryBtn->setBounds(area.removeFromLeft(c.btnWidth));

    area.removeFromLeft(GuiValue::MainGroup::Button::Pan::paddingHeight);

    c.HalfBtn->setBounds(area.removeFromLeft(c.btnWidth));

    area.removeFromLeft(GuiValue::MainGroup::Button::Pan::paddingHeight);

    c.wetBtn->setBounds(area.removeFromLeft(c.btnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutComponentsLtoRSettingsIoRow(const LtoRConfigSettingsIoRow& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.loadSettingsBtn->setBounds(area.removeFromLeft(c.btnWidth));

    area.removeFromLeft(GuiValue::MainGroup::Button::Pan::paddingHeight);

    c.saveSettingsBtn->setBounds(area.removeFromLeft(c.btnWidth));

    area.removeFromLeft(GuiValue::MainGroup::Button::Pan::paddingHeight);

    c.saveStartupSettingsBtn->setBounds(area.removeFromLeft(c.btnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}
