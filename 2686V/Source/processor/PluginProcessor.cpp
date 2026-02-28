#include "PluginProcessor.h"
#include "../core/GuiValues.h"
#include "../core/PrNames.h"
#include "../core/SettingsKeys.h"
#include "../core/SettingsValues.h"

// ============================================================================
// Constructor
// ============================================================================
AudioPlugin2686V::AudioPlugin2686V()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if defined(BUILD_AS_FX_PLUGIN)
        // FXモード: ステレオ入力あり、ステレオ出力あり
        .withInput(Global::Audio::input, juce::AudioChannelSet::stereo(), true)
        .withOutput(Global::Audio::output, juce::AudioChannelSet::stereo(), true)
#else
        // シンセモード: 入力なし、ステレオ出力あり
        .withOutput(Global::Audio::output, juce::AudioChannelSet::stereo(), true)
#endif
    ),
    // Initialize APVTS (Parameters are created here)
    apvts(*this, nullptr, Global::Plugin::parameters, createParameterLayout()) // APVTSの初期化
#endif
{
#if !defined(BUILD_AS_FX_PLUGIN)
    m_synth.addSound(new SynthSound());
    for (int i = 0; i < Global::voices; i++) {
        m_synth.addVoice(new SynthVoice());
    }

    previewSynth.addSound(new SynthSound());
    previewSynth.addVoice(new SynthVoice());

    formatManager.registerBasicFormats();
#endif
    loadStartupSettings();
}

// ============================================================================
// Destructor
// ============================================================================
AudioPlugin2686V::~AudioPlugin2686V() {}

// ============================================================================
// Parameter Layout Definition (Visible to DAW and GUI)
// ============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout AudioPlugin2686V::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

#if defined(BUILD_AS_FX_PLUGIN)
    // Mode: 0:FX, 1:SETTING, 2:ABOUT
    layout.add(std::make_unique<juce::AudioParameterInt>(PrKey::mode, PrName::mode, 0, GuiValue::TabNumber, 0));
#else
    // Mode: 0:OPNA, 1:OPN, 2:OPL, 3:OPLL, 4:OPL3, 5:OPM, 6: OPZX3 7:SSG, 8:WAVETABLE 9:RHYTHM, 10:ADPCM. 11:FX, 12:PRESET, 13:SETTING, 14:ABOUT
    layout.add(std::make_unique<juce::AudioParameterInt>(PrKey::mode, PrName::mode, 0, GuiValue::TabNumber, 0));

    prOpna.createLayout(layout);
	prOpn.createLayout(layout);
	prOpl.createLayout(layout);
	prOpl3.createLayout(layout);
	prOpm.createLayout(layout);
	prOpzx3.createLayout(layout);
	prSsg.createLayout(layout);
	prWt.createLayout(layout);
	prRhythm.createLayout(layout);
	prAdpcm.createLayout(layout);
#endif
	prFx.createLayout(layout);

    // マスターボリューム追加
    // 範囲: -60dB (無音に近い) ～ +6dB (少しブースト可能)
    // 初期値: -3.0dB (FMは音がデカいので少し下げておくのがコツ)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        PrKey::masterVol,      // パラメータID
        PrName::master_vol,      // 表示名
        juce::NormalisableRange<float>(PrValue::MasterVol::min, PrValue::MasterVol::max, PrValue::MasterVol::interval, PrValue::MasterVol::skew), // 範囲とステップ
        PrValue::MasterVol::initial              // デフォルト値
    ));

    return layout;
}

// ============================================================================
// Prepare To Play
// ============================================================================
void AudioPlugin2686V::prepareToPlay(double sampleRate, int samplesPerBlock)
{
#if !defined(BUILD_AS_FX_PLUGIN)
    m_synth.setCurrentPlaybackSampleRate(sampleRate);

    // Prepare RhythmCore for all voices
    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
            voice->getRhythmCore()->prepare(sampleRate);
        }
    }
#endif

	prFx.prepare(sampleRate);
}

// ============================================================================
// Release Resources
// ============================================================================
void AudioPlugin2686V::releaseResources()
{
    // Memory release is handled automatically by JUCE Synthesiser class,
    // so this can basically be empty.
}

// ============================================================================
// Process Block (Main Audio Processing Loop)
// ============================================================================
void AudioPlugin2686V::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // 不要なチャンネルのクリア
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

#if defined(BUILD_AS_FX_PLUGIN)
    SynthParams params;
#else
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    SynthParams params;

    // 【シンセモード】
    // 入力バッファはノイズの原因になるのでクリアする
    buffer.clear();

    // --- Global ---
    int m = (int)*apvts.getRawParameterValue(PrKey::mode);
    params.mode = (OscMode)m; // 0, 1, 2(RHYTHM)

    switch (params.mode)
    {
    case OscMode::OPNA:
        prOpna.processBlock(params, apvts);
        break;
    case OscMode::OPN:
		prOpn.processBlock(params, apvts);
        break;
    case OscMode::OPL:
		prOpl.processBlock(params, apvts);
        break;
    case OscMode::OPL3:
		prOpl3.processBlock(params, apvts);
        break;
    case OscMode::OPM:
		prOpm.processBlock(params, apvts);
        break;
    case OscMode::OPZX3:
		prOpzx3.processBlock(params, apvts);
        break;
    case OscMode::SSG:
		prSsg.processBlock(params, apvts);
        break;
    case OscMode::WAVETABLE:
		prWt.processBlock(params, apvts);
        break;
    case OscMode::RHYTHM:
		prRhythm.processBlock(params, apvts);
        break;
    case OscMode::ADPCM:
		prAdpcm.processBlock(params, apvts);
        break;
    }

    // Apply to each voice
    for (int i = 0; i < m_synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i)))
        {
            voice->setParameters(params);
        }
    }

    // シンセの発音 (FXモードではスキップ)
    m_synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // ヘッドルーム適応
    if (useHeadroom)
    {
        buffer.applyGain(headroomGain);
    }
#endif

	prFx.processBlock(buffer, params, apvts);

    float dbValue = *apvts.getRawParameterValue(PrKey::masterVol);

    // 2. dBをリニアな倍率(0.0 ～ 2.0くらい)に変換
    // -60dBなら 0.001, 0dBなら 1.0, +6dBなら 2.0
    float linearGain = juce::Decibels::decibelsToGain(dbValue);

    // 3. バッファ全体に適用
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        int numSamples = buffer.getNumSamples();

        for (int i = 0; i < numSamples; ++i)
        {
            // 1. マスターボリューム適用
            float sample = data[i] * linearGain;

            // 2. ソフトクリップ処理 (Satuation)
            // どんなに大きな音が来ても -1.0 ～ +1.0 の範囲に滑らかに収める
            // これにより「バリバリ」という割れが「太い歪み」に変わります
            sample = std::tanh(sample);

            data[i] = sample;
        }
    }

    // FX専用プラグインでもリアルタイム波形を取得できるようにする
    if (previewVisiblity)
    {
        auto* finalOutL = buffer.getReadPointer(0);
        int numSamples = buffer.getNumSamples();
        int bufferSize = 512;

        int shiftAmount = juce::jmin(numSamples, bufferSize);
        int keepAmount = bufferSize - shiftAmount;

        for (int i = 0; i < keepAmount; ++i) {
            float oldVal = realTimeBuffer[i + shiftAmount].load(std::memory_order_relaxed);
            realTimeBuffer[i].store(oldVal, std::memory_order_relaxed);
        }

        for (int i = 0; i < shiftAmount; ++i) {
            float newVal = finalOutL[numSamples - shiftAmount + i];
            realTimeBuffer[keepAmount + i].store(newVal, std::memory_order_relaxed);
        }
    }
}

// ============================================================================
// Editor (GUI) Related
// ============================================================================
juce::AudioProcessorEditor* AudioPlugin2686V::createEditor()
{
    // Use custom editor instead of Generic editor
    return new AudioPlugin2686VEditor(*this);
}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686V::loadAdpcmFile(const juce::File& file)
{
    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        adpcmFilePath = file.getFullPathName();

        std::unique_ptr<juce::AudioFormatReader> audioReader(reader);

        // Buffer to load the entire file
        juce::AudioBuffer<float> fileBuffer;
        fileBuffer.setSize(audioReader->numChannels, (int)audioReader->lengthInSamples);

        // Execute load
        audioReader->read(&fileBuffer, 0, (int)audioReader->lengthInSamples, 0, true, true);

        // Convert to mono std::vector<float>
        std::vector<float> sourceData;
        sourceData.resize(fileBuffer.getNumSamples());

        // Get only L channel or mix LR
        auto* channelData = fileBuffer.getReadPointer(0);
        for (int i = 0; i < fileBuffer.getNumSamples(); ++i)
        {
            sourceData[i] = channelData[i];
            // Add mixing logic here if stereo support is needed
        }

        // --- Set data to AdpcmCore for all voices ---
        // Important: Distribute data to all Voices
        for (int i = 0; i < m_synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i)))
            {
                // Set while letting AdpcmCore handle "Resampling & 4bit degradation"
                voice->getAdpcmCore()->setSampleData(sourceData, audioReader->sampleRate);
            }
        }
    }
}

// Function to load Rhythm file
void AudioPlugin2686V::loadRhythmFile(const juce::File& file, int padIndex)
{
    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        if (padIndex >= 0 && padIndex < 8) {
            rhythmFilePaths[padIndex] = file.getFullPathName();
        }

        std::unique_ptr<juce::AudioFormatReader> audioReader(reader);
        juce::AudioBuffer<float> fileBuffer;
        fileBuffer.setSize(audioReader->numChannels, (int)audioReader->lengthInSamples);
        audioReader->read(&fileBuffer, 0, (int)audioReader->lengthInSamples, 0, true, true);

        std::vector<float> sourceData(fileBuffer.getNumSamples());
        auto* channelData = fileBuffer.getReadPointer(0);
        for (int i = 0; i < fileBuffer.getNumSamples(); ++i) {
            sourceData[i] = channelData[i];
        }

        // Set data to the specified pad of RhythmCore for all voices
        for (int i = 0; i < m_synth.getNumVoices(); ++i) {
            if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
                voice->getRhythmCore()->setSampleData(padIndex, sourceData, reader->sampleRate);
            }
        }
    }
}
#endif

bool AudioPlugin2686V::hasEditor() const { return true; }

// Parameters / Settings Related
const juce::String AudioPlugin2686V::getName() const { return Global::Plugin::name; }
bool AudioPlugin2686V::acceptsMidi() const { return true; }
bool AudioPlugin2686V::producesMidi() const { return false; }
bool AudioPlugin2686V::isMidiEffect() const { return false; }
double AudioPlugin2686V::getTailLengthSeconds() const { return 0.0; }
int AudioPlugin2686V::getNumPrograms() { return 1; }
int AudioPlugin2686V::getCurrentProgram() { return 0; }
void AudioPlugin2686V::setCurrentProgram(int index) {}
const juce::String AudioPlugin2686V::getProgramName(int index) { return {}; }
void AudioPlugin2686V::changeProgramName(int index, const juce::String& newName) {}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686V::setPresetToXml(std::unique_ptr<juce::XmlElement>& xml)
{
    // メタデータとパスを属性として追加
    xml->setAttribute(PresetKey::name, sanitizeString(presetName, PresetValue::MetaData::Length::name));
    xml->setAttribute(PresetKey::author, sanitizeString(presetAuthor, PresetValue::MetaData::Length::author));
    xml->setAttribute(PresetKey::version, sanitizeString(presetVersion, PresetValue::MetaData::Length::version));
    xml->setAttribute(PresetKey::comment, sanitizeString(presetComment, PresetValue::MetaData::Length::comment));
    xml->setAttribute(PresetKey::mode, getModeName((OscMode)(int)*apvts.getRawParameterValue(PrKey::mode)));
    xml->setAttribute(PresetKey::puginVersion, Global::Plugin::version);

    // サンプルパス保存 (ADPCM)
    xml->setAttribute(PresetKey::adpcmPath, makePathRelative(juce::File(adpcmFilePath)));

    // サンプルパス保存 (RHYTHM)
    for (int i = 0; i < 8; ++i) {
        xml->setAttribute(PresetKey::rhythmPathPrefix + juce::String(i), makePathRelative(juce::File(rhythmFilePaths[i])));
    }

    // サンプルパス保存 (OPZX3)
    for (int i = 0; i < 4; ++i) {
        xml->setAttribute(PresetKey::opzx3PathPrefix + juce::String(i), makePathRelative(juce::File(opzx3PcmFilePaths[i])));
    }
};

void AudioPlugin2686V::getPresetFromXml(std::unique_ptr<juce::XmlElement>& xmlState)
{
    if (xmlState.get() != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        // パラメータ復帰
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

        // メタデータ復帰
        presetName = xmlState->getStringAttribute(PresetKey::name, PresetValue::MetaData::Initial::name);
        presetAuthor = xmlState->getStringAttribute(PresetKey::author, PresetValue::MetaData::Initial::author);
        presetVersion = xmlState->getStringAttribute(PresetKey::version, PresetValue::MetaData::Initial::version);
        presetComment = xmlState->getStringAttribute(PresetKey::comment, PresetValue::MetaData::Initial::comment);
        presetPluginVersion = xmlState->getStringAttribute(PresetKey::puginVersion, Global::Plugin::version);

        // サンプル復帰 (ADPCM)
        juce::String storedAdpcm = xmlState->getStringAttribute(PresetKey::adpcmPath);
        juce::File adpcmFile = resolvePath(storedAdpcm);
        if (adpcmFile.existsAsFile()) {
            loadAdpcmFile(adpcmFile);
        }

        // サンプル復帰 (RHYTHM)
        for (int i = 0; i < 8; ++i) {
            juce::String storedRhy = xmlState->getStringAttribute(PresetKey::rhythmPathPrefix + juce::String(i));
            juce::File rhyFile = resolvePath(storedRhy);
            if (rhyFile.existsAsFile()) {
                loadRhythmFile(rhyFile, i);
            }
        }

        // サンプル復帰 (OPZX3)
        for (int i = 0; i < 4; ++i) {
            juce::String storedPath = xmlState->getStringAttribute(PresetKey::opzx3PathPrefix + juce::String(i));
            juce::File file = resolvePath(storedPath);
            if (file.existsAsFile()) {
                loadOpzx3PcmFile(i, file);
            }
        }
    }
};
#endif

// ============================================================================
// State Information
// ============================================================================
void AudioPlugin2686V::getStateInformation(juce::MemoryBlock& destData) {
// 2686FXではプリセット情報のやり取りは無いので、何もしない関数を定義する
#if !defined(BUILD_AS_FX_PLUGIN)
    // APVTSの状態をXMLとして取得
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    setPresetToXml(xml);

    copyXmlToBinary(*xml, destData);
#endif
}

void AudioPlugin2686V::setStateInformation(const void* data, int sizeInBytes) {
    // 2686FXではプリセット情報のやり取りは無いので、何もしない関数を定義する
#if !defined(BUILD_AS_FX_PLUGIN)
// 1. データ自体のバリデーション
    if (data == nullptr || sizeInBytes <= 0) return;

    std::unique_ptr<juce::XmlElement> xmlState;

    // データの先頭をチェックして、テキストかバイナリかを判別する
    // 先頭の空白をスキップして '<' で始まっていればテキストとみなす
    const char* charData = static_cast<const char*>(data);
    int start = 0;
    while (start < sizeInBytes && std::isspace(charData[start])) {
        start++;
    }

    bool isProbablyText = (start < sizeInBytes && charData[start] == '<');

    if (isProbablyText)
    {
        // テキストとして解析 (不正なタグがあれば nullptr が返る)
        juce::String textData = juce::String::createStringFromData(data, sizeInBytes);
        xmlState = juce::XmlDocument::parse(textData);
    }
    else
    {
        // バイナリとして解析 (テキストではない場合のみ実行することで jassert を回避)
        xmlState = getXmlFromBinary(data, sizeInBytes);
    }

    // 3. バイナリ読み込みに失敗した場合、テキストXMLとして解析を試みる (フォールバック)
    // (手動で編集されたXMLや、DAWによってはテキストで保存される場合があるため)
    if (xmlState == nullptr)
    {
        juce::String textData = juce::String::createStringFromData(data, sizeInBytes);
        xmlState = juce::XmlDocument::parse(textData);
    }

    // 4. XMLオブジェクトが生成できた場合のみ処理を進める
    if (xmlState != nullptr)
    {
        // さらに安全性を高めるなら、ここでルートタグ名のチェックも可能です
        // if (xmlState->hasTagName("Plugin_2686V") || xmlState->hasTagName("Parameters")) 
        // {
        getPresetFromXml(xmlState);
        // }
    }
    else
    {
        // 読み込み失敗（データ破損など）。
        // ここでは何もしないか、デバッグログを出す程度に留めることでクラッシュを防ぎます。
        DBG("setStateInformation: Failed to parse XML state.");
    }
#endif
}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686V::savePreset(const juce::File& file)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    setPresetToXml(xml);

    xml->writeTo(file);
}

void AudioPlugin2686V::loadPreset(const juce::File& file)
{
    juce::XmlDocument xmlDoc(file);
    std::unique_ptr<juce::XmlElement> xmlState = xmlDoc.getDocumentElement();

    getPresetFromXml(xmlState);
}
#endif

void AudioPlugin2686V::addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::ar, prefix + PrKey::Post::Adsr::ar, PrValue::Adsr::Ar::min, PrValue::Adsr::Ar::max, PrValue::Adsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::dr, prefix + PrKey::Post::Adsr::dr, PrValue::Adsr::Ar::min, PrValue::Adsr::Ar::max, PrValue::Adsr::Ar::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::sl, prefix + PrKey::Post::Adsr::sl, PrValue::Adsr::Sl::min, PrValue::Adsr::Sl::max, PrValue::Adsr::Sl::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + PrKey::Post::Adsr::rr, prefix + PrKey::Post::Adsr::rr, PrValue::Adsr::Rr::min, PrValue::Adsr::Rr::max, PrValue::Adsr::Rr::initial));
}

// 環境設定を保存
void AudioPlugin2686V::saveEnvironment(const juce::File& file)
{
    juce::XmlElement xml(SettingsKey::envCode);

    xml.setAttribute(SettingsKey::wallpaperPath, wallpaperPath);
    xml.setAttribute(SettingsKey::wallpaperMode, wallpaperMode);
    xml.setAttribute(SettingsKey::defaultSampleDir, defaultSampleDir);
    xml.setAttribute(SettingsKey::defaultPresetDir, defaultPresetDir);
    xml.setAttribute(SettingsKey::showTooltips, showTooltips);
    xml.setAttribute(SettingsKey::useHeadroom, useHeadroom);
    xml.setAttribute(SettingsKey::headroomGain, headroomGain);
    xml.setAttribute(SettingsKey::showVirtualKeyboard, showVirtualKeyboard);

    xml.writeTo(file);
}

// 環境設定を読み込み
void AudioPlugin2686V::loadEnvironment(const juce::File& file)
{
    juce::XmlDocument xmlDoc(file);
    std::unique_ptr<juce::XmlElement> xml = xmlDoc.getDocumentElement();

    if (xml.get() != nullptr && xml->hasTagName(SettingsKey::envCode))
    {
        wallpaperPath = xml->getStringAttribute(SettingsKey::wallpaperPath);
        wallpaperMode = xml->getIntAttribute(SettingsKey::wallpaperMode);
        defaultSampleDir = xml->getStringAttribute(SettingsKey::defaultSampleDir);
        defaultPresetDir = xml->getStringAttribute(SettingsKey::defaultPresetDir);
        showTooltips = xml->getBoolAttribute(SettingsKey::showTooltips, SettingsValue::Initial::showTooltip);
        useHeadroom = xml->getBoolAttribute(SettingsKey::useHeadroom, SettingsValue::Initial::useHeadroom);
        headroomGain = xml->getDoubleAttribute(SettingsKey::headroomGain, SettingsValue::Initial::headroomGain);
        showVirtualKeyboard = xml->getBoolAttribute(SettingsKey::showVirtualKeyboard, SettingsValue::Initial::showVirtualKeyboard);
#if !defined(BUILD_AS_FX_PLUGIN)
        // 内部変数の更新
        if (juce::File(defaultSampleDir).isDirectory()) {
            lastSampleDirectory = juce::File(defaultSampleDir);
        }
#endif
    }
}

void AudioPlugin2686V::loadStartupSettings()
{
    // 1. 読み込むディレクトリとファイル名を指定
    // 例: マイドキュメントフォルダ内の "2686V" フォルダにある "init_preset.xml"
    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    auto pluginDir = docDir.getChildFile(Io::Folder::asset);
    auto presetFile = pluginDir.getChildFile(SettingsValue::File::Name::initial);

    if (!pluginDir.exists()) {
        pluginDir.createDirectory();
    }

    bool loadSuccess = false;

    // 2. ファイルが存在するかチェック
    if (presetFile.existsAsFile())
    {
        juce::XmlDocument xmlDoc(presetFile);
        std::unique_ptr<juce::XmlElement> xml = xmlDoc.getDocumentElement();

        // XMLとして不正、またはルートタグが期待するものでない場合は破損とみなす
        if (xml == nullptr || !xml->hasTagName(SettingsKey::envCode))
        {
            DBG("Startup settings file is corrupted. Deleting...");
            presetFile.deleteFile(); // 破損ファイルを削除
            // loadSuccess = false のまま
        }
        else
        {
            // 正常なら読み込む
            loadEnvironment(presetFile);
            loadSuccess = true;
        }
    }

    // プリセットディレクトリ・ADPCMディレクトリが空の時は初期値を設定する
    if (defaultPresetDir.isEmpty() || !juce::File(defaultPresetDir).isDirectory())
    {
        auto newPresetDir = pluginDir.getChildFile(Io::Folder::preset);

        // 存在していなければ作成
        if (!newPresetDir.exists()) {
            newPresetDir.createDirectory();
        }

        defaultPresetDir = newPresetDir.getFullPathName();
    }

    if (defaultSampleDir.isEmpty() || !juce::File(defaultSampleDir).isDirectory())
    {
        auto newSampleDir = pluginDir.getChildFile(Io::Folder::sample);

        // 存在していなければ作成
        if (!newSampleDir.exists()) {
            newSampleDir.createDirectory();
        }

        defaultSampleDir = newSampleDir.getFullPathName();
#if !defined(BUILD_AS_FX_PLUGIN)
        lastSampleDirectory = newSampleDir;
#endif
    }

}

juce::String AudioPlugin2686V::getDefaultPresetDir()
{
    return defaultPresetDir;
}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686V::unloadAdpcmFile()
{
    // パス情報を削除
    adpcmFilePath.clear();

    // 空のデータを作成
    std::vector<float> emptyData(1, 0.0f);

    // 全ボイスの ADPCM Core に空データをセット（＝クリア）
    for (int i = 0; i < m_synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i)))
        {
            // レートはなんでも良いので適当な値(44100)を渡す
            voice->getAdpcmCore()->setSampleData(emptyData, 44100.0);
        }
    }
}

void AudioPlugin2686V::unloadRhythmFile(int padIndex)
{
    // インデックスチェック
    if (padIndex < 0 || padIndex >= 8) return;

    // パス情報を削除
    rhythmFilePaths[padIndex].clear();

    // 空のデータを作成
    std::vector<float> emptyData(1, 0.0f);

    // 全ボイスの Rhythm Core の該当パッドに空データをセット
    for (int i = 0; i < m_synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i)))
        {
            voice->getRhythmCore()->setSampleData(padIndex, emptyData, 44100.0);
        }
    }
}
#endif

// 絶対パスのFileを、defaultSampleDirからの相対パス文字列に変換する
juce::String AudioPlugin2686V::makePathRelative(const juce::File& targetFile)
{
    // ファイルが無効、またはディレクトリ未設定ならそのまま絶対パスを返す
    if (targetFile == juce::File() || defaultSampleDir.isEmpty())
        return targetFile.getFullPathName();

    std::filesystem::path fpath = std::filesystem::path(targetFile.getFullPathName().toStdString());

    return juce::String(fpath.lexically_relative(defaultSampleDir.toStdString()).generic_string());
}

// パス文字列（相対 or 絶対）を、読み込み可能なFileオブジェクトに復元する
juce::File AudioPlugin2686V::resolvePath(const juce::String& pathStr)
{
    if (pathStr.isEmpty()) return juce::File();

    std::string fpathStr = pathStr.toStdString();

    std::filesystem::path fPath = fpathStr;

    // すでに絶対パスであれば、そのまま使う (互換性維持のため)
    if (fPath.is_absolute())
    {
        juce::File file(pathStr);

        return file;
    }

    // 相対パスの場合は defaultSampleDir と結合する
    if (defaultSampleDir.isNotEmpty())
    {
        std::filesystem::path baseDir = defaultSampleDir.toStdString();
        juce::String fullPath = juce::String(baseDir.append(fpathStr).generic_string());

        return juce::File(fullPath);
    }

    // ベースディレクトリがない場合は解決不能だが、一応そのまま返す
    juce::File file(pathStr);

    return file;
}

// コメントなどのサニタイズ (static)
juce::String AudioPlugin2686V::sanitizeString(const juce::String& input, int length = 1024)
{
    // 1. 文字列のトリム（前後の空白除去）
    juce::String clean = input.trim();

    // 2. 文字数制限 (例: length文字まで)
    // バッファオーバーフロー対策の基本です
    if (clean.length() > length)
    {
        clean = clean.substring(0, length);
    }

    juce::String sanitized = "";
    for (auto t : clean)
    {
        // 印字可能文字、または許可された制御文字なら採用
        if (juce::CharacterFunctions::isPrintable(t) || Global::Plugin::allowedControlChars.containsChar(t))
        {
            sanitized += t;
        }
    }

    return sanitized;
}

// ============================================================================
// Entry Point
// ============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPlugin2686V();
}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686V::initPreset()
{
    // 1. 全パラメータをデフォルト値(Normalized 0.0-1.0)にリセット
    auto& params = getParameters();
    for (auto* param : params)
    {
        if (auto* p = dynamic_cast<juce::AudioProcessorParameterWithID*>(param))
        {
            // getDefaultValue() は正規化された値を返すのでそのままセット
            p->setValueNotifyingHost(p->getDefaultValue());
        }
    }

    // 2. メタデータのリセット
    presetName = PresetValue::MetaData::Initial::name;
    presetAuthor = PresetValue::MetaData::Initial::author;
    presetVersion = PresetValue::MetaData::Initial::version;
    presetComment = PresetValue::MetaData::Initial::comment;

    // 3. サンプルのアンロードとパスクリア
    unloadAdpcmFile();
    // unloadAdpcmFile内で adpcmFilePath.clear() されています

    for (int i = 0; i < 8; ++i) {
        unloadRhythmFile(i);
        // unloadRhythmFile内で rhythmFilePaths[i].clear() されています
    }
}

void AudioPlugin2686V::loadOpzx3PcmFile(int opIndex, const juce::File& file)
{
    if (opIndex < 0 || opIndex >= 4) return;

    if (auto* reader = formatManager.createReaderFor(file))
    {
        juce::AudioBuffer<float> tempBuffer(1, (int)reader->lengthInSamples);
        reader->read(&tempBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
        delete reader;

        auto* readPtr = tempBuffer.getReadPointer(0);
        opzx3PcmBuffers[opIndex].assign(readPtr, readPtr + tempBuffer.getNumSamples());
        opzx3PcmFilePaths[opIndex] = file.getFullPathName();

        for (int i = 0; i < m_synth.getNumVoices(); ++i) {
            if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
                voice->setOpzx3PcmBuffer(opIndex, &opzx3PcmBuffers[opIndex]);
            }
        }
    }
}

void AudioPlugin2686V::unloadOpzx3PcmFile(int opIndex)
{
    if (opIndex < 0 || opIndex >= 4) return;

    opzx3PcmBuffers[opIndex].clear();
    opzx3PcmFilePaths[opIndex] = juce::String();

    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
            voice->setOpzx3PcmBuffer(opIndex, &opzx3PcmBuffers[opIndex]);
        }
    }
}

void AudioPlugin2686V::generatePreviewWaveform(std::vector<float>& destBuffer)
{
    // 1. パラメータの取得と設定
    SynthParams currentParams;
    int m = (int)*apvts.getRawParameterValue(PrKey::mode);
    currentParams.mode = (OscMode)m;

    switch (currentParams.mode) {
    case OscMode::OPNA:      prOpna.processBlock(currentParams, apvts); break;
    case OscMode::OPN:       prOpn.processBlock(currentParams, apvts); break;
    case OscMode::OPL:       prOpl.processBlock(currentParams, apvts); break;
    case OscMode::OPL3:      prOpl3.processBlock(currentParams, apvts); break;
    case OscMode::OPM:       prOpm.processBlock(currentParams, apvts); break;
    case OscMode::OPZX3:     prOpzx3.processBlock(currentParams, apvts); break;
    case OscMode::SSG:       prSsg.processBlock(currentParams, apvts); break;
    case OscMode::WAVETABLE: prWt.processBlock(currentParams, apvts); break;
    case OscMode::RHYTHM:    prRhythm.processBlock(currentParams, apvts); break;
    case OscMode::ADPCM:     prAdpcm.processBlock(currentParams, apvts); break;
    }

    if (auto* voice = dynamic_cast<SynthVoice*>(previewSynth.getVoice(0))) {
        voice->setParameters(currentParams);
        for (int i = 0; i < 4; ++i) voice->setOpzx3PcmBuffer(i, &opzx3PcmBuffers[i]);
    }

    // 2. 1周期をピッタリ整数サンプルにするため、SampleRateを44000Hzに偽装する
    previewSynth.setCurrentPlaybackSampleRate(44000.0);
    previewSynth.noteOn(1, 69, 1.0f); // 69 = A3 (440.0Hz)

    // 3. アタックフェーズのスキップ (エンベロープを安定させる)
    juce::AudioBuffer<float> skipBuffer(2, 512);
    for (int i = 0; i < 40; ++i) {
        skipBuffer.clear();
        previewSynth.renderNextBlock(skipBuffer, juce::MidiBuffer(), 0, 512);
    }

    // 4. 1周期をピッタリ100サンプルにする
    int samplesPerCycle = 100; // 44000 / 440 = 100
    int renderSamples = samplesPerCycle * 3;
    juce::AudioBuffer<float> renderBuffer(2, renderSamples);
    renderBuffer.clear();
    previewSynth.renderNextBlock(renderBuffer, juce::MidiBuffer(), 0, renderSamples);

    // 一時的なFXプロセッサを作成（現在の状態を汚さないため）
    FxProcessor tempFx;
    tempFx.prepare(44000.0);
    // 現在のパラメータを適用してFX処理
    tempFx.processBlock(renderBuffer, currentParams, apvts);

    // マスターボリュームとソフトクリッパーの適用
    float dbValue = *apvts.getRawParameterValue(PrKey::masterVol);
    float linearGain = juce::Decibels::decibelsToGain(dbValue);

    for (int ch = 0; ch < renderBuffer.getNumChannels(); ++ch) {
        auto* data = renderBuffer.getWritePointer(ch);
        for (int i = 0; i < renderSamples; ++i) {
            data[i] = std::tanh(data[i] * linearGain);
        }
    }

    auto* readPtr = renderBuffer.getReadPointer(0);

    // 5. DCオフセット（波形の全体的な上下のズレ）を計算
    float dcOffset = 0.0f;
    for (int i = 0; i < renderSamples; ++i) {
        dcOffset += readPtr[i];
    }
    dcOffset /= renderSamples;

    // 6. オシロスコープのトリガー（ゼロクロッシング）を探す
    int startIndex = 0;
    for (int i = 0; i < renderSamples - samplesPerCycle; ++i) {
        float current = readPtr[i] - dcOffset;
        float next = readPtr[i + 1] - dcOffset;

        if (current <= 0.0f && next > 0.0f) {
            startIndex = i;
            break;
        }
    }

    // 1周期分 ＋ 1サンプル（次の周期の始まりの0）を取得する
    int drawSamples = samplesPerCycle + 1;
    destBuffer.assign(drawSamples, 0.0f);

    float maxAmplitude = 0.0001f;
    for (int i = 0; i < drawSamples; ++i) {
        float val = readPtr[startIndex + i] - dcOffset;
        maxAmplitude = std::max(maxAmplitude, std::abs(val));
    }

    for (int i = 0; i < drawSamples; ++i) {
        float val = readPtr[startIndex + i] - dcOffset;
        destBuffer[i] = val / maxAmplitude;
    }

    // 8. 停止
    previewSynth.noteOff(1, 69, 0.0f, false);
}
#endif
