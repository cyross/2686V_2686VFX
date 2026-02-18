#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <filesystem>
#include <fstream>

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
    m_synth.addSound(new SynthSound());

    for (int i = 0; i < 32; i++)
    {
        m_synth.addVoice(new SynthVoice());
    }

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

    createOpnaParameterLayout(layout);
    createOpnParameterLayout(layout);
    createOplParameterLayout(layout);
	createOpl3ParameterLayout(layout);
	createOpmParameterLayout(layout);
    createOpzx3ParameterLayout(layout);
    createSsgParameterLayout(layout);
    createWavetableParameterLayout(layout);
    createRhythmParameterLayout(layout);
    createAdpcmParameterLayout(layout);
#endif
    createFxParameterLayout(layout);

    // ★マスターボリューム追加
    // 範囲: -60dB (無音に近い) ～ +6dB (少しブースト可能)
    // 初期値: -3.0dB (FMは音がデカいので少し下げておくのがコツ)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        codeMasterVol,      // パラメータID
        masterVolumeLabel,      // 表示名
        juce::NormalisableRange<float>(-60.0f, 6.0f, 0.1f, 1.0f), // 範囲とステップ
        -6.0f              // デフォルト値
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

    effects.prepare(sampleRate);
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
        processOpnaBlock(params);
        break;
    case OscMode::OPN:
        processOpnBlock(params);
        break;
    case OscMode::OPL:
        processOplBlock(params);
        break;
    case OscMode::OPL3:
        processOpl3Block(params);
        break;
    case OscMode::OPM:
        processOpmBlock(params);
        break;
    case OscMode::OPZX3:
        processOpzx3Block(params);
        break;
    case OscMode::SSG:
        processSsgBlock(params);
        break;
    case OscMode::WAVETABLE:
        processWavetableBlock(params);
        break;
    case OscMode::RHYTHM:
        processRhythmBlock(params);
        break;
    case OscMode::ADPCM:
        processAdpcmBlock(params);
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

    processFxBlock(buffer, params);

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

    xml->setAttribute(settingAdpcmPath, makePathRelative(juce::File(adpcmFilePath)));
    for (int i = 0; i < 8; ++i) {
        xml->setAttribute(settingRhythmPathPrefix + juce::String(i), makePathRelative(juce::File(rhythmFilePaths[i])));
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

        // サンプル復帰 (Rhythm)
        for (int i = 0; i < 8; ++i) {
            juce::String storedRhy = xmlState->getStringAttribute(defaultPresetComment + juce::String(i));
            juce::File rhyFile = resolvePath(storedRhy);
            if (rhyFile.existsAsFile()) {
                loadRhythmFile(rhyFile, i);
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
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, prefix + opPostArLabel, 0.03f, 5.0f, 0.03f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postDr, prefix + opPostDrLabel, 0.0f, 5.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSl, prefix + opPostSlLabel, 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, prefix + opPostRrLabel, 0.03f, 5.0f, 0.03f));
}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686V::createOpnaParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPNA (YM2608) Parameters
    // ==========================================
    juce::String code = codeOpna;

    layout.add(std::make_unique<juce::AudioParameterInt>(code + postAlg, code + mPostAlgTitle, 0, 7, 7));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb0, code + mPostFb0Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb2, code + mPostFb2Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postBit, code + mPostBitTitle, 0, 4, 4));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postRate, code + mPostRateTitle, 1, 7, 2)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postLFreq, code + mPostLfoFreq, 0.1f, 200.0f, 5.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postLPms, code + mPostLfoPms, 0, 7, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postLAms, code + mPostLfoAms, 0, 3, 0));

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = code + codeOp + juce::String(op);
        juce::String namePrefix = code + opLabel + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMul, namePrefix + opPostMulLabel, 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt, namePrefix + opPostDtLabel, 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, namePrefix + opPostArLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postDr, namePrefix + opPostDrLabel, 0.0f, 5.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSr, namePrefix + opPostSrLabel, 0.0f, 10.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSl, namePrefix + opPostSlLabel, 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postSe, namePrefix + opPostSeLabel, 0, 15, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSeFreq, namePrefix + opPostSeFreqLabel, 0.1f, 20.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postTl, namePrefix + opPostTlLabel, 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postKs, namePrefix + opPostKsLabel, 0, 3, 0)); // KS (0 to 3)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postAm, namePrefix + opPostAmLabel, false)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postFix, namePrefix + opPostFixLabel, false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postFixFreq, namePrefix + opPostFixFreqLabel, juce::NormalisableRange<float>(0.0f, 8000.0f, 0.0f, 0.3f), 440.0f));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, false)); // OP Mask (Switch)
    }
}

void AudioPlugin2686V::createOpnParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPN (YM2203) Parameters
    // ==========================================
    juce::String code = codeOpn;

    layout.add(std::make_unique<juce::AudioParameterInt>(code + postAlg, code + mPostAlgTitle, 0, 7, 7));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb0, code + mPostFb0Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb2, code + mPostFb2Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postBit, code + mPostBitTitle, 0, 4, 4));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postRate, code + mPostRateTitle, 1, 7, 2)); // Default 6 (16kHz)

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = code + codeOp + juce::String(op);
        juce::String namePrefix = code + opLabel + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMul, namePrefix + opPostMulLabel, 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt, namePrefix + opPostDtLabel, 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, namePrefix + opPostArLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postDr, namePrefix + opPostDrLabel, 0.0f, 5.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSr, namePrefix + opPostSrLabel, 0.0f, 10.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSl, namePrefix + opPostSlLabel, 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postTl, namePrefix + opPostTlLabel, 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postKs, namePrefix + opPostKsLabel, 0, 3, 0)); // KS (0 to 3)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postFix, namePrefix + opPostFixLabel, false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postFixFreq, namePrefix + opPostFixFreqLabel, juce::NormalisableRange<float>(0.0f, 8000.0f, 0.0f, 0.3f), 440.0f));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, false)); // OP Mask (Switch)
    }
}

void AudioPlugin2686V::createOplParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPL (YM3526) Parameters
    // ==========================================
    juce::String code = codeOpl;

    layout.add(std::make_unique<juce::AudioParameterInt>(code + postAlg, code + mPostAlgTitle, 0, 1, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb0, code + mPostFb0Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postBit, code + mPostBitTitle, 0, 4, 4));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postRate, code + mPostRateTitle, 1, 7, 2)); // Default 6 (16kHz)

    for (int op = 0; op < 2; ++op)
    {
        juce::String prefix = code + codeOp + juce::String(op);
        juce::String namePrefix = code + opLabel + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMul, namePrefix + opPostMulLabel, 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt, namePrefix + opPostDtLabel, 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, namePrefix + opPostArLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postDr, namePrefix + opPostDrLabel, 0.0f, 5.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSr, namePrefix + opPostSrLabel, 0.0f, 10.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSl, namePrefix + opPostSlLabel, 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postTl, namePrefix + opPostTlLabel, 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postAm, namePrefix + opPostAmLabel, false)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postVib, namePrefix + opPostVibLabel, false));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postEgType, namePrefix + opPostEgTypeLabel, true)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postKsr, namePrefix + opPostKsrLabel, false)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postKsl, namePrefix + opPostKslLabel, 0, 3, 0));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postEg, namePrefix + opPostEgLabel, 0, 3, 0));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, false)); // OP Mask (Switch)
    }
}

void AudioPlugin2686V::createOpl3ParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==============================================================================
    // OPL3 Parameters (4 Ops, 8 Waveforms)
    // ==============================================================================
    juce::String code = codeOpl3;

    layout.add(std::make_unique<juce::AudioParameterInt>(code + postAlg, code + mPostAlgTitle, 1, 8, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb0, code + mPostFb0Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb2, code + mPostFb2Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postBit, code + mPostBitTitle, 0, 4, 4));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postRate, code + mPostRateTitle, 1, 7, 2)); // Default 6 (16kHz)

    for (int i = 0; i < 4; ++i)
    {
        juce::String prefix = code + codeOp + juce::String(i);
        juce::String namePrefix = code + opLabel + juce::String(i + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMul, namePrefix + opPostMulLabel, 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postTl, namePrefix + opPostTlLabel, 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, namePrefix + opPostArLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postDr, namePrefix + opPostDrLabel, 0.0f, 5.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSr, namePrefix + opPostSrLabel, 0.0f, 10.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSl, namePrefix + opPostSlLabel, 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postAm, namePrefix + opPostAmLabel, false)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postVib, namePrefix + opPostVibLabel, false));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postEgType, namePrefix + opPostEgTypeLabel, true)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postKsr, namePrefix + opPostKsrLabel, false)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postKsl, namePrefix + opPostKslLabel, 0, 3, 0));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postEg, namePrefix + opPostEgLabel, 0, 3, 0));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, false)); // OP Mask (Switch)
    }
}

void AudioPlugin2686V::createOpmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPM (YM2151) Parameters
    // ==========================================
    juce::String code = codeOpm;

    layout.add(std::make_unique<juce::AudioParameterInt>(code + postAlg, code + mPostAlgTitle, 0, 7, 7));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb0, code + mPostFb0Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb2, code + mPostFb2Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postBit, code + mPostBitTitle, 0, 4, 4));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postRate, code + mPostRateTitle, 1, 7, 2)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postLFreq, code + mPostLfoFreq, 0.1f, 200.0f, 5.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postLPms, code + mPostLfoPms, 0, 7, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postLAms, code + mPostLfoAms, 0, 3, 0));

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = code + codeOp + juce::String(op);
        juce::String namePrefix = code + opLabel + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMul, namePrefix + opPostMulLabel, 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt, namePrefix + opPostDt1Label, 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt2, namePrefix + opPostDt2Label, 0, 3, 0)); // Coarse Detune (OPM Only)
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, namePrefix + opPostArLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postD1r, namePrefix + opPostD1rLabel, 0.0f, 5.0f, 0.1f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postD1l, namePrefix + opPostD1lLabel, 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postD2r, namePrefix + opPostD2rLabel, 0.0f, 10.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postSe, namePrefix + opPostSeLabel, 0, 15, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSeFreq, namePrefix + opPostSeFreqLabel, 0.1f, 20.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postTl, namePrefix + opPostTlLabel, 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postKs, namePrefix + opPostKsLabel, 0, 3, 0)); // KS (0 to 3)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postFix, namePrefix + opPostFixLabel, false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postFixFreq, namePrefix + opPostFixFreqLabel, juce::NormalisableRange<float>(0.0f, 8000.0f, 0.0f, 0.3f), 440.0f));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, false)); // OP Mask (Switch)
    }
}

void AudioPlugin2686V::createOpzx3ParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPZ + OPX + MA-3(SD-1) Parameters
    // ==========================================
    juce::String code = codeOpzx3;

    layout.add(std::make_unique<juce::AudioParameterInt>(code + postAlg, code + mPostAlgTitle, 0, 27, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb0, code + mPostFb0Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postFb2, code + mPostFb2Title, 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postBit, code + mPostBitTitle, 0, 4, 4));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postRate, code + mPostRateTitle, 1, 7, 2)); // Default 6 (16kHz)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postLFreq, code + mPostLfoFreq, 0.1f, 200.0f, 5.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postLPms, code + mPostLfoPms, 0, 7, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postLAms, code + mPostLfoAms, 0, 3, 0));

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = code + codeOp + juce::String(op);
        juce::String namePrefix = code + opLabel + juce::String(op + 1);

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMul, namePrefix + opPostMulLabel, 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt, namePrefix + opPostDt1Label, 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postDt2, namePrefix + opPostDt2Label, 0, 3, 0)); // Coarse Detune (OPM Only)
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postAr, namePrefix + opPostArLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postD1r, namePrefix + opPostD1rLabel, 0.0f, 5.0f, 0.1f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postD1l, namePrefix + opPostD1lLabel, 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postD2r, namePrefix + opPostD2rLabel, 0.0f, 10.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + opPostRrLabel, 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postSe, namePrefix + opPostSeLabel, 0, 15, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postSeFreq, namePrefix + opPostSeFreqLabel, 0.1f, 20.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postTl, namePrefix + opPostTlLabel, 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postKs, namePrefix + opPostKsLabel, 0, 3, 0)); // KS (0 to 3)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postFix, namePrefix + opPostFixLabel, false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postFixFreq, namePrefix + opPostFixFreqLabel, juce::NormalisableRange<float>(0.0f, 8000.0f, 0.0f, 0.3f), 440.0f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postWs, namePrefix + opPostWsLabel, 0, 28, 0));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postMask, namePrefix + opPostMaskLabel, false)); // OP Mask (Switch)
    }
}

void AudioPlugin2686V::createSsgParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // SSG Parameters
    // ==========================================
    juce::String code = codeSsg;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postLevel, code + " Tone", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postNoise, code + " Noise", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postNoiseFreq, code + " Noise Freq", juce::NormalisableRange<float>(50.0f, 30000.0f, 1.0f, 0.3f), 12000.0f)); // Noise Frequency Default: 12000Hz (Standard PSG feel)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + postNoiseOnNote, code + " Noise On Note", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postMix, code + " Tone/Noise Mix", 0.0f, 1.0f, 0.5f)); // デフォルトは 0.5 (Mix)

    layout.add(std::make_unique<juce::AudioParameterInt>(code + postWaveform, code + " Waveform", 0, 1, 0)); // 0:Pulse, 1:Triangle

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + codeAdsr + postBypass, code + " ADSR Bypass", false));

    layout.add(std::make_unique<juce::AudioParameterInt>(code + postDutyMode, code + " Duty Mode", 0, 1, 0)); // Duty Mode: 0=Preset, 1=Variable
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postDutyPreset, code + " Duty Preset", 0, 8, 0)); // Preset: 0~8
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postDutyVar, code + " Duty Var", 0.0f, 0.5f, 0.5f)); // Variable: 0.0 ~ 0.5
    layout.add(std::make_unique<juce::AudioParameterBool>(code + postDutyInv, code + " Duty Invert", false)); // Invert

    layout.add(std::make_unique<juce::AudioParameterBool>(code + postTriKeyTrk, code + " Triangle Key Track", true)); // Key Track Switch (Default ON)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postTriPeak, code + " Triangle Peak", 0.0f, 1.0f, 0.5f)); // Triangle Peak: 0.0 ~ 1.0
    // Triangle Manual Frequency
    // Range: 0.1Hz - 8000Hz, Skew: 0.3 (Log的挙動), Default: 440Hz
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postTriFreq, code + " Tri Manual Freq", juce::NormalisableRange<float>(0.1f, 8000.0f, 0.1f, 0.3f), 440.0f));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postBit, code + mPostBitTitle, 0, 4, 3));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postRate, code + mPostRateTitle, 1, 7, 6)); // Default 6 (16kHz)

    // Hardware Envelope Parameters
    layout.add(std::make_unique<juce::AudioParameterBool>(code + postEnvEnable, code + " HW Env Enable", false)); // HW Env Enable(Bool)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postEnvShape,  code + " Env Shape", 0, 7, 0)); // Shape: 0-7
    // 実機のPeriodは値が大きいほど遅いですが、スライダーは右に行くほど速い方が直感的なためHzにします
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postEnvPeriod, code + " Env Speed", 0.1f, 200.0f, 1.0f)); // Period: ここでは周波数(Hz)として扱います (0.1Hz ~ 200Hz)

    addEnvParameters(layout, code);
}

void AudioPlugin2686V::createWavetableParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    juce::String code = codeWt;

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postBit, code + mPostBitTitle, 0, 4, 3));
    // Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postRate, code + mPostRateTitle, 1, 7, 6)); // Default 6 (16kHz)
    // Size: 0:32, 1:64
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postSize, code + " Sample Size", 0, 1, 0));
    // Waveform Preset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postWave, code + " Waveform", 0, 8, 0));

    // Custom Waveform Sliders (32 params)
    // Range: -1.0 to 1.0, Default: 0.0
    for (int i = 0; i < 32; ++i)
    {
        auto paramId = code + codeCustom32 + juce::String(i);
        auto paramName = code + " Custom32 " + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, -1.0f, 1.0f, 0.0f));
    }

    for (int i = 0; i < 64; ++i)
    {
        auto paramId = code + codeCustom64 + juce::String(i);
        auto paramName = code + " Custom64 " + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, -1.0f, 1.0f, 0.0f));
    }

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>(code + postModEnable, code + " Mod Enable", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postModDepth, code + " Mod Depth", 0.0f, 1.0f, 0.2f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postModSpeed, code + " Mod Speed", 0.1f, 10.0f, 1.0f));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postLevel, code + " Level", 0.0f, 1.0f, 1.0f));

    addEnvParameters(layout, code);
}

void AudioPlugin2686V::createRhythmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // Rhythm Parameters
    // ==========================================
    juce::String code = codeRhythm;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postLevel, code + " Vol", 0.0f, 1.0f, 1.0f));

    // Create parameters for each of the 8 pads
    for (int i = 0; i < 8; ++i) {
        juce::String prefix = code + codePad + juce::String(i);
        juce::String namePrefix = code + " Pad" + juce::String(i + 1);
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postVolume, namePrefix + " Vol", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postPan, namePrefix + opPostPanLabel, 0.0f, 1.0f, 0.5f));

        // Note number
        int defNote = 60 + i;

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postNote, namePrefix + " Note", 0, 127, defNote));

        // 1: Raw 32bit 2: PCM 24bit 3: PCM 16bit 4: PCM 8bit 5: PCM 5bit 6: PCM 4bit 7: ADPCM 4bit
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postMode, namePrefix + " Quality", 1, 7, 7));

        // 1: 96kHz 2: 55.5kHz 3: 48kHz 4: 44.1kHz 5: 22.5kHz 6: 16k 7: 8k
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + postRate, namePrefix + mPostRateTitle, 1, 7, 6));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + postOneShot, namePrefix + " One Shot", true));

        // Release Parameter
        // 範囲: 0.03秒 ～ 5.0秒, 初期値: 0.1秒
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + postRr, namePrefix + " Release", 0.03f, 5.0f, 0.1f));
    }
}

void AudioPlugin2686V::createAdpcmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // ADPCM Parameters
    // ==========================================
    juce::String code = codeAdpcm;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postLevel, code + " Level", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + postPan, code + opPostPanLabel, 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + postLoop, code + " Loop", true));
    // 0:Raw, 1:24bit, 2:16bit, 3:8bit, 4:5bit, 5:4bit PCM 6:ADPCM(4bit)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postMode, code + " Quality", 0, 6, 0));
    // 0: 96kHz 1: 55.5kHz 2: 48kHz 3: 44.1kHz 4: 22.5kHz 5: 16k 6: 8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + postRate, code + mPostRateTitle, 0, 6, 3)); // Default: 3 (44.1kHz)

    addEnvParameters(layout, code);
}
#endif

void AudioPlugin2686V::createFxParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // FX Parameters
    // ==========================================
    juce::String code = codeFx;

    // Bypass
    layout.add(std::make_unique<juce::AudioParameterBool>(code + postBypass, code + masterBypassLabel, false));

    // --- Tremolo ---
    layout.add(std::make_unique<juce::AudioParameterBool>(code + codeFxTrm + postBypass, code + fxTremoloLabel + fxPostBypassLabel, false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxTrm + postRate, code + fxTremoloLabel + fxPostRateLabel, 0.1f, 20.0f, 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxTrm + postDepth, code + fxTremoloLabel + fxPostDepthLabel, 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxTrm + postMix, code + fxTremoloLabel + fxPostMixLabel, 0.0f, 1.0f, 0.0f));

    // --- Vibrato / Detune ---
    layout.add(std::make_unique<juce::AudioParameterBool>(code + codeFxVib + postBypass, code + fxVibratoLabel + fxPostBypassLabel, false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxVib + postRate, code + fxVibratoLabel + fxPostRateLabel, 0.1f, 10.0f, 2.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxVib + postDepth, code + fxVibratoLabel + fxPostDepthLabel, 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxVib + postMix, code + fxVibratoLabel + fxPostMixLabel, 0.0f, 1.0f, 0.0f));

    // --- Modern Bit Crusher ---
    layout.add(std::make_unique<juce::AudioParameterBool>(code + codeFxMbc + postBypass, code + fxMBCLabel + fxPostBypassLabel, false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxMbc + postRate, code + fxMBCLabel + fxPostRateLabel, 1.0f, 50.0f, 1.0f)); // Rate: 1(High) ～ 50(Low)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxMbc + postFxBit, code + fxMBCLabel + fxPostQualityLabel, 2.0f, 24.0f, 24.0f)); // Bits: 24(Clean) ～ 2(Noisy)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxMbc + postMix, code + fxMBCLabel + fxPostMixLabel, 0.0f, 1.0f, 0.0f));

    // --- Delay ---
    layout.add(std::make_unique<juce::AudioParameterBool>(code + codeFxDly + postBypass, code + fxDelayLabel + fxPostBypassLabel, false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxDly + postFxTime, code + fxDelayLabel + fxPostTimeLabel, 10.0f, 1000.0f, 375.0f)); // ms
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxDly + postFxFb, code + fxDelayLabel + fxPostFbLabel, 0.0f, 0.95f, 0.4f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxDly + postMix, code + fxDelayLabel + fxPostMixLabel, 0.0f, 1.0f, 0.0f));

    // --- Reverb ---
    layout.add(std::make_unique<juce::AudioParameterBool>(code + codeFxRvb + postBypass, code + fxReverbLabel + fxPostBypassLabel, false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxRvb + postFxSize, code + fxReverbLabel + fxPostSizeLabel, 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxRvb + postFxDamp, code + fxReverbLabel + fxPostDampLabel, 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxRvb + postMix, code + fxReverbLabel + fxPostMixLabel, 0.0f, 1.0f, 0.0f));

    // --- Retro Bit Crusher ---
    layout.add(std::make_unique<juce::AudioParameterBool>(code + codeFxRbc + postBypass, code + fxRBCLabel + fxPostBypassLabel, false));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + codeFxRbc + postRate, code + fxRBCLabel + fxPostRateLabel, 1, 7, 6));    // Default: 16kHz
    layout.add(std::make_unique<juce::AudioParameterInt>(code + codeFxRbc + postFxBit, code + fxRBCLabel + fxPostQualityLabel, 1, 7, 7)); // Default: ADPCM
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + codeFxRbc + postMix, code + fxRBCLabel + fxPostMixLabel, 0.0f, 1.0f, 0.0f));
}

#if !defined(BUILD_AS_FX_PLUGIN)
void AudioPlugin2686V::processOpnaBlock(SynthParams &params)
{
    juce::String code = codeOpna;

    params.algorithm = (int)*apvts.getRawParameterValue(code + postAlg);
    params.feedback = *apvts.getRawParameterValue(code + postFb0);
    params.feedback2 = *apvts.getRawParameterValue(code + postFb2);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + postBit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + postRate);
    params.lfoFreq = *apvts.getRawParameterValue(code + postLFreq);
    params.pms = (int)*apvts.getRawParameterValue(code + postLPms);
    params.ams = (int)*apvts.getRawParameterValue(code + postLAms);

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = code + codeOp + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + postMul);
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + postDt);
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + postAr);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + postDr);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + postSl);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + postRr);
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + postKs);
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + postTl);
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + postSr);
        params.fmOp[op].ssgEg = (int)*apvts.getRawParameterValue(p + postSe);
        params.fmOp[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + postSeFreq);
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + postFix) > 0.5f);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + postFixFreq);
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = (*apvts.getRawParameterValue(p + postAm) > 0.5f);
        params.fmOp[op].vibEnable = true;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > 0.5f);
    }
}

void AudioPlugin2686V::processOpnBlock(SynthParams &params)
{
    juce::String code = codeOpn;

    params.algorithm = (int)*apvts.getRawParameterValue(code + postAlg);
    params.feedback = *apvts.getRawParameterValue(code + postFb0);
    params.feedback2 = *apvts.getRawParameterValue(code + postFb2);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + postBit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + postRate);

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = code + codeOp + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + postMul);
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + postDt);
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + postAr);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + postDr);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + postSl);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + postRr);
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + postKs);
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + postTl);
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + postSr);
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fmSsgEgFreq = 0.0f;
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + postFix) > 0.5f);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + postFixFreq);
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = false; // OPNにはAMは無い
        params.fmOp[op].vibEnable = false;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > 0.5f);
    }
}

void AudioPlugin2686V::processOplBlock(SynthParams &params)
{
    juce::String code = codeOpl;

    params.algorithm = (int)*apvts.getRawParameterValue(code + postAlg);
    params.feedback = *apvts.getRawParameterValue(code + postFb0);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + postBit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + postRate);

    for (int op = 0; op < 2; ++op)
    {
        juce::String p = code + codeOp + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + postMul);
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + postDt);
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + postAr);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + postDr);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + postSl);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + postRr);
        bool ksrOn = *apvts.getRawParameterValue(p + postKsr) > 0.5f;
        params.fmOp[op].keyScale = ksrOn ? 3 : 0;
        params.fmOp[op].keyScaleLevel = (int)*apvts.getRawParameterValue(p + postKsl);
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + postTl);
        params.fmOp[op].sustainRate = 0.0f;
        params.fmOp[op].ssgEg = 0; // OPLにはSSG-EGは無い
        params.fmOp[op].fmSsgEgFreq = 0.0f;
        params.fmOp[op].fixedMode = false; // OPLにはFixed Modeは無い
        params.fmOp[op].fixedFreq = 0.0f;
        params.fmOp[op].waveSelect = (int)*apvts.getRawParameterValue(p + postEg);
        params.fmOp[op].amEnable = (*apvts.getRawParameterValue(p + postAm) > 0.5f);
        params.fmOp[op].vibEnable = *apvts.getRawParameterValue(p + postVib) > 0.5f;
        params.fmOp[op].egType = *apvts.getRawParameterValue(p + postEgType) > 0.5f;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > 0.5f);
    }
}

void AudioPlugin2686V::processOpl3Block(SynthParams& params)
{
    juce::String code = codeOpl3;

    params.algorithm = (int)*apvts.getRawParameterValue(code + postAlg);
    params.feedback = *apvts.getRawParameterValue(code + postFb0);
    params.feedback2 = *apvts.getRawParameterValue(code + postFb2);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + postBit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + postRate);

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = code + codeOp + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + postMul);
        params.fmOp[op].detune = 0;
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + postAr);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + postDr);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + postSl);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + postRr);
        bool ksrOn = *apvts.getRawParameterValue(p + postKsr) > 0.5f;
        params.fmOp[op].keyScale = ksrOn ? 3 : 0;
        params.fmOp[op].keyScaleLevel = (int)*apvts.getRawParameterValue(p + postKsl);
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + postTl);
        params.fmOp[op].sustainRate = 0.0f;
        params.fmOp[op].ssgEg = 0; // OPLにはSSG-EGは無い
        params.fmOp[op].fmSsgEgFreq = 0.0f;
        params.fmOp[op].fixedMode = false; // OPLにはFixed Modeは無い
        params.fmOp[op].fixedFreq = 0.0f;
        params.fmOp[op].waveSelect = (int)*apvts.getRawParameterValue(p + postEg);
        params.fmOp[op].amEnable = (*apvts.getRawParameterValue(p + postAm) > 0.5f);
        params.fmOp[op].vibEnable = *apvts.getRawParameterValue(p + postVib) > 0.5f;
        params.fmOp[op].egType = *apvts.getRawParameterValue(p + postEgType) > 0.5f;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > 0.5f);
    }
}

void AudioPlugin2686V::processOpmBlock(SynthParams& params)
{
    juce::String code = codeOpm;

    params.algorithm = (int)*apvts.getRawParameterValue(code + postAlg);
    params.feedback = *apvts.getRawParameterValue(code + postFb0);
    params.feedback2 = *apvts.getRawParameterValue(code + postFb2);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + postBit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + postRate);
    params.lfoFreq = *apvts.getRawParameterValue(code + postLFreq);
    params.pms = (int)*apvts.getRawParameterValue(code + postLPms);
    params.ams = (int)*apvts.getRawParameterValue(code + postLAms);

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = code + codeOp + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + postMul);
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + postDt);
        params.fmOp[op].detune2 = (int)*apvts.getRawParameterValue(p + postDt2);
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + postAr);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + postD1r);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + postD1l);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + postRr);
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + postKs);
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + postTl);
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + postD2r);
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + postSeFreq);
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + postFix) > 0.5f);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + postFixFreq);
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = false;
        params.fmOp[op].vibEnable = true;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > 0.5f);
    }
}

void AudioPlugin2686V::processOpzx3Block(SynthParams& params)
{
    juce::String code = codeOpzx3;

    params.algorithm = (int)*apvts.getRawParameterValue(code + postAlg);
    params.feedback = *apvts.getRawParameterValue(code + postFb0);
    params.feedback2 = *apvts.getRawParameterValue(code + postFb2);
    params.fmBitDepth = (int)*apvts.getRawParameterValue(code + postBit);
    params.fmRateIndex = (int)*apvts.getRawParameterValue(code + postRate);
    params.lfoFreq = *apvts.getRawParameterValue(code + postLFreq);
    params.pms = (int)*apvts.getRawParameterValue(code + postLPms);
    params.ams = (int)*apvts.getRawParameterValue(code + postLAms);

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = code + codeOp + juce::String(op);

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + postMul);
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + postDt);
        params.fmOp[op].detune2 = (int)*apvts.getRawParameterValue(p + postDt2);
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + postAr);
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + postD1r);
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + postD1l);
        params.fmOp[op].release = *apvts.getRawParameterValue(p + postRr);
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + postKs);
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + postTl);
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + postD2r);
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + postSeFreq);
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + postFix) > 0.5f);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + postFixFreq);
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = false;
        params.fmOp[op].vibEnable = true;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + postMask) > 0.5f);
    }
}

void AudioPlugin2686V::processSsgBlock(SynthParams &params)
{
    juce::String code = codeSsg;

    // --- SSG Parameters ---
    params.ssgLevel = *apvts.getRawParameterValue(code + postLevel);
    params.ssgNoiseLevel = *apvts.getRawParameterValue(code + postNoise);
    params.ssgNoiseFreq = *apvts.getRawParameterValue(code + postNoiseFreq);
    params.ssgNoiseOnNote = (*apvts.getRawParameterValue(code + postNoiseOnNote) > 0.5f);
    params.ssgMix = *apvts.getRawParameterValue(code + postMix);
    params.ssgWaveform = (int)*apvts.getRawParameterValue(code + postWaveform);
    params.ssgBitDepth = (int)*apvts.getRawParameterValue(code + postBit);
    params.ssgRateIndex = (int)*apvts.getRawParameterValue(code + postRate);

    params.ssgUseHwEnv = (*apvts.getRawParameterValue(code + postEnvEnable) > 0.5f);
    params.ssgEnvShape = (int)*apvts.getRawParameterValue(code + postEnvShape);
    params.ssgEnvPeriod = *apvts.getRawParameterValue(code + postEnvPeriod);

    params.ssgDutyMode = (int)*apvts.getRawParameterValue(code + postDutyMode);
    params.ssgDutyPreset = (int)*apvts.getRawParameterValue(code + postDutyPreset);
    params.ssgDutyVar = *apvts.getRawParameterValue(code + postDutyVar);
    params.ssgDutyInvert = (*apvts.getRawParameterValue(code + postDutyInv) > 0.5f);

    params.ssgTriKeyTrack = (*apvts.getRawParameterValue(code + postTriKeyTrk) > 0.5f);
    params.ssgTriPeak = *apvts.getRawParameterValue(code + postTriPeak);
    params.ssgTriFreq = *apvts.getRawParameterValue(code + postTriFreq);

    params.ssgAdsrBypass = (*apvts.getRawParameterValue(code + codeAdsr + postBypass) > 0.5f);
    params.ssgAdsr.a = *apvts.getRawParameterValue(code + postAr);
    params.ssgAdsr.d = *apvts.getRawParameterValue(code + postDr);
    params.ssgAdsr.s = *apvts.getRawParameterValue(code + postSl);
    params.ssgAdsr.r = *apvts.getRawParameterValue(code + postRr);
}

void AudioPlugin2686V::processWavetableBlock(SynthParams &params)
{
    juce::String code = codeWt;

    params.wtBitDepth = (int)*apvts.getRawParameterValue(code + postBit);
    params.wtRateIndex = (int)*apvts.getRawParameterValue(code + postRate);
    params.wtTableSize = (int)*apvts.getRawParameterValue(code + postSize);
    params.wtWaveform = (int)*apvts.getRawParameterValue(code + postWave);

    // Custom Wave params reading
    for (int i = 0; i < 32; ++i)
    {
        auto paramId = code + codeCustom32 + juce::String(i);
        params.wtCustomWave32[i] = *apvts.getRawParameterValue(paramId);
    }


    // Custom Wave params reading
    for (int i = 0; i < 64; ++i)
    {
        auto paramId = code + codeCustom64 + juce::String(i);
        params.wtCustomWave64[i] = *apvts.getRawParameterValue(paramId);
    }

    params.wtModEnable = (*apvts.getRawParameterValue(code + postModEnable) > 0.5f);
    params.wtModDepth = *apvts.getRawParameterValue(code + postModDepth);
    params.wtModSpeed = *apvts.getRawParameterValue(code + postModSpeed);
    params.wtLevel = *apvts.getRawParameterValue(code + postLevel);

    params.wtAdsr.a = *apvts.getRawParameterValue(code + postAr);
    params.wtAdsr.d = *apvts.getRawParameterValue(code + postDr);
    params.wtAdsr.s = *apvts.getRawParameterValue(code + postSl);
    params.wtAdsr.r = *apvts.getRawParameterValue(code + postRr);
}

void AudioPlugin2686V::processRhythmBlock(SynthParams &params)
{
    juce::String code = codeRhythm;

    // --- Rhythm Parameters ---
    params.rhythmLevel = *apvts.getRawParameterValue(code + postLevel);

    for (int i = 0; i < 8; ++i) {
        juce::String prefix = code + codePad + juce::String(i);
        auto& pad = params.rhythmPads[i];
        pad.level = *apvts.getRawParameterValue(prefix + postVolume);
        pad.pan = *apvts.getRawParameterValue(prefix + postPan);
        pad.noteNumber = (int)*apvts.getRawParameterValue(prefix + postNote);
        pad.qualityMode = (int)*apvts.getRawParameterValue(prefix + postMode);
        pad.rateIndex = (int)*apvts.getRawParameterValue(prefix + postRate);
        pad.isOneShot = (bool)*apvts.getRawParameterValue(prefix + postOneShot);
        pad.release = *apvts.getRawParameterValue(prefix + postRr);
    }
}

void AudioPlugin2686V::processAdpcmBlock(SynthParams &params)
{
    juce::String code = codeAdpcm;

    // get Bool value
    // getRawParameterValue は float* を返すので、0.5f以上かどうかで判定するのが通例
    params.adpcmLevel = *apvts.getRawParameterValue(code + postLevel);
    params.adpcmPan = *apvts.getRawParameterValue(code + postPan);
    params.adpcmLoop = (*apvts.getRawParameterValue(code + postLoop) > 0.5f);
    params.adpcmQualityMode = (int)*apvts.getRawParameterValue(code + postMode);
    params.adpcmRateIndex = (int)*apvts.getRawParameterValue(code + postRate);

    params.adpcmAdsr.a = *apvts.getRawParameterValue(code + postAr);
    params.adpcmAdsr.d = *apvts.getRawParameterValue(code + postDr);
    params.adpcmAdsr.s = *apvts.getRawParameterValue(code + postSl);
    params.adpcmAdsr.r = *apvts.getRawParameterValue(code + postRr);
}
#endif

void AudioPlugin2686V::processFxBlock(juce::AudioBuffer<float>& buffer, SynthParams& params)
{
    juce::String code = codeFx;

    if (*apvts.getRawParameterValue(code + postBypass) > 0.5f)
    {
        return;
    }

    // Vibrato
    bool vB = *apvts.getRawParameterValue(code + codeFxVib + postBypass) > 0.5f;
    float vRate = *apvts.getRawParameterValue(code + codeFxVib + postRate);
    float vDepth = *apvts.getRawParameterValue(code + codeFxVib + postDepth);
    float vMix = *apvts.getRawParameterValue(code + codeFxVib + postMix);
    effects.setVibratoParams(vRate, vDepth, vMix);

    // Tremolo
    bool tB = *apvts.getRawParameterValue(code + codeFxTrm + postBypass) > 0.5f;
    float tRate = *apvts.getRawParameterValue(code + codeFxTrm + postRate);
    float tDepth = *apvts.getRawParameterValue(code + codeFxTrm + postDepth);
    float tMix = *apvts.getRawParameterValue(code + codeFxTrm + postMix);
    effects.setTremoloParams(tRate, tDepth, tMix);

    // Modern Bit Crusher
    bool mcB = *apvts.getRawParameterValue(code + codeFxMbc + postBypass) > 0.5f;
    float mbcRate = *apvts.getRawParameterValue(code + codeFxMbc + postRate);
    float mbcBits = *apvts.getRawParameterValue(code + codeFxMbc + postFxBit);
    float mbcMix = *apvts.getRawParameterValue(code + codeFxMbc + postMix);
    effects.setModernBitCrusherParams(mbcRate, mbcBits, mbcMix);

    // Delay
    bool dB = *apvts.getRawParameterValue(code + codeFxDly + postBypass) > 0.5f;
    float dTime = *apvts.getRawParameterValue(code + codeFxDly + postFxTime);
    float dFb = *apvts.getRawParameterValue(code + codeFxDly + postFxFb);
    float dMix = *apvts.getRawParameterValue(code + codeFxDly + postMix);
    effects.setDelayParams(dTime, dFb, dMix);

    // Reverb
    bool rB = *apvts.getRawParameterValue(code + codeFxRvb + postBypass) > 0.5f;
    float rSize = *apvts.getRawParameterValue(code + codeFxRvb + postFxSize);
    float rDamp = *apvts.getRawParameterValue(code + codeFxRvb + postFxDamp);
    float rMix = *apvts.getRawParameterValue(code + codeFxRvb + postMix);

    // Retro Bit Crusher
    bool rcB = *apvts.getRawParameterValue(code + codeFxRbc + postBypass) > 0.5f;
    int rbcRate = (int)*apvts.getRawParameterValue(code + codeFxRbc + postRate);
    int rbcBits = (int)*apvts.getRawParameterValue(code + codeFxRbc + postFxBit);
    float rbcMix = *apvts.getRawParameterValue(code + codeFxRbc + postMix);
    effects.setRetroBitCrusherParams(rbcRate, rbcBits, rbcMix);

    effects.setReverbParams(rSize, rDamp, 1.0f, rMix); // Width=1.0固定

    // バイパス設定
    effects.setBypasses(tB, vB, mcB, rcB, dB, rB);

    // エフェクト処理実行
    effects.process(buffer);
}

// 環境設定を保存
void AudioPlugin2686V::saveEnvironment(const juce::File& file)
{
    juce::XmlElement xml(envCode);

    xml.setAttribute(settingWallpaperPath, wallpaperPath);
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
        lastSampleDirectory = newSampleDir;
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
#endif
