#pragma once
#include <JuceHeader.h>
#include <vector>
#include <cmath>
#include <array>
#include "Mode.h"

// --- YM2608 ADPCM Algorithm Implementation ---
namespace OpnaAdpcm {
    const int StepAdjustTable[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };
    const int StepSizeTable[49] = {
        16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66,
        73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 253,
        279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876,
        963, 1060, 1166, 1282, 1411, 1552
    };
}

class Ym2608AdpcmCodec {
private:
    int predictedValue = 0;
    int stepIndex = 0;

public:
    void reset() {
        predictedValue = 0;
        stepIndex = 0;
    }

    // Encode: 16bit PCM -> 4bit ADPCM
    uint8_t encode(int16_t pcmSample) {
        int step = OpnaAdpcm::StepSizeTable[stepIndex];
        int diff = pcmSample - predictedValue;
        uint8_t nibble = 0;

        if (diff < 0) {
            nibble = 8;
            diff = -diff;
        }

        int tempStep = step;
        if (diff >= tempStep) {
            nibble |= 4;
            diff -= tempStep;
        }
        tempStep >>= 1;
        if (diff >= tempStep) {
            nibble |= 2;
            diff -= tempStep;
        }
        tempStep >>= 1;
        if (diff >= tempStep) {
            nibble |= 1;
        }

        decodeAndUpdateState(nibble); // Update internal state
        return nibble;
    }

    // Decode: 4bit ADPCM -> 16bit PCM
    int16_t decode(uint8_t nibble) {
        return decodeAndUpdateState(nibble);
    }

private:
    int16_t decodeAndUpdateState(uint8_t nibble) {
        int step = OpnaAdpcm::StepSizeTable[stepIndex];
        int diff = step >> 3;

        if (nibble & 4) diff += step;
        if (nibble & 2) diff += (step >> 1);
        if (nibble & 1) diff += (step >> 2);

        if (nibble & 8) predictedValue -= diff;
        else            predictedValue += diff;

        if (predictedValue > 32767) predictedValue = 32767;
        else if (predictedValue < -32768) predictedValue = -32768;

        stepIndex += OpnaAdpcm::StepAdjustTable[nibble & 7];
        if (stepIndex < 0) stepIndex = 0;
        else if (stepIndex > 48) stepIndex = 48;

        return (int16_t)predictedValue;
    }
};

// --- Core Class ---

class AdpcmCore
{
public:
    AdpcmCore() {}

    void prepare(double sampleRate)
    {
        m_sampleRate = sampleRate;
        updateIncrements();
        // Generate demo waveform if empty
        if (m_pcmBuffer.empty()) generateDemoWaveform();
    }

    void setParameters(const SynthParams& params)
    {
        m_level = params.adpcmLevel;
        m_pan = params.adpcmPan;
        m_isLooping = params.adpcmLoop;
        m_adsr = params.adpcmAdsr;
        m_rootNote = params.adpcmRootNote;

        bool needRefresh = false;

        if (m_qualityMode != params.adpcmQualityMode) {
            m_qualityMode = params.adpcmQualityMode;
        }

        if (m_rateIndex != params.adpcmRateIndex) {
            m_rateIndex = params.adpcmRateIndex;
            needRefresh = true;
        }

        if (needRefresh) {
            refreshAdpcmBuffer();
        }

        updateIncrements();
    }

    // Set sample data from external source
    void setSampleData(const std::vector<float>& sourceData, double sourceRate)
    {
        // 1. Rawデータ (32bit float) をそのまま保持
        m_rawBuffer = sourceData;
        m_bufferSampleRate = sourceRate;

        // 2. ADPCMデータ (4bit emulation) も事前に作っておく
        // OPNAのレート(約16kHz)にリサンプリングして変換
        double targetRate = 16000.0;
        double step = sourceRate / targetRate;

        Ym2608AdpcmCodec codec;
        codec.reset();

        m_adpcmBuffer.clear();
        m_adpcmBuffer.reserve((size_t)(sourceData.size() / step));

        double pos = 0;
        while (pos < sourceData.size()) {
            int index = (int)pos;
            if (index >= sourceData.size()) break;

            int16_t input = (int16_t)(sourceData[index] * 32767.0f);

            // Encode -> Decode (劣化処理)
            uint8_t encoded = codec.encode(input);
            int16_t decoded = codec.decode(encoded);

            m_adpcmBuffer.push_back(decoded);
            pos += step;
        }
    }

    void noteOn(float frequency)
    {
        m_position = 0.0;

        float rootFreq = (float)juce::MidiMessage::getMidiNoteInHertz(m_rootNote);

        double currentBufferRate;

        if (m_qualityMode == 6) // ADPCM Mode (OPNA)
        {
            currentBufferRate = m_bufferSampleRate;
        }
        else // Raw, 24bit, 16bit, 8bit, 5bit, 4bit-Linear
        {
            currentBufferRate = m_sourceRate;
        }

        // ホストDAWのレートとの比率
        double rateRatio = currentBufferRate / m_sampleRate;

        m_pitchRatio = (frequency / rootFreq) * rateRatio;

        m_currentLevel = 0.0f;
        m_state = State::Attack;
        m_hasFinished = false;
    }

    void noteOff()
    {
        m_state = State::Release;
    }

    bool isPlaying() const { return m_state != State::Idle; }

    float getCurrentPan() const { return m_pan; }

    float getSample()
    {
        if (m_state == State::Idle || m_rawBuffer.empty()) return 0.0f;

        // Safety check for empty buffers
        if ((m_qualityMode == 7 && m_adpcmBuffer.empty()) ||
            (m_qualityMode != 7 && m_rawBuffer.empty())) {
            return 0.0f;
        }

        if (m_hasFinished) return 0.0f;

        processAdsr();

        float sample = 0.0f;

        // --- Mode Switching ---
        if (m_qualityMode == 7) // 4-bit ADPCM (OPNA Style)
        {
            // use ADPCM Buffer (Nearest Neighbor)
            if (m_position >= m_adpcmBuffer.size()) {
                if (m_isLooping) m_position = std::fmod(m_position, (double)m_adpcmBuffer.size());
                else { m_hasFinished = true; return 0.0f; }
            }

            int index = (int)m_position;
            if (index < m_adpcmBuffer.size()) {
                sample = m_adpcmBuffer[index] / 32768.0f;
            }
        }
        else // PCM Modes (Bit Crushing)
        {
            // use Raw Buffer
            if (m_position >= m_rawBuffer.size()) {
                if (m_isLooping) m_position = std::fmod(m_position, (double)m_rawBuffer.size());
                else { m_hasFinished = true; return 0.0f; }
            }

            // Linear Interpolation
            int idx0 = (int)m_position;
            int idx1 = idx0 + 1;
            if (idx1 >= m_rawBuffer.size()) idx1 = m_isLooping ? 0 : idx0;

            float frac = (float)(m_position - idx0);
            float s0 = m_rawBuffer[idx0];
            float s1 = m_rawBuffer[idx1];
            sample = s0 * (1.0f - frac) + s1 * frac;

            float maxVal = 0.0f;
            switch (m_qualityMode)
            {
                case 1: maxVal = 0.0f; break;       // ADD: Raw 32-bit (No crush)
                case 2: maxVal = 8388607.0f; break; // 24bit
                case 3: maxVal = 32767.0f; break;   // 16bit
                case 4: maxVal = 127.0f; break;     // 8bit
                case 5: maxVal = 15.0f; break;      // 5bit
                case 6: maxVal = 7.0f; break;       // 4bit Linear
            }

            if (maxVal > 0.0f) {
                sample = std::floor(sample * maxVal) / maxVal;
            }
        }

        m_position += m_pitchRatio;

        return sample * m_level * m_currentLevel;
    }

private:
    void refreshAdpcmBuffer()
    {
        if (m_rawBuffer.empty()) return;

        double targetRate = 16000.0;
        switch (m_rateIndex) {
        case 1: targetRate = 96000.0; break;
        case 2: targetRate = 55500.0; break; // OPNA Limit
        case 3: targetRate = 48000.0; break;
        case 4: targetRate = 44100.0; break; // CD
        case 5: targetRate = 22050.0; break; // Half
        case 6: targetRate = 16000.0; break; // PC-98 Standard
        case 7: targetRate = 8000.0;  break; // Telephone
        default: targetRate = 16000.0; break;
        }

        // Do not upsample beyond source rate for the ADPCM buffer gen
        if (targetRate > m_sourceRate) targetRate = m_sourceRate;

        m_bufferSampleRate = targetRate;

        // Resample & Encode
        double step = m_sourceRate / targetRate;
        if (step <= 0.0) step = 1.0;

        Ym2608AdpcmCodec codec;
        codec.reset();

        m_adpcmBuffer.clear();
        m_adpcmBuffer.reserve((size_t)(m_rawBuffer.size() / step) + 1);

        double pos = 0;
        while (pos < m_rawBuffer.size()) {
            int index = (int)pos;
            if (index >= m_rawBuffer.size()) break;

            int16_t input = (int16_t)(m_rawBuffer[index] * 32767.0f);

            m_adpcmBuffer.push_back(codec.decode(codec.encode(input)));

            pos += step;
        }
    }

    void processAdsr()
    {
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
        else if (m_state == State::Sustain) {
            m_currentLevel = m_adsr.s;
        }
        else if (m_state == State::Release) {
            m_currentLevel -= m_releaseDec;
            if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
        }
    }

    void updateIncrements() {
        if (m_sampleRate <= 0.0) return;
        m_attackInc = 1.0f / (float)(std::max(0.001f, m_adsr.a) * m_sampleRate);
        m_decayDec = 1.0f / (float)(std::max(0.001f, m_adsr.d) * m_sampleRate);
        m_releaseDec = 1.0f / (float)(std::max(0.001f, m_adsr.r) * m_sampleRate);
    }

    // デモ用：ノコギリ波を生成してADPCM変換しておく
    void generateDemoWaveform()
    {
        std::vector<float> tempSource;
        int length = 16000; // 1秒分 (16kHz想定)
        for (int i = 0; i < length; ++i) {
            float t = (float)i / length;
            // 減衰するノコギリ波 (ドラムっぽい音)
            float raw = (1.0f - (2.0f * (float)(i % 100) / 100.0f)) * (1.0f - t);
            tempSource.push_back(raw);
        }
        setSampleData(tempSource, 16000.0);
    }

    enum class State { Idle, Attack, Decay, Sustain, Release };
    State m_state = State::Idle;
    double m_sampleRate = 44100.0; // DAW Host Sample Rate
    double m_sourceRate = 44100.0;
    double m_bufferSampleRate = 16000.0; // Internal Data Sample Rate

    // Processed ADPCM Data (stored as int16 for playback)
    std::vector<int16_t> m_pcmBuffer;
    std::vector<float> m_rawBuffer;       // Raw Data (32bit)
    std::vector<int16_t> m_adpcmBuffer;   // Processed Data (4bit ADPCM)
    int m_qualityMode = 6;
    int m_rateIndex = 3;

    double m_position = 0.0;
    float m_pitchRatio = 1.0f;
    int m_rootNote = 60; // Middle C

    // Params
    float m_level = 1.0f;
    float m_pan = 0.5f;
    SimpleAdsr m_adsr;
    float m_currentLevel = 0.0f;
    float m_attackInc = 0.0f; float m_decayDec = 0.0f; float m_releaseDec = 0.0f;

    bool m_isLooping = false;
    bool m_hasFinished = false;
};
