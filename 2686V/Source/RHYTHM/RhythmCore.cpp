#include "RhythmCore.h"
#include "../synth/SynthHelpers.h"

// Set data (Same logic as AdpcmCore)
void RhythmPad::setSampleData(const std::vector<float>& sourceData, double sourceRate)
{
    m_rawBuffer = sourceData;
    m_sourceRate = sourceRate;
    refreshAdpcmBuffer();
}

// Update parameters and check for buffer regeneration
void RhythmPad::setParameters(const RhythmPadParams& params)
{
    m_noteNumber = params.noteNumber;
    m_level = params.level;
    m_pan = params.pan;
    m_isOneShot = params.isOneShot;
    m_releaseParam = params.release;

    bool needRefresh = false;
    if (m_qualityMode != params.qualityMode) { m_qualityMode = params.qualityMode; }
    if (m_rateIndex != params.rateIndex) {
        m_rateIndex = params.rateIndex;
        needRefresh = true;
    }

    if (needRefresh) refreshAdpcmBuffer();
}

void RhythmPad::triggerRelease(double hostSampleRate)
{
    if (m_state == State::Playing)
    {
        m_state = State::Release;

        // 減衰量を計算 (現在のサンプルレートに基づく)
        // 0.001秒未満にならないようガード
        float releaseTime = std::max(0.001f, m_releaseParam);
        m_releaseDec = 1.0f / (float)(releaseTime * hostSampleRate);
    }
}

void RhythmPad::start()
{
    m_position = 0.0;
    m_state = State::Playing; // 再生状態へ
    m_currentEnv = 1.0f;      // 音量は最大から
}

void RhythmPad::stop()
{
    m_state = State::Idle;
    m_currentEnv = 0.0f;
}

bool RhythmPad::isPlaying() const
{
    return m_state != State::Idle;
}

float RhythmPad::getSample(double hostSampleRate, float pitchRatio)
{
    if (m_state == State::Idle) return 0.0f;

    // --- リリース処理 ---
    if (m_state == State::Release)
    {
        m_currentEnv -= m_releaseDec;
        if (m_currentEnv <= 0.0f)
        {
            m_currentEnv = 0.0f;
            m_state = State::Idle;
            return 0.0f;
        }
    }
    // ---------------------------

    double currentBufferRate = (m_qualityMode == 7) ? m_bufferSampleRate : m_sourceRate;

    // 再生速度にピッチ比率を掛ける
    double increment = (currentBufferRate / hostSampleRate) * pitchRatio;

    float output = 0.0f;
    size_t bufferSize = 0;

    if (m_qualityMode == 7) // ADPCM Playback
    {
        bufferSize = m_adpcmBuffer.size();
        if (m_adpcmBuffer.empty()) return 0.0f;

        if (m_position >= bufferSize) {
            if (m_isOneShot) {
                m_state = State::Idle; // Stop
                return 0.0f;
            }
            else { m_position = std::fmod(m_position, (double)bufferSize); }
        }

        int idx = (int)m_position;
        if (idx >= bufferSize) idx = 0;
        output = m_adpcmBuffer[idx] / 32768.0f;
    }
    else // Raw / PCM Playback
    {
        bufferSize = m_rawBuffer.size();
        if (m_rawBuffer.empty()) return 0.0f;


        if (m_position >= bufferSize) {
            if (m_isOneShot) {
                m_state = State::Idle; // Stop
                return 0.0f;
            }
            else { m_position = std::fmod(m_position, (double)bufferSize); }
        }

        // Linear Interpolation
        int idx0 = (int)m_position;
        int idx1 = idx0 + 1;
        if (idx1 >= bufferSize) idx1 = 0;

        float frac = (float)(m_position - idx0);
        output = m_rawBuffer[idx0] * (1.0f - frac) + m_rawBuffer[idx1] * frac;

        // Bitcrush Logic
        float maxVal = getTargetMaxVal(m_qualityMode);

        if (maxVal > 0.0f) output = std::floor(output * maxVal) / maxVal;
    }

    m_position += increment;

    return output * m_level * m_currentEnv;
}

void RhythmPad::refreshAdpcmBuffer()
{
    // Copy the same resampling & encoding logic as AdpcmCore here
    // (To avoid code duplication, it's best to extract Codec to a separate header, but omitted here)
    if (m_rawBuffer.empty()) return;

    double targetRate = getTargetRate(m_rateIndex - 1);

    if (targetRate > m_sourceRate) targetRate = m_sourceRate;
    m_bufferSampleRate = targetRate;

    double step = m_sourceRate / targetRate;
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

void RhythmCore::prepare(double sampleRate)
{
    m_sampleRate = sampleRate;
}

void RhythmCore::setParameters(const SynthParams& params)
{
    for (int i = 0; i < MaxRhythmPads; ++i) {
        pads[i].setParameters(params.rhythmPads[i]);
    }
}

// Load sample from external source (Specify Pad index)
void RhythmCore::setSampleData(int padIndex, const std::vector<float>& data, double rate)
{
    if (padIndex >= 0 && padIndex < MaxRhythmPads) {
        pads[padIndex].setSampleData(data, rate);
    }
}

void RhythmCore::noteOn(int midiNote, float velocity)
{
    for (auto& pad : pads) {
        if (pad.m_noteNumber == midiNote) {
            pad.start();
        }
    }
}

void RhythmCore::noteOff()
{
    for (auto& pad : pads) {
        if (pad.isPlaying()) {
            pad.triggerRelease(m_sampleRate);
        }
    }
}

bool RhythmCore::isPlaying() const
{
    for (const auto& pad : pads) {
        if (pad.isPlaying()) return true;
    }
    return false;
}

// ピッチベンド (0 - 16383, Center=8192)
void RhythmCore::setPitchBend(int pitchWheelValue)
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
void RhythmCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

void RhythmCore::setPitchBendRatio(float ratio)
{
    m_pitchBendRatio = ratio;
}

void RhythmCore::getSampleStereo(float& outL, float& outR)
{
    outL = 0.0f;
    outR = 0.0f;

    if (!isPlaying()) return;

    // --- Modulation Calculation ---
    double lfoInc = 5.0 / m_sampleRate;
    m_lfoPhase += lfoInc;
    if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

    float lfoVal = std::sin(m_lfoPhase * 2.0 * juce::MathConstants<double>::pi);
    float modDepth = m_modWheel * 0.1f;
    float lfoPitchMod = 1.0f + (lfoVal * modDepth);
    float totalPitchRatio = m_pitchBendRatio * lfoPitchMod;

    // すべてのパッドの音を計算し、それぞれの Pan 設定に従って左右に振り分けてミックス
    for (auto& pad : pads) {
        if (pad.isPlaying()) {
            float sample = pad.getSample(m_sampleRate, totalPitchRatio);
            outL += sample * (1.0f - pad.m_pan);
            outR += sample * pad.m_pan;
        }
    }
}
