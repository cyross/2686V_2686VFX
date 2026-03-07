#include "GuiComponents.h"

#include "../editor/PluginEditor.h"

#include "../gui/GuiText.h"

void ColoredGroupComponent::setBackgroundColor(juce::Colour c)
{
    backgroundColor = c;
    repaint();
}

void ColoredGroupComponent::paint(juce::Graphics& g)
{
    // 背景色があれば描画 (角丸で少し柔らかく)
    if (!backgroundColor.isTransparent())
    {
        g.setColour(backgroundColor);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 4.0f);
    }

    // 親クラス（枠線とテキスト）の描画処理を呼ぶ
    juce::GroupComponent::paint(g);
}

void GuiGroup::setup(juce::Component& parent, const juce::String title)
{
    parent.addAndMakeVisible(*this);

    this->setText(title);

    this->setColour(juce::GroupComponent::textColourId, textColor);
    this->setColour(juce::GroupComponent::outlineColourId, borderColor);

    // 枠線が他のコンポーネントを削らないように最背面へ
    this->toBack();
}

void GuiLabel::setup(const Config& c)
{
    c.parent.addAndMakeVisible(*this);

    this->setText(c.title, juce::NotificationType::sendNotification);
    this->setJustificationType(c.justification);

    if (!c.color.isTransparent())
    {
        this->setColour(juce::Label::textColourId, c.color);
    }
}

void GuiSlider::setup(const Config& c)
{
    label.setup({ .parent = c.parent, .title = c.title, .color = c.labelColor });

    c.parent.addAndMakeVisible(*this);
    this->setSliderStyle(juce::Slider::LinearHorizontal);
    this->setTextBoxStyle(juce::Slider::TextBoxRight, false, 42, 20);

    if (!c.trackColor.isTransparent())
    {
        this->setColour(juce::Slider::trackColourId, c.trackColor);
    }

    if (!c.thumbColor.isTransparent())
    {
        this->setColour(juce::Slider::thumbColourId, c.thumbColor);
    }

    if (!c.valueBorderColor.isTransparent())
    {
        this->setColour(juce::Slider::textBoxOutlineColourId, c.valueBorderColor);
    }

    if (!c.valueTextColor.isTransparent())
    {
        this->setColour(juce::Slider::textBoxTextColourId, c.valueTextColor);
    }

    if (!c.valueHighlightColor.isTransparent())
    {
        this->setColour(juce::Slider::textBoxHighlightColourId, c.valueHighlightColor);
    }

    if (c.isReset)
    {
        att.reset(new SliderAttachment(ctx.apvts, c.id, *this));
    }

    if (c.regType != RegisterType::None)
    {
        ctx.sliderRegMap[this] = c.regType;

        this->addMouseListener(&ctx.editor, false);
    }
}

void GuiComboBox::setup(const Config& c)
{
    label.setup({ .parent = c.parent, .title = c.title, .color = c.labelColor });

    c.parent.addAndMakeVisible(*this);

    if (!c.color.isTransparent())
    {
        this->setColour(juce::ComboBox::textColourId, c.color);
    }

    if (!c.bgColor.isTransparent())
    {
        this->setColour(juce::ComboBox::backgroundColourId, c.bgColor);
    }

    for (SelectItem& item : c.items)
    {
        this->addItem(item.name, item.value);
    }

    if (c.isReset)
    {
        att.reset(new ComboBoxAttachment(ctx.apvts, c.id, *this));
    }

    if (c.isResized)
    {
        this->onChange = [this] { ctx.editor.resized(); };
    }

    if (c.regType != RegisterType::None)
    {
        ctx.sliderRegMap[this] = c.regType;

        this->addMouseListener(&ctx.editor, false);
    }
}

void GuiToggleButton::setup(const Config& c)
{
    c.parent.addAndMakeVisible(*this);

    this->setButtonText(c.title);

    if (!c.color.isTransparent())
    {
        this->setColour(juce::Label::textColourId, c.color);
    }

    if (c.isReset) {
        att.reset(new ButtonAttachment(ctx.apvts, c.id, *this));
    }

    if (c.isResized) {
        this->onClick = [this] { ctx.editor.resized(); };
    }
}

void GuiTextButton::setup(const Config& c)
{
    c.parent.addAndMakeVisible(*this);

    this->setButtonText(c.title);

    if (!c.textColor.isTransparent())
    {
        this->setColour(juce::Label::textColourId, c.textColor);
    }

    if (!c.bgColor.isTransparent())
    {
        this->setColour(juce::TextButton::buttonColourId, c.bgColor);
    }

    if (c.isReset) {
        att.reset(new ButtonAttachment(ctx.apvts, c.id, *this));
    }

    if (c.isResized) {
        this->onClick = [this] { ctx.editor.resized(); };
    }
}

void GuiHyperLink::setup(const Config& c)
{
    c.parent.addAndMakeVisible(*this);

    this->setButtonText(c.title);

    if (c.font.has_value())
    {
        this->setFont(c.font.value(), false);
    }

    this->setJustificationType(c.justification);

    this->setURL(c.url);

    if (!c.color.isTransparent())
    {
        this->setColour(juce::Label::textColourId, c.color);
    }

    if (c.isReset) {
        att.reset(new ButtonAttachment(ctx.apvts, c.id, *this));
    }

    if (c.isResized) {
        this->onClick = [this] { ctx.editor.resized(); };
    }
}

void GuiImage::setup(juce::Component& parent)
{
    parent.addAndMakeVisible(*this);
}

void GuiTableList::setup(const Config& c)
{
    c.parent.addAndMakeVisible(*this);

	this->setMultipleSelectionEnabled(c.canMultipleSelection);

    if (!c.color.isTransparent())
    {
        this->setColour(juce::Label::textColourId, c.color);
    }

    selectedBgColor = c.selectedBgColor;
    bgColor1 = c.bgColor1;
    bgColor2 = c.bgColor2;
}

void GuiTableList::addColumn(const juce::String& columnName, int columnId, int width)
{
	this->getHeader().addColumn(columnName, columnId, width);
}

// 行数を返す
int GuiTableList::getNumRows()
{
    // ラムダ式がセットされていればそれを呼び出し、なければ0を返す
    return onGetNumRows ? onGetNumRows() : 0;
}

// 行の背景を描画する
void GuiTableList::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
    if (rowIsSelected) {
        g.fillAll(selectedBgColor);
    }
    else if (rowNumber % 2) {
        g.fillAll(bgColor1); // ストライプ
    }
    else {
        g.fillAll(bgColor2); // ストライプ
    }
}

void GuiTableList::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
    // 文字色
    g.setColour(rowIsSelected ? juce::Colours::white : juce::Colours::lightgrey);

    // ラムダ式からテキストを取得
    juce::String text = onGetCellText ? onGetCellText(rowNumber, columnId) : "";
    g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
}

void GuiTableList::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& e) {
    if (onDoubleClicked) onDoubleClicked(rowNumber);
}

void GuiTableList::selectedRowsChanged(int lastRowSelected) {
    if (onSelectionChanged) onSelectionChanged(lastRowSelected);
}

void GuiTextEditor::setup(const Config& c)
{
    label.setup({ .parent = c.parent, .title = c.title, .color = c.labelColor });

    c.parent.addAndMakeVisible(*this);

	this->setMultiLine(c.isMultiLine);

    if (!c.color.isTransparent())
    {
        this->setColour(juce::TextEditor::textColourId, c.color);
    }

    if (!c.bgColor.isTransparent())
    {
        this->setColour(juce::TextEditor::backgroundColourId, c.bgColor);
    }

    if (!c.borderColor.isTransparent())
    {
        this->setColour(juce::TextEditor::outlineColourId, c.borderColor);
    }
}

void GuiFbSlider::setup(const GuiSlider::Config& c)
{
    GuiSlider::setup(c);
    GuiSlider::setRange(0.0, 7.0, 1.0);
}

void GuiMasterVolumeSlider::setup(const GuiSlider::Config& c)
{
    GuiSlider::setup(c);
    GuiSlider::setTextValueSuffix(GuiText::MasterVol::unit); // 単位表示
}

void GuiMmlButton::setupMml(const MmlConfig& c)
{
    // ボタンがクリックされた時の処理をここに閉じ込める
    this->onClick = [c] {
        // オペレーター番号は 0始まりを想定しているので +1 して表示
        auto* w = new juce::AlertWindow(
            "MML Input (Op " + juce::String(c.opIndex + 1) + ")",
            c.hintMessage, // ← 外から渡された音源ごとのメッセージ
            juce::AlertWindow::QuestionIcon);

        w->addTextEditor("mmlInput", "", "");
        w->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
        w->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

        // モーダル表示 (ラムダ式には設定値 c と ウィンドウ w をコピーキャプチャする)
        w->enterModalState(true, juce::ModalCallbackFunction::create([c, w](int result) {
            if (result == 1) {
                juce::String mmlText = w->getTextEditorContents("mmlInput");

                // 親から渡されたコールバック関数を実行し、入力された文字列を返す
                if (c.onMmlApplied) {
                    c.onMmlApplied(mmlText);
                }
            }
        }), true); // deleteWhenDismissed = true になっているので w は自動で破棄されます

        if (auto* editor = w->getTextEditor("mmlInput"))
        {
            editor->grabKeyboardFocus();
        }
    };
}

void GuiCategoryLabel::setup(const Config& c)
{
    GuiLabel::setup({ .parent = c.parent, .title = c.title, .font = c.font, .justification = c.justification, .color = c.color });
}