#include "PluginProcessor.h"
#include <limits>
#include <algorithm>
#include <cmath>
#include <set>

#include "../Processor/ProcessorNames.h"
#include "../Processor/ProcessorHelper.h"
#include "../../Gui/Settings/SettingsKeys.h"
#include "../../Gui/Settings/SettingsValues.h"

#include "../Gui/GuiValues.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat settingsFormat{ "settings", 1 };
	const Io::ParamFormat presetFormat{ "preset", 1 };

	// 読み込んだ音声ファイルが、そのまま扱える大きさかどうか。
	// createReaderFor は「形式が分かった」ことしか言わない。チャンネル数 0 や、
	// int に収まらない長さがそのまま来ることがある。AudioBuffer::setSize は
	// 負の大きさを、getReadPointer(0) は無いチャンネルを、どちらも黙って受け取る。
	inline bool isLoadableAudio(const juce::AudioFormatReader& reader)
	{
		return reader.numChannels > 0
			&& reader.lengthInSamples > 0
			&& reader.lengthInSamples <= (juce::int64)std::numeric_limits<int>::max();
	}

	// 環境設定を書き出す側。項目の型ごとに受け口を分けてある。
	struct EnvironmentWriter
	{
		Io::ParamWriter& writer;

		void operator()(const juce::String& key, int value) { writer.set(key, value); }
		void operator()(const juce::String& key, bool value) { writer.set(key, value); }
		void operator()(const juce::String& key, float value) { writer.set(key, value); }
		void operator()(const juce::String& key, const juce::String& value) { writer.set(key, value); }
	};

	// 読み込む側。書かれていない項目は今の値のまま残す。
	struct EnvironmentReader
	{
		const Io::ParamReader& reader;

		void operator()(const juce::String& key, int& value) { value = reader.getInt(key, value); }
		void operator()(const juce::String& key, bool& value) { value = reader.getBool(key, value); }
		void operator()(const juce::String& key, float& value) { value = reader.getFloat(key, value); }
		void operator()(const juce::String& key, juce::String& value) { value = reader.getString(key, value); }
	};
}

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
    prMap[OscMode::SSG] = &prSsg;
    prMap[OscMode::RHYTHM] = &prRhythm;
    prMap[OscMode::ADPCM] = &prAdpcm;

    pMode = apvts.getRawParameterValue(CPK::mode);
    pMonoMode = apvts.getRawParameterValue(CPK::Midi::monoMode);
    pUseVelocity = apvts.getRawParameterValue(CPK::Midi::useVelocity);
    pPitchResetOnLegato = apvts.getRawParameterValue(CPK::Midi::pitchResetOnLegato);
    pFixedVelocity = apvts.getRawParameterValue(CPK::Midi::fixedVelocity);

    prOpna.init(apvts, modWaveSlots);
    prSsg.init(apvts, modWaveSlots);
	prRhythm.init(apvts, modWaveSlots);
    prAdpcm.init(apvts, modWaveSlots);
    prFx.init(apvts);

    m_synth.addSound(new SynthSound());
    for (int i = 0; i < Global::totalVoices; i++) {
        auto voice = new SynthVoice();

        voice->prepare(44100.0);
        m_synth.addVoice(voice);
    }

    prFx.prepare(44100.0);

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
    layout.add(std::make_unique<juce::AudioParameterInt>(CPK::mode, CPN::mode, 0, CoreGuiValue::TabNumber, 0));

    prOpna.createLayout(layout);
	prSsg.createLayout(layout);
    prRhythm.createLayout(layout);
	prAdpcm.createLayout(layout);
	prFx.createLayout(layout);

    layout.add(std::make_unique<juce::AudioParameterBool>(
        CPK::Midi::monoMode,
        CPN::Midi::monoMode,
        CPV::Midi::MonoMode::initial
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        CPK::Midi::useVelocity,
        CPN::Midi::useVelocity,
        CPV::Midi::UseVelocity::initial
    ));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        CPK::Midi::pitchResetOnLegato,
        CPN::Midi::pitchResetOnLegato,
        CPV::Midi::PitchResetOnLegato::initial
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        CPK::Midi::fixedVelocity,
        CPN::Midi::fixedVelocity,
        CPV::Midi::FixedVelocity::min,
        CPV::Midi::FixedVelocity::max,
        CPV::Midi::FixedVelocity::initial
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

    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    m_synth.currentParams = &m_currentParams;

    // 【シンセモード】
    // 入力バッファはノイズの原因になるのでクリアする
    buffer.clear();

    // --- Global ---
    int m = PrHelper::getInt(pMode);

    // 持っていないチャンネルの番号が来ることがある。チャンネルを全部持つ
    // プラグインで作ったプリセットを読み込んだときで、MODE の上限はタブの
    // 数なので、チャンネルの少ないプラグインでは通ってしまう。
    //
    // そのまま引くと、prMap が無いキーへ空のポインタを挿し込んで返す。
    // 辿った先で落ちるので、知らない番号は先頭のチャンネルへ戻す。
    if (m < 0 || m >= (int)OscMode::Count) m = 0;

    m_currentParams.mode = (OscMode)m;

    // map を [] で引くと、無いキーのときに空のポインタを挿し込んでしまう。
    // 上で丸めてあるので届かないはずだが、辿る前に確かめておく。
    auto found = prMap.find(m_currentParams.mode);

    if (found == prMap.end() || found->second == nullptr) return;

    found->second->processBlock(m_currentParams, apvts);

    // ADPCM の素材。新しく符号化されたものが出ていれば持ち替える。
    m_adpcmPcm.acquireForAudio();
    m_currentParams.adpcm.source = &m_adpcmPcm.forAudio();

    if (m_currentParams.mode == OscMode::ADPCM)
    {
        const int q = m_currentParams.adpcm.quality.mode;
        const int r = m_currentParams.adpcm.quality.rate;

        // 符号化は素材まるごとを舐める上に中で確保する。ここでやると音が途切れる
        // ので、指定だけ置いてメッセージスレッドへ頼む。出来上がるまでは
        // 前の符号化で鳴らし続ける (数ミリ秒遅れて切り替わる)。
        if (m_adpcmPcm.needsRebuild(q, r)) {
            m_adpcmWantQuality.store(q, std::memory_order_relaxed);
            m_adpcmWantRate.store(r, std::memory_order_relaxed);

            triggerAsyncUpdate();
        }
    }

    bool isMono = PrHelper::getBool(pMonoMode);

    m_synth.isMonoMode = isMono;
    m_currentParams.monoMode = isMono;

    bool useVelo = PrHelper::getBool(pUseVelocity);

    m_synth.useVelocity = useVelo;
    m_currentParams.useVelocity = useVelo;

    bool ptResetOnLegato = PrHelper::getBool(pPitchResetOnLegato);

    m_synth.pitchResetOnLegato = ptResetOnLegato;
    m_currentParams.pitchResetOnLegato = ptResetOnLegato;

    float fixedVelocity = PrHelper::getFloat(pFixedVelocity);

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
    // 画面の鍵盤ぶんを混ぜたので、入ってきたときより数が増えている。
    // MIDI は出さない宣言なので、ここで片付けておかないと、受け皿が
    // 「送るつもりの音が捨てられている」と見なして止まる。
    midiMessages.clear();
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
        std::unique_ptr<juce::AudioFormatReader> audioReader(reader);

        if (!isLoadableAudio(*audioReader)) return;

        adpcmFilePath = file.getFullPathName();

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

        // 素材はここで 1 つだけ持ち、ボイスへは指させるだけにする。
        // 以前は全ボイスへ丸ごと複製し、その場で符号化までしていた。
        m_adpcmPcm.setSource(sourceData, audioReader->sampleRate);
        m_adpcmPcm.rebuildIfNeeded(m_adpcmWantQuality.load(std::memory_order_relaxed),
                                   m_adpcmWantRate.load(std::memory_order_relaxed),
                                   16000.0);

        // 画面表示用の控え
        adpcmPreviewBuffer = std::move(sourceData);
        adpcmPreviewRate = audioReader->sampleRate;
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

        if (!isLoadableAudio(*audioReader)) return;

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

        // 画面表示用の控え
        if (padIndex >= 0 && padIndex < RhythmPrValue::pads) {
            rhythmPreviewBuffers[padIndex] = std::move(sourceData);
            rhythmPreviewRates[padIndex] = audioReader->sampleRate;
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

// WT PITCH MOD (HuC6280 モード) の変調波形を読み込む。
//
// 実機の波形メモリは 32 サンプルなので、そこへ落としてから持つ。
// 落とし方は Smooth の設定で変わる。
//   Smooth ON  : 区間平均してから元のピークへ正規化する。
//                32 点のナイキストより上の成分が低い次数へフルの振幅で
//                折り返すのを抑えつつ、変調の振れ幅は保つ。
//   Smooth OFF : 単純間引き。元波形の値をそのまま拾う。
void AudioPlugin2686V::loadWtModWaveFile(const juce::String& code, int slot, const juce::File& file)
{
    if (slot < 0 || slot >= Global::WtMod::slots) return;

    juce::StringArray lines;
    file.readLines(lines);

    if (lines.size() == 0) return;

    int sampleCount = lines[0].trim().getIntValue();

    if (sampleCount != 32 && sampleCount != 64 && sampleCount != 128 && sampleCount != 256) return;

    std::vector<float> values((size_t)sampleCount, 0.0f);

    if (file.getFileExtension().equalsIgnoreCase(".wt2")) {
        // .wt2 は 2 行目が解像度、3 行目以降が 0〜(解像度-1) の整数。
        // 中央を 0 として -1.0〜1.0 へ正規化する。
        int resolution = (lines.size() > 1) ? lines[1].trim().getIntValue() : 0;

        if (resolution != 16 && resolution != 32 && resolution != 64
            && resolution != 128 && resolution != 256) {
            return;
        }

        float center = (float)(resolution >> 1);

        for (int i = 0; i < sampleCount; ++i) {
            if (i + 2 < lines.size()) {
                int raw = std::clamp(lines[i + 2].getIntValue(), 0, resolution - 1);

                values[(size_t)i] = std::clamp(((float)raw - center) / center, -1.0f, 1.0f);
            }
        }
    }
    else {
        // .wt は 2 行目以降が -1.0〜1.0 の実数
        for (int i = 0; i < sampleCount; ++i) {
            if (i + 1 < lines.size()) {
                values[(size_t)i] = std::clamp(lines[i + 1].getFloatValue(), -1.0f, 1.0f);
            }
        }
    }

    const int step = sampleCount / Global::WtMod::waveSize;

    bool smooth = false;

    if (auto* p = apvts.getRawParameterValue(code + CPK::WtMod::waveSmooth)) {
        smooth = p->load() > 0.5f;
    }

    std::array<float, Global::WtMod::waveSize> reduced = { 0.0f };

    if (smooth && step > 1) {
        float srcPeak = 0.0f;

        for (float v : values) srcPeak = std::max(srcPeak, std::fabs(v));

        float dstPeak = 0.0f;

        for (int i = 0; i < Global::WtMod::waveSize; ++i) {
            float sum = 0.0f;

            for (int k = 0; k < step; ++k) sum += values[(size_t)(i * step + k)];

            reduced[(size_t)i] = sum / (float)step;
            dstPeak = std::max(dstPeak, std::fabs(reduced[(size_t)i]));
        }

        // 平均でなまったぶんのピークを戻す (無音の波形はそのまま)
        if (srcPeak > 1.0e-6f && dstPeak > 1.0e-6f) {
            float gain = srcPeak / dstPeak;

            for (auto& v : reduced) v = std::clamp(v * gain, -1.0f, 1.0f);
        }
    }
    else {
        for (int i = 0; i < Global::WtMod::waveSize; ++i) reduced[(size_t)i] = values[(size_t)(i * step)];
    }

    // 音源コアはこの配列を直接読むので、差し替えの瞬間だけ処理を止める
    suspendProcessing(true);

    auto& slots = modWaveSlots[code];

    slots[(size_t)slot].data = reduced;
    slots[(size_t)slot].hasData = true;

    suspendProcessing(false);

    modWavePaths[code][(size_t)slot] = file.getFullPathName();
}

void AudioPlugin2686V::unloadWtModWaveFile(const juce::String& code, int slot)
{
    if (slot < 0 || slot >= Global::WtMod::slots) return;

    suspendProcessing(true);

    auto& slots = modWaveSlots[code];

    slots[(size_t)slot].data.fill(0.0f);
    slots[(size_t)slot].hasData = false;

    suspendProcessing(false);

    modWavePaths[code][(size_t)slot].clear();
}

void AudioPlugin2686V::setPresetToXml(std::unique_ptr<juce::XmlElement>& xml)
{
    // セーブ時にAPVTSから現在のModeを確実に取得して同期させる
    int currentMode = PrHelper::getInt(pMode);

    if (currentMode >= 0 && currentMode <= (int)OscMode::ADPCM) {
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


    // チャンネルごとの MODULATION 変調波形パス
    // 属性名の末尾にスロット番号を付ける
    for (const auto& kv : modWavePaths) {
        for (int i = 0; i < Global::WtMod::slots; ++i) {
            if (kv.second[i].isEmpty()) continue;

            xml->setAttribute(PresetKey::modWavePathPrefix + kv.first + "_" + juce::String(i),
                makeWtPathRelative(juce::File(kv.second[i])));
        }
    }

    // サンプルパス保存 (RHYTHM)
    for (int i = 0; i < RhythmPrValue::pads; ++i) {
        xml->setAttribute(PresetKey::rhythmPathPrefix + juce::String(i), makePathRelative(juce::File(rhythmFilePaths[i])));
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


        // チャンネルごとの WT PITCH MOD 変調波形。
        // エディタが開いていないと map は空なので、属性側から拾って作る。
        // 実データはファイルから読み直す。
        modWavePaths.clear();

        // modWaveSlots は音源側へ要素のポインタを配ってあるので、
        // 要素そのものを消してはいけない。中身だけ空にする。
        for (auto& kv : modWaveSlots) {
            for (auto& slot : kv.second) {
                slot.data.fill(0.0f);
                slot.hasData = false;
            }
        }

        for (int i = 0; i < xmlState->getNumAttributes(); ++i) {
            const juce::String& name = xmlState->getAttributeName(i);

            if (!name.startsWith(PresetKey::modWavePathPrefix)) continue;

            // 「プレフィックス + コード + _ + スロット番号」から後ろ 2 つを外す
            juce::String tail = name.substring(PresetKey::modWavePathPrefix.length());
            int sep = tail.lastIndexOfChar('_');

            if (sep < 0) continue;

            juce::String code = tail.substring(0, sep);
            int slot = tail.substring(sep + 1).getIntValue();

            if (slot < 0 || slot >= Global::WtMod::slots) continue;

            juce::String stored = xmlState->getAttributeValue(i);

            if (stored.isEmpty()) continue;

            juce::File file = resolveWtPath(stored);

            if (file.existsAsFile()) loadWtModWaveFile(code, slot, file);
        }

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

    // 使われていない項目を落としてから書く
    removeUnknownParams(*xml);

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

    // 使われていない項目を落としてから書く
    removeUnknownParams(*xml);

    setPresetToXml(xml);

    // 中身の組み立ては今までどおり。書き出す形だけを名前式にする。
    Io::ParamWriter writer(presetFormat);

    Io::writeStateXml(writer, *xml);

    writer.writeTo(file);
}

void AudioPlugin2686V::loadPreset(const juce::File& file)
{
    // 3.0.0 より前のプリセットは XML。作り溜めたものが読めなくなると困るので、
    // 読み込みだけは残してある。書き出しは新しい形式だけ。
    if (auto reader = Io::ParamReader::open(file, presetFormat, false))
    {
        auto xmlState = Io::readStateXml(*reader, apvts.state.getType().toString());

        getPresetFromXml(xmlState);

        return;
    }

    juce::XmlDocument xmlDoc(file);
    std::unique_ptr<juce::XmlElement> xmlState = xmlDoc.getDocumentElement();

    getPresetFromXml(xmlState);
}

// 環境設定を保存
bool AudioPlugin2686V::saveEnvironment(const juce::File& file)
{
    Io::ParamWriter writer(settingsFormat);

    EnvironmentWriter visit{ writer };

    visitEnvironment(visit);

    return writer.writeTo(file);
}

// 環境設定を読み込み
bool AudioPlugin2686V::loadEnvironment(const juce::File& file, bool tellIfLegacy)
{
    auto reader = Io::ParamReader::open(file, settingsFormat, tellIfLegacy);

    if (!reader.has_value()) return false;

    EnvironmentReader visit{ *reader };

    visitEnvironment(visit);

    // ファイルから来た値は画面の制限を通っていない。音に直に掛かるものと
    // 表を引くものだけ、ここで妥当な範囲へ丸める。
    // headroomGain は processBlock で buffer.applyGain に渡るので、
    // 桁違いの値や NaN が入ると爆音や NaN 汚染になる。
    if (std::isfinite(headroomGain)) {
        headroomGain = std::clamp(headroomGain, 0.0f, 1.0f);
    }
    else {
        headroomGain = SettingsValue::Initial::headroomGain;
    }

    // 読んだ番号を書き出し先へ映す
    applyFileFormat();

    // 内部変数の更新
    if (juce::File(defaultSampleDir).isDirectory()) {
        lastSampleDirectory = juce::File(defaultSampleDir);
    }

    return true;
}

// オーディオスレッドから頼まれた符号化を、ここ (メッセージスレッド) で行う。
void AudioPlugin2686V::handleAsyncUpdate()
{
    m_adpcmPcm.rebuildIfNeeded(m_adpcmWantQuality.load(std::memory_order_relaxed),
                               m_adpcmWantRate.load(std::memory_order_relaxed),
                               16000.0);
}

void AudioPlugin2686V::loadStartupSettings()
{
    // 1. 読み込むディレクトリとファイル名を指定
    // 例: マイドキュメントフォルダ内の "2686V" フォルダにある "init_preset_vl.xml"
    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    auto pluginDir = docDir.getChildFile(Io::Folder::asset);
    auto presetFile = getStartupSettingsFile();

    if (!pluginDir.exists()) {
        pluginDir.createDirectory();
    }

    bool loadSuccess = false;

    // 2. ファイルが存在するかチェック
    if (presetFile.existsAsFile())
    {
        // 読めるかどうかは読み込み側が判断する。ここで形を決め打ちすると、
        // 形式を変えたときに正しいファイルまで壊れていると見なしてしまう。
        //
        // 読めなくても消さない。こちらから開いたわけではないので黙って
        // 見送り、初期値で立ち上げる。
        loadSuccess = loadEnvironment(presetFile, false);
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

    if (defaultChannelParamDir.isEmpty() || !juce::File(defaultChannelParamDir).isDirectory())
    {
        auto newChannelParamDir = pluginDir.getChildFile(Io::Folder::channelParam);

        // 存在していなければ作成
        if (!newChannelParamDir.exists()) {
            newChannelParamDir.createDirectory();
        }

        defaultChannelParamDir = newChannelParamDir.getFullPathName();
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

    if (defaultSsgHwEnvParamDir.isEmpty() || !juce::File(defaultSsgHwEnvParamDir).isDirectory())
    {
        auto newSsgHwEnvParamDir = pluginDir.getChildFile(Io::Folder::ssgHwEnvParam);

        // 存在していなければ作成
        if (!newSsgHwEnvParamDir.exists()) {
            newSsgHwEnvParamDir.createDirectory();
        }

        defaultSsgHwEnvParamDir = newSsgHwEnvParamDir.getFullPathName();
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

    if (defaultColorSettingDir.isEmpty() || !juce::File(defaultColorSettingDir).isDirectory())
    {
        auto newColorSettingDir = pluginDir.getChildFile(Io::Folder::colorSetting);

        // 存在していなければ作成
        if (!newColorSettingDir.exists()) {
            newColorSettingDir.createDirectory();
        }

        defaultColorSettingDir = newColorSettingDir.getFullPathName();
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

    // 画面表示用の控えも捨てる
    adpcmPreviewBuffer.clear();

    m_adpcmPcm.clear();
}

void AudioPlugin2686V::unloadRhythmFile(int padIndex)
{
    // インデックスチェック
    if (padIndex < 0 || padIndex >= 8) return;

    // パス情報を削除
    rhythmFilePaths[padIndex].clear();

    // 画面表示用の控えも捨てる
    if (padIndex < RhythmPrValue::pads) rhythmPreviewBuffers[padIndex].clear();

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
    int m = PrHelper::getInt(pMode);

    return (OscMode)m;
}

void AudioPlugin2686V::removeUnknownParams(juce::XmlElement& xml) const
{
    // 7,000 を超えるので、端から順に見比べると保存のたびに数千万回の
    // 文字列比較になる。引ける形に入れておく。
    std::set<juce::String> known;

    for (auto* param : getParameters())
    {
        if (auto* withId = dynamic_cast<const juce::AudioProcessorParameterWithID*>(param))
        {
            known.insert(withId->paramID);
        }
    }

    juce::Array<juce::XmlElement*> doomed;

    for (auto* child : xml.getChildWithTagNameIterator(Io::StateKey::param))
    {
        if (known.find(child->getStringAttribute(Io::StateKey::id)) == known.end())
        {
            doomed.add(child);
        }
    }

    // 回している最中に消さない
    for (auto* child : doomed) xml.removeChildElement(child, true);
}
