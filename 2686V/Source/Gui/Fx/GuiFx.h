#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "./GuiFxText.h"
#include "../../Effect/Fx/Fx.h"

class GuiFx : public GuiBase
{
    bool isShowRoute = false;
    std::vector<int> order = { 0 };
    static inline const std::array<juce::String, NumEffects> effectNames = {
            juce::String("") + "フィルター",         // 0: FxType::Filter
            juce::String("") + "3バンドイコライザー", // 1: FxType::Eq3b
            juce::String("") + "トレモロ",           // 2: FxType::Tremolo
            juce::String("") + "ビブラート",         // 3: FxType::Vibrato
            juce::String("") + "ビットクラッシャー", // 4: FxType::ModernBitCrusher
            juce::String("") + "ディレイ",           // 5: FxType::Delay
            juce::String("") + "リバーブ",           // 6: FxType::Reverb
            juce::String("") + "SFCエコー"           // 7: FxType::SpcEcho
    };

    GuiGroup mainGroup;

    GuiGroup tremGroup;
    GuiGroup vibGroup;
    GuiGroup mbcGroup;
    GuiGroup delayGroup;
    GuiGroup reverbGroup;
    GuiGroup filterGroup;
    GuiGroup eq3bGroup;
    GuiGroup sfceGroup;

    GuiToggleButton bypassToggle;
    GuiSeparator mainSeparator;
    GuiTextButton resetBtn;
    GuiSeparator routeSeparator;
    GuiTextButton showRouteBtn;
    std::array<GuiLabel, NumEffects> routeFx;
    std::array<GuiTextButton, NumEffects> routeUp;
    std::array<GuiTextButton, NumEffects> routeDown;
    GuiSeparator fileSeparator;
    GuiTextButton importFxOrderBtn;
    GuiTextButton exportFxOrderBtn;
    GuiTextButton importFxParamBtn;
    GuiTextButton exportFxParamBtn;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // 以降、エフェクトごとの設定

    // トレモロ(Tremolo)
    GuiToggleButton tBypassBtn;
    GuiSeparator tSeparator;
    GuiSlider tRateSlider, tDepthSlider;
    GuiSlider tMixSlider;
    GuiTextButton tDryBtn, tHalfBtn, tWetBtn;

    // ビブラート(Vibrato)
    GuiToggleButton vBypassBtn;
    GuiSeparator vSeparator;
    GuiSlider vRateSlider, vDepthSlider;
    GuiSlider vMixSlider;
    GuiTextButton vDryBtn, vHalfBtn, vWetBtn;

    // ビットクラッシャー(Modern Bit Crusher)
    GuiToggleButton mbcBypassBtn;
    GuiSeparator mbcSeparator;
    GuiSlider mbcRateSlider, mbcBitsSlider;
    GuiSlider mbcMixSlider;
    GuiTextButton mbcDryBtn, mbcHalfBtn, mbcWetBtn;

    // ディレイ(Delay)
    GuiToggleButton dBypassBtn;
    GuiSeparator dSeparator;
    GuiSlider dTimeSlider, dFbSlider;
    GuiSlider dMixSlider;
    GuiTextButton dDryBtn, dHalfBtn, dWetBtn;

    // Reverb
    GuiToggleButton rBypassBtn;
    GuiSeparator rSeparator;
    GuiSlider rSizeSlider, rDampSlider;
    GuiSlider rMixSlider;
    GuiTextButton rDryBtn, rHalfBtn, rWetBtn;

    // Filter
    GuiToggleButton flBypassBtn;
    GuiSeparator flSeparator;
    GuiComboBox flTypeSelector;
    GuiSlider flFreqSlider;
    GuiSlider flQSlider;
    GuiSlider flMixSlider;
    GuiTextButton flDryBtn, flHalfBtn, flWetBtn;

    // 3Band EQ
    GuiToggleButton eq3bBypassBtn;
    GuiSeparator eq3bSeparator;
    GuiSlider eq3bLowGainDbSlider;
    GuiSlider eq3bMidFreqSlider;
    GuiSlider eq3bMidGainDbSlider;
    GuiSlider eq3bHighGainDbSlider;
    GuiSlider eq3bMixSlider;
    GuiTextButton eq3bDryBtn, eq3bHalfBtn, eq3bWetBtn;

    // SFC Echo
    GuiToggleButton sfceBypassBtn;
    GuiSeparator sfceSeparator;
    GuiSlider sfceTimeSlider, sfceFbSlider;
    GuiSlider sfceFirCoef0Slider, sfceFirCoef1Slider;
    GuiSlider sfceFirCoef2Slider, sfceFirCoef3Slider;
    GuiSlider sfceFirCoef4Slider, sfceFirCoef5Slider;
    GuiSlider sfceFirCoef6Slider, sfceFirCoef7Slider;
    GuiSlider sfceMixSlider;
    GuiTextButton sfceDryBtn, sfceHalfBtn, sfceWetBtn;

    void updateFilterEnabled();
    void updateTremoloEnabled();
    void updateVibratoEnabled();
    void updateMBCEnabled();
    void updateDelayEnabled();
    void updateReverbEnabled();
    void updateEq3bEnabled();
    void updateSfcEchoEnabled();
    void importFxOrder();
    void exportFxOrder();
    void importFxParam();
    void exportFxParam();
public:
    GuiFx(const GuiContext& context);

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void layoutFxOrder(juce::Rectangle<int> rect);
    void updateFxOrder();
};
