#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"

// ==========================================================
// WT MODULATION
// ==========================================================
// WT / WT2 / WT+ の 3 タブが同じ内容を持っていたので 1 つにまとめた。
// タブごとに違うのは APVTS のプレフィックス(code)と、変調波形ファイルの
// パスをどのメンバへ覚えるかの 2 点だけなので、その 2 つを setupComponent
// で受け取る。
//
// 変調方式は 3 系統ある (詳細は SynthWt.cpp 側のコメントを参照)。
//   Sine / FDS ... ファミコンディスクシステム(2C33)。周波数そのものを動かす
//   WS Sweep  ... WonderSwan ch3。周波数分周器を動かす
//   HuC6280   ... PC Engine。別チャンネルの波形メモリ 32 サンプルで分周器を動かす
class GuiComponentWtMod : public GuiBase {
    GuiCategoryLabel cat;
    GuiToggleButton enableButton;
    GuiSlider depthSlider;
    GuiSlider speedSlider;
    GuiComboBox shapeSelector;
    GuiTextButton waveWtBtn;
    GuiTextButton waveWt2Btn;
    GuiTextButton waveClearBtn;
    GuiLabel waveFileNameLabel;
    GuiToggleButton waveSmoothBtn;

    // HuC6280 モードの変調波形 (32 サンプル)
    std::array<juce::RangedAudioParameter*, 32> waveParams = { nullptr };

    // 読み込んだ変調波形ファイルのパス。実体はタブごとにプロセッサが持つ。
    juce::String* p_wavePath = nullptr;

    void importWave(bool isWt2);
    bool applyWaveFile(const juce::File& file, bool isWt2);
    void clearWave();
    void updateWaveFileName(const juce::String& fileName);
public:
    GuiComponentWtMod(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        enableButton(context),
        depthSlider(context),
        speedSlider(context),
        shapeSelector(context),
        waveWtBtn(context),
        waveWt2Btn(context),
        waveClearBtn(context),
        waveFileNameLabel(context),
        waveSmoothBtn(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, juce::String& wavePath);
    void layoutComponent(juce::Rectangle<int>& rect);

    // CH パラメータの入出力。
    // Enable / Depth / Speed はファイルの前寄りに、Shape は後から足した
    // パラメータなので旧フォーマットとの互換のため末尾に置いてある。
    // その並びを崩さないよう、あえて 2 組に分けている。
    void setImportingBaseParams(juce::StringArray& lines, int& index);
    juce::String getExportedBaseParams();
    void setImportingShapeParam(juce::StringArray& lines, int& index);
    juce::String getExportedShapeParam();
};
