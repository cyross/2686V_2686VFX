#pragma once
#include <JuceHeader.h>
#include <filesystem>
#include <fstream>
#if !defined(BUILD_AS_FX_PLUGIN)
#include "../synth/SynthVoice.h"
#include "../OPNA/PrOpna.h"
#include "../OPN/PrOpn.h"
#include "../OPL/PrOpl.h"
#include "../OPL3/PrOpl3.h"
#include "../OPM/PrOpm.h"
#include "../OPZX3/PrOpzx3.h"
#include "../SSG/PrSsg.h"
#include "../WT/PrWt.h"
#include "../RHYTHM/PrRhythm.h"
#include "../ADPCM/PrAdpcm.h"
#endif
#include "../FX/FxCore.h"
#include "../const/GlobalConstants.h"
#include "../fm/OpConstants.h"
#include "../const/FileConstants.h"
#include "../const/PresetConstants.h"
#include "../gui/LabelConstants.h"
#include "../fm/OpValueRange.h"
#include "../FX/PrFx.h"
#include "../editor/PluginEditor.h"

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
    juce::String presetName = defaultPresetName;
    juce::String presetAuthor = defaultPresetAuthor;
    juce::String presetVersion = defaultPresetVersion;
    juce::String presetComment = defaultPresetComment;
    juce::String presetPluginVersion = pluginVersion;

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

private:
#if !defined(BUILD_AS_FX_PLUGIN)
    OpnaProcessor prOpna;
    OpnProcessor prOpn;
    OplProcessor prOpl;
    Opl3Processor prOpl3;
    OpmProcessor prOpm;
    Opzx3Processor prOpzx3;
    SsgProcessor prSsg;
    WtProcessor prWt;
    RhythmProcessor prRhythm;
    AdpcmProcessor prAdpcm;
#endif
    FxProcessor prFx;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);

#if !defined(BUILD_AS_FX_PLUGIN)
    juce::Synthesiser m_synth;
#endif
    void loadStartupSettings(); // 設定の自動読み込み用関数
    void setPresetToXml(std::unique_ptr<juce::XmlElement>& xml);
    void getPresetFromXml(std::unique_ptr<juce::XmlElement>& xmlState);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686V)
};
