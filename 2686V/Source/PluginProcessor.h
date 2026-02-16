#pragma once
#include <JuceHeader.h>
#if !defined(BUILD_AS_FX_PLUGIN)
#include "SynthVoice.h"
#endif
#include "SimpleEffects.h"

#if defined(BUILD_AS_FX_PLUGIN)
static const juce::String VstName = "2686VFX";
#else
static const juce::String VstName = "2686V";
#endif
static const juce::String VstVersion = "Version 0.0.3";
static const juce::String VstAuthor = "Copyright (C) 2026 CYROSS";

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
#if !defined(BUILD_AS_FX_PLUGIN)
    // Function to load ADPCM file (Global/Voice)
    void loadAdpcmFile(const juce::File& file);
    void unloadAdpcmFile();
    // Function to load Rhythm sample file (Specific Pad)
    void loadRhythmFile(const juce::File& file, int padIndex);
    void unloadRhythmFile(int padIndex);

    juce::AudioFormatManager formatManager;
    juce::File lastSampleDirectory{ juce::File::getSpecialLocation(juce::File::userHomeDirectory) };
#endif

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState apvts;

#if !defined(BUILD_AS_FX_PLUGIN)
    // --- Metadata ---
    juce::String presetName = "Init Preset";
    juce::String presetAuthor = "User";
    juce::String presetVersion = "1.0";
    juce::String presetComment = "";
    juce::String presetPluginVersion = VstVersion;

    static const int presetNameLength = 512;
    static const int presetAuthorLength = 128;
    static const int presetVersionLength = 64;
    static const int commentLength = 2048; // コメント欄の長さ(2048文字まで)

    // --- File Paths (To restore samples) ---
    juce::String adpcmFilePath;
    std::array<juce::String, 8> rhythmFilePaths;

    // --- Preset I/O ---
    void savePreset(const juce::File& file);
    void loadPreset(const juce::File& file);
    void initPreset();
#endif

    // --- Settings Data ---
    juce::String wallpaperPath;
    juce::String defaultSampleDir;  // For ADPCM & Rhythm
    juce::String defaultPresetDir; // For Presets
    bool showTooltips = true; // For show Parameter Range Tooltop
    bool useHeadroom = true; // ヘッドルーム適応
    float headroomGain = 0.25; // ヘッドルーム圧縮値

    void saveEnvironment(const juce::File& file);
    void loadEnvironment(const juce::File& file); 

    juce::String makePathRelative(const juce::File& targetFile); // 相対ディレクトリへ変換
    juce::File resolvePath(const juce::String& pathStr); // 相対ディレクトリからの展開

    juce::String getDefaultPresetDir();
    static juce::String sanitizeString(const juce::String& input, int length);

    // --- Effect ---
    EffectChain effects;
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);
#if !defined(BUILD_AS_FX_PLUGIN)
    void createOpnaParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOpnParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOplParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOpl3ParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOpmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createOpzx3ParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createSsgParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createWavetableParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createRhythmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
    void createAdpcmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
#endif
    void createFxParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);

#if !defined(BUILD_AS_FX_PLUGIN)
    void processOpnaBlock(SynthParams &params);
    void processOpnBlock(SynthParams &params);
    void processOplBlock(SynthParams &params);
    void processOpl3Block(SynthParams& params);
    void processOpmBlock(SynthParams& params);
    void processOpzx3Block(SynthParams& params);
    void processSsgBlock(SynthParams &params);
    void processWavetableBlock(SynthParams &params);
    void processRhythmBlock(SynthParams &params);
    void processAdpcmBlock(SynthParams &params);
#endif
    void processFxBlock(juce::AudioBuffer<float>& buffer, SynthParams& params);

#if !defined(BUILD_AS_FX_PLUGIN)
    juce::Synthesiser m_synth;
#endif
    void loadStartupSettings(); // 設定の自動読み込み用関数
    void setPresetToXml(std::unique_ptr<juce::XmlElement>& xml);
    void getPresetFromXml(std::unique_ptr<juce::XmlElement>& xmlState);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686V)
};
