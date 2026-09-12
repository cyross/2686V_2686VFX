#pragma once

#include <JuceHeader.h>
#include <functional>
#include <optional>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Io/ParamFile.h"
#include "../../../Core/Synth/WaveHold.h"
#include "../CountButtons/CountButtons.h"
#include "../NudgeButtons/NudgeButtons.h"
#include "../NudgeSlider/NudgeSliderFloat.h"
#include "../NudgeSlider/NudgeSliderInt.h"
#include "../PitchButtons/PitchButtons.h"
#include "../Separator/NormalSeparator.h"

// ==========================================================
// ホールドと部分再生
// ==========================================================
// 波形を周期で回すものに共通で付くひと組。WT の変調 (ピッチ / 音量)、
// SSG HW ENV (ピッチ / 音量)、LFO、そして波形メモリと SSG のチャンネルで
// 同じものを使う。並びは次のとおり。
//
//   ─────────── (区切り線)
//   HOLD                入り切り
//   COUNT               1〜3000 (2 段の刻みボタン付き)
//   TARGET              MIN / MAX のどちらで止まるか
//   HOLD MIN / HOLD MAX 保つ値
//   ─────────── (区切り線)
//   KEEP                部分再生の入り切り
//   START               鳴らし始める位置 (0.0〜1.0)
//   KEEP START          START より手前で端の値を保つか
//   END                 鳴らし終える位置 (START より手前へは行かない)
//   KEEP END            END より後ろで端の値を保つか
//
// 保つ値の単位は当て先で変わる。音量へ掛けるものは倍率 (0.0〜1.0)、
// 音程へ効くものはセント (±4800)。刻みボタンもそれに合わせて変える。
class GuiComponentWaveHold : public GuiBase {
    // 保つ値の単位。セントのときだけ刻みボタンが変わる。
    WaveHoldUnit m_unit = WaveHoldUnit::Level;

    // 親の区分が開いているか。setEnables で受け取って覚えておく。
    bool m_enabled = false;

    // 波形の側がすでに止まる振る舞いを持っていないか。
    // 持っている形 (SSG HW ENV の Saw Down & Hold など) では
    // 後付けのホールドを重ねても意味がないので、閉じておく。
    bool m_holdAvailable = true;

    // --- ホールド ---
    NormalSeparator holdSeparator;
    GuiToggleButton holdEnableBtn;
    GuiComponentNudgeSliderInt holdCount;
    GuiComponentCountButtons holdCountButtons;
    GuiComboBox holdTarget;
    GuiComponentNudgeSliderFloat holdMin;
    GuiComponentNudgeSliderFloat holdMax;

    // 刻みボタンは単位で使い分ける。使わない方は親へ付けないので、
    // 置いてあるだけで場所も描画も取らない。
    GuiComponentNudgeButtons holdMinNudge;
    GuiComponentNudgeButtons holdMaxNudge;
    GuiComponentPitchButtons holdMinPitch;
    GuiComponentPitchButtons holdMaxPitch;

    // --- 部分再生 ---
    NormalSeparator keepSeparator;
    GuiToggleButton keepEnableBtn;
    GuiComponentNudgeSliderFloat waveStart;
    GuiComponentNudgeButtons waveStartNudge;
    GuiToggleButton keepStartBtn;
    GuiComponentNudgeSliderFloat waveEnd;
    GuiComponentNudgeButtons waveEndNudge;
    GuiToggleButton keepEndBtn;

    // START と END が互いを押すときの、入れ子呼び出しを弾くための印
    bool isClampingWindow = false;

    // 形が変わったときに呼ぶ先 (波形プレビューの引き直しなど)
    std::function<void()> m_onChanged;

    void notifyChanged() { if (m_onChanged) m_onChanged(); }

    void applyEnables();
public:
    GuiComponentWaveHold(const GuiContext& context) :
        GuiBase(context),
        holdSeparator(context),
        holdEnableBtn(context),
        holdCount(context),
        holdCountButtons(context),
        holdTarget(context),
        holdMin(context),
        holdMax(context),
        holdMinNudge(context),
        holdMaxNudge(context),
        holdMinPitch(context),
        holdMaxPitch(context),
        keepSeparator(context),
        keepEnableBtn(context),
        waveStart(context),
        waveStartNudge(context),
        keepStartBtn(context),
        waveEnd(context),
        waveEndNudge(context),
        keepEndBtn(context)
    {
    }

    // idPrefix は「チャンネルの印 + 当て先の印」まで組み立てたもの。
    // 例: code + CPK::WtMod::holdPrefix。尻尾は CPK::WaveHold が付ける。
    void setupComponent(juce::Component& parent, const juce::String& idPrefix, int& tabOrder,
        WaveHoldUnit unit, std::optional<std::function<void()>> onChanged = std::nullopt);

    void setVisibles(bool visible);

    // 場所を取る側の区分に合わせて使い分ける。
    // layoutComponent は MainGroup、layoutComponentRow は ParamGroup 向け。
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);

    // 親の区分が切ってあるときは丸ごと閉じる。
    // 中は HOLD / KEEP それぞれの入り切りでさらに閉じる。
    void setEnables(bool enabled);

    // 選んでいる波形がすでに止まる形かどうかを渡す。
    // 偽を渡すとホールドの一式が閉じる (部分再生はそのまま使える)。
    void setHoldAvailable(bool available);

    void readParams(const Io::ParamReader& reader);
    void writeParams(Io::ParamWriter& writer);

    // 今の設定。波形プレビューを引き直すときに渡す。
    WaveHoldParams getParams();
};
