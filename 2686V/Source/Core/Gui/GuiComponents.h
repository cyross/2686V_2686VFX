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

// カテゴリ見出しから広げた共通の角の丸み。
// 見出し・板・帯・ボタンで同じ値を使うことで、部品が同じ仲間に見えるようにする。
inline constexpr float guiCornerRadius = 3.0f;

// ============================================================================
// 影
// ============================================================================
// 面を持つ部品の右下へ落とす。
//
// juce::DropShadow は描くたびに画像を作ってぼかすため、ドラッグ中に何度も
// 描き直す部品では割に合わない。ここでは輪郭を少しずつ広げながら薄く重ねて、
// 画像を作らずに柔らかい縁を出している。
//
// 部品は自分の領域の外へ描けないので、本体を右下ぶんだけ内側へ寄せて
// 場所を空ける。レイアウトの寸法は変えずに済む。
namespace GuiShadow
{
    inline constexpr float offsetX = 2.0f;
    inline constexpr float offsetY = 2.0f;

    // ぼかしの層数。多いほど柔らかいが、そのぶん外へはみ出す。
    inline constexpr int layers = 2;

    // 本体を描く領域。影のぶんだけ右下を空ける。
    inline juce::Rectangle<float> reserve(juce::Rectangle<float> bounds)
    {
        return bounds.withTrimmedRight(offsetX).withTrimmedBottom(offsetY);
    }

    inline void drawRounded(juce::Graphics& g, juce::Rectangle<float> face, float cornerRadius)
    {
        auto shape = face.translated(offsetX, offsetY);

        // 外へ広げるほど重なりが減り、縁が自然に薄くなる
        for (int i = layers; i >= 1; --i)
        {
            float grow = (float)i - 1.0f;

            g.setColour(GuiColor::Palette::ShadowGray.get().withMultipliedAlpha(1.0f / (float)layers));
            g.fillRoundedRectangle(shape.expanded(grow), cornerRadius + grow);
        }
    }
}

class ColoredGroupComponent : public juce::GroupComponent
{
    juce::Colour backgroundColor = GuiColor::Group::Bg;
public:
    // 見出しの寸法。JUCE の既定描画と同じ場所へ収まるようにしてあるので、
    // 各タブが確保している上部の余白はそのままでよい。
    static inline constexpr float titleHeight = 16.0f;
    static inline constexpr float titleFontHeight = 13.0f;
    static inline constexpr float titlePaddingX = 6.0f;
    static inline constexpr float titleInsetX = 8.0f;

    // 枠の上辺は見出しの中ほどを通す。角の丸みはカテゴリ見出しに合わせる。
    static inline constexpr float frameInset = 3.0f;
    static inline constexpr float cornerRadius = 5.0f;
    static inline constexpr float titleCornerRadius = 3.0f;

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
// カテゴリの中身に敷く板のうち、下端がまだ決まっていないものを閉じる。
// GuiCategoryLabel はこの下で定義されるので、ここでは前方宣言と関数だけ置く。
class GuiCategoryLabel;

// 各タブは中身の高さに末尾の余白を足してから欄の高さを決めるので、
// 最後の板はそのぶんを差し引いた位置で閉じる。
inline constexpr int categoryContentTrailingPadding = 20;

void closeCategoryBackdrops(juce::Component* parent, int bottom);

// ============================================================================
// ダイアログの見た目
// ============================================================================
// AlertWindow は JUCE が共有の LookAndFeel で描く。個々のウィンドウへ
// 色を設定しても、showMessageBoxAsync のように中でウィンドウを組み立てる
// ものには手が届かないので、共有の LookAndFeel 側へ入れる。
namespace GuiDialog
{
    // 起動時に 1 度だけ呼ぶ
    void applyTheme();

    // 自前で組み立てたダイアログのボタンを塗り分ける。
    // 決定と取り消しで色を変えるため、ウィンドウ単位で呼ぶ必要がある。
    void styleButtons(juce::AlertWindow& window);
}
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
        // スクロールバーを常に表示させ、常に幅を占有させる
        // (表示・非表示が切り替わるたびにレイアウト可能幅が変動し、中身が見切れるのを防ぐ)
        viewport.getVerticalScrollBar().setAutoHide(false);
        viewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, GuiColor::ScrollBar::Thumb);
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

    // レイアウトにもキャンバス幅にも使う「中身の幅」
    // スクロールバーの現在の表示状態に依存しないので、レイアウト結果が毎回同じになる
    int getContentWidth() const
    {
        return juce::jmax(0, viewport.getWidth() - viewport.getScrollBarThickness());
    }

    // 中身のキャンバスの「本当の高さ」をセットする関数
    // (これを呼ばないとスクロール範囲が設定されません)
    void setContentHeight(int totalHeight)
    {
        // 幅はViewportと同じにする（スクロールバーの幅を考慮）
        contentCanvas.setSize(getContentWidth(), totalHeight);

        // 欄の終わりなので、開きっぱなしのカテゴリ背景をここで閉じる。
        // 渡ってくる高さには末尾の余白が含まれているため、そのぶんを引く。
        closeCategoryBackdrops(&contentCanvas, totalHeight - categoryContentTrailingPadding);
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

    // 地色。透明なら塗らない。
    juce::Colour bgColor = juce::Colours::transparentBlack;

    struct Config {
        juce::Component& parent;
        juce::String title;
        std::optional<juce::Font> font = std::nullopt;
        juce::Justification justification = juce::Justification::centred;
        juce::Colour color = GuiColor::Label::Text;

        // 地色。既定は透明で、これまでどおり文字だけを出す。
        // 行の左に置くラベルだけ色を渡して帯にする。
        juce::Colour bgColor = juce::Colours::transparentBlack;
    };

    void setup(const Config& c);
    void paint(juce::Graphics& g) override;

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

    // =======================================================
    // 値の表示枠を他の部品と同じ丸みで描くための LookAndFeel
    // =======================================================
    class SliderLF : public juce::LookAndFeel_V4
    {
    public:
        // 帯の太さ。行の高さいっぱいに塗ると太くなりすぎる。
        static inline constexpr float barHeight = 10.0f;

        // FF1 のステータスバーを意識した形。つまみは置かず、
        // 枠の中を左から塗って現在値を示す。
        void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
            float sliderPos, float minSliderPos, float maxSliderPos,
            juce::Slider::SliderStyle style, juce::Slider& slider) override
        {
            // 横向き以外は使っていないが、来たときは JUCE の既定へ渡す
            if (style != juce::Slider::LinearHorizontal) {
                juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height,
                    sliderPos, minSliderPos, maxSliderPos, style, slider);

                return;
            }

            auto area = GuiShadow::reserve(juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height));
            auto bar = area.withSizeKeepingCentre(area.getWidth(), juce::jmin(area.getHeight(), barHeight));

            float alpha = slider.isEnabled() ? 1.0f : 0.5f;

            if (slider.isEnabled()) GuiShadow::drawRounded(g, bar, guiCornerRadius);

            g.setColour(GuiColor::Slider::Trough.get().withMultipliedAlpha(alpha));
            g.fillRoundedRectangle(bar, guiCornerRadius);

            // sliderPos は絶対座標で来る。左端からの塗り幅に直す。
            float filled = juce::jlimit(bar.getX(), bar.getRight(), sliderPos) - bar.getX();

            if (filled > 0.0f) {
                juce::Graphics::ScopedSaveState state(g);

                // 端の丸みを保ったまま塗り量だけを見せたいので、帯全体を
                // 描いて幅で切り取る。幅で角丸を描くと、値が小さいときに
                // 左端の丸みまで潰れてしまう。
                g.reduceClipRegion(juce::Rectangle<float>(bar.getX(), bar.getY(), filled, bar.getHeight()).toNearestInt());

                g.setColour(slider.findColour(juce::Slider::trackColourId).withMultipliedAlpha(alpha));
                g.fillRoundedRectangle(bar, guiCornerRadius);
            }

            g.setColour(GuiColor::Slider::Frame.get().withMultipliedAlpha(alpha));
            g.drawRoundedRectangle(bar.reduced(0.5f), guiCornerRadius, 1.0f);
        }

        // スライダーの値表示は内部の juce::Label が描いている。
        // JUCE の既定は角の立った四角なので、ここだけ差し替える。
        void drawLabel(juce::Graphics& g, juce::Label& label) override
        {
            auto bounds = label.getLocalBounds().toFloat();

            g.setColour(label.findColour(juce::Label::backgroundColourId));
            g.fillRoundedRectangle(bounds, guiCornerRadius);

            float alpha = label.isEnabled() ? 1.0f : 0.5f;

            // 編集中は JUCE 側が TextEditor を重ねるので、文字は描かない
            if (!label.isBeingEdited())
            {
                auto area = label.getBorderSize().subtractedFrom(label.getLocalBounds());

                g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
                g.setFont(label.getFont());
                g.drawFittedText(label.getText(), area, label.getJustificationType(),
                    juce::jmax(1, (int)((float)area.getHeight() / label.getFont().getHeight())),
                    label.getMinimumHorizontalScale());
            }

            g.setColour(label.findColour(juce::Label::outlineColourId).withMultipliedAlpha(alpha));
            g.drawRoundedRectangle(bounds.reduced(0.5f), guiCornerRadius, 1.0f);
        }
    };

    SliderLF customLF;
public:
    GuiSlider(const GuiContext& context) : GuiBaseComponent(context), label(context) {
        this->setLookAndFeel(&customLF);
    }

    ~GuiSlider() override
    {
        // メンバの customLF が壊れる前に必ず外す
        this->setLookAndFeel(nullptr);
    }

    // 束縛先のパラメータを差し替える。
    // 1組のスライダーで複数パラメータを切り替えて編集する用途で使う
    // (例: UNISON の対象ボイス切り替え)。
    void rebind(const juce::String& id)
    {
        // 必ず古い束縛を先に破棄すること。
        // unique_ptr::reset(p) は「新しいポインタを格納してから古い方を破棄」するため、
        // reset(new ...) と書くと新アタッチメントの初期値反映が
        // まだ生きている古いアタッチメント経由で切り替え前のパラメータへ書き戻されてしまう。
        att.reset();
        att.reset(new SliderAttachment(ctx.apvts, id, *this));
    }

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
            // ドロップダウンは PopupMenu が描く。コンボボックス本体へ色を
            // 設定しても伝わらないので、LookAndFeel 側で指定する。
            setColour(juce::PopupMenu::backgroundColourId, GuiColor::ComboBox::PopupBg);
            setColour(juce::PopupMenu::textColourId, GuiColor::ComboBox::Text);
            setColour(juce::PopupMenu::highlightedBackgroundColourId, GuiColor::ComboBox::PopupSelectedBg);
            setColour(juce::PopupMenu::highlightedTextColourId, GuiColor::ComboBox::Text);
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

        // 面を持つ部品なので、ボタンと同じ影と縁取りで描く。
        // JUCE の既定にも角丸はあるが、影を足すため自前で描く。
        void drawComboBox(juce::Graphics& g, int width, int height, bool,
            int, int, int, int, juce::ComboBox& box) override
        {
            auto face = GuiShadow::reserve(juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height));
            float alpha = box.isEnabled() ? 1.0f : 0.4f;

            if (box.isEnabled()) GuiShadow::drawRounded(g, face, guiCornerRadius);

            g.setColour(box.findColour(juce::ComboBox::backgroundColourId).withMultipliedAlpha(alpha));
            g.fillRoundedRectangle(face, guiCornerRadius);

            g.setColour(box.findColour(juce::ComboBox::outlineColourId).withMultipliedAlpha(alpha));
            g.drawRoundedRectangle(face.reduced(0.5f), guiCornerRadius, 1.0f);

            // 右端の矢印
            auto arrowArea = face.removeFromRight(face.getHeight()).reduced(face.getHeight() * 0.3f);

            juce::Path arrow;
            arrow.startNewSubPath(arrowArea.getX(), arrowArea.getCentreY() - arrowArea.getHeight() * 0.2f);
            arrow.lineTo(arrowArea.getCentreX(), arrowArea.getCentreY() + arrowArea.getHeight() * 0.3f);
            arrow.lineTo(arrowArea.getRight(), arrowArea.getCentreY() - arrowArea.getHeight() * 0.2f);

            g.setColour(box.findColour(juce::ComboBox::arrowColourId).withMultipliedAlpha(alpha));
            g.strokePath(arrow, juce::PathStrokeType(1.5f));
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
    float boxW = 12.0f; // 縮小しても視認しやすい四角のサイズ
    float boxH = 12.0f; // 縮小しても視認しやすい四角のサイズ
    float boxGapW = 2.0f;      // 四角と文字の隙間
    float boxGapH = 2.0f;      // 四角と文字の隙間
    float labelGapW = 6.0f;      // 四角と文字の隙間
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
	void setBoxSize(float w, float h) { boxW = w; boxH = h; }
	void setBoxGap(float w, float h) { boxGapW = w; boxGapH = h; }
    void setLabelGap(float w) { labelGapW = w; }
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
            auto face = GuiShadow::reserve(button.getLocalBounds().toFloat());

            if (button.isEnabled()) GuiShadow::drawRounded(g, face, guiCornerRadius);

            // 背景の塗りつぶし
            juce::Colour baseColour = backgroundColour.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.4f);

            // マウスホバー時やクリック時は少し明るくする
            if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
                baseColour = baseColour.brighter(0.5f);

            g.setColour(baseColour);
            g.fillRoundedRectangle(face, guiCornerRadius);

            // 縁取りはカテゴリ見出しのマーカーと同じ色。明るい面が背景へ
            // 溶けないよう、輪郭だけ引き締める。
            g.setColour(GuiColor::Outline.get().withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.4f));
            g.drawRoundedRectangle(face.reduced(0.5f), guiCornerRadius, 1.0f);
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

    // セルを自分で描きたいときに使う。true を返すと既定の文字描画を行わない。
    std::function<bool(juce::Graphics&, int row, int columnId, int width, int height, bool selected)> onPaintCell = nullptr;

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

// カテゴリを開いたときに、その中身の背後へ敷く半透明の黒。
// 見出しの下から次の見出し (または欄の終わり) までを覆う。
// 装飾なのでマウスは素通しにしてある。
class GuiCategoryBackdrop : public juce::Component
{
public:
    GuiCategoryBackdrop() { setInterceptsMouseClicks(false, false); }

    void paint(juce::Graphics& g) override
    {
        g.setColour(GuiColor::Category::ContentBg);
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.0f);
    }
};
class GuiCategoryLabel : public GuiLabel
{
    bool enableChangeDetailVisible = false;
    bool detailVisible = false;

    // 見出しの文字。開閉のマーカーと区切りはこのクラスが描くので、
    // 呼び出し側はタイトルだけを渡せばよい。
    juce::String captionText = juce::String();

    juce::Font font = juce::Font(juce::FontOptions(16.0f, juce::Font::bold));

    // 背景色。カテゴリの種類で決まる。
    juce::Colour bgColor = GuiColor::Category::HwBg;

    // 中身の背後へ敷く板。開いているときだけ見せる。
    GuiCategoryBackdrop backdrop;
public:
    GuiCategoryLabel(const GuiContext& context) : GuiLabel(context) {}

    struct Config {
        juce::Component& parent;
        juce::String title;
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

	// 背景色を呼び出し側が決める。同じ部品でも、置かれたチャンネルによって
	// ハードとソフトのどちらに見せたいかが変わる場合に使う。
	void setupCategory(const Config& c, juce::Colour bgColor);
	bool isDetailVisible() const { return this->detailVisible; }

    // 折りたたみを持たないカテゴリ (ALGORITHM/FEEDBACK など) は常に開いている扱い。
    bool isOpen() const { return this->detailVisible || !this->enableChangeDetailVisible; }

    void paint(juce::Graphics& g) override;

    // 見た目の寸法
    static inline constexpr float cornerRadius = 3.0f;

    // 中身を板の内側へ寄せる量。左右と上下に効く。
    static inline constexpr int contentPadding = 4;

    // 板の下端と、次の見出しとの間に空ける量。
    // ここを詰めすぎると板が下のカテゴリのものに見えてしまう。
    static inline constexpr int gapBelow = 6;


    // ---- 中身の背景 ----
    // 見出しを置いた直後に呼ぶと、そこから下を覆う板の上端が決まる。
    // 下端は次の見出しが置かれたとき、または欄の終わりで確定する。
    void beginBackdrop(const juce::Rectangle<int>& contentArea);

    // 同じ親に対して開きっぱなしの板があれば、指定の位置で閉じる。
    // 閉じる板があれば true を返す。中身を内側へ寄せていた幅を
    // 呼び出し側が元に戻せるようにするため。
    static bool closePending(juce::Component* parent, int bottom);
    static inline constexpr float paddingX = 4.0f;
    static inline constexpr float markerSize = 10.0f;
private:
    void setupInner(const Config& c, juce::Colour background);

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

        // 角の丸み。太さの半分を超える指定は端が欠けるので頭打ちにする。
        float cornerRadius = 2.0f;
    };

    GuiSeparator(const GuiContext& context) : GuiBaseComponent(context) {}

    void setup(const Config& c) {
        lineStyle = c.lineStyle;
        lineRate = c.lineRate;
        lineThick = c.lineThick;
        lineColour = c.lineColour;
        cornerRadius = c.cornerRadius;
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(lineColour); // 区切り線の色

        // 端を丸めた帯として描く。カテゴリ見出しや板と手触りを揃えるため。
        float radius = juce::jmin(cornerRadius, lineThick * 0.5f);

        if (lineStyle == Style::Horizontal) {
            // 水平線の描画 (中央の高さに引く)
            float realWidth = (float)getWidth();
            float width = realWidth * lineRate;
            float paddingX = (realWidth - width) / 2.0f;

            float y = ((float)getHeight() - lineThick) / 2.0f;

            g.fillRoundedRectangle(paddingX, y, width, lineThick, radius);
        }
        else {
            // 垂直線の描画
            float realHeight = (float)getHeight();
            float height = realHeight * lineRate;
            float paddingY = (realHeight - height) / 2.0f;

            float x = ((float)getWidth() - lineThick) / 2.0f;

            g.fillRoundedRectangle(x, paddingY, lineThick, height, radius);
        }
    }

private:
    Style lineStyle = Style::Horizontal;
    float lineRate = 100.0f;
    float lineThick = 1.0f;
    float cornerRadius = 2.0f;
    juce::Colour lineColour = juce::Colours::grey.withAlpha(0.5f);
};
