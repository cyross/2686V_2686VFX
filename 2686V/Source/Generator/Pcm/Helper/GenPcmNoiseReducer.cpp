#include <algorithm>
#include <cmath>

#include "./GenPcmNoiseReducer.h"

namespace
{
    constexpr double kPi = 3.14159265358979323846;

    // 時定数 (秒) から 1 標本あたりの係数を作る
    float coefFor(double seconds, double sampleRate)
    {
        if (seconds <= 0.0 || sampleRate <= 0.0) return 1.0f;

        return (float)(1.0 - std::exp(-1.0 / (seconds * sampleRate)));
    }
}

void PcmNoiseReducer::setup(double sampleRate, double contentRatio, bool gate, float gateDb, int lpfLevel)
{
    if (sampleRate <= 0.0) sampleRate = 44100.0;

    // ---------------- 無音ゲート ----------------
    m_gate = gate;

    // 直流を取る高域通過。10Hz なら耳に入る低音は削らない。
    m_dcR = (float)std::exp(-2.0 * kPi * 10.0 / sampleRate);

    m_threshold = std::pow(10.0f, gateDb / 20.0f);

    // 包絡は 20ms で戻す。波形の山と山のあいだで閉じないように。
    m_envRelease = (float)std::exp(-1.0 / (0.020 * sampleRate));

    // 開くのは 1ms で素早く (頭を欠かない)、閉じるのは 30ms でゆっくり
    // (余韻を切った音にしない)。
    m_gainOpen = coefFor(0.001, sampleRate);
    m_gainClose = coefFor(0.030, sampleRate);

    // ---------------- 高域カット ----------------
    m_lpf = (lpfLevel > lpfOff);

    if (m_lpf) {
        // 素材の帯域の上端に対する割合。段階が上がるほど低く切る。
        const double factor = (lpfLevel == lpfLight) ? 0.9 : (lpfLevel == lpfMedium) ? 0.7 : 0.5;

        const double nyquist = sampleRate * 0.5;
        const double contentTop = nyquist * std::clamp(contentRatio, 0.0, 1.0);

        // 出力のナイキストに近すぎると係数が崩れるので、手前で止める
        double fc = std::clamp(contentTop * factor, 20.0, nyquist * 0.9);

        // RBJ の低域通過 (Q = 1/√2)
        const double w0 = 2.0 * kPi * fc / sampleRate;
        const double cw = std::cos(w0);
        const double alpha = std::sin(w0) / (2.0 * 0.70710678118654752);
        const double a0 = 1.0 + alpha;

        m_b0 = (float)(((1.0 - cw) * 0.5) / a0);
        m_b1 = (float)((1.0 - cw) / a0);
        m_b2 = m_b0;
        m_a1 = (float)((-2.0 * cw) / a0);
        m_a2 = (float)((1.0 - alpha) / a0);
    }
}

void PcmNoiseReducer::reset()
{
    m_dcX1 = 0.0f;
    m_dcY1 = 0.0f;
    m_envelope = 0.0f;

    // 鳴らし始めは開いておく。頭の小さい音を削らないように。
    m_gain = 1.0f;
    m_open = true;

    m_z1 = 0.0f;
    m_z2 = 0.0f;
}

float PcmNoiseReducer::process(float x)
{
    if (m_gate) {
        // 1. 直流を取る
        const float y = x - m_dcX1 + m_dcR * m_dcY1;

        m_dcX1 = x;
        m_dcY1 = y;
        x = y;

        // 2. 包絡を追う
        const float level = std::abs(x);

        m_envelope = std::max(level, m_envelope * m_envRelease);

        // 3. 開け閉め。閉じる側を少し下げて、しきい値の上下で
        //    ばたつかないようにする。
        if (m_open) {
            if (m_envelope < m_threshold * 0.5f) m_open = false;
        }
        else {
            if (m_envelope > m_threshold) m_open = true;
        }

        const float target = m_open ? 1.0f : 0.0f;

        m_gain += (target - m_gain) * (m_open ? m_gainOpen : m_gainClose);

        x *= m_gain;
    }

    if (m_lpf) {
        // 直接形 II 転置
        const float y = m_b0 * x + m_z1;

        m_z1 = m_b1 * x - m_a1 * y + m_z2;
        m_z2 = m_b2 * x - m_a2 * y;

        x = y;
    }

    return x;
}
