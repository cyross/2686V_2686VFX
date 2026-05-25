#pragma once
#include <JuceHeader.h>
#include <vector>
#include "../../Core/Gui/GuiComponents.h"

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

private:
    juce::String title;
    int currentLogic = 0;
    std::vector<GuiSlider*> boundSliders;
    GuiSlider* boundKSlider = nullptr; // 追加: Kの値を追跡
    int draggingHandleIndex = -1;

    bool enabled = false;

    struct HandleDef { int xIndex; int yIndex; juce::Colour color; };
    std::vector<HandleDef> getActiveHandles() const;

    juce::Point<float> getPixelFromNorm(float normX, float normY) const;
    juce::Point<float> getNormFromPixel(float px, float py) const;

    float evaluateCurve(float x) const;
};
