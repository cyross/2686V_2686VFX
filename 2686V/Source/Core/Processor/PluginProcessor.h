#pragma once
#include <JuceHeader.h>
#include <algorithm>

#include "../Synth/SynthVoice.h"

#include "../Fm/FmClipboard.h"

#include "../../Processor/Opna/ProcessorOpna.h"
#include "../../Processor/Opn/ProcessorOpn.h"
#include "../../Processor/Opl/ProcessorOpl.h"
#include "../../Processor/Opl3/ProcessorOpl3.h"
#include "../../Processor/Opm/ProcessorOpm.h"
#include "../../Processor/Opzx7/ProcessorOpzx7.h"
#include "../../Processor/Ssg/ProcessorSsg.h"
#include "../../Processor/Wavetable/ProcessorWt.h"
#include "../../Processor/Rhythm/ProcessorRhythm.h"
#include "../../Processor/Adpcm/ProcessorAdpcm.h"
#include "../../Processor/Beep/ProcessorBeep.h"
#include "../../Processor/Fx/ProcessorFx.h"
#include "../../Processor/Curve/ProcessorCurve.h"

#include "../Const/ConstGlobal.h"
#include "../Processor/ProcessorKeys.h"
#include "../Processor/ProcessorValues.h"
#include "../Const/ConstFileValues.h"
#include "../../Gui/Preset/PresetKeys.h"
#include "../../Gui/Preset/PresetValues.h"

#include "../Editor/PluginEditor.h"

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

    SynthParams* currentParams = nullptr;

    void voiceUnison(int voices, int detune, float spread, int midiChannel, int midiNoteNumber, float velocity, bool isLegato)
    {
        int uVoices = voices; // (※モードに応じて切り替えるように後で調整)

        if (!isMonoMode && uVoices <= 1) {
            if (auto* voice = dynamic_cast<SynthVoice*>(findFreeVoice(getSound(0).get(), midiChannel, midiNoteNumber, true))) {
                voice->setUnisonParams(0, 1, 0.0f, 0.0f);
                startVoice(voice, getSound(0).get(), midiChannel, midiNoteNumber, velocity);
            }
            return;
        }

        for (int i = 0; i < uVoices; ++i)
        {
            if (isMonoMode) {
                // モノフォニック時は、ユニゾン数ぶんの専用ボイス(0番目から順)を使用する
                if (auto* voice = dynamic_cast<SynthVoice*>(getVoice(i))) {
                    voice->setUnisonParams(i, uVoices, detune, spread);

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
                    voice->setUnisonParams(i, uVoices, detune, spread);
                    startVoice(voice, getSound(0).get(), midiChannel, midiNoteNumber, velocity);
                }
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
                currentParams->opna.unisonVoices,
                currentParams->opna.unisonDetuneCents,
                currentParams->opna.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::OPN:
            voiceUnison(
                currentParams->opn.unisonVoices,
                currentParams->opn.unisonDetuneCents,
                currentParams->opn.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::OPL:
            voiceUnison(
                currentParams->opl.unisonVoices,
                currentParams->opl.unisonDetuneCents,
                currentParams->opl.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::OPL3:
            voiceUnison(
                currentParams->opl3.unisonVoices,
                currentParams->opl3.unisonDetuneCents,
                currentParams->opl3.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::OPM:
            voiceUnison(
                currentParams->opm.unisonVoices,
                currentParams->opm.unisonDetuneCents,
                currentParams->opm.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::OPZX7:
            voiceUnison(
                currentParams->opzx7.unisonVoices,
                currentParams->opzx7.unisonDetuneCents,
                currentParams->opzx7.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::SSG:
            voiceUnison(
                currentParams->ssg.unisonVoices,
                currentParams->ssg.unisonDetuneCents,
                currentParams->ssg.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::WAVETABLE:
            voiceUnison(
                currentParams->wt.unisonVoices,
                currentParams->wt.unisonDetuneCents,
                currentParams->wt.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::RHYTHM:
            voiceUnison(
                currentParams->rhythm.unisonVoices,
                currentParams->rhythm.unisonDetuneCents,
                currentParams->rhythm.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::ADPCM:
            voiceUnison(
                currentParams->adpcm.unisonVoices,
                currentParams->adpcm.unisonDetuneCents,
                currentParams->adpcm.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::BEEP:
            voiceUnison(
                currentParams->beep.unisonVoices,
                currentParams->beep.unisonDetuneCents,
                currentParams->beep.unisonSpread,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
        };
    }

    // ユニゾン・ハーモニー向けにオーバーライド
    // 鍵盤を離した時の挙動をハックする
    void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) override
    {
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
                        currentParams->opna.unisonVoices,
                        currentParams->opna.unisonDetuneCents,
                        currentParams->opna.unisonSpread,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::OPN:
                    voiceUnison(
                        currentParams->opn.unisonVoices,
                        currentParams->opn.unisonDetuneCents,
                        currentParams->opn.unisonSpread,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::OPL:
                    voiceUnison(
                        currentParams->opl.unisonVoices,
                        currentParams->opl.unisonDetuneCents,
                        currentParams->opl.unisonSpread,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::OPL3:
                    voiceUnison(
                        currentParams->opl3.unisonVoices,
                        currentParams->opl3.unisonDetuneCents,
                        currentParams->opl3.unisonSpread,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::OPM:
                    voiceUnison(
                        currentParams->opm.unisonVoices,
                        currentParams->opm.unisonDetuneCents,
                        currentParams->opm.unisonSpread,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::OPZX7:
                    voiceUnison(
                        currentParams->opzx7.unisonVoices,
                        currentParams->opzx7.unisonDetuneCents,
                        currentParams->opzx7.unisonSpread,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::SSG:
                    voiceUnison(
                        currentParams->ssg.unisonVoices,
                        currentParams->ssg.unisonDetuneCents,
                        currentParams->ssg.unisonSpread,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::WAVETABLE:
                    voiceUnison(
                        currentParams->wt.unisonVoices,
                        currentParams->wt.unisonDetuneCents,
                        currentParams->wt.unisonSpread,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::RHYTHM:
                    voiceUnison(
                        currentParams->rhythm.unisonVoices,
                        currentParams->rhythm.unisonDetuneCents,
                        currentParams->rhythm.unisonSpread,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::ADPCM:
                    voiceUnison(
                        currentParams->adpcm.unisonVoices,
                        currentParams->adpcm.unisonDetuneCents,
                        currentParams->adpcm.unisonSpread,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::BEEP:
                    voiceUnison(
                        currentParams->beep.unisonVoices,
                        currentParams->beep.unisonDetuneCents,
                        currentParams->beep.unisonSpread,
                        midiChannel,
                        midiNoteNumber,
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

    SynthParams m_currentParams;
    SynthParams m_previewParams;

    std::atomic<float>* pMode = nullptr;
    std::atomic<float>* pMonoMode = nullptr;
    std::atomic<float>* pUseVelocity = nullptr;
    std::atomic<float>* pPitchResetOnLegato = nullptr;
    std::atomic<float>* pFixedVelocity = nullptr;

    std::map<OscMode, PrBase*> prMap;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix);

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
    void generatePreviewWaveform(std::vector<float>* destBuffer);

    // --- 仮想キーボード ---
    juce::MidiKeyboardState keyboardState;

    // --- Preview ---
    bool previewVisiblity = true; // Editorとの同期用
    std::atomic<float> realTimeBuffer[512];

    // --- Settings Data ---
    int uiScaleIndex = 9; // 高解像度対応(0ベース、初期値: 100%)
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

    void bakeCurves();
    void bakeCurvesPrim(int positionIndex, int targetIndex, int paramIndex);
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686V)
};
