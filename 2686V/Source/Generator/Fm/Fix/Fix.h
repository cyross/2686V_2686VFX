#pragma once

#include "FixModeParams.h"

class FixMode : public FixModeParams
{
    bool enable = false;
    float freq = 440.0f;
public:
    void setParameters(const FixModeParams& params);
};
