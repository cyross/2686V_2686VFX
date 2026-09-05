#include <cmath>
#include <algorithm>

#include "./EnvSsgHw.h"

SsgHwPEnv::SsgHwPEnv() {
    updateSmoothCoeff();
}

void SsgHwPEnv::prepare(double sampleRate) {
    updateTargetSampleRate(sampleRate);
}

void SsgHwPEnv::updateTargetSampleRate(double newSampleRate)
{
    this->sampleRate = newSampleRate;
    updateSmoothCoeff();
}

void SsgHwPEnv::setParameters(const SsgHwPEnvParams& params) {
    this->m_useHwEnv = params.enable;
    this->m_envShape = params.shape;
    this->m_envFreq = params.period;
    this->m_min = (float)params.min;
    this->m_max = (float)params.max;
    this->m_smooth = params.smooth;
}

void SsgHwPEnv::updateSampleRate(double newSampleRate) {
    this->sampleRate = newSampleRate;
    updateSmoothCoeff();
}

void SsgHwPEnv::updateSmoothCoeff() {
    // 1 次ローパス y += (x - y) * coeff の係数。
    // coeff = 1 - exp(-1 / (時定数 * サンプリングレート))
    if (sampleRate <= 0.0) {
        m_smoothCoeff = 1.0f;
        return;
    }

    m_smoothCoeff = 1.0f - std::exp(-1.0f / (smoothTimeSec * (float)sampleRate));
    m_smoothCoeff = std::clamp(m_smoothCoeff, 0.0f, 1.0f);
}

float SsgHwPEnv::nextRandom() {
    // xorshift32
    m_rngState ^= m_rngState << 13;
    m_rngState ^= m_rngState >> 17;
    m_rngState ^= m_rngState << 5;

    return (float)(m_rngState & 0x00FFFFFFu) / (float)0x01000000u;
}

void SsgHwPEnv::noteOn() {
    this->m_hwEnvPhase = 0.0f;

    this->m_prevCycle = 0;
    this->m_cycleCount = 0;
    this->m_holdLevel = nextRandom();
}

void SsgHwPEnv::noteOff() {
    // 何もしない
}

float SsgHwPEnv::processCent() {
    // ==========================================
    // 1. Hardware Envelope Update
    // ==========================================
    float hwEnvDelta = m_envFreq / (float)sampleRate;
    m_hwEnvPhase += hwEnvDelta;

    // 実機由来のスロット 0〜7 のうち、保持形(1/3/5/7)だけが 1周期で止まる。
    // 繰り返し形(0/2/4/6)とオリジナル波形(8 以降)はすべて繰り返す。
    const bool isHolding =
        (m_envShape < (int)SsgHwShape::Square75) && ((m_envShape % 2) != 0);

    // 位相が無限増大して小数の精度が落ちるのを防ぐラップアラウンド
    if (m_hwEnvPhase >= 2.0) {
        if (isHolding) {
            m_hwEnvPhase = 2.0;
        }
        else {
            while (m_hwEnvPhase >= 2.0) m_hwEnvPhase -= 2.0;
        }
    }

    // ==========================================
    // 2. Sample & Hold 用のサイクル検出
    // ==========================================
    {
        int cycle = (int)m_hwEnvPhase;

        if (cycle != m_prevCycle) {
            m_prevCycle = cycle;
            m_cycleCount++;

            int holdCycles = 0;

            switch ((SsgHwShape)m_envShape) {
            case SsgHwShape::SampleHold:   holdCycles = 1;  break;
            case SsgHwShape::SampleHold4:  holdCycles = 4;  break;
            case SsgHwShape::SampleHold8:  holdCycles = 8;  break;
            case SsgHwShape::SampleHold16: holdCycles = 16; break;
            case SsgHwShape::SampleHold32: holdCycles = 32; break;
            case SsgHwShape::SampleHold64: holdCycles = 64; break;
            default: break;
            }

            if (holdCycles > 0 && (m_cycleCount % (uint32_t)holdCycles) == 0) {
                m_holdLevel = nextRandom();
            }
        }
    }

    // Min / Max は出力レンジ。大小が逆に設定されても破綻しないよう並べ替える。
    float lo = std::min(this->m_min, this->m_max);
    float hi = std::max(this->m_min, this->m_max);

    float hwEnvCent = hi;

    if (m_useHwEnv)
    {
        double p = m_hwEnvPhase;
        bool isEvenCycle = ((int)p % 2 == 0);
        float phaseNorm = (float)(p - std::floor(p));

        // 1周期を走り終えたか (保持形の判定)
        bool held = (p >= 1.0);

        // 下降・上昇はどちらも hi と lo の全域を使う
        auto down = [&](float x) { return hi - x * (hi - lo); };
        auto up = [&](float x) { return lo + x * (hi - lo); };

        // 最大値から始まる矩形波 / 最小値から始まる矩形波
        auto squareHigh = [&](float duty) { return (phaseNorm < duty) ? hi : lo; };
        auto squareLow = [&](float duty) { return (phaseNorm < duty) ? lo : hi; };

        // n 段の階段。レベルは i / (n - 1) で 0.0〜1.0 を等分する。
        auto stepIndex = [&](int total) {
            return std::min((int)(phaseNorm * (float)total), total - 1);
            };

        // 山形・谷形は 1周期で上って下る (下って上る) ので 2n-2 段になる
        auto foldIndex = [&](int n) {
            int total = n * 2 - 2;
            int i = stepIndex(total);

            return (i < n) ? i : (total - i);
            };

        auto stepUp = [&](int n) { return up((float)stepIndex(n) / (float)(n - 1)); };
        auto stepDown = [&](int n) { return down((float)stepIndex(n) / (float)(n - 1)); };
        auto stepPeak = [&](int n) { return up((float)foldIndex(n) / (float)(n - 1)); };
        auto stepValley = [&](int n) { return down((float)foldIndex(n) / (float)(n - 1)); };

        // 指数減衰。x=0 で 1.0、x=1 で 0.0 になるよう正規化してある。
        auto expCurve = [](float x) {
            constexpr float k = 5.0f;
            const float e0 = std::exp(-k);

            return (std::exp(-k * x) - e0) / (1.0f - e0);
            };

        switch ((SsgHwShape)m_envShape) {
        // ---------------- 実機準拠 (AY-3-8910 shape 8〜15) ----------------
        case SsgHwShape::SawDown:        hwEnvCent = down(phaseNorm); break;
        case SsgHwShape::SawDownHold:    hwEnvCent = held ? lo : down(phaseNorm); break;
        case SsgHwShape::Triangle:       hwEnvCent = isEvenCycle ? down(phaseNorm) : up(phaseNorm); break;
        case SsgHwShape::AltSawDownHold: hwEnvCent = held ? hi : down(phaseNorm); break;
        case SsgHwShape::SawUp:          hwEnvCent = up(phaseNorm); break;
        case SsgHwShape::SawUpHold:      hwEnvCent = held ? hi : up(phaseNorm); break;
        case SsgHwShape::TriangleInvert: hwEnvCent = isEvenCycle ? up(phaseNorm) : down(phaseNorm); break;
        case SsgHwShape::AltSawUpHold:   hwEnvCent = held ? lo : up(phaseNorm); break;

        // ---------------- 矩形波 ----------------
        case SsgHwShape::Square75:     hwEnvCent = squareHigh(0.75f); break;
        case SsgHwShape::Square50:     hwEnvCent = squareHigh(0.5f); break;
        case SsgHwShape::Square25:     hwEnvCent = squareHigh(0.25f); break;
        case SsgHwShape::Square125:    hwEnvCent = squareHigh(0.125f); break;
        case SsgHwShape::SquareInv75:  hwEnvCent = squareLow(0.75f); break;
        case SsgHwShape::SquareInv50:  hwEnvCent = squareLow(0.5f); break;
        case SsgHwShape::SquareInv25:  hwEnvCent = squareLow(0.25f); break;
        case SsgHwShape::SquareInv125: hwEnvCent = squareLow(0.125f); break;

        // ---------------- 階段 ----------------
        case SsgHwShape::StepUp4:      hwEnvCent = stepUp(4); break;
        case SsgHwShape::StepUp5:      hwEnvCent = stepUp(5); break;
        case SsgHwShape::StepUp6:      hwEnvCent = stepUp(6); break;
        case SsgHwShape::StepUp11:     hwEnvCent = stepUp(11); break;
        case SsgHwShape::StepDown4:    hwEnvCent = stepDown(4); break;
        case SsgHwShape::StepDown5:    hwEnvCent = stepDown(5); break;
        case SsgHwShape::StepDown6:    hwEnvCent = stepDown(6); break;
        case SsgHwShape::StepDown11:   hwEnvCent = stepDown(11); break;
        case SsgHwShape::StepPeak4:    hwEnvCent = stepPeak(4); break;
        case SsgHwShape::StepPeak5:    hwEnvCent = stepPeak(5); break;
        case SsgHwShape::StepPeak6:    hwEnvCent = stepPeak(6); break;
        case SsgHwShape::StepPeak11:   hwEnvCent = stepPeak(11); break;
        case SsgHwShape::StepValley4:  hwEnvCent = stepValley(4); break;
        case SsgHwShape::StepValley5:  hwEnvCent = stepValley(5); break;
        case SsgHwShape::StepValley6:  hwEnvCent = stepValley(6); break;
        case SsgHwShape::StepValley11: hwEnvCent = stepValley(11); break;

        // ---------------- Sample & Hold ----------------
        case SsgHwShape::SampleHold:
        case SsgHwShape::SampleHold4:
        case SsgHwShape::SampleHold8:
        case SsgHwShape::SampleHold16:
        case SsgHwShape::SampleHold32:
        case SsgHwShape::SampleHold64:
            hwEnvCent = up(m_holdLevel);
            break;

        // ---------------- 追加提案分 ----------------
        case SsgHwShape::ExpDecay:
            hwEnvCent = up(expCurve(phaseNorm));
            break;
        case SsgHwShape::ExpAttack:
            // 減衰カーブを時間反転すると、ゆっくり立ち上がって最後に一気に開く
            hwEnvCent = up(expCurve(1.0f - phaseNorm));
            break;
        case SsgHwShape::Sine:
            // 最小値から始まり、中央で最大値になって戻る
            hwEnvCent = up(0.5f * (1.0f - std::cos(phaseNorm * 6.283185307179586f)));
            break;
        case SsgHwShape::AttackDecay:
        {
            constexpr float attack = 0.125f;

            hwEnvCent = (phaseNorm < attack)
                ? up(phaseNorm / attack)
                : up(expCurve((phaseNorm - attack) / (1.0f - attack)));
            break;
        }
        case SsgHwShape::DoublePulse:
            // 1周期を 4 等分し、交互に最大 / 最小へ振る
            hwEnvCent = (((int)(phaseNorm * 4.0f) % 2) == 0) ? hi : lo;
            break;
        case SsgHwShape::Random:
            hwEnvCent = up(nextRandom());
            break;

        default:
            hwEnvCent = up(phaseNorm);
            break;
        }

        hwEnvCent = std::clamp(hwEnvCent, lo, hi);
    }
    else {
        // 切っているときは音程を動かさない
        hwEnvCent = 0.0f;
    }

    // ==========================================
    // 3. Smoothing
    // ==========================================
    // 段差がそのまま音程の飛びになるので、1 次ローパスで角を鈍らせる。
    if (!m_smooth) {
        // OFF のときも値だけは追従させておき、
        // ON に切り替えた瞬間に飛ばないようにする
        m_smoothedCent = hwEnvCent;
        currentCent = hwEnvCent;

        return hwEnvCent;
    }

    m_smoothedCent += (hwEnvCent - m_smoothedCent) * m_smoothCoeff;
    currentCent = m_smoothedCent;

    return m_smoothedCent;
}

float SsgHwPEnv::process(float phaseDelta) {
    float cent = processCent();

    if (cent == 0.0f) return phaseDelta;

    // 1200 セント = 1 オクターブ (2 倍の周波数)
    return phaseDelta * std::pow(2.0f, cent / 1200.0f);
}
