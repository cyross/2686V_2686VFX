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
    uint64_t makeEncodeKey(const std::vector<float>& source, double step, int qIndex)
    {
        uint64_t h = 1469598103934665603ull;

        auto mix = [&h](uint64_t v) {
            h ^= v;
            h *= 1099511628211ull;
            };

        mix((uint64_t)source.size());
        mix((uint64_t)qIndex);

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

void GenPcmHelper::encodeBuffer(
    const std::vector<float>& source,
    double step,
    int qIndex,
    std::vector<int16_t>& dest
)
{
    dest.clear();

    if (source.empty()) return;
    if (step <= 0.0) step = 1.0;

    // すでに同じ条件で作ったものがあればコピーするだけで済ませる
    const uint64_t key = makeEncodeKey(source, step, qIndex);

    for (const auto& e : g_encodeCache) {
        if (e.valid && e.key == key) {
            dest = e.data;
            return;
        }
    }

    dest.reserve((size_t)((double)source.size() / step) + 1);

    // 1. 目的のレートへ間引きながら int16 化する
    for (double pos = 0.0; pos < (double)source.size(); pos += step) {
        size_t index = (size_t)pos;

        if (index >= source.size()) break;

        dest.push_back((int16_t)std::clamp(source[index] * 32767.0f, -32768.0f, 32767.0f));
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
