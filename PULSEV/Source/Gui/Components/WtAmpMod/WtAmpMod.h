#pragma once

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"
#include <array>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Gui/Components/ParamBarEditor/ParamBarEditor.h"
#include "../WavePreview/WavePreview.h"
#include "../WavePreview/WavePreviewGrid.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Generator/Fds/GenFdsModTable.h"
#include "../WtMod/WtMod.h"
#include "../NudgeSlider/NudgeSliderFloat.h"
#include "../NudgeButtons/NudgeButtons.h"

// ==========================================================
// WT AMP MOD
// ==========================================================
// WT PITCH MOD (Gui/Components/WtMod) と同じ変調波形を、周波数ではなく
// 音量へ当てるもの。実機の波形メモリ音源が変調をピッチへ掛けるのに対して、
// こちらはこのプラグインの追加になる。
//
// 出力は MIN〜MAX の間を動く。MIN / MAX は波形スロットをまたいで 1 組だけ
// 持ち、どのスロット・どの Shape を選んでも同じ範囲で振れる。
//
// 変調テーブルの編集は WT PITCH MOD と同じ FdsTableEditor を使い回す。
// 変調波形スロットはピッチ版と別に持てるよう、プロセッサ側の置き場所を
// 引く鍵に CPK::WtAmpMod::waveStoreSuffix を付けてある。
class GuiComponentWtAmpMod : public GuiBase {
    GuiCategoryLabel cat;
    GuiToggleButton enableButton;
    GuiSlider depthSlider;
    GuiSlider speedSlider;
    GuiComboBox shapeSelector;
    GuiToggleButton waveSmoothBtn;

    // 出力の下端と上端。ナッジボタンを添える。
    GuiComponentNudgeSliderFloat minSlider;
    GuiComponentNudgeButtons minNudge;
    GuiComponentNudgeSliderFloat maxSlider;
    GuiComponentNudgeButtons maxNudge;

    // どのスロットを使うか。演奏中に切り替えられるようパラメータにしてある。
    GuiSlider waveSlotSlider;

    // 変調波形スロット。読み込み行とプレビューを枚数ぶん持つ。
    // スロットごとに読み込みボタンを並べる代わりに、対象を選ぶつまみと
    // 1 組のボタンを置く。波形はまとめて 1 つの区画へ描く。
    GuiSlider slotTarget;
    GuiTextButton slotWtBtn;
    GuiTextButton slotWt2Btn;
    GuiTextButton slotClearBtn;
    GuiLabel slotFileNameLabel;
    GuiWavePreviewGrid slotPreviews;

    // 選んでいる Shape の変調のかかり方を見せるプレビュー
    GuiWavePreview modPreview;

    // APVTS のプレフィックス。つまみの id を組み立てるのに使う。
    juce::String m_code;

    // 変調波形の置き場所を引く鍵。ピッチ版と分けるため尻尾が付く。
    juce::String m_waveKey;

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

    // 対象のスロットが変わったときに、ボタンと名前の指す先をそろえる。
    void applySlotTarget();

    // いま読み込み・クリアの対象になっているスロット。
    int targetSlot() const { return juce::jlimit(0, Global::WtMod::slots - 1, (int)slotTarget.getValue()); }
    void updateSlotPreview(int slot);

    // 今どのスロットを使っているか。Shape のプレビューが参照する。
    int currentSlot() const;
    juce::String wavePath(int slot) const;
    void updateModPreview();
public:

    // 簡易表示モードで丸ごと隠す。見出しごと消え、縦の場所も取らない。
    //
    // 見出しを見せるかどうかはレイアウト側では戻らない (あちらは場所を
    // 決めるだけ) ので、ここで両方向とも面倒を見る。
    void setCategoryVisible(bool visible) {
        cat.setHidden(!visible);
        cat.setVisible(visible);
    }

    // 簡易表示モードの一括操作で使う口。
    //
    // 区分によって「バイパス」だったり「有効」だったりするので、
    // ここで意味を揃えて「切ってあるか」で答える。
    bool hasBypassSwitch() const { return true; }

    bool isCategoryBypassed() const { return !enableButton.getToggleState(); }

    void setCategoryBypassed(bool bypassed) {
        enableButton.setToggleState(!bypassed, juce::sendNotification);
    }

    // 見出しの開閉
    void setCategoryOpen(bool open) { cat.setDetailVisible(open); }
    GuiComponentWtAmpMod(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        enableButton(context),
        depthSlider(context),
        speedSlider(context),
        shapeSelector(context),
        waveSmoothBtn(context),
        minSlider(context),
        minNudge(context),
        maxSlider(context),
        maxNudge(context),
        waveSlotSlider(context),
        slotTarget(context),
        slotWtBtn(context),
        slotWt2Btn(context),
        slotClearBtn(context),
        slotFileNameLabel(context),
        slotPreviews(context),
        modPreview(context),
        fdsCat(context),
        fdsEditor(context),
        fdsPresetBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) }
    {
    }

    // MIN と MAX が互いを押すときの、入れ子呼び出しを弾くための印
    bool isClampingRange = false;

    // categoryBg は見出しの背景色。実機由来ではなくこちらの追加分なので、
    // 既定はソフトウェア区分の色を使う。
    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder,
        juce::Colour categoryBg = GuiColor::Category::SwAmpBg);
    void layoutComponent(juce::Rectangle<int>& rect);

    // 単体ファイル (.wtampmod) での入出力。
    void importParams();
    void exportParams();

    // 3.1.0 で足したものなので、3.0.0 より前の並び順のファイルには入っていない。
    void setImportingParams(juce::StringArray& lines, int& index);

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();
};
