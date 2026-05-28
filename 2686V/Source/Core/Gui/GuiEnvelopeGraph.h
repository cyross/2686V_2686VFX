#pragma once
#include <JuceHeader.h>
#include <vector>
#include <functional>

#include "./GuiComponents.h"
#include "../../Advanced/Curve/Core.h"

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
    void updatePitchEnv(
        const GuiSlider& pitchAttackSlider,
        const GuiSlider& pitchDecaySlider,
        const GuiSlider& pitchReleaseSlider,
        const GuiSlider& pitchStartLevelSlider,
        const GuiSlider& pitchAttackLevelSlider,
        const GuiSlider& pitchSustainLevelSlider,
        const GuiSlider& pitchReleaseLevelSlider,
        CurveCore* p_curveCore,
        bool isCurveMode,
        int posIdx
    );
    void updateSsgSwEnv(
        const GuiSlider& ssgSwStepsSlider,
        const GuiToggleButton& ssgSwEnvLoopButton,
        const GuiSlider& ssgSwLoopToSlider,
        const GuiSlider& ssgSwLoopCountSlider,
        const std::array<juce::Slider*, 7>& rSl,
        const std::array<juce::Slider*, 7>& lSl,
        CurveCore* p_curveCore,
        bool isCurveMode,
        int posIdx
    );
    void updateAmpEnv(
        const GuiSlider& startLevelSlider,
        const GuiSlider& attackSlider,
        const GuiSlider& decaySlider,
        const GuiSlider& sustainSlider,
        const GuiSlider& releaseSlider,
        CurveCore* p_curveCore,
        bool isCurveMode,
        int posIdx
    );
private:
    EnvType currentType = EnvType::Amp;
    juce::String currentTitle;
    std::vector<PhaseDef> currentPhases;
};