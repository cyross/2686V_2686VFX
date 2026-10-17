#include "./EnvSsgSw256.h"

#include <cmath>

void SsgSwPEnv256::prepare(int targetIndex, double sampleRate)
{
    env.prepare(targetIndex, (int)CurveParams::Target::SsgSwPEnv256, sampleRate);
}

float SsgSwPEnv256::process(float phaseDelta)
{
    if (env.isBypass()) return phaseDelta;

    // リリースを走り終えたあと。ENDL を触っていなければ、これまでどおり
    // 素通しする。触っているときだけ、その音程を保つ。
    if (env.isIdle()) {
        if (!env.usesEndl()) return phaseDelta;

        return phaseDelta * std::pow(2.0f, env.endLevel() / 1200.0f);
    }

    const float cents = env.process();

    // 1200セント = 1オクターブ (2倍の周波数)
    if (cents != 0.0f) phaseDelta *= std::pow(2.0f, cents / 1200.0f);

    return phaseDelta;
}
