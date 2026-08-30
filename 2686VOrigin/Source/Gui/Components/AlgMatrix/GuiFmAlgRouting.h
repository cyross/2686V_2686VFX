#pragma once
#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"
#include <array>
#include <vector>
#include <functional>

#include "./FmAlgState.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiComponents.h"

// ==============================================================================
// アルゴリズムを可視化する汎用グラフコンポーネント
// ==============================================================================
class GuiFmAlgGraph : public juce::Component {
public:
    FmAlgState state;
    void paint(juce::Graphics& g) override;
    void updateState(const FmAlgState& s) {
        state = s;
        repaint();
    }
};

// ==============================================================================
// NxN マトリックス入力汎用コンポーネント
// ==============================================================================
class GuiFmAlgMatrix : public juce::Component, public GuiBaseComponent {
public:
    int numOps;
    GuiFmAlgMatrix(const GuiContext& context, int ops = 8);
    void resized() override;
    void paint(juce::Graphics& g) override;

    std::vector<std::unique_ptr<GuiToggleButton>> carrierBtns;
    std::vector<std::vector<std::unique_ptr<GuiToggleButton>>> modBtns;
    std::vector<std::vector<std::unique_ptr<GuiToggleButton>>> fbBtns;

    std::function<void(const FmAlgState&)> onMatrixChanged;

    void updateValidity();
    FmAlgState getState() const;
    void setState(const FmAlgState& s);
    void setImportingParams(juce::StringArray& lines, int& index);

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();

private:
    std::vector<bool> m_opReachable;
    int totalW, modChkStartY, modTotalH, fbStartY, fbChkStartY, fbTotalH;
};
