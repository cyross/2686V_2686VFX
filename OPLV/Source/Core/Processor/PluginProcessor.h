#pragma once
#include "Shared/Generator/Pcm/Helper/GenPcmShared.h"
#include "../Const/ConstPlugin.h"
#include <array>
#include <atomic>
#include <bitset>
#include <map>
#include <JuceHeader.h>

#include "Shared/Core/Io/ParamFile.h"
#include "../../Gui/Settings/SettingsKeys.h"
#include "../../Gui/Settings/SettingsValues.h"
#include "Shared/Core/Gui/GuiI18n.h"
#include "Shared/Core/Gui/GuiSimpleView.h"
#include "Shared/Core/Gui/GuiToggleAlign.h"
#include <algorithm>

#include "../Synth/SynthVoice.h"

#include "Shared/Processor/Opl/ProcessorOpl.h"
#include "Shared/Processor/Opl3/ProcessorOpl3.h"
#include "../../Processor/Fx/ProcessorFx.h"
#include "Shared/Processor/Curve/ProcessorCurve.h"

#include "Shared/Core/Const/ConstGlobal.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Shared/Core/Processor/ProcessorValues.h"
#include "Shared/Core/Const/ConstFileValues.h"
#include "../../Gui/Preset/PresetKeys.h"
#include "../../Gui/Preset/PresetValues.h"

#include "../Editor/PluginEditor.h"


#include "./PluginProcessorStateKey.h"

#include "Shared/Gui/Components/AlgMatrix/FmAlgState.h"
#include "Shared/Core/Gui/GuiHost.h"

class RetroSynthesiser : public juce::Synthesiser
{
private:
    // モノフォニック用の「押されているキーの履歴（スタック）」
    juce::Array<int> heldNotes;

    // 押している鍵盤。再生ランプを出すために、どのモードでも数える。
    std::bitset<128> m_midiHeldKeys;
public:
    RetroSynthesiser() : juce::Synthesiser() {
    }

    bool isMonoMode = false;
    bool useVelocity = false;
    bool pitchResetOnLegato = false;
    float fixedVelocity = 1.0f;

    // 鍵盤が押されているか。再生ランプが読む。
    //
    // 以前は「最後に来たのが押しか離しか」を覚える札だった。和音の 1 つを
    // 離しただけで倒れてしまい、逆にオールノートオフ (CC123 / CC120) は
    // noteOff を通らないので立ったまま残り、ランプが消えなかった。
    // 押している鍵盤を数えて持てば、どちらも正しく出る。
    //
    // オーディオスレッドが書き、画面のスレッドが読む。
    std::atomic<bool> isMidiProcessing{ false };

    void midiKeyDown(int note)
    {
        if (note >= 0 && note < (int)m_midiHeldKeys.size()) m_midiHeldKeys.set((size_t)note);

        isMidiProcessing = m_midiHeldKeys.any();
    }

    void midiKeyUp(int note)
    {
        if (note >= 0 && note < (int)m_midiHeldKeys.size()) m_midiHeldKeys.reset((size_t)note);

        isMidiProcessing = m_midiHeldKeys.any();
    }

    // 全部離した扱いにする。オールノートオフ・パニック・チャンネルの
    // 切り替えで使う。
    void midiKeysClear()
    {
        m_midiHeldKeys.reset();
        heldNotes.clear();

        isMidiProcessing = false;
    }

    // オールノートオフ (CC123) とオールサウンドオフ (CC120) は JUCE が直に
    // 受け取るので、noteOff を通らない。押している鍵盤もここで落とす。
    void allNotesOff(int midiChannel, bool allowTailOff) override
    {
        midiKeysClear();

        juce::Synthesiser::allNotesOff(midiChannel, allowTailOff);
    }

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
                        voice->coreMap[(size_t)currentParams->mode]->noteOn(cyclesPerSecond, velocity, midiNoteNumber, isLegato);
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
        midiKeyDown(midiNoteNumber);

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
		case OscMode::OPL:
            voiceUnison(
                currentParams->opl.unison,
                midiChannel,
                midiNoteNumber,
                targetVelocity,
                isLegato
            );
            break;
		case OscMode::OPL3:
            voiceUnison(
                currentParams->opl3.unison,
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
        midiKeyUp(midiNoteNumber);

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
                case OscMode::OPL:
                    voiceUnison(
                        currentParams->opl.unison,
                        midiChannel,
                        previousNote,
                        targetVelocity,
                        true
                    );
                    break;
                case OscMode::OPL3:
                    voiceUnison(
                        currentParams->opl3.unison,
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

class AudioPlugin2686V : public juce::AudioProcessor,
    public GuiProcessorHost,
    public juce::AsyncUpdater
{
private:
    OplProcessor prOpl;
    Opl3Processor prOpl3;
    FxProcessor prFx;

    CurveCore m_curveCore;

    // ADPCM の素材と符号化したもの。ボイスごとに複製せず、ここで 1 つだけ持つ。
    // 符号化はメッセージスレッドで行い、オーディオスレッドは出来たものを指すだけ。
    PcmSharedStore m_adpcmPcm;

    // オーディオスレッドが「この指定で作り直してほしい」と置いていく場所。
    std::atomic<int> m_adpcmWantQuality{ -1 };
    std::atomic<int> m_adpcmWantRate{ -1 };

    // QUALITY のノイズリダクション (きれいな間引き) も、作り直しの条件に入る。
    std::atomic<bool> m_adpcmWantClean{ false };

    // ADPCM+ も同じ。PCM のスロットごとに 1 つずつ持つ。
    // 鳴らすのは TARGET で選んだ 1 本だけだが、差し替えずに済むよう
    // 読み込んだものはすべて持っておく。
    std::array<PcmSharedStore, Global::AdpcmPlus::slots> m_adpcmPlusPcm;
    std::array<std::atomic<int>, Global::AdpcmPlus::slots> m_adpcmPlusWantQuality{};
    std::array<std::atomic<int>, Global::AdpcmPlus::slots> m_adpcmPlusWantRate{};
    std::array<std::atomic<bool>, Global::AdpcmPlus::slots> m_adpcmPlusWantClean{};


    SynthParams m_currentParams;

    // 直近のブロックで鳴らしたチャンネル。切り替わりを見つけるために持つ。
    // Count は「まだ鳴らしていない」印で、どのチャンネルとも一致しない。
    OscMode m_lastRenderedMode = OscMode::Count;

    // 音が出ているか (再生ランプ用)。オーディオスレッドが書き、画面が読む。
    //
    // ボイスが生きているかでは足りない。RR を遅くした音色では、耳に届かなく
    // なったあとも包絡だけが数秒走り続ける。音が消えているのにランプだけが
    // 点いたままになるので、出てきた音そのものを見る。
    std::atomic<bool> m_audible{ false };

    // 消えた瞬間に落とすと、音のうねりや音と音の間でちらつく。
    // 最後に音が出てからしばらくは点けたままにする。
    int m_audibleHoldLeft = 0;

    // これを下回ったら「音は出ていない」とみなす (-100dB ほど)
    static inline constexpr float audibleLevel = 1.0e-5f;

    // 音が絶えてからランプを落とすまで
    static inline constexpr double audibleHoldSeconds = 0.15;

    // 上の秒数を、いまのレートでのサンプル数に直したもの (prepareToPlay で決める)
    int m_audibleHoldSamples = 0;

    std::atomic<float>* pMode = nullptr;
    std::atomic<float>* pMonoMode = nullptr;
    std::atomic<float>* pUseVelocity = nullptr;
    std::atomic<float>* pPitchResetOnLegato = nullptr;
    std::atomic<float>* pFixedVelocity = nullptr;

    std::map<OscMode, PrBase*> prMap;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // 今のパラメータに無い項目を状態から落とす。
    //
    // かつて登録していたパラメータは、古いファイルを読み込むと状態へ入り、
    // そのまま保存で書き戻され続ける。Curve を APVTS から外したときの名残が
    // 実際に 2,000 件近く残っていた。動きはしないが、ファイルを太らせる。
    void removeUnknownParams(juce::XmlElement& xml) const;

    RetroSynthesiser m_synth;

    void loadStartupSettings(); // 設定の自動読み込み用関数

    // オーディオスレッドから頼まれた符号化をここで行う (メッセージスレッド)
    void handleAsyncUpdate() override;
    void setPresetToXml(std::unique_ptr<juce::XmlElement>& xml);
    void getPresetFromXml(std::unique_ptr<juce::XmlElement>& xmlState);
public:
    AudioPlugin2686V();
    ~AudioPlugin2686V() override;

    CurveProcessor prCurve;

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

    juce::File lastSampleDirectory{ juce::File::getSpecialLocation(juce::File::userHomeDirectory) };

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState apvts;

    // --- Metadata ---
    juce::String presetName = PresetValue::MetaData::Initial::name;
    juce::String presetAuthor = PresetValue::MetaData::Initial::author;
    juce::String presetVersion = PresetValue::MetaData::Initial::version;
    juce::String presetComment = PresetValue::MetaData::Initial::comment;
    juce::String presetGenre = PresetValue::MetaData::Initial::genre;
    juce::String presetFilePath = "";
    juce::String presetPluginVersion = Global::Plugin::version;

    OscMode lastActiveSynthMode = OscMode::OPL;




    // 変調波形の読み書き。実データは modWaveSlots が持ち、
    // state へは相対パスだけを保存して読み直す。
    void loadWtModWaveFile(const juce::String& code, int slot, const juce::File& file) override;
    void unloadWtModWaveFile(const juce::String& code, int slot) override;




    // ------------------------------------------------------------------
    // 波形プレビューの計算用
    // ------------------------------------------------------------------
    // プレビューは、鳴っている音とは別に、出荷される音源のクラスを
    // そのまま回して計算する。プラグインをもう一つ立てて鳴らす造りには
    // しない。状態の複製もファイルの読み直しも要らないうえ、一覧の
    // 一括生成のように何百件も回す使い方に耐えない。
    //
    // どちらもメッセージスレッドから呼ぶこと。仕立てたボイスを回すのは
    // 別のスレッドでよい。
    SynthParams buildRenderParams();
    void prepareRenderVoice(SynthVoice& voice, double sampleRate);

    // 上の 2 つで、生成波形の計算に使う音源一式を組み立てる (窓口の実装)
    std::unique_ptr<GuiRenderRig> createRenderRig(double sampleRate) override;

    // --- Preset I/O ---
    void savePreset(const juce::File& file);
    void loadPreset(const juce::File& file);

    // 別のプラグインで書かれたプリセットなら、断って false を返す。
    bool isPresetForThisPlugin(const juce::XmlElement* xmlState, const juce::File& file);
    void initPreset();

    void initParams(const juce::String& code);








    void publishWtPlusWaveSlots();

    // --- Preview(Static) ---

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
    // 画面に出す文字列の言語。"ja" か "en" で持つ。
    //
    // 空は「まだ決めていない」を表す。初めて立ち上げたときは設定
    // ファイルが無いのでここが空のままになり、applyLanguage が OS の
    // 言語から見立てる。
    juce::String languageCode;

    // 言語を設定から画面へ映す。設定を読んだ後に呼ぶ。
    void applyLanguage()
    {
        if (languageCode.isEmpty()) languageCode = I18n::toCode(I18n::detect());

        I18n::setCurrent(I18n::fromCode(languageCode));
    }

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
    juce::String defaultFxOrderDir; // For FX Order
    juce::String defaultFxParamDir;
    juce::String defaultCurveParamDir;
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
        visit(SettingsKey::language, languageCode);
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
        visit(SettingsKey::defaultWtModParamDir, defaultWtModParamDir);
        visit(SettingsKey::defaultColorSettingDir, defaultColorSettingDir);

        visit(SettingsKey::showTooltips, showTooltips);
        visit(SettingsKey::simpleView, simpleView);

        // 隠さない区分。項目の並びを変えても迷子にならないよう名前で持つ。
        for (int i = 0; i < SimpleView::Size; ++i) {
            visit(juce::String(SimpleView::items()[(size_t)i].key), simpleViewShow[(size_t)i]);
        }
        visit(SettingsKey::toggleAlign, toggleAlign);
        visit(SettingsKey::useHeadroom, useHeadroom);
        visit(SettingsKey::headroomGain, headroomGain);
        visit(SettingsKey::showVirtualKeyboard, showVirtualKeyboard);
    }
    bool showTooltips = true; // For show Parameter Range Tooltop

    // 簡易表示モード。区分の一部を隠して画面を短くする。表示だけの話で、
    // 音には影響しない。
    bool simpleView = false;

    // 簡易表示モードでも隠さない区分。既定はどれも隠す。
    std::array<bool, SimpleView::Size> simpleViewShow{};

    // 隠す対象の区分を、いま出すかどうか
    bool isSimpleShown(SimpleView::Cat cat) const {
        return SimpleView::isShown(simpleView, simpleViewShow, cat);
    }

    bool useHeadroom = true; // ヘッドルーム適応
    float headroomGain = 0.25; // ヘッドルーム圧縮値
    bool showVirtualKeyboard = true; // 仮想キーボードの表示フラグ（デフォルトON）

    bool saveEnvironment(const juce::File& file);
    // プラグインが使うフォルダ。ドキュメントの下に 1 つ作り、
    // 既定の保存先はすべてこの中にする。
    juce::File getPluginDirectory() const override
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
    CurveCore* getCurveCore();

    void bakeCurves();
    void bakeCurvesPrim(int positionIndex, int targetIndex, int paramIndex);
    void resetMidiSettings();
    std::vector<int> getFxOrder();
    void updateFxOrder(std::vector<int> newOrder);
    bool isPlaying();
    bool isMidiProcessing();
    OscMode getCurrentMode();
public:
    int getOpzx7AlgMode() const;


    // 画面から読むほう。メッセージスレッド専用。
    FmAlgState getOpzx7AlgMatrix() const;

    // processBlock から読むほう。錠を取らないので待たされない。
    const FmAlgState& getOpzx7AlgMatrixForAudio();

    // プリセットロード時などにAPVTSからキャッシュを復元するための関数
    void updateAlgMatrixCacheFromState();
private:
    // 出来上がった中身をオーディオスレッドへ渡す。
    void publishAlgMatrix();

    // オーディオスレッドから安全に読み取るためのキャッシュ
    std::atomic<int> m_opzx7AlgMode{ 0 };

    // 正本。触るのはメッセージスレッドだけ (画面の操作と、状態の読み込み)。
    FmAlgState m_opzx7AlgMatrixState;

    // オーディオスレッドへの受け渡し。枠を 3 枚回す。
    //
    // 以前は juce::CriticalSection を processBlock の中で取っていた。
    // 画面側が同じ錠を持っている間に OS が画面スレッドを止めると、
    // オーディオが解放待ちで止まる (優先度の逆転)。数ミリ秒でも音が途切れる。
    //
    // 書き手は書き枠へ書いてから ready と入れ替え、読み手は読み枠を
    // ready と入れ替える。枠は 3 枚あるので、書き手と読み手が同じ枠に
    // 触ることがない。待ちも確保も無い。
    std::array<FmAlgState, 3> m_algMatrixSlots;
    std::atomic<int> m_algMatrixReady{ 0 };       // 出来上がっている枠
    std::atomic<bool> m_algMatrixDirty{ false };  // 新しい枠があるか
    int m_algMatrixWriteSlot = 1;                 // 書き手だけが触る
    int m_algMatrixReadSlot = 2;                  // 読み手だけが触る
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686V)
};
