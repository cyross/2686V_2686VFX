#include <JuceHeader.h>
#include <algorithm>

#include "./Core.h"

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (N888BASIC(86)準拠)
// -----------------------------------------------------------
const std::array<N88LfoCore::N88LfoCalculator, 6> N88LfoCore::pmStrategies = { {
        // 0: Saw Up
        [](double phase, float /*noise*/) -> float {
            return (float)(phase < 0.5 ? phase * 2.0 - 1.0 : phase * 2.0 - 2.0);
        },
    // 1: Square
    [](double phase, float /*noise*/) -> float {
        return (phase < 0.5) ? 1.0f : 0.0f;
    },
    // 2: Triangle
    [](double phase, float /*noise*/) -> float {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        return pm;
    },
    // 3: Sample & Hold
    [](double /*phase*/, float noise) -> float {
        return noise;
    },
    // 4: Saw Down & One Shot
    [](double phase, float /*noise*/) -> float {
        return (float)(phase < 0.5 ? 1.0 - phase * 2.0 : 0.0);
    },
    // 5: Triangle & One Shot
    [](double phase, float /*noise*/) ->float {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.5)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = 0.0;

        return pm;
    }
} };

const std::array<N88LfoCore::N88LfoCalculator, 6> N88LfoCore::amStrategies = { {
        // 0: Saw Up
        [](double phase, float /*noise*/) -> float {
            return (float)phase;
        },
    // 1: Square
    [](double phase, float /*noise*/) -> float {
        return (phase < 0.5) ? 1.0f : 0.0f;
    },
    // 2: Triangle
    [](double phase, float /*noise*/) -> float {
        return (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
    },
    // 3: Sample & Hold
    [](double /*phase*/, float noise) -> float {
        return (noise + 1.0f) * 0.5f;
    },
    // 4: Saw Down & One Shot
    [](double phase, float /*noise*/) -> float {
        return (float)(phase < 0.5 ? 1.0 - phase : 0.0);
    },
    // 5: Triangle & One Shot
    [](double phase, float /*noise*/) -> float {
        return (phase < 0.5) ? (float)(phase * 2.0) : 0.0f;
    }
} };

inline void N88LfoCore::updatePhaseDelta()
{
    this->m_pmPhaseDelta = (double)this->m_pmFreq / this->m_sampleRate;
    this->m_amPhaseDelta = (double)this->m_amFreq / this->m_sampleRate;
}

void N88LfoCore::prepare(double sampleRate) {
    this->amSmooth = 0.0f;

    updateTargetSampleRate(sampleRate);
}

void N88LfoCore::updateTargetSampleRate(double newSampleRate) {
    this->m_sampleRate = newSampleRate;

    updatePhaseDelta();
}

void N88LfoCore::setParameters(int syncDelay, bool pm, bool am, float pmFreq, float amFreq, int pmIndex, int amIndex, float pms, float pmd, float amd, float amSmoothRate)
{
	this->m_sdParam = syncDelay;
	this->m_sd = (float)(m_sdParam - 1) * (1000.0f / 60.0f);

    this->pmEnable = pm;
    this->m_pmFreq = pmFreq;
    this->m_pmWaveIndex = std::clamp(pmIndex, 0, 5);
    this->m_isOneshotPm = this->m_pmWaveIndex == 4 || this->m_pmWaveIndex == 5;

    this->pms = pms;
    this->pmd = pmd;

    this->amEnable = am;
    this->m_amFreq = amFreq;
    this->m_amWaveIndex = std::clamp(amIndex, 0, 5);
    this->m_isOneshotAm = this->m_amWaveIndex == 4 || this->m_amWaveIndex == 5;
    this->ams = 0.0f;
    this->amd = amd;

    this->signDb = (this->amd < 0.0f) ? -1.0f : 1.0f;
    this->depthDb = std::abs(this->amd / 127.0f);
    this->depthNorm = (this->pmd / 127.0f) * (this->pms / 15.0f);

    this->m_amSmoothRate = amSmoothRate;

    updatePhaseDelta();
}

void N88LfoCore::noteOn()
{
    // LFO Sync Delay が 0より大きければ、位相をリセット(Sync)してディレイ開始
    if (this->m_sdParam == 0) {
        this->m_sdCounter = 0.0f; // フリーラン継続

        // ディレイ0(フリーラン設定)であっても、ワンショット波形の時は
        // 毎回アタマから再生されないと不自然なので強制的にSyncさせる
        if (this->m_isOneshotPm || this->m_isOneshotAm) {
            this->m_pmPhase = 0.0;
            this->m_amPhase = 0.0;
        }
    }
    else if (this->m_sdParam == 1) {
        // 位相を0に戻す (Sync)
        this->m_pmPhase = 0.0;
        this->m_amPhase = 0.0;

        this->m_sdCounter = 0.0f; // ms -> 秒
    }
    else {
        // 位相を0に戻す (Sync)
        this->m_pmPhase = 0.0;
        this->m_amPhase = 0.0;

        this->m_sdCounter = this->m_sd / 1000.0f; // ms -> 秒
    }

    this->m_pmCycleCount = 0;
    this->m_amCycleCount = 0;
}

void N88LfoCore::getSample()
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
            pmVal = pmStrategies[this->m_pmWaveIndex](this->m_pmPhase, this->m_currentNoiseSample);
            amVal = amStrategies[this->m_amWaveIndex](this->m_amPhase, this->m_currentNoiseSample);

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