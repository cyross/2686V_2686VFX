#include "./Params.h"

class FixMode
{
    bool enable = false;
    float freq = 440.0f;
public:
    void setParameters(const FixModeParams& params);
};
