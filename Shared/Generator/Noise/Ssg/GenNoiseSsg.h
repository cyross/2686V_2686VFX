#pragma once

#include "./GenNoiseSsgParams.h"
#include "Shared/Generator/Pulse/GenTnd2A03.h"

class SsgNoiseGen
{
    unsigned int lfsr = 0x1FFFF;

    float level = 1.0f;
    bool noiseOnNote = false;
    float phase = 0.0f;
    float delta = 0.0f;
    float baseFreq = 12000.0f; // Slider Value
    float targetFreq = 12000.0f; // Active Frequency
    double targetSampleRate = 44100.0;

    float currentSample = 0.0f;

    // More Real FC (4.3.0)。ファミコン (2A03) の 15bit LFSR (長周期) で鳴らし、
    // 進む速さを 16 段の表へ寄せる。出力は 0 / 1 の単極
    bool fcMode = false;
    unsigned int lfsrFc = Tnd2A03::noiseInitial;
public:
    void prepare(double newTargetRate);
    void setParameters(const SsgNoiseGenParams &params);
	void updateTargetRate(double newTargetRate) {
        this->targetSampleRate = newTargetRate;
        updateDelta();
    }
    void generate();
    void updateDelta();
	void updateFrequency(float currentFreq);
    float generateSample(float gain);

    void setFcMode(bool on)
    {
        if (on == this->fcMode) return;

        this->fcMode = on;
        this->lfsrFc = Tnd2A03::noiseInitial;
        this->currentSample = 0.0f;

        updateDelta();
    }

    // いまの出力の高低 (0 / 1)。SSG のトーンとの論理積 (More Real SSG) に使う
    float bit() const { return this->currentSample > 0.0f ? 1.0f : 0.0f; }

    // NOISE の大きさ (つまみの値)
    float gainLevel() const { return this->level; }
};
