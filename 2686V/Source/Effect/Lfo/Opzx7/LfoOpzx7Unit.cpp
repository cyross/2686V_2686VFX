#include <JuceHeader.h>
#include <algorithm>

#include "./LfoOpzx7Unit.h"

Opzx7LfoCoreUnit::Opzx7LfoCoreUnit() {
    m_noteOnFunctions = {
        [this]() {
            // フリーラン継続
            this->m_sdCounter = 0.0f;

            // ディレイ0(フリーラン設定)であっても、ワンショット波形の時は
            // 毎回アタマから再生されないと不自然なので強制的にSyncさせる
            if (this->m_isOneshot) {
                this->m_phase = 0.0;
            }
        },
        [this]() {
            // 位相を0に戻す (Sync)
            this->m_phase = 0.0;
            this->m_sdCounter = 0.0f;
        },
        [this]() {
            // 位相を0に戻す (Sync)
            this->m_phase = 0.0;
            this->m_sdCounter = this->m_sd / 1000.0f;
        }
    };
}

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム
// -----------------------------------------------------------
const std::array<Opzx7LfoCoreUnit::Opzx7LfoCalculator, 8> Opzx7LfoCoreUnit::lfoStrategies = { {
        // 0: Sine
        [](double phase, float /*noise*/) -> float {
            return (float)std::sin(phase * 2.0 * juce::MathConstants<double>::pi);
        },
    // 1: Saw Up
    [](double phase, float /*noise*/) -> float {
        float pm = 0.0f;
        if (phase < 0.5) pm = (float)(phase * 2.0);
        else             pm = (float)(-1.0 + (phase - 0.5) * 2.0);

        return pm;
    },
    // 2: Saw Down
    [](double phase, float /*noise*/) -> float {
        return (float)(1.0 - phase * 2.0);
    },
    // 3: Square
    [](double phase, float /*noise*/) -> float {
        return (phase < 0.5) ? 1.0f : -1.0f;
    },
    // 4: Triangle
    [](double phase, float /*noise*/) -> float {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        return pm;
    },
    // 5: Sample & Hold (Noise)
    [](double /*phase*/, float noise) -> float {
        return noise;
    },
    // 6: Saw Down & One Shot
    [](double phase, float /*noise*/) -> float {
        return (float)(phase < 0.5 ? 1.0 - phase * 2.0 : 0.0);
    },
    // 7: Triangle & One Shot
    [](double phase, float /*noise*/) -> float {
        if (phase < 0.25)      return (float)(phase * 4.0);
        else if (phase < 0.5)  return (float)(1.0 - (phase - 0.25) * 4.0);
        else                   return 0.0;
    }
} };

inline void Opzx7LfoCoreUnit::updatePhaseDelta()
{
    this->m_phaseDelta = (double)this->m_freq / this->m_sampleRate;
}

void Opzx7LfoCoreUnit::prepare(double sampleRate) {
    this->m_smooth = 0.0f;

    updateTargetSampleRate(sampleRate);
}

void Opzx7LfoCoreUnit::updateTargetSampleRate(double newSampleRate) {
    this->m_sampleRate = newSampleRate;

    updatePhaseDelta();
}

void Opzx7LfoCoreUnit::setParameters(int syncDelay, bool enable, float freq, int index, float ms, float md, float smoothRate)
{
	this->m_sdParam = syncDelay;
    this->m_sdIndex = std::clamp(this->m_sdParam, 0, 2);
    this->m_sd = (float)(m_sdParam - 1) * (1000.0f / 60.0f);

    this->enable = enable;
    this->m_freq = freq;
    this->m_waveIndex = std::clamp(index, 0, 7);
    this->m_isOneshot = this->m_waveIndex == 6 || this->m_waveIndex == 7;

    this->ms = ms;
    this->md = md;

    this->depthDb = (this->ms * this->md) * 96.0f;
    this->depthCent = (this->ms * this->md) * 1200.0f;

    this->m_smoothRate = smoothRate;

    updatePhaseDelta();
}

void Opzx7LfoCoreUnit::noteOn()
{
    // LFO Sync Delay が 0より大きければ、位相をリセット(Sync)してディレイ開始
    this->m_noteOnFunctions[this->m_sdIndex]();
    this->m_sdCycleCount = 0;
}

float Opzx7LfoCoreUnit::getSample()
{
    // Sync Delay 更新
    if (this->m_sdCounter > 0.0f) {
        this->m_sdCounter -= 1.0f / (float)this->m_sampleRate;
        if (this->m_sdCounter < 0.0f) this->m_sdCounter = 0.0f;
    }

    // ========================================================
    // 0. ローカル LFO の計算 (波形独立)
    // ========================================================
    float val = 0.0f;

    if (this->enable) {
        if (this->m_sdCounter <= 0.0f) {
            // ローカルのLFO Freq (そのままHzとして使用)
            this->m_phase += this->m_phaseDelta;

            bool cycled = this->m_phase >= 1.0;

            if (cycled) {
                this->m_currentNoiseSample = m_noiseGen.generate();
                this->m_phase -= 1.0;
                this->m_sdCycleCount++;
            }

            // (※ノイズが必要な場合は共有のノイズジェネレータか乱数を使用)
            val = Opzx7LfoCoreUnit::lfoStrategies[this->m_waveIndex](this->m_phase, this->m_currentNoiseSample);

            // ワンショット波形 (6, 7) のミュート処理
            if (this->m_isOneshot && this->m_sdCycleCount > 0) val = 0.0f;
        }
    }

    return val;
}

float Opzx7LfoCoreUnit::getSamplePm()
{
    return getSample();
}

float Opzx7LfoCoreUnit::getSampleAm()
{
    float val = getSample();

    // AMクリックノイズ防止スムージング
    this->m_smooth += (val - this->m_smooth) * this->m_smoothRate;

    return this->m_smooth;
}
