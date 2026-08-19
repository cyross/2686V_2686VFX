#pragma once
#include <JuceHeader.h>
#include <vector>
#include "../../Core/Gui/GuiComponents.h"
#include "../../Processor/Curve/ProcessorCurveValues.h"

class GuiCurveGraph : public juce::Component
{
public:
    GuiCurveGraph();

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    // kSlider を追加
    void bindActiveParameters(const juce::String& graphTitle, int logicIndex, std::vector<GuiSlider*> activeSliders, GuiSlider* kSlider);
    void setEnabled(bool enabled);

    std::function<void()> onGraphDragEnd = nullptr;
private:
    juce::String title;
    int currentLogic = 0;
    std::vector<GuiSlider*> boundSliders;
    GuiSlider* boundKSlider = nullptr; // Kの値を追跡
    int draggingHandleIndex = -1;

    bool enabled = true;

    // --- 動的スケーリング用変数 ---
    float displayMinX = 0.0f;
    float displayMaxX = 1.0f;
    float displayMinY = 0.0f;
    float displayMaxY = 1.0f;

    void updateDisplayRange();

    // --- 高速化: paint内でのヒープアロケーションを防ぐための固定長リスト ---
    struct HandleDef { int xIndex; int yIndex; juce::Colour color; };
    struct HandleList {
        std::array<HandleDef, CurvePrValue::values> items;
        int count = 0;
        void push_back(HandleDef h) { if (count < CurvePrValue::values) items[count++] = h; }
        const HandleDef* begin() const { return items.data(); }
        const HandleDef* end() const { return items.data() + count; }
        size_t size() const { return count; }
        const HandleDef& operator[](int i) const { return items[i]; }
    };

    HandleList getActiveHandles() const;

    juce::Point<float> getPixelFromNorm(float normX, float normY) const;
    juce::Point<float> getNormFromPixel(float px, float py) const;

    float evaluateCurve(float x) const;
};
