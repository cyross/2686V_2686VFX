#pragma once
#include <JuceHeader.h>
#include <vector>
#include <functional>

#include "./GuiComponents.h"
#include "./GuiColor.h"
#include "../../Advanced/Curve/AdvancedCurve.h"

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
        juce::Colour color = GuiColor::EnvelopeGraph::AmpLine;
        std::function<float(float)> curveFunc = [](float x) { return x; };

        // カーブのどの部分を切り出すか（0.0 〜 1.0）
        float curveStartX = 0.0f;
        float curveEndX = 1.0f;

        // 線の描画を前の終点から繋げず、指定位置から新しく開始するための設定
        bool moveToStart = false;
        float startXOffsetPx = 0.0f; // グラフ左端からの相対X座標 (moveToStart が true の場合のみ使用)
        bool drawPhaseLine = true;
        juce::Colour phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine;
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
        // つまみを段の数だけ置かなくなったので、値と上限だけを受け取る。
        // 並びは元のつまみ配列と同じ (R 側の [0] は使わない)。
        const std::array<float, 7>& rVal, float rMax,
        const std::array<float, 7>& lVal, float lMax,
        CurveCore* p_curveCore,
        bool isCurveMode,
        int posIdx
    );
    void updateSsgSwEnv11(
        const GuiSlider& ssgSwStepsSlider,
        const GuiToggleButton& ssgSwEnvLoopButton,
        const GuiSlider& ssgSwLoopToSlider,
        const GuiSlider& ssgSwLoopCountSlider,
        // つまみを段の数だけ置かなくなったので、値と上限だけを受け取る。
        // 並びは元のつまみ配列と同じ (R 側の [0] は使わない)。
        const std::array<float, 12>& rVal, float rMax,
        const std::array<float, 12>& lVal, float lMax,
        CurveCore* p_curveCore,
        bool isCurveMode,
        int posIdx
    );
    void updateSsgSwPEnv11(
        const GuiSlider& ssgSwStepsSlider,
        const GuiToggleButton& ssgSwEnvLoopButton,
        const GuiSlider& ssgSwLoopToSlider,
        const GuiSlider& ssgSwLoopCountSlider,
        // つまみを段の数だけ置かなくなったので、値と上限だけを受け取る。
        // 並びは元のつまみ配列と同じ (R 側の [0] は使わない)。
        const std::array<float, 12>& rVal, float rMax,
        const std::array<float, 12>& lVal, float lMax,
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
        const GuiToggleButton& korButton,
        CurveCore* p_curveCore,
        bool isCurveMode,
        int posIdx
    );
    void updateBypass(bool bypass) { this->isBypass = bypass; }
private:
    EnvType currentType = EnvType::Amp;
    juce::String currentTitle;
    std::vector<PhaseDef> currentPhases;
    bool isBypass;
};