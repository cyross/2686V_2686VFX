#include <JuceHeader.h>
#include <algorithm>

#include "./LfoOpzx7Unit.h"
#include "../../../Core/Const/ConstGlobal.h"

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

void Opzx7LfoCoreUnit::setParameters(int syncDelay, bool enable, float freq, int index, float ms, float md, float smoothRate,
    const WaveHoldParams& hold)
{
	this->m_sdParam = syncDelay;
    this->m_sdIndex = std::clamp(this->m_sdParam, 0, 2);
    this->m_sd = (float)(m_sdParam - 1) * (1000.0f / 60.0f);

    this->enable = enable;
    this->m_freq = freq;
    this->m_waveIndex = std::clamp(index, 0, 12);
    this->m_isOneshot = this->m_waveIndex == 6 || this->m_waveIndex == 7;

    this->ms = ms;
    this->md = md;

    this->depthDb = (this->ms * this->md) * Global::Lfo::maxAmDepthDb;
    this->depthCent = (this->ms * this->md) * 1200.0f;

    this->m_smoothRate = smoothRate;
    this->m_hold.setParameters(hold);

    updatePhaseDelta();
}

void Opzx7LfoCoreUnit::noteOn()
{
    this->m_hold.reset();

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

    // Sample & Hold はキーオンの時点で 1 つ値を持っておく。
    // ここが 0 のままだと、最初のホールドが終わるまで出力が 0 に張り付く。
    this->m_currentNoiseSample = m_noiseGen.generate();
    this->m_currentHoldingSample = this->m_currentNoiseSample;
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

                // 決めた回数まで回したら、そこから先は保つ。
                // もともと 1 周で止まるワンショット波形へは掛けない。
                if (!this->m_isOneshot) this->m_hold.countCycle();
            }

            // 部分再生。波形を引く位相だけを動かし、進み方は変えない。
            double phase = this->m_phase;

            const bool muted = this->m_hold.windowPhase(phase);

            // (※ノイズが必要な場合は共有のノイズジェネレータか乱数を使用)
            switch (this->m_waveIndex) {
            case 0:
                val = (float)std::sin(phase * 2.0 * juce::MathConstants<double>::pi);

                break;
            case 1:
                if (phase < 0.5) val = (float)(phase * 2.0);
                else                     val = (float)(-1.0 + (phase - 0.5) * 2.0);

                break;
            case 2:
                val = (float)(1.0 - phase * 2.0);

                break;
            case 3:
                val = (phase < 0.5) ? 1.0f : -1.0f;

                break;
            case 4:
                if (phase < 0.25)       val = (float)(phase * 4.0);
                else if (phase < 0.75)  val = (float)(1.0 - (phase - 0.25) * 4.0);
                else                            val = (float)(-1.0 + (phase - 0.75) * 4.0);

                break;
            case 5:
                val = this->m_currentNoiseSample;

                break;
            case 6:
                val = (float)(phase < 0.5 ? 1.0 - phase * 2.0 : 0.0);

                break;
            case 7:
                if (phase < 0.25)      val = (float)(phase * 4.0);
                else if (phase < 0.5)  val = (float)(1.0 - (phase - 0.25) * 4.0);
                else                           val = 0.0;

                break;
            case 8:
                if (this->m_sdCycleCount % 4 == 0) {
                    this->m_currentHoldingSample = this->m_currentNoiseSample;
                }
                val = this->m_currentHoldingSample;

                break;
            case 9:
                if (this->m_sdCycleCount % 8 == 0) {
                    this->m_currentHoldingSample = this->m_currentNoiseSample;
                }
                val = this->m_currentHoldingSample;

                break;
            case 10:
                if (this->m_sdCycleCount % 16 == 0) {
                    this->m_currentHoldingSample = this->m_currentNoiseSample;
                }
                val = this->m_currentHoldingSample;

                break;
            case 11:
                if (this->m_sdCycleCount % 32 == 0) {
                    this->m_currentHoldingSample = this->m_currentNoiseSample;
                }
                val = this->m_currentHoldingSample;

                break;
            case 12:
                if (this->m_sdCycleCount % 64 == 0) {
                    this->m_currentHoldingSample = this->m_currentNoiseSample;
                }
                val = this->m_currentHoldingSample;

                break;
            }

            // ワンショット波形 (6, 7) のミュート処理
            if (this->m_isOneshot && this->m_sdCycleCount > 0) val = 0.0f;

            // 区間の外で端の値を保たない側は 0 にする
            if (muted) val = 0.0f;

            // 決めた回数まで回したら、キーが離れるまで保つ
            if (this->m_hold.isHolding()) val = this->m_hold.holdValue();
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
        return 0.0f; // 減衰なし (呼び出し側は depthDb を掛けて dB 減衰に使う)
    }

    // 波形は -1.0〜1.0 のバイポーラ。そのまま dB 減衰に使うと
    // 負の半周期でゲインが 1.0 を超えて爆音になるため、0.0〜1.0 に直す。
    float val = (getSample() + 1.0f) * 0.5f;

    // AMクリックノイズ防止スムージング
    this->m_smooth += (val - this->m_smooth) * this->m_smoothRate;

    return this->m_smooth;
}
