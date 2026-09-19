#include "doctest/doctest.h"

#include <cmath>
#include <cstdint>
#include <vector>

#include "Generator/Pcm/Helper/GenPcmHelper.h"
#include "Generator/Pcm/Helper/GenPcmNoiseReducer.h"
#include "Generator/Pcm/Adpcm/GenAdpcm.h"

// ============================================================================
// QUALITY (PCM) のノイズリダクション
// ============================================================================
// 既定 (切) のままなら今までと同じ音になること、入れたときに雑音と折り返しが
// 減ること、ゲートと高域カットが決めたとおりに効くことを見る。
namespace
{
    constexpr double kPi = 3.14159265358979323846;

    std::vector<float> sine(double freq, double rate, int n, double amp = 0.5)
    {
        std::vector<float> v((size_t)n);

        for (int i = 0; i < n; ++i) v[(size_t)i] = (float)(amp * std::sin(2.0 * kPi * freq * i / rate));

        return v;
    }

    // 既知の周波数の正弦波を最小二乗で当てはめ、残りを雑音 + 歪みとみなす (SINAD)
    double sinadDb(const std::vector<double>& x, double f, double fs)
    {
        double sss = 0, scc = 0, ssc = 0, ss = 0, sc = 0, sx = 0, sxs = 0, sxc = 0;
        const double n = (double)x.size();

        for (size_t i = 0; i < x.size(); ++i) {
            const double s = std::sin(2 * kPi * f * i / fs), c = std::cos(2 * kPi * f * i / fs);
            sss += s * s; scc += c * c; ssc += s * c; ss += s; sc += c;
            sx += x[i]; sxs += x[i] * s; sxc += x[i] * c;
        }

        double A[3][4] = { { sss, ssc, ss, sxs }, { ssc, scc, sc, sxc }, { ss, sc, n, sx } };

        for (int i = 0; i < 3; ++i) {
            for (int k = i + 1; k < 3; ++k) {
                const double r = A[k][i] / A[i][i];
                for (int j = i; j < 4; ++j) A[k][j] -= r * A[i][j];
            }
        }

        double co[3];
        for (int i = 2; i >= 0; --i) {
            double v = A[i][3];
            for (int j = i + 1; j < 3; ++j) v -= A[i][j] * co[j];
            co[i] = v / A[i][i];
        }

        double sig = 0, res = 0;
        for (size_t i = 0; i < x.size(); ++i) {
            const double s = std::sin(2 * kPi * f * i / fs), c = std::cos(2 * kPi * f * i / fs);
            const double fit = co[0] * s + co[1] * c + co[2];
            sig += fit * fit;
            res += (x[i] - fit) * (x[i] - fit);
        }

        return 10.0 * std::log10(sig / res);
    }

    // 立ち上がりと終わりの 5% を捨てて、-1〜1 へ戻す
    std::vector<double> middle(const std::vector<int16_t>& v)
    {
        std::vector<double> out;

        for (size_t i = v.size() / 20; i < v.size() - v.size() / 20; ++i) out.push_back(v[i] / 32767.0);

        return out;
    }

    double rmsDb(const std::vector<int16_t>& v)
    {
        double p = 0;
        for (auto s : v) p += (s / 32767.0) * (s / 32767.0);
        return 10.0 * std::log10(p / (double)v.size() + 1e-30);
    }
}

TEST_CASE("きれいな間引きを切っているときは、これまでと同じ素材になる")
{
    // 既存のパッチの鳴りを変えないことの確認。これまでの手順 (手前の 1 点を
    // 拾う → 符号化 → 2 点平均) をここで組み立て、同じになるかを見る。
    const auto src = sine(1000.0, 44100.0, 22050);
    const double step = 44100.0 / 16000.0;

    std::vector<int16_t> expected;
    for (double pos = 0.0; pos < (double)src.size(); pos += step) {
        expected.push_back((int16_t)std::clamp(src[(size_t)pos] * 32767.0f, -32768.0f, 32767.0f));
    }
    Ym2608AdpcmCodec::process(expected);
    GenPcmHelper::lowPassFilter(expected);

    std::vector<int16_t> actual;
    GenPcmHelper::encodeBuffer(src, step, 13, false, actual);

    CHECK(actual == expected);
}

TEST_CASE("きれいな間引きは、割り切れない刻みでも雑音が少ない")
{
    // 44.1kHz → 16kHz は刻みが割り切れない。手前の 1 点を拾うと、拾う位置の
    // 揺れがそのまま雑音になる (約 28dB)。
    const auto src = sine(1000.0, 44100.0, 44100);
    const double step = 44100.0 / 16000.0;

    std::vector<int16_t> plain;
    for (double pos = 0.0; pos < (double)src.size(); pos += step) {
        plain.push_back((int16_t)std::clamp(src[(size_t)pos] * 32767.0f, -32768.0f, 32767.0f));
    }

    std::vector<int16_t> clean;
    GenPcmHelper::resampleClean(src, step, clean);

    const double plainDb = sinadDb(middle(plain), 1000.0, 16000.0);
    const double cleanDb = sinadDb(middle(clean), 1000.0, 16000.0);

    MESSAGE("SINAD 手前の 1 点: " << plainDb << " dB / きれいな間引き: " << cleanDb << " dB");

    CHECK(plainDb < 35.0);
    CHECK(cleanDb > 70.0);   // 16bit へ丸めたぶんが残るだけ
    CHECK(clean.size() == plain.size());
}

TEST_CASE("きれいな間引きは、目的のナイキストを超える音を折り返さない")
{
    // 7kHz の音を 8kHz へ落とす。ナイキスト (4kHz) を超えるので、本来は
    // 消えるべき音。手前の 1 点を拾うと 1kHz へ折り返して残る。
    const auto src = sine(7000.0, 44100.0, 44100);
    const double step = 44100.0 / 8000.0;

    std::vector<int16_t> plain;
    for (double pos = 0.0; pos < (double)src.size(); pos += step) {
        plain.push_back((int16_t)std::clamp(src[(size_t)pos] * 32767.0f, -32768.0f, 32767.0f));
    }

    std::vector<int16_t> clean;
    GenPcmHelper::resampleClean(src, step, clean);

    std::vector<int16_t> plainMid(plain.begin() + 400, plain.end() - 400);
    std::vector<int16_t> cleanMid(clean.begin() + 400, clean.end() - 400);

    MESSAGE("折り返し 手前の 1 点: " << rmsDb(plainMid) << " dBFS / きれいな間引き: " << rmsDb(cleanMid) << " dBFS");

    CHECK(rmsDb(plainMid) > -12.0);   // 入力 (-9dBFS) がほぼそのまま残る
    CHECK(rmsDb(cleanMid) < -60.0);
}

TEST_CASE("きれいな間引きを入れると、符号化したあとの雑音も減る")
{
    const auto src = sine(1000.0, 44100.0, 44100);
    const double step = 44100.0 / 16000.0;

    // SNES BRR: 符号化そのものの雑音より、間引きの雑音のほうが大きい
    std::vector<int16_t> plain, clean;
    GenPcmHelper::encodeBuffer(src, step, 15, false, plain);
    GenPcmHelper::encodeBuffer(src, step, 15, true, clean);

    const double plainDb = sinadDb(middle(plain), 1000.0, 16000.0);
    const double cleanDb = sinadDb(middle(clean), 1000.0, 16000.0);

    MESSAGE("SNES BRR 16kHz: " << plainDb << " dB -> " << cleanDb << " dB");

    CHECK(cleanDb > plainDb + 6.0);
}

TEST_CASE("ノイズ対策を切っているときは、入れた値をそのまま返す")
{
    PcmNoiseReducer nr;
    nr.setup(48000.0, 0.33, false, -60.0f, PcmNoiseReducer::lpfOff);
    nr.reset();

    CHECK_FALSE(nr.isActive());

    const auto x = sine(440.0, 48000.0, 4800);
    for (float v : x) CHECK(nr.process(v) == v);
}

TEST_CASE("無音ゲートは、直流のずれだけが残る無音を 0 まで絞る")
{
    // 1-bit DPCM は音が止んでも -512 (約 -36dBFS) のまま止まる
    PcmNoiseReducer nr;
    nr.setup(48000.0, 0.33, true, -60.0f, PcmNoiseReducer::lpfOff);
    nr.reset();

    float last = 1.0f;
    for (int i = 0; i < 48000; ++i) last = nr.process(-512.0f / 32767.0f);

    CHECK(std::abs(last) < 1.0e-5f);
}

TEST_CASE("無音ゲートは、しきい値より大きい音を削らない")
{
    PcmNoiseReducer nr;
    nr.setup(48000.0, 0.33, true, -60.0f, PcmNoiseReducer::lpfOff);
    nr.reset();

    // -20dBFS の 1kHz
    const auto x = sine(1000.0, 48000.0, 48000, 0.1);

    double inP = 0, outP = 0;
    for (size_t i = 0; i < x.size(); ++i) {
        const float y = nr.process(x[i]);

        if (i > 4800) {  // 直流取りが落ち着いてから
            inP += (double)x[i] * x[i];
            outP += (double)y * y;
        }
    }

    const double diffDb = 10.0 * std::log10(outP / inP);

    CHECK(std::abs(diffDb) < 0.1);
}

TEST_CASE("高域カットは、段階に応じて素材の帯域の上のほうを削る")
{
    // 16kHz の素材を 48kHz で鳴らしている (素材の帯域の上端は 8kHz)
    const double ratio = 16000.0 / 48000.0;

    auto gainDb = [ratio](int level, double freq) {
        PcmNoiseReducer nr;
        nr.setup(48000.0, ratio, false, -60.0f, level);
        nr.reset();

        const auto x = sine(freq, 48000.0, 48000, 0.25);

        double inP = 0, outP = 0;
        for (size_t i = 0; i < x.size(); ++i) {
            const float y = nr.process(x[i]);

            if (i > 4800) {
                inP += (double)x[i] * x[i];
                outP += (double)y * y;
            }
        }

        return 10.0 * std::log10(outP / inP);
    };

    // 低い音はどの段階でもほとんど変わらない
    for (int level : { PcmNoiseReducer::lpfLight, PcmNoiseReducer::lpfMedium, PcmNoiseReducer::lpfStrong }) {
        CHECK(gainDb(level, 300.0) > -0.5);
    }

    // 素材の上端 (7.5kHz) は、段階が上がるほど深く削る
    const double light = gainDb(PcmNoiseReducer::lpfLight, 7500.0);
    const double medium = gainDb(PcmNoiseReducer::lpfMedium, 7500.0);
    const double strong = gainDb(PcmNoiseReducer::lpfStrong, 7500.0);

    MESSAGE("7.5kHz: 弱 " << light << " / 中 " << medium << " / 強 " << strong << " dB");

    CHECK(light < -0.5);
    CHECK(medium < light - 3.0);
    CHECK(strong < medium - 3.0);
}
