#include <vector>
#include <cmath>
#include <array>

#include "./SynthAdpcm.h"

#include "../../Core/Synth/SynthMode.h"
#include "../../Core/Synth/SynthHelpers.h"
#include "../../Generator/Pcm/Adpcm/GenAdpcm.h"
#include "../../Generator/Pcm/Dpcm/GenDpcm.h"
#include "../../Generator/Pcm/Helper/GenPcmHelper.h"

void AdpcmCore::prepare(double sampleRate)
{
    m_sampleRate = sampleRate;

    m_adsr.prepare(m_sampleRate);
    m_pitchAdsr.prepare(0, m_sampleRate);
    m_ssgSwEnv.prepare(0, m_sampleRate);

    double target = getTargetRate(m_rateIndex);

    m_noiseGen.prepare(target);
    m_lfo.prepare(target);

    m_lfoPhase = 0.0;
}

void AdpcmCore::setCurveCore(CurveCore* p_curveCore)
{
    m_adsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv.setCurveCore(p_curveCore);
}

void AdpcmCore::setSampleRate(double sampleRate)
{
	m_sampleRate = sampleRate;
	m_adsr.updateSampleRate(m_sampleRate);
	m_pitchAdsr.updateSampleRate(m_sampleRate);
	m_ssgSwEnv.updateSampleRate(m_sampleRate);
}

void AdpcmCore::setParameters(const SynthParams& params)
{
    m_level = params.adpcm.level;
    m_pan = params.adpcm.pan;
    m_tone = params.adpcm.tone;
    m_mix = params.adpcm.mix;

	if (m_pan == 0.5f) {
		m_panL = 1.0f;
		m_panR = 1.0f;
	}
	else {
		m_panL = (float)((1 - m_pan) * 2);
		m_panR = (float)((m_pan) * 2);
	}

    m_pcmOffset = params.adpcm.offset;
    m_pcmRatio = params.adpcm.ratio;

    m_fixMode.setParameters(params.adpcm.fixedMode, params.adpcm.fixedFreq);

    bool newLoopState = params.adpcm.loop;
    if (m_isLooping != newLoopState) {
        m_isLooping = newLoopState;
        if (m_isLooping) m_hasFinished = false;
    }

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_pitchResetOnLegato = params.pitchResetOnLegato;

    m_adsr.setParameters(params.adpcm.adsr);
    m_pitchAdsr.setParameters(params.adpcm.pitchAdsr);
	m_ssgSwEnv.setParameters(params.adpcm.ssgSwEnv);
    m_detune.setParameters(params.adpcm.detune, params.adpcm.detune2, params.adpcm.detune3, params.adpcm.multiple, params.adpcm.multipleRatio);
    m_lfo.setParameters(
        params.adpcm.lfoPmSyncDelay,
        params.adpcm.lfoAmSyncDelay,
        params.adpcm.lfoPmEnable,
        params.adpcm.lfoAmEnable,
        params.adpcm.lfoPmFreq,
        params.adpcm.lfoAmFreq,
        params.adpcm.lfoPmWave,
        params.adpcm.lfoAmWave,
        params.adpcm.lfoPms,
        params.adpcm.lfoPmd,
        params.adpcm.lfoAms,
        params.adpcm.lfoAmd,
        params.adpcm.lfoAmSmRt
    );
    m_noiseGen.setParameters(params.adpcm.noiseLevel, params.adpcm.noiseFreq, false);

    m_rootNote = params.adpcm.rootNote;

    bool needRefresh = false;

    if (m_qualityMode != params.adpcm.qualityMode) {
        m_qualityMode = params.adpcm.qualityMode;
        needRefresh = true; // ADPCM <-> DPCM <-> PCMの切り替えで再生成が必要
    }

    if (m_rateIndex != params.adpcm.rateIndex) {
        m_rateIndex = params.adpcm.rateIndex;

        m_targetRate = getTargetRate(m_rateIndex);

        m_noiseGen.updateTargetRate(m_targetRate);
        needRefresh = true;
    }

    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();

    if (needRefresh) {
        refreshPcmBuffer();
    }
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

    m_adsr.prepare(m_bufferSampleRate);
    m_pitchAdsr.prepare(0, m_bufferSampleRate);
    m_ssgSwEnv.prepare(0, m_bufferSampleRate);

    double step = sourceRate / targetRate;

    m_pcmBuffer.clear();
    m_pcmBuffer.reserve((size_t)(sourceData.size() / step));

    double pos = 0;

    // --- ロード時もDPCM/ADPCM設定に従ってエンコードする ---
    if (m_qualityMode == dpcmMode)
    {
        DpcmCodec codec;
        codec.reset();
        while (pos < sourceData.size()) {
            int index = (int)pos;

            if (index >= sourceData.size()) break;

            int16_t input = (int16_t)(sourceData[index] * 32767.0f);

            m_pcmBuffer.push_back(codec.decode(codec.encode(input)));

            pos += step;
        }
    }
    else
    {
        Ym2608AdpcmCodec codec;
        codec.reset();
        while (pos < sourceData.size()) {
            int index = (int)pos;

            if (index >= sourceData.size()) break;

            int16_t input = (int16_t)(sourceData[index] * 32767.0f);

            m_pcmBuffer.push_back(codec.decode(codec.encode(input)));

            pos += step;
        }
    }

    GenPcmHelper::lowPassFilter(m_pcmBuffer);
}

void AdpcmCore::noteOn(float freq, float velocity, int midiNote, bool isLegato)
{
    // =====================================================================
    // 1. ベロシティとベースレベルの更新 (非レガート時のみ)
    // =====================================================================
    if (!isLegato) {
        m_baseLevel = std::max(0.01f, velocity);
    }

    // =====================================================================
    // 2. ピッチ・周波数の計算 (レガート時も音程は変わるため毎回実行)
    // =====================================================================
    float rootFreq = (float)juce::MidiMessage::getMidiNoteInHertz(m_rootNote);

    // ADPCMモードとDPCMモードを共通で「エンコードバッファ使用モード」として判定
    bool isEncodedMode = (m_qualityMode == adpcmMode || m_qualityMode == dpcmMode);
    double currentBufferRate = isEncodedMode ? m_bufferSampleRate : m_sourceRate;
    float finalFreq = freq;

    m_unisonPhaseOffset = 0.0f;

    if (m_unisonTotal > 1) {
        float spreadPos = ((float)m_unisonIndex / (float)(m_unisonTotal - 1)) * 2.0f - 1.0f;
        float centOffset = spreadPos * (float)m_unisonDetuneAmt;
        finalFreq = freq * std::pow(2.0f, centOffset / 1200.0f);

        // ADPCMでは直接波形の位相としては使いませんが、計算自体は残しておきます
        m_unisonPhaseOffset = (float)m_unisonIndex / (float)m_unisonTotal;
    }

    // ホストDAWのレートとの比率を加味した再生速度レシオの更新
    double rateRatio = currentBufferRate / m_sampleRate;

    finalFreq = m_fixMode.noteOn(finalFreq);
    m_currentFrequency = m_detune.noteOn(finalFreq);

    m_pitchRatio = (m_currentFrequency / rootFreq) * rateRatio;

    // =====================================================================
    // 3. 発音の初期化 (非レガート・新規発音時のみ実行)
    // =====================================================================
    if (!isLegato) {
        // 再生位置を先頭(オフセット位置)に戻す
        m_position = (m_pcmOffset / 1000.0) * currentBufferRate;
        m_hasFinished = false;
        m_lfoPhase = 0.0;

        // エンベロープの再トリガー
        m_currentLevel = m_adsr.noteOn();

        if (!m_pitchAdsr.isBypass() && !m_pitchResetOnLegato) {
            m_pitchAdsr.noteOn();
        }

        if (!m_ssgSwEnv.isBypass()) {
            m_ssgSwEnv.noteOn();
        }
        m_lfo.noteOn();
    }

    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();

    if (!m_pitchAdsr.isBypass() && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }
}

void AdpcmCore::noteOff()
{
    m_adsr.noteOff();
    m_pitchAdsr.noteOff();
	m_ssgSwEnv.noteOff();
}

bool AdpcmCore::isPlaying() const
{
    return m_adsr.isPlaying() || m_ssgSwEnv.isPlaying();
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
    if (!isPlaying()) {
        // ADSRとSwEnvの両方がバイパスの時は、完全な矩形波（Gate）動作
        // ピッチエンベロープは強制的に終了させる（そうしないと、次のノートオンでピッチが変になったりする）
        m_pitchAdsr.bypassedReleasedProcess();

        return 0.0f;
    }

    bool isEncodedMode = (m_qualityMode == adpcmMode || m_qualityMode == dpcmMode);

    // Safety check for empty buffers
    if ((isEncodedMode && m_pcmBuffer.empty()) ||
        (!isEncodedMode && m_rawBuffer.empty())) {
        return 0.0f;
    }

    if (m_hasFinished) return 0.0f;

    float finalEnv = 1.0f;

    // --- ADSR & SwEnv Gate Logic ---
    if (m_adsr.isBypass() && m_ssgSwEnv.isBypass())
    {
        // どちらもバイパスの時は完全な矩形波（Gate）動作
        if (m_adsr.isRelease() || m_ssgSwEnv.isRelease()) {
            m_adsr.bypassedReleasedProcess();
            m_ssgSwEnv.bypassedReleasedProcess();
            finalEnv = 1.0f;
        }
    }
    else
    {
        // 1. 従来のADSR処理 (内部の m_currentLevel はADSR専用として維持する)
        if (!m_adsr.isBypass()) {
            m_currentLevel = m_adsr.process(m_currentLevel);
            finalEnv *= m_currentLevel; // 掛け算
        }
        else {
            if (m_adsr.isRelease()) m_adsr.bypassedReleasedProcess();
        }

        // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
        if (!m_ssgSwEnv.isBypass()) {
            finalEnv *= m_ssgSwEnv.process(); // 掛け算
        }
        else {
            if (m_ssgSwEnv.isRelease()) m_ssgSwEnv.bypassedReleasedProcess();
        }
    }

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
    double currentIncrement = m_pitchAdsr.process(m_pitchRatio * m_pitchBendRatio * lfoPitchMod);

    float output = 0.0f;

    double currentBufferRate = (m_qualityMode == adpcmMode) ? m_bufferSampleRate : m_sourceRate;
    size_t totalSize = (m_qualityMode == adpcmMode) ? m_pcmBuffer.size() : m_rawBuffer.size();

    // 終端位置の計算
    double offsetSamples = (m_pcmOffset / 1000.0) * currentBufferRate;
    if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;

    double remainingSize = totalSize - offsetSamples;
    double playSize = remainingSize * m_pcmRatio;
    if (playSize < 1.0) playSize = 1.0;

    double endPosition = offsetSamples + playSize;

    // --- Mode Switching ---
    if (isEncodedMode) // 4-bit ADPCM (OPNA Style) or 1-bit DPCM
    {
        size_t bufSize = m_pcmBuffer.size();

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
        if (index < m_pcmBuffer.size()) {
            output = m_pcmBuffer[index] / 32768.0f;
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

        output = GenPcmHelper::bitReduction(output, m_qualityMode);
    }

    // ==========================================
    // 3. Noise Generator
    // ==========================================
    m_noiseGen.generate();

    // ==========================================
    // 4. Mixing
    // ==========================================
    float toneGain = 1.0f - m_mix;
    float noiseGain = m_mix;
    float rawMixed = (output * m_tone * toneGain) + m_noiseGen.generateSample(noiseGain);

    // ==========================================
    // Opzx7 LFO の計算 (AM / PM)
    // ==========================================
    m_lfo.getSample();

    // 1. Amplitude Modulation (AM / 音量)
    float amMultiplier = 1.0f;

    if (m_lfo.am.enable) {
        // depthDb はセットアップ時に計算済みなので、そのままdB減衰に変換
        float attenDb = m_lfo.value.am * m_lfo.am.depthDb;
        amMultiplier = std::pow(10.0f, -attenDb / 20.0f);
    }

    // 2. Pitch Modulation (PM / 音程)
    float pitchModCents = 0.0f;

    if (m_lfo.pm.enable) {
        // depthCent も計算済みなので、そのままセント値に変換
        pitchModCents += m_lfo.value.pm * m_lfo.pm.depthCent;
    }

    // セントを周波数倍率(レシオ)に変換
    float opzx7PitchMod = std::pow(2.0f, pitchModCents / 1200.0f);

    // ==========================================
    // 周波数倍率の決定
    // (PitchBend × Opzx7のPM × ModWheelのPM)
    // ==========================================
    float freqMult = m_pitchBendRatio * opzx7PitchMod;

    // Advance position
    m_position += currentIncrement * freqMult;

    return rawMixed * m_level * finalEnv * m_baseLevel * amMultiplier * 4.0f;
}

void AdpcmCore::refreshPcmBuffer()
{
    if (m_rawBuffer.empty()) return;

    double targetRate = getTargetRate(m_rateIndex, 16000.0f);

    // Do not upsample beyond source rate for the ADPCM buffer gen
    if (targetRate > m_sourceRate) targetRate = m_sourceRate;

    m_bufferSampleRate = targetRate;

    m_adsr.prepare(m_bufferSampleRate);
    m_pitchAdsr.prepare(0, m_bufferSampleRate);
    m_ssgSwEnv.prepare(0, m_bufferSampleRate);
    m_noiseGen.prepare(m_bufferSampleRate);

    // Resample & Encode
    double step = m_sourceRate / targetRate;
    if (step <= 0.0) step = 1.0;

    m_pcmBuffer.clear();
    m_pcmBuffer.reserve((size_t)(m_rawBuffer.size() / step) + 1);

    double pos = 0;

    // --- DPCMとADPCMの分岐エンコード ---
    if (m_qualityMode == dpcmMode)
    {
        DpcmCodec codec;
        codec.reset();

        while (pos < m_rawBuffer.size()) {
            int index = (int)pos;

            if (index >= m_rawBuffer.size()) break;

            int16_t input = (int16_t)(m_rawBuffer[index] * 32767.0f);

            m_pcmBuffer.push_back(codec.decode(codec.encode(input)));

            pos += step;
        }
    }
    else
    {
        Ym2608AdpcmCodec codec;
        codec.reset();

        while (pos < m_rawBuffer.size()) {
            int index = (int)pos;

            if (index >= m_rawBuffer.size()) break;

            int16_t input = (int16_t)(m_rawBuffer[index] * 32767.0f);

            m_pcmBuffer.push_back(codec.decode(codec.encode(input)));

            pos += step;
        }
    }

    GenPcmHelper::lowPassFilter(m_pcmBuffer);
}

void AdpcmCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();
    float pan = getCurrentPan();

    // ユニゾン・ハーモニー向けに変更
    float basePanL = m_panL;
    float basePanR = m_panR;

    if (m_unisonTotal > 1) {
        float spreadPos = ((float)m_unisonIndex / (float)(m_unisonTotal - 1)) * 2.0f - 1.0f; // -1.0 to 1.0

        // spreadPosが -1(L) の時、Right側の音量を下げる。逆も然り。
        float panOffset = spreadPos * m_unisonSpreadAmt * 0.5f; // 最大で ±0.5 動く

        basePanL = std::clamp(basePanL - panOffset, 0.0f, 1.0f);
        basePanR = std::clamp(basePanR + panOffset, 0.0f, 1.0f);

        // 音量補正 (ボイス数が増えると爆音になるため下げる)
        // ルートを取るか、単純に割るかは好みですが、単純割りの方が安全です
        float gainComp = 1.0f / std::sqrt((float)m_unisonTotal);
        sample *= gainComp;
    }

    outL[startSample + sampleIdx] += sample * basePanL;
    outR[startSample + sampleIdx] += sample * basePanR;

    isActive = isPlaying();
}
