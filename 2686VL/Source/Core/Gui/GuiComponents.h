#pragma once

#include <JuceHeader.h>
#include <array>
#include <functional>
#include <span>
#include <vector>
#include <atomic>

#include "./GuiColor.h"
#include "./GuiStructs.h"
#include "./GuiContext.h"
#include "./GuiValues.h"
#include "./GuiText.h"

#include "../../Core/Fm/FmSliderRegMap.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

class ColoredGroupComponent : public juce::GroupComponent
{
    juce::Colour backgroundColor = GuiColor::Group::Bg;
public:
    void setBackgroundColor(juce::Colour c);
    void paint(juce::Graphics& g) override;
};

class GuiWaveformPreview : public juce::Component
{
public:
    juce::Colour bgColor;
    juce::Colour lineColor;
    juce::Colour borderColor;
    juce::Colour axisColor;

    // コンストラクタで色を受け取る
    GuiWaveformPreview(juce::Colour background, juce::Colour line, juce::Colour border = juce::Colours::white, juce::Colour axis = juce::Colours::yellow);
    void pushBuffer(const float* data, int numSamples);
    void paint(juce::Graphics& g) override;
private:
    std::vector<float> m_displayBuffer;
};

class GuiStateView : public juce::Component
{
public:
    bool state;
    juce::Colour trueColor;
    juce::Colour falseColor;
    juce::Colour borderColor;

    // コンストラクタで色を受け取る
    GuiStateView(juce::Colour tColor = juce::Colours::red.withAlpha(0.85f), juce::Colour fColor = juce::Colours::black.withAlpha(0.85f), juce::Colour bColor = juce::Colours::darkgrey);
    void updateState(bool state);
    void paint(juce::Graphics& g) override;
};

class GuiBaseComponent
{
protected:
    GuiContext ctx;
public:
    GuiBaseComponent(const GuiContext& context) : ctx(context) {}
};

class GuiGroup : public ColoredGroupComponent, public GuiBaseComponent
{
    juce::Colour borderColor = GuiColor::Group::Border;
    juce::Colour textColor = GuiColor::Group::Text;
public:
    GuiGroup(const GuiContext& context) : GuiBaseComponent(context) {}

    void setup(juce::Component& parent, const juce::String title);
};

// =======================================================
// スクロールバー付きのグループコンポーネント
// =======================================================
class GuiScrollGroup : public ColoredGroupComponent, public GuiBaseComponent
{
    juce::Colour borderColor = GuiColor::Group::Border;
    juce::Colour textColor = GuiColor::Group::Text;

    juce::Rectangle<int> customViewportBounds;
public:
    // Viewport本体
    juce::Viewport viewport;

    // Viewportの中に入れてスクロールさせる、巨大なキャンバス
    juce::Component contentCanvas;

    GuiScrollGroup(const GuiContext& context) : GuiBaseComponent(context)
    {
        // Viewportの設定
        viewport.setScrollBarsShown(true, false); // 縦スクロールのみON, 横はOFF
        viewport.setOpaque(false);
        viewport.setViewedComponent(&contentCanvas, false); // キャンバスをセット(所有権は持たせない)
    }

    void setup(juce::Component& parent, const juce::String title)
    {
        parent.addAndMakeVisible(*this);
        this->setText(title);
        this->setColour(juce::GroupComponent::textColourId, textColor);
        this->setColour(juce::GroupComponent::outlineColourId, borderColor);
        this->toBack();

        // 自身の内側にViewportを配置
        addAndMakeVisible(viewport);
    }

    // 外部からViewportの領域を指定できるようにする
    void setViewportCustomBounds(juce::Rectangle<int> bounds)
    {
        customViewportBounds = bounds;
        viewport.setBounds(customViewportBounds);
    }

    void resized() override
    {
        if (!customViewportBounds.isEmpty()) {
            viewport.setBounds(customViewportBounds);
        }
        else {
            auto bounds = getLocalBounds().reduced(4, 16);
            bounds.removeFromTop(10);
            viewport.setBounds(bounds);
        }
    }

    // 中身のキャンバスの「本当の高さ」をセットする関数
    // (これを呼ばないとスクロールバーが出ません)
    void setContentHeight(int totalHeight)
    {
        // 幅はViewportと同じにする（スクロールバーの幅を考慮）
        int contentWidth = viewport.getMaximumVisibleWidth();
        contentCanvas.setSize(contentWidth, totalHeight);
    }
};

class GuiLabel : public juce::Label, public GuiBaseComponent
{
public:
    GuiLabel(const GuiContext& context) : GuiBaseComponent(context) {}
    // =======================================================
    // クリック時の処理を保持するコールバック
    // =======================================================

    std::function<void()> onClick = nullptr;

    struct Config {
        juce::Component& parent;
        juce::String title;
        std::optional<juce::Font> font = std::nullopt;
        juce::Justification justification = juce::Justification::centred;
        juce::Colour color = GuiColor::Label::Text;
    };

    void setup(const Config& c);

    // =======================================================
    // マウスボタンが押された瞬間のイベントを検知
    // =======================================================
    void mouseDown(const juce::MouseEvent& e) override
    {
        // 左クリックで、かつ onClick に処理が登録されていれば実行
        if (e.mods.isLeftButtonDown() && onClick != nullptr)
        {
            onClick();
        }

        // 親クラス（通常のLabel）のクリック処理も一応呼んでおく
        juce::Label::mouseDown(e);
    }

    // =======================================================
    // マウスが乗った時に「押せる感（指カーソル）」を出す
    // =======================================================
    void mouseEnter(const juce::MouseEvent& e) override
    {
        if (onClick != nullptr) {
            setMouseCursor(juce::MouseCursor::PointingHandCursor); // 👆カーソルにする
        }
        juce::Label::mouseEnter(e);
    }
};

class GuiSlider : public juce::Slider, public GuiBaseComponent
{
protected:
    std::unique_ptr<SliderAttachment> att;
public:
    GuiSlider(const GuiContext& context) : GuiBaseComponent(context), label(context) {}

    GuiLabel label;

    struct Config {
        juce::Component& parent;
        juce::String id = "";
        juce::String title;
        juce::Colour trackColor = GuiColor::Slider::Track;
        juce::Colour thumbColor = GuiColor::Slider::Thumb;
        juce::Colour valueBorderColor = GuiColor::Slider::Value::Border;
        juce::Colour valueTextColor = GuiColor::Slider::Value::Text;
        juce::Colour valueHighlightColor = GuiColor::Slider::Value::Highlight;
        bool isReset = false;
        std::optional<juce::Font> labelFont = std::nullopt;
        juce::Justification labelJustification = juce::Justification::centred;
        juce::Colour labelColor = GuiColor::Label::Text;
        RegisterType regType = RegisterType::None;
    };

    void setup(const Config& c);

    // 右クリックによるドラッグ操作を完全にブロックする
    void mouseDown(const juce::MouseEvent& e) override
    {
        if (e.mods.isRightButtonDown()) return; // 右クリックなら何もしない（値を変えない）

        juce::Slider::mouseDown(e);             // 左クリックなら通常の処理を継続
    }

    // 右クリックによるドラッグ操作を完全にブロックする
    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (e.mods.isRightButtonDown()) return; // 右ドラッグなら何もしない

        juce::Slider::mouseDrag(e);             // 左ドラッグなら通常の処理を継続
    }

    inline void setEnabledWithLabel(bool enable)
    {
        this->setEnabled(enable);
        this->label.setEnabled(enable);
    }

    inline void setVisibleWithLabel(bool visible)
    {
        this->setVisible(visible);
        this->label.setVisible(visible);
    }
};

class GuiComboBox : public juce::ComboBox, public GuiBaseComponent
{
protected:
    std::unique_ptr<ComboBoxAttachment> att;

    // =======================================================
    // ドロップダウン用のカスタム LookAndFeel
    // =======================================================
    class ComboBoxLF : public juce::LookAndFeel_V4
    {
    public:
        ComboBoxLF(): juce::LookAndFeel_V4() {
        }

        // デフォルトのフォントサイズ（少し大きめの14.0fなどを指定）
        juce::Font selectedFont = juce::Font(juce::FontOptions(13.0f));
        juce::Font dropdownFont = juce::Font(juce::FontOptions(14.0f));

        // ドロップダウンメニューのフォントサイズを上書き
        juce::Font getPopupMenuFont() override {
            return dropdownFont;
        }

        // コンボボックス表面（選択中のテキスト）のフォントサイズを上書き
        juce::Font getComboBoxFont(juce::ComboBox&) override {
            return selectedFont;
        }

        juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(juce::ComboBox& box, juce::Label& label) override
        {
            // カスタムフォントの高さに合わせて、ドロップダウン1行の「高さ」を計算
            // (+6 は文字がギリギリにならないための上下の少しの余白です)
            int itemHeight = (int)dropdownFont.getHeight() + 6;

            // デフォルトの挙動を保ちつつ、高さ(StandardItemHeight)だけを強制上書きして返す
            return juce::PopupMenu::Options().withTargetComponent(&box)
                .withItemThatMustBeVisible(box.getSelectedId())
                .withMinimumWidth(box.getWidth())
                .withMaximumNumColumns(1)
                .withStandardItemHeight(itemHeight); // ←ここが最大のポイント
        }
    };

    // クラスのメンバとして保持する
    ComboBoxLF customLF;
public:
    GuiComboBox(const GuiContext& context) : GuiBaseComponent(context), label(context) {
        this->setLookAndFeel(&customLF);
    }

    ~GuiComboBox() override
    {
        // メンバ変数(customLF)が破棄される前に、必ず nullptr に戻して安全に解除する
        this->setLookAndFeel(nullptr);
    }

    GuiLabel label;

    struct Config {
        juce::Component& parent;
        juce::String id = "";
        juce::String title;
        std::vector<SelectItem>& items;
        juce::Colour color = GuiColor::ComboBox::Text;
        juce::Colour bgColor = GuiColor::ComboBox::Bg;
        bool isReset = true;
        bool isResized = false;
        std::optional<juce::Font> labelFont = std::nullopt;
        juce::Justification labelJustification = juce::Justification::centred;
        juce::Colour labelColor = GuiColor::Label::Text;
        std::optional<juce::Font> selectedFont = juce::Font(juce::FontOptions(13.0f));
        std::optional<juce::Font> dropdownFont = juce::Font(juce::FontOptions(16.0f));
        RegisterType regType = RegisterType::None;
    };

    void setup(const Config& c);

    inline void setEnabledWithLabel(bool enable)
    {
        this->setEnabled(enable);
        this->label.setEnabled(enable);
    }

    inline void setVisibleWithLabel(bool visible)
    {
        this->setVisible(visible);
        this->label.setVisible(visible);
    }
};

class GuiToggleButton : public juce::ToggleButton, public GuiBaseComponent
{
protected:
    std::unique_ptr<ButtonAttachment> att;
    juce::Justification textJustification = juce::Justification::centred;
    juce::Font buttonFont = juce::Font(juce::FontOptions(12.0f));
public:
    GuiToggleButton(const GuiContext& context) : GuiBaseComponent(context) {
    }

    struct Config {
        juce::Component& parent;
        juce::String id = "";
        juce::String title;
        std::optional<juce::Font> font = std::nullopt;
        juce::Justification justification = juce::Justification::centred;
        juce::Colour color = GuiColor::ToggleButton::Fg;
        bool isReset = false;
        bool isResized = false;
    };

    void setup(const Config& c);
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class GuiTextButton : public juce::TextButton, public GuiBaseComponent
{
protected:
    std::unique_ptr<ButtonAttachment> att;

    class TextButtonLF : public juce::LookAndFeel_V4
    {
    public:
        std::optional<juce::Font> customFont = juce::Font(juce::FontOptions(13.0f));

        // テキストボタンのフォントを要求された時に呼ばれる関数をオーバーライド
        juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
        {
            if (customFont.has_value()) {
                return customFont.value();
            }
            // 指定がない場合は、ボタンの高さに合わせたJUCEの標準フォントを返す
            return juce::Font(juce::FontOptions(juce::jmin(16.0f, (float)buttonHeight * 0.6f)));
        }

        // =======================================================
        // ボタンの背景と枠線の描画を完全にコントロールする
        // =======================================================
        void drawButtonBackground(juce::Graphics& g, juce::Button& button,
            const juce::Colour& backgroundColour,
            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            auto bounds = button.getLocalBounds().toFloat();
            float cornerSize = 1.0f; // 角丸のサイズ（お好みで調整してください）

            // 背景の塗りつぶし
            juce::Colour baseColour = backgroundColour.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.4f);

            // マウスホバー時やクリック時は少し明るくする
            if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
                baseColour = baseColour.brighter(0.5f);

            g.setColour(baseColour);
            g.fillRoundedRectangle(bounds, cornerSize);
        }
    };

    TextButtonLF customLF;
public:
    GuiTextButton(const GuiContext& context) : GuiBaseComponent(context) {
        this->setLookAndFeel(&customLF);
    }

    ~GuiTextButton() override
    {
        this->setLookAndFeel(nullptr);
    }

    struct Config {
        juce::Component& parent;
        juce::String id = "";
        juce::String title;
        std::optional<juce::Font> font = std::nullopt;
        juce::Colour textColor = GuiColor::TextButton::Text;
        juce::Colour textOnColor = GuiColor::TextButton::TextOn;
        juce::Colour bgColor = GuiColor::TextButton::Bg;
        juce::Colour borderColor = GuiColor::TextButton::Border;
        bool isReset = false;
        bool isResized = false;
    };

    void setup(const Config& c);
};

class GuiHyperLink : public juce::HyperlinkButton, public GuiBaseComponent
{
protected:
    std::unique_ptr<ButtonAttachment> att;
public:
    GuiHyperLink(const GuiContext& context) : GuiBaseComponent(context) {}

    struct Config {
        juce::Component& parent;
        juce::String id = "";
        juce::String title;
		juce::URL url;
        std::optional<juce::Font> font = std::nullopt;
        juce::Justification justification = juce::Justification::centred;
        juce::Colour color = juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
        bool isReset = false;
        bool isResized = false;
    };

    void setup(const Config& c);
};

class GuiImage : public juce::ImageComponent, public GuiBaseComponent
{
protected:
    std::unique_ptr<ButtonAttachment> att;
public:
    GuiImage(const GuiContext& context) : GuiBaseComponent(context) {}

    void setup(juce::Component& parent);
};

class GuiTableList : public juce::TableListBox, public juce::TableListBoxModel, public GuiBaseComponent
{
protected:
    std::unique_ptr<ButtonAttachment> att;
    juce::Colour selectedBgColor;
    juce::Colour bgColor1;
    juce::Colour bgColor2;
public:
    GuiTableList(const GuiContext& context) : GuiBaseComponent(context) {
        this->setModel(this);
    }

    std::function<int()> onGetNumRows = nullptr;
    std::function<juce::String(int row, int columnId)> onGetCellText = nullptr;
    std::function<void(int row)> onDoubleClicked = nullptr;
    std::function<void(int lastRowSelected)> onSelectionChanged = nullptr;
    std::function<juce::String(int row, int columnId)> onGetCellTooltip = nullptr;

    struct Config {
        juce::Component& parent;
        juce::String id = "";
        juce::String title;
        std::optional<juce::Font> font = std::nullopt;
        juce::Justification justification = juce::Justification::centred;
        juce::Colour color = GuiColor::TableList::Text;
        juce::Colour bgColor1 = GuiColor::TableList::RowBg1;
        juce::Colour bgColor2 = GuiColor::TableList::RowBg2;
        juce::Colour selectedBgColor = GuiColor::TableList::SelectedBg;
        bool canMultipleSelection = false;
    };

    void setup(const Config& c);
	void addColumn(const juce::String& columnName, int columnId, int width);
    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::String getCellTooltip(int rowNumber, int columnId) override;
    void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& e) override;
    void selectedRowsChanged(int lastRowSelected) override;
    std::function<void(int columnId, bool isForwards)> onSortOrderChanged;

    void sortOrderChanged(int newSortColumnId, bool isForwards) override
    {
        if (onSortOrderChanged != nullptr) {
            onSortOrderChanged(newSortColumnId, isForwards);
        }
    }
};

class GuiTextEditor : public juce::TextEditor, public GuiBaseComponent
{
protected:
    std::unique_ptr<ButtonAttachment> att;
public:
    GuiTextEditor(const GuiContext& context) : GuiBaseComponent(context), label(context) {}

    GuiLabel label;

    struct Config {
        juce::Component& parent;
        juce::String id = "";
        juce::String title;
        juce::Colour color = GuiColor::TextEditor::Text;
        juce::Colour bgColor = GuiColor::TextEditor::Bg;
        juce::Colour borderColor = GuiColor::TextEditor::Border;
        std::optional<juce::Font> labelFont = std::nullopt;
        juce::Justification labelJustification = juce::Justification::centred;
        juce::Colour labelColor = GuiColor::Label::Text;
        bool isMultiLine = false;
        bool isReturnKeyStartsNewLine = false;
    };

    void setup(const Config& c);

    inline void setEnabledWithLabel(bool enable)
    {
        this->setEnabled(enable);
        this->label.setEnabled(enable);
    }

    inline void setVisibleWithLabel(bool visible)
    {
        this->setVisible(visible);
        this->label.setVisible(visible);
    }
};

class GuiFbSlider : public GuiSlider
{
public:
    // 親のコンストラクタをそのまま使う
    using GuiSlider::GuiSlider;

    void setup(const GuiSlider::Config& c);
};

class GuiMasterVolumeSlider : public GuiSlider
{
public:
    // 親のコンストラクタをそのまま使う
    using GuiSlider::GuiSlider;

    void setup(const GuiSlider::Config& c);
};

class GuiMmlButton : public GuiTextButton
{
public:
    // 親のコンストラクタをそのまま使う
    using GuiTextButton::GuiTextButton;

    // MML入力ポップアップのための専用設定
    struct MmlConfig {
        int opIndex;                     // オペレーター番号 (0〜)
        juce::String hintMessage;        // 音源ごとのヒントテキスト
        std::function<void(juce::String)> onMmlApplied; // OKが押された時の処理
    };

    void setupMml(const MmlConfig& c);
};

class GuiCategoryLabel : public GuiLabel
{
    bool enableChangeDetailVisible = false;
    bool detailVisible = false;
	juce::String visibleText = juce::String();
    juce::String invisibleText = juce::String();
    juce::Font font = juce::Font(juce::FontOptions(16.0f, juce::Font::bold));
	juce::Colour labelColor = GuiColor::Label::CategoryText;
public:
    GuiCategoryLabel(const GuiContext& context) : GuiLabel(context) {}

    struct Config {
        juce::Component& parent;
        juce::String title;
		std::optional<juce::String> invisibleTitle = std::nullopt; // 詳細テキスト（省略可能）
        std::optional<juce::Font> font = std::nullopt;
        juce::Justification justification = juce::Justification::centred;
        juce::Colour color = GuiColor::Label::CategoryText;
        bool detailVisible = false;
        bool enableChangeDetailVisible = false;
    };

    void setup(const Config& c);
	void setupHwCategory(const Config& c); // ハードウェアカテゴリ用の簡易設定
	void setupSwCategory(const Config& c); // ソフトウェアカテゴリ用の簡易設定
	void setupOtherCategory(const Config& c); // その他カテゴリ用の簡易設定
	bool isDetailVisible() const { return this->detailVisible; }
private:
    void setupInner(const Config& c, juce::Colour colour);
};

class GuiSeparator : public juce::Component, public GuiBaseComponent
{
public:
    enum class Style { Horizontal, Vertical };

    struct Config {
        Style lineStyle = Style::Horizontal;
        float lineRate = 100.0;
        float lineThick = 1.0;
        juce::Colour lineColour = juce::Colours::grey.withAlpha(0.5f);
    };

    GuiSeparator(const GuiContext& context) : GuiBaseComponent(context) {}

    void setup(const Config& c) {
        lineStyle = c.lineStyle;
        lineRate = c.lineRate;
        lineThick = c.lineThick;
        lineColour = c.lineColour;
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(lineColour); // 区切り線の色

        if (lineStyle == Style::Horizontal) {
            // 水平線の描画 (中央の高さに引く)
            float realWidth = (float)getWidth();
            float width = realWidth * lineRate;
            float paddingX = (realWidth - width) / 2.0;

            float y = (getHeight() - lineThick) / 2.0f;

            g.drawLine(paddingX, y, paddingX + width, y, lineThick);
        }
        else {
            // 垂直線の描画
            float realHeight = (float)getHeight();
            float height = realHeight * lineRate;
            float paddingY = (realHeight - height) / 2.0;

            float x = (getWidth() - lineThick) / 2.0f;

            g.drawLine(x, paddingY, x, paddingY + height, lineThick);
        }
    }

private:
    Style lineStyle = Style::Horizontal;
    float lineRate = 100.0f;
    float lineThick = 1.0f;
    juce::Colour lineColour = juce::Colours::grey.withAlpha(0.5f);
};
