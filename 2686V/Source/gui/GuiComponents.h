#pragma once

#include <JuceHeader.h>
#include <array>
#include <functional>
#include <span>
#include <vector>
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
public:
    void setBackgroundColor(juce::Colour c);
    void paint(juce::Graphics& g) override;

private:
    juce::Colour backgroundColor = juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 0.4f);
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
        juce::Colour color = juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
        juce::Colour bgColor = juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f);
        juce::Colour labelColor = juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
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
        juce::Colour trackColor = juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
        juce::Colour thumbColor = juce::Colour::fromFloatRGBA(0.03f, 0.03f, 0.7f, 1.0f);
        bool isReset = false;
        std::optional<juce::Font> labelFont = std::nullopt;
        juce::Justification labelJustification = juce::Justification::centred;
        juce::Colour labelColor = juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
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
        juce::Colour color = juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
        juce::Colour bgColor = juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f);
        bool isReset = true;
        bool isResized = false;
        std::optional<juce::Font> labelFont = std::nullopt;
        juce::Justification labelJustification = juce::Justification::centred;
        juce::Colour labelColor = juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
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
        juce::Colour color = juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
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
        juce::Colour textColor = juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
        juce::Colour bgColor = juce::Colour::fromFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f);
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
        juce::Colour color = juce::Colour::fromFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
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