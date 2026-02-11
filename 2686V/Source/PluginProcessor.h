#pragma once
#include <JuceHeader.h>
#include "SynthVoice.h"
#include "SimpleEffects.h"

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
    // Function to load ADPCM file (Global/Voice)
    void loadAdpcmFile(const juce::File& file);
    // Function to load Rhythm sample file (Specific Pad)
    void loadRhythmFile(const juce::File& file, int padIndex);

    juce::AudioFormatManager formatManager;
    juce::File lastAdpcmDirectory{ juce::File::getSpecialLocation(juce::File::userHomeDirectory) };

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState apvts;

    // --- Metadata ---
    juce::String presetName = "Init Preset";
    juce::String presetAuthor = "User";
    juce::String presetVersion = "1.0";

    // --- File Paths (To restore samples) ---
    juce::String adpcmFilePath;
    std::array<juce::String, 8> rhythmFilePaths;

    // --- Preset I/O ---
    void savePreset(const juce::File& file);
    void loadPreset(const juce::File& file);

    // --- Settings Data ---
    juce::String wallpaperPath;
    juce::String defaultAdpcmDir;  // For ADPCM & Rhythm
    juce::String defaultPresetDir; // For Presets
    bool showTooltips = true; // For show Parameter Range Tooltop

    void saveEnvironment(const juce::File& file);
    void loadEnvironment(const juce::File& file); 

    // --- Effect ---
    EffectChain effects;
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
    void createOpnaParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOpnParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOplParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOpllParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOpl3ParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOpmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createSsgParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createWavetableParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createRhythmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createAdpcmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createFxParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);

    void processOpnaBlock(SynthParams &params);
    void processOpnBlock(SynthParams &params);
    void processOplBlock(SynthParams &params);
    void processOpllBlock(SynthParams& params);
    void processOpl3Block(SynthParams& params);
    void processOpmBlock(SynthParams& params);
    void processSsgBlock(SynthParams &params);
    void processWavetableBlock(SynthParams &params);
    void processRhythmBlock(SynthParams &params);
    void processAdpcmBlock(SynthParams &params);
    void processFxBlock(juce::AudioBuffer<float>& buffer, SynthParams& params);

    juce::Synthesiser m_synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686V)
};
