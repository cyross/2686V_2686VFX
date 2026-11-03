#pragma once

#include <array>
#include <cmath>

#include "./GenPulse2A03.h"

// ============================================================================
// FC (2A03) の三角波とノイズ (4.3.0)
// ============================================================================
// SSG の三角波・ノイズの「More Real FC」で使う。値は NESdev Wiki
// (APU Triangle / APU Noise / APU Mixer) による。出力の後ろのフィルタは
// 矩形波と同じ FC のもの (Pulse2A03::OutputFilter::Model::Fc)。
namespace Tnd2A03
{
    // ------------------------------------------------------------------------
    // 三角波
    // ------------------------------------------------------------------------
    // 4bit (0〜15) × 32 段の階段。15, 14 … 1, 0, 0, 1 … 14, 15 と並ぶ。
    // 周波数は 11bit のタイマーで決まる。ftri = fCPU / (32 × (t + 1))。
    // 矩形波より 1 オクターブ低い。音量のレジスタは無い (常に 0〜15 で振れる)。
    inline constexpr int triangleSteps = 32;

    // タイマーが 2 未満だと超音波になる。実機のドライバは使わないので鳴らさない
    inline constexpr int triangleTimerMin = 2;

    // 位相 (0〜1) での段の値 (0〜15)
    inline int triangleLevel(float phase)
    {
        int step = (int)(phase * (float)triangleSteps);

        if (step < 0) step = 0;
        if (step > triangleSteps - 1) step = triangleSteps - 1;

        return step < 16 ? 15 - step : step - 16;
    }

    // 三角波とノイズが通る DAC (ピン 2) の曲がり。1 つだけ鳴らすときの形で、
    // 15 のときに 1 になるよう割る。tnd_out = 159.79 / (1 / (T / 8227) + 100)
    inline float triangleDac(int level)
    {
        if (level <= 0) return 0.0f;

        const float full = 159.79f / (8227.0f / 15.0f + 100.0f);
        const float out = 159.79f / (8227.0f / (float)level + 100.0f);

        return out / full;
    }

    // 位相 (0〜1) での出力。0〜1 の単極
    inline float triangle(float phase) { return triangleDac(triangleLevel(phase)); }

    // 周波数をタイマーの刻みへ寄せる。鳴らない (t < 2) ときは 0 を返す。
    // タイマーの上限を超える低い音は、上限で鳴らす。
    inline double quantizeTriangleFrequency(double freq)
    {
        if (freq <= 0.0) return 0.0;

        long t = std::lround(Pulse2A03::cpuClock / (32.0 * freq) - 1.0);

        if (t < triangleTimerMin) return 0.0;
        if (t > Pulse2A03::timerMax) t = Pulse2A03::timerMax;

        return Pulse2A03::cpuClock / (32.0 * (double)(t + 1));
    }

    // ------------------------------------------------------------------------
    // ノイズ
    // ------------------------------------------------------------------------
    // 15bit の LFSR。初期型 FC (RP2A03 無印) と同じ長周期だけ
    // (ビット 0 とビット 1 の XOR をビット 14 へ戻す。32,767 段)。
    // ビット 0 が 0 のときに音量を出し、1 のときは 0。
    // 進む速さは 16 段の表 (CPU の何サイクルごとか) で決まる。
    inline constexpr std::array<int, 16> noisePeriods = {
        4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068,
    };

    // 求めた速さ (Hz) に最も近い表の速さ。比で近いものを選ぶ。
    // ボイスごとに毎ブロック呼ばれるので、表の速さの log は前もって求めておき、
    // 呼ぶたびの log は 1 回だけにする
    inline double nearestNoiseRate(double rate)
    {
        if (rate <= 0.0) return 0.0;

        static const std::array<double, noisePeriods.size()> logRates = [] {
            std::array<double, noisePeriods.size()> v{};

            for (size_t i = 0; i < noisePeriods.size(); ++i) v[i] = std::log(Pulse2A03::cpuClock / (double)noisePeriods[i]);

            return v;
        }();

        const double lr = std::log(rate);

        size_t best = 0;
        double bestDist = 1.0e30;

        for (size_t i = 0; i < noisePeriods.size(); ++i) {
            const double d = std::abs(logRates[i] - lr);

            if (d < bestDist) {
                bestDist = d;
                best = i;
            }
        }

        return Pulse2A03::cpuClock / (double)noisePeriods[best];
    }

    // SSG のノイズの速さ (0〜knobMax Hz) を、表の速さへ写す。SSG の範囲のままでは
    // 表の速い 4 段 (period 4〜32) に届かないので、knobMax を表のいちばん速い段に
    // 合わせて引き伸ばしてから、最も近い段を選ぶ
    inline double noiseRateFromSsg(double rate, double knobMax)
    {
        if (knobMax <= 0.0) return nearestNoiseRate(rate);

        const double fastest = Pulse2A03::cpuClock / (double)noisePeriods.front();

        return nearestNoiseRate(rate * fastest / knobMax);
    }

    // 1 段進める。戻り値は進めたあとの出力 (0 / 1)
    inline float stepNoise(unsigned int& lfsr)
    {
        const unsigned int feedback = (lfsr ^ (lfsr >> 1)) & 1u;

        lfsr = (lfsr >> 1) | (feedback << 14);

        return (lfsr & 1u) ? 0.0f : 1.0f;
    }

    inline constexpr unsigned int noiseInitial = 1u;
    inline constexpr int noisePeriodLong = 32767;
}

// ============================================================================
// SSG (YM2203 / YM2608B) の出力 (4.3.0)
// ============================================================================
// 矩形波の「More Real SSG」で使う。SSG の出力は 0 から上だけに振れる単極で、
// 基板のコンデンサ (直流カット) を通って 0 中心へ戻る。そのとき矩形波の平らな
// ところが傾き (サグ)、音量を急に上げると直流が跳ねて「パツッ」と鳴る。
// これを一次のハイパス (約 30Hz) で真似る。
namespace SsgReal
{
    inline constexpr double dcCutHz = 30.0;

    class DcBlocker
    {
    public:
        void prepare(double sampleRate)
        {
            if (sampleRate == m_rate) return;

            m_rate = sampleRate;

            const double rc = 1.0 / (2.0 * 3.14159265358979 * dcCutHz);

            m_a = (float)(rc / (rc + 1.0 / sampleRate));
        }

        void reset() { m_x = m_y = 0.0f; }

        float process(float x)
        {
            m_y = m_a * (m_y + x - m_x);
            m_x = x;

            return m_y;
        }

    private:
        double m_rate = 0.0;
        float m_a = 1.0f;
        float m_x = 0.0f;
        float m_y = 0.0f;
    };

    // トーンとノイズは論理積 (AND) で混ざる。MIX が 0 ならトーンだけ、1 ならノイズだけ、
    // 0.5 で両方を AND したもの。そのあいだは近いほうと AND を混ぜる (どれも 0〜1)
    inline float mix(float tone, float noise, float mixAmount)
    {
        const float m = mixAmount < 0.0f ? 0.0f : (mixAmount > 1.0f ? 1.0f : mixAmount);
        const float both = tone * noise;
        const float wBoth = 1.0f - std::abs(2.0f * m - 1.0f);

        if (m < 0.5f) return (1.0f - 2.0f * m) * tone + wBoth * both;

        return (2.0f * m - 1.0f) * noise + wBoth * both;
    }
}
