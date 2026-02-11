#pragma once
#include <JuceHeader.h>
#include <vector>
#include <cmath>
#include "Mode.h"

class WavetableCore
{
public:
    WavetableCore()
    {
        // 初期波形: サイン波
        m_sourceWave.resize(64);
        generateWaveform(0); // Default Sine
    }

    void prepare(double sampleRate)
    {
        if (sampleRate > 0.0) m_sampleRate = sampleRate;
        updateIncrements();
        updatePhaseDelta();
    }

    void setParameters(const SynthParams& params)
    {
        m_level = params.wtLevel;
        m_adsr = params.wtAdsr;

        // Bit Depth & Table Size
        switch (params.wtBitDepth) {
        case 0: m_quantizeSteps = 15.0f; break;
        case 1: m_quantizeSteps = 31.0f; break;
        case 2: m_quantizeSteps = 63.0f; break;
        case 3: m_quantizeSteps = 255.0f; break;
        case 4: m_quantizeSteps = 0.0f; break; // Raw (No Quantize) - 追加の可能性を考慮
        default: m_quantizeSteps = 255.0f; break;
        }
        m_rateIndex = params.wtRateIndex;
        m_tableSize = (params.wtTableSize == 0) ? 32 : 64;

        // 波形変更検知
        // カスタム波形データのコピー (値が変わっていれば更新するため)
        if (params.wtWaveform == 8) // 8 = Custom
        {
            // パラメータ配列をメンバに保存
            m_customWaveCache32 = params.wtCustomWave32;
            m_customWaveCache64 = params.wtCustomWave64;
            // 強制的に再生成
            generateWaveform(8);
        }
        else if (m_waveform != params.wtWaveform)
        {
            m_waveform = params.wtWaveform;
            generateWaveform(m_waveform);
        }

        m_modEnable = params.wtModEnable;
        m_modDepth = params.wtModDepth;
        m_modSpeed = params.wtModSpeed;

        updateIncrements();
    }

    void noteOn(float frequency)
    {
        juce::Logger::getCurrentLogger()->writeToLog("Rate:" + juce::String(m_rateIndex));
        m_phase = 0.0f;
        m_modPhase = 0.0f;
        m_phaseDelta = frequency / m_sampleRate;

        m_currentLevel = 0.0f;
        m_state = State::Attack;
        m_rateAccumulator = 1.0f; // Force update on first sample
        m_lastSample = 0.0f;
    }

    void noteOff()
    {
        m_state = State::Release;
    }

    bool isPlaying() const { return m_state != State::Idle; }

    // ピッチベンド (0 - 16383, Center=8192)
    void setPitchBend(int pitchWheelValue)
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
    void setModulationWheel(int wheelValue)
    {
        // 0.0 ～ 1.0 に正規化
        m_modWheel = (float)wheelValue / 127.0f;
    }
    void setPitchBendRatio(float ratio) { m_pitchBendRatio = ratio; }

    float getSample()
    {
        if (m_state == State::Idle) return 0.0f;

        // --- ADSR Logic ---
        if (m_state == State::Attack) {
            m_currentLevel += m_attackInc;
            if (m_currentLevel >= 1.0f) { m_currentLevel = 1.0f; m_state = State::Decay; }
        }
        else if (m_state == State::Decay) {
            if (m_currentLevel > m_adsr.s) {
                m_currentLevel -= m_decayDec;
                if (m_currentLevel <= m_adsr.s) { m_currentLevel = m_adsr.s; m_state = State::Sustain; }
            }
            else { m_currentLevel = m_adsr.s; m_state = State::Sustain; }
        }
        else if (m_state == State::Release) {
            m_currentLevel -= m_releaseDec;
            if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
        }

        // --- Sample Rate Emulation ---
        double targetRate = getTargetRate();
        double step = targetRate / m_sampleRate;
        m_rateAccumulator += step;

        // --- Wavetable Synthesis ---
        if (m_rateAccumulator >= 1.0)
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
                m_modPhase += (m_phaseDelta * m_modSpeed);
                if (m_modPhase >= 1.0f) m_modPhase -= 1.0f;
            }

            // 2. Phase
            // Apply Pitch Bend to Phase Delta
            float currentDelta = m_phaseDelta * m_pitchBendRatio;

            // Apply Modulation to Phase (Vibrato effect) or Read Index?
            // Here we apply to Phase for Vibrato-like effect on table lookup
            float effectivePhase = m_phase + modOffset;
            effectivePhase -= std::floor(effectivePhase);
            if (effectivePhase < 0.0f) effectivePhase += 1.0f;

            // 3. Table Lookup
            float readIndex = effectivePhase * (float)m_tableSize;
            int idx = (int)readIndex;
            if (idx >= m_tableSize) idx = m_tableSize - 1;

            int sourceIdx = (m_tableSize == 32) ? (idx * 2) : idx;
            if (sourceIdx >= 64) sourceIdx = 63;

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
private:
    // 波形データ生成
    void generateWaveform(int type)
    {
        const int N = 64;
        const double PI = juce::MathConstants<double>::pi;

        for (int i = 0; i < N; ++i)
        {
            double phase = (double)i / (double)N; // 0.0 to 1.0
            float sample = 0.0f;

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
            case 8: // Custom
                {
				if (m_tableSize == 64)
                {
                    // 64サンプルをそのまま使う
                    sample = m_customWaveCache64[i];
                }
                else
                {
                    // 64サンプルを32ステップで埋める (index / 2)
                    int step = i / 2;
                    if (step >= 32) step = 31;
                        sample = m_customWaveCache32[step];
                    }
                }
                break;
            }
            m_sourceWave[i] = sample;
        }
    }

    void updateIncrements() {
        if (m_sampleRate <= 0.0) return;
        m_attackInc = 1.0f / (float)(std::max(0.001f, m_adsr.a) * m_sampleRate);
        m_decayDec = 1.0f / (float)(std::max(0.001f, m_adsr.d) * m_sampleRate);
        m_releaseDec = 1.0f / (float)(std::max(0.001f, m_adsr.r) * m_sampleRate);
    }

    double getTargetRate() const {
        // Internal Index (1-7)
        switch (m_rateIndex) {
            case 1: return 96000.0;
            case 2: return 55500.0;
            case 3: return 48000.0;
            case 4: return 44100.0;
            case 5: return 22050.0;
            case 6: return 16000.0;
            case 7: return 8000.0;
            default: return 16000.0;
        }
    }

    void updatePhaseDelta() {
        double targetRate = getTargetRate();
        if (targetRate > 0.0) {
            m_phaseDelta = m_currentFrequency / targetRate;
        }
    }

    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0;

    SimpleAdsr m_adsr;
    float m_level = 1.0f;

    // Wave Data
    std::vector<float> m_sourceWave; // Internal High-Res (Length 64)
    int m_tableSize = 32;            // Playback Size (32 or 64)
    float m_quantizeSteps = 15.0f;   // 4bit=15
    int m_waveform = -1; // for initialize
    std::array<float, 32> m_customWaveCache32; // for data storage
    std::array<float, 64> m_customWaveCache64; // for data storage

    // Rate / Lo-Fi
    int m_rateIndex = 5; // Default ID 6 (16kHz) -> Index 5
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;
    float m_currentFrequency = 440.0f;

    // Modulation
    bool m_modEnable = false;
    float m_modDepth = 0.0f;
    float m_modSpeed = 1.0f;
    float m_modPhase = 0.0f;

    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_currentLevel = 0.0f;
    float m_attackInc = 0.0f;
    float m_decayDec = 0.0f;
    float m_releaseDec = 0.0f;

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;
};
