#pragma once

#include <JuceHeader.h>

#include "./SynthMode.h"
#include "./SynthParams.h"
#include "./SynthCore.h"

#include "../../Synth/Opzx7/SynthOpzx7.h"
#include "../../Advanced/Curve/AdvancedCurve.h"

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();

    void prepare(double sampleRate);
    void setParameters(const SynthParams& params);

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }

    void startNote(int midiNote, float velocity, juce::SynthesiserSound*, int) override;

    void stopNote(float, bool allowTailOff) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void setCurrentPlaybackSampleRate(double newRate) override;

    // ピッチベンド
    void pitchWheelMoved(int newPitchWheelValue) override;

    // コントローラー (CC)
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void setOpzx7PcmBuffer(int opIndex, std::vector<float>* pcmData); 

    void setOpzx7WtBuffer(int opIndex, std::vector<float>* wtData);

    void setOpzx7Wt2Buffer(int opIndex, std::vector<float>* wtData);

    void clearOpzx7PcmBuffer(int opIndex);

    void clearOpzx7WtBuffer(int opIndex);

    void clearOpzx7Wt2Buffer(int opIndex);

    void setCurveCore(CurveCore* p_curveCore);

    bool isPlaying();

    std::map<OscMode, SynthCore *> coreMap;

    // ユニゾン・ハーモニー用
    void setUnisonParams(int index, int total, float detune, float spread) 
    {
        coreMap[m_mode]->setUnisonParams(index, total, detune, spread);
    }
private:
    OscMode m_mode = OscMode::OPZX7;
    Opzx7Core m_opzx7Core;
};
