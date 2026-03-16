#include "AdpcmCore.h"

#include <vector>
#include <cmath>
#include <array>

#include "../synth/Mode.h"
#include "../synth/Pcm.h"
#include "../synth/SynthHelpers.h"

void AdpcmCore::prepare(double sampleRate)
{
    m_sampleRate = sampleRate;
    updateIncrements();

    m_lfoPhase = 0.0;
}

void AdpcmCore::setParameters(const SynthParams& params)
{
    m_level = params.adpcmLevel;
    m_pan = params.adpcmPan;

    m_pcmOffset = params.adpcmPcmOffset;
    m_pcmRatio = params.adpcmPcmRatio;

    bool newLoopState = params.adpcmLoop;
    if (m_isLooping != newLoopState) {
        m_isLooping = newLoopState;
        if (m_isLooping) m_hasFinished = false;
    }

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
void AdpcmCore::setSampleData(const std::vector<float>& sourceData, double sourceRate)
{
    // 1. Rawデータ (32bit float) をそのまま保持
    m_rawBuffer = sourceData;
    m_sourceRate = sourceRate;

    // 2. ADPCMデータ (4bit emulation) も事前に作っておく
    // OPNAのレート(約16kHz)にリサンプリングして変換
    double targetRate = 16000.0;

    m_bufferSampleRate = targetRate;

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

    adpcmLowPassFilter(m_adpcmBuffer);
}

void AdpcmCore::noteOn(float freq, float velocity, int midiNote)
{
    m_position = 0.0;

    float rootFreq = (float)juce::MidiMessage::getMidiNoteInHertz(m_rootNote);
    double currentBufferRate = (m_qualityMode == adpcmMode) ? m_bufferSampleRate : m_sourceRate;

    if (m_qualityMode == adpcmMode) // ADPCM Mode
    {
        currentBufferRate = m_bufferSampleRate;
    }
    else // Raw, 24bit, 16bit, 8bit, 5bit, 4bit-Linear
    {
        currentBufferRate = m_sourceRate;
    }

    m_position = (m_pcmOffset / 1000.0) * currentBufferRate;

    // ホストDAWのレートとの比率
    double rateRatio = currentBufferRate / m_sampleRate;

    m_pitchRatio = (freq / rootFreq) * rateRatio;

    //m_currentLevel = 0.0f;
    m_state = State::Attack;
    m_hasFinished = false;
}

void AdpcmCore::noteOff()
{
    m_state = State::Release;
}

bool AdpcmCore::isPlaying() const
{
    return m_state != State::Idle;
}

// ピッチベンド (0 - 16383, Center=8192)
void AdpcmCore::setPitchBend(int pitchWheelValue)
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
void AdpcmCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

float AdpcmCore::getCurrentPan() const
{
    return m_pan;
}

void AdpcmCore::setPitchBendRatio(float ratio)
{
    m_pitchBendRatio = ratio;
}

float AdpcmCore::getSample()
{
    if (m_state == State::Idle) return 0.0f;

    // Safety check for empty buffers
    if ((m_qualityMode == 6 && m_adpcmBuffer.empty()) ||
        (m_qualityMode != 6 && m_rawBuffer.empty())) {
        return 0.0f;
    }

    if (m_hasFinished) return 0.0f;

    processAdsr();

    // --- Pitch Modulation (Vibrato) ---
    // Simple 5Hz LFO
    double lfoInc = 5.0 / m_sampleRate;
    m_lfoPhase += lfoInc;
    if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

    float lfoVal = std::sin(m_lfoPhase * 2.0 * juce::MathConstants<double>::pi);

    // Mod Wheel Depth (Max +/- 10% speed change -> approx +/- 2 semitones)
    float modDepth = m_modWheel * 0.1f;
    float lfoPitchMod = 1.0f + (lfoVal * modDepth);

    // Calculate Total Playback Speed Increment
    // Base Speed * Pitch Bend * Vibrato
    double currentIncrement = m_pitchRatio * m_pitchBendRatio * lfoPitchMod;

    float output = 0.0f;

    double currentBufferRate = (m_qualityMode == adpcmMode) ? m_bufferSampleRate : m_sourceRate;
    size_t totalSize = (m_qualityMode == adpcmMode) ? m_adpcmBuffer.size() : m_rawBuffer.size();

    // ★追加: 終端位置の計算
    double offsetSamples = (m_pcmOffset / 1000.0) * currentBufferRate;
    if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;

    double remainingSize = totalSize - offsetSamples;
    double playSize = remainingSize * m_pcmRatio;
    if (playSize < 1.0) playSize = 1.0;

    double endPosition = offsetSamples + playSize;

    // --- Mode Switching ---
    if (m_qualityMode == adpcmMode) // 4-bit ADPCM (OPNA Style)
    {
        size_t bufSize = m_adpcmBuffer.size();

        // ループ・終了判定
        if (m_position >= endPosition) {
            if (m_isLooping) {
                m_position = offsetSamples + std::fmod(m_position - endPosition, playSize);
            }
            else {
                m_hasFinished = true;
                return 0.0f;
            }
        }

        int index = (int)m_position;
        if (index < m_adpcmBuffer.size()) {
            output = m_adpcmBuffer[index] / 32768.0f;
        }
    }
    else // PCM Modes (Bit Crushing)
    {
        if (m_position >= endPosition) {
            if (m_isLooping) {
                m_position = offsetSamples + std::fmod(m_position - endPosition, playSize);
            }
            else {
                m_hasFinished = true;
                return 0.0f;
            }
        }

        int idx0 = (int)m_position;
        int idx1 = idx0 + 1;
        // ループ端の処理
        if (idx1 >= endPosition || idx1 >= m_rawBuffer.size()) {
            idx1 = m_isLooping ? offsetSamples : idx0;
        }

        float frac = (float)(m_position - idx0);

        if (idx0 < totalSize && idx1 < totalSize) {
            float s0 = m_rawBuffer[idx0];
            float s1 = m_rawBuffer[idx1];
            output = s0 * (1.0f - frac) + s1 * frac;
        }

        output = bitReduction(output, m_qualityMode);
    }

    // Advance position
    m_position += currentIncrement;

    return output * m_level * m_currentLevel;
}

void AdpcmCore::refreshAdpcmBuffer()
{
    if (m_rawBuffer.empty()) return;

    double targetRate = getTargetRate(m_rateIndex, 16000.0f);

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

    adpcmLowPassFilter(m_adpcmBuffer);
}

void AdpcmCore::processAdsr()
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

void AdpcmCore::updateIncrements()
{
    if (m_sampleRate <= 0.0) return;
    m_attackInc = 1.0f / (float)(std::max(0.001f, m_adsr.a) * m_sampleRate);
    m_decayDec = 1.0f / (float)(std::max(0.001f, m_adsr.d) * m_sampleRate);
    m_releaseDec = 1.0f / (float)(std::max(0.001f, m_adsr.r) * m_sampleRate);
}

void AdpcmCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();
    float pan = getCurrentPan();

    outL[startSample + sampleIdx] += sample * (1.0f - pan);
    outR[startSample + sampleIdx] += sample * pan;

    isActive = isPlaying();
}
