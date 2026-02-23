#include "PluginProcessor.h"

// ============================================================================
// Constructor
// ============================================================================
AudioPlugin2686V::AudioPlugin2686V()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if defined(BUILD_AS_FX_PLUGIN)
        // FXモード: ステレオ入力あり、ステレオ出力あり
        .withInput(audioInputName, juce::AudioChannelSet::stereo(), true)
        .withOutput(audioOutputName, juce::AudioChannelSet::stereo(), true)
#else
        // シンセモード: 入力なし、ステレオ出力あり
        .withOutput(audioOutputName, juce::AudioChannelSet::stereo(), true)
#endif
    ),
    // Initialize APVTS (Parameters are created here)
    apvts(*this, nullptr, pluginParametersName, createParameterLayout()) // APVTSの初期化
#endif
{
#if !defined(BUILD_AS_FX_PLUGIN)
    synthSound = std::make_unique<SynthSound>();

    for (int i = 0; i < voices; i++)
    {
        synthVoices.push_back(std::make_unique<SynthVoice>());
    }

    m_synth.addSound(synthSound.get());

    for (int i = 0; i < voices; i++)
    {
        m_synth.addVoice(synthVoices[i].get());
    }

    previewSound = std::make_unique<SynthSound>();
    previewSynth.addSound(previewSound.get());
    previewSynth.addVoice(new SynthVoice()); // 1ボイスだけ追加

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
    layout.add(std::make_unique<juce::AudioParameterInt>(codeMode, modeLabel, 0, TabNumber, 0));
#else
    // Mode: 0:OPNA, 1:OPN, 2:OPL, 3:OPLL, 4:OPL3, 5:OPM, 6: OPZX3 7:SSG, 8:WAVETABLE 9:RHYTHM, 10:ADPCM. 11:FX, 12:PRESET, 13:SETTING, 14:ABOUT
    layout.add(std::make_unique<juce::AudioParameterInt>(codeMode, modeLabel, 0, TabNumber, 0));

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

    // ★マスターボリューム追加
    // 範囲: -60dB (無音に近い) ～ +6dB (少しブースト可能)
    // 初期値: -3.0dB (FMは音がデカいので少し下げておくのがコツ)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        codeMasterVol,      // パラメータID
        masterVolumeLabel,      // 表示名
        juce::NormalisableRange<float>(masterVolumeMin, masterVolumeMax, masterVolumeInterval, masterVolumeSkew), // 範囲とステップ
        masterVolumeDefault              // デフォルト値
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

    SynthParams params;

    // FXモードかどうかで分岐
#if defined(BUILD_AS_FX_PLUGIN)
    // 【FXモード】
    // 入力音(bufferに入っている音)をそのまま使うので、buffer.clear() はしない！
    // シンセの発音処理(m_synth.renderNextBlock)もしない！
#else
    // 【シンセモード】
    // 入力バッファはノイズの原因になるのでクリアする
    buffer.clear();

    // --- Global ---
    int m = (int)*apvts.getRawParameterValue(codeMode);
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

    float dbValue = *apvts.getRawParameterValue(codeMasterVol);

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
const juce::String AudioPlugin2686V::getName() const { return pluginName; }
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
    xml->setAttribute(settingPresetName, sanitizeString(presetName, presetNameLength));
    xml->setAttribute(settingPresetAuthor, sanitizeString(presetAuthor, presetAuthorLength));
    xml->setAttribute(settingPresetVersion, sanitizeString(presetVersion, presetVersionLength));
    xml->setAttribute(settingPresetComment, sanitizeString(presetComment, commentLength));
    xml->setAttribute(settingActiveModeName, getModeName((OscMode)(int)*apvts.getRawParameterValue(codeMode)));
    xml->setAttribute(settingPluginVersion, pluginVersion);

    // サンプルパス保存 (ADPCM)
    xml->setAttribute(settingAdpcmPath, makePathRelative(juce::File(adpcmFilePath)));

    // サンプルパス保存 (RHYTHM)
    for (int i = 0; i < 8; ++i) {
        xml->setAttribute(settingRhythmPathPrefix + juce::String(i), makePathRelative(juce::File(rhythmFilePaths[i])));
    }

    // サンプルパス保存 (OPZX3)
    for (int i = 0; i < 4; ++i) {
        xml->setAttribute(settingOpzx3PathPrefix + juce::String(i), makePathRelative(juce::File(opzx3PcmFilePaths[i])));
    }
};

void AudioPlugin2686V::getPresetFromXml(std::unique_ptr<juce::XmlElement>& xmlState)
{
    if (xmlState.get() != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        // パラメータ復帰
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

        // メタデータ復帰
        presetName = xmlState->getStringAttribute(settingPresetName, defaultPresetName);
        presetAuthor = xmlState->getStringAttribute(settingPresetAuthor, defaultPresetAuthor);
        presetVersion = xmlState->getStringAttribute(settingPresetVersion, defaultPresetVersion);
        presetComment = xmlState->getStringAttribute(settingPresetComment, defaultPresetComment);
        presetPluginVersion = xmlState->getStringAttribute(settingPluginVersion, pluginVersion);

        // サンプル復帰 (ADPCM)
        juce::String storedAdpcm = xmlState->getStringAttribute(settingAdpcmPath);
        juce::File adpcmFile = resolvePath(storedAdpcm);
        if (adpcmFile.existsAsFile()) {
            loadAdpcmFile(adpcmFile);
        }

        // サンプル復帰 (RHYTHM)
        for (int i = 0; i < 8; ++i) {
            juce::String storedRhy = xmlState->getStringAttribute(defaultPresetComment + juce::String(i));
            juce::File rhyFile = resolvePath(storedRhy);
            if (rhyFile.existsAsFile()) {
                loadRhythmFile(rhyFile, i);
            }
        }

        // サンプル復帰 (OPZX3)
        for (int i = 0; i < 4; ++i) {
            juce::String storedPath = xmlState->getStringAttribute(settingOpzx3PathPrefix + juce::String(i));
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
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, prefix + opPostArLabel, opArMin, opArMax, opArDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postDr, prefix + opPostDrLabel, opDrMin, opDrMax, opDrDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSl, prefix + opPostSlLabel, opSlMin, opSlMax, opSlDefault));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, prefix + opPostRrLabel, opRrMin, opRrMax, opRrDefault));
}

// 環境設定を保存
void AudioPlugin2686V::saveEnvironment(const juce::File& file)
{
    juce::XmlElement xml(envCode);

    xml.setAttribute(settingWallpaperPath, wallpaperPath);
    xml.setAttribute(settingWallpaperMode, wallpaperMode);
    xml.setAttribute(settingDefaultSampleDir, defaultSampleDir);
    xml.setAttribute(settingDefaultPresetDir, defaultPresetDir);
    xml.setAttribute(settingShowTooltips, showTooltips);
    xml.setAttribute(settingUseHeadroom, useHeadroom);
    xml.setAttribute(settingHeadroomGain, headroomGain);

    xml.writeTo(file);
}

// 環境設定を読み込み
void AudioPlugin2686V::loadEnvironment(const juce::File& file)
{
    juce::XmlDocument xmlDoc(file);
    std::unique_ptr<juce::XmlElement> xml = xmlDoc.getDocumentElement();

    if (xml.get() != nullptr && xml->hasTagName(envCode))
    {
        wallpaperPath = xml->getStringAttribute(settingWallpaperPath);
        wallpaperMode = xml->getIntAttribute(settingWallpaperMode);
        defaultSampleDir = xml->getStringAttribute(settingDefaultSampleDir);
        defaultPresetDir = xml->getStringAttribute(settingDefaultPresetDir);
        showTooltips = xml->getBoolAttribute(settingShowTooltips, defaultShowTooltip);
        useHeadroom = xml->getBoolAttribute(settingUseHeadroom, defaultUseHeadroom);
        headroomGain = xml->getDoubleAttribute(settingHeadroomGain, defaultHeadroomGain);
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
    auto pluginDir = docDir.getChildFile(assetFolder);
    auto presetFile = pluginDir.getChildFile(defaultSettingFilename);

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
        if (xml == nullptr || !xml->hasTagName("PREF_2686V"))
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
        auto newPresetDir = pluginDir.getChildFile(presetFolder);

        // 存在していなければ作成
        if (!newPresetDir.exists()) {
            newPresetDir.createDirectory();
        }

        defaultPresetDir = newPresetDir.getFullPathName();
    }

    if (defaultSampleDir.isEmpty() || !juce::File(defaultSampleDir).isDirectory())
    {
        auto newSampleDir = pluginDir.getChildFile(sampleFolder);

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
        if (juce::CharacterFunctions::isPrintable(t) || allowedControlChars.containsChar(t))
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
    presetName = defaultPresetName;
    presetAuthor = defaultPresetAuthor;
    presetVersion = defaultPresetVersion;
    presetComment = defaultPresetComment;

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

        for (int i = 0; i < voices; i++)
        {
            synthVoices[i]->setOpzx3PcmBuffer(opIndex, &opzx3PcmBuffers[opIndex]);
        }
    }
}

void AudioPlugin2686V::unloadOpzx3PcmFile(int opIndex)
{
    if (opIndex < 0 || opIndex >= 4) return;

    opzx3PcmBuffers[opIndex].clear();
    opzx3PcmFilePaths[opIndex] = juce::String();

    for (int i = 0; i < voices; i++)
    {
        synthVoices[i]->setOpzx3PcmBuffer(opIndex, &opzx3PcmBuffers[opIndex]);
    }
}
#endif

void AudioPlugin2686V::generatePreviewWaveform(std::vector<float>& destBuffer)
{
    // 1. 現在の全パラメータを APVTS から取得して SynthParams に詰める
    SynthParams currentParams;
    int m = (int)*apvts.getRawParameterValue(codeMode);
    currentParams.mode = (OscMode)m;

    // 現在のモードに合わせてパラメータを更新
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

    // 2. プレビュー用ボイスにパラメータを適用
    if (auto* voice = dynamic_cast<SynthVoice*>(previewSynth.getVoice(0))) {
        voice->setParameters(currentParams);
        // (必要であれば PCM バッファなどもここで渡す)
        for (int i = 0; i < 4; ++i) {
            voice->setOpzx3PcmBuffer(i, &opzx3PcmBuffers[i]);
        }
    }

    // 3. サンプルレートを固定して発音開始 (110Hz = A2)
    previewSynth.setCurrentPlaybackSampleRate(44100.0);
    previewSynth.noteOn(1, 45, 1.0f);

    // 4. 空回し (アタックフェーズをスキップして波形を安定させる)
    // 512サンプル × 20回 ≒ 約0.23秒ほど時間を進める
    juce::AudioBuffer<float> skipBuffer(2, 512);
    for (int i = 0; i < 20; ++i) {
        skipBuffer.clear();
        previewSynth.renderNextBlock(skipBuffer, juce::MidiBuffer(), 0, 512);
    }

    // 5. 描画用の波形を取得 (400サンプル ≒ 110Hzのちょうど1周期分)
    juce::AudioBuffer<float> renderBuffer(2, 400);
    renderBuffer.clear();
    previewSynth.renderNextBlock(renderBuffer, juce::MidiBuffer(), 0, 400);

    // 6. オートノーマライズ (音量が小さくても画面いっぱいに波形の形を描画する)
    destBuffer.assign(400, 0.0f);
    auto* readPtr = renderBuffer.getReadPointer(0); // Lチャンネルを使用

    float maxAmplitude = 0.0001f; // ゼロ除算防止
    for (int i = 0; i < 400; ++i) {
        maxAmplitude = std::max(maxAmplitude, std::abs(readPtr[i]));
    }

    for (int i = 0; i < 400; ++i) {
        // 波形の形を見やすくするため、最大値が 1.0 になるように拡大
        destBuffer[i] = readPtr[i] / maxAmplitude;
    }

    // 7. 発音停止
    previewSynth.noteOff(1, 45, 0.0f, false);
}
