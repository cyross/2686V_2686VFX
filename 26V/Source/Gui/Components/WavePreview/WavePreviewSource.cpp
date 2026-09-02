#include "./WavePreviewSource.h"

#include <algorithm>
#include <cstdint>
#include "../../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../../Generator/WtMod/GenWtModulator.h"
#include "../../../Effect/Lfo/Opzx7/LfoOpzx7Unit.h"
#include "../../../Effect/Lfo/N88/LfoN88.h"

#include <cmath>

namespace
{
    // ノイズや Sample & Hold は値が周期単位でしか変わらないので、1 周期だけ
    // 描くと横一直線になってしまう。
    //
    // このとき Shape ごとに「段が 8 つ見えるまで」窓を伸ばすと、保持の長さが
    // 4 でも 64 でも同じ見た目になり、違いが消えてしまう。段のある波形は
    // 時間軸を揃えて描き、保持の長い Shape ほど段が長く見えるようにする。
    constexpr int steppyPeriods = WavePreviewSource::points;

    // 段のある波形は 1 周期を細かく描く必要がない。周期数を稼ぐ側に回す。
    constexpr int steppySamplesPerPeriod = 32;

    // 全体が points 点に収まるよう、何サンプルおきに拾うか
    inline int decimation(int totalSamples)
    {
        return (totalSamples > WavePreviewSource::points) ? (totalSamples / WavePreviewSource::points) : 1;
    }
}

namespace
{
    // LFO の速さは固定にして、窓の長さはサンプリングレート側で変える。
    // 位相の進みを速くして窓を広げると、1 サンプルあたりの位相が大きく
    // なりすぎて周期の区切りを取りこぼす。またスムースの時定数は秒で
    // 効くので、速さを変えると滑らかさの見え方まで変わってしまう。
    constexpr float previewLfoFreq = 8.0f;

    // Sync Delay = 1 は「遅延なしで位相を頭出し」。毎回同じ形にするため。
    constexpr int previewSyncDelay = 1;

    struct LfoWindow
    {
        int samplesPerPeriod = WavePreviewSource::points;
        int totalSamples = WavePreviewSource::points;
        double rate = 0.0;
    };

    // steppy は「値が周期単位でしか変わらない波形か」。
    LfoWindow makeLfoWindow(bool steppy)
    {
        LfoWindow w;

        w.samplesPerPeriod = steppy ? steppySamplesPerPeriod : WavePreviewSource::points;
        w.totalSamples = w.samplesPerPeriod * (steppy ? steppyPeriods : 1);
        w.rate = (double)previewLfoFreq * w.samplesPerPeriod;

        return w;
    }
}

// ============================================================================
// SSG HW ENV
// ============================================================================
namespace
{
    bool isSsgHwSampleHold(int shapeIndex)
    {
        switch ((SsgHwShape)shapeIndex) {
        case SsgHwShape::SampleHold:
        case SsgHwShape::SampleHold4:
        case SsgHwShape::SampleHold8:
        case SsgHwShape::SampleHold16:
        case SsgHwShape::SampleHold32:
        case SsgHwShape::SampleHold64:
            return true;
        default:
            return false;
        }
    }
}

std::vector<float> WavePreviewSource::ssgHwEnv(int shapeIndex, float minLevel, float maxLevel, bool smooth)
{
    const bool steppy = isSsgHwSampleHold(shapeIndex);

    // 1 周期は位相 2.0 ぶん。Sample & Hold の 1 サイクルはその半分なので、
    // サイクルを points 個並べるには 1 周期を細かく描かない方がよい。
    const int samplesPerPeriod = steppy ? steppySamplesPerPeriod : points;
    const int totalSamples = steppy ? (samplesPerPeriod / 2 * steppyPeriods) : samplesPerPeriod;

    // エンベロープの速さは固定にして、窓の長さはサンプリングレートで変える。
    // こうするとスムースの時定数が周期に対して常に同じ比率で効くので、
    // 窓を広げても滑らかさの見え方が変わらない。
    constexpr float previewFreq = 375.0f;
    const double previewRate = (double)previewFreq * samplesPerPeriod / 2.0;

    SsgHwEnvParams p;

    p.enable = true;
    p.shape = shapeIndex;
    p.period = previewFreq;
    p.min = minLevel;
    p.max = maxLevel;
    p.smooth = smooth;

    SsgHwEnv env;

    env.prepare(previewRate);
    env.setParameters(p);
    env.noteOn();

    const int step = decimation(totalSamples);

    std::vector<float> out;
    out.reserve((size_t)(totalSamples / step) + 1);

    for (int i = 0; i < totalSamples; ++i) {
        float v = env.process();

        if ((i % step) == 0) out.push_back(v);
    }

    return out;
}

// ============================================================================
// WT MODULATION
// ============================================================================
std::vector<float> WavePreviewSource::wtMod(int shapeIndex, const std::array<float, 32>& wave, const std::array<int, 32>& fdsTable)
{
    WtModParams p;

    p.enable = true;

    // Depth を実際の設定で回すと、浅いときに形が読み取れない。
    // ここで見せたいのは Shape の形なので、深さは最大で回す。
    p.depth = 1.0f;

    // Speed は搬送波に対する比率。1.0 にすると搬送波 1 周 = 変調 1 周になる。
    p.speed = 1.0f;
    p.shape = shapeIndex;
    p.wave = wave;
    p.fdsTable = fdsTable;

    WtModulator mod;

    mod.setParameters(p);
    mod.reset();

    // 変調 1 周ぶん
    const float delta = 1.0f / (float)points;

    std::vector<float> out;
    out.reserve((size_t)points);

    float peak = 0.0f;

    for (int i = 0; i < points; ++i) {
        // 返ってくるのは搬送波の周波数に掛ける比。比のままだと上げ側と
        // 下げ側で振れ幅が揃わないので、オクターブ (log2) に直して見る。
        // WonderSwan のスイープは比が 0 に近づくため、下は頭打ちにする。
        float ratio = std::max(mod.process(delta), 1.0f / 16.0f);
        float octaves = std::log2(ratio);

        out.push_back(octaves);
        peak = std::max(peak, std::fabs(octaves));
    }

    // いちばん振れたところが上下いっぱいに来るよう揃える。
    // まったく振れない (波形未読込の HuC6280 など) ときは 0 のまま。
    if (peak > 1.0e-6f) {
        for (auto& v : out) v /= peak;
    }

    return out;
}

// ============================================================================
// OPZX7 LFO
// ============================================================================
namespace
{
    // 5 は毎周期引き直すノイズ、8〜12 は 4/8/16/32/64 周期ごとの Sample & Hold。
    // どれも値が周期単位でしか変わらない。
    bool isOpzx7Steppy(int shapeIndex)
    {
        return shapeIndex == 5 || shapeIndex >= 8;
    }

    std::vector<float> runOpzx7(int shapeIndex, bool isAm, float smoothRate)
    {
        const LfoWindow w = makeLfoWindow(isOpzx7Steppy(shapeIndex));

        Opzx7LfoCoreUnit unit;

        unit.prepare(w.rate);
        unit.setParameters(previewSyncDelay, true, previewLfoFreq, shapeIndex, 1.0f, 1.0f, smoothRate);
        unit.noteOn();

        const int step = decimation(w.totalSamples);

        std::vector<float> out;
        out.reserve((size_t)(w.totalSamples / step) + 1);

        for (int i = 0; i < w.totalSamples; ++i) {
            float v = isAm ? unit.getSampleAm() : unit.getSamplePm();

            if ((i % step) == 0) out.push_back(v);
        }

        return out;
    }
}

std::vector<float> WavePreviewSource::opzx7LfoPm(int shapeIndex)
{
    return runOpzx7(shapeIndex, false, 1.0f);
}

std::vector<float> WavePreviewSource::opzx7LfoAm(int shapeIndex, float smoothRate)
{
    return runOpzx7(shapeIndex, true, smoothRate);
}

// ============================================================================
// N88 LFO
// ============================================================================
namespace
{
    std::vector<float> runN88(int shapeIndex, bool isAm, float smoothRate)
    {
        // 3 は毎周期引き直すノイズ。それ以外に段のある波形は無い。
        const LfoWindow w = makeLfoWindow(shapeIndex == 3);

        // PM と AM のどちらも有効にしておく。片方だけだと計算そのものが
        // 省かれてしまうため。
        LfoN88Params p;

        p.syncDelay = previewSyncDelay;
        p.pm = true;
        p.am = true;
        p.pmIndex = shapeIndex;
        p.amIndex = shapeIndex;
        p.pmFreq = previewLfoFreq;
        p.amFreq = previewLfoFreq;
        p.amSmoothRate = smoothRate;

        N88LfoCore core;

        core.prepare(w.rate);
        core.setParameters(p);
        core.noteOn();

        const int step = decimation(w.totalSamples);

        std::vector<float> out;
        out.reserve((size_t)(w.totalSamples / step) + 1);

        for (int i = 0; i < w.totalSamples; ++i) {
            core.getSample();

            if ((i % step) == 0) out.push_back(isAm ? core.value.am : core.value.pm);
        }

        return out;
    }
}

std::vector<float> WavePreviewSource::n88LfoPm(int shapeIndex)
{
    return runN88(shapeIndex, false, 1.0f);
}

std::vector<float> WavePreviewSource::n88LfoAm(int shapeIndex, float smoothRate)
{
    return runN88(shapeIndex, true, smoothRate);
}
