#pragma once
#include <map>
#include <JuceHeader.h>

#include "../Io/ParamFile.h"
#include "../../Gui/Settings/SettingsKeys.h"
#include "../../Gui/Settings/SettingsValues.h"
#include <algorithm>

#include "../Synth/SynthVoice.h"

#include "../../Processor/Opna/ProcessorOpna.h"
#include "../../Processor/Ssg/ProcessorSsg.h"
#include "../../Processor/Rhythm/ProcessorRhythm.h"
#include "../../Processor/Adpcm/ProcessorAdpcm.h"
#include "../../Processor/Fx/ProcessorFx.h"

#include "../Const/ConstGlobal.h"
#include "../Processor/ProcessorKeys.h"
#include "../Processor/ProcessorValues.h"
#include "../Const/ConstFileValues.h"
#include "../../Gui/Preset/PresetKeys.h"
#include "../../Gui/Preset/PresetValues.h"

#include "../Editor/PluginEditor.h"

#include "../../Processor/Rhythm/ProcessorRhythmValues.h"

#include "./PluginProcessorStateKey.h"

class RetroSynthesiser : public juce::Synthesiser
{
private:
    // モノフォニック用の「押されているキーの履歴（スタック）」
    juce::Array<int> heldNotes;
public:
    RetroSynthesiser() : juce::Synthesiser() {
    }

    bool isMonoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;
    bool isMidiProcessing = false;

    SynthParams* currentParams = nullptr;

    void voiceUnison(const UnisonParams& unison, int midiChannel, int midiNoteNumber, float velocity, bool isLegato)
    {
        const int voices = unison.voices;
        const int detune = unison.detuneCents;
        const float spread = unison.spread;

        // ボイス0はメイン(素の音程・定位)なので Para は適用しない。
        // ボイス1以降が paraXxx[0..] に対応する。
        auto paraDetuneOf = [&unison](int i) -> float {
            if (i < 1 || i > Global::unisonParaVoices) return 0.0f;
            return (float)unison.paraDetune[i - 1];
            };
        auto paraDistanceOf = [&unison](int i) -> float {
            if (i < 1 || i > Global::unisonParaVoices) return 0.0f;
            return unison.paraDistance[i - 1];
            };

        int uVoices = voices; // (※モードに応じて切り替えるように後で調整)

        if (!isMonoMode && uVoices <= 1) {
            if (auto* voice = dynamic_cast<SynthVoice*>(findFreeVoice(getSound(0).get(), midiChannel, midiNoteNumber, true))) {
                voice->setUnisonParams(0, 1, 0.0f, 0.0f);
                voice->setArpParams(false, unison.arpFreq, unison.arpSmooth);
                startVoice(voice, getSound(0).get(), midiChannel, midiNoteNumber, velocity);
            }
            return;
        }

        for (int i = 0; i < uVoices; ++i)
        {
            if (isMonoMode) {
                // モノフォニック時は、ユニゾン数ぶんの専用ボイス(0番目から順)を使用する
                if (auto* voice = dynamic_cast<SynthVoice*>(getVoice(i))) {
                    voice->setUnisonParams(i, uVoices, detune, spread, paraDetuneOf(i), paraDistanceOf(i));
                    voice->setArpParams(unison.arpEnable, unison.arpFreq, unison.arpSmooth);

                    // 真のレガート処理: JUCEの startVoice は呼ばず、直接コアを叩く！
                    // これにより、波形が強制キルされず、位相や音量が完全に引き継がれます。
                    if (voice->isVoiceActive()) {
                        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
                        voice->coreMap[currentParams->mode]->noteOn(cyclesPerSecond, velocity, midiNoteNumber, isLegato);
                    }
                    else {
                        // 完全に音が消えている時だけ、通常の startVoice でボイスを起こす
                        startVoice(voice, getSound(0).get(), midiChannel, midiNoteNumber, velocity);
                    }
                }
            }
            else {
                // ポリフォニック時 (既存のまま)
                juce::SynthesiserVoice* rawVoice = findFreeVoice(getSound(0).get(), midiChannel, midiNoteNumber, true);
                if (auto* voice = dynamic_cast<SynthVoice*>(rawVoice)) {
                    voice->setUnisonParams(i, uVoices, detune, spread, paraDetuneOf(i), paraDistanceOf(i));
                    voice->setArpParams(unison.arpEnable, unison.arpFreq, unison.arpSmooth);
                    startVoice(voice, getSound(0).get(), midiChannel, midiNoteNumber, velocity);
                }
            }
        }
    }

    // ユニゾン・ハーモニー向けにオーバーライド
    // 鍵盤を押した時の挙動をハックする
    void noteOn(int midiChannel, int midiNoteNumber, float velocity) override
    {
        isMidiProcessing = true;

        if (currentParams == nullptr) {
            juce::Synthesiser::noteOn(midiChannel, midiNoteNumber, velocity);
            return;
        }

        float targetVelocity = useVelocity ? velocity : fixedVelocity;
        bool isLegato = false;

        if (isMonoMode) {
            // 前のキーが押されたままならレガート（シングル・トリガー）と判定！
            if (heldNotes.size() > 0) {
                isLegato = true;
            }

            // 履歴から一旦削除して末尾に追加 (最新のキーを一番後ろにする)
            heldNotes.removeAllInstancesOf(midiNoteNumber);
            heldNotes.add(midiNoteNumber);
        }

        switch (currentParams->mode) {
        case OscMode::OPNA:
            voiceUnison(
                currentParams->opna.unison,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
        case OscMode::SSG:
            voiceUnison(
                currentParams->ssg.unison,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
        case OscMode::RHYTHM:
            voiceUnison(
                currentParams->rhythm.unison,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
        case OscMode::ADPCM:
            voiceUnison(
                currentParams->adpcm.unison,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
        }
    }

    // ユニゾン・ハーモニー向けにオーバーライド
    // 鍵盤を離した時の挙動をハックする
    void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) override
    {
        isMidiProcessing = false;

        float targetVelocity = useVelocity ? velocity : fixedVelocity;

        if (isMonoMode)
        {
            // 離されたキーを履歴から削除
            heldNotes.removeAllInstancesOf(midiNoteNumber);

            // まだ押されているキーが残っているか？
            if (heldNotes.isEmpty()) {
                // もう何も押されていないので、全ボイス(ユニゾン含む)を停止して音を消す
                for (int i = 0; i < getNumVoices(); ++i) {
                    if (auto* voice = getVoice(i)) {
                        if (voice->isVoiceActive()) {
                            voice->stopNote(targetVelocity, allowTailOff);
                        }
                    }
                }
            }
            else {
                // まだ別のキーが押されている！
                // 最新のキー(スタックの末尾)の音程に、レガートで戻して鳴らし続ける
                int previousNote = heldNotes.getLast();
                // ※ベロシティは再トリガー時のもの（ここでは便宜上 velocity を渡しますが、
                // 実機感を出したい場合は記録しておいた当時のベロシティを使うこともあります）
                switch (currentParams->mode) {
                case OscMode::OPNA:
                    voiceUnison(
                        currentParams->opna.unison,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::SSG:
                    voiceUnison(
                        currentParams->ssg.unison,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::RHYTHM:
                    voiceUnison(
                        currentParams->rhythm.unison,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::ADPCM:
                    voiceUnison(
                        currentParams->adpcm.unison,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                };
            }
        }
        else
        {
            juce::Synthesiser::noteOff(midiChannel, midiNoteNumber, targetVelocity, allowTailOff);
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
private:
    OpnaProcessor prOpna;
    SsgProcessor prSsg;
    RhythmProcessor prRhythm;
    AdpcmProcessor prAdpcm;
    FxProcessor prFx;

    SynthParams m_currentParams;
    SynthParams m_previewParams;

    std::atomic<float>* pMode = nullptr;
    std::atomic<float>* pMonoMode = nullptr;
    std::atomic<float>* pUseVelocity = nullptr;
    std::atomic<float>* pPitchResetOnLegato = nullptr;
    std::atomic<float>* pFixedVelocity = nullptr;

    std::map<OscMode, PrBase*> prMap;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    RetroSynthesiser m_synth;

    // 波形プレビュー用
    juce::Synthesiser previewSynth;
    std::unique_ptr<SynthSound> previewSound;
    FxProcessor previewFx;

    void loadStartupSettings(); // 設定の自動読み込み用関数
    void setPresetToXml(std::unique_ptr<juce::XmlElement>& xml);
    void getPresetFromXml(std::unique_ptr<juce::XmlElement>& xmlState);
public:
    AudioPlugin2686V();
    ~AudioPlugin2686V() override;

    static inline constexpr int previewBufferSize = 200;

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
    // チャンネルごとの MODULATION 変調波形ファイルのパス。
    // キーは APVTS のプレフィックス (OPNA / SSG / OPZX7 など)。
    // 波形そのものは 32 個のパラメータ側に入っているので、ここは表示用。
    // 1 チャンネルにつきスロットの数だけ持つ。
    using WtModWavePaths = std::array<juce::String, Global::WtMod::slots>;
    std::map<juce::String, WtModWavePaths> modWavePaths;

    // WT PITCH MOD の変調波形。チャンネルごとに複数スロット持つ。
    // 32 サンプル × 枚数をパラメータで持つと数が膨大になるため、
    // 実データはここが所有し、state には相対パスだけを保存する。
    WtModWaveStore modWaveSlots;
    // 変調波形の読み書き。実データは modWaveSlots が持ち、
    // state へは相対パスだけを保存して読み直す。
    void loadWtModWaveFile(const juce::String& code, int slot, const juce::File& file);
    void unloadWtModWaveFile(const juce::String& code, int slot);
    juce::String adpcmFilePath;
    std::array<juce::String, RhythmPrValue::pads> rhythmFilePaths;

    // 画面へ波形を描くために持っておくサンプル。
    // 音は各ボイスが自分の持ち分で鳴らすので、こちらは表示専用。
    // 読み込んだままのデータなので、音源側の品質劣化は掛かっていない。
    std::vector<float> adpcmPreviewBuffer;
    double adpcmPreviewRate = 44100.0;

    std::array<std::vector<float>, RhythmPrValue::pads> rhythmPreviewBuffers;
    std::array<double, RhythmPrValue::pads> rhythmPreviewRates{};

    // --- Preset I/O ---
    void savePreset(const juce::File& file);
    void loadPreset(const juce::File& file);
    void initPreset();

    void initParams(const juce::String& code);

    // --- Preview(Static) ---
    void generatePreviewWaveform(std::vector<float>* destBuffer);

    // --- 仮想キーボード ---
    juce::MidiKeyboardState keyboardState;

    // --- Preview ---
    bool previewVisiblity = true; // Editorとの同期用

    // L, Mono, R の3チャンネル分のバッファを用意
    // 余裕を持たせたリングバッファ (ただのfloat配列でOK)
    static inline constexpr int ringBufferSize = 2048;
    float realTimeBufferL[ringBufferSize] = { 0.0f };
    float realTimeBufferMono[ringBufferSize] = { 0.0f };
    float realTimeBufferR[ringBufferSize] = { 0.0f };

    // 現在の書き込み位置だけをスレッドセーフに管理
    std::atomic<int> realTimeWritePos{ 0 };

    // --- Settings Data ---
    int uiScaleIndex = 7; // 高解像度対応(0ベース、初期値: 80%)

    // パラメータファイルを書き出す形。0 = JSON, 1 = YAML。
    // 設定として持ち回るので番号で持つ。
    int fileFormatIndex = 0;

    // 番号を実際の書き出し先へ映す。設定を読んだ後と、画面で
    // 変えたときに呼ぶ。
    void applyFileFormat() const
    {
        Io::setFileFormat(fileFormatIndex == 1 ? Io::FileFormat::yaml : Io::FileFormat::json);
    }
    juce::String wallpaperPath;
    int wallpaperMode = 0; // 0=Stretch, 1=Fill, 2=Fit, 3=Original
    juce::String defaultSampleDir;  // For ADPCM & Rhythm
    juce::String defaultPresetDir; // For Presets
	juce::String defaultWavetableDir; // For Wavetables
    juce::String defaultFxOrderDir; // For FX Order
    juce::String defaultFxParamDir;
    juce::String defaultChannelParamDir;
    juce::String defaultCurveParamDir;
    juce::String defaultLfoParamDir;
    juce::String defaultAmpEnvParamDir;
    juce::String defaultPitchEnvParamDir;
    juce::String defaultSsgHwEnvParamDir;
    juce::String defaultWtModParamDir;
    juce::String defaultSsgSwEnvParamDir;
    juce::String defaultDetuneParamDir;
    juce::String defaultUnisonParamDir;
    juce::String defaultQualityParamDir;
    juce::String defaultPcmPlayParamDir;
    juce::String defaultToneNoiseParamDir;
    juce::String defaultColorSettingDir;

    // ------------------------------------------------------------------
    // 環境設定の項目
    // ------------------------------------------------------------------
    // 保存と読み込みをこの 1 つの並びから作る。同じ項目を 2 か所に書くと、
    // 片方だけ書き忘れて値が失われる。実際に起きていた。
    template <typename Visitor>
    void visitEnvironment(Visitor& visit)
    {
        visit(SettingsKey::uiScaleIndex, uiScaleIndex);
        visit(SettingsKey::fileFormat, fileFormatIndex);
        visit(SettingsKey::wallpaperPath, wallpaperPath);
        visit(SettingsKey::wallpaperMode, wallpaperMode);

        visit(SettingsKey::defaultSampleDir, defaultSampleDir);
        visit(SettingsKey::defaultPresetDir, defaultPresetDir);
        visit(SettingsKey::defaultWavetableDir, defaultWavetableDir);
        visit(SettingsKey::defaultFxOrderDir, defaultFxOrderDir);
        visit(SettingsKey::defaultFxParamDir, defaultFxParamDir);
        visit(SettingsKey::defaultChannelParamDir, defaultChannelParamDir);
        visit(SettingsKey::defaultCurveParamDir, defaultCurveParamDir);
        visit(SettingsKey::defaultLfoParamDir, defaultLfoParamDir);
        visit(SettingsKey::defaultAmpEnvParamDir, defaultAmpEnvParamDir);
        visit(SettingsKey::defaultPitchEnvParamDir, defaultPitchEnvParamDir);
        visit(SettingsKey::defaultSsgSwEnvParamDir, defaultSsgSwEnvParamDir);
        visit(SettingsKey::defaultSsgHwEnvParamDir, defaultSsgHwEnvParamDir);
        visit(SettingsKey::defaultDetuneParamDir, defaultDetuneParamDir);
        visit(SettingsKey::defaultUnisonParamDir, defaultUnisonParamDir);
        visit(SettingsKey::defaultQualityParamDir, defaultQualityParamDir);
        visit(SettingsKey::defaultPcmPlayParamDir, defaultPcmPlayParamDir);
        visit(SettingsKey::defaultToneNoiseParamDir, defaultToneNoiseParamDir);
        visit(SettingsKey::defaultColorSettingDir, defaultColorSettingDir);

        visit(SettingsKey::showTooltips, showTooltips);
        visit(SettingsKey::useHeadroom, useHeadroom);
        visit(SettingsKey::headroomGain, headroomGain);
        visit(SettingsKey::showVirtualKeyboard, showVirtualKeyboard);
    }
    bool showTooltips = true; // For show Parameter Range Tooltop
    bool useHeadroom = true; // ヘッドルーム適応
    float headroomGain = 0.25; // ヘッドルーム圧縮値
    bool showVirtualKeyboard = true; // 仮想キーボードの表示フラグ（デフォルトON）

    bool saveEnvironment(const juce::File& file);
    // プラグインが使うフォルダ。ドキュメントの下に 1 つ作り、
    // 既定の保存先はすべてこの中にする。
    juce::File getPluginDirectory() const
    {
        auto dir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
            .getChildFile(Io::Folder::asset);

        if (!dir.exists()) dir.createDirectory();

        return dir;
    }

    // 起動時に読む設定ファイル。JSON と YAML のどちらで保存されていても
    // 拾えるよう、あるほうを返す。両方あれば新しいほう。どちらも無ければ
    // 今の形で作る名前を返す。
    juce::File getStartupSettingsFile() const
    {
        return Io::resolveFile(getPluginDirectory(), SettingsValue::File::Name::initial);
    }

    // 標準設定を書き出す先。今の形の名前になる。
    juce::File getStartupSettingsFileToWrite() const
    {
        return Io::fileToWrite(getPluginDirectory(), SettingsValue::File::Name::initial);
    }

    bool loadEnvironment(const juce::File& file, bool tellIfLegacy = true); 

    void panic();

    juce::String makePathRelative(const juce::File& targetFile); // 相対ディレクトリへ変換
    juce::File resolvePath(const juce::String& pathStr); // 相対ディレクトリからの展開
    juce::String makeWtPathRelative(const juce::File& targetFile); // 相対ディレクトリへ変換
    juce::File resolveWtPath(const juce::String& pathStr); // 相対ディレクトリからの展開
    juce::String makeFxOrderPathRelative(const juce::File& targetFile); // 相対ディレクトリへ変換
    juce::File resolveFxOrderPath(const juce::String& pathStr); // 相対ディレクトリからの展開

    juce::String getDefaultPresetDir();
    static juce::String sanitizeString(const juce::String& input, int length);

    void resetMidiSettings();
    std::vector<int> getFxOrder();
    void updateFxOrder(std::vector<int> newOrder);
    bool isPlaying();
    bool isMidiProcessing();
    OscMode getCurrentMode();
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686V)
};
