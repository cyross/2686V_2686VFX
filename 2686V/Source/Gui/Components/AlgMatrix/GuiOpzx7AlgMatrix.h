#pragma once

#include <JuceHeader.h>
#include <array>
#include <functional>
#include <vector>

#include "./AlgMatrixState.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Processor/Opzx7/ProcessorOpzx7Values.h"

// ==============================================================================
// アルゴリズムを可視化するグラフコンポーネント
// ==============================================================================
class GuiOpzx7AlgGraph : public juce::Component {
public:
    AlgMatrixState state;
    void paint(juce::Graphics& g) override;
    void updateState(const AlgMatrixState& s) {
        state = s;
        repaint();
    }
};

// ==============================================================================
// 8x8 マトリックス入力コンポーネント
// ==============================================================================
class GuiOpzx7AlgMatrix : public juce::Component, public GuiBaseComponent {
public:
    GuiOpzx7AlgMatrix(const GuiContext& context);
    void resized() override;
    void paint(juce::Graphics& g) override;

    std::array<std::unique_ptr<GuiToggleButton>, Opzx7PrValue::ops> carrierBtns;
    std::array<std::array<std::unique_ptr<GuiToggleButton>, Opzx7PrValue::ops>, Opzx7PrValue::ops> modBtns;
    std::array<std::array<std::unique_ptr<GuiToggleButton>, Opzx7PrValue::ops>, Opzx7PrValue::ops> fbBtns;

    std::function<void(const AlgMatrixState&)> onMatrixChanged;

    void updateValidity();
    AlgMatrixState getState() const;
    void setState(const AlgMatrixState& s);
    void setImportingParams(juce::StringArray& lines, int& index);
    juce::String getExportedParams();
private:
    // 描画時にグレーアウトさせるための到達可能フラグ
    std::array<bool, Opzx7PrValue::ops> m_opReachable{ false };
};
