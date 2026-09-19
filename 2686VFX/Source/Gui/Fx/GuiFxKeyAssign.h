#pragma once

#include <JuceHeader.h>

#include <array>
#include <functional>
#include <set>

#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Io/ParamFile.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Processor/Mod/ProcessorModKeys.h"

// ============================================================================
// 変調を動かす鍵盤の割り当て (キーアサイン)
// ============================================================================
// 「シングルキーアサイン」はこれまでどおり、どの鍵盤でも全部が動く。
// 「キーアサインのカスタマイズ」を選ぶと、対象ごとの鍵盤の一覧を出す。
//
// エフェクターの枠の中、ファイルの読み書きの上に置く。
class GuiFxKeyAssign : public GuiBase
{
    NormalSeparator separator;
    GuiComboBox mode;
    std::array<GuiComboBox, ModPrKey::KeyAssign::NumTargets> keys;

public:
    GuiFxKeyAssign(const GuiContext& context);

    // 選び直したときに並べ直してもらう。一覧が出たり消えたりするため。
    std::function<void()> onModeChanged;

    void setup(juce::Component& parent, int& tabOrder);

    // 区切り線・キーアサイン・(カスタマイズのときは) 一覧を上から積む
    void layout(juce::Rectangle<int>& rect);

    bool isCustom() const;

    // 自分の部品を並べる。エフェクターの枠に残すものとして数えてもらうため。
    void collectComponents(std::set<juce::Component*>& out);

    // FX のパラメータファイルの "keyAssign" のまとまり。
    //
    // 値はパラメータから直に読み書きする。画面の部品は束縛で追随する。
    static void writeParams(juce::AudioProcessorValueTreeState& apvts, Io::ParamWriter& writer);
    static void readParams(juce::AudioProcessorValueTreeState& apvts, const Io::ParamReader& reader);
};
