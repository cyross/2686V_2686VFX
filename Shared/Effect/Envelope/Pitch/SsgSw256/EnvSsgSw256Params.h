#pragma once

#include "../../Step/EnvStep.h"

// SSG SW PITCH ENV[256] の値。段の値はセント。段の R / L は表 (StepTable) が
// 持つので、ここには頭の値 (STEPS・LOOP・STL・LR・RR など) だけがある。
class SsgSwPEnv256Params : public StepEnvParams {};
