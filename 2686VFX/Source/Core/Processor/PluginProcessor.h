#pragma once
#include <map>
#include "../Const/ConstPlugin.h"
#include <JuceHeader.h>

#include "Shared/Core/Io/ParamFile.h"
#include "Shared/Core/Gui/GuiI18n.h"
#include "../../Gui/Settings/SettingsKeys.h"
#include "../../Gui/Settings/SettingsValues.h"
#include "Shared/Core/Gui/GuiSimpleView.h"
#include "Shared/Core/Gui/GuiToggleAlign.h"
#include <algorithm>


#include "../../Processor/Fx/ProcessorFx.h"
#include "../../Processor/Mod/ProcessorMod.h"

#include "Shared/Core/Const/ConstGlobal.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Shared/Core/Processor/ProcessorValues.h"
#include "Shared/Core/Const/ConstFileValues.h"
#include "../../Gui/Preset/PresetKeys.h"
#include "../../Gui/Preset/PresetValues.h"

#include "../Editor/PluginEditor.h"


#include "./PluginProcessorStateKey.h"
#include "Shared/Core/Synth/WtModWave.h"
#include "Shared/Core/Gui/GuiHost.h"


class AudioPlugin2686V : public juce::AudioProcessor,
    public GuiProcessorHost
{
private:
    FxProcessor prFx;
    ModProcessor prMod;

    // 出力の音量 (LEVEL)。つまみを動かしたときにぷつっと鳴らないよう、
    // 1 ブロックごとに飛ばさず滑らかにつなぐ。
    juce::LinearSmoothedValue<float> m_outputLevel{ 1.0f };
    static constexpr double levelRampSeconds = 0.02;

    void applyOutputLevel(juce::AudioBuffer<float>& buffer);

    SynthParams m_currentParams;

    std::atomic<float>* pMode = nullptr;
    std::atomic<float>* pMonoMode = nullptr;
    std::atomic<float>* pUseVelocity = nullptr;
    std::atomic<float>* pPitchResetOnLegato = nullptr;
    std::atomic<float>* pFixedVelocity = nullptr;


    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // 今のパラメータに無い項目を状態から落とす。
    //
    // かつて登録していたパラメータは、古いファイルを読み込むと状態へ入り、
    // そのまま保存で書き戻され続ける。Curve を APVTS から外したときの名残が
    // 実際に 2,000 件近く残っていた。動きはしないが、ファイルを太らせる。
    void removeUnknownParams(juce::XmlElement& xml) const;


    void loadStartupSettings(); // 設定の自動読み込み用関数
    void setPresetToXml(std::unique_ptr<juce::XmlElement>& xml);
    void getPresetFromXml(std::unique_ptr<juce::XmlElement>& xmlState);
public:
    AudioPlugin2686V();
    ~AudioPlugin2686V() override;

    static inline constexpr int previewBufferSize = 200;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
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

    OscMode lastActiveSynthMode = OscMode::OPNA;


    // 変調波形の読み書き。実データは modWaveSlots が持ち、
    // state へは相対パスだけを保存して読み直す。
    void loadWtModWaveFile(const juce::String& code, int slot, const juce::File& file) override;
    void unloadWtModWaveFile(const juce::String& code, int slot) override;

    // 生成波形の計算に使う音源一式 (窓口)。2686VFX は音源を持たないので作らない。
    std::unique_ptr<GuiRenderRig> createRenderRig(double) override { return nullptr; }

    // 画面へ波形を描くために持っておくサンプル。
    // 音は各ボイスが自分の持ち分で鳴らすので、こちらは表示専用。
    // 読み込んだままのデータなので、音源側の品質劣化は掛かっていない。


    // --- Preset I/O ---
    void savePreset(const juce::File& file);
    void loadPreset(const juce::File& file);

    // 別のプラグインで書かれたプリセットなら、断って false を返す。
    bool isPresetForThisPlugin(const juce::XmlElement* xmlState, const juce::File& file);
    void initPreset();

    void initParams(const juce::String& code);

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

    // 加工前の音。エフェクトなので、入ってきたものと出ていくものを
    // 並べて見せる。書き込む数は加工後と同じなので、位置は共用する。
    float dryBufferL[ringBufferSize] = { 0.0f };
    float dryBufferMono[ringBufferSize] = { 0.0f };
    float dryBufferR[ringBufferSize] = { 0.0f };

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

    void resetMidiSettings();
    std::vector<int> getFxOrder();
    void updateFxOrder(std::vector<int> newOrder);
    bool isPlaying();
    bool isMidiProcessing();

    // 押さえている鍵盤を割り当てた変調の対象 (キーアサインの Target のビット)
    uint32_t getModHeldTargets() const { return prMod.getHeldTargets(); }
    OscMode getCurrentMode();
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlugin2686V)
};
