#include "GuiComponents.h"
#include "../editor/PluginEditor.h"

// アイコンのパスを生成するヘルパー関数
juce::Path CustomTabLookAndFeel::getIconPath(const juce::String& name, juce::Rectangle<float> area)
{
    juce::Path p;
    auto center = area.getCentre();
    float s = std::min(area.getWidth(), area.getHeight()) * 0.5f; // アイコンサイズ

    if (name == fxTabName) // エフェクトを示すアイコン
    {
        // [FX] Icon
        juce::Path fPath;
        fPath.addRectangle(center.x - s * 0.8f, center.y - s * 0.6f, s * 0.2f, s * 1.2f); // 縦棒
        fPath.addRectangle(center.x - s * 0.6f, center.y - s * 0.6f, s * 0.5f, s * 0.2f); // 横棒上
        fPath.addRectangle(center.x - s * 0.6f, center.y - s * 0.1f, s * 0.3f, s * 0.2f); // 横棒中
        p.addPath(fPath);

        // X
        juce::Path xPath;
        float xw = s * 0.2f;
        float xh = s * 1.2f;
        float xPivot = center.x + s * 0.4f; // Xの交差点（中心）

        // Xのクロスを、同じ中心位置にある長方形を左右に傾けて表現する
        juce::Path bar1;
        bar1.addRectangle(xPivot - xw * 0.5f, center.y - xh * 0.5f, xw, xh);
        bar1.applyTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::pi * 0.15f, xPivot, center.y));

        juce::Path bar2;
        bar2.addRectangle(xPivot - xw * 0.5f, center.y - xh * 0.5f, xw, xh);
        bar2.applyTransform(juce::AffineTransform::rotation(-juce::MathConstants<float>::pi * 0.15f, xPivot, center.y));

        p.addPath(bar1);
        p.addPath(bar2);
    }
    else if (name == presetTabName) // プリセットブラウザタブはフォルダーアイコン
    {
        // --- Folder / List Icon ---
        // フォルダーの形
        float w = s * 1.2f;
        float h = s * 0.9f;
        juce::Rectangle<float> folderRect(center.x - w / 2, center.y - h / 2 + s * 0.1f, w, h);
        p.addRoundedRectangle(folderRect, 2.0f);
        // タブ部分
        p.addRectangle(center.x - w / 2, center.y - h / 2 - s * 0.2f, w * 0.4f, s * 0.3f);

        // 中身の線 (リストっぽく)
        p.startNewSubPath(center.x - w * 0.3f, center.y);
        p.lineTo(center.x + w * 0.3f, center.y);
        p.startNewSubPath(center.x - w * 0.3f, center.y + h * 0.3f);
        p.lineTo(center.x + w * 0.3f, center.y + h * 0.3f);
    }
    else if (name == settingsTabName) // 設定タブは歯車アイコン
    {
        // --- Gear Icon ---
        // 歯車
        float rOut = s * 0.6f;
        float rIn = s * 0.45f;
        float rHole = s * 0.2f;

        p.addCentredArc(center.x, center.y, rIn, rIn, 0, 0, juce::MathConstants<float>::twoPi, true);

        // 歯を描く (8個)
        for (int i = 0; i < 8; ++i)
        {
            float angle = i * juce::MathConstants<float>::twoPi / 8.0f;
            juce::Path tooth;
            tooth.addRectangle(center.x - s * 0.12f, center.y - rOut, s * 0.24f, rOut - rIn + 2.0f);
            tooth.applyTransform(juce::AffineTransform::rotation(angle, center.x, center.y));
            p.addPath(tooth);
        }
        // 真ん中の穴
        p.addEllipse(center.x - rHole, center.y - rHole, rHole * 2, rHole * 2);
        p.setUsingNonZeroWinding(false); // 穴を抜く設定
    }
    else if (name == aboutTabName) // Aboutタブは丸囲みiアイコン
    {
        // --- Info Icon (i) ---
        // 丸
        float r = s * 0.6f;
        p.addEllipse(center.x - r, center.y - r, r * 2, r * 2);

        // i の点
        float dotSize = s * 0.15f;
        juce::Path dot;
        dot.addEllipse(center.x - dotSize / 2, center.y - r * 0.5f, dotSize, dotSize);
        p.addPath(dot);

        // i の棒
        juce::Path bar;
        bar.addRectangle(center.x - dotSize / 2, center.y - r * 0.1f, dotSize, r * 0.8f);
        p.addPath(bar);

        p.setUsingNonZeroWinding(false); // 穴を抜く設定
    }
    return p;
}

void CustomTabLookAndFeel::drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown)
{
    auto area = button.getActiveArea().toFloat();
    juce::String name = button.getButtonText();

    // 1. タブ背景の描画
    // 選択中タブの背景色
    auto selectedBg = juce::Colour::fromFloatRGBA(0.4f, 0.4f, 0.4f, 1.0f);
    // 非選択タブの背景色
    auto unselectedBg = juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0f);

    if (button.isFrontTab()) {
        g.setColour(selectedBg);
    }
    else {
        g.setColour(unselectedBg);
        // マウスオーバーで背景も少し明るく
        if (isMouseOver) g.setColour(unselectedBg.brighter(0.1f));
    }
    g.fillRect(area);

    // 2. コンテンツ（テキストまたはアイコン）の色決定
    juce::Colour contentColour;
    if (button.isFrontTab()) {
        // 選択時は完全な白（強調）
        contentColour = juce::Colours::white;
    }
    else {
        // 非選択時はグレー
        contentColour = juce::Colours::grey;
        // マウスオーバー時は少し明るくして反応させる
        if (isMouseOver) contentColour = contentColour.brighter(0.3f);
    }

    // 3. 描画
    g.setColour(contentColour); // 色を確定

    if (name == fxTabName || name == presetTabName || name == settingsTabName || name == aboutTabName)
    {
        // アイコン描画
        juce::Path icon = getIconPath(name, area);
        g.fillPath(icon);
    }
    else
    {
        // テキスト描画
        g.setFont(juce::Font(16.0f).withStyle(button.isFrontTab() ? juce::Font::bold : juce::Font::plain));
        g.drawText(name, area, juce::Justification::centred, true);
    }
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
    GuiSlider::setTextValueSuffix(masterVolumeUnit); // 単位表示
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
        };
}
