#pragma once
#include <JuceHeader.h>
#include <vector>
#include <functional>

#include "./GuiComponents.h"

class GuiEnvelopeGraph : public juce::Component
{
public:
    GuiEnvelopeGraph();

    void paint(juce::Graphics& g) override;

    enum class EnvType { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };

    struct PhaseDef {
        float widthPx;
        float startLevel;
        float endLevel;
        bool isDashed = false;
        juce::Colour color = juce::Colours::cyan;

        // 線の描画を前の終点から繋げず、指定位置から新しく開始するための設定
        bool moveToStart = false;
        float startXOffsetPx = 0.0f; // グラフ左端からの相対X座標 (moveToStart が true の場合のみ使用)
        bool drawPhaseLine = true;
        juce::Colour phaseLineColor = juce::Colours::white;
        bool isMax = false; // 値が最大値 = 縦の直線
    };

    void setEnvelope(EnvType type, const juce::String& title, const std::vector<PhaseDef>& phases);
    void updatePitchEnv(
        const GuiSlider& pitchAttackSlider,
        const GuiSlider& pitchDecaySlider,
        const GuiSlider& pitchReleaseSlider,
        const GuiSlider& pitchStartLevelSlider,
        const GuiSlider& pitchAttackLevelSlider,
        const GuiSlider& pitchSustainLevelSlider,
        const GuiSlider& pitchReleaseLevelSlider
    );
    void updateSsgSwEnv(
        const GuiSlider& ssgSwStepsSlider,
        const GuiToggleButton& ssgSwEnvLoopButton,
        const GuiSlider& ssgSwLoopToSlider,
        const GuiSlider& ssgSwLoopCountSlider,
        const std::array<juce::Slider*, 7>& rSl,
        const std::array<juce::Slider*, 7>& lSl
    );
    void updateSsgSwEnv11(
        const GuiSlider& ssgSwStepsSlider,
        const GuiToggleButton& ssgSwEnvLoopButton,
        const GuiSlider& ssgSwLoopToSlider,
        const GuiSlider& ssgSwLoopCountSlider,
        const std::array<juce::Slider*, 12>& rSl,
        const std::array<juce::Slider*, 12>& lSl
    );
    void updateSsgSwPEnv11(
        const GuiSlider& ssgSwStepsSlider,
        const GuiToggleButton& ssgSwEnvLoopButton,
        const GuiSlider& ssgSwLoopToSlider,
        const GuiSlider& ssgSwLoopCountSlider,
        const std::array<juce::Slider*, 12>& rSl,
        const std::array<juce::Slider*, 12>& lSl
    );
    void updateAmpEnv(
        const GuiSlider& startLevelSlider,
        const GuiSlider& attackSlider,
        const GuiSlider& decaySlider,
        const GuiSlider& sustainSlider,
        const GuiSlider& releaseSlider,
        const GuiToggleButton& korButton
    );
    void updateBypass(bool bypass) { this->isBypass = bypass; }
private:
    EnvType currentType = EnvType::Amp;
    juce::String currentTitle;
    std::vector<PhaseDef> currentPhases;
    bool isBypass;
};