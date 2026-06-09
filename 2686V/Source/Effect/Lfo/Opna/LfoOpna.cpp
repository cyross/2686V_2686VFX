#include <JuceHeader.h>
#include <algorithm>

#include "./LfoOpna.h"

OpnaLfoCore::OpnaLfoCore() {
}

const std::array<float, 8> OpnaLfoCore::freqs = { 3.98f, 5.56f, 6.02f, 6.37f, 6.88f, 9.63f, 48.1f, 72.2f };
// 新しい厳密なPMSデプス値 (0, 3.4, 6.7, 10, 14, 20, 40, 80 cent)
const std::array<float, 8> OpnaLfoCore::pmsDepths = { 0.0f, 0.001965f, 0.003876f, 0.005793f, 0.008122f, 0.011619f, 0.023374f, 0.047294f };
const std::array<float, 4> OpnaLfoCore::amsDepths = { 0.0f, 0.14886f, 0.49305f, 0.74296f };

inline void OpnaLfoCore::updatePhaseDelta()
{
    this->m_pmPhaseDelta = (double)this->m_pmFreq / this->m_sampleRate;
    this->m_amPhaseDelta = (double)this->m_amFreq / this->m_sampleRate;
}

void OpnaLfoCore::prepare(double sampleRate) {
    this->amSmooth = 0.0f;

    updateTargetSampleRate(sampleRate);
}

void OpnaLfoCore::updateTargetSampleRate(double newSampleRate) {
    this->m_sampleRate = newSampleRate;

    updatePhaseDelta();
}

void OpnaLfoCore::setParameters(int syncDelay, bool pm, bool am, int pmFreqIndex, int amFreqIndex, float pms, float ams, float amSmoothRate)
{
	this->m_sdParam = syncDelay;
    this->m_sdIndex = std::clamp(this->m_sdParam, 0, 2);
    this->m_sd = (float)(m_sdParam - 1) * (1000.0f / 60.0f);

    this->pmEnable = pm;
    this->m_pmFreq = freqs[std::clamp(pmFreqIndex, 0, 7)];
    this->pms = pmsDepths[std::clamp((int)pms, 0, 7)];

    this->amEnable = am;
    this->m_amFreq = freqs[std::clamp(amFreqIndex, 0, 7)];
    this->ams = amsDepths[std::clamp((int)ams, 0, 3)];

    this->m_amSmoothRate = amSmoothRate;

    updatePhaseDelta();
}

void OpnaLfoCore::noteOn()
{
    // LFO Sync Delay が 0より大きければ、位相をリセット(Sync)してディレイ開始
    switch (this->m_sdIndex) {
    case 0:
        this->m_sdCounter = 0.0f; // フリーラン継続

        break;
    case 1:
        // 位相を0に戻す (Sync)
        this->m_pmPhase = 0.0;
        this->m_amPhase = 0.0;

        this->m_sdCounter = 0.0f; // ms -> 秒

        break;
    case 2:
        // 位相を0に戻す (Sync)
        this->m_pmPhase = 0.0;
        this->m_amPhase = 0.0;

        this->m_sdCounter = this->m_sd / 1000.0f; // ms -> 秒
    }

    this->m_pmCycleCount = 0;
    this->m_amCycleCount = 0;
}

void OpnaLfoCore::getSample()
{
    if (!this->pmEnable && !this->amEnable) {
        // PMは即座に0.0に
        this->value.pm = 0.0f;

        // AMは「無効な状態(1.0f)」へ向けてスムージングを継続し、完全に到達したら計算をスキップ
        if (std::abs(this->amSmooth) > 0.0001f) {
            this->amSmooth += (0.0f - this->amSmooth) * this->m_amSmoothRate;
            this->value.am = 1.0f - (this->amSmooth * this->ams);
        }
        else {
            this->amSmooth = 0.0f;
            this->value.am = 1.0f; // 無効時のデフォルト値
        }

        // 余分な計算をせずに抜ける
        return;
    }

    float localAmLfo = 0.0f;
    float localPmLfo = 0.0f;

    // Sync Delay 更新
    if (this->m_sdCounter > 0.0f) {
        this->m_sdCounter -= 1.0f / (float)m_sampleRate;
        if (this->m_sdCounter < 0.0f) this->m_sdCounter = 0.0f;
    }

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
            switch (this->m_pgIndex) {
            case 0:
                localPmLfo = (float)std::sin(this->m_pmPhase * 2.0 * juce::MathConstants<double>::pi);

                break;
            case 1:
                localPmLfo = (float)(1.0 - this->m_pmPhase);

                break;
            case 2:
                localPmLfo = (this->m_pmPhase < 0.5) ? 1.0f : -1.0f;

                break;
            case 3:
                if (this->m_pmPhase < 0.25)       localPmLfo = (float)(this->m_pmPhase * 4.0);
                else if (this->m_pmPhase < 0.75)  localPmLfo = (float)(1.0 - (this->m_pmPhase - 0.25) * 4.0);
                else                              localPmLfo = (float)(-1.0 + (this->m_pmPhase - 0.75) * 4.0);

                break;
            case 4:
                localPmLfo = 0.0f;
            }

            switch (this->m_egIndex) {
            case 0:
                localAmLfo = ((float)std::sin(this->m_amPhase * 2.0 * juce::MathConstants<double>::pi) + 1.0f) * 0.5f;

                break;
            case 1:
                localAmLfo = (float)(1.0 - this->m_amPhase);

                break;
            case 2:
                localAmLfo = (this->m_amPhase < 0.5) ? 1.0f : 0.0f;

                break;
            case 3:
                localAmLfo = (this->m_amPhase < 0.5) ? (float)(this->m_amPhase * 2.0) : (float)(1.0 - (this->m_amPhase - 0.5) * 2.0);

                break;
            case 4:
                localAmLfo = (0.0f + 1.0f) * 0.5f;
            }
        }
    }

    // AMスムージング (クリックノイズ防止)
    this->amSmooth += (localAmLfo - this->amSmooth) * this->m_amSmoothRate;

    // ========================================================
    // 2. Pitch Modulation (Vibrato) の計算
    // ========================================================
    this->value.pm = this->pmEnable ? localPmLfo * this->pms : 0.0f;

    // ========================================================
    // 1. Amplitude Modulation (Tremolo) の計算
    // ========================================================
    this->value.am = this->amEnable ? (1.0f - (this->amSmooth * this->ams)) : 1.0f;
}
