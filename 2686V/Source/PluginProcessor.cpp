#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <filesystem>

// ============================================================================
// Constructor
// ============================================================================
AudioPlugin2686V::AudioPlugin2686V()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if defined(BUILD_AS_FX_PLUGIN)
        // FXモード: ステレオ入力あり、ステレオ出力あり
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#else
        // シンセモード: 入力なし、ステレオ出力あり
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    // Initialize APVTS (Parameters are created here)
    apvts(*this, nullptr, "Parameters", createParameterLayout()) // APVTSの初期化
#endif
{
    m_synth.addSound(new SynthSound());

    for (int i = 0; i < 32; i++)
    {
        m_synth.addVoice(new SynthVoice());
    }

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

    // Mode: 0:OPNA, 1:OPN, 2:OPL, 3:OPLL, 4:OPL3, 5:OPM, 6: OPZX3 7:SSG, 8:WAVETABLE 9:RHYTHM, 10:ADPCM
    layout.add(std::make_unique<juce::AudioParameterInt>("MODE", "Mode", 0, TabNumber, 0));

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
    createFxParameterLayout(layout);

    // ★マスターボリューム追加
    // 範囲: -60dB (無音に近い) ～ +6dB (少しブースト可能)
    // 初期値: -3.0dB (FMは音がデカいので少し下げておくのがコツ)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "MASTER_VOL",      // パラメータID
        "Master Vol",      // 表示名
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
    m_synth.setCurrentPlaybackSampleRate(sampleRate);

    // Prepare RhythmCore for all voices
    for (int i = 0; i < m_synth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i))) {
            voice->getRhythmCore()->prepare(sampleRate);
        }
    }

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
    int m = (int)*apvts.getRawParameterValue("MODE");
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

    float dbValue = *apvts.getRawParameterValue("MASTER_VOL");

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

bool AudioPlugin2686V::hasEditor() const { return true; }

// Parameters / Settings Related
#if defined(BUILD_AS_FX_PLUGIN)
const juce::String AudioPlugin2686V::getName() const { return "2686VFX"; }
#else
const juce::String AudioPlugin2686V::getName() const { return "2686V"; }
#endif
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
    // メタデータとパスを属性として追加
    xml->setAttribute("presetName", sanitizeString(presetName, presetNameLength));
    xml->setAttribute("presetAuthor", sanitizeString(presetAuthor, presetAuthorLength));
    xml->setAttribute("presetVersion", sanitizeString(presetVersion, presetVersionLength));
    xml->setAttribute("presetComment", sanitizeString(presetComment, commentLength));
    xml->setAttribute("activeModeName", getModeName((OscMode)(int)*apvts.getRawParameterValue("MODE")));

    xml->setAttribute("adpcmPath", makePathRelative(juce::File(adpcmFilePath)));
    for (int i = 0; i < 8; ++i) {
        xml->setAttribute("rhythmPath" + juce::String(i), makePathRelative(juce::File(rhythmFilePaths[i])));
    }
};

void AudioPlugin2686V::getPresetFromXml(std::unique_ptr<juce::XmlElement>& xmlState)
{
    if (xmlState.get() != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        // パラメータ復帰
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

        // メタデータ復帰
        presetName = xmlState->getStringAttribute("presetName", "Init Preset");
        presetAuthor = xmlState->getStringAttribute("presetAuthor", "Anonymous User");
        presetVersion = xmlState->getStringAttribute("presetVersion", "1.0.0");
        presetComment = xmlState->getStringAttribute("presetComment", "");

        // サンプル復帰 (ADPCM)
        juce::String storedAdpcm = xmlState->getStringAttribute("adpcmPath");
        juce::File adpcmFile = resolvePath(storedAdpcm);
        if (adpcmFile.existsAsFile()) {
            loadAdpcmFile(adpcmFile);
        }

        // サンプル復帰 (Rhythm)
        for (int i = 0; i < 8; ++i) {
            juce::String storedRhy = xmlState->getStringAttribute("rhythmPath" + juce::String(i));
            juce::File rhyFile = resolvePath(storedRhy);
            if (rhyFile.existsAsFile()) {
                loadRhythmFile(rhyFile, i);
            }
        }
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
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    getPresetFromXml(xmlState);
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

void AudioPlugin2686V::addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_AR", prefix + " Env", 0.03f, 5.0f, 0.03f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_DR", prefix + " Env", 0.0f, 5.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_SL", prefix + " Env", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_RR", prefix + " Env", 0.03f, 5.0f, 0.03f));
}

void AudioPlugin2686V::createOpnaParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPNA (YM2608) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>("OPNA_ALGORITHM", "OPNA Algorithm", 0, 7, 7));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPNA_FEEDBACK", "OPNA Feedback", 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPNA_FEEDBACK2", "OPNA Feedback2", 0.0f, 7.0f, 0.0f));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>("OPNA_BIT", "OPNA Bit Depth", 0, 4, 4));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>("OPNA_RATE", "OPNA Rate", 1, 7, 2)); // Default 6 (16kHz)

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = "OPNA_OP" + juce::String(op) + "_";
        juce::String namePrefix = "OPNA OP" + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "MUL", namePrefix + "MUL", 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "DT", namePrefix + "DT", 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "AR", namePrefix + "AR", 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "DR", namePrefix + "DR", 0.0f, 5.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SR", namePrefix + "SR", 0.0f, 10.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SL", namePrefix + "SL", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "RR", namePrefix + "RR", 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "SE", namePrefix + "SE", 0, 15, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SEFREQ", namePrefix + "SEFREQ", 0.1f, 20.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "TL", namePrefix + "TL", 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "KS", namePrefix + "KS", 0, 3, 0)); // KS (0 to 3)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "AM", namePrefix + "AM", false)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "FIX", namePrefix + "FIX", false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "FREQ", namePrefix + "FREQ", juce::NormalisableRange<float>(0.0f, 8000.0f, 0.0f, 0.3f), 440.0f));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "MASK", namePrefix + "MASK", false)); // OP Mask (Switch)
    }

    layout.add(std::make_unique<juce::AudioParameterFloat>("OPNA_LFO_FREQ", "OPNA LFO Freq", 0.1f, 200.0f, 5.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>("OPNA_LFO_PMS", "OPNA LFO PMS", 0, 7, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>("OPNA_LFO_AMS", "OPNA LFO AMS", 0, 3, 0));
}

void AudioPlugin2686V::createOpnParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPN (YM2203) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>("OPN_ALGORITHM", "OPN Algorithm", 0, 7, 7));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPN_FEEDBACK", "OPN Feedback", 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPN_FEEDBACK2", "OPN Feedback2", 0.0f, 7.0f, 0.0f));
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>("OPN_BIT", "OPN Bit Depth", 0, 4, 4));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>("OPN_RATE", "OPN Rate", 1, 7, 2)); // Default 6 (16kHz)

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = "OPN_OP" + juce::String(op) + "_";
        juce::String namePrefix = "OPN OP" + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "MUL", namePrefix + "MUL", 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "DT", namePrefix + "DT", 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "AR", namePrefix + "AR", 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "DR", namePrefix + "DR", 0.0f, 5.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SR", namePrefix + "SR", 0.0f, 10.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SL", namePrefix + "SL", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "RR", namePrefix + "RR", 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "TL", namePrefix + "TL", 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "KS", namePrefix + "KS", 0, 3, 0)); // KS (0 to 3)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "FIX", namePrefix + "FIX", false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "FREQ", namePrefix + "FREQ", juce::NormalisableRange<float>(0.0f, 8000.0f, 0.0f, 0.3f), 440.0f));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "MASK", namePrefix + "MASK", false)); // OP Mask (Switch)
    }
}

void AudioPlugin2686V::createOplParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPL (YM3526) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>("OPL_ALGORITHM", "OPL Algorithm", 0, 1, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPL_FEEDBACK", "OPL Feedback", 0.0f, 7.0f, 0.0f));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>("OPL_BIT", "OPL Bit Depth", 0, 4, 4));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>("OPL_RATE", "OPL Rate", 1, 7, 2)); // Default 6 (16kHz)

    for (int op = 0; op < 2; ++op)
    {
        juce::String prefix = "OPL_OP" + juce::String(op) + "_";
        juce::String namePrefix = "OPL OP" + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "MUL", namePrefix + "MUL", 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "DT", namePrefix + "DT", 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "TL", namePrefix + "TL", 0.0f, 1.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "AM", "AM", false));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "VIB", "VIB", false));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "EG_TYP", "EG TYPE", true)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "KSR", "KSR", false)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "KSL", "KSL", 0, 3, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "AR", namePrefix + "AR", 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "DR", namePrefix + "DR", 0.0f, 5.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SL", namePrefix + "SL", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "RR", namePrefix + "RR", 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "EG", namePrefix + "EG", 0, 3, 0));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "MASK", namePrefix + "MASK", false)); // OP Mask (Switch)
    }
}

void AudioPlugin2686V::createOpl3ParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==============================================================================
    // OPL3 Parameters (4 Ops, 8 Waveforms)
    // ==============================================================================
    // Algorithm (0-7) - Using generic FM algorithms mapped to OPL3 4-op modes
    layout.add(std::make_unique<juce::AudioParameterInt>("OPL3_ALGORITHM", "OPL3 Algorithm", 1, 8, 1));

    // Feedback (Op1/Op3)
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPL3_FEEDBACK", "OPL3 Feedback", 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPL3_FEEDBACK2", "OPL3 Feedback2", 0.0f, 7.0f, 0.0f));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>("OPL3_BIT", "OPL3 Bit Depth", 0, 4, 4));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>("OPL3_RATE", "OPL3 Rate", 1, 7, 2)); // Default 6 (16kHz)

    for (int i = 0; i < 4; ++i)
    {
        juce::String prefix = "OPL3_OP" + juce::String(i) + "_";
        juce::String namePrefix = "OPL3 OP" + juce::String(i + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "MUL", namePrefix + "MUL", 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "TL", namePrefix + "TL", 0.0f, 1.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "AR", namePrefix + "AR", 0.03f, 1.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "DR", namePrefix + "DR", 0.0f, 1.0f, 0.1f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SL", namePrefix + "SL", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "RR", namePrefix + "RR", 0.03f, 1.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "AM", "AM", false));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "VIB", "VIB", false));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "EG_TYP", "EG TYPE", true)); // 1=Sustain, 0=Decay
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "KSR", "KSR", false)); // 0=Low, 1=High
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "KSL", "KSL", 0, 3, 0));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "EG", namePrefix + "EG", 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "MASK", namePrefix + "MASK", false)); // OP Mask (Switch)
    }
}

void AudioPlugin2686V::createOpmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPM (YM2151) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>("OPM_ALGORITHM", "OPM Algorithm", 0, 7, 7));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPM_FEEDBACK", "OPM Feedback", 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPM_FEEDBACK2", "OPM Feedback2", 0.0f, 7.0f, 0.0f));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>("OPM_BIT", "OPM Bit Depth", 0, 4, 4));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>("OPM_RATE", "OPM Rate", 1, 7, 2)); // Default 6 (16kHz)

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = "OPM_OP" + juce::String(op) + "_";
        juce::String namePrefix = "OPM OP" + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "MUL", namePrefix + "MUL", 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "DT", namePrefix + "DT", 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "DT2", namePrefix + "DT2", 0, 3, 0)); // Coarse Detune (OPM Only)
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "AR", namePrefix + "AR", 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "DR", namePrefix + "D1R", 0.0f, 1.0f, 0.1f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SL", namePrefix + "D1L", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SR", namePrefix + "D2R", 0.0f, 1.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "RR", namePrefix + "RR", 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "TL", namePrefix + "TL", 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "KS", namePrefix + " KS", 0, 3, 0)); // KS (0 to 3)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "FIX", namePrefix + "FIX", false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "FREQ", namePrefix + "FREQ", juce::NormalisableRange<float>(0.0f, 8000.0f, 0.0f, 0.3f), 440.0f));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "MASK", namePrefix + "MASK", false)); // OP Mask (Switch)
    }

    layout.add(std::make_unique<juce::AudioParameterFloat>("OPM_LFO_FREQ", "OPM LFO Freq", 0.1f, 200.0f, 5.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>("OPM_LFO_PMS", "OPM LFO PMS", 0, 7, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>("OPM_LFO_AMS", "OPM LFO AMS", 0, 3, 0));
}

void AudioPlugin2686V::createOpzx3ParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // OPZ + OPX + MA-3(SD-1) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>("OPZX3_ALGORITHM", "OPZX3 Algorithm", 0, 27, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPZX3_FEEDBACK", "OPZX3 Feedback", 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPZX3_FEEDBACK2", "OPZX3 Feedback2", 0.0f, 7.0f, 0.0f));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>("OPZX3_BIT", "OPZX3 Bit Depth", 0, 4, 4));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>("OPZX3_RATE", "OPZX3 Rate", 1, 7, 2)); // Default 6 (16kHz)

    for (int op = 0; op < 4; ++op)
    {
        juce::String prefix = "OPZX3_OP" + juce::String(op) + "_";
        juce::String namePrefix = "OPZX3 OP" + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "MUL", namePrefix + "MUL", 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "DT", namePrefix + "DT", 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "DT2", namePrefix + "DT2", 0, 3, 0)); // Coarse Detune (OPM Only)
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "AR", namePrefix + "AR", 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "DR", namePrefix + "D1R", 0.0f, 1.0f, 0.1f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SL", namePrefix + "D1L", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SR", namePrefix + "D2R", 0.0f, 1.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "RR", namePrefix + "RR", 0.03f, 5.0f, 0.03f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "TL", namePrefix + "TL", 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "KS", namePrefix + " KS", 0, 3, 0)); // KS (0 to 3)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "FIX", namePrefix + "FIX", false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "FREQ", namePrefix + "FREQ", juce::NormalisableRange<float>(0.0f, 8000.0f, 0.0f, 0.3f), 440.0f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "WS", namePrefix + "WS", 0, 28, 0));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "MASK", namePrefix + "MASK", false)); // OP Mask (Switch)
    }

    layout.add(std::make_unique<juce::AudioParameterFloat>("OPZX3_LFO_FREQ", "OPZX3 LFO Freq", 0.1f, 200.0f, 5.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>("OPZX3_LFO_PMS", "OPZX3 LFO PMS", 0, 7, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>("OPZX3_LFO_AMS", "OPZX3 LFO AMS", 0, 3, 0));
}

void AudioPlugin2686V::createSsgParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // SSG Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_LEVEL", "SSG Tone", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_NOISE", "SSG Noise", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_MIX", "SSG Tone/Noise Mix", 0.0f, 1.0f, 0.5f)); // デフォルトは 0.5 (Mix)
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_NOISE_FREQ", "Noise Freq", juce::NormalisableRange<float>(50.0f, 20000.0f, 1.0f, 0.3f), 12000.0f)); // Noise Frequency Default: 12000Hz (Standard PSG feel)

    layout.add(std::make_unique<juce::AudioParameterInt>("SSG_WAVEFORM", "SSG Waveform", 0, 1, 0)); // 0:Pulse, 1:Triangle

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>("SSG_ADSR_BYPASS", "SSG ADSR Bypass", false));

	addEnvParameters(layout, "SSG");

    layout.add(std::make_unique<juce::AudioParameterInt>("SSG_DUTY_MODE", "SSG Duty Mode", 0, 1, 0)); // Duty Mode: 0=Preset, 1=Variable
    layout.add(std::make_unique<juce::AudioParameterInt>("SSG_DUTY_PRESET", "SSG Duty Preset", 0, 8, 0)); // Preset: 0~8
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_DUTY_VAR", "SSG Duty Var", 0.0f, 0.5f, 0.5f)); // Variable: 0.0 ~ 0.5
    layout.add(std::make_unique<juce::AudioParameterBool>("SSG_DUTY_INV", "SSG Duty Invert", false)); // Invert

    layout.add(std::make_unique<juce::AudioParameterBool>("SSG_TRI_KEYTRK", "SSG Triangle Key Track", true)); // Key Track Switch (Default ON)
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_TRI_PEAK", "SSG Triangle Peak", 0.0f, 1.0f, 0.5f)); // Triangle Peak: 0.0 ~ 1.0
    // Triangle Manual Frequency
    // Range: 0.1Hz - 8000Hz, Skew: 0.3 (Log的挙動), Default: 440Hz
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_TRI_FREQ", "SSG Tri Manual Freq", juce::NormalisableRange<float>(0.1f, 8000.0f, 0.1f, 0.3f), 440.0f));

    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>("SSG_BIT", "SSG Bit Depth", 0, 4, 3));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>("SSG_RATE", "SSG Rate", 1, 7, 6)); // Default 6 (16kHz)

    // Hardware Envelope Parameters
    layout.add(std::make_unique<juce::AudioParameterBool>("SSG_ENV_ENABLE", "SSG HW Env Enable", false)); // HW Env Enable(Bool)
    layout.add(std::make_unique<juce::AudioParameterInt>("SSG_ENV_SHAPE", "SSG Env Shape", 0, 7, 0)); // Shape: 0-7
    // 実機のPeriodは値が大きいほど遅いですが、スライダーは右に行くほど速い方が直感的なためHzにします
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_ENV_PERIOD", "SSG Env Speed", 0.1f, 200.0f, 1.0f)); // Period: ここでは周波数(Hz)として扱います (0.1Hz ~ 200Hz)
}

void AudioPlugin2686V::createWavetableParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>("WT_BIT", "WT Bit Depth", 0, 4, 3));
    // Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>("WT_RATE", "WT Rate", 1, 7, 6)); // Default 6 (16kHz)
    // Size: 0:32, 1:64
    layout.add(std::make_unique<juce::AudioParameterInt>("WT_SIZE", "WT Sample Size", 0, 1, 0));
    // Waveform Preset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>("WT_WAVE", "WT Waveform", 0, 8, 0));

    // Custom Waveform Sliders (32 params)
    // Range: -1.0 to 1.0, Default: 0.0
    for (int i = 0; i < 32; ++i)
    {
        auto paramId = "WT_CUSTOM32_" + juce::String(i);
        auto paramName = "WT Custom32 " + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, -1.0f, 1.0f, 0.0f));
    }

    for (int i = 0; i < 64; ++i)
    {
        auto paramId = "WT_CUSTOM64_" + juce::String(i);
        auto paramName = "WT Custom64 " + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, -1.0f, 1.0f, 0.0f));
    }

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>("WT_MOD_ENABLE", "WT Mod Enable", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("WT_MOD_DEPTH", "WT Mod Depth", 0.0f, 1.0f, 0.2f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("WT_MOD_SPEED", "WT Mod Speed", 0.1f, 10.0f, 1.0f));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>("WT_LEVEL", "WT Level", 0.0f, 1.0f, 1.0f));
    addEnvParameters(layout, "WT");
}

void AudioPlugin2686V::createRhythmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // Rhythm Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>("RHYTHM_LEVEL", "RHYTHM Vol", 0.0f, 1.0f, 1.0f));

    // Create parameters for each of the 8 pads
    for (int i = 0; i < 8; ++i) {
        juce::String prefix = "RHYTHM_PAD" + juce::String(i);
        juce::String namePrefix = "RHYTHM Pad" + juce::String(i + 1);
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_VOL", namePrefix + " Vol", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_PAN", namePrefix + " Pan", 0.0f, 1.0f, 0.5f));

        // Note number
        int defNote = 60 + i;

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "_NOTE", namePrefix + " Note", 0, 127, defNote));

        // 1: Raw 32bit 2: PCM 24bit 3: PCM 16bit 4: PCM 8bit 5: PCM 5bit 6: PCM 4bit 7: ADPCM 4bit
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "_MODE", namePrefix + " Quality", 1, 7, 7));

        // 1: 96kHz 2: 55.5kHz 3: 48kHz 4: 44.1kHz 5: 22.5kHz 6: 16k 7: 8k
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "_RATE", namePrefix + " Rate", 1, 7, 6));

        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "_ONESHOT", namePrefix + " One Shot", true));

        // Release Parameter
        // 範囲: 0.03秒 ～ 5.0秒, 初期値: 0.1秒
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_RR", namePrefix + " Release", 0.03f, 5.0f, 0.1f));
    }
}

void AudioPlugin2686V::createAdpcmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // ADPCM Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>("ADPCM_LEVEL", "ADPCM Level", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("ADPCM_PAN", "ADPCM Pan", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterBool>("ADPCM_LOOP", "ADPCM Loop", true));
    // 0:Raw, 1:24bit, 2:16bit, 3:8bit, 4:5bit, 5:4bit PCM 6:ADPCM(4bit)
    layout.add(std::make_unique<juce::AudioParameterInt>("ADPCM_MODE", "ADPCM Quality", 0, 6, 0));
    // 0: 96kHz 1: 55.5kHz 2: 48kHz 3: 44.1kHz 4: 22.5kHz 5: 16k 6: 8k
    layout.add(std::make_unique<juce::AudioParameterInt>("ADPCM_RATE", "ADPCM Rate", 0, 6, 3)); // Default: 3 (44.1kHz)

    addEnvParameters(layout, "ADPCM");
}

void AudioPlugin2686V::createFxParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // FX Parameters
    // ==========================================
    // Bypass
    layout.add(std::make_unique<juce::AudioParameterBool>("FX_BYPASS", "Master FX Bypass", false));

    // --- Tremolo ---
    layout.add(std::make_unique<juce::AudioParameterBool>("FX_TRM_BYPASS", "Tremolo Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_TRM_RATE", "Tremolo Rate", 0.1f, 20.0f, 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_TRM_DEPTH", "Tremolo Depth", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_TRM_MIX", "Tremolo Mix", 0.0f, 1.0f, 0.0f));

    // --- Vibrato / Detune ---
    layout.add(std::make_unique<juce::AudioParameterBool>("FX_VIB_BYPASS", "Vibrato Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_VIB_RATE", "Vibrato Rate", 0.1f, 10.0f, 2.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_VIB_DEPTH", "Vibrato Depth", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_VIB_MIX", "Vibrato Mix", 0.0f, 1.0f, 0.0f));

    // --- Modern Bit Crusher ---
    layout.add(std::make_unique<juce::AudioParameterBool>("FX_MBC_BYPASS", "Modern BC Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_MBC_RATE", "Modern BC Rate", 1.0f, 50.0f, 1.0f)); // Rate: 1(High) ～ 50(Low)
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_MBC_BITS", "Modern BC Quality", 2.0f, 24.0f, 24.0f)); // Bits: 24(Clean) ～ 2(Noisy)
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_MBC_MIX", "Modern BX Mix", 0.0f, 1.0f, 0.0f));

    // --- Delay ---
    layout.add(std::make_unique<juce::AudioParameterBool>("FX_DLY_BYPASS", "Delay Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_DLY_TIME", "Delay Time", 10.0f, 1000.0f, 375.0f)); // ms
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_DLY_FB", "Delay Feedback", 0.0f, 0.95f, 0.4f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_DLY_MIX", "Delay Mix", 0.0f, 1.0f, 0.0f));

    // --- Reverb ---
    layout.add(std::make_unique<juce::AudioParameterBool>("FX_RVB_BYPASS", "Reverb Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_RVB_SIZE", "Reverb Size", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_RVB_DAMP", "Reverb Damp", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_RVB_MIX", "Reverb Mix", 0.0f, 1.0f, 0.0f));

    // --- Retro Bit Crusher ---
    layout.add(std::make_unique<juce::AudioParameterBool>("FX_RBC_BYPASS", "Retro BC Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterInt>("FX_RBC_RATE", "Retro BC Rate", 1, 7, 6));    // Default: 16kHz
    layout.add(std::make_unique<juce::AudioParameterInt>("FX_RBC_BITS", "Retro BC Quality", 1, 7, 7)); // Default: ADPCM
    layout.add(std::make_unique<juce::AudioParameterFloat>("FX_RBC_MIX", "Retro BC Mix", 0.0f, 1.0f, 0.0f));
}

void AudioPlugin2686V::processOpnaBlock(SynthParams &params)
{
    params.feedback = *apvts.getRawParameterValue("OPNA_FEEDBACK");
    params.feedback2 = *apvts.getRawParameterValue("OPNA_FEEDBACK2");
    params.algorithm = (int)*apvts.getRawParameterValue("OPNA_ALGORITHM");
    params.fmBitDepth = (int)*apvts.getRawParameterValue("OPNA_BIT");
    params.fmRateIndex = (int)*apvts.getRawParameterValue("OPNA_RATE");

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = "OPNA_OP" + juce::String(op) + "_";

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + "MUL");
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + "DT");
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + "AR");
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + "DR");
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + "SL");
        params.fmOp[op].release = *apvts.getRawParameterValue(p + "RR");
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + "KS");
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + "TL");
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + "SR");
        params.fmOp[op].ssgEg = (int)*apvts.getRawParameterValue(p + "SE");
        params.fmOp[op].fmSsgEgFreq = *apvts.getRawParameterValue(p + "SEFREQ");
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + "FIX") > 0.5f);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + "FREQ");
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = (*apvts.getRawParameterValue(p + "AM") > 0.5f);
        params.fmOp[op].vibEnable = true;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + "MASK") > 0.5f);
    }

    params.lfoFreq = *apvts.getRawParameterValue("OPNA_LFO_FREQ");
    params.pms = (int)*apvts.getRawParameterValue("OPNA_LFO_PMS");
    params.ams = (int)*apvts.getRawParameterValue("OPNA_LFO_AMS");
}

void AudioPlugin2686V::processOpnBlock(SynthParams &params)
{
    params.feedback = *apvts.getRawParameterValue("OPN_FEEDBACK");
    params.feedback2 = *apvts.getRawParameterValue("OPN_FEEDBACK2");
    params.algorithm = (int)*apvts.getRawParameterValue("OPN_ALGORITHM");
    params.fmBitDepth = (int)*apvts.getRawParameterValue("OPN_BIT");
    params.fmRateIndex = (int)*apvts.getRawParameterValue("OPN_RATE");

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = "OPN_OP" + juce::String(op) + "_";

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + "MUL");
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + "DT");
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + "AR");
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + "DR");
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + "SL");
        params.fmOp[op].release = *apvts.getRawParameterValue(p + "RR");
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + "KS");
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + "TL");
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + "SR");
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + "FIX") > 0.5f);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + "FREQ");
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = false; // OPNにはAMは無い
        params.fmOp[op].vibEnable = false;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + "MASK") > 0.5f);
    }
}

void AudioPlugin2686V::processOplBlock(SynthParams &params)
{
    params.feedback = *apvts.getRawParameterValue("OPL_FEEDBACK");
    params.algorithm = (int)*apvts.getRawParameterValue("OPL_ALGORITHM");
    params.fmBitDepth = (int)*apvts.getRawParameterValue("OPL_BIT");
    params.fmRateIndex = (int)*apvts.getRawParameterValue("OPL_RATE");

    for (int op = 0; op < 2; ++op)
    {
        juce::String p = "OPL_OP" + juce::String(op) + "_";

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + "MUL");
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + "TL");
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + "DT");
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + "AR");
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + "DR");
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + "SL");
        params.fmOp[op].release = *apvts.getRawParameterValue(p + "RR");
        params.fmOp[op].amEnable = *apvts.getRawParameterValue(p + "AM") > 0.5f;
        params.fmOp[op].vibEnable = *apvts.getRawParameterValue(p + "VIB") > 0.5f;
        params.fmOp[op].egType = *apvts.getRawParameterValue(p + "EG_TYP") > 0.5f;
        params.fmOp[op].sustainRate = 0.0f; // EG_TYPEがONの時に影響されるため
        // KSR=1だとオフセットが小さすぎるため、3に増加させる
        bool ksrOn = *apvts.getRawParameterValue(p + "KSR") > 0.5f;
        params.fmOp[op].keyScale = ksrOn ? 3 : 0;
        params.fmOp[op].keyScaleLevel = (int)*apvts.getRawParameterValue(p + "KSL");
        params.fmOp[op].waveSelect = (int)*apvts.getRawParameterValue(p + "EG");
        params.fmOp[op].ssgEg = 0; // OPLにはSSG-EGは無い
        params.fmOp[op].fixedMode = false; // OPLにはFixed Modeは無い
        params.fmOp[op].fixedFreq = 0.0f;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + "MASK") > 0.5f);
    }
}

void AudioPlugin2686V::processOpl3Block(SynthParams& params)
{
    params.feedback = *apvts.getRawParameterValue("OPL3_FEEDBACK");
    params.feedback2 = *apvts.getRawParameterValue("OPL3_FEEDBACK2");
    params.algorithm = (int)*apvts.getRawParameterValue("OPL3_ALGORITHM");
    params.fmBitDepth = (int)*apvts.getRawParameterValue("OPL3_BIT");
    params.fmRateIndex = (int)*apvts.getRawParameterValue("OPL3_RATE");

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = "OPL3_OP" + juce::String(op) + "_";

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + "MUL");
        params.fmOp[op].detune = 0;
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + "TL");
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + "AR");
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + "DR");
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + "SL");
        params.fmOp[op].release = *apvts.getRawParameterValue(p + "RR");
        params.fmOp[op].amEnable = *apvts.getRawParameterValue(p + "AM") > 0.5f;
        params.fmOp[op].vibEnable = *apvts.getRawParameterValue(p + "VIB") > 0.5f;
        params.fmOp[op].egType = *apvts.getRawParameterValue(p + "EG_TYP") > 0.5f;
        params.fmOp[op].sustainRate = 0.0f; // EG_TYPEがONの時に影響されるため
        // KSR=1だとオフセットが小さすぎるため、3に増加させる
        bool ksrOn = *apvts.getRawParameterValue(p + "KSR") > 0.5f;
        params.fmOp[op].keyScale = ksrOn ? 3 : 0;
        params.fmOp[op].keyScaleLevel = (int)*apvts.getRawParameterValue(p + "KSL");
        params.fmOp[op].waveSelect = (int)*apvts.getRawParameterValue(p + "EG");
        params.fmOp[op].ssgEg = 0; // OPLにはSSG-EGは無い
        params.fmOp[op].fixedMode = false; // OPLにはFixed Modeは無い
        params.fmOp[op].fixedFreq = 0.0f;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + "MASK") > 0.5f);
    }
}

void AudioPlugin2686V::processOpmBlock(SynthParams& params)
{
    params.feedback = *apvts.getRawParameterValue("OPM_FEEDBACK");
    params.feedback2 = *apvts.getRawParameterValue("OPM_FEEDBACK2");
    params.algorithm = (int)*apvts.getRawParameterValue("OPM_ALGORITHM");
    params.fmBitDepth = (int)*apvts.getRawParameterValue("OPM_BIT");
    params.fmRateIndex = (int)*apvts.getRawParameterValue("OPM_RATE");

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = "OPM_OP" + juce::String(op) + "_";

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + "MUL");
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + "DT");
        params.fmOp[op].detune2 = (int)*apvts.getRawParameterValue(p + "DT2");
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + "TL");
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + "AR");
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + "DR");
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + "SL");
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + "SR");
        params.fmOp[op].release = *apvts.getRawParameterValue(p + "RR");
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + "KS");
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + "FIX") > 0.5f);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + "FREQ");
        params.fmOp[op].waveSelect = 0;
        params.fmOp[op].amEnable = false;
        params.fmOp[op].vibEnable = true;
        params.fmOp[op].egType = true;
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + "MASK") > 0.5f);
    }

    params.lfoFreq = *apvts.getRawParameterValue("OPM_LFO_FREQ");
    params.pms = (int)*apvts.getRawParameterValue("OPM_LFO_PMS");
    params.ams = (int)*apvts.getRawParameterValue("OPM_LFO_AMS");
}

void AudioPlugin2686V::processOpzx3Block(SynthParams& params)
{
    params.feedback = *apvts.getRawParameterValue("OPZX3_FEEDBACK");
    params.feedback2 = *apvts.getRawParameterValue("OPZX3_FEEDBACK2");
    params.algorithm = (int)*apvts.getRawParameterValue("OPZX3_ALGORITHM");
    params.fmBitDepth = (int)*apvts.getRawParameterValue("OPZX3_BIT");
    params.fmRateIndex = (int)*apvts.getRawParameterValue("OPZX3_RATE");

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = "OPZX3_OP" + juce::String(op) + "_";

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + "MUL");
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + "DT");
        params.fmOp[op].detune2 = (int)*apvts.getRawParameterValue(p + "DT2");
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + "TL");
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + "AR");
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + "DR");
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + "SL");
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + "SR");
        params.fmOp[op].release = *apvts.getRawParameterValue(p + "RR");
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + "KS");
        params.fmOp[op].keyScaleLevel = 0;
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + "FIX") > 0.5f);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + "FREQ");
        params.fmOp[op].waveSelect = 0;
        params.fmOp[op].amEnable = false;
        params.fmOp[op].vibEnable = true;
        params.fmOp[op].egType = true;
        params.fmOp[op].waveSelect = (int)*apvts.getRawParameterValue(p + "WS");
        params.fmOp[op].mask = (*apvts.getRawParameterValue(p + "MASK") > 0.5f);
    }

    params.lfoFreq = *apvts.getRawParameterValue("OPZX3_LFO_FREQ");
    params.pms = (int)*apvts.getRawParameterValue("OPZX3_LFO_PMS");
    params.ams = (int)*apvts.getRawParameterValue("OPZX3_LFO_AMS");
}

void AudioPlugin2686V::processSsgBlock(SynthParams &params)
{
    // --- SSG Parameters ---
    params.ssgLevel = *apvts.getRawParameterValue("SSG_LEVEL");
    params.ssgNoiseLevel = *apvts.getRawParameterValue("SSG_NOISE");
    params.ssgMix = *apvts.getRawParameterValue("SSG_MIX");
    params.ssgNoiseFreq = *apvts.getRawParameterValue("SSG_NOISE_FREQ");
    params.ssgWaveform = (int)*apvts.getRawParameterValue("SSG_WAVEFORM");
    params.ssgBitDepth = (int)*apvts.getRawParameterValue("SSG_BIT");
    params.ssgRateIndex = (int)*apvts.getRawParameterValue("SSG_RATE");

    params.ssgUseHwEnv = (*apvts.getRawParameterValue("SSG_ENV_ENABLE") > 0.5f);
    params.ssgEnvShape = (int)*apvts.getRawParameterValue("SSG_ENV_SHAPE");
    params.ssgEnvPeriod = *apvts.getRawParameterValue("SSG_ENV_PERIOD");

    params.ssgDutyMode = (int)*apvts.getRawParameterValue("SSG_DUTY_MODE");
    params.ssgDutyPreset = (int)*apvts.getRawParameterValue("SSG_DUTY_PRESET");
    params.ssgDutyVar = *apvts.getRawParameterValue("SSG_DUTY_VAR");
    params.ssgDutyInvert = (*apvts.getRawParameterValue("SSG_DUTY_INV") > 0.5f);

    params.ssgTriKeyTrack = (*apvts.getRawParameterValue("SSG_TRI_KEYTRK") > 0.5f);
    params.ssgTriPeak = *apvts.getRawParameterValue("SSG_TRI_PEAK");
    params.ssgTriFreq = *apvts.getRawParameterValue("SSG_TRI_FREQ");

    params.ssgAdsrBypass = (*apvts.getRawParameterValue("SSG_ADSR_BYPASS") > 0.5f);
    params.ssgAdsr.a = *apvts.getRawParameterValue("SSG_AR");
    params.ssgAdsr.d = *apvts.getRawParameterValue("SSG_DR");
    params.ssgAdsr.s = *apvts.getRawParameterValue("SSG_SL");
    params.ssgAdsr.r = *apvts.getRawParameterValue("SSG_RR");
}

void AudioPlugin2686V::processWavetableBlock(SynthParams &params)
{
    params.wtBitDepth = (int)*apvts.getRawParameterValue("WT_BIT");
    params.wtRateIndex = (int)*apvts.getRawParameterValue("WT_RATE");
    params.wtTableSize = (int)*apvts.getRawParameterValue("WT_SIZE");
    params.wtWaveform = (int)*apvts.getRawParameterValue("WT_WAVE");

    // Custom Wave params reading
    for (int i = 0; i < 32; ++i)
    {
        auto paramId = "WT_CUSTOM32_" + juce::String(i);
        params.wtCustomWave32[i] = *apvts.getRawParameterValue(paramId);
    }


    // Custom Wave params reading
    for (int i = 0; i < 64; ++i)
    {
        auto paramId = "WT_CUSTOM64_" + juce::String(i);
        params.wtCustomWave64[i] = *apvts.getRawParameterValue(paramId);
    }

    params.wtModEnable = (*apvts.getRawParameterValue("WT_MOD_ENABLE") > 0.5f);
    params.wtModDepth = *apvts.getRawParameterValue("WT_MOD_DEPTH");
    params.wtModSpeed = *apvts.getRawParameterValue("WT_MOD_SPEED");
    params.wtLevel = *apvts.getRawParameterValue("WT_LEVEL");

    params.wtAdsr.a = *apvts.getRawParameterValue("WT_AR");
    params.wtAdsr.d = *apvts.getRawParameterValue("WT_DR");
    params.wtAdsr.s = *apvts.getRawParameterValue("WT_SL");
    params.wtAdsr.r = *apvts.getRawParameterValue("WT_RR");
}

void AudioPlugin2686V::processRhythmBlock(SynthParams &params)
{
    // --- Rhythm Parameters ---
    params.rhythmLevel = *apvts.getRawParameterValue("RHYTHM_LEVEL");

    for (int i = 0; i < 8; ++i) {
        juce::String prefix = "RHYTHM_PAD" + juce::String(i);
        auto& pad = params.rhythmPads[i];
        pad.level = *apvts.getRawParameterValue(prefix + "_VOL");
        pad.pan = *apvts.getRawParameterValue(prefix + "_PAN");
        pad.noteNumber = (int)*apvts.getRawParameterValue(prefix + "_NOTE");
        pad.qualityMode = (int)*apvts.getRawParameterValue(prefix + "_MODE");
        pad.rateIndex = (int)*apvts.getRawParameterValue(prefix + "_RATE");
        pad.isOneShot = (bool)*apvts.getRawParameterValue(prefix + "_ONESHOT");
        pad.release = *apvts.getRawParameterValue(prefix + "_RR");
    }
}

void AudioPlugin2686V::processAdpcmBlock(SynthParams &params)
{
    // get Bool value
    // getRawParameterValue は float* を返すので、0.5f以上かどうかで判定するのが通例
    params.adpcmLevel = *apvts.getRawParameterValue("ADPCM_LEVEL");
    params.adpcmPan = *apvts.getRawParameterValue("ADPCM_PAN");
    params.adpcmLoop = (*apvts.getRawParameterValue("ADPCM_LOOP") > 0.5f);
    params.adpcmQualityMode = (int)*apvts.getRawParameterValue("ADPCM_MODE");
    params.adpcmRateIndex = (int)*apvts.getRawParameterValue("ADPCM_RATE");

    params.adpcmAdsr.a = *apvts.getRawParameterValue("ADPCM_AR");
    params.adpcmAdsr.d = *apvts.getRawParameterValue("ADPCM_DR");
    params.adpcmAdsr.s = *apvts.getRawParameterValue("ADPCM_SL");
    params.adpcmAdsr.r = *apvts.getRawParameterValue("ADPCM_RR");
}

void AudioPlugin2686V::processFxBlock(juce::AudioBuffer<float>& buffer, SynthParams& params)
{
    if (*apvts.getRawParameterValue("FX_BYPASS") > 0.5f)
    {
        return;
    }

    // Vibrato
    bool vB = *apvts.getRawParameterValue("FX_VIB_BYPASS") > 0.5f;
    float vRate = *apvts.getRawParameterValue("FX_VIB_RATE");
    float vDepth = *apvts.getRawParameterValue("FX_VIB_DEPTH");
    float vMix = *apvts.getRawParameterValue("FX_VIB_MIX");
    effects.setVibratoParams(vRate, vDepth, vMix);

    // Tremolo
    bool tB = *apvts.getRawParameterValue("FX_TRM_BYPASS") > 0.5f;
    float tRate = *apvts.getRawParameterValue("FX_TRM_RATE");
    float tDepth = *apvts.getRawParameterValue("FX_TRM_DEPTH");
    float tMix = *apvts.getRawParameterValue("FX_TRM_MIX");
    effects.setTremoloParams(tRate, tDepth, tMix);

    // Modern Bit Crusher
    bool mcB = *apvts.getRawParameterValue("FX_MBC_BYPASS") > 0.5f;
    float mbcRate = *apvts.getRawParameterValue("FX_MBC_RATE");
    float mbcBits = *apvts.getRawParameterValue("FX_MBC_BITS");
    float mbcMix = *apvts.getRawParameterValue("FX_MBC_MIX");
    effects.setModernBitCrusherParams(mbcRate, mbcBits, mbcMix);

    // Delay
    bool dB = *apvts.getRawParameterValue("FX_DLY_BYPASS") > 0.5f;
    float dTime = *apvts.getRawParameterValue("FX_DLY_TIME");
    float dFb = *apvts.getRawParameterValue("FX_DLY_FB");
    float dMix = *apvts.getRawParameterValue("FX_DLY_MIX");
    effects.setDelayParams(dTime, dFb, dMix);

    // Reverb
    bool rB = *apvts.getRawParameterValue("FX_RVB_BYPASS") > 0.5f;
    float rSize = *apvts.getRawParameterValue("FX_RVB_SIZE");
    float rDamp = *apvts.getRawParameterValue("FX_RVB_DAMP");
    float rMix = *apvts.getRawParameterValue("FX_RVB_MIX");

    // Retro Bit Crusher
    bool rcB = *apvts.getRawParameterValue("FX_RBC_BYPASS") > 0.5f;
    int rbcRate = (int)*apvts.getRawParameterValue("FX_RBC_RATE");
    int rbcBits = (int)*apvts.getRawParameterValue("FX_RBC_BITS");
    float rbcMix = *apvts.getRawParameterValue("FX_RBC_MIX");
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
    juce::XmlElement xml("PREF_2686V");

    xml.setAttribute("wallpaperPath", wallpaperPath);
    xml.setAttribute("defaultSampleDir", defaultSampleDir);
    xml.setAttribute("defaultPresetDir", defaultPresetDir);
    xml.setAttribute("showTooltips", showTooltips);
    xml.setAttribute("useHeadRoom", useHeadroom);
    xml.setAttribute("headRoomGain", headroomGain);

    xml.writeTo(file);
}

// 環境設定を読み込み
void AudioPlugin2686V::loadEnvironment(const juce::File& file)
{
    juce::XmlDocument xmlDoc(file);
    std::unique_ptr<juce::XmlElement> xml = xmlDoc.getDocumentElement();

    if (xml.get() != nullptr && xml->hasTagName("PREF_2686V"))
    {
        wallpaperPath = xml->getStringAttribute("wallpaperPath");
        defaultSampleDir = xml->getStringAttribute("defaultSampleDir");
        defaultPresetDir = xml->getStringAttribute("defaultPresetDir");
        showTooltips = xml->getBoolAttribute("showTooltips", true);
        useHeadroom = xml->getBoolAttribute("useHeadRoom", true);
        headroomGain = xml->getDoubleAttribute("headRoomGain", 0.25f);

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
    auto presetFile = docDir.getChildFile("2686V").getChildFile("init_settings.xml");

    // 2. ファイルが存在するかチェック
    if (presetFile.existsAsFile())
    {
        loadEnvironment(presetFile);
    }
    else
    {
        DBG("Startup preset not found at: " + presetFile.getFullPathName());
    }

    // プリセットディレクトリ・ADPCMディレクトリが空の時は初期値を設定する
    if (defaultPresetDir.isEmpty())
    {
        auto newPresetDir = docDir.getChildFile("2686V").getChildFile("Presets");

        // 存在していなければ作成
        if (!newPresetDir.exists()) {
            newPresetDir.createDirectory();
        }

        defaultPresetDir = newPresetDir.getFullPathName();
    }

    if (defaultSampleDir.isEmpty())
    {
        auto newSampleDir = docDir.getChildFile("2686V").getChildFile("Samples");

        // 存在していなければ作成
        if (!newSampleDir.exists()) {
            newSampleDir.createDirectory();
        }

        defaultSampleDir = newSampleDir.getFullPathName();
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

    // 3. 制御文字の除去 (セキュリティ対策)
    // XMLパーサーを混乱させる可能性のある文字や、バイナリゴミを除去します
    // ただし、改行(\n, \r)とタブ(\t)はコメント用に許可します
    juce::String allowedControlChars = "\r\n\t";

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
    presetName = "Init Preset";
    presetAuthor = "User";
    presetVersion = "1.0";
    presetComment = "";

    // 3. サンプルのアンロードとパスクリア
    unloadAdpcmFile();
    // unloadAdpcmFile内で adpcmFilePath.clear() されています

    for (int i = 0; i < 8; ++i) {
        unloadRhythmFile(i);
        // unloadRhythmFile内で rhythmFilePaths[i].clear() されています
    }
}
