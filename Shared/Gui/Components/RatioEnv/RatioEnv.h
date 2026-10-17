#pragma once

#include "../StepEnv/StepEnv.h"

// ============================================================================
// SSG の波形を動かす 256 段のエンベロープ
// ============================================================================
// DUTY RATIO ENV : 矩形波のデューティ比 (%)。「矩形波(デューティ比)設定」の一番下
// PEAK RATIO ENV : 三角波の頂点 (0〜1)。「三角波設定」の一番下
// TL ENV         : FM 音源のオペレーターの TL。[[AMP ENV]] の一番下
//
// どちらも SSG SW AMP ENV[256] と同じ作り (GuiComponentStepEnv) で、
// 値の名前と範囲、値を置くボタンだけが違う。
class GuiComponentDutyRatioEnv : public GuiComponentStepEnv {
public:
    GuiComponentDutyRatioEnv(const GuiContext& context) : GuiComponentStepEnv(context, spec()) {}

    static const Spec& spec();
};

class GuiComponentPeakRatioEnv : public GuiComponentStepEnv {
public:
    GuiComponentPeakRatioEnv(const GuiContext& context) : GuiComponentStepEnv(context, spec()) {}

    static const Spec& spec();
};

// TL ENV。FM 音源のオペレーターの TL を段ごとに置き換える。値は TL のレジスタ値で、
// 幅は音源で違う (OPNA / OPN / OPM は 0〜127、OPL / OPL3 / OPZX7 は 0〜63)。
class GuiComponentTlEnv : public GuiComponentStepEnv {
public:
    GuiComponentTlEnv(const GuiContext& context, int tlMax) : GuiComponentStepEnv(context, spec(tlMax)) {}

    static const Spec& spec(int tlMax);
};
