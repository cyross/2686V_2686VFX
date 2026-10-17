#pragma once

#include "../StepEnv/StepEnv.h"

// SSG SW PITCH ENV[256]。256 段のエンベロープの区分 (GuiComponentStepEnv) を、
// 音程 (セント)の鍵と範囲で組んだもの。
class GuiComponentSsgSwPEnv256 : public GuiComponentStepEnv {
public:
    GuiComponentSsgSwPEnv256(const GuiContext& context) : GuiComponentStepEnv(context, spec()) {}

    static const Spec& spec();
};
