#pragma once
#include <JuceHeader.h>

#include "../Synth/SynthVoice.h"

#include "../../Processor/Opna/Core.h"
#include "../../Processor/Opn/Core.h"
#include "../../Processor/Opl/Core.h"
#include "../../Processor/Opl3/Core.h"
#include "../../Processor/Opm/Core.h"
#include "../../Processor/Opzx3/Core.h"
#include "../../Processor/Ssg/Core.h"
#include "../../Processor/Wavetable/Core.h"
#include "../../Processor/RHYTHM/Core.h"
#include "../../Processor/ADPCM/Core.h"
#include "../../Processor/BEEP/Core.h"
#include "../../Processor/Fx/Core.h"

#include "../Const/Global.h"
#include "../Const/PrKeys.h"
#include "../Const/PrValues.h"
#include "../Const/FileValues.h"
#include "../Const/PresetKeys.h"
#include "../Const/PresetValues.h"

#include "../Editor/PluginEditor.h"

class RetroSynthesiser : public juce::Synthesiser
{
public:
    bool isMonoMode = false;

    // 新しい音が鳴る時、どのボイス(回路)を使うかを決める関数をハックする
    juce::SynthesiserVoice* findFreeVoice(juce::SynthesiserSound* soundToPlay,
        int midiChannel,
        int midiNoteNumber,
        bool stealIfNoneAvailable) const override
    {
        if (isMonoMode)
        {
            // モノフォニック時は、和音が弾かれても「強制的にVoice 0（最初の回路）」だけを返す
            if (auto* voice = getVoice(0))
            {
                return voice; // 現在鳴っていても、容赦なく奪い取る(Steal)
            }
        }
        // ポリフォニック時(OFF)は、通常のJUCEの和音割り当て機能を使う
        return juce::Synthesiser::findFreeVoice(soundToPlay, midiChannel, midiNoteNumber, stealIfNoneAvailable);
    }

    // 鍵盤を離した時の挙動をハックする
    void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) override
    {
        if (isMonoMode)
        {
            // モノフォニック時特有のバグ(後から押した鍵盤が、前の鍵盤を離したせいで消える)を防ぐ
            if (auto* voice = getVoice(0))
            {
                // 「現在Voice0で鳴っているノート」と「離されたノート」が一致した時だけ音を消す
                if (voice->getCurrentlyPlayingNote() == midiNoteNumber)
                {
                    voice->stopNote(velocity, allowTailOff);
                }
            }
        }
        else
        {
            juce::Synthesiser::noteOff(midiChannel, midiNoteNumber, velocity, allowTailOff);
        }
    }
};

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
    void unloadAdpcmFile();
    // Function to load Rhythm sample file (Specific Pad)
    void loadRhythmFile(const juce::File& file, int padIndex);
    void unloadRhythmFile(int padIndex);

    juce::AudioFormatManager formatManager;
    juce::File lastSampleDirectory{ juce::File::getSpecialLocation(juce::File::userHomeDirectory) };

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;

    // --- Metadata ---
    juce::String presetName = PresetValue::MetaData::Initial::name;
    juce::String presetAuthor = PresetValue::MetaData::Initial::author;
    juce::String presetVersion = PresetValue::MetaData::Initial::version;
    juce::String presetComment = PresetValue::MetaData::Initial::comment;
    juce::String presetGenre = PresetValue::MetaData::Initial::genre;
    juce::String presetFilePath = "";
    juce::String presetPluginVersion = Global::Plugin::version;

    OscMode lastActiveSynthMode = OscMode::OPNA;

    // --- File Paths (To restore samples) ---
    juce::String adpcmFilePath;
    std::array<juce::String, 8> rhythmFilePaths;

    // --- Preset I/O ---
    void savePreset(const juce::File& file);
    void loadPreset(const juce::File& file);
    void initPreset();

    // --- OPZX3 PCM File ---
    std::array<std::vector<float>, 4> opzx3PcmBuffers;
    std::array<juce::String, 4> opzx3PcmFilePaths;

    void loadOpzx3PcmFile(int opIndex, const juce::File& file);
    void unloadOpzx3PcmFile(int opIndex);

    // --- OPZX3 Wavetable ---
    std::array<std::vector<float>, 4> opzx3WtBuffers;
    std::array<juce::String, 4> opzx3WtFilePaths;

    void loadOpzx3WtFile(int opIndex, const juce::File& file);
    void unloadOpzx3WtFile(int opIndex);

    // --- Preview(Static) ---
    void generatePreviewWaveform(std::vector<float>& destBuffer);

    // --- 仮想キーボード ---
    juce::MidiKeyboardState keyboardState;

    // --- Preview ---
    bool previewVisiblity = true; // Editorとの同期用
    std::atomic<float> realTimeBuffer[512];

    // --- Settings Data ---
    juce::String wallpaperPath;
    int wallpaperMode = 0; // 0=Stretch, 1=Fill, 2=Fit, 3=Original
    juce::String defaultSampleDir;  // For ADPCM & Rhythm
    juce::String defaultPresetDir; // For Presets
	juce::String defaultWavetableDir; // For Wavetables
    bool showTooltips = true; // For show Parameter Range Tooltop
    bool useHeadroom = true; // ヘッドルーム適応
    float headroomGain = 0.25; // ヘッドルーム圧縮値
    bool showVirtualKeyboard = true; // 仮想キーボードの表示フラグ（デフォルトON）

    void saveEnvironment(const juce::File& file);
    void loadEnvironment(const juce::File& file); 

    void panic();

    juce::String makePathRelative(const juce::File& targetFile); // 相対ディレクトリへ変換
    juce::File resolvePath(const juce::String& pathStr); // 相対ディレクトリからの展開

    juce::String getDefaultPresetDir();
    static juce::String sanitizeString(const juce::String& input, int length);
private:
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
    BeepProcessor prBeep;
    FxProcessor prFx;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);

    RetroSynthesiser m_synth;

    // 波形プレビュー用
    juce::Synthesiser previewSynth;
    std::unique_ptr<SynthSound> previewSound;
    void loadStartupSettings(); // 設定の自動読み込み用関数
    void setPresetToXml(std::unique_ptr<juce::XmlElement>& xml);
    void getPresetFromXml(std::unique_ptr<juce::XmlElement>& xmlState);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686V)
};
