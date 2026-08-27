#include <cmath>
#include <algorithm>

#include "./EnvSsgHw.h"

SsgHwEnv::SsgHwEnv() {
    updateSmoothCoeff();
}

void SsgHwEnv::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void SsgHwEnv::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = newSampleRate;
    updateSmoothCoeff();
}

void SsgHwEnv::setParameters(const SsgHwEnvParams& params) {
    this->m_useHwEnv = params.enable;
    this->m_envShape = params.shape;
    this->m_envFreq = params.period;
    this->m_min = params.min;
    this->m_max = params.max;
    this->m_smooth = params.smooth;
}

void SsgHwEnv::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
    updateSmoothCoeff();
}

void SsgHwEnv::updateSmoothCoeff() {
    // 1 次ローパス y += (x - y) * coeff の係数。
    // coeff = 1 - exp(-1 / (時定数 * サンプリングレート))
    if (sampleRate <= 0.0) {
        m_smoothCoeff = 1.0f;
        return;
    }

    m_smoothCoeff = 1.0f - std::exp(-1.0f / (smoothTimeSec * (float)sampleRate));
    m_smoothCoeff = std::clamp(m_smoothCoeff, 0.0f, 1.0f);
}

void SsgHwEnv::noteOn() {
    this->m_hwEnvPhase = 0.0f;
}

void SsgHwEnv::noteOff() {
    // 何もしない
}

float SsgHwEnv::process() {
    // ==========================================
    // 1. Hardware Envelope Update
    // ==========================================
    float hwEnvDelta = m_envFreq / (float)sampleRate;
    m_hwEnvPhase += hwEnvDelta;

    // 位相が無限増大して小数の精度が落ちるのを防ぐラップアラウンド。
    // 繰り返し形(スロット 0/2/4/6)は 2周期でラップし、
    // 保持形(スロット 1/3/5/7)は 1周期を走り終えた位置で止める。
    if (m_hwEnvPhase >= 2.0) {
        if (m_envShape % 2 == 0) {
            while (m_hwEnvPhase >= 2.0) m_hwEnvPhase -= 2.0;
        }
        else {
            m_hwEnvPhase = 2.0;
        }
    }

    // Min / Max は出力レンジ。大小が逆に設定されても破綻しないよう並べ替える。
    float lo = std::min(this->m_min, this->m_max);
    float hi = std::max(this->m_min, this->m_max);

    float hwEnvGain = hi;

    if (m_useHwEnv)
    {
        double p = m_hwEnvPhase;
        bool isEvenCycle = ((int)p % 2 == 0);
        float phaseNorm = (float)(p - std::floor(p));

        // 1周期を走り終えたか (保持形の判定)。
        // 以前は位相 p を出力レベル m_max と直接比べていたため、
        // Max を 1.0 未満にすると保持に入るのが早まっていた。
        bool held = (p >= 1.0);

        // 下降・上昇はどちらも hi と lo の全域を使う
        auto down = [&](float x) { return hi - x * (hi - lo); };
        auto up = [&](float x) { return lo + x * (hi - lo); };

        // 実機 AY-3-8910 / YM2149 のエンベロープ形状 (shape 8〜15) に対応する
        switch (m_envShape) {
        case 0: // Saw Down            : 実機 shape 8  (下降を繰り返す)
            hwEnvGain = down(phaseNorm);
            break;
        case 1: // Saw Down & Hold     : 実機 shape 9  (1回下降して最小値で保持)
            hwEnvGain = held ? lo : down(phaseNorm);
            break;
        case 2: // Triangle            : 実機 shape 10 (下降と上昇を繰り返す)
            hwEnvGain = isEvenCycle ? down(phaseNorm) : up(phaseNorm);
            break;
        case 3: // Alt Saw Down & Hold : 実機 shape 11 (1回下降して最大値で保持)
            hwEnvGain = held ? hi : down(phaseNorm);
            break;
        case 4: // Saw Up              : 実機 shape 12 (上昇を繰り返す)
            hwEnvGain = up(phaseNorm);
            break;
        case 5: // Saw Up & Hold       : 実機 shape 13 (1回上昇して最大値で保持)
            hwEnvGain = held ? hi : up(phaseNorm);
            break;
        case 6: // Triangle Invert     : 実機 shape 14 (上昇と下降を繰り返す)
            hwEnvGain = isEvenCycle ? up(phaseNorm) : down(phaseNorm);
            break;
        case 7: // Alt Saw Up & Hold   : 実機 shape 15 (1回上昇して最小値で保持)
            hwEnvGain = held ? lo : up(phaseNorm);
            break;
        default:
            hwEnvGain = up(phaseNorm);
            break;
        }

        hwEnvGain = std::clamp(hwEnvGain, lo, hi);
    }

    // ==========================================
    // 2. Smoothing
    // ==========================================
    // 波形の折り返しで生じる段差がブツブツ音の原因なので、
    // 1 次ローパスで角を鈍らせる。
    if (!m_smooth) {
        // OFF のときも値だけは追従させておき、
        // ON に切り替えた瞬間に飛ばないようにする
        m_smoothedGain = hwEnvGain;
        return hwEnvGain;
    }

    m_smoothedGain += (hwEnvGain - m_smoothedGain) * m_smoothCoeff;

    return m_smoothedGain;
}
