#include "./GuiComponents.h"

#include "../Editor/PluginEditor.h"

#include "./GuiText.h"

// コンストラクタで色を受け取る
GuiWaveformPreview::GuiWaveformPreview(juce::Colour background, juce::Colour line, juce::Colour border, juce::Colour axis)
    : bgColor(background), lineColor(line), borderColor(border), axisColor(axis)
{
}

void GuiWaveformPreview::pushBuffer(const float* data, int numSamples)
{
    if (numSamples <= 0) return;
    m_displayBuffer.assign(data, data + numSamples);
    repaint(); // データが来たら再描画
}

void GuiWaveformPreview::paint(juce::Graphics& g)
{
    // カスタム背景色で塗りつぶし
    g.fillAll(bgColor);

    // 軸線とメモリを描画
    float xAxis = getWidth() / 8.0f;
    float yAxis = getHeight() / 8.0f;

    g.setColour(axisColor.withAlpha(0.8f));

    g.drawLine(0.0f, yAxis * 4.0f, xAxis * 8.0f, yAxis * 4.0f);
    g.drawLine(xAxis * 4.0f, 0.0f, xAxis * 4.0f, yAxis * 8.0f);

    g.setColour(axisColor.darker(0.1f).withAlpha(0.8f));

    g.drawLine(0.0f, yAxis * 2.0f, xAxis * 8.0f, yAxis * 2.0f);
    g.drawLine(0.0f, yAxis * 6.0f, xAxis * 8.0f, yAxis * 6.0f);
    g.drawLine(xAxis * 2.0f, 0.0f, xAxis * 2.0f, yAxis * 8.0f);
    g.drawLine(xAxis * 6.0f, 0.0f, xAxis * 6.0f, yAxis * 8.0f);

    g.setColour(axisColor.darker(0.3f).withAlpha(0.5f));

    g.drawLine(0.0f, yAxis * 1.0f, xAxis * 8.0f, yAxis * 1.0f);
    g.drawLine(0.0f, yAxis * 3.0f, xAxis * 8.0f, yAxis * 3.0f);
    g.drawLine(0.0f, yAxis * 5.0f, xAxis * 8.0f, yAxis * 5.0f);
    g.drawLine(0.0f, yAxis * 7.0f, xAxis * 8.0f, yAxis * 7.0f);

    g.drawLine(xAxis * 1.0f, 0.0f, xAxis * 1.0f, yAxis * 8.0f);
    g.drawLine(xAxis * 3.0f, 0.0f, xAxis * 3.0f, yAxis * 8.0f);
    g.drawLine(xAxis * 5.0f, 0.0f, xAxis * 5.0f, yAxis * 8.0f);
    g.drawLine(xAxis * 7.0f, 0.0f, xAxis * 7.0f, yAxis * 8.0f);

    if (m_displayBuffer.empty()) return;

    juce::Path wavePath;
    float halfHeight = getHeight() / 2.0f;

    float xStep = 0.0f;
    if (m_displayBuffer.size() > 1) {
        xStep = (float)getWidth() / (m_displayBuffer.size() - 1);
    }

    wavePath.startNewSubPath(0, halfHeight - (m_displayBuffer[0] * halfHeight));

    for (size_t i = 1; i < m_displayBuffer.size(); ++i) {
        float x = i * xStep;
        float y = halfHeight - (m_displayBuffer[i] * halfHeight);
        wavePath.lineTo(x, y);
    }

    // カスタム波形色で描画
    g.setColour(lineColor);
    g.strokePath(wavePath, juce::PathStrokeType(2.0f));

    g.setColour(borderColor);
    g.drawRect(getLocalBounds(), 1);
}

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
    this->setWantsKeyboardFocus(false);
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

    if (c.selectedFont.has_value()) {
        customLF.selectedFont = c.selectedFont.value();
    }

    if (c.dropdownFont.has_value()) {
        customLF.dropdownFont = c.dropdownFont.value();
    }

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

    this->textJustification = c.justification;

    if (c.font.has_value()) {
        this->buttonFont = c.font.value();
    }

    if (!c.color.isTransparent())
    {
        this->setColour(juce::ToggleButton::textColourId, c.color);
    }

    if (c.isReset) {
        att.reset(new ButtonAttachment(ctx.apvts, c.id, *this));
    }

    if (c.isResized) {
        this->onClick = [this] { ctx.editor.resized(); };
    }
}

void GuiToggleButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    if (!this->isVisible())
    {
		return; // 非表示なら描画しない
    }

    float boxSize = 12.0f; // 縮小しても視認しやすい四角のサイズ
    float gap = 6.0f;      // 四角と文字の隙間

    g.setFont(buttonFont);
    juce::String text = getButtonText();
    float textWidth = juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), juce::StringRef(text));
    float totalWidth = boxSize + gap + textWidth;

    float alpha = isEnabled() ? 1.0f : 0.5f;
    juce::Colour textColor = findColour(juce::ToggleButton::textColourId);
    if (textColor.isTransparent()) textColor = juce::Colours::white; // フォールバック

    juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    float startX = 0.0f;

    // Configで中央寄せ(centred)が指定されている場合、ボックスと文字のセットを中央に配置する
    if (textJustification == juce::Justification::centred) {
        startX = std::max(0.0f, (bounds.getWidth() - totalWidth) * 0.5f);
    }

    // 1. 枠線の描画
    float boxY = (bounds.getHeight() - boxSize) * 0.5f;
    juce::Rectangle<float> box(startX, boxY, boxSize, boxSize);

    g.setColour(textColor.withMultipliedAlpha(alpha));
    g.drawRect(box, 1.0f);

    // 2. ONのときの塗りつぶし (■)
    if (getToggleState()) {
        g.fillRect(box.reduced(2.0f)); // 2px内側を塗りつぶす
    }

    // 3. テキストの描画
    juce::Rectangle<float> textBounds(box.getRight() + gap, 0.0f, bounds.getWidth() - box.getRight() - gap, bounds.getHeight());
    g.drawFittedText(text, textBounds.toNearestInt(), juce::Justification::centredLeft, 1);
}

void GuiTextButton::setup(const Config& c)
{
    c.parent.addAndMakeVisible(*this);

    this->setButtonText(c.title);

    if (c.font.has_value()) {
        customLF.customFont = c.font.value();
    }

    if (!c.textColor.isTransparent())
    {
        this->setColour(juce::TextButton::textColourOffId, c.textColor); // 通常時
    }

    if (!c.textOnColor.isTransparent())
    {
        this->setColour(juce::TextButton::textColourOnId, c.textOnColor);  // 押下時
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

// セル（行）ごとのツールチップテキストを返す処理
juce::String GuiTableList::getCellTooltip(int rowNumber, int columnId)
{
    // ラムダ式が設定されていればそれを呼び出し、なければ空文字(表示しない)を返す
    if (onGetCellTooltip != nullptr) {
        return onGetCellTooltip(rowNumber, columnId);
    }
    return juce::String();
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
    this->setReturnKeyStartsNewLine(c.isReturnKeyStartsNewLine);

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
    GuiSlider::setTextValueSuffix(CoreGuiText::MasterVol::unit); // 単位表示
}

void GuiMmlButton::setupMml(const MmlConfig& c)
{
    // ボタンがクリックされた時の処理をここに閉じ込める
    this->onClick = [c] {
        // オペレーター番号は 0始まりを想定しているので +1 して表示
        auto* w = new juce::AlertWindow(
            juce::String("") + "MML風入力(オペレーター" + juce::String(c.opIndex + 1) + ")",
            c.hintMessage, // ← 外から渡されたチャンネルごとのメッセージ
            juce::AlertWindow::QuestionIcon);

        w->addTextEditor("mmlInput", "", "");
        w->addButton(juce::String("") + "決定", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
        w->addButton(juce::String("") + "キャンセル", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

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

            // テキストエディタ内でEnterキーが押されたら、OK(1)として終了する
            editor->onReturnKey = [w] { w->exitModalState(1); };
        }
    };
}

void GuiCategoryLabel::setupInner(const Config& c, juce::Colour colour)
{
    this->visibleText = c.title;
    this->invisibleText = c.invisibleTitle.value_or(""); // 詳細テキストがない場合は空文字
    this->detailVisible = c.detailVisible;
    this->enableChangeDetailVisible = c.enableChangeDetailVisible;
    this->font = c.font.value_or(juce::Font(juce::FontOptions(16.0f, juce::Font::bold))); // ハードウェアカテゴリは太字の大きめフォントがデフォルト

    GuiLabel::setup({ .parent = c.parent, .title = ((!this->enableChangeDetailVisible || this->detailVisible) ? this->visibleText : this->invisibleText), .font = font, .justification = c.justification, .color = colour });

    this->setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.4f));

    // 切り替え可能なときは、表示・非表示をトグルする関数を追加
    if (this->enableChangeDetailVisible) {
        this->onClick = [this] {
            this->detailVisible = !this->detailVisible;

            if (detailVisible) {
                this->setText(visibleText, juce::NotificationType::sendNotification);
            }
            else {
                this->setText(invisibleText, juce::NotificationType::sendNotification);
            }

            ctx.editor.resized();
            };
    }
}

void GuiCategoryLabel::setup(const Config& c)
{
	setupInner(c, c.color);
}

void GuiCategoryLabel::setupHwCategory(const Config& c)
{
    setupInner(c, juce::Colours::yellow);
}

void GuiCategoryLabel::setupSwCategory(const Config& c)
{
    setupInner(c, juce::Colours::aqua);
}

void GuiCategoryLabel::setupOtherCategory(const Config& c)
{
    setupInner(c, juce::Colours::lime);
}
