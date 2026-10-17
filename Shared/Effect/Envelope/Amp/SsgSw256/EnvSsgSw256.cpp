#include "./EnvSsgSw256.h"

void SsgSwEnv256::prepare(int targetIndex, double sampleRate)
{
    env.prepare(targetIndex, (int)CurveParams::Target::SsgSwEnv256, sampleRate);
}

float SsgSwEnv256::process()
{
    if (env.isBypass()) return 1.0f; // バイパス時は音量1.0(影響なし)を返す

    return env.process();
}
