#pragma once

#include "./SynthParams.h"

class SynthCore
{
public:
    bool m_pitchResetOnLegato = false;
    void virtual prepare(double sampleRate) {};
    void virtual setSampleRate(double sampleRate) {};
    void virtual setParameters(const SynthParams& params) {};
    void virtual noteOn(float freq, float velocity, int midiNote, bool isLegato = false) {};
    void virtual noteOff() {};
    bool virtual isPlaying() const { return false;  };
    void virtual setPitchBend(int pitchWheelValue) {};
    void virtual setPitchBendRatio(float ratio) {};
    void virtual setModulationWheel(int wheelValue) {};
    float virtual getSample() { return 0.0f; };
    void virtual renderNextBlock(float * outR, float* outL, int startSample, int sampleIdx, bool& isActive) {};
    void virtual setUnisonParams(int index, int total, float detune, float spread) {};
};