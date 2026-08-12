#pragma once

#include <JuceHeader.h>
#include <array>
#include <functional>
#include <vector>

#include "./AlgMatrixState.h"

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
class GuiOpzx7AlgMatrix : public juce::Component {
public:
    GuiOpzx7AlgMatrix();
    void resized() override;
    void paint(juce::Graphics& g) override;

    std::array<std::unique_ptr<juce::ToggleButton>, 8> carrierBtns;
    std::array<std::array<std::unique_ptr<juce::ToggleButton>, 8>, 8> modBtns;

    // 状態が変更された際に呼ばれるコールバック
    std::function<void(const AlgMatrixState&)> onMatrixChanged;

    void updateValidity();
    AlgMatrixState getState() const;
    void setState(const AlgMatrixState& s);
};
