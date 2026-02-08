#include "PluginProcessor.h"
#include "PluginEditor.h"

// ============================================================================
// コンストラクタ
// ============================================================================
AudioPlugin2686V::AudioPlugin2686V()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    // APVTSの初期化 (ここでパラメータを作ります)
    apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    m_synth.addSound(new SynthSound());

    for (int i = 0; i < 32; i++)
    {
        m_synth.addVoice(new SynthVoice());
    }
}

// ============================================================================
// デストラクタ
// ============================================================================
AudioPlugin2686V::~AudioPlugin2686V() {}

// ============================================================================
// パラメータレイアウトの定義 (ここにあるものがDAWやGUIから見えます)
// ============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout AudioPlugin2686V::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Mode: 0:OPNA, 1:OPN, 2:OPL, 3:SSG, 4:WAVETABLE 5:RHYTHM, 6:ADPCM
    layout.add(std::make_unique<juce::AudioParameterInt>("MODE", "Mode", 0, 6, 0));

	createOpnaParameterLayout(layout);
    createOpnParameterLayout(layout);
    createOplParameterLayout(layout);
    createSsgParameterLayout(layout);
    createWavetableParameterLayout(layout);
    createRhythmParameterLayout(layout);
    createAdpcmParameterLayout(layout);

    return layout;
}

// ============================================================================
// Prepare To Play (再生準備)
// ============================================================================
void AudioPlugin2686V::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    m_synth.setCurrentPlaybackSampleRate(sampleRate);
}

// ============================================================================
// Release Resources
// ============================================================================
void AudioPlugin2686V::releaseResources()
{
    // メモリ解放などはJUCEのSynthesiserクラスが自動で行うため、
    // 基本的にここは空で大丈夫です
}

// ============================================================================
// Process Block (オーディオ処理のメインループ)
// ============================================================================
void AudioPlugin2686V::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    buffer.clear();

    SynthParams params;

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

    // 各ボイスへ適用
    for (int i = 0; i < m_synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(m_synth.getVoice(i)))
        {
            voice->setParameters(params);
        }
    }

    m_synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

// ============================================================================
// Editor (GUI) 関連
// ============================================================================
juce::AudioProcessorEditor* AudioPlugin2686V::createEditor()
{
    // Genericエディタではなく、自作のエディタを使うように変更
    return new AudioPlugin2686VEditor(*this);
}

bool AudioPlugin2686V::hasEditor() const { return true; }

// パラメータ・設定関連
const juce::String AudioPlugin2686V::getName() const { return "2686V"; }
bool AudioPlugin2686V::acceptsMidi() const { return true; }
bool AudioPlugin2686V::producesMidi() const { return false; }
bool AudioPlugin2686V::isMidiEffect() const { return false; }
double AudioPlugin2686V::getTailLengthSeconds() const { return 0.0; }
int AudioPlugin2686V::getNumPrograms() { return 1; }
int AudioPlugin2686V::getCurrentProgram() { return 0; }
void AudioPlugin2686V::setCurrentProgram(int index) {}
const juce::String AudioPlugin2686V::getProgramName(int index) { return {}; }
void AudioPlugin2686V::changeProgramName(int index, const juce::String& newName) {}

// ============================================================================
// 状態保存 (State Information)
// ============================================================================
void AudioPlugin2686V::getStateInformation(juce::MemoryBlock& destData) {
    // 将来的にプリセット保存処理をここに書きます
}

void AudioPlugin2686V::setStateInformation(const void* data, int sizeInBytes) {
    // 将来的にプリセット読み込み処理をここに書きます
}

void AudioPlugin2686V::addEnvParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_AR", prefix + " Env", 0.0f, 5.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_DR", prefix + " Env", 0.0f, 5.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_SL", prefix + " Env", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "_RR", prefix + " Env", 0.0f, 5.0f, 0.0f));
}

void AudioPlugin2686V::createOpnaParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // 1. OPNA (YM2608) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPNA_FEEDBACK", "OPNA Feedback", 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>("OPNA_ALGORITHM", "OPNA Algorithm", 0, 7, 7));

    for (int op = 0; op < MaxFmOperators; ++op)
    {
        juce::String prefix = "OPNA_OP" + juce::String(op) + "_";
        juce::String namePrefix = "OPNA OP" + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "MULTI", namePrefix + "MULTI", 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "DT", namePrefix + "DT", 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "AR", namePrefix + "AR", 0.0f, 5.0f, 0.01f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "DR", namePrefix + "DR", 0.0f, 5.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SR", namePrefix + "SR", 0.0f, 10.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SL", namePrefix + "SL", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "RR", namePrefix + "RR", 0.0f, 5.0f, 0.2f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "SE", namePrefix + "SE", 0, 15, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "TL", namePrefix + " TL", 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "KS", namePrefix + " KS", 0, 3, 0)); // KS (0 to 3)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "AM", namePrefix + " AM", false)); // AM Enable (Switch)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "FIX", namePrefix + "Fixed Mode", false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "FREQ", namePrefix + "Fixed Freq", juce::NormalisableRange<float>(10.0f, 8000.0f, 0.0f, 0.3f), 440.0f));
    }

    layout.add(std::make_unique<juce::AudioParameterFloat>("OPNA_LFO_FREQ", "LFO Freq", 0.1f, 50.0f, 5.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>("OPNA_LFO_PMS", "LFO PMS", 0, 7, 0));
    layout.add(std::make_unique<juce::AudioParameterInt>("OPNA_LFO_AMS", "LFO AMS", 0, 3, 0));
}

void AudioPlugin2686V::createOpnParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // 2. OPN (YM2203) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPN_FEEDBACK", "OPN Feedback", 0.0f, 7.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterInt>("OPN_ALGORITHM", "OPN Algorithm", 0, 7, 7));

    for (int op = 0; op < MaxFmOperators; ++op)
    {
        juce::String prefix = "OPN_OP" + juce::String(op) + "_";
        juce::String namePrefix = "OPN OP" + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "MULTI", namePrefix + "MULTI", 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "DT", namePrefix + "DT", 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "AR", namePrefix + "AR", 0.0f, 5.0f, 0.01f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "DR", namePrefix + "DR", 0.0f, 5.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SR", namePrefix + "SR", 0.0f, 10.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SL", namePrefix + "SL", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "RR", namePrefix + "RR", 0.0f, 5.0f, 0.2f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "SE", namePrefix + "SE", 0, 15, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "TL", namePrefix + " TL", 0.0f, 1.0f, 0.0f)); // TL (0.0 to 1.0)
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "KS", namePrefix + " KS", 0, 3, 0)); // KS (0 to 3)
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "FIX", namePrefix + "Fixed Mode", false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "FREQ", namePrefix + "Fixed Freq", juce::NormalisableRange<float>(10.0f, 8000.0f, 0.0f, 0.3f), 440.0f));
    }
}

void AudioPlugin2686V::createOplParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // 3. OPL (YM3526) Parameters
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterInt>("OPL_ALGORITHM", "OPL Algorithm", 0, 1, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("OPL_FEEDBACK", "OPL Feedback", 0.0f, 7.0f, 0.0f));

    for (int op = 0; op < 2; ++op)
    {
        juce::String prefix = "OPL_OP" + juce::String(op) + "_";
        juce::String namePrefix = "OPL OP" + juce::String(op + 1) + " ";

        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "MULTI", namePrefix + "MULTI", 0, 15, 1));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "DT", namePrefix + "DT", 0, 7, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "AR", namePrefix + "AR", 0.0f, 5.0f, 0.01f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "DR", namePrefix + "DR", 0.0f, 5.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "SL", namePrefix + "SL", 0.0f, 1.0f, 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "RR", namePrefix + "RR", 0.0f, 5.0f, 0.2f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "WS", namePrefix + "Wave", 0, 3, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(prefix + "TL", namePrefix + " TL", 0.0f, 1.0f, 0.0f));
        layout.add(std::make_unique<juce::AudioParameterInt>(prefix + "KS", namePrefix + " KS", 0, 3, 0));
        layout.add(std::make_unique<juce::AudioParameterBool>(prefix + "AM", namePrefix + " AM", false));
    }
}

void AudioPlugin2686V::createSsgParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // 4. SSG
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_LEVEL", "SSG Tone", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_NOISE", "SSG Noise", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_MIX", "SSG Tone/Noise Mix", 0.0f, 1.0f, 0.5f)); // デフォルトは 0.5 (Mix)
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_NOISE_FREQ", "Noise Freq", juce::NormalisableRange<float>(50.0f, 20000.0f, 1.0f, 0.3f), 12000.0f)); // Noise Frequency Default: 12000Hz (Standard PSG feel)

    layout.add(std::make_unique<juce::AudioParameterInt>("SSG_WAVEFORM", "SSG Waveform", 0, 1, 0)); // 0:Pulse, 1:Triangle

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>("SSG_ADSR_BYPASS", "SSG ADSR Bypass", false));

	addEnvParameters(layout, "SSG");

    layout.add(std::make_unique<juce::AudioParameterInt>("SSG_DUTY_MODE", "Duty Mode", 0, 1, 0)); // Duty Mode: 0=Preset, 1=Variable
    layout.add(std::make_unique<juce::AudioParameterInt>("SSG_DUTY_PRESET", "Duty Preset", 0, 8, 0)); // Preset: 0~8
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_DUTY_VAR", "Duty Var", 0.0f, 0.5f, 0.5f)); // Variable: 0.0 ~ 0.5
    layout.add(std::make_unique<juce::AudioParameterBool>("SSG_DUTY_INV", "Duty Invert", false)); // Invert

    layout.add(std::make_unique<juce::AudioParameterBool>("SSG_TRI_KEYTRK", "Triangle Key Track", true)); // Key Track Switch (Default ON)
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_TRI_PEAK", "Triangle Peak", 0.0f, 1.0f, 0.5f)); // Triangle Peak: 0.0 ~ 1.0
    // Triangle Manual Frequency
    // Range: 0.1Hz - 8000Hz, Skew: 0.3 (Log的挙動), Default: 440Hz
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_TRI_FREQ", "Tri Manual Freq", juce::NormalisableRange<float>(0.1f, 8000.0f, 0.1f, 0.3f), 440.0f));

    // Hardware Envelope Parameters
    layout.add(std::make_unique<juce::AudioParameterBool>("SSG_ENV_ENABLE", "SSG HW Env Enable", false)); // HW Env Enable(Bool)
    layout.add(std::make_unique<juce::AudioParameterInt>("SSG_ENV_SHAPE", "SSG Env Shape", 0, 7, 0)); // Shape: 0-7
    // 実機のPeriodは値が大きいほど遅いですが、スライダーは右に行くほど速い方が直感的なためHzにします
    layout.add(std::make_unique<juce::AudioParameterFloat>("SSG_ENV_PERIOD", "SSG Env Speed", 0.1f, 50.0f, 1.0f)); // Period: ここでは周波数(Hz)として扱います (0.1Hz ~ 50Hz)
}

void AudioPlugin2686V::createWavetableParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit
    layout.add(std::make_unique<juce::AudioParameterInt>("WT_BIT", "WT Bit Depth", 0, 3, 3));
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
    // 5. Rhythm
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>("RHYTHM_LEVEL", "Rhythm Vol", 0.0f, 1.0f, 1.0f));
}

void AudioPlugin2686V::createAdpcmParameterLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    // ==========================================
    // 6. ADPCM
    // ==========================================
    layout.add(std::make_unique<juce::AudioParameterFloat>("ADPCM_LEVEL", "ADPCM Level", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("ADPCM_LOOP", "ADPCM Loop", true));
    addEnvParameters(layout, "ADPCM");
}

void AudioPlugin2686V::processOpnaBlock(SynthParams &params)
{
    params.feedback = *apvts.getRawParameterValue("OPNA_FEEDBACK");
    params.algorithm = (int)*apvts.getRawParameterValue("OPNA_ALGORITHM");

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = "OPNA_OP" + juce::String(op) + "_";

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + "MULTI");
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + "DT");
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + "AR");
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + "DR");
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + "SL");
        params.fmOp[op].release = *apvts.getRawParameterValue(p + "RR");
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + "KS");
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + "TL");
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + "SR");
        params.fmOp[op].ssgEg = (int)*apvts.getRawParameterValue(p + "SE");
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + "FIX") > 0.5f);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + "FREQ");
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = (bool)*apvts.getRawParameterValue(p + "AM");
    }

    params.lfoFreq = *apvts.getRawParameterValue("OPNA_LFO_FREQ");
    params.pms = (int)*apvts.getRawParameterValue("OPNA_LFO_PMS");
    params.ams = (int)*apvts.getRawParameterValue("OPNA_LFO_AMS");
}

void AudioPlugin2686V::processOpnBlock(SynthParams &params)
{
    params.feedback = *apvts.getRawParameterValue("OPN_FEEDBACK");
    params.algorithm = (int)*apvts.getRawParameterValue("OPN_ALGORITHM");

    for (int op = 0; op < 4; ++op)
    {
        juce::String p = "OPN_OP" + juce::String(op) + "_";

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + "MULTI");
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + "DT");
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + "AR");
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + "DR");
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + "SL");
        params.fmOp[op].release = *apvts.getRawParameterValue(p + "RR");
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + "KS");
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + "TL");
        params.fmOp[op].sustainRate = *apvts.getRawParameterValue(p + "SR");
        params.fmOp[op].ssgEg = 0;
        params.fmOp[op].fixedMode = (*apvts.getRawParameterValue(p + "FIX") > 0.5f);
        params.fmOp[op].fixedFreq = *apvts.getRawParameterValue(p + "FREQ");
        params.fmOp[op].waveSelect = 0; // Sine
        params.fmOp[op].amEnable = false; // OPNにはAMは無い
    }
}

void AudioPlugin2686V::processOplBlock(SynthParams &params)
{
    params.feedback = *apvts.getRawParameterValue("OPL_FEEDBACK");
    params.algorithm = (int)*apvts.getRawParameterValue("OPL_ALGORITHM");

    for (int op = 0; op < 2; ++op)
    {
        juce::String p = "OPL_OP" + juce::String(op) + "_";

        params.fmOp[op].multiple = (int)*apvts.getRawParameterValue(p + "MULTI");
        params.fmOp[op].detune = (int)*apvts.getRawParameterValue(p + "DT");
        params.fmOp[op].attack = *apvts.getRawParameterValue(p + "AR");
        params.fmOp[op].decay = *apvts.getRawParameterValue(p + "DR");
        params.fmOp[op].sustain = *apvts.getRawParameterValue(p + "SL");
        params.fmOp[op].release = *apvts.getRawParameterValue(p + "RR");
        params.fmOp[op].keyScale = (int)*apvts.getRawParameterValue(p + "KS");
        params.fmOp[op].totalLevel = *apvts.getRawParameterValue(p + "TL");
        params.fmOp[op].waveSelect = (int)*apvts.getRawParameterValue(p + "WS");
        params.fmOp[op].sustainRate = 0.0f;
        params.fmOp[op].ssgEg = 0; // OPLにはSSG-EGは無い
        params.fmOp[op].fixedMode = false; // OPLにはFixed Modeは無い
        params.fmOp[op].fixedFreq = 0.0f;
        params.fmOp[op].amEnable = false; // OPLにはAMは無い
    }
}

void AudioPlugin2686V::processSsgBlock(SynthParams &params)
{
    // --- SSG Parameters ---
    params.ssgLevel = *apvts.getRawParameterValue("SSG_LEVEL");
    params.ssgNoiseLevel = *apvts.getRawParameterValue("SSG_NOISE");
    params.ssgMix = *apvts.getRawParameterValue("SSG_MIX");
    params.ssgNoiseFreq = *apvts.getRawParameterValue("SSG_NOISE_FREQ");
    params.ssgWaveform = (int)*apvts.getRawParameterValue("SSG_WAVEFORM");

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
}

void AudioPlugin2686V::processAdpcmBlock(SynthParams &params)
{
    // Bool値の取得
    // getRawParameterValue は float* を返すので、0.5f以上かどうかで判定するのが通例
    params.adpcmLoop = (*apvts.getRawParameterValue("ADPCM_LOOP") > 0.5f);

    // --- ADPCM Parameters ---
    params.adpcmLevel = *apvts.getRawParameterValue("ADPCM_LEVEL");
    params.adpcmLoop = (*apvts.getRawParameterValue("ADPCM_LOOP") > 0.5f);
    params.adpcmAdsr.a = *apvts.getRawParameterValue("ADPCM_AR");
    params.adpcmAdsr.d = *apvts.getRawParameterValue("ADPCM_DR");
    params.adpcmAdsr.s = *apvts.getRawParameterValue("ADPCM_SL");
    params.adpcmAdsr.r = *apvts.getRawParameterValue("ADPCM_RR");
}

// ============================================================================
// エントリーポイント
// ============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPlugin2686V();
}
