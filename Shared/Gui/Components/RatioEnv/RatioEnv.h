#pragma once

#include "../StepEnv/StepEnv.h"
#include "Shared/Core/Processor/ProcessorParamMod.h"

// ============================================================================
// SSG の波形を動かす 256 段のエンベロープ
// ============================================================================
// DUTY RATIO ENV : 矩形波のデューティ比 (%)。「矩形波(デューティ比)設定」の一番下
// PEAK RATIO ENV : 三角波の頂点 (0〜1)。「三角波設定」の一番下
// ALG ENV        : FM 音源のチャンネルの ALG。FEEDBACK の下
// PARAM ENV[256] : 選んだパラメータ (4.3.0)。4.0.0 の TL ENV はこの指し先 TL になった
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

// ALG ENV。FM 音源のチャンネルの ALG を段ごとに置き換える。値は ALG の番号 (0 から)。
// ALGORITHM/FEEDBACK の FEEDBACK の下に置く。
class GuiComponentAlgEnv : public GuiComponentStepEnv {
public:
    GuiComponentAlgEnv(const GuiContext& context, int algorithms) : GuiComponentStepEnv(context, spec(algorithms)) {}

    static const Spec& spec(int algorithms);
};

// PARAM ENV[256] (4.3.0)。選んだパラメータ (TARGET) を段ごとに動かす。値は指し先の
// パラメータの 0〜1 で持ち、画面には指し先の書き方で出す。
//
//   オペレーター : FM 音源のオペレーターごと。TARGET はオペレーターの指し先で、初期値は TL。
//                  4.0.0 の TL ENV の代わりで、TL ENV のファイルも読める (TL の最大値で割る)
//   チャンネル   : チャンネルに 1 本。TARGET はチャンネルの指し先 (RHYTHM はパッドの指し先も)
class GuiComponentParamEnv : public GuiComponentStepEnv {
public:
    GuiComponentParamEnv(const GuiContext& context, const Spec& s) : GuiComponentStepEnv(context, s) {}

    // tlMax は TL の最大値 (OPNA / OPN / OPM は 127、OPL / OPL3 / OPZX7 は 63)
    static Spec opSpec(const ParamModCatalog::Catalog& catalog, float tlMax);

    // ops はオペレーター (パッド) の数
    static Spec channelSpec(const ParamModCatalog::Catalog& catalog, int ops);
};
