#pragma once
#include <JuceHeader.h>
#include <vector>
#include <functional>

class GuiEnvelopeGraph : public juce::Component
{
public:
    GuiEnvelopeGraph();

    void paint(juce::Graphics& g) override;

    enum class EnvType { Amp, Pitch, SsgSw };

    struct PhaseDef {
        float widthPx;
        float startLevel;
        float endLevel;
        bool isDashed = false;
        juce::Colour color = juce::Colours::cyan;
        std::function<float(float)> curveFunc = [](float x) { return x; };

        // カーブのどの部分を切り出すか（0.0 〜 1.0）
        float curveStartX = 0.0f;
        float curveEndX = 1.0f;

        // 線の描画を前の終点から繋げず、指定位置から新しく開始するための設定
        bool moveToStart = false;
        float startXOffsetPx = 0.0f; // グラフ左端からの相対X座標 (moveToStart が true の場合のみ使用)
        bool drawPhaseLine = true;
        juce::Colour phaseLineColor = juce::Colours::white;
    };

    void setEnvelope(EnvType type, const juce::String& title, const std::vector<PhaseDef>& phases);

private:
    EnvType currentType = EnvType::Amp;
    juce::String currentTitle;
    std::vector<PhaseDef> currentPhases;
};