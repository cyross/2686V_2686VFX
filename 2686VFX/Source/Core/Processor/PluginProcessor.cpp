#include "PluginProcessor.h"
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
        // エフェクトなので、入ってきた音を加工する。入力が無いと
        // ホストは音を渡してくれず、何も鳴らないままになる。
        .withInput(Global::Audio::input, juce::AudioChannelSet::stereo(), true)
        .withOutput(Global::Audio::output, juce::AudioChannelSet::stereo(), true)
    ),
    // Initialize APVTS (Parameters are created here)
    apvts(*this, &undoManager, Global::Plugin::parameters, createParameterLayout()) // APVTSの初期化
#endif
{

    pMode = apvts.getRawParameterValue(CPK::mode);
    pMonoMode = apvts.getRawParameterValue(CPK::Midi::monoMode);
    pUseVelocity = apvts.getRawParameterValue(CPK::Midi::useVelocity);
    pPitchResetOnLegato = apvts.getRawParameterValue(CPK::Midi::pitchResetOnLegato);
    pFixedVelocity = apvts.getRawParameterValue(CPK::Midi::fixedVelocity);

    prFx.init(apvts);
    prFx.prepare(44100.0);

    prMod.init(apvts);
    prMod.prepare(44100.0);

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
    layout.add(std::make_unique<juce::AudioParameterInt>(CPK::mode, CPN::mode, 0, CoreGuiValue::TabNumber, 0));


	prFx.createLayout(layout);
	prMod.createLayout(layout);

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
    juce::ignoreUnused(samplesPerBlock);

    prFx.prepare(sampleRate);
    prMod.prepare(sampleRate);
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
// 入ってきた音を FX へ通す。
//
// 音源のプラグインと違い、ここでは音を作らない。入力をそのまま加工する
// ので、バッファを消してはいけない。MIDI は受け取るが、鳴らすためではなく、
// 鍵盤の押し離しでエンベロープや LFO を動かすために使う。
// 入力と出力は同じ並びだけを受ける。モノラルとステレオに対応する。
// 数が食い違うと、加工した先を書き戻せない。
bool AudioPlugin2686V::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo()) return false;

    return layouts.getMainInputChannelSet() == out;
}

void AudioPlugin2686V::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // 画面の鍵盤から入った音も混ぜる
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    // 鍵盤の押し離しで変調を動かす。音を鳴らすためではないので、
    // どの音程かは見ない。
    for (const auto meta : midiMessages)
    {
        const auto message = meta.getMessage();

        if (message.isNoteOn()) prMod.noteOn();
        else if (message.isNoteOff() || message.isAllNotesOff()) prMod.noteOff();
    }

    // 使わない出力は消しておく。入力より出力が多いときに、前の中身が残る。
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
    {
        buffer.clear(ch, 0, buffer.getNumSamples());
    }

    // 加工前を控えておく。FX を通したあとと並べて見せるため。
    // 位置は加工後と共用するので、ここでは進めない。
    if (previewVisiblity && buffer.getNumChannels() >= 2)
    {
        auto* dryL = buffer.getReadPointer(0);
        auto* dryR = buffer.getReadPointer(1);

        int pos = realTimeWritePos.load(std::memory_order_relaxed);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            dryBufferL[pos] = dryL[i];
            dryBufferR[pos] = dryR[i];
            dryBufferMono[pos] = (dryL[i] + dryR[i]) * 0.5f;

            pos++;

            if (pos >= ringBufferSize) pos = 0;
        }
    }

    if (useHeadroom)
    {
        buffer.applyGain(headroomGain);
    }

    // 変調は FX より前へ掛ける。音量の動きも FX に通したいため。
    prMod.processBlock(buffer, apvts);

    prFx.processBlock(buffer, m_currentParams, apvts);

    if (previewVisiblity && buffer.getNumChannels() >= 2)
    {
        auto* finalOutL = buffer.getReadPointer(0);
        auto* finalOutR = buffer.getReadPointer(1);
        int numSamples = buffer.getNumSamples();

        int pos = realTimeWritePos.load(std::memory_order_relaxed);

        for (int i = 0; i < numSamples; ++i) {
            float l = finalOutL[i];
            float r = finalOutR[i];

            realTimeBufferL[pos] = l;
            realTimeBufferR[pos] = r;
            realTimeBufferMono[pos] = (l + r) * 0.5f;

            pos++;

            if (pos >= ringBufferSize) pos = 0;
        }

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

// Function to load Rhythm file

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


    // チャンネルごとの MODULATION 変調波形パス
    // 属性名の末尾にスロット番号を付ける
    for (const auto& kv : modWavePaths) {
        for (int i = 0; i < Global::WtMod::slots; ++i) {
            if (kv.second[i].isEmpty()) continue;

            xml->setAttribute(PresetKey::modWavePathPrefix + kv.first + "_" + juce::String(i),
                makeWtPathRelative(juce::File(kv.second[i])));
        }
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

    // 読んだ番号を書き出し先へ映す
    applyFileFormat();

    // 内部変数の更新
    if (juce::File(defaultSampleDir).isDirectory()) {
        lastSampleDirectory = juce::File(defaultSampleDir);
    }

    return true;
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

}
// 波形のプレビュー。
//
// 音源のプラグインでは、実際に音を作って線を引いていた。エフェクトには
// 作る音が無いので、加工前と加工後を並べて見せる形にする。作りは別に
// 用意するので、ここでは何も返さない。
void AudioPlugin2686V::generatePreviewWaveform(std::vector<float>* destBuffer)
{
    juce::ignoreUnused(destBuffer);
}
// 鳴りっぱなしを止める。音を作っていないので、FX の中身を洗うだけ。
void AudioPlugin2686V::panic()
{
    keyboardState.allNotesOff(0);

}
void AudioPlugin2686V::resetMidiSettings()
{
    keyboardState.allNotesOff(0);
}

std::vector<int> AudioPlugin2686V::getFxOrder() {
    return prFx.getOrder();
}

void AudioPlugin2686V::updateFxOrder(std::vector<int> newOrder)
{
    prFx.updateOrder(newOrder);
}
// 音を作っていないので、鳴っているかどうかは持たない
bool AudioPlugin2686V::isPlaying()
{
    return false;
}
bool AudioPlugin2686V::isMidiProcessing()
{
    return false;
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
