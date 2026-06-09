#include <JuceHeader.h>
#include <algorithm>

#include "./LfoOpm.h"

OpmLfoCore::OpmLfoCore() {
}

const std::array<float, 4> OpmLfoCore::amsDepths = { 0.0f, 0.93622f, 0.99593f, 0.99998f };

// (2^(Cent/1200) - 1.0)
// 0: 0cent / 1: ±5cent / 2: ±10cent / 3: ±20cent / 4: ±50cent / 5: ±100cent / 6: ±400cent / 7: ±700cent
const std::array<float, 8> OpmLfoCore::pmsDepths = { 0.0f, 0.002892f, 0.005793f, 0.011619, 0.029302, 0.059463f, 0.259921f, 0.498307f };

inline void OpmLfoCore::updatePhaseDelta()
{
    this->m_pmPhaseDelta = (double)this->m_pmFreq / this->m_sampleRate;
    this->m_amPhaseDelta = (double)this->m_amFreq / this->m_sampleRate;
}

void OpmLfoCore::prepare(double sampleRate) {
    this->amSmooth = 0.0f;

    updateTargetSampleRate(sampleRate);
}

void OpmLfoCore::updateTargetSampleRate(double newSampleRate) {
    this->m_sampleRate = newSampleRate;

    updatePhaseDelta();
}

void OpmLfoCore::setParameters(int syncDelay, bool pm, bool am, float pmFreq, float amFreq, int pgIndex, int egIndex, int pmsIndex, int pmd, int amsIndex, int amd, float amSmoothRate)
{
	this->m_sdParam = syncDelay;
    this->m_sdIndex = std::clamp(this->m_sdParam, 0, 2);
    this->m_sd = (float)(m_sdParam - 1) * (1000.0f / 60.0f);

    this->pmEnable = pm;
    this->m_pmFreq = pmFreq;
    this->m_pmWaveIndex = std::clamp(pgIndex, 0, 7);
    this->m_isOneshotPm = this->m_pmWaveIndex == 6 || this->m_pmWaveIndex == 7;

    this->m_pmsIndex = std::clamp(pmsIndex, 0, 3);
    this->pms = pmsDepths[this->m_pmsIndex];
    this->pmd = (float)pmd;

    this->amEnable = am;
    this->m_amFreq = amFreq;
    this->m_amWaveIndex = std::clamp(egIndex, 0, 7);
    this->m_isOneshotAm = this->m_amWaveIndex == 6 || this->m_amWaveIndex == 7;

    this->m_amsIndex = std::clamp(amsIndex, 0, 7);
    this->ams = amsDepths[this->m_amsIndex];
    this->amd = (float)amd;

    this->depthDb = (this->amd >= 0.0f) ? (this->amd / 127.0f) : 1.0f;
    this->depthCent = (this->pmd >= 0.0f) ? (this->pmd / 127.0f) : 1.0f;

    this->m_amSmoothRate = amSmoothRate;

    updatePhaseDelta();
}

void OpmLfoCore::noteOn()
{
    // LFO Sync Delay が 0より大きければ、位相をリセット(Sync)してディレイ開始
    switch (this->m_sdIndex) {
    case 0:
        this->m_sdCounter = 0.0f; // フリーラン継続

        // ディレイ0(フリーラン設定)であっても、ワンショット波形の時は
        // 毎回アタマから再生されないと不自然なので強制的にSyncさせる
        if (this->m_isOneshotPm || this->m_isOneshotAm) {
            this->m_pmPhase = 0.0;
            this->m_amPhase = 0.0;
        }

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

void OpmLfoCore::getSample()
{
    // 両方無効なら何も計算せずに現在のスムージング値だけ返して終了
    if (!this->pmEnable && !this->amEnable) {
        this->value.pm = 0.0f;
        // AMのスムージング（0に向かって減衰する処理）だけは行うか、必要に応じて処理
        this->amSmooth += (0.0f - this->amSmooth) * this->m_amSmoothRate;
        this->value.am = this->amSmooth;

        return;
    }

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
            switch (this->m_pmWaveIndex) {
            case 0:
                if (this->m_pmPhase < 0.5) pmVal = (float)(this->m_pmPhase * 2.0);
                else                       pmVal = (float)(-1.0 + (this->m_pmPhase - 0.5) * 2.0);

                break;
            case 1:
                pmVal = (this->m_pmPhase < 0.5) ? 1.0f : -1.0f;

                break;
            case 2:
                if (this->m_pmPhase < 0.25)      pmVal = (float)(this->m_pmPhase * 4.0);
                else if (this->m_pmPhase < 0.75) pmVal = (float)(1.0 - (this->m_pmPhase - 0.25) * 4.0);
                else                             pmVal = (float)(-1.0 + (this->m_pmPhase - 0.75) * 4.0);

                break;
            case 3:
                pmVal = this->m_currentNoiseSample;
            }

            // ワンショット波形 (6, 7) のミュート処理
            if (this->m_isOneshotPm && this->m_pmCycleCount > 0) pmVal = 0.0f;

            switch (this->m_amWaveIndex) {
            case 0:
                amVal = (float)(1.0 - this->m_pmPhase);
                break;
            case 1:
                amVal = (this->m_pmPhase < 0.5) ? 1.0f : 0.0f;
                break;
            case 2:
                if (this->m_pmPhase < 0.5) amVal = (float)(1.0 - this->m_pmPhase * 2.0);
                else                       amVal = (float)((this->m_pmPhase - 0.5) * 2.0);

                break;
            case 3:
                amVal = (this->m_currentNoiseSample + 1.0f) * 0.5f;
            }

            // ワンショット波形 (6, 7) のミュート処理
            if (this->m_isOneshotAm && this->m_amCycleCount > 0) amVal = 0.0f;
        }
    }

    // AMクリックノイズ防止スムージング
    this->amSmooth += (amVal - this->amSmooth) * this->m_amSmoothRate;

    this->value.am = this->amSmooth;
    this->value.pm = pmVal;
}