#include "./Core.h"

#include "../../Core/Synth/SynthHelpers.h"
#include "../../Generator/Pcm/Core.h"

void RhythmPad::prepare(double hostSampleRate)
{
	m_adsr.prepare(hostSampleRate);
	m_pitchAdsr.prepare(0, hostSampleRate);
}

void RhythmPad::setCurveCore(CurveCore* p_curveCore)
{
    m_adsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
}

void RhythmPad::setSampleRate(double sampleRate)
{
    m_adsr.updateTargetSampleRate(sampleRate);
    m_pitchAdsr.updateTargetSampleRate(sampleRate);
}

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

    if (m_pan == 0.5f) {
		m_panL = 1.0f;
		m_panR = 1.0f;

	}
    else {
        m_panL = (float)((1 - m_pan) * 2);
        m_panR = (float)((m_pan) * 2);
    }

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

void RhythmPad::start(float velocity, bool isLegato)
{
    double currentBufferRate = (m_qualityMode == 7) ? m_bufferSampleRate : m_sourceRate;

    float oldBaseLevel = m_baseLevel;

    if (!isLegato) {
        // 非レガート（新規発音）時のみ、再生位置を初期化し、ベロシティを更新する
        m_position = (m_pcmOffset / 1000.0) * currentBufferRate;
        m_baseLevel = std::max(0.01f, velocity);
    }

    // =====================================================================
    // モノフォニック・レガート時の音量ジャンプ防止処理
    // =====================================================================
    if (isLegato && m_currentEnv > 0.0f) {
        // レガート時は m_baseLevel を更新しないのが一番安全ですが、
        // もし更新する場合(Velocityを反映させたい場合)は以下の逆算を行います。
        // ※今回はSSGと同じく「レガート時は m_baseLevel を維持する（更新しない）」仕様とします。
        // なので、ここは特に何もしなくてOKです。
    }

    // エンベロープも非レガート時のみ再トリガーする
    if (!isLegato) {
        m_currentEnv = m_adsr.noteOn();

        if (!m_pitchAdsr.isBypass() && !m_pitchResetOnLegato) {
            m_pitchAdsr.noteOn();
        }
    }

    if (!m_pitchAdsr.isBypass() && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }
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
    if (m_adsr.isBypass())
    {
        // どちらもバイパスの時は完全な矩形波（Gate）動作
        if (m_adsr.isRelease()) {
            m_adsr.bypassedReleasedProcess();
        }
    }
    else
    {
        // 1. 従来のADSR処理 (内部の m_currentLevel はADSR専用として維持する)
        if (!m_adsr.isBypass()) {
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
    return output * m_level * finalEnv * m_baseLevel;
}

void RhythmPad::refreshAdpcmBuffer()
{
    // Copy the same resampling & encoding logic as AdpcmCore here
    // (To avoid code duplication, it's best to extract Codec to a separate header, but omitted here)
    if (m_rawBuffer.empty()) return;

    double targetRate = getTargetRate(m_rateIndex);

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

    adpcmLowPassFilter(m_adpcmBuffer);
}

void RhythmCore::prepare(double sampleRate)
{
    m_sampleRate = sampleRate;
    for (auto& pad : pads) {
        pad.prepare(sampleRate);
    }
}

void RhythmCore::setCurveCore(CurveCore* p_curveCore)
{
    for (auto& pad : pads) {
        pad.setCurveCore(p_curveCore);
    }
}

void RhythmCore::setSampleRate(double sampleRate)
{
	m_sampleRate = sampleRate;
	for (auto& pad : pads) {
		pad.setSampleRate(sampleRate);
	}
}

void RhythmCore::setParameters(const SynthParams& params)
{
    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    for (int i = 0; i < MaxRhythmPads; ++i) {
        pads[i].setParameters(params.rhythm.pads[i]);
        pads[i].m_pitchResetOnLegato = params.pitchResetOnLegato;
    }
}

// Load sample from external source (Specify Pad index)
void RhythmCore::setSampleData(int padIndex, const std::vector<float>& data, double rate)
{
    if (padIndex >= 0 && padIndex < MaxRhythmPads) {
        pads[padIndex].setSampleData(data, rate);
    }
}

void RhythmCore::noteOn(float freq, float velocity, int midiNote, bool isLegato)
{
    // ユニゾン・ハーモニー用
    // ユニゾンデチューンの計算
    float finalFreq = freq;

    // ユニゾン時の位相のズレ(0.0〜1.0)を算出
    float phaseOffsetNorm = 0.0f;

    if (m_unisonTotal > 1) {
        // 現在のボイスが全体のどこに配置されるかを -1.0(一番下) 〜 1.0(一番上) で算出
        // 例(3ボイス): -1.0,  0.0,  1.0
        // 例(4ボイス): -1.0, -0.33, 0.33, 1.0
        float spreadPos = ((float)m_unisonIndex / (float)(m_unisonTotal - 1)) * 2.0f - 1.0f;

        // 最大デチューン幅に位置を掛け合わせて、このボイスのズレ量(セント)を決定
        float centOffset = spreadPos * (float)m_unisonDetuneAmt;

        // セント値(Cents) を周波数倍率(Ratio)に変換する
        // (1200セント ＝ 1オクターブ ＝ 周波数2倍)
        finalFreq = freq * std::pow(2.0f, centOffset / 1200.0f);

        // ボイスインデックスに応じて位相を均等に散らす (例: 3ボイスなら 0.0, 0.33, 0.66)
        phaseOffsetNorm = (float)m_unisonIndex / (float)m_unisonTotal;
    }

    for (auto& pad : pads) {
        if (pad.m_noteNumber == midiNote) {
            // ループモード(isOneShot == false)で、かつ既に再生中の場合は、
            // 新たなNoteOnを無視してループを継続させる。
            // (あるいは、完全に最初から鳴らし直したい場合は m_position=0 とするが、
            //  今回は「音が重なる/途切れる」のを防ぐため無視するアプローチをとる)
            if (!pad.m_isOneShot && pad.isPlaying() && !isLegato) {
                // 非レガート時のみ、再トリガーを無視してループを継続（既存の仕様）
                continue; // 何もしない
            }

            // ユニゾン・ハーモニー向けに変更
            // 計算した位相のズレをオペレータに渡す
            pad.setUnisonPhaseOffset(phaseOffsetNorm);

            pad.start(velocity, isLegato);
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
            float basePanL = pad.m_panL;
            float basePanR = pad.m_panR;

            if (m_unisonTotal > 1) {
                float spreadPos = ((float)m_unisonIndex / (float)(m_unisonTotal - 1)) * 2.0f - 1.0f; // -1.0 to 1.0

                // spreadPosが -1(L) の時、Right側の音量を下げる。逆も然り。
                float panOffset = spreadPos * m_unisonSpreadAmt * 0.5f; // 最大で ±0.5 動く

                basePanL = std::clamp(basePanL - panOffset, 0.0f, 1.0f);
                basePanR = std::clamp(basePanR + panOffset, 0.0f, 1.0f);

                // 音量補正 (ボイス数が増えると爆音になるため下げる)
                // ルートを取るか、単純に割るかは好みですが、単純割りの方が安全です
                float gainComp = 1.0f / std::sqrt((float)m_unisonTotal);
            }

            float sample = pad.getSample(m_sampleRate, totalPitchRatio);

            outL += sample * basePanL;
            outR += sample * basePanR;
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
