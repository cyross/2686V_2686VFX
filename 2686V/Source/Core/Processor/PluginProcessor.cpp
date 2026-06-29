#include "PluginProcessor.h"

#include "../Processor/ProcessorNames.h"
#include "../../Gui/Settings/SettingsKeys.h"
#include "../../Gui/Settings/SettingsValues.h"

#include "../Gui/GuiValues.h"

// ============================================================================
// Constructor
// ============================================================================
AudioPlugin2686V::AudioPlugin2686V()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
        // シンセモード: 入力なし、ステレオ出力あり
        .withOutput(Global::Audio::output, juce::AudioChannelSet::stereo(), true)
    ),
    // Initialize APVTS (Parameters are created here)
    apvts(*this, &undoManager, Global::Plugin::parameters, createParameterLayout()) // APVTSの初期化
#endif
{
    prMap[OscMode::OPNA] = &prOpna;
    prMap[OscMode::OPN] = &prOpn;
    prMap[OscMode::OPL] = &prOpl;
    prMap[OscMode::OPL3] = &prOpl3;
    prMap[OscMode::OPM] = &prOpm;
    prMap[OscMode::OPZX7] = &prOpzx7;
    prMap[OscMode::SSG] = &prSsg;
    prMap[OscMode::WAVETABLE] = &prWt;
    prMap[OscMode::WT2] = &prWt2;
    prMap[OscMode::RHYTHM] = &prRhythm;
    prMap[OscMode::ADPCM] = &prAdpcm;
    prMap[OscMode::BEEP] = &prBeep;

    pMode = apvts.getRawParameterValue(CorePrKey::mode);
    pMonoMode = apvts.getRawParameterValue(CorePrKey::monoMode);
    pUseVelocity = apvts.getRawParameterValue(CorePrKey::useVelocity);
    pPitchResetOnLegato = apvts.getRawParameterValue(CorePrKey::pitchResetOnLegato);
    pFixedVelocity = apvts.getRawParameterValue(CorePrKey::fixedVelocity);

    prOpna.init(apvts);
    prOpn.init(apvts);
    prOpl.init(apvts);
    prOpl3.init(apvts);
    prOpm.init(apvts);
    prOpzx7.init(apvts);
    prSsg.init(apvts);
    prWt.init(apvts);
    prWt2.init(apvts);
    prRhythm.init(apvts);
    prAdpcm.init(apvts);
    prBeep.init(apvts);
    prFx.init(apvts);
    prCurve.init(apvts);

    m_synth.addSound(new SynthSound());
    for (int i = 0; i < Global::totalVoices; i++) {
        auto voice = new SynthVoice();

        voice->prepare(44100.0);
        voice->setCurveCore(&m_curveCore);
        m_synth.addVoice(voice);
    }

    prFx.prepare(44100.0);

    m_curveCore.bakeCurves();

    previewSynth.addSound(new SynthSound());

    auto prevVoice = new SynthVoice();

    prevVoice->setCurveCore(&m_curveCore);
    prevVoice->prepare(44100.0);
    previewSynth.addVoice(prevVoice);

	previewFx.init(apvts);
    previewFx.prepare(44100.0);

    formatManager.registerBasicFormats();
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

    // Mode: 0:OPNA, 1:OPN, 2:OPL, 3:OPLL, 4:OPL3, 5:OPM, 6: OPZX7 7:SSG, 8:WAVETABLE 9:WT2 10:RHYTHM, 11:ADPCM. 12:FX, 13:PRESET, 14:SETTING, 15:ABOUT
    layout.add(std::make_unique<juce::AudioParameterInt>(CorePrKey::mode, CorePrName::mode, 0, CoreGuiValue::TabNumber, 0));

    prOpna.createLayout(layout);
	prOpn.createLayout(layout);
	prOpl.createLayout(layout);
	prOpl3.createLayout(layout);
	prOpm.createLayout(layout);
	prOpzx7.createLayout(layout);
	prSsg.createLayout(layout);
	prWt.createLayout(layout);
    prWt2.createLayout(layout);
    prRhythm.createLayout(layout);
	prAdpcm.createLayout(layout);
    prBeep.createLayout(layout);
	prFx.createLayout(layout);
	prCurve.createLayout(layout);

    layout.add(std::make_unique<juce::AudioParameterBool>(
        CorePrKey::monoMode,
        CorePrName::monoMode,
        CorePrValue::MonoMode::initial
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        CorePrKey::useVelocity,
        CorePrName::useVelocity,
        CorePrValue::UseVelocity::initial
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        CorePrKey::pitchResetOnLegato,
        CorePrName::pitchResetOnLegato,
        CorePrValue::PitchResetOnLegato::initial
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        CorePrKey::fixedVelocity,
        CorePrName::fixedVelocity,
        CorePrValue::FixedVelocity::min,
        CorePrValue::FixedVelocity::max,
        CorePrValue::FixedVelocity::initial
    ));

    return layout;
}

// ============================================================================
// Prepare To Play
// ============================================================================
void AudioPlugin2686V::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    m_synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = static_cast<SynthVoice*>(m_synth.getVoice(i))) {
            voice->prepare(sampleRate);
        }
    }

    previewSynth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < previewSynth.getNumVoices(); ++i) {
        if (auto* voice = static_cast<SynthVoice*>(m_synth.getVoice(i))) {
            voice->prepare(sampleRate);
        }
    }

    prFx.prepare(sampleRate);
    previewFx.prepare(sampleRate);
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

    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    m_synth.currentParams = &m_currentParams;

    // 【シンセモード】
    // 入力バッファはノイズの原因になるのでクリアする
    buffer.clear();

    // --- Global ---
    int m = (int)pMode->load(std::memory_order_relaxed);
    m_currentParams.mode = (OscMode)m; // 0, 1, 2(RHYTHM)

    prMap[m_currentParams.mode]->processBlock(m_currentParams, apvts);

	// エンベロープカーブの処理は、シンセモードに関わらず常に行う
	prCurve.processBlock(m_currentParams, apvts);

    bool isMono = (pMonoMode->load(std::memory_order_relaxed) > 0.5f);

    m_synth.isMonoMode = isMono;
    m_currentParams.monoMode = isMono;

    bool useVelo = (pUseVelocity->load(std::memory_order_relaxed) > 0.5f);

    m_synth.useVelocity = useVelo;
    m_currentParams.useVelocity = useVelo;

    bool ptResetOnLegato = (pPitchResetOnLegato->load(std::memory_order_relaxed) > 0.5f);

    m_synth.pitchResetOnLegato = ptResetOnLegato;
    m_currentParams.pitchResetOnLegato = ptResetOnLegato;

    float fixedVelocity = pFixedVelocity->load(std::memory_order_relaxed);

    m_synth.fixedVelocity = fixedVelocity;
    m_currentParams.fixedVelocity = fixedVelocity;

    // Apply to each voice
    for (int i = 0; i < m_synth.getNumVoices(); ++i)
    {
        if (auto* voice = static_cast<SynthVoice*>(m_synth.getVoice(i)))
        {
            voice->setParameters(m_currentParams);
        }
    }

    m_curveCore.setParameters(m_currentParams.curve);

    // シンセの発音
    m_synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // ヘッドルーム適応
    if (useHeadroom)
    {
        buffer.applyGain(headroomGain);
    }

	prFx.processBlock(buffer, m_currentParams, apvts);

    if (previewVisiblity)
    {
        auto* finalOutL = buffer.getReadPointer(0);
        auto* finalOutR = buffer.getReadPointer(1);
        int numSamples = buffer.getNumSamples();

        // 現在の書き込み位置を取得
        int pos = realTimeWritePos.load(std::memory_order_relaxed);

        for (int i = 0; i < numSamples; ++i) {
            float l = finalOutL[i];
            float r = finalOutR[i];

            // リングバッファに書き込み
            realTimeBufferL[pos] = l;
            realTimeBufferR[pos] = r;
            realTimeBufferMono[pos] = (l + r) * 0.5f;

            // 位置を進め、バッファ終端に来たら0に戻す
            pos++;

            if (pos >= ringBufferSize) pos = 0;
        }

        // 最新の書き込み位置を保存 (GUI側がここを読み取る)
        realTimeWritePos.store(pos, std::memory_order_release);
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
            if (auto* voice = static_cast<SynthVoice*>(m_synth.getVoice(i)))
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
        if (padIndex >= 0 && padIndex < RhythmPrValue::pads) {
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
            if (auto* voice = static_cast<SynthVoice*>(m_synth.getVoice(i))) {
                voice->getRhythmCore()->setSampleData(padIndex, sourceData, reader->sampleRate);
            }
        }
    }
}

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

void AudioPlugin2686V::setPresetToXml(std::unique_ptr<juce::XmlElement>& xml)
{
    // セーブ時にAPVTSから現在のModeを確実に取得して同期させる
    int currentMode = (int)pMode->load(std::memory_order_relaxed);

    if (currentMode >= 0 && currentMode <= (int)OscMode::BEEP) {
        lastActiveSynthMode = (OscMode)currentMode;
    }

    // メタデータとパスを属性として追加
    xml->setAttribute(PresetKey::name, sanitizeString(presetName, PresetValue::MetaData::Length::name));
    xml->setAttribute(PresetKey::author, sanitizeString(presetAuthor, PresetValue::MetaData::Length::author));
    xml->setAttribute(PresetKey::version, sanitizeString(presetVersion, PresetValue::MetaData::Length::version));
    xml->setAttribute(PresetKey::comment, sanitizeString(presetComment, PresetValue::MetaData::Length::comment));
    xml->setAttribute(PresetKey::genre, sanitizeString(presetGenre, PresetValue::MetaData::Length::genre));
    xml->setAttribute(PresetKey::mode, getModeName(lastActiveSynthMode));
    xml->setAttribute(PresetKey::puginVersion, Global::Plugin::version);

    // サンプルパス保存 (ADPCM)
    xml->setAttribute(PresetKey::adpcmPath, makePathRelative(juce::File(adpcmFilePath)));

    // サンプルパス保存 (RHYTHM)
    for (int i = 0; i < RhythmPrValue::pads; ++i) {
        xml->setAttribute(PresetKey::rhythmPathPrefix + juce::String(i), makePathRelative(juce::File(rhythmFilePaths[i])));
    }

    // サンプルパス保存 (OPZX7 PCM/WT/WT2)
    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        xml->setAttribute(PresetKey::opzx7PathPrefix + juce::String(i), makePathRelative(juce::File(opzx7PcmFilePaths[i])));
        xml->setAttribute(PresetKey::opzx7WtPathPrefix + juce::String(i), makeWtPathRelative(juce::File(opzx7WtFilePaths[i])));
        xml->setAttribute(PresetKey::opzx7Wt2PathPrefix + juce::String(i), makeWtPathRelative(juce::File(opzx7Wt2FilePaths[i])));
    }

    // FXルーティング
    juce::StringArray sa;
    for (int fxId : prFx.getOrder())
        sa.add(juce::String(fxId));

    xml->setAttribute(SettingsKey::fxOrder, sa.joinIntoString(" "));
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
        presetGenre = xmlState->getStringAttribute(PresetKey::genre, PresetValue::MetaData::Initial::genre);
        presetPluginVersion = xmlState->getStringAttribute(PresetKey::puginVersion, Global::Plugin::version);

        // サンプル復帰 (ADPCM)
        juce::String storedAdpcm = xmlState->getStringAttribute(PresetKey::adpcmPath);
        juce::File adpcmFile = resolvePath(storedAdpcm);
        if (adpcmFile.existsAsFile()) {
            loadAdpcmFile(adpcmFile);
        }

        // サンプル復帰 (RHYTHM)
        for (int i = 0; i < RhythmPrValue::pads; ++i) {
            juce::String storedRhy = xmlState->getStringAttribute(PresetKey::rhythmPathPrefix + juce::String(i));
            juce::File rhyFile = resolvePath(storedRhy);
            if (rhyFile.existsAsFile()) {
                loadRhythmFile(rhyFile, i);
            }
        }

        // サンプル復帰 (OPZX7)
        for (int i = 0; i < Opzx7PrValue::ops; ++i) {
            juce::String storedPcmPath = xmlState->getStringAttribute(PresetKey::opzx7PathPrefix + juce::String(i));
            juce::File pcmFile = resolvePath(storedPcmPath);
            if (pcmFile.existsAsFile()) {
                loadOpzx7PcmFile(i, pcmFile);
            }

            juce::String storedWtPath = xmlState->getStringAttribute(PresetKey::opzx7WtPathPrefix + juce::String(i));
            juce::File wtFile = resolveWtPath(storedWtPath);
            if (wtFile.existsAsFile()) {
                loadOpzx7WtFile(i, wtFile);
            }

            juce::String storedWt2Path = xmlState->getStringAttribute(PresetKey::opzx7Wt2PathPrefix + juce::String(i));
            juce::File wt2File = resolveWtPath(storedWt2Path);
            if (wt2File.existsAsFile()) {
                loadOpzx7Wt2File(i, wt2File);
            }
        }

        // FXルーティング
        juce::String fxOrderStr = xmlState->getStringAttribute(SettingsKey::fxOrder);

        // 2. スペースで分割して StringArray に展開
        juce::StringArray sa;
        sa.addTokens(fxOrderStr, " ", "");

        // 3. int 配列に復元
        std::vector<int> loadedFxOrder;
        for (const auto& token : sa)
        {
            loadedFxOrder.push_back(token.getIntValue());
        }

        int loadedSize = loadedFxOrder.size();
        int effectSize = prFx.getEffectsNumber();

        // プリセットのエフェクト数とプラグイン内のエフェクト数にズレがあるときは、残りを埋める
        if (loadedSize < effectSize) {
            for (int i = loadedSize; i < effectSize; i++) {
                loadedFxOrder.push_back(i);
            }
        }

        prFx.updateOrder(loadedFxOrder);
    }
};

// ============================================================================
// State Information
// ============================================================================
void AudioPlugin2686V::getStateInformation(juce::MemoryBlock& destData) {
    // APVTSの状態をXMLとして取得
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    setPresetToXml(xml);

    copyXmlToBinary(*xml, destData);
}

void AudioPlugin2686V::setStateInformation(const void* data, int sizeInBytes) {
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
}

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

// 環境設定を保存
void AudioPlugin2686V::saveEnvironment(const juce::File& file)
{
    juce::XmlElement xml(SettingsKey::envCode);

    xml.setAttribute(SettingsKey::uiScaleIndex, uiScaleIndex);
    xml.setAttribute(SettingsKey::wallpaperPath, wallpaperPath);
    xml.setAttribute(SettingsKey::wallpaperMode, wallpaperMode);
    xml.setAttribute(SettingsKey::defaultSampleDir, defaultSampleDir);
    xml.setAttribute(SettingsKey::defaultPresetDir, defaultPresetDir);
    xml.setAttribute(SettingsKey::defaultWavetableDir, defaultWavetableDir);
    xml.setAttribute(SettingsKey::defaultFxOrderDir, defaultFxOrderDir);
    xml.setAttribute(SettingsKey::defaultFxParamDir, defaultFxParamDir);
    xml.setAttribute(SettingsKey::defaultLfoParamDir, defaultLfoParamDir);
    xml.setAttribute(SettingsKey::defaultAmpEnvParamDir, defaultAmpEnvParamDir);
    xml.setAttribute(SettingsKey::defaultPitchEnvParamDir, defaultPitchEnvParamDir);
    xml.setAttribute(SettingsKey::defaultSsgSwEnvParamDir, defaultSsgSwEnvParamDir);
    xml.setAttribute(SettingsKey::defaultDetuneParamDir, defaultDetuneParamDir);
    xml.setAttribute(SettingsKey::defaultUnisonParamDir, defaultUnisonParamDir);
    xml.setAttribute(SettingsKey::defaultQualityParamDir, defaultQualityParamDir);
    xml.setAttribute(SettingsKey::defaultPcmPlayParamDir, defaultPcmPlayParamDir);
    xml.setAttribute(SettingsKey::defaultToneNoiseParamDir, defaultToneNoiseParamDir);
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
        uiScaleIndex = xml->getIntAttribute(SettingsKey::uiScaleIndex, 7);
        wallpaperPath = xml->getStringAttribute(SettingsKey::wallpaperPath);
        wallpaperMode = xml->getIntAttribute(SettingsKey::wallpaperMode);
        defaultSampleDir = xml->getStringAttribute(SettingsKey::defaultSampleDir);
        defaultPresetDir = xml->getStringAttribute(SettingsKey::defaultPresetDir);
		defaultWavetableDir = xml->getStringAttribute(SettingsKey::defaultWavetableDir);
        defaultFxOrderDir = xml->getStringAttribute(SettingsKey::defaultFxOrderDir);
        defaultFxParamDir = xml->getStringAttribute(SettingsKey::defaultFxParamDir);
        defaultLfoParamDir = xml->getStringAttribute(SettingsKey::defaultLfoParamDir);
        defaultAmpEnvParamDir = xml->getStringAttribute(SettingsKey::defaultAmpEnvParamDir);
        defaultPitchEnvParamDir = xml->getStringAttribute(SettingsKey::defaultPitchEnvParamDir);
        defaultSsgSwEnvParamDir = xml->getStringAttribute(SettingsKey::defaultSsgSwEnvParamDir);
        defaultDetuneParamDir = xml->getStringAttribute(SettingsKey::defaultDetuneParamDir);
        defaultQualityParamDir = xml->getStringAttribute(SettingsKey::defaultQualityParamDir);
        defaultPcmPlayParamDir = xml->getStringAttribute(SettingsKey::defaultPcmPlayParamDir);
        defaultToneNoiseParamDir = xml->getStringAttribute(SettingsKey::defaultToneNoiseParamDir);
        showTooltips = xml->getBoolAttribute(SettingsKey::showTooltips, SettingsValue::Initial::showTooltip);
        useHeadroom = xml->getBoolAttribute(SettingsKey::useHeadroom, SettingsValue::Initial::useHeadroom);
        headroomGain = xml->getDoubleAttribute(SettingsKey::headroomGain, SettingsValue::Initial::headroomGain);
        showVirtualKeyboard = xml->getBoolAttribute(SettingsKey::showVirtualKeyboard, SettingsValue::Initial::showVirtualKeyboard);

        // 内部変数の更新
        if (juce::File(defaultSampleDir).isDirectory()) {
            lastSampleDirectory = juce::File(defaultSampleDir);
        }
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
        lastSampleDirectory = newSampleDir;
    }

    if (defaultWavetableDir.isEmpty() || !juce::File(defaultWavetableDir).isDirectory())
    {
        auto newWavetableDir = pluginDir.getChildFile(Io::Folder::wavetable);

        // 存在していなければ作成
        if (!newWavetableDir.exists()) {
            newWavetableDir.createDirectory();
        }

        defaultWavetableDir = newWavetableDir.getFullPathName();
    }

    if (defaultFxOrderDir.isEmpty() || !juce::File(defaultFxOrderDir).isDirectory())
    {
        auto newFxOrderDir = pluginDir.getChildFile(Io::Folder::fxOrder);

        // 存在していなければ作成
        if (!newFxOrderDir.exists()) {
            newFxOrderDir.createDirectory();
        }

        defaultFxOrderDir = newFxOrderDir.getFullPathName();
    }

    if (defaultFxParamDir.isEmpty() || !juce::File(defaultFxParamDir).isDirectory())
    {
        auto newFxParamDir = pluginDir.getChildFile(Io::Folder::fxParam);

        // 存在していなければ作成
        if (!newFxParamDir.exists()) {
            newFxParamDir.createDirectory();
        }

        defaultFxParamDir = newFxParamDir.getFullPathName();
    }

    if (defaultLfoParamDir.isEmpty() || !juce::File(defaultLfoParamDir).isDirectory())
    {
        auto newLfoParamDir = pluginDir.getChildFile(Io::Folder::lfoParam);

        // 存在していなければ作成
        if (!newLfoParamDir.exists()) {
            newLfoParamDir.createDirectory();
        }

        defaultLfoParamDir = newLfoParamDir.getFullPathName();
    }

    if (defaultAmpEnvParamDir.isEmpty() || !juce::File(defaultAmpEnvParamDir).isDirectory())
    {
        auto newAmpEnvParamDir = pluginDir.getChildFile(Io::Folder::ampEnvParam);

        // 存在していなければ作成
        if (!newAmpEnvParamDir.exists()) {
            newAmpEnvParamDir.createDirectory();
        }

        defaultAmpEnvParamDir = newAmpEnvParamDir.getFullPathName();
    }

    if (defaultPitchEnvParamDir.isEmpty() || !juce::File(defaultPitchEnvParamDir).isDirectory())
    {
        auto newPitchEnvParamDir = pluginDir.getChildFile(Io::Folder::pitchEnvParam);

        // 存在していなければ作成
        if (!newPitchEnvParamDir.exists()) {
            newPitchEnvParamDir.createDirectory();
        }

        defaultPitchEnvParamDir = newPitchEnvParamDir.getFullPathName();
    }

    if (defaultSsgSwEnvParamDir.isEmpty() || !juce::File(defaultSsgSwEnvParamDir).isDirectory())
    {
        auto newSsgSwEnvParamDir = pluginDir.getChildFile(Io::Folder::ssgSwEnvParam);

        // 存在していなければ作成
        if (!newSsgSwEnvParamDir.exists()) {
            newSsgSwEnvParamDir.createDirectory();
        }

        defaultSsgSwEnvParamDir = newSsgSwEnvParamDir.getFullPathName();
    }

    if (defaultDetuneParamDir.isEmpty() || !juce::File(defaultDetuneParamDir).isDirectory())
    {
        auto newDetuneParamDir = pluginDir.getChildFile(Io::Folder::detuneParam);

        // 存在していなければ作成
        if (!newDetuneParamDir.exists()) {
            newDetuneParamDir.createDirectory();
        }

        defaultDetuneParamDir = newDetuneParamDir.getFullPathName();
    }

    if (defaultUnisonParamDir.isEmpty() || !juce::File(defaultUnisonParamDir).isDirectory())
    {
        auto newUnisonParamDir = pluginDir.getChildFile(Io::Folder::unisonParam);

        // 存在していなければ作成
        if (!newUnisonParamDir.exists()) {
            newUnisonParamDir.createDirectory();
        }

        defaultUnisonParamDir = newUnisonParamDir.getFullPathName();
    }

    if (defaultQualityParamDir.isEmpty() || !juce::File(defaultQualityParamDir).isDirectory())
    {
        auto newQualityParamDir = pluginDir.getChildFile(Io::Folder::qualityParam);

        // 存在していなければ作成
        if (!newQualityParamDir.exists()) {
            newQualityParamDir.createDirectory();
        }

        defaultQualityParamDir = newQualityParamDir.getFullPathName();
    }

    if (defaultPcmPlayParamDir.isEmpty() || !juce::File(defaultPcmPlayParamDir).isDirectory())
    {
        auto newPcmPlayParamDir = pluginDir.getChildFile(Io::Folder::pcmPlayParam);

        // 存在していなければ作成
        if (!newPcmPlayParamDir.exists()) {
            newPcmPlayParamDir.createDirectory();
        }

        defaultPcmPlayParamDir = newPcmPlayParamDir.getFullPathName();
    }

    if (defaultToneNoiseParamDir.isEmpty() || !juce::File(defaultToneNoiseParamDir).isDirectory())
    {
        auto newToneNoiseParamDir = pluginDir.getChildFile(Io::Folder::toneNoiseParam);

        // 存在していなければ作成
        if (!newToneNoiseParamDir.exists()) {
            newToneNoiseParamDir.createDirectory();
        }

        defaultToneNoiseParamDir = newToneNoiseParamDir.getFullPathName();
    }
}

juce::String AudioPlugin2686V::getDefaultPresetDir()
{
    return defaultPresetDir;
}

void AudioPlugin2686V::unloadAdpcmFile()
{
    // パス情報を削除
    adpcmFilePath.clear();

    // 空のデータを作成
    std::vector<float> emptyData(1, 0.0f);

    // 全ボイスの ADPCM Core に空データをセット（＝クリア）
    for (int i = 0; i < m_synth.getNumVoices(); ++i)
    {
        if (auto* voice = static_cast<SynthVoice*>(m_synth.getVoice(i)))
        {
            // レートはなんでも良いので適当な値(44100)を渡す
            voice->getAdpcmCore()->clearBuffer();
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
        if (auto* voice = static_cast<SynthVoice*>(m_synth.getVoice(i)))
        {
            voice->getRhythmCore()->clearBuffer(padIndex);
        }
    }
}

// 絶対パスのFileを、defaultSampleDirからの相対パス文字列に変換する
juce::String AudioPlugin2686V::makePathRelative(const juce::File& targetFile)
{
    // ファイルが無効、またはディレクトリ未設定ならそのまま絶対パスを返す
    if (targetFile == juce::File() || defaultSampleDir.isEmpty())
        return targetFile.getFullPathName();

    juce::File baseDir(defaultSampleDir);

    // JUCEネイティブの相対パス取得メソッドを使用（文字化けしない！）
    return targetFile.getRelativePathFrom(baseDir);
}

// パス文字列（相対 or 絶対）を、読み込み可能なFileオブジェクトに復元する
juce::File AudioPlugin2686V::resolvePath(const juce::String& pathStr)
{
    if (pathStr.isEmpty()) return juce::File();

    // すでに絶対パスであれば、そのまま使う (JUCEのメソッドで判定)
    if (juce::File::isAbsolutePath(pathStr))
    {
        return juce::File(pathStr);
    }

    // 相対パスの場合は defaultSampleDir と結合する
    if (defaultSampleDir.isNotEmpty())
    {
        juce::File baseDir(defaultSampleDir);

        // getChildFile は相対パス文字列を渡すと安全にフルパスに結合してくれます
        return baseDir.getChildFile(pathStr);
    }

    // ベースディレクトリがない場合は一応そのまま返す
    return juce::File(pathStr);
}

// 絶対パスのFileを、defaultSampleDirからの相対パス文字列に変換する
juce::String AudioPlugin2686V::makeWtPathRelative(const juce::File& targetFile)
{
    // ファイルが無効、またはディレクトリ未設定ならそのまま絶対パスを返す
    if (targetFile == juce::File() || defaultWavetableDir.isEmpty())
        return targetFile.getFullPathName();

    juce::File baseDir(defaultWavetableDir);

    // JUCEネイティブの相対パス取得メソッドを使用（文字化けしない！）
    return targetFile.getRelativePathFrom(baseDir);
}

// パス文字列（相対 or 絶対）を、読み込み可能なFileオブジェクトに復元する
juce::File AudioPlugin2686V::resolveWtPath(const juce::String& pathStr)
{
    if (pathStr.isEmpty()) return juce::File();

    // すでに絶対パスであれば、そのまま使う (JUCEのメソッドで判定)
    if (juce::File::isAbsolutePath(pathStr))
    {
        return juce::File(pathStr);
    }

    // 相対パスの場合は defaultWavetableDir と結合する
    if (defaultWavetableDir.isNotEmpty())
    {
        juce::File baseDir(defaultWavetableDir);

        // getChildFile は相対パス文字列を渡すと安全にフルパスに結合してくれます
        return baseDir.getChildFile(pathStr);
    }

    // ベースディレクトリがない場合は一応そのまま返す
    return juce::File(pathStr);
}

// 絶対パスのFileを、defaultSampleDirからの相対パス文字列に変換する
juce::String AudioPlugin2686V::makeFxOrderPathRelative(const juce::File& targetFile)
{
    // ファイルが無効、またはディレクトリ未設定ならそのまま絶対パスを返す
    if (targetFile == juce::File() || defaultFxOrderDir.isEmpty())
        return targetFile.getFullPathName();

    juce::File baseDir(defaultFxOrderDir);

    // JUCEネイティブの相対パス取得メソッドを使用（文字化けしない！）
    return targetFile.getRelativePathFrom(baseDir);
}

// パス文字列（相対 or 絶対）を、読み込み可能なFileオブジェクトに復元する
juce::File AudioPlugin2686V::resolveFxOrderPath(const juce::String& pathStr)
{
    if (pathStr.isEmpty()) return juce::File();

    // すでに絶対パスであれば、そのまま使う (JUCEのメソッドで判定)
    if (juce::File::isAbsolutePath(pathStr))
    {
        return juce::File(pathStr);
    }

    // 相対パスの場合は defaultFxOrderDir と結合する
    if (defaultFxOrderDir.isNotEmpty())
    {
        juce::File baseDir(defaultFxOrderDir);

        // getChildFile は相対パス文字列を渡すと安全にフルパスに結合してくれます
        return baseDir.getChildFile(pathStr);
    }

    // ベースディレクトリがない場合は一応そのまま返す
    return juce::File(pathStr);
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

void AudioPlugin2686V::initPreset()
{
    // 1. 全パラメータをデフォルト値(Normalized 0.0-1.0)にリセット
    auto& params = getParameters();
    for (auto* param : params)
    {
        if (auto* p = static_cast<juce::AudioProcessorParameterWithID*>(param))
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
    presetGenre = PresetValue::MetaData::Initial::genre;
    presetFilePath = "";

    // 3. サンプルのアンロードとパスクリア
    unloadAdpcmFile();
    // unloadAdpcmFile内で adpcmFilePath.clear() されています

    for (int i = 0; i < RhythmPrValue::pads; ++i) {
        unloadRhythmFile(i);
        // unloadRhythmFile内で rhythmFilePaths[i].clear() されています
    }

    for (int i = 0; i < Opzx7PrValue::ops; i++) {
        unloadOpzx7PcmFile(i);
        unloadOpzx7WtFile(i);
    }
}

void AudioPlugin2686V::initParams(const juce::String& code)
{
    // 1. 全パラメータをデフォルト値(Normalized 0.0-1.0)にリセット
    auto& params = getParameters();
    for (auto* param : params)
    {
        if (auto* p = dynamic_cast<juce::AudioProcessorParameterWithID*>(param))
        {
            if (p->paramID.startsWith(code) || p->paramID == "MASTER_VOL") { // マスターボリュームも初期化
                p->setValueNotifyingHost(p->getDefaultValue());
            }
        }
    }


    if (code == "ADPCM_") {
        unloadAdpcmFile();
    }

    if (code == "RHYTHM_") {
        for (int i = 0; i < RhythmPrValue::pads; ++i) {
            unloadRhythmFile(i);
        }
    }

    if (code == "OPZX7_") {
        for (int i = 0; i < Opzx7PrValue::ops; i++) {
            unloadOpzx7PcmFile(i);
            unloadOpzx7WtFile(i);
        }
    }
}


void AudioPlugin2686V::loadOpzx7PcmFile(int opIndex, const juce::File& file)
{
    if (opIndex < 0 || opIndex >= Opzx7PrValue::ops) return;

    if (auto* reader = formatManager.createReaderFor(file))
    {
        juce::AudioBuffer<float> tempBuffer(1, (int)reader->lengthInSamples);
        reader->read(&tempBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
        delete reader;

        auto* readPtr = tempBuffer.getReadPointer(0);
        opzx7PcmBuffers[opIndex].assign(readPtr, readPtr + tempBuffer.getNumSamples());
        opzx7PcmFilePaths[opIndex] = file.getFullPathName();

        for (int i = 0; i < m_synth.getNumVoices(); ++i) {
            if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
                voice->setOpzx7PcmBuffer(opIndex, &opzx7PcmBuffers[opIndex]);
            }
        }
    }
}

void AudioPlugin2686V::unloadOpzx7PcmFile(int opIndex)
{
    if (opIndex < 0 || opIndex >= Opzx7PrValue::ops) return;

    opzx7PcmBuffers[opIndex].clear();
    opzx7PcmFilePaths[opIndex] = juce::String();

    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
            voice-> clearOpzx7PcmBuffer(opIndex);
        }
    }
}

void AudioPlugin2686V::generatePreviewWaveform(std::vector<float>* destBuffer)
{
    // 1. パラメータの取得と設定
    int m = (int)pMode->load(std::memory_order_relaxed);
    m_previewParams.mode = (OscMode)m;

    switch (m_previewParams.mode) {
    case OscMode::OPNA:      prOpna.processBlock(m_previewParams, apvts); break;
    case OscMode::OPN:       prOpn.processBlock(m_previewParams, apvts); break;
    case OscMode::OPL:       prOpl.processBlock(m_previewParams, apvts); break;
    case OscMode::OPL3:      prOpl3.processBlock(m_previewParams, apvts); break;
    case OscMode::OPM:       prOpm.processBlock(m_previewParams, apvts); break;
    case OscMode::OPZX7:     prOpzx7.processBlock(m_previewParams, apvts); break;
    case OscMode::SSG:       prSsg.processBlock(m_previewParams, apvts); break;
    case OscMode::WAVETABLE: prWt.processBlock(m_previewParams, apvts); break;
    case OscMode::WT2:       prWt2.processBlock(m_previewParams, apvts); break;
    case OscMode::RHYTHM:    prRhythm.processBlock(m_previewParams, apvts); break;
    case OscMode::ADPCM:     prAdpcm.processBlock(m_previewParams, apvts); break;
    case OscMode::BEEP:      prBeep.processBlock(m_previewParams, apvts); break;
    }

    if (auto* voice = dynamic_cast<SynthVoice*>(previewSynth.getVoice(0))) {
        voice->setParameters(m_previewParams);
        for (int i = 0; i < Opzx7PrValue::ops; ++i) {
            voice->setOpzx7PcmBuffer(i, &opzx7PcmBuffers[i]);
            voice->setOpzx7WtBuffer(i, &opzx7WtBuffers[i]);
            voice->setOpzx7Wt2Buffer(i, &opzx7Wt2Buffers[i]);
        }

        // ユニゾン・ハーモニー向けに追加
        voice->stopNote(0.0f, false);
    }

    // 2. 1周期をピッタリ整数サンプルにするため、SampleRateを44000Hzに偽装する
    previewSynth.setCurrentPlaybackSampleRate(44000.0);
    previewSynth.noteOn(1, 69, 1.0f); // 69 = A3 (440.0Hz)

    // 3. アタックフェーズのスキップ (エンベロープを安定させる)
    juce::AudioBuffer<float> skipBuffer(2, previewBufferSize);
    for (int i = 0; i < 40; ++i) {
        skipBuffer.clear();
        previewSynth.renderNextBlock(skipBuffer, juce::MidiBuffer(), 0, previewBufferSize);
    }

    // 4. 1周期をピッタリ100サンプルにする
    int samplesPerCycle = 100; // 44000 / 440 = 100
    int renderSamples = samplesPerCycle * 3;
    juce::AudioBuffer<float> renderBuffer(2, renderSamples);
    renderBuffer.clear();
    previewSynth.renderNextBlock(renderBuffer, juce::MidiBuffer(), 0, renderSamples);

	previewFx.processBlock(renderBuffer, m_previewParams, apvts);

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
    destBuffer->assign(drawSamples, 0.0f);

    float maxAmplitude = 0.0001f;
    for (int i = 0; i < drawSamples; ++i) {
        float val = readPtr[startIndex + i] - dcOffset;
        maxAmplitude = std::max(maxAmplitude, std::abs(val));
    }

    for (int i = 0; i < drawSamples; ++i) {
        float val = readPtr[startIndex + i] - dcOffset;
        (*destBuffer)[i] = val / maxAmplitude;
    }

    // 8. 停止
    previewSynth.noteOff(1, 69, 0.0f, false);

    // ユニゾン・ハーモニー向けに追加
    if (auto* voice = dynamic_cast<SynthVoice*>(previewSynth.getVoice(0))) {
        voice->stopNote(0.0f, false);
    }
}

void AudioPlugin2686V::panic()
{
    // 1. 全てのボイス（回路）の音を強制的に停止（切り離し）します
    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = m_synth.getVoice(i)) {
            // clearCurrentNote() を呼ぶと、JUCE側で即座にそのボイスの計算がスキップされます
            voice->stopNote(0.0f, false);
        }
    }

    prFx.clear();
}

void AudioPlugin2686V::loadOpzx7WtFile(int opIndex, const juce::File& file)
{
    if (opIndex < 0 || opIndex >= Opzx7PrValue::ops) return;

    juce::StringArray lines;
    file.readLines(lines);

    if (lines.size() == 0) return;

    int sampleCount = lines[0].trim().getIntValue();

    if (sampleCount != 32 && sampleCount != 64 && sampleCount != 128 && sampleCount != 256) return;

    std::vector<float> values(sampleCount, 0.0f);

    for (int i = 0; i < sampleCount; ++i) {
        if (i + 1 < lines.size()) {
            float val = lines[i + 1].getFloatValue();
            values[i] = std::clamp(val, -1.0f, 1.0f);
        }
    }

    opzx7WtBuffers[opIndex] = values;
    opzx7WtFilePaths[opIndex] = file.getFullPathName();

    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
            voice->setOpzx7WtBuffer(opIndex, &opzx7WtBuffers[opIndex]);
        }
    }
}

void AudioPlugin2686V::unloadOpzx7WtFile(int opIndex)
{
    if (opIndex < 0 || opIndex >= Opzx7PrValue::ops) return;

    opzx7WtBuffers[opIndex].clear();
    opzx7WtFilePaths[opIndex] = juce::String();

    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
            voice->clearOpzx7WtBuffer(opIndex);
        }
    }
}

void AudioPlugin2686V::loadOpzx7Wt2File(int opIndex, const juce::File& file)
{
    if (opIndex < 0 || opIndex >= Opzx7PrValue::ops) return;

    juce::StringArray lines;
    file.readLines(lines);

    if (lines.size() == 0) return;

    int sampleCount = lines[0].trim().getIntValue();

    if (sampleCount != 32 && sampleCount != 64 && sampleCount != 128 && sampleCount != 256) return;

    int resolution = lines[1].trim().getIntValue();

    if (resolution != 16 && resolution != 32 && resolution != 64 && resolution != 128 && resolution != 256) return;

    int center = resolution / 2;
    std::vector<float> values(sampleCount, 0.0f);

    for (int i = 0; i < sampleCount; ++i) {
        if (i + 2 < lines.size()) {
            int val = lines[i + 2].getIntValue();
            val = std::clamp(val, 0, resolution - 1);

            // 整数を実数に変換 (0 -> -1.0f, center -> 0.0f, max -> ~1.0f)
            values[i] = (float)(val - center) / (float)center;
        }
    }

    opzx7Wt2Buffers[opIndex] = values;
    opzx7Wt2FilePaths[opIndex] = file.getFullPathName();

    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
            voice->setOpzx7Wt2Buffer(opIndex, &opzx7Wt2Buffers[opIndex]);
        }
    }
}

void AudioPlugin2686V::unloadOpzx7Wt2File(int opIndex)
{
    if (opIndex < 0 || opIndex >= Opzx7PrValue::ops) return;

    opzx7Wt2Buffers[opIndex].clear();
    opzx7Wt2FilePaths[opIndex] = juce::String();

    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
            voice->clearOpzx7Wt2Buffer(opIndex);
        }
    }
}

CurveCore* AudioPlugin2686V::getCurveCore()
{
    return &m_curveCore;
}

void AudioPlugin2686V::bakeCurves()
{
	m_curveCore.bakeCurves(); // 内部で必要な計算を行う
}

void AudioPlugin2686V::bakeCurvesPrim(int positionIndex, int targetIndex, int paramIndex)
{
	m_curveCore.bakeCurvesPrim(positionIndex, targetIndex, paramIndex); // 内部で必要な計算を行う
}

void AudioPlugin2686V::resetMidiSettings() {
    m_synth.isMonoMode = false;
    m_synth.useVelocity = false;
    m_synth.fixedVelocity = 0.5f;
    m_synth.pitchResetOnLegato = false;
}

std::vector<int> AudioPlugin2686V::getFxOrder() {
    return prFx.getOrder();
}

void AudioPlugin2686V::updateFxOrder(std::vector<int> newOrder)
{
    prFx.updateOrder(newOrder);
}

bool AudioPlugin2686V::isPlaying()
{
    bool flag = false;

    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
            flag = flag || voice->isVoiceActive() || voice->isPlaying();
        }
    }

    return flag;
}

bool AudioPlugin2686V::isMidiProcessing() {
    return m_synth.isMidiProcessing;
}

OscMode AudioPlugin2686V::getCurrentMode()
{
    int m = (int)pMode->load(std::memory_order_relaxed);

    return (OscMode)m;
}
