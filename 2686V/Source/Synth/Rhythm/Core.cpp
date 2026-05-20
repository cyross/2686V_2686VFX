#include "./Core.h"

#include "../../Core/Synth/SynthHelpers.h"
#include "../../Generator/Pcm/Core.h"

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

    m_pcmOffset = params.pcmOffset;
    m_pcmRatio = params.pcmRatio;

    m_adsr.setParameters(params.adsr);
    m_pitchAdsr.setParameters(params.pitchAdsr);

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
    m_adsr.noteOff();
    m_pitchAdsr.noteOff();
}

void RhythmPad::start()
{
    double currentBufferRate = (m_qualityMode == 7) ? m_bufferSampleRate : m_sourceRate;
    m_position = (m_pcmOffset / 1000.0) * currentBufferRate;

    m_currentEnv = m_adsr.noteOn();
    m_pitchAdsr.noteOn();
}

void RhythmPad::stop()
{
    m_adsr.noteOff();
    m_pitchAdsr.noteOff();

    m_currentEnv = 0.0f;
}

bool RhythmPad::isPlaying() const
{
    return m_adsr.isPlaying();
}

float RhythmPad::getSample(double hostSampleRate, float pitchRatio)
{
    if (!isPlaying()) {
        // ADSRとSwEnvの両方がバイパスの時は、完全な矩形波（Gate）動作
        // ピッチエンベロープは強制的に終了させる（そうしないと、次のノートオンでピッチが変になったりする）
        m_pitchAdsr.bypassedReleasedProcess();

        return 0.0f;
    }

    float finalEnv = 1.0f;

    // --- ADSR & SwEnv Gate Logic ---
    if (m_adsr.isBypassed())
    {
        // どちらもバイパスの時は完全な矩形波（Gate）動作
        if (m_adsr.isRelease()) {
            m_adsr.bypassedReleasedProcess();
        }
    }
    else
    {
        // 1. 従来のADSR処理 (内部の m_currentLevel はADSR専用として維持する)
        if (!m_adsr.isBypassed()) {
            m_currentEnv = m_adsr.process(m_currentEnv);
            finalEnv = m_currentEnv;
        }
        else {
            if (m_adsr.isRelease()) m_adsr.bypassedReleasedProcess();
        }
    }

    double currentBufferRate = (m_qualityMode == 7) ? m_bufferSampleRate : m_sourceRate;
    double increment = m_pitchAdsr.process((currentBufferRate / hostSampleRate) * pitchRatio);

    // 総サイズと再生終了位置の計算
    size_t totalSize = (m_qualityMode == adpcmMode) ? m_adpcmBuffer.size() : m_rawBuffer.size();
    if (totalSize == 0) return 0.0f;

    double offsetSamples = (m_pcmOffset / 1000.0) * currentBufferRate;
    if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;

    double remainingSize = totalSize - offsetSamples;
    double playSize = remainingSize * m_pcmRatio;
    if (playSize < 1.0) playSize = 1.0;

    double endPosition = offsetSamples + playSize;

    float output = 0.0f;

    // ループと終端の判定を endPosition に基づいて行う
    if (m_qualityMode == adpcmMode)
    {
        if (m_position >= endPosition) {
            if (m_isOneShot) {
                return 0.0f;
            }
            else {
                // ループ時はオフセット位置に戻る
                m_position = offsetSamples + std::fmod(m_position - endPosition, playSize);
            }
        }

        int idx = (int)m_position;
        if (idx >= totalSize) idx = offsetSamples; // 安全ガード
        output = m_adpcmBuffer[idx] / 32768.0f;
    }
    else // Raw / PCM Playback
    {
        if (m_position >= endPosition) {
            if (m_isOneShot) {
                return 0.0f;
            }
            else {
                m_position = offsetSamples + std::fmod(m_position - endPosition, playSize);
            }
        }

        int idx0 = (int)m_position;
        int idx1 = idx0 + 1;
        // ループ端の処理
        if (idx1 >= endPosition || idx1 >= totalSize) idx1 = offsetSamples;

        float frac = (float)(m_position - idx0);
        output = m_rawBuffer[idx0] * (1.0f - frac) + m_rawBuffer[idx1] * frac;

        output = bitReduction(output, m_qualityMode);
    }

    m_position += increment;
    return output * m_level * finalEnv;
}

void RhythmPad::refreshAdpcmBuffer()
{
    // Copy the same resampling & encoding logic as AdpcmCore here
    // (To avoid code duplication, it's best to extract Codec to a separate header, but omitted here)
    if (m_rawBuffer.empty()) return;

    double targetRate = getTargetRate(m_rateIndex);

    if (targetRate > m_sourceRate) targetRate = m_sourceRate;
    m_bufferSampleRate = targetRate;

    m_adsr.updateTargetSampleRate(targetRate);
    m_pitchAdsr.updateTargetSampleRate(targetRate);

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

    adpcmLowPassFilter(m_adpcmBuffer);
}

void RhythmCore::prepare(double sampleRate)
{
    m_sampleRate = sampleRate;
}

void RhythmCore::setParameters(const SynthParams& params)
{
    for (int i = 0; i < MaxRhythmPads; ++i) {
        pads[i].setParameters(params.rhythm.pads[i]);
    }
}

// Load sample from external source (Specify Pad index)
void RhythmCore::setSampleData(int padIndex, const std::vector<float>& data, double rate)
{
    if (padIndex >= 0 && padIndex < MaxRhythmPads) {
        pads[padIndex].setSampleData(data, rate);
    }
}

void RhythmCore::noteOn(float freq, float velocity, int midiNote)
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

void RhythmCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float padOutL = 0.0f;
    float padOutR = 0.0f;

    // RhythmCore 内部で Pan 適用済みのステレオミックスを受け取る
    getSampleStereo(padOutL, padOutR);

    outL[startSample + sampleIdx] += padOutL;
    outR[startSample + sampleIdx] += padOutR;

    isActive = isPlaying();
}
