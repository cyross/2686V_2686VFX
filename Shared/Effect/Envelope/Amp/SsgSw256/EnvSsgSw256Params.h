#pragma once

#include "../../Step/EnvStep.h"

// SSG SW AMP ENV[256] の値。段の R / L は表 (StepTable) が持つので、
// ここには頭の値 (STEPS・LOOP・STL・LR・RR など) だけがある。
class SsgSwEnv256Params : public StepEnvParams {};
