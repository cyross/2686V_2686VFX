#pragma once

#include <JuceHeader.h>
#include <array>
#include <functional>
#include <span>
#include <vector>
#include <atomic>
#include "GuiColor.h"
#include "GuiStructs.h"
#include "GuiConstants.h"
#include "GuiHelpers.h"
#include "LabelConstants.h"
#include "../fm/SliderRegMap.h"
#include "GuiContext.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

class CustomTabLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Path getIconPath(const juce::String& name, juce::Rectangle<float> area);
    void drawTabButton(juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override;
};

class ColoredGroupComponent : public juce::GroupComponent
{
    juce::Colour backgroundColor = GuiColor::Group::Bg;
public:
    void setBackgroundColor(juce::Colour c);
    void paint(juce::Graphics& g) override;
};

#if !defined(BUILD_AS_FX_PLUGIN)
class GuiWaveformPreview : public juce::Component
{
public:
    juce::Colour bgColor;
    juce::Colour lineColor;

    // コンストラクタで色を受け取る
    GuiWaveformPreview(juce::Colour background, juce::Colour line)
        : bgColor(background), lineColor(line)
    {
    }

    void pushBuffer(const float* data, int numSamples)
    {
        if (numSamples <= 0) return;
        m_displayBuffer.assign(data, data + numSamples);
        repaint(); // データが来たら再描画
    }

    void paint(juce::Graphics& g) override
    {
        // カスタム背景色で塗りつぶし
        g.fillAll(bgColor);
        g.setColour(juce::Colours::darkgrey);
        g.drawRect(getLocalBounds(), 1);

        g.setColour(juce::Colours::grey.withAlpha(0.5f));
        g.drawLine(0, getHeight() / 2.0f, getWidth(), getHeight() / 2.0f);

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
    }

private:
    std::vector<float> m_displayBuffer;
};
#endif

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

class GuiLabel : public juce::Label, public GuiBaseComponent
{
public:
    GuiLabel(const GuiContext& context) : GuiBaseComponent(context) {}

    struct Config {
        juce::Component& parent;
        juce::String title;
        std::optional<juce::Font> font = std::nullopt;
        juce::Justification justification = juce::Justification::centred;
        juce::Colour color = GuiColor::Label::Text;
    };

    void setup(const Config& c);
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
};

class GuiComboBox : public juce::ComboBox, public GuiBaseComponent
{
protected:
    std::unique_ptr<ComboBoxAttachment> att;
public:
    GuiComboBox(const GuiContext& context) : GuiBaseComponent(context), label(context) {}

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
    };

    void setup(const Config& c);
};

class GuiToggleButton : public juce::ToggleButton, public GuiBaseComponent
{
protected:
    std::unique_ptr<ButtonAttachment> att;
public:
    GuiToggleButton(const GuiContext& context) : GuiBaseComponent(context) {}

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
};

class GuiTextButton : public juce::TextButton, public GuiBaseComponent
{
protected:
    std::unique_ptr<ButtonAttachment> att;
public:
    GuiTextButton(const GuiContext& context) : GuiBaseComponent(context) {}

    struct Config {
        juce::Component& parent;
        juce::String id = "";
        juce::String title;
        juce::Colour textColor = GuiColor::TextButton::Text;
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
    void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& e) override;
    void selectedRowsChanged(int lastRowSelected) override;
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