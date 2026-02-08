#pragma once
#include <JuceHeader.h>
#include "SynthVoice.h"

class AudioPlugin2686V : public juce::AudioProcessor
{
public:
    AudioPlugin2686V();
    ~AudioPlugin2686V() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
    void createOpnaParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOpnParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOplParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createSsgParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createWavetableParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createRhythmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createAdpcmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);

    void processOpnaBlock(SynthParams &params);
    void processOpnBlock(SynthParams &params);
    void processOplBlock(SynthParams &params);
    void processSsgBlock(SynthParams &params);
    void processWavetableBlock(SynthParams &params);
    void processRhythmBlock(SynthParams &params);
    void processAdpcmBlock(SynthParams &params);

    juce::Synthesiser m_synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686V)
};
