#include <algorithm>
#include <cstring>

#include "./GenPcmHelper.h"

#include "../../../Core/Synth/SynthHelpers.h"
#include "../Adpcm/GenAdpcm.h"
#include "../Dpcm/GenDpcm.h"
#include "../Brr/GenBrr.h"
#include "../Vag/GenVag.h"
#include "../Ima/GenIma.h"
#include "../Arcade/GenArcadeAdpcm.h"

// ADPCM(DPCM)特有の無音時ノイズ（ピー音）を打ち消すローパスフィルタ
void GenPcmHelper::lowPassFilter(std::vector<int16_t>& buffer)
{
    if (buffer.size() > 1) {
        int16_t prev = buffer[0];
        for (size_t i = 1; i < buffer.size(); ++i) {
            int16_t current = buffer[i];
            buffer[i] = (int16_t)(((int32_t)current + (int32_t)prev) / 2);
            prev = current;
        }
    }
}

float GenPcmHelper::bitReduction(float input, int qIndex)
{
    float maxVal = getTargetMaxVal(qIndex);

    if (maxVal > 0.0f) {
        float dither = (pcmDis(pcmGen) - 0.5f) * (1.0f / maxVal);

        // ディザを足してから丸めることで、無音にならずにノイズとして残る
        return std::round((input + dither) * maxVal) / maxVal;
    }

    return input;
}

bool GenPcmHelper::isEncodedMode(int qIndex)
{
    return qIndex >= PcmCodecMode::first && qIndex <= PcmCodecMode::last;
}

namespace {
    // 同じ素材・同じ設定なら結果は必ず同じになる。
    // 全ボイスが同じエンコードを繰り返すと音声スレッドが止まってしまうため、
    // 直近の結果を使い回す。スレッドごとに持つので排他は不要。
    struct EncodeCacheEntry {
        uint64_t key = 0;
        bool valid = false;
        std::vector<int16_t> data;
    };

    constexpr int encodeCacheSize = 4;

    thread_local EncodeCacheEntry g_encodeCache[encodeCacheSize];
    thread_local int g_encodeCacheNext = 0;

    // 素材と設定から鍵を作る (FNV-1a)
    uint64_t makeEncodeKey(const std::vector<float>& source, double step, int qIndex, bool cleanResample)
    {
        uint64_t h = 1469598103934665603ull;

        auto mix = [&h](uint64_t v) {
            h ^= v;
            h *= 1099511628211ull;
            };

        mix((uint64_t)source.size());
        mix((uint64_t)qIndex);
        mix((uint64_t)(cleanResample ? 1 : 0));

        // step は倍精度なのでビット列のまま混ぜる
        uint64_t stepBits = 0;
        std::memcpy(&stepBits, &step, sizeof(stepBits));
        mix(stepBits);

        // 32bit 単位で回して、素材そのものの違いも見る
        const uint32_t* words = reinterpret_cast<const uint32_t*>(source.data());
        for (size_t i = 0; i < source.size(); ++i) mix(words[i]);

        return h;
    }
}

void GenPcmHelper::resampleClean(const std::vector<float>& source, double step, std::vector<int16_t>& dest)
{
    dest.clear();

    if (source.empty()) return;
    if (step < 1.0) step = 1.0;

    // 目的のレートのナイキストの少し手前で切る (素材の 1 標本あたりの周期)
    const double cutoff = 0.5 / step * 0.92;

    // 片側に取る零点の数。多いほど切れがよくなるが、そのぶん重い。
    constexpr int zeroCrossings = 8;

    // sinc × Blackman 窓を、零点 1 つあたり 512 刻みの表にしておく。
    // 位置ごとに三角関数を呼ぶと、長い素材で作り直しが目に見えて遅くなる。
    constexpr int tableRes = 512;
    constexpr int tableSize = zeroCrossings * tableRes + 1;

    static const std::vector<double> table = [] {
        std::vector<double> t((size_t)tableSize);

        constexpr double pi = 3.14159265358979323846;

        for (int i = 0; i < tableSize; ++i) {
            const double u = (double)i / tableRes;          // 零点の単位
            const double sinc = (u == 0.0) ? 1.0 : std::sin(pi * u) / (pi * u);
            const double w = 0.42 + 0.5 * std::cos(pi * u / zeroCrossings)
                           + 0.08 * std::cos(2.0 * pi * u / zeroCrossings);

            t[(size_t)i] = sinc * w;
        }

        return t;
    }();

    // 素材の標本の単位で、片側の幅
    const double half = zeroCrossings / (2.0 * cutoff);
    const int n = (int)source.size();

    dest.reserve((size_t)((double)n / step) + 1);

    for (double pos = 0.0; pos < (double)n; pos += step) {
        const int first = std::max(0, (int)std::ceil(pos - half));
        const int last = std::min(n - 1, (int)std::floor(pos + half));

        double acc = 0.0;
        double weightSum = 0.0;

        for (int k = first; k <= last; ++k) {
            // 零点の単位での距離
            const double u = std::abs((double)k - pos) * 2.0 * cutoff;

            if (u >= zeroCrossings) continue;

            const double fi = u * tableRes;
            const int i0 = (int)fi;
            const double frac = fi - i0;
            const double w = table[(size_t)i0] + (table[(size_t)std::min(i0 + 1, tableSize - 1)] - table[(size_t)i0]) * frac;

            acc += source[(size_t)k] * w;
            weightSum += w;
        }

        // 重みの和で割って、直流の大きさを保つ (端で重みが欠けても同じ)
        const double v = (weightSum != 0.0) ? acc / weightSum : 0.0;

        dest.push_back((int16_t)std::clamp(v * 32767.0, -32768.0, 32767.0));
    }
}

void GenPcmHelper::encodeBuffer(
    const std::vector<float>& source,
    double step,
    int qIndex,
    bool cleanResample,
    std::vector<int16_t>& dest
)
{
    dest.clear();

    if (source.empty()) return;
    if (step <= 0.0) step = 1.0;

    // すでに同じ条件で作ったものがあればコピーするだけで済ませる
    const uint64_t key = makeEncodeKey(source, step, qIndex, cleanResample);

    for (const auto& e : g_encodeCache) {
        if (e.valid && e.key == key) {
            dest = e.data;
            return;
        }
    }

    // 1. 目的のレートへ間引きながら int16 化する
    if (cleanResample) {
        resampleClean(source, step, dest);
    }
    else {
        dest.reserve((size_t)((double)source.size() / step) + 1);

        for (double pos = 0.0; pos < (double)source.size(); pos += step) {
            size_t index = (size_t)pos;

            if (index >= source.size()) break;

            dest.push_back((int16_t)std::clamp(source[index] * 32767.0f, -32768.0f, 32767.0f));
        }
    }

    // 2. コーデックでエンコード → デコードして、圧縮による歪みを焼き込む
    switch (qIndex) {
    case PcmCodecMode::dpcm:
        DpcmCodec::process(dest);
        break;
    case PcmCodecMode::snesBrr:
        BrrCodec::process(dest);
        break;
    case PcmCodecMode::psxVag:
        PsxAdpcmCodec::process(dest, PsxAdpcm::vagFilterCount);
        break;
    case PcmCodecMode::imaAdpcm:
        ImaAdpcmCodec::process(dest);
        break;
    case PcmCodecMode::cdromXa:
        PsxAdpcmCodec::process(dest, PsxAdpcm::xaFilterCount);
        break;
    case PcmCodecMode::ymz280b:
        Ymz280bAdpcmCodec::process(dest);
        break;
    case PcmCodecMode::k053260:
        KonamiAdpcmCodec::process(dest, false);
        break;
    case PcmCodecMode::k054539:
        KonamiAdpcmCodec::process(dest, true);
        break;
    case PcmCodecMode::ym2608Adpcm:
    default:
        Ym2608AdpcmCodec::process(dest);
        break;
    }

    // 3. 無音時のノイズを抑えるローパス
    lowPassFilter(dest);

    // 4. 次のボイスのために結果を控えておく
    EncodeCacheEntry& slot = g_encodeCache[g_encodeCacheNext];
    g_encodeCacheNext = (g_encodeCacheNext + 1) % encodeCacheSize;

    slot.key = key;
    slot.data = dest;
    slot.valid = true;
}
