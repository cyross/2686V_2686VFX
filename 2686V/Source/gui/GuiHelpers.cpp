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

void layoutRowOneSide(
    juce::Rectangle<int>& rect,
    int height,
    int paddingTop,
    int paddingBottom,
    juce::Component *comp,
    int width,
    int paddingRight
) 
{
    auto area = rect.removeFromTop(height);

    rect.removeFromTop(paddingTop);

    comp->setBounds(area.removeFromLeft(width));

    area.removeFromLeft(paddingRight);

    rect.removeFromTop(paddingBottom);
}

void layoutRowBoth(
    juce::Rectangle<int>& rect,
    int height,
    int paddingTop,
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

    rect.removeFromTop(paddingTop);

    label->setBounds(area.removeFromLeft(labelWidth));

    area.removeFromLeft(labelPaddingRight);

    comp->setBounds(area.removeFromLeft(compWidth));

    area.removeFromLeft(compPaddingRight);

    rect.removeFromTop(paddingBottom);
}

void layoutRow(
    juce::Rectangle<int>& rect,
    GuiLabel *label,
    juce::Component *component,
    int rowHeight,
    int paddingTop,
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
        int newLabelWidth = labelWidth == -1 ? GuiValue::MainGroup::Comps::One::width : labelWidth;
        int newLabelPaddingRight = labelPaddingRight == -1 ? 0 : labelPaddingRight;

        layoutRowOneSide(rect, rowHeight, paddingTop, paddingBottom, label, newLabelWidth, newLabelPaddingRight);
    }
    // コンポーネントのみ配置
    else if (label == nullptr)
    {
        int newCompWidth = compWidth == -1 ? GuiValue::MainGroup::Comps::One::width : compWidth;
        int newCompPaddingRight = compPaddingRight == -1 ? 0 : compPaddingRight;

        layoutRowOneSide(rect, rowHeight, paddingTop, paddingBottom, component, newCompWidth, newCompPaddingRight);
    }
    // 両方配置
    else
    {
        int newLabelWidth = labelWidth == -1 ? GuiValue::MainGroup::Label::width : labelWidth;
        int newLabelPaddingRight = labelPaddingRight == -1 ? GuiValue::MainGroup::Row::Padding::right : labelPaddingRight;
        int newCompWidth = compWidth == -1 ? GuiValue::MainGroup::Value::width : compWidth;

        layoutRowBoth(rect, rowHeight, paddingTop, paddingBottom, label, newLabelWidth, newLabelPaddingRight, component, newCompWidth, compPaddingRight);
    }

}

void layoutMain(const MainConfig& c)
{
    layoutRow(c.mainRect, c.label, c.component, c.rowHeight, c.paddingTop, c.paddingBottom, c.labelWidth, c.labelPaddingRight, c.compWidth, c.compPaddingRight);
}

void layoutRow(const RowConfig& c)
{
    layoutRow(c.rowRect, c.label, c.component, c.rowHeight, c.paddingTop, c.paddingBottom, c.labelWidth, c.labelPaddingRight, c.compWidth, c.compPaddingRight);
}

void layoutMainCategory(const MainConfigCategory& c)
{
    layoutRow(c.mainRect, c.label, c.component, c.rowHeight, c.paddingTop, c.paddingBottom, c.labelWidth, c.labelPaddingRight, c.compWidth, c.compPaddingRight);
}

void layoutRowCategory(const RowConfigCategory& c)
{
    layoutRow(c.rowRect, c.label, c.component, c.rowHeight, c.paddingTop, c.paddingBottom, c.labelWidth, c.labelPaddingRight, c.compWidth, c.compPaddingRight);
}

void layoutRowOpzx3Pcm(const RowConfigOpzx3Pcm& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.loadPcmBtn->setBounds(area.removeFromLeft(c.loadPcmBtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.pcmFileNameLabel->setBounds(area.removeFromLeft(c.pcmFileNameLabelWidth));

    area.removeFromLeft(c.paddingRight);

    c.clearPcmBtn->setBounds(area.removeFromLeft(c.clearPcmBtnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowWtWaveValueUpdate(const RowConfigWtWaveValueUpdate& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.resetTo0Btn->setBounds(area.removeFromLeft(c.resetTo0BtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.resetTo1Btn->setBounds(area.removeFromLeft(c.resetTo1Width));

    area.removeFromLeft(c.paddingRight);

    c.resetToM1Btn->setBounds(area.removeFromLeft(c.resetToM1Width));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowRhythmPadPcmFile(const RowConfigRhythmPadPcmFile& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.loadBtn->setBounds(area.removeFromLeft(c.loadBtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.filenameLabel->setBounds(area.removeFromLeft(c.filenameLabelWidth));

    area.removeFromLeft(c.paddingRight);

    c.clearBtn->setBounds(area.removeFromLeft(c.clearBtnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowSettingsIo(const RowConfigSettingsIo& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.loadSettingsBtn->setBounds(area.removeFromLeft(c.loadSettingsBtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.saveSettingsBtn->setBounds(area.removeFromLeft(c.saveSettingsBtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.saveStartupSettingsBtn->setBounds(area.removeFromLeft(c.saveStartupSettingsBtnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowOneComp(const RowConfigOneComp& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.comp->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowTwoComps(const RowConfigTwoComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.comp1->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp2->setBounds(area.removeFromLeft(c.compWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowThreeComps(const RowConfigThreeComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.comp1->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp2->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp3->setBounds(area.removeFromLeft(c.compWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowFourComps(const RowConfigFourComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.comp1->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp2->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp3->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp4->setBounds(area.removeFromLeft(c.compWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowFiveComps(const RowConfigFiveComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.comp1->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp2->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp3->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp4->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp5->setBounds(area.removeFromLeft(c.compWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに1つのコンポーネントを貼り付け
void layoutMainOneComp(const MainConfigOneComp& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.paddingLeft);

    c.comp->setBounds(area.removeFromLeft(c.compWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに2つのコンポーネントを均等に貼り付け
void layoutMainTwoComps(const MainConfigTwoComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.paddingLeft);

    c.comp1->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp2->setBounds(area.removeFromLeft(c.compWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに3つのコンポーネントを均等に貼り付け
void layoutMainThreeComps(const MainConfigThreeComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.paddingLeft);

    c.comp1->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp2->setBounds(area.removeFromLeft(c.compWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp3->setBounds(area.removeFromLeft(c.compWidth));

    c.rect.removeFromTop(c.paddingBottom);
}
