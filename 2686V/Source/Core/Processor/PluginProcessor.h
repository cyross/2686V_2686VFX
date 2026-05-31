#pragma once
#include <JuceHeader.h>
#include <algorithm>

#include "../Synth/SynthVoice.h"

#include "../Fm/FmClipboard.h"

#include "../../Processor/Opna/Core.h"
#include "../../Processor/Opn/Core.h"
#include "../../Processor/Opl/Core.h"
#include "../../Processor/Opl3/Core.h"
#include "../../Processor/Opm/Core.h"
#include "../../Processor/Opzx7/Core.h"
#include "../../Processor/Ssg/Core.h"
#include "../../Processor/Wavetable/Core.h"
#include "../../Processor/RHYTHM/Core.h"
#include "../../Processor/ADPCM/Core.h"
#include "../../Processor/BEEP/Core.h"
#include "../../Processor/Fx/Core.h"
#include "../../Processor/Curve/Core.h"

#include "../Const/Global.h"
#include "../Processor/Keys.h"
#include "../Processor/Values.h"
#include "../Const/FileValues.h"
#include "../../Gui/Preset/Keys.h"
#include "../../Gui/Preset/Values.h"

#include "../Editor/PluginEditor.h"

class RetroSynthesiser : public juce::Synthesiser
{
private:
    std::map<OscMode, std::function<void(int midiChannel, int midiNoteNumber, float velocity)>> voiceUnisonMap;
public:
    RetroSynthesiser() : juce::Synthesiser() {
        voiceUnisonMap[OscMode::OPNA] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->opna.unisonVoices,
                currentParams->opna.unisonDetuneCents,
                currentParams->opna.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
        voiceUnisonMap[OscMode::OPN] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->opn.unisonVoices,
                currentParams->opn.unisonDetuneCents,
                currentParams->opn.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
        voiceUnisonMap[OscMode::OPL] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->opl.unisonVoices,
                currentParams->opl.unisonDetuneCents,
                currentParams->opl.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
        voiceUnisonMap[OscMode::OPL3] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->opl3.unisonVoices,
                currentParams->opl3.unisonDetuneCents,
                currentParams->opl3.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
        voiceUnisonMap[OscMode::OPM] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->opm.unisonVoices,
                currentParams->opm.unisonDetuneCents,
                currentParams->opm.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
        voiceUnisonMap[OscMode::OPZX7] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->opzx7.unisonVoices,
                currentParams->opzx7.unisonDetuneCents,
                currentParams->opzx7.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
        voiceUnisonMap[OscMode::SSG] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->ssg.unisonVoices,
                currentParams->ssg.unisonDetuneCents,
                currentParams->ssg.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
        voiceUnisonMap[OscMode::WAVETABLE] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->wt.unisonVoices,
                currentParams->wt.unisonDetuneCents,
                currentParams->wt.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
        voiceUnisonMap[OscMode::RHYTHM] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->rhythm.unisonVoices,
                currentParams->rhythm.unisonDetuneCents,
                currentParams->rhythm.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
        voiceUnisonMap[OscMode::ADPCM] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->adpcm.unisonVoices,
                currentParams->adpcm.unisonDetuneCents,
                currentParams->adpcm.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
        voiceUnisonMap[OscMode::BEEP] = [=](int midiChannel, int midiNoteNumber, float velocity) {
            voiceUnison(
                currentParams->beep.unisonVoices,
                currentParams->beep.unisonDetuneCents,
                currentParams->beep.unisonSpread,
                midiChannel,
                midiNoteNumber,
                velocity
            );
            };
    }

    bool isMonoMode = false;

    SynthParams* currentParams = nullptr;

    void voiceUnison(int voices, int detune, float spread, int midiChannel, int midiNoteNumber, float velocity)
    {
        int uVoices = voices; // (※モードに応じて切り替えるように後で調整)

        // 1ボイスなら通常動作
        if (uVoices <= 1) {
            if (auto* voice = dynamic_cast<SynthVoice*>(findFreeVoice(getSound(0).get(), midiChannel, midiNoteNumber, true))) {
                voice->setUnisonParams(0, 1, 0.0f, 0.0f);

                startVoice(voice, getSound(0).get(), midiChannel, midiNoteNumber, velocity);
            }
            return;
        }

        // ユニゾンの場合：指定された数だけボイスを確保して鳴らす
        for (int i = 0; i < uVoices; ++i)
        {
            juce::SynthesiserVoice* rawVoice = nullptr;

            if (isMonoMode) {
                // モノフォニック時は、ユニゾン数ぶんの専用ボイス(0番目から順に)を強制使用する
                rawVoice = getVoice(i);
                if (rawVoice != nullptr && rawVoice->getCurrentlyPlayingNote() != -1) {
                    // もし前の音が鳴っていたら安全に止める
                    rawVoice->stopNote(0.0f, false);
                }
            }
            else {
                // ポリフォニック時はJUCEの空きボイス検索を使う
                rawVoice = findFreeVoice(getSound(0).get(), midiChannel, midiNoteNumber, true);
            }

            if (auto* voice = dynamic_cast<SynthVoice*>(rawVoice))
            {
                // ユニゾンのインデックス(0, 1, 2...)と総数をVoiceに教える
                voice->setUnisonParams(i, uVoices, detune, spread);

                // ここで実際に音を鳴らす
                startVoice(voice, getSound(0).get(), midiChannel, midiNoteNumber, velocity);
            }
        }
    }

    // ユニゾン・ハーモニー向けにオーバーライド
    // 鍵盤を押した時の挙動をハックする
    void noteOn(int midiChannel, int midiNoteNumber, float velocity) override
    {
        if (currentParams == nullptr) {
            juce::Synthesiser::noteOn(midiChannel, midiNoteNumber, velocity);
            return;
        }

        voiceUnisonMap[currentParams->mode](midiChannel, midiNoteNumber, velocity);
    }

    // ユニゾン・ハーモニー向けにオーバーライド
    // 鍵盤を離した時の挙動をハックする
    void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) override
    {
        if (isMonoMode)
        {
            // モノフォニック時でも、ユニゾンで複数ボイスが鳴っている可能性があるため、全ボイスをループで止める
            for (int i = 0; i < getNumVoices(); ++i)
            {
                if (auto* voice = getVoice(i))
                {
                    // 「現在鳴っているノート」と「離されたノート」が一致した時だけ音を消す
                    if (voice->getCurrentlyPlayingNote() == midiNoteNumber)
                    {
                        voice->stopNote(velocity, allowTailOff);
                    }
                }
            }
        }
        else
        {
            juce::Synthesiser::noteOff(midiChannel, midiNoteNumber, velocity, allowTailOff);
        }
    }

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
};

class AudioPlugin2686V : public juce::AudioProcessor
{
public:
    AudioPlugin2686V();
    ~AudioPlugin2686V() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

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

    void initParams(const juce::String& code);

    // --- OPZX7 PCM File ---
    std::array<std::vector<float>, 4> opzx7PcmBuffers;
    std::array<juce::String, 4> opzx7PcmFilePaths;

    void loadOpzx7PcmFile(int opIndex, const juce::File& file);
    void unloadOpzx7PcmFile(int opIndex);

    // --- OPZX7 Wavetable ---
    std::array<std::vector<float>, 4> opzx7WtBuffers;
    std::array<juce::String, 4> opzx7WtFilePaths;

    void loadOpzx7WtFile(int opIndex, const juce::File& file);
    void unloadOpzx7WtFile(int opIndex);

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
    juce::String makeWtPathRelative(const juce::File& targetFile); // 相対ディレクトリへ変換
    juce::File resolveWtPath(const juce::String& pathStr); // 相対ディレクトリからの展開

    juce::String getDefaultPresetDir();
    static juce::String sanitizeString(const juce::String& input, int length);
    CurveCore* getCurveCore();

    FmClipboard fmClipboard;
private:
    OpnaProcessor prOpna;
    OpnProcessor prOpn;
    OplProcessor prOpl;
    Opl3Processor prOpl3;
    OpmProcessor prOpm;
    Opzx7Processor prOpzx7;
    SsgProcessor prSsg;
    WtProcessor prWt;
    RhythmProcessor prRhythm;
    AdpcmProcessor prAdpcm;
    BeepProcessor prBeep;
    FxProcessor prFx;
	CurveProcessor prCurve;

    CurveCore m_curveCore;

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
