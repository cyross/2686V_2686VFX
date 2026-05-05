#include "./Core.h"

#include "../../Core/Synth/SynthHelpers.h"

WtCore::WtCore() : SynthCore()
{
    // 初期波形: サイン波
    m_sourceWave.resize(256);
    m_tableSizes = { 32, 64, 128, 256 };
    m_customWaves = { m_customWaveCache32.data(), m_customWaveCache64.data(), m_customWaveCache128.data(), m_customWaveCache256.data() };
    generateWaveform(0); // Default Sine
}

void WtCore::prepare(double sampleRate)
{
    if (sampleRate > 0.0) m_sampleRate = sampleRate;
	m_adsr.prepare(sampleRate);
    m_pitchAdsr.prepare(m_sampleRate);
    updatePhaseDelta();
}

void WtCore::setParameters(const SynthParams& params)
{
    m_level = params.wt.level;

    m_adsr.setParameters(params.wt.adsr);
    m_pitchAdsr.setParameters(params.wt.pitchAdsr);

    // Bit Depth & Table Size
    m_quantizeSteps = getTargetBitDepth(params.wt.bitDepth);
    m_rateIndex = params.wt.rateIndex;

    // 波形・テーブルサイズ変更検知
    int newTableSize = m_tableSizes[params.wt.tableSize];
    bool waveformChanged = (m_waveform != params.wt.waveform);
    bool sizeChanged = (m_prevTableSize != newTableSize);

    m_tableSizeIndex = params.wt.tableSize;
    m_tableSize = newTableSize;
    m_prevTableSize = newTableSize;
    m_waveform = params.wt.waveform;

    // 波形変更検知
    // カスタム波形データのコピー (値が変わっていれば更新するため)
    if (params.wt.waveform == 8) // 8 = Custom
    {
        // パラメータ配列をメンバに保存
        m_customWaveCache32 = params.wt.customWave32;
        m_customWaveCache64 = params.wt.customWave64;
        m_customWaveCache128 = params.wt.customWave128;
        m_customWaveCache256 = params.wt.customWave256;
        // 強制的に再生成
        generateWaveform(8);
    }
    else if (waveformChanged || sizeChanged)
    {
        m_waveform = params.wt.waveform;

        generateWaveform(m_waveform);
    }

    m_modEnable = params.wt.modEnable;
    m_modDepth = params.wt.modDepth;
    m_modSpeed = params.wt.modSpeed;
}

void WtCore::noteOn(float freq, float velocity, int midiNote)
{
    m_phase = 0.0f;
    m_modPhase = 0.0f;

    double targetRate = getTargetRate(m_rateIndex);
    if (targetRate > 0.0) {
        m_phaseDelta = freq / targetRate;
    }
    else {
        m_phaseDelta = 0.0;
    }

    m_currentLevel = 0.0f;
    m_rateAccumulator = 1.0f; // Force update on first sample
    m_lastSample = 0.0f;

    m_adsr.noteOn();
	m_pitchAdsr.noteOn();
}

void WtCore::noteOff()
{
    m_adsr.noteOff();
	m_pitchAdsr.noteOff();
}

bool WtCore::isPlaying() const
{
    return m_adsr.isPlaying() || m_pitchAdsr.isPlaying();
}

// ピッチベンド (0 - 16383, Center=8192)
void WtCore::setPitchBend(int pitchWheelValue)
{
    // 範囲を -1.0 ～ 1.0 に正規化
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;

    // 半音単位のレンジ (例: +/- 2半音)
    float semitones = 2.0f;

    // 比率計算: 2^(semitones / 12)
    // norm * semitones で変化量を決定
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);

    setPitchBendRatio(ratio);
}

// モジュレーションホイール (0 - 127)
void WtCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

void WtCore::setPitchBendRatio(float ratio)
{
    m_pitchBendRatio = ratio;
}

float WtCore::getSample()
{
    if (!m_adsr.isBypassed())
    {
        m_currentLevel = m_adsr.process(m_currentLevel);
    }
    else
    {
        m_currentLevel = m_adsr.isRelease() ? 0.0f : 1.0f;
    }

    float newPhaseDelta = m_pitchAdsr.process(m_phaseDelta);

    // --- Sample Rate Emulation ---
    double targetRate = getTargetRate(m_rateIndex);
    double step = targetRate / m_sampleRate;
    m_rateAccumulator += step;

    // --- Wavetable Synthesis ---
    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        // --- Synthesis at Target Rate ---

        // 1. Modulation (Vibrato / Table Scan)
        // LFO for modulation
        float modLfoVal = std::sin(m_modPhase * 2.0 * juce::MathConstants<float>::pi);

        // Base Depth + Wheel Depth
        // Default mod depth from params, add wheel influence (max +0.1)
        float totalModDepth = m_modDepth + (m_modWheel * 0.1f);

        float modOffset = 0.0f;
        if (m_modEnable || m_modWheel > 0.0f) // Apply if enable OR wheel is up
        {
            modOffset = modLfoVal * totalModDepth;

            // Advance Mod LFO
            // Speed depends on note frequency (Ratio) or Fixed? 
            // Using m_modSpeed as ratio to Note Freq
            m_modPhase += (newPhaseDelta * m_modSpeed);
            if (m_modPhase >= 1.0f) m_modPhase -= 1.0f;
        }

        // 2. Phase
        // Apply Pitch Bend to Phase Delta
        float currentDelta = newPhaseDelta * m_pitchBendRatio;

        // Apply Modulation to Phase (Vibrato effect) or Read Index?
        // Here we apply to Phase for Vibrato-like effect on table lookup
        float effectivePhase = m_phase + modOffset;
        effectivePhase -= std::floor(effectivePhase);
        if (effectivePhase < 0.0f) effectivePhase += 1.0f;

        // 3. Table Lookup
        float readIndex = effectivePhase * (float)m_tableSize;
        int idx = (int)readIndex;
        if (idx >= m_tableSize) idx = m_tableSize - 1;

        int sourceIdx = idx * (256 / m_tableSize);
        if (sourceIdx >= 256) sourceIdx = 255;

        float rawSample = m_sourceWave[sourceIdx];

        // 4. Quantize
        if (m_quantizeSteps > 0.0f) {
            float norm = (rawSample + 1.0f) * 0.5f;
            float quantized = std::floor(norm * m_quantizeSteps) / m_quantizeSteps;
            m_lastSample = (quantized * 2.0f) - 1.0f;
        }
        else {
            m_lastSample = rawSample;
        }

        // Advance Main Phase
        m_phase += currentDelta;
        if (m_phase >= 1.0f) m_phase -= 1.0f;
    }

    return m_lastSample * m_currentLevel * m_level * 0.5f;
}

// 波形データ生成
void WtCore::generateWaveform(int type)
{
    const int N = 256;
    const double PI = juce::MathConstants<double>::pi;

    for (int i = 0; i < N; ++i)
    {
        double phase = (double)i / (double)N; // 0.0 to 1.0
        float sample = 0.0f;

        if (type == 8) // custom
        {
            sample = m_customWaves[m_tableSizeIndex][i / (256 / m_tableSize)];
        }
        else
        {
            switch (type)
            {
            case 0: // Sine
                sample = (float)std::sin(2.0 * PI * phase);
                break;
            case 1: // Triangle
                sample = (float)(phase < 0.5 ? (-1.0 + 4.0 * phase) : (3.0 - 4.0 * phase));
                break;
            case 2: // Saw Up
                sample = (float)(-1.0 + 2.0 * phase);
                break;
            case 3: // Saw Down
                sample = (float)(1.0 - 2.0 * phase);
                break;
            case 4: // Square (50%)
                sample = (phase < 0.5) ? 1.0f : -1.0f;
                break;
            case 5: // Pulse 25%
                sample = (phase < 0.25) ? 1.0f : -1.0f;
                break;
            case 6: // Pulse 12.5%
                sample = (phase < 0.125) ? 1.0f : -1.0f;
                break;
            case 7: // Noise (Pseudo-random but fixed cycle for "Digital" feel)
                sample = ((float)(i * 12345 % 100) / 50.0f) - 1.0f;
                break;
            break;
            }
        }

        m_sourceWave[i] = sample;
    }
}

void WtCore::updatePhaseDelta()
{
    double targetRate = getTargetRate(m_rateIndex);
    if (targetRate > 0.0) {
        m_phaseDelta = m_currentFrequency / targetRate;
    }
}

void WtCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    outL[startSample + sampleIdx] += sample;
    outR[startSample + sampleIdx] += sample;

    isActive = isPlaying();
}
