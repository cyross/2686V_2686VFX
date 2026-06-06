#include <JuceHeader.h>
#include <algorithm>

#include "./LfoOpzx7.h"

Opzx7LfoCore::Opzx7LfoCore() {
    m_noteOnFunctions = {
        [this]() {
            this->m_sdCounter = 0.0f; // フリーラン継続

            // ディレイ0(フリーラン設定)であっても、ワンショット波形の時は
            // 毎回アタマから再生されないと不自然なので強制的にSyncさせる
            if (this->m_isOneshotPm || this->m_isOneshotAm) {
                this->m_pmPhase = 0.0;
                this->m_amPhase = 0.0;
            }
        },
        [this]() {
            // 位相を0に戻す (Sync)
            this->m_pmPhase = 0.0;
            this->m_amPhase = 0.0;

            this->m_sdCounter = 0.0f; // ms -> 秒
        },
        [this]() {
            // 位相を0に戻す (Sync)
            this->m_pmPhase = 0.0;
            this->m_amPhase = 0.0;

            this->m_sdCounter = this->m_sd / 1000.0f; // ms -> 秒
        }
    };
}

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (OPM PG)
// -----------------------------------------------------------
const std::array<Opzx7LfoCore::Opzx7LfoCalculator, 8> Opzx7LfoCore::lfoPmStrategies = { {
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
    // 5: Sample & Hold
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

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (OPM EG)
// -----------------------------------------------------------
const std::array<Opzx7LfoCore::Opzx7LfoCalculator, 8> Opzx7LfoCore::lfoAmStrategies = { {
    // 0: Sine
    [](double phase, float /*noise*/)-> float {
        float pm = (float)std::sin(phase * 2.0 * juce::MathConstants<double>::pi);

        return (pm + 1.0f) * 0.5f;
    },
    // 1: Saw Up
    [](double phase, float /*noise*/) -> float {
        return (float)phase;
    },
    // 2: Saw Down
    [](double phase, float /*noise*/) -> float {
        return (float)(1.0 - phase);
    },
    // 3: Square
    [](double phase, float /*noise*/) -> float {
        return (phase < 0.5) ? 1.0f : 0.0f;
    },
    // 4: Triangle
    [](double phase, float /*noise*/) -> float {
        float am = 0.0f;

        if (phase < 0.5) am = (float)(1.0 - phase * 2.0);
        else             am = (float)((phase - 0.5) * 2.0);

        return am;
    },
    // 5: Sample & Hold
    [](double /*phase*/, float noise) -> float {
        return (noise + 1.0f) * 0.5f;
    },
    // 6: Saw Down & One Shot
    [](double phase, float /*noise*/) -> float {
        return (float)(phase < 0.5 ? 1.0 - phase : 0.0);
    },
    // 7: Triangle & One Shot
    [](double phase, float /*noise*/) -> float {
        return (phase < 0.5) ? (float)(phase * 2.0) : 0.0f;
    }
} };

inline void Opzx7LfoCore::updatePhaseDelta()
{
    this->m_pmPhaseDelta = (double)this->m_pmFreq / this->m_sampleRate;
    this->m_amPhaseDelta = (double)this->m_amFreq / this->m_sampleRate;
}

void Opzx7LfoCore::prepare(double sampleRate) {
    this->amSmooth = 0.0f;

    updateTargetSampleRate(sampleRate);
}

void Opzx7LfoCore::updateTargetSampleRate(double newSampleRate) {
    this->m_sampleRate = newSampleRate;

    updatePhaseDelta();
}

void Opzx7LfoCore::setParameters(int syncDelay, bool pm, bool am, float pmFreq, float amFreq, int pgIndex, int egIndex, float pms, float pmd, float ams, float amd, float amSmoothRate)
{
	this->m_sdParam = syncDelay;
    this->m_sdIndex = std::clamp(this->m_sdParam, 0, 2);
    this->m_sd = (float)(m_sdParam - 1) * (1000.0f / 60.0f);

    this->pmEnable = pm;
    this->m_pmFreq = pmFreq;
    this->m_pmWaveIndex = std::clamp(pgIndex, 0, 7);
    this->m_isOneshotPm = this->m_pmWaveIndex == 6 || this->m_pmWaveIndex == 7;

    this->pms = pms;
    this->pmd = pmd;

    this->amEnable = am;
    this->m_amFreq = amFreq;
    this->m_amWaveIndex = std::clamp(egIndex, 0, 7);
    this->m_isOneshotAm = this->m_amWaveIndex == 6 || this->m_amWaveIndex == 7;
    this->ams = ams;
    this->amd = amd;

    this->depthDb = (this->ams * this->amd) * 96.0f;
    this->depthCent = (this->pms * this->pmd) * 1200.0f;

    this->m_amSmoothRate = amSmoothRate;

    updatePhaseDelta();
}

void Opzx7LfoCore::noteOn()
{
    // LFO Sync Delay が 0より大きければ、位相をリセット(Sync)してディレイ開始
    this->m_noteOnFunctions[this->m_sdIndex]();
    this->m_pmCycleCount = 0;
    this->m_amCycleCount = 0;
}

void Opzx7LfoCore::getSample()
{
    // Sync Delay 更新
    if (this->m_sdCounter > 0.0f) {
        this->m_sdCounter -= 1.0f / (float)this->m_sampleRate;
        if (this->m_sdCounter < 0.0f) this->m_sdCounter = 0.0f;
    }

    // ========================================================
    // 0. ローカル LFO の計算 (波形独立)
    // ========================================================
    float amVal = 0.0f;
    float pmVal = 0.0f;

    if (this->pmEnable || this->amEnable) {
        if (this->m_sdCounter <= 0.0f) {
            // ローカルのLFO Freq (そのままHzとして使用)
            this->m_pmPhase += this->m_pmPhaseDelta;
            this->m_amPhase += this->m_amPhaseDelta;

            bool pmCycled = this->m_pmPhase >= 1.0;
            bool amCycled = this->m_amPhase >= 1.0;

            if (pmCycled || amCycled) {
                this->m_currentNoiseSample = m_noiseGen.generate();

                if (pmCycled) {
                    this->m_pmPhase -= 1.0;
                    this->m_pmCycleCount++;
                }

                if (amCycled) {
                    this->m_amPhase -= 1.0;
                    this->m_amCycleCount++;
                }
            }

            // (※ノイズが必要な場合は共有のノイズジェネレータか乱数を使用)
            pmVal = Opzx7LfoCore::lfoPmStrategies[this->m_pmWaveIndex](this->m_pmPhase, this->m_currentNoiseSample);
            amVal = Opzx7LfoCore::lfoAmStrategies[this->m_amWaveIndex](this->m_amPhase, this->m_currentNoiseSample);

            // ワンショット波形 (6, 7) のミュート処理
            if (this->m_isOneshotPm && this->m_pmCycleCount > 0) pmVal = 0.0f;
            if (this->m_isOneshotAm && this->m_amCycleCount > 0) amVal = 0.0f;
        }
    }

    // AMクリックノイズ防止スムージング
    this->amSmooth += (amVal - this->amSmooth) * this->m_amSmoothRate;

    this->value.am = this->amSmooth;
    this->value.pm = pmVal;
}