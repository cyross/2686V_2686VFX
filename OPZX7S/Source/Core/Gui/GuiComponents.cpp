#include <map>
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
        float y = halfHeight - (m_displayBuffer[i] * halfHeight * 2.0f);
        wavePath.lineTo(x, y);
    }

    // カスタム波形色で描画
    g.setColour(lineColor);
    g.strokePath(wavePath, juce::PathStrokeType(2.0f));

    g.setColour(borderColor);
    g.drawRect(getLocalBounds(), 1);
}

// コンストラクタで色を受け取る
GuiStateView::GuiStateView(juce::Colour tColor, juce::Colour fColor, juce::Colour bColor)
    : trueColor(tColor), falseColor(fColor), borderColor(bColor)
{
}

void GuiStateView::updateState(bool state)
{
    this->state = state;

    repaint(); // データが来たら再描画
}

void GuiStateView::paint(juce::Graphics& g)
{
    g.fillAll(this->state ? trueColor : falseColor);


    g.setColour(borderColor);
    g.drawRect(getLocalBounds(), 2);
}

void ColoredGroupComponent::setBackgroundColor(juce::Colour c)
{
    backgroundColor = c;
    repaint();
}

void ColoredGroupComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // 背景色があれば描画 (角丸で少し柔らかく)
    if (!backgroundColor.isTransparent())
    {
        g.setColour(backgroundColor);
        g.fillRoundedRectangle(bounds, 4.0f);
    }

    // ---------------- 枠 ----------------
    // 上辺は見出しの中ほどを通す。JUCE の既定描画と同じ位置関係。
    auto frame = bounds.reduced(frameInset).withTrimmedTop(titleHeight * 0.5f - frameInset);

    if (frame.getWidth() > 1.0f && frame.getHeight() > 1.0f) {
        g.setColour(findColour(juce::GroupComponent::outlineColourId));
        g.drawRoundedRectangle(frame, cornerRadius, 1.0f);
    }

    // ---------------- 見出し ----------------
    // カテゴリ見出しと同じ手触りにする。明るい帯へ黒文字を載せ、
    // 枠線より後に描くことで、線が文字の後ろを通らないようにする。
    auto title = getText();

    if (title.isEmpty()) return;

    auto font = juce::Font(juce::FontOptions(titleFontHeight)).withStyle(juce::Font::bold);

    float textWidth = (float)juce::GlyphArrangement::getStringWidthInt(font, title);
    float chipWidth = juce::jmin(textWidth + titlePaddingX * 2.0f, bounds.getWidth() - titleInsetX * 2.0f);

    if (chipWidth <= 0.0f) return;

    juce::Rectangle<float> chip(bounds.getX() + titleInsetX, bounds.getY(), chipWidth, titleHeight);

    g.setColour(GuiColor::Group::TitleBg);
    g.fillRoundedRectangle(chip, titleCornerRadius);

    g.setColour(GuiColor::Group::TitleText);
    g.setFont(font);
    g.drawText(title, chip, juce::Justification::centred, true);
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

    this->bgColor = c.bgColor;

    if (!c.color.isTransparent())
    {
        this->setColour(juce::Label::textColourId, c.color);
    }
}

void GuiLabel::paint(juce::Graphics& g)
{
    // 地色を渡されたときだけ、カテゴリ見出しと同じ丸みで帯を敷く
    if (!bgColor.isTransparent())
    {
        g.setColour(bgColor);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), guiCornerRadius);
    }

    juce::Label::paint(g);
}

void GuiSlider::setup(const Config& c)
{
    label.setup({ .parent = c.parent, .title = c.title, .color = c.labelColor, .bgColor = GuiColor::Label::RowBg });

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
    label.setup({ .parent = c.parent, .title = c.title, .color = c.labelColor, .bgColor = GuiColor::Label::RowBg });

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

    // 明るい面を持つ部品なので、ボタンと同じ輪郭で締める。
    // 角の丸みは JUCE 側が既に guiCornerRadius と同じ値で描いている。
    this->setColour(juce::ComboBox::outlineColourId, GuiColor::Outline);
    this->setColour(juce::ComboBox::arrowColourId, GuiColor::ComboBox::Arrow);

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

    g.setFont(buttonFont);
    juce::String text = getButtonText();
    float textWidth = juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), juce::StringRef(text));
    float totalWidth = boxW + labelGapW + textWidth;

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
    float boxY = (bounds.getHeight() - boxH) * 0.5f;
    juce::Rectangle<float> box(startX, boxY, boxW, boxH);

    // 角の丸みは他の部品と揃える。小さな四角なので、辺の半分を上限にする。
    float boxRadius = juce::jmin(guiCornerRadius, juce::jmin(boxW, boxH) * 0.5f);

    g.setColour(GuiColor::ToggleButton::Box.withMultipliedAlpha(alpha));
    g.drawRoundedRectangle(box, boxRadius, 1.0f);

    // 2. 中のランプ。消えているときも枠の中を塗って、
    //    「空か塗りか」ではなく「色が違う」で状態が分かるようにする。
    auto inner = box.reduced(boxGapW, boxGapH);

    g.setColour((getToggleState() ? GuiColor::ToggleButton::LampOn : GuiColor::ToggleButton::LampOff)
        .withMultipliedAlpha(alpha));
    g.fillRoundedRectangle(inner, juce::jmin(boxRadius, juce::jmin(inner.getWidth(), inner.getHeight()) * 0.5f));

    // 3. テキストの描画
    juce::Rectangle<float> textBounds(box.getRight() + labelGapW, 0.0f, bounds.getWidth() - box.getRight() - labelGapW, bounds.getHeight());
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
    label.setup({ .parent = c.parent, .title = c.title, .color = c.labelColor, .bgColor = GuiColor::Label::RowBg });

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

        GuiDialog::styleButtons(*w);

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

void GuiCategoryLabel::setupInner(const Config& c, juce::Colour background)
{
    this->captionText = c.title;
    this->detailVisible = c.detailVisible;
    this->enableChangeDetailVisible = c.enableChangeDetailVisible;
    this->bgColor = background;
    this->font = c.font.value_or(juce::Font(juce::FontOptions(16.0f, juce::Font::bold)));

    GuiLabel::setup({ .parent = c.parent, .title = this->captionText, .font = font, .justification = c.justification, .color = GuiColor::Category::Text });

    // 中身の背後へ敷く板。見出しより先に親へ入れて背面へ送る。
    c.parent.addAndMakeVisible(backdrop);
    backdrop.setVisible(false);
    backdrop.toBack();

    // 背景はこのクラスが自前で描くので、Label 側の塗りは切っておく
    this->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);

    // 切り替え可能なときは、表示・非表示をトグルする
    if (this->enableChangeDetailVisible) {
        this->onClick = [this] {
            this->detailVisible = !this->detailVisible;

            this->repaint();

            ctx.editor.resized();
            };
    }
}

void GuiCategoryLabel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    if (bounds.isEmpty()) return;

    // ---------------- 背景 ----------------
    g.setColour(bgColor);
    g.fillRoundedRectangle(bounds, cornerRadius);

    auto textArea = bounds.reduced(paddingX, 0.0f);

    // ---------------- 開閉マーカー ----------------
    // 位置は左端固定。開いているときはピンク、閉じているときは黒。
    if (enableChangeDetailVisible) {
        float side = juce::jmin(markerSize, bounds.getHeight() - 4.0f);

        juce::Rectangle<float> marker(
            bounds.getX() + paddingX,
            bounds.getCentreY() - side * 0.5f,
            side,
            side);

        g.setColour(detailVisible ? GuiColor::Category::MarkerOpen : GuiColor::Category::MarkerClosed);
        g.fillRoundedRectangle(marker, 1.5f);

        g.setColour(GuiColor::Category::MarkerBorder);
        g.drawRoundedRectangle(marker, 1.5f, 1.0f);

        textArea.removeFromLeft(side + paddingX);
    }

    // ---------------- 見出し ----------------
    g.setColour(GuiColor::Category::Text);
    g.setFont(font);
    g.drawText(captionText, textArea, getJustificationType(), false);
}

// ----------------------------------------------------------------------------
// カテゴリの中身に敷く板
// ----------------------------------------------------------------------------
// 見出しを置いた時点では中身の高さが分からないので、次の見出しが置かれた
// ときに 1 つ前の板を閉じる、という繋ぎ方にしてある。最後の 1 枚は欄の
// 終わり (GuiScrollGroup::setContentHeight) で閉じる。
namespace
{
    // 親コンポーネントごとに「まだ下端が決まっていない板」を覚えておく。
    // レイアウトはメッセージスレッドでしか走らないので単純な map でよい。
    std::map<juce::Component*, GuiCategoryLabel*> g_pendingBackdrops;
}

void GuiCategoryLabel::beginBackdrop(const juce::Rectangle<int>& contentArea)
{
    auto* parent = getParentComponent();

    if (parent == nullptr) return;

    if (!isOpen()) {
        backdrop.setVisible(false);
        return;
    }

    // 上端だけ決めておく。高さは closePending で入る。
    // 板は見出しと同じ幅。中身はこのあと内側へ寄せるので、左右に余白ができる。
    backdrop.setBounds(contentArea.getX(), contentArea.getY(), contentArea.getWidth(), 0);
    backdrop.setVisible(false);

    g_pendingBackdrops[parent] = this;
}

bool GuiCategoryLabel::closePending(juce::Component* parent, int bottom)
{
    if (parent == nullptr) return false;

    auto it = g_pendingBackdrops.find(parent);

    if (it == g_pendingBackdrops.end()) return false;

    auto* label = it->second;

    g_pendingBackdrops.erase(it);

    if (label == nullptr) return false;

    auto b = label->backdrop.getBounds();

    // 次の見出しにくっつくと、板が下のカテゴリのものに見えてしまう。
    int height = (bottom - gapBelow) - b.getY();

    if (height <= 0) {
        label->backdrop.setVisible(false);
        return true;
    }

    label->backdrop.setBounds(b.getX(), b.getY(), b.getWidth(), height);
    label->backdrop.setVisible(true);
    label->backdrop.toBack();

    return true;
}


void GuiCategoryLabel::setup(const Config& c)
{
    setupInner(c, c.color);
}

void GuiCategoryLabel::setupHwCategory(const Config& c)
{
    setupInner(c, GuiColor::Category::HwBg);
}

void GuiCategoryLabel::setupSwCategory(const Config& c)
{
    setupInner(c, GuiColor::Category::SwBg);
}

void GuiCategoryLabel::setupOtherCategory(const Config& c)
{
    setupInner(c, GuiColor::Category::OtherBg);
}

void GuiCategoryLabel::setupCategory(const Config& c, juce::Colour bgColor)
{
    setupInner(c, bgColor);
}

void closeCategoryBackdrops(juce::Component* parent, int bottom)
{
    GuiCategoryLabel::closePending(parent, bottom);
}

void GuiDialog::applyTheme()
{
    auto& lf = juce::LookAndFeel::getDefaultLookAndFeel();

    lf.setColour(juce::AlertWindow::backgroundColourId, GuiColor::Palette::OffWhite);
    lf.setColour(juce::AlertWindow::textColourId, GuiColor::Palette::OffBlack);
    lf.setColour(juce::AlertWindow::outlineColourId, GuiColor::Palette::OffBlack);

    // 文章や入力欄も明るい地に黒文字で揃える
    lf.setColour(juce::Label::textColourId, GuiColor::Palette::OffBlack);
    lf.setColour(juce::TextEditor::backgroundColourId, GuiColor::Palette::OffWhite);
    lf.setColour(juce::TextEditor::textColourId, GuiColor::Palette::OffBlack);
    lf.setColour(juce::TextEditor::outlineColourId, GuiColor::Palette::OffBlack);
    lf.setColour(juce::TextEditor::highlightedTextColourId, GuiColor::Palette::OffBlack);

    // ボタンの既定は決定ボタンの色にしておく。警告のように OK が 1 つしか
    // 無いダイアログは中でボタンが作られ、あとから触れないため。
    lf.setColour(juce::TextButton::buttonColourId, GuiColor::Palette::DialogOkBackBlue);
    lf.setColour(juce::TextButton::textColourOffId, GuiColor::Palette::OffBlack);
    lf.setColour(juce::TextButton::textColourOnId, GuiColor::Palette::OffBlack);

    // JUCE はボタンの枠を ComboBox の枠色で描く。
    // 画面側のコンボボックスは自前の LookAndFeel を持つので影響しない。
    lf.setColour(juce::ComboBox::outlineColourId, GuiColor::Palette::OffBlack);
}

void GuiDialog::styleButtons(juce::AlertWindow& window)
{
    int index = 0;

    for (int i = 0; i < window.getNumChildComponents(); ++i)
    {
        auto* button = dynamic_cast<juce::TextButton*>(window.getChildComponent(i));

        if (button == nullptr) continue;

        // 先に addButton した方を決定として扱う。子は追加した順に並ぶ。
        button->setColour(juce::TextButton::buttonColourId,
            (index == 0) ? GuiColor::Palette::DialogOkBackBlue : GuiColor::Palette::OffWhite);

        ++index;
    }
}
