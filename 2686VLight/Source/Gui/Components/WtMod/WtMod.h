#pragma once

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"
#include <array>

#include "../../../Core/Const/ConstGlobal.h"
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

    // 下段の階段波を描く色。どの変調へ付いているかで変える。
    // 実体は SETTINGS から差し替えられるので、色そのものではなく登録を指す。
    const GuiColor::Entry* stepColourEntry = &GuiColor::WavePreview::PitchEnv;

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
    GuiToggleButton waveSmoothBtn;

    // どのスロットを使うか。演奏中に切り替えられるようパラメータにしてある。
    GuiSlider waveSlotSlider;

    // 変調波形スロット。読み込み行とプレビューを枚数ぶん持つ。
    std::array<GuiTextButton, Global::WtMod::slots> slotWtBtn;
    std::array<GuiTextButton, Global::WtMod::slots> slotWt2Btn;
    std::array<GuiTextButton, Global::WtMod::slots> slotClearBtn;
    std::array<GuiLabel, Global::WtMod::slots> slotFileNameLabel;
    std::array<GuiWavePreview, Global::WtMod::slots> slotPreview;

    // 選んでいる Shape の変調波形を見せるプレビュー
    GuiWavePreview modPreview;

    // どのチャンネルの持ち分かを覚えておく。
    // 変調波形の実データはプロセッサが持っており、これが引き当ての鍵になる。
    juce::String m_code;

    // FdsUser モードの変調テーブル
    GuiCategoryLabel fdsCat;
    FdsTableEditor fdsEditor;
    std::array<GuiTextButton, FdsMod::tableCount> fdsPresetBtn;

    std::unique_ptr<juce::FileChooser> fileChooser;

    // 波形の読み書きはプロセッサ側で行う。ここは指示と表示だけ。
    void importWave(int slot, bool isWt2);
    void clearWave(int slot);

    // Smooth は読み込み時の落とし方を決めるので、切り替えたら取り込み直す
    void reapplyWaveFiles();

    void updateSlotFileName(int slot);
    void updateSlotPreview(int slot);

    // 今どのスロットを使っているか。Shape のプレビューが参照する。
    int currentSlot() const;
    juce::String wavePath(int slot) const;
    void updateModPreview();
public:

    // 簡易表示モードで丸ごと隠す。見出しごと消え、縦の場所も取らない。
    void setCategoryVisible(bool visible) { cat.setHidden(!visible); }
    GuiComponentWtMod(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        enableButton(context),
        depthSlider(context),
        speedSlider(context),
        shapeSelector(context),
        waveSmoothBtn(context),
        waveSlotSlider(context),
        slotWtBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        slotWt2Btn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        slotClearBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        slotFileNameLabel{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
        slotPreview{ GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context), GuiWavePreview(context) },
        modPreview(context),
        fdsCat(context),
        fdsEditor(context),
        fdsPresetBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) }
    {
    }

    // categoryBg は見出しの背景色。WT / WT2 / WT+ チャンネル自身の機能なので、
    // そこでは HwBg、他チャンネルへ借りて置く場合は既定の SwBg を使う。
    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder,
        juce::Colour categoryBg = GuiColor::Category::SwPitchBg);
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
    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();
};
