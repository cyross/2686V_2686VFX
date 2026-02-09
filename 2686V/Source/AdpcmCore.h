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
        // 初期状態としてデモ波形を生成 (本来はファイルをロードする)
        if (m_pcmBuffer.empty()) generateDemoWaveform();
    }

    void setParameters(const SynthParams& params)
    {
        m_level = params.adpcmLevel;
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

    // 外部から音声データをセットする関数 (Processor側から呼ぶ)
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

    float getSample()
    {
        if (m_state == State::Idle || m_rawBuffer.empty()) return 0.0f;
        if (m_hasFinished) return 0.0f;

        processAdsr();

        float sample = 0.0f;

        // --- Mode Switching ---
        if (m_qualityMode == 6) // 4-bit ADPCM (OPNA Style)
        {
            // ADPCMバッファを使用 (Nearest Neighbor)
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
            // Rawバッファを使用
            if (m_position >= m_rawBuffer.size()) {
                if (m_isLooping) m_position = std::fmod(m_position, (double)m_rawBuffer.size());
                else { m_hasFinished = true; return 0.0f; }
            }

            // 線形補間 (Linear Interpolation)
            int idx0 = (int)m_position;
            int idx1 = idx0 + 1;
            if (idx1 >= m_rawBuffer.size()) idx1 = m_isLooping ? 0 : idx0;

            float frac = (float)(m_position - idx0);
            float s0 = m_rawBuffer[idx0];
            float s1 = m_rawBuffer[idx1];
            sample = s0 * (1.0f - frac) + s1 * frac;

            // ★修正: ビット深度シミュレーション
            switch (m_qualityMode)
            {
                case 0: // Raw (32-bit Float) - 何もしない
                    break;

                case 1: // 24-bit PCM
                {
                    // 2^23 - 1 = 8388607
                    // 24bitは精度が高すぎるため聴感上の差はほぼありませんが、理論値として実装します
                    const float maxVal = 8388607.0f;
                    sample = std::floor(sample * maxVal) / maxVal;
                    break;
                }
                case 2: // 16-bit PCM (CD Quality)
                {
                    const float maxVal = 32767.0f;
                    sample = std::floor(sample * maxVal) / maxVal;
                    break;
                }
                case 3: // 8-bit PCM (Retro PC / Sampler)
                {
                    const float maxVal = 127.0f;
                    sample = std::floor(sample * maxVal) / maxVal;
                    break;
                }
                case 4: // 5-bit PCM (PC Engine Wavetable style)
                {
                    // 2^4 - 1 = 15 (Signed 5-bit: -16 to +15 range approximation)
                    // かなり階段状になり、独特の「ジー」という倍音が乗ります
                    const float maxVal = 15.0f;
                    sample = std::floor(sample * maxVal) / maxVal;
                    break;
                }
                case 5: // 4-bit Linear PCM
                {
                    // 2^3 - 1 = 7 (Signed 4-bit: -8 to +7 range)
                    // ADPCMではない、純粋なビット落としです。激しいノイズが乗ります。
                    const float maxVal = 7.0f;
                    sample = std::floor(sample * maxVal) / maxVal;
                    break;
                }
            }
        }

        m_position += m_pitchRatio;

        return sample * m_level * m_currentLevel;
    }

private:
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

    void refreshAdpcmBuffer()
    {
        if (m_rawBuffer.empty()) return;

        // レートの決定
        double targetRate = 16000.0;
        switch (m_rateIndex) {
            case 0: targetRate = 55500.0; break; // OPNA Limit
            case 1: targetRate = 44100.0; break; // CD
            case 2: targetRate = 22050.0; break; // Half
            case 3: targetRate = 16000.0; break; // PC-98 Standard
            case 4: targetRate = 8000.0;  break; // Telephone
            default: targetRate = 16000.0; break;
        }

        // 元データよりも高いレートを指定された場合は、元データのレートを上限とする（アップサンプリングは無意味なので）
        if (targetRate > m_sourceRate) targetRate = m_sourceRate;

        // 再生用変数の更新
        m_bufferSampleRate = targetRate;

        // リサンプリング & エンコード開始
        double step = m_sourceRate / targetRate;

        Ym2608AdpcmCodec codec;
        codec.reset();

        m_adpcmBuffer.clear();
        // 予想サイズ確保
        m_adpcmBuffer.reserve((size_t)(m_rawBuffer.size() / step) + 1);

        double pos = 0;
        while (pos < m_rawBuffer.size()) {
            int index = (int)pos;
            if (index >= m_rawBuffer.size()) break;

            // Rawバッファから取得
            float rawSample = m_rawBuffer[index];
            int16_t input = (int16_t)(rawSample * 32767.0f);

            // Encode -> Decode (ADPCM劣化)
            uint8_t encoded = codec.encode(input);
            int16_t decoded = codec.decode(encoded);

            m_adpcmBuffer.push_back(decoded);

            pos += step;
        }
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
    int m_qualityMode = 0;
    int m_rateIndex = 3;

    double m_position = 0.0;
    float m_pitchRatio = 1.0f;
    int m_rootNote = 60; // Middle C

    // Params
    float m_level = 1.0f;
    SimpleAdsr m_adsr;
    float m_currentLevel = 0.0f;
    float m_attackInc = 0.0f; float m_decayDec = 0.0f; float m_releaseDec = 0.0f;

    bool m_isLooping = false;
    bool m_hasFinished = false;
};
