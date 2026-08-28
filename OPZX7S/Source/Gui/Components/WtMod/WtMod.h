#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Gui/Components/ParamBarEditor/ParamBarEditor.h"
#include "../WavePreview/WavePreview.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Generator/Fds/GenFdsModTable.h"

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
// ==========================================================
// FDS(2C33) の変調テーブルエディタ
// ==========================================================
// 実機のテーブルは 32 エントリ・各 3bit で、持っているのは波形そのもの
// ではなく「カウンタへの増減値」。3bit 値と増減量の対応は飛び飛びで、
// レジスタの並び順(0..7)のまま棒グラフにしても意味を成さないため、
// 増減量の順に並べ替えて表示・編集する。
//   表示の下から  5(-4) 6(-2) 7(-1) 0(+0) 1(+1) 2(+2) 3(+4)
// 4 はカウンタのリセットで増減量の軸に乗らないので、右クリックで
// 単独に立てる扱いにしてある。
// 下段には、そのテーブルを 1 周ぶん積算した実際の階段波を出す。
class FdsTableEditor : public ParamBarEditorBase
{
    void updateSliderValue(const juce::MouseEvent& e) override;
    void updateHoverState(const juce::MouseEvent& e) override;
public:
    FdsTableEditor(const GuiContext& context) : ParamBarEditorBase(context) {}

    void setup(juce::Component& parent, const juce::String& idPrefix);
    void loadTable(const std::array<int, 32>& table);
    std::array<int, 32> currentTable() const;

    void paint(juce::Graphics& g) override;
    void paintOverChildren(juce::Graphics& g) override;

    // 右クリックでリセットエントリを立てるため、左クリック限定の
    // 土台の受け口ではなく自前で受ける。
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
};

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

    // 選んでいる Shape の変調波形を見せるプレビュー
    GuiWavePreview modPreview;

    // HuC6280 モードの変調波形 (32 サンプル)
    std::array<juce::RangedAudioParameter*, 32> waveParams = { nullptr };

    // FdsUser モードの変調テーブル
    GuiCategoryLabel fdsCat;
    FdsTableEditor fdsEditor;
    std::array<GuiTextButton, FdsMod::tableCount> fdsPresetBtn;
    // 読み込んだ変調波形ファイルのパス。実体はタブごとにプロセッサが持つ。
    juce::String* p_wavePath = nullptr;

    std::unique_ptr<juce::FileChooser> fileChooser;

    void importWave(bool isWt2);
    bool applyWaveFile(const juce::File& file, bool isWt2);
    void reapplyWaveFile();
    void clearWave();
    void updateWaveFileName(const juce::String& fileName);
    void updateModPreview();
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
        waveSmoothBtn(context),
        modPreview(context),
        fdsCat(context),
        fdsEditor(context),
        fdsPresetBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) }
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

    // 単体ファイル (.wtmod) での入出力。Enable / Depth / Speed / Shape /
    // Smooth と、FdsUser 用の 32 エントリをまとめて扱う。
    void importParams();
    void exportParams();
    void setImportingParams(juce::StringArray& lines, int& index);
    juce::String getExportedParams();
};
