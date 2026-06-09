#include <JuceHeader.h>
#include <algorithm>

#include "./LfoOpzx7Unit.h"

Opzx7LfoCoreUnit::Opzx7LfoCoreUnit() {
}

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
    switch (this->m_sdIndex) {
    case 0:
        // フリーラン継続
        this->m_sdCounter = 0.0f;

        // ディレイ0(フリーラン設定)であっても、ワンショット波形の時は
        // 毎回アタマから再生されないと不自然なので強制的にSyncさせる
        if (this->m_isOneshot) {
            this->m_phase = 0.0;
        }

        break;
    case 1:
        // 位相を0に戻す (Sync)
        this->m_phase = 0.0;
        this->m_sdCounter = 0.0f;

        break;
    case 2:
        // 位相を0に戻す (Sync)
        this->m_phase = 0.0;
        this->m_sdCounter = this->m_sd / 1000.0f;
    }

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
            switch (this->m_waveIndex) {
            case 0:
                val = (float)std::sin(this->m_phase * 2.0 * juce::MathConstants<double>::pi);

                break;
            case 1:
                if (this->m_phase < 0.5) val = (float)(this->m_phase * 2.0);
                else                     val = (float)(-1.0 + (this->m_phase - 0.5) * 2.0);

                break;
            case 2:
                val = (float)(1.0 - this->m_phase * 2.0);

                break;
            case 3:
                val = (this->m_phase < 0.5) ? 1.0f : -1.0f;

                break;
            case 4:
                if (this->m_phase < 0.25)       val = (float)(this->m_phase * 4.0);
                else if (this->m_phase < 0.75)  val = (float)(1.0 - (this->m_phase - 0.25) * 4.0);
                else                            val = (float)(-1.0 + (this->m_phase - 0.75) * 4.0);

                break;
            case 5:
                val = this->m_currentNoiseSample;

                break;
            case 6:
                val = (float)(this->m_phase < 0.5 ? 1.0 - this->m_phase * 2.0 : 0.0);

                break;
            case 7:
                if (this->m_phase < 0.25)      val = (float)(this->m_phase * 4.0);
                else if (this->m_phase < 0.5)  val = (float)(1.0 - (this->m_phase - 0.25) * 4.0);
                else                           val = 0.0;
            }

            // ワンショット波形 (6, 7) のミュート処理
            if (this->m_isOneshot && this->m_sdCycleCount > 0) val = 0.0f;
        }
    }

    return val;
}

float Opzx7LfoCoreUnit::getSamplePm()
{
    if (!this->enable) {
        return 0.0f;
    }

    return getSample();
}

float Opzx7LfoCoreUnit::getSampleAm()
{
    if (!this->enable) {
        return 1.0f;
    }

    float val = getSample();

    // AMクリックノイズ防止スムージング
    this->m_smooth += (val - this->m_smooth) * this->m_smoothRate;

    return this->m_smooth;
}
