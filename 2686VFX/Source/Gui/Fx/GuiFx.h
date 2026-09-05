#pragma once

#include <JuceHeader.h>

#include "../../Core/Io/ParamFile.h"
#include <array>
#include <vector>
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../Components/AmpEnv/AmpEnv.h"
#include "../Components/SsgHwEnv/SsgHwEnv.h"
#include "../Components/SsgHwPEnv/SsgHwPEnv.h"
#include "../Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../Components/LfoOpzx7/LfoOpzx7.h"
#include "../Components/PitchEnv/PitchEnv.h"
#include "../Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../Components/WtMod/WtMod.h"
#include "../Components/WtAmpMod/WtAmpMod.h"
#include "../Components/MulDetune/MulDetune.h"
#include "../Components/Unison/Unison.h"
#include "../Components/Quality/QualityPcm.h"
#include "../../Core/Gui/GuiContext.h"
#include "./GuiFxText.h"
#include "../../Effect/Fx/Fx.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"

class GuiFx : public GuiBase
{
    // 順番の設定は最初から開いておく。メインが縦長の 1 列になり、丈が足りる。
    bool isShowRoute = true;
    std::vector<int> order = { 0 };
    static inline const std::array<juce::String, NumEffects> effectNames = {
            juce::String("") + "フィルター",         // 0: FxType::Filter
            juce::String("") + "3バンドイコライザー", // 1: FxType::Eq3b
            juce::String("") + "トレモロ",           // 2: FxType::Tremolo
            juce::String("") + "ビブラート",         // 3: FxType::Vibrato
            juce::String("") + "ビットクラッシャー", // 4: FxType::ModernBitCrusher
            juce::String("") + "ディレイ",           // 5: FxType::Delay
            juce::String("") + "リバーブ",           // 6: FxType::Reverb
            juce::String("") + "SFCエコー",          // 7: FxType::SpcEcho
            juce::String("") + "PCMビットクラッシャー"  // 8: FxType::PcmBitCrusher
    };

    GuiGroup mainGroup;

    // メイン以外はここへ入れて、横へまとめてスクロールさせる。
    // 1 つの枠を上下いっぱいに使うので、縦には送らない。
    juce::Viewport stripViewport;
    juce::Component stripCanvas;

    // 作り終えた部品を、メインに残すもの以外まとめて移す。
    void moveToStrip();

    // 出力へ掛ける変調。音源ではチャンネルごとに持っていたものを、
    // ここでは出力に対して 1 組だけ持つ。
    //
    // 縦に積むと入りきらないので、機能ごとに列を分ける。列の中は
    // スクロールできるので、中身が伸びても見切れない。
    // 変調の入り切り。効果と同じくバイパスなので、エフェクターの枠へ
    // 一緒に置く。区切り線で本体のバイパスと分ける。
    NormalSeparator modSwitchSeparator;

    GuiToggleButton envBypassToggle;
    GuiToggleButton lfoBypassToggle;
    GuiToggleButton pitchBypassToggle;
    GuiToggleButton shiftBypassToggle;

    GuiScrollGroup modAmpEnvGroup;
    GuiScrollGroup modSsgHwEnvGroup;
    GuiScrollGroup modWtAmpModGroup;
    GuiScrollGroup modSsgSwEnv11Group;
    GuiScrollGroup modLfoGroup;
    GuiScrollGroup modPitchEnvGroup;
    GuiScrollGroup modSsgSwPEnv11Group;
    GuiScrollGroup modWtModGroup;
    GuiScrollGroup modSsgHwPEnvGroup;
    GuiScrollGroup modMulDetuneGroup;
    GuiScrollGroup modUnisonGroup;

    GuiComponentAmpEnv ampEnvComponent;
    GuiComponentSsgHwEnv ssgHwEnvComponent;
    GuiComponentWtAmpMod wtAmpModComponent;
    GuiComponentSsgSwEnv11 ssgSwEnv11Component;
    GuiComponentLfoOpzx7 lfoComponent;
    GuiComponentPitchEnv pitchEnvComponent;
    GuiComponentSsgSwPEnv11 ssgSwPEnv11Component;
    GuiComponentWtMod wtModComponent;
    GuiComponentSsgHwPEnv ssgHwPEnvComponent;
    GuiComponentMulDetune mulDetuneComponent;
    GuiComponentUnison unisonComponent;

    // WT PITCH MOD の速さは実機では搬送波との比。エフェクトには搬送波が
    // 無いので、その代わりの周波数をここで決められるようにしてある。
    GuiSlider wtModBaseFreqSlider;

    GuiGroup tremGroup;
    GuiGroup vibGroup;
    GuiGroup mbcGroup;
    GuiGroup delayGroup;
    GuiGroup reverbGroup;
    GuiGroup filterGroup;
    GuiGroup eq3bGroup;
    GuiGroup sfceGroup;

    // 2686V PCM Bit Crusher。実機のレートとビットの刻みで落とす。
    GuiGroup pcmGroup;

    GuiToggleButton bypassToggle;
    NormalSeparator mainSeparator;
    GuiTextButton resetBtn;
    NormalSeparator routeSeparator;
    GuiTextButton showRouteBtn;
    std::array<GuiLabel, NumEffects> routeFx;
    std::array<GuiTextButton, NumEffects> routeUp;
    std::array<GuiTextButton, NumEffects> routeDown;
    NormalSeparator fileSeparator;
    GuiTextButton importFxOrderBtn;
    GuiTextButton exportFxOrderBtn;
    GuiTextButton importFxParamBtn;
    GuiTextButton exportFxParamBtn;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // 以降、エフェクトごとの設定

    // トレモロ(Tremolo)
    GuiToggleButton tBypassBtn;
    NormalSeparator tSeparator;
    GuiSlider tRateSlider, tDepthSlider;
    GuiSlider tMixSlider;
    GuiTextButton tDryBtn, tHalfBtn, tWetBtn;

    // ビブラート(Vibrato)
    GuiToggleButton vBypassBtn;
    NormalSeparator vSeparator;
    GuiSlider vRateSlider, vDepthSlider;
    GuiSlider vMixSlider;
    GuiTextButton vDryBtn, vHalfBtn, vWetBtn;

    // ビットクラッシャー(Modern Bit Crusher)
    GuiToggleButton mbcBypassBtn;
    NormalSeparator mbcSeparator;
    GuiSlider mbcRateSlider, mbcBitsSlider;
    GuiSlider mbcMixSlider;
    GuiTextButton mbcDryBtn, mbcHalfBtn, mbcWetBtn;

    // ディレイ(Delay)
    GuiToggleButton dBypassBtn;
    NormalSeparator dSeparator;
    GuiSlider dTimeSlider, dFbSlider;
    GuiSlider dMixSlider;
    GuiTextButton dDryBtn, dHalfBtn, dWetBtn;

    // Reverb
    GuiToggleButton rBypassBtn;
    NormalSeparator rSeparator;
    GuiSlider rSizeSlider, rDampSlider;
    GuiSlider rMixSlider;
    GuiTextButton rDryBtn, rHalfBtn, rWetBtn;

    // Filter
    GuiToggleButton flBypassBtn;
    NormalSeparator flSeparator;
    GuiComboBox flTypeSelector;

    GuiToggleButton pcmBypassBtn;
    NormalSeparator pcmSeparator;
    GuiComboBox pcmBitSelector, pcmRateSelector, pcmInterpSelector;

    // ビットの一覧は QUALITY のものを借りるが、頭の 12 個だけを出す。
    // 13 以降は ADPCM などの圧縮で、曲の頭から順に符号化するもの。
    // 流れてくる音を塊ごとに切って符号化すると継ぎ目で音が飛ぶため、
    // ここでは扱わない。
    static std::vector<SelectItem>& getPcmBitItems();
    GuiSlider pcmMixSlider;
    GuiTextButton pcmDryBtn, pcmHalfBtn, pcmWetBtn;
    GuiSlider flFreqSlider;
    GuiSlider flQSlider;
    GuiSlider flMixSlider;
    GuiTextButton flDryBtn, flHalfBtn, flWetBtn;

    // 3Band EQ
    GuiToggleButton eq3bBypassBtn;
    NormalSeparator eq3bSeparator;
    GuiSlider eq3bLowGainDbSlider;
    GuiSlider eq3bMidFreqSlider;
    GuiSlider eq3bMidGainDbSlider;
    GuiSlider eq3bHighGainDbSlider;
    GuiSlider eq3bMixSlider;
    GuiTextButton eq3bDryBtn, eq3bHalfBtn, eq3bWetBtn;

    // SFC Echo
    GuiToggleButton sfceBypassBtn;
    NormalSeparator sfceSeparator;
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
    void updatePcmEnabled();
    void updateDelayEnabled();
    void updateReverbEnabled();
    void updateEq3bEnabled();
    void updateSfcEchoEnabled();
    void importFxOrder();

    // 3.0.0 より前の形式を読む
    void setImportingFxOrder(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeFxOrder(Io::ParamWriter& writer);
    void exportFxOrder();
    void importFxParam();

    // 3.0.0 より前の形式を読む
    void setImportingFxParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeFxParams(Io::ParamWriter& writer);
    void exportFxParam();
public:
    // FX のパラメータを初期値へ戻す。編集画面の INIT から呼ぶ。
    void initParams();
    GuiFx(const GuiContext& context);

    void setup() override;

    // 簡易表示モードで隠す区分への一括操作
    void bypassHiddenCategories() override;
    void openEnabledCategories() override;
    void closeBypassedCategories() override;
    void layout(juce::Rectangle<int> content) override;
    void layoutFxOrder(juce::Rectangle<int> rect);
    void updateFxOrder();
};
