#pragma once

#include "../StepEnv/StepEnv.h"

// SSG SW AMP ENV[256]。256 段のエンベロープの区分 (GuiComponentStepEnv) を、
// 音量の鍵と範囲で組んだもの。
class GuiComponentSsgSwEnv256 : public GuiComponentStepEnv {
public:
    GuiComponentSsgSwEnv256(const GuiContext& context) : GuiComponentStepEnv(context, spec()) {}

    static const Spec& spec();
};
