#pragma once

#include <array>
#include <cmath>

// ============================================================================
// ファミコン / NES (2A03) の矩形波の鳴り方
// ============================================================================
// SSG の「More Real FC」「More Real NES」で使う。値は NESdev Wiki
// (APU Pulse / APU Mixer) による。
//
// - デューティ比は 12.5% / 25% / 50% / 75% の 4 種類だけ。8 段の並びで出す
// - 周波数は 11bit のタイマーで決まる。fpulse = fCPU / (16 × (t + 1))。
//   t が 8 未満のときは鳴らない
// - 出力は 0 か音量かの単極で、4bit (0〜15) の音量を非線形の DAC へ通す
// - 本体の出力の後ろに一次のフィルタが並ぶ。ここが FC と NES の違いになる
//     FC  : 37Hz のハイパス
//     NES : 90Hz と 440Hz のハイパス、14kHz のローパス
namespace Pulse2A03
{
    // NTSC の CPU クロック
    inline constexpr double cpuClock = 1789772.727;

    inline constexpr int timerMin = 8;
    inline constexpr int timerMax = 0x7FF;

    // 4 種類のデューティ比と、8 段の並び (1 が高い側)
    inline constexpr std::array<float, 4> duties = { 0.125f, 0.25f, 0.5f, 0.75f };
    inline constexpr std::array<std::array<int, 8>, 4> sequences = { {
        { 0, 1, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 0, 0, 0 },
        { 1, 0, 0, 1, 1, 1, 1, 1 },
    } };

    // 求めたデューティ比に最も近い実機の 1 つを選ぶ
    inline int nearestDuty(float duty)
    {
        int best = 0;
        float bestDist = 2.0f;

        for (int i = 0; i < (int)duties.size(); ++i) {
            const float d = std::abs(duties[(size_t)i] - duty);

            if (d < bestDist) { bestDist = d; best = i; }
        }

        return best;
    }

    // 位相 (0〜1) での出力。0 か 1 の単極。
    inline float sample(float phase, int dutyIndex)
    {
        int step = (int)(phase * 8.0f);

        if (step < 0) step = 0;
        if (step > 7) step = 7;

        return (float)sequences[(size_t)dutyIndex][(size_t)step];
    }

    // 周波数をタイマーの刻みへ寄せる。鳴らない (t < 8) ときは 0 を返す。
    // タイマーの上限を超える低い音は、上限で鳴らす。
    inline double quantizeFrequency(double freq)
    {
        if (freq <= 0.0) return 0.0;

        long t = std::lround(cpuClock / (16.0 * freq) - 1.0);

        if (t < timerMin) return 0.0;
        if (t > timerMax) t = timerMax;

        return cpuClock / (16.0 * (double)(t + 1));
    }

    // 音量 (0〜1) を 4bit に丸め、DAC の非線形を通す。15 のときに 1 になる。
    // pulse_out = 95.88 / (8128 / v + 100)
    inline float volume(float level)
    {
        if (level <= 0.0f) return 0.0f;

        const float clipped = level > 1.0f ? 1.0f : level;
        const float v = std::round(clipped * 15.0f);

        if (v <= 0.0f) return 0.0f;

        const float full = 95.88f / (8128.0f / 15.0f + 100.0f);
        const float out = 95.88f / (8128.0f / v + 100.0f);

        // 1 を超える指定 (LEVEL の上げ分) は、比のまま残す
        return (out / full) * (level / clipped);
    }

    // 出力の後ろのフィルタ。どちらも一次。
    class OutputFilter
    {
    public:
        enum class Model { Fc, Nes };

        void prepare(double sampleRate, Model model)
        {
            if (sampleRate == m_rate && model == m_model) return;

            m_rate = sampleRate;
            m_model = model;

            const double dt = 1.0 / sampleRate;

            auto hp = [dt](double fc) { const double rc = 1.0 / (2.0 * 3.14159265358979 * fc); return (float)(rc / (rc + dt)); };
            auto lp = [dt](double fc) { const double rc = 1.0 / (2.0 * 3.14159265358979 * fc); return (float)(dt / (rc + dt)); };

            if (model == Model::Fc) {
                m_hp1 = hp(37.0);
                m_useHp2 = false;
                m_useLp = false;
            }
            else {
                m_hp1 = hp(90.0);
                m_hp2 = hp(440.0);
                m_useHp2 = true;

                // ナイキストを超える遮断は掛けても意味が無いので外す
                m_useLp = 14000.0 < sampleRate * 0.5;
                m_lp = lp(14000.0);
            }
        }

        void reset()
        {
            m_x1 = m_y1 = m_x2 = m_y2 = m_yl = 0.0f;
        }

        float process(float x)
        {
            float y = m_hp1 * (m_y1 + x - m_x1);
            m_x1 = x;
            m_y1 = y;

            if (m_useHp2) {
                const float y2 = m_hp2 * (m_y2 + y - m_x2);
                m_x2 = y;
                m_y2 = y2;
                y = y2;
            }

            if (m_useLp) {
                m_yl += (y - m_yl) * m_lp;
                y = m_yl;
            }

            return y;
        }

    private:
        double m_rate = 0.0;
        Model m_model = Model::Fc;

        float m_hp1 = 1.0f;
        float m_hp2 = 1.0f;
        float m_lp = 1.0f;
        bool m_useHp2 = false;
        bool m_useLp = false;

        float m_x1 = 0.0f, m_y1 = 0.0f;
        float m_x2 = 0.0f, m_y2 = 0.0f;
        float m_yl = 0.0f;
    };
}
