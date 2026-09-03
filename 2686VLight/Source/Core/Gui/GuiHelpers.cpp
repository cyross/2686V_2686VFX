#include "./GuiHelpers.h"

#include "../Const/ConstMmlKeys.h"

#include "../../Core/Fm/FmRegisterType.h"
#include "../../Core/Fm/FmRegisterConverter.h"

#include "./GuiValues.h"

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
        int newLabelWidth = labelWidth == -1 ? rect.getWidth() : labelWidth;
        int newLabelPaddingRight = labelPaddingRight == -1 ? 0 : labelPaddingRight;

        layoutRowOneSide(rect, rowHeight, paddingTop, paddingBottom, label, newLabelWidth, newLabelPaddingRight);
    }
    // コンポーネントのみ配置
    else if (label == nullptr)
    {
        int newCompWidth = compWidth == -1 ? rect.getWidth() : compWidth;
        int newCompPaddingRight = compPaddingRight == -1 ? 0 : compPaddingRight;

        layoutRowOneSide(rect, rowHeight, paddingTop, paddingBottom, component, newCompWidth, newCompPaddingRight);
    }
    // 両方配置
    else
    {
        int newLabelWidth = labelWidth == -1 ? CoreGuiValue::MainGroup::Label::width : labelWidth;
        int newLabelPaddingRight = labelPaddingRight == -1 ? CoreGuiValue::MainGroup::Row::Padding::right : labelPaddingRight;
        int newCompWidth = compWidth == -1 ? CoreGuiValue::MainGroup::Value::width : compWidth;

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
    // 見出しは label と component のどちらで渡ってくることもある
    auto* asComponent = (c.label != nullptr) ? static_cast<juce::Component*>(c.label) : c.component;
    auto* category = dynamic_cast<GuiCategoryLabel*>(asComponent);

    // 隠している見出しは、場所も取らずに何も出さない。
    // 閉じ待ちはそのまま次の見出しへ引き継ぐ。
    if (category != nullptr && category->isHidden()) {
        category->setVisible(false);
        category->hideBackdrop();

        return;
    }

    const int pad = GuiCategoryLabel::contentPadding;

    if (category != nullptr) {
        // 1 つ前に開いたカテゴリの板を、この見出しの手前で閉じる。
        // 閉じたときだけ、その中身のために内側へ寄せていた幅を元に戻す。
        if (GuiCategoryLabel::closePending(category->getParentComponent(), c.mainRect.getY())) {
            c.mainRect.setX(c.mainRect.getX() - pad);
            c.mainRect.setWidth(c.mainRect.getWidth() + pad * 2);
        }
    }

    layoutRow(c.mainRect, c.label, c.component, c.rowHeight, c.paddingTop, c.paddingBottom, c.labelWidth, c.labelPaddingRight, c.compWidth, c.compPaddingRight);

    if (category != nullptr) {
        // 畳んでいるときもここを通す。beginBackdrop の中で板を隠すため、
        // 通さないと前回開いていたときの板が出たままになる。
        category->beginBackdrop(c.mainRect);

        if (category->isOpen()) {
            // 中身は板の内側へ寄せる。次の見出しの手前で元に戻す。
            c.mainRect.setX(c.mainRect.getX() + pad);
            c.mainRect.setWidth(c.mainRect.getWidth() - pad * 2);
            c.mainRect.removeFromTop(pad);
        }
    }
}

void layoutMainParamName(const MainConfigParamName& c)
{
    layoutRow(c.mainRect, c.label, c.component, c.rowHeight, c.paddingTop, c.paddingBottom, c.labelWidth, c.labelPaddingRight, c.compWidth, c.compPaddingRight);
}

void layoutRowCategory(const RowConfigCategory& c)
{
    // 見出しは label と component のどちらで渡ってくることもある
    auto* asComponent = (c.label != nullptr) ? static_cast<juce::Component*>(c.label) : c.component;
    auto* category = dynamic_cast<GuiCategoryLabel*>(asComponent);

    // 隠している見出しは、場所も取らずに何も出さない。
    // 閉じ待ちはそのまま次の見出しへ引き継ぐ。
    if (category != nullptr && category->isHidden()) {
        category->setVisible(false);
        category->hideBackdrop();

        return;
    }

    const int pad = GuiCategoryLabel::contentPadding;

    if (category != nullptr) {
        // 1 つ前に開いたカテゴリの板を、この見出しの手前で閉じる。
        // 閉じたときだけ、その中身のために内側へ寄せていた幅を元に戻す。
        if (GuiCategoryLabel::closePending(category->getParentComponent(), c.rowRect.getY())) {
            c.rowRect.setX(c.rowRect.getX() - pad);
            c.rowRect.setWidth(c.rowRect.getWidth() + pad * 2);
        }
    }

    layoutRow(c.rowRect, c.label, c.component, c.rowHeight, c.paddingTop, c.paddingBottom, c.labelWidth, c.labelPaddingRight, c.compWidth, c.compPaddingRight);

    if (category != nullptr) {
        // 畳んでいるときもここを通す。beginBackdrop の中で板を隠すため、
        // 通さないと前回開いていたときの板が出たままになる。
        category->beginBackdrop(c.rowRect);

        if (category->isOpen()) {
            // 中身は板の内側へ寄せる。次の見出しの手前で元に戻す。
            c.rowRect.setX(c.rowRect.getX() + pad);
            c.rowRect.setWidth(c.rowRect.getWidth() - pad * 2);
            c.rowRect.removeFromTop(pad);
        }
    }
}

void layoutRowOneComp(const RowConfigOneComp& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? area.getWidth() - c.compPaddingLeft - c.compPaddingRight : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowTwoComps(const RowConfigTwoComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner) / 2 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowThreeComps(const RowConfigThreeComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner * 2) / 3 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp3->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowFourComps(const RowConfigFourComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner * 3) / 4 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp3->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp4->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowFiveComps(const RowConfigFiveComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner * 4) / 5 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp3->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp4->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp5->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// パラメータグループに6つのコンポーネントを均等に貼り付け
void layoutRowSixComps(const RowConfigSixComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner * 5) / 6 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp3->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp4->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp5->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp6->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// パラメータグループに7つのコンポーネントを均等に貼り付け
void layoutRowSevenComps(const RowConfigSevenComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner * 6) / 7 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp3->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp4->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp5->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp6->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp7->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに1つのコンポーネントを貼り付け
void layoutMainOneComp(const MainConfigOneComp& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? area.getWidth() - c.compPaddingLeft - c.compPaddingRight : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに2つのコンポーネントを均等に貼り付け
void layoutMainTwoComps(const MainConfigTwoComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner) / 2 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに3つのコンポーネントを均等に貼り付け
void layoutMainThreeComps(const MainConfigThreeComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner * 2) / 3 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp3->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに4つのコンポーネントを均等に貼り付け
void layoutMainFourComps(const MainConfigFourComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner * 3) / 4 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp3->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp4->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに5つのコンポーネントを均等に貼り付け
void layoutMainFiveComps(const MainConfigFiveComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner * 4) / 5 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp3->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp4->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp5->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに6つのコンポーネントを均等に貼り付け
void layoutMainSixComps(const MainConfigSixComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner * 5) / 6 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp3->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp4->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp5->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp6->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに7つのコンポーネントを均等に貼り付け
void layoutMainSevenComps(const MainConfigSevenComps& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    int cWidth = c.compWidth == 0 ? (area.getWidth() - c.compPaddingLeft - c.compPaddingRight - c.compPaddingInner * 6) / 7 : c.compWidth;

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.compPaddingLeft);

    c.comp1->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp2->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp3->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp4->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp5->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp6->setBounds(area.removeFromLeft(cWidth));

    area.removeFromLeft(c.compPaddingInner);

    c.comp7->setBounds(area.removeFromLeft(cWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// メイングループに3つのコンポーネントを均等に貼り付け
void layoutMainFxOrder(const MainConfigFxOrder& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.paddingLeft);

    c.comp1->setBounds(area.removeFromLeft(c.labelWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp2->setBounds(area.removeFromLeft(c.buttonWidth));

    area.removeFromLeft(c.compPaddingRight);

    c.comp3->setBounds(area.removeFromLeft(c.buttonWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

// ビューモード選択用
void layoutMainViewMode(const MainConfigViewMode& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    area.removeFromLeft(c.paddingLeft);

    c.comp3->setBounds(area.removeFromRight(c.compWidth));

    area.removeFromRight(c.compPaddingRight);

    c.comp2->setBounds(area.removeFromRight(c.compWidth));

    area.removeFromRight(c.compPaddingRight);

    c.comp1->setBounds(area.removeFromRight(c.compWidth));

    area.removeFromRight(c.compPaddingRight);

    c.label.setBounds(area);

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutRowWtFiles(const RowConfigWtFiles& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.loadWtBtn->setBounds(area.removeFromLeft(c.loadBtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.loadWt2Btn->setBounds(area.removeFromLeft(c.loadBtnWidth));

    area.removeFromLeft(c.paddingRight);

    int fileNameLabelWidth = area.getWidth() - c.paddingRight - c.clearBtnWidth;

    c.fileNameLabel->setBounds(area.removeFromLeft(fileNameLabelWidth));

    area.removeFromLeft(c.paddingRight);

    c.clearBtn->setBounds(area.removeFromLeft(c.clearBtnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}

void layoutMainWtFiles(const MainConfigWtFiles& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.loadWtBtn->setBounds(area.removeFromLeft(c.loadBtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.loadWt2Btn->setBounds(area.removeFromLeft(c.loadBtnWidth));

    area.removeFromLeft(c.paddingRight);

    int fileNameLabelWidth = area.getWidth() - c.paddingRight - c.clearBtnWidth;

    c.fileNameLabel->setBounds(area.removeFromLeft(fileNameLabelWidth));

    area.removeFromLeft(c.paddingRight);

    c.clearBtn->setBounds(area.removeFromLeft(c.clearBtnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}
