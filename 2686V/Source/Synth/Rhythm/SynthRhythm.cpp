#include "./SynthRhythm.h"

#include "../../Core/Synth/SynthHelpers.h"
#include "../../Generator/Pcm/Adpcm/GenAdpcm.h"
#include "../../Generator/Pcm/Dpcm/GenDpcm.h"
#include "../../Generator/Pcm/Helper/GenPcmHelper.h"

void RhythmPad::prepare(double hostSampleRate)
{
    m_sampleRate = hostSampleRate;
	m_adsr.prepare(m_sampleRate);
	m_pitchAdsr.prepare(0, m_sampleRate);
    m_ssgSwEnv.prepare(0, m_sampleRate);
    m_lfo.prepare(m_sampleRate);
    m_noiseGen.prepare(m_sampleRate);
}

void RhythmPad::setCurveCore(CurveCore* p_curveCore)
{
    m_adsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
}

void RhythmPad::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;
    m_adsr.updateTargetSampleRate(m_sampleRate);
    m_pitchAdsr.updateTargetSampleRate(m_sampleRate);
    m_ssgSwEnv.updateSampleRate(m_sampleRate);
    m_noiseGen.updateTargetRate(m_sampleRate);
    m_lfo.updateTargetSampleRate(m_sampleRate);
}

// Set data (Same logic as AdpcmCore)
void RhythmPad::setSampleData(const std::vector<float>& sourceData, double sourceRate)
{
    m_rawBuffer = sourceData;
    m_sourceRate = sourceRate;
    refreshPcmBuffer();
}

// Update parameters and check for buffer regeneration
void RhythmPad::setParameters(const RhythmPadParams& params)
{
    m_noteNumber = params.noteNumber;
    m_level = params.level;
    m_tone = params.tone;
    m_mix = params.mix;
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
    if (!m_isOneShot) m_hasFinished = false;

    m_pcmOffset = params.pcmOffset;
    m_pcmRatio = params.pcmRatio;

    m_adsr.setParameters(params.adsr);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_fixMode.setParameters(params.fixedMode, params.fixedFreq);
    m_ssgSwEnv.setParameters(params.ssgSwEnv);
    m_detune.setParameters(params.detune, params.detune2, params.detune3, params.multiple, params.multipleRatio);
    m_lfo.setParameters(
        params.lfoPmSyncDelay,
        params.lfoAmSyncDelay,
        params.lfoPmEnable,
        params.lfoAmEnable,
        params.lfoPmFreq,
        params.lfoAmFreq,
        params.lfoPmWave,
        params.lfoAmWave,
        params.lfoPms,
        params.lfoPmd,
        params.lfoAms,
        params.lfoAmd,
        params.lfoAmSmRt
    );
    m_noiseGen.setParameters(params.noiseLevel, params.noiseFreq, false);

    bool needRefresh = false;
    if (m_qualityMode != params.qualityMode) {
        m_qualityMode = params.qualityMode;
        needRefresh = true; // ADPCM <-> DPCM <-> PCMの切り替えで再生成が必要
    }
    if (m_rateIndex != params.rateIndex) {
        m_rateIndex = params.rateIndex;
        needRefresh = true;
    }

    m_interpolationMode = params.interpolationMode;

    if (needRefresh) refreshPcmBuffer();
}

void RhythmPad::triggerRelease(double hostSampleRate)
{
    m_adsr.noteOff();

    if (!m_pitchAdsr.isBypass()) {
        m_pitchAdsr.noteOff();
    }

    if (!m_ssgSwEnv.isBypass()) {
        m_ssgSwEnv.noteOff();
    }
}

void RhythmPad::setPitchBend(float pitchBend) {
    m_pitchBendRatio = pitchBend;
}

void RhythmPad::setModulationWheel(float modWheel) {
    m_modWheel = modWheel;
}

void RhythmPad::start(float velocity, bool isLegato, float freq, float uOffset, int uTotal)
{
    m_unisonPhaseOffset = uOffset;
    m_unisonTotal = uTotal;

    // ADPCMモードとDPCMモードを共通で「エンコードバッファ使用モード」として判定
    bool isEncodedMode = (m_qualityMode == adpcmMode || m_qualityMode == dpcmMode);
    double currentBufferRate = isEncodedMode ? m_bufferSampleRate : m_sourceRate;
    float finalFreq = freq;
    float oldBaseLevel = m_baseLevel;

    finalFreq = m_fixMode.noteOn(finalFreq);

    m_currentFrequency = m_detune.noteOn(finalFreq);
    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();
    m_pitchRatio = currentBufferRate / m_sampleRate;

    // =====================================================================
    // モノフォニック・レガート時の音量ジャンプ防止処理
    // =====================================================================
    if (isLegato && m_currentEnv > 0.0f) {
        // レガート時は m_baseLevel を更新しないのが一番安全ですが、
        // もし更新する場合(Velocityを反映させたい場合)は以下の逆算を行います。
        // ※今回はSSGと同じく「レガート時は m_baseLevel を維持する（更新しない）」仕様とします。
        // なので、ここは特に何もしなくてOKです。
    }

    if (!isLegato) {
        // 非レガート（新規発音）時のみ、再生位置を初期化し、ベロシティを更新する
        m_position = (m_pcmOffset / 1000.0) * currentBufferRate;
        m_baseLevel = std::max(0.01f, velocity);
        m_hasFinished = false;

        // エンベロープも非レガート時のみ再トリガーする
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

    if (!m_pitchAdsr.isBypass()) {
        m_pitchAdsr.noteOff();
    }

    if (!m_ssgSwEnv.isBypass()) {
        m_ssgSwEnv.noteOff();
    }
}

bool RhythmPad::isPlaying() const
{
    return m_adsr.isPlaying() || m_ssgSwEnv.isPlaying();
}

float RhythmPad::getSample()
{
    if (!isPlaying()) {
        // ADSRとSwEnvの両方がバイパスの時は、完全な矩形波（Gate）動作
        // ピッチエンベロープは強制的に終了させる（そうしないと、次のノートオンでピッチが変になったりする）
        m_pitchAdsr.bypassedReleasedProcess();

        return 0.0f;
    }

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
            m_currentEnv = m_adsr.process(m_currentEnv);
            finalEnv *= m_currentEnv; // 掛け算
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

    float output = 0.0f;
    bool isEncodedMode = (m_qualityMode == adpcmMode || m_qualityMode == dpcmMode);
    double currentBufferRate = m_sampleRate;

    // ノイズを出すために、バッファが空でも最後まで通す
    if (isEncodedMode && !m_pcmBuffer.empty()) {
        if (m_hasFinished) return 0.0f;

        currentBufferRate = m_bufferSampleRate;

        size_t totalSize = m_pcmBuffer.size();

        if (totalSize == 0) return 0.0f;

        double offsetSamples = (m_pcmOffset / 1000.0) * currentBufferRate;
        if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;

        double remainingSize = totalSize - offsetSamples;
        double playSize = remainingSize * m_pcmRatio;
        if (playSize < 1.0) playSize = 1.0;

        double endPosition = offsetSamples + playSize;

        // ループ・終了判定
        if (m_position >= endPosition) {
            if (m_isOneShot) {
                m_hasFinished = true;

                return 0.0f;
            }
            else {
                m_position = offsetSamples + std::fmod(m_position - endPosition, playSize);
            }
        }

        // =========================================================
        // 補間用の4点インデックス (過去1、現在、未来2) を計算
        // =========================================================
        int idx_0 = (int)m_position;
        int idx_1 = idx_0 + 1;
        int idx_2 = idx_0 + 2;
        int idx_m1 = idx_0 - 1;

        // ループ端の処理 (はみ出した場合はループ先頭/末尾に戻すか、クランプする)
        if (m_isOneShot) {
            if (idx_m1 < 0) idx_m1 = 0;
            if (idx_1 >= (int)totalSize) idx_1 = idx_0;
            if (idx_2 >= (int)totalSize) idx_2 = idx_1;
        }
        else {
            if (idx_m1 < (int)offsetSamples) idx_m1 += (int)playSize;
            if (idx_1 >= (int)endPosition) idx_1 -= (int)playSize;
            if (idx_2 >= (int)endPosition) idx_2 -= (int)playSize;
        }

        // 最終的な安全策 (バッファ外アクセス防止)
        idx_m1 = std::clamp(idx_m1, 0, (int)totalSize - 1);
        idx_0 = std::clamp(idx_0, 0, (int)totalSize - 1);
        idx_1 = std::clamp(idx_1, 0, (int)totalSize - 1);
        idx_2 = std::clamp(idx_2, 0, (int)totalSize - 1);

        // =========================================================
        // バッファから4点の値を取得 (-1.0f 〜 1.0f)
        // =========================================================
        float s_m1, s_0, s_1, s_2;

        // エンコードバッファ (int16_t) から読み込み、正規化
        s_m1 = m_pcmBuffer[idx_m1] / 32768.0f;
        s_0 = m_pcmBuffer[idx_0] / 32768.0f;
        s_1 = m_pcmBuffer[idx_1] / 32768.0f;
        s_2 = m_pcmBuffer[idx_2] / 32768.0f;

        // =========================================================
        // 補間処理 (Interpolation)
        // =========================================================
        float frac = (float)(m_position - idx_0);

        switch (m_interpolationMode) {
        case 0: // 0: Nearest (補間なし・エイリアスノイズが出るオールドスクール)
            output = (frac < 0.5f) ? s_0 : s_1;
            break;
        case 1: // 1: Linear (線形補間・現在の標準)
            output = s_0 * (1.0f - frac) + s_1 * frac;
            break;
        case 2: // 2: Gaussian/Cubic (SFC風の丸みのある補間)
        {
            // 3次エルミートスプライン近似による滑らかなカーブ生成
            float c0 = s_0;
            float c1 = 0.5f * (s_1 - s_m1);
            float c2 = s_m1 - 2.5f * s_0 + 2.0f * s_1 - 0.5f * s_2;
            float c3 = 0.5f * (s_2 - s_m1) + 1.5f * (s_0 - s_1);
            output = ((c3 * frac + c2) * frac + c1) * frac + c0;
            break;
        }
        case 3: // 3: Zero-Order Hold (最も粗いLo-Fiサンプラー風)
            output = s_0;
            break;
        case 4: // 4: Cosine (LinearとCubicの中間的な滑らかさ)
        {
            float mu2 = (1.0f - std::cos(frac * juce::MathConstants<float>::pi)) / 2.0f;
            output = s_0 * (1.0f - mu2) + s_1 * mu2;
            break;
        }
        case 5: // 5: B-Spline (強烈なローパス効果・SFCのこもり感を強調)
        {
            float c0 = (s_m1 + 4.0f * s_0 + s_1) / 6.0f;
            float c1 = (s_1 - s_m1) / 2.0f;
            float c2 = (s_m1 - 2.0f * s_0 + s_1) / 2.0f;
            float c3 = (s_2 - 3.0f * s_1 + 3.0f * s_0 - s_m1) / 6.0f;
            output = ((c3 * frac + c2) * frac + c1) * frac + c0;
            break;
        }
        case 6: // 6: Lagrange (4点補間、Cubicとは異なる倍音特性)
        {
            float l_m1 = -frac * (frac - 1.0f) * (frac - 2.0f) / 6.0f;
            float l_0 = (frac + 1.0f) * (frac - 1.0f) * (frac - 2.0f) / 2.0f;
            float l_1 = -(frac + 1.0f) * frac * (frac - 2.0f) / 2.0f;
            float l_2 = (frac + 1.0f) * frac * (frac - 1.0f) / 6.0f;
            output = s_m1 * l_m1 + s_0 * l_0 + s_1 * l_1 + s_2 * l_2;
            break;
        }
        }
    }
    else if (!isEncodedMode && !m_rawBuffer.empty()){
        if (m_hasFinished) return 0.0f;

        // 総サイズと再生終了位置の計算
        size_t totalSize = m_rawBuffer.size();

        currentBufferRate = m_sourceRate;

        double offsetSamples = (m_pcmOffset / 1000.0) * currentBufferRate;
        if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;

        double remainingSize = totalSize - offsetSamples;
        double playSize = remainingSize * m_pcmRatio;
        if (playSize < 1.0) playSize = 1.0;

        double endPosition = offsetSamples + playSize;

        // ループ・終了判定
        if (m_position >= endPosition) {
            if (m_isOneShot) {
                m_hasFinished = true;

                return 0.0f;
            }
            else {
                m_position = offsetSamples + std::fmod(m_position - endPosition, playSize);
            }
        }

        // =========================================================
        // 補間用の4点インデックス (過去1、現在、未来2) を計算
        // =========================================================
        int idx_0 = (int)m_position;
        int idx_1 = idx_0 + 1;
        int idx_2 = idx_0 + 2;
        int idx_m1 = idx_0 - 1;

        // ループ端の処理 (はみ出した場合はループ先頭/末尾に戻すか、クランプする)
        if (m_isOneShot) {
            if (idx_m1 < 0) idx_m1 = 0;
            if (idx_1 >= (int)totalSize) idx_1 = idx_0;
            if (idx_2 >= (int)totalSize) idx_2 = idx_1;
        }
        else {
            if (idx_m1 < (int)offsetSamples) idx_m1 += (int)playSize;
            if (idx_1 >= (int)endPosition) idx_1 -= (int)playSize;
            if (idx_2 >= (int)endPosition) idx_2 -= (int)playSize;
        }

        // 最終的な安全策 (バッファ外アクセス防止)
        idx_m1 = std::clamp(idx_m1, 0, (int)totalSize - 1);
        idx_0 = std::clamp(idx_0, 0, (int)totalSize - 1);
        idx_1 = std::clamp(idx_1, 0, (int)totalSize - 1);
        idx_2 = std::clamp(idx_2, 0, (int)totalSize - 1);

        // =========================================================
        // バッファから4点の値を取得 (-1.0f 〜 1.0f)
        // =========================================================
        float s_m1, s_0, s_1, s_2;

        s_m1 = m_rawBuffer[idx_m1];
        s_0 = m_rawBuffer[idx_0];
        s_1 = m_rawBuffer[idx_1];
        s_2 = m_rawBuffer[idx_2];

        // =========================================================
        // 補間処理 (Interpolation)
        // =========================================================
        float frac = (float)(m_position - idx_0);

        switch (m_interpolationMode) {
        case 0: // 0: Nearest (補間なし・エイリアスノイズが出るオールドスクール)
            output = (frac < 0.5f) ? s_0 : s_1;
            break;
        case 1: // 1: Linear (線形補間・現在の標準)
            output = s_0 * (1.0f - frac) + s_1 * frac;
            break;
        case 2: // 2: Gaussian/Cubic (SFC風の丸みのある補間)
        {
            // 3次エルミートスプライン近似による滑らかなカーブ生成
            float c0 = s_0;
            float c1 = 0.5f * (s_1 - s_m1);
            float c2 = s_m1 - 2.5f * s_0 + 2.0f * s_1 - 0.5f * s_2;
            float c3 = 0.5f * (s_2 - s_m1) + 1.5f * (s_0 - s_1);
            output = ((c3 * frac + c2) * frac + c1) * frac + c0;
            break;
        }
        case 3: // 3: Zero-Order Hold (最も粗いLo-Fiサンプラー風)
            output = s_0;
            break;
        case 4: // 4: Cosine (LinearとCubicの中間的な滑らかさ)
        {
            float mu2 = (1.0f - std::cos(frac * juce::MathConstants<float>::pi)) / 2.0f;
            output = s_0 * (1.0f - mu2) + s_1 * mu2;
            break;
        }
        case 5: // 5: B-Spline (強烈なローパス効果・SFCのこもり感を強調)
        {
            float c0 = (s_m1 + 4.0f * s_0 + s_1) / 6.0f;
            float c1 = (s_1 - s_m1) / 2.0f;
            float c2 = (s_m1 - 2.0f * s_0 + s_1) / 2.0f;
            float c3 = (s_2 - 3.0f * s_1 + 3.0f * s_0 - s_m1) / 6.0f;
            output = ((c3 * frac + c2) * frac + c1) * frac + c0;
            break;
        }
        case 6: // 6: Lagrange (4点補間、Cubicとは異なる倍音特性)
        {
            float l_m1 = -frac * (frac - 1.0f) * (frac - 2.0f) / 6.0f;
            float l_0 = (frac + 1.0f) * (frac - 1.0f) * (frac - 2.0f) / 2.0f;
            float l_1 = -(frac + 1.0f) * frac * (frac - 2.0f) / 2.0f;
            float l_2 = (frac + 1.0f) * frac * (frac - 1.0f) / 6.0f;
            output = s_m1 * l_m1 + s_0 * l_0 + s_1 * l_1 + s_2 * l_2;
            break;
        }
        }

        // Raw/BitCrusher モード時のビットリダクション
        output = GenPcmHelper::bitReduction(output, m_qualityMode);
    }

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
    float mwPitchMod = 1.0f + (m_lfo.value.pm * (m_modWheel * 0.03f));
    double currentIncrement = m_pitchAdsr.process(m_pitchRatio * m_pitchBendRatio * mwPitchMod);

    // ==========================================
    // 周波数倍率の決定
    // (PitchBend × Opzx7のPM × ModWheelのPM)
    // ==========================================
    float freqMult = m_pitchBendRatio * opzx7PitchMod;

    // Advance position
    m_position += currentIncrement * freqMult;

    // ==========================================
    // 3. Noise Generator
    // ==========================================
    m_noiseGen.generate();

    // ==========================================
    // 4. Mixing
    // ==========================================
    float toneGain = 1.0f - m_mix;
    float noiseGain = m_mix;
    float rawMixed = (output * m_tone * toneGain * 4.0f) + m_noiseGen.generateSample(noiseGain) * 0.4f;

    return rawMixed * m_level * finalEnv * m_baseLevel * amMultiplier;
}

void RhythmPad::refreshPcmBuffer()
{
    // Copy the same resampling & encoding logic as AdpcmCore here
    // (To avoid code duplication, it's best to extract Codec to a separate header, but omitted here)
    if (m_rawBuffer.empty()) return;

    double targetRate = getTargetRate(m_rateIndex);

    if (targetRate > m_sourceRate) targetRate = m_sourceRate;
    m_bufferSampleRate = targetRate;

    double step = m_sourceRate / targetRate;

    m_pcmBuffer.clear();
    m_pcmBuffer.reserve((size_t)(m_rawBuffer.size() / step) + 1);

    double pos = 0;

    if (m_qualityMode == dpcmMode) {
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
    else { // adpcmMode
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

void RhythmPad::clearBuffer() {
    m_pcmBuffer.clear();
    m_rawBuffer.clear();
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
            pad.start(velocity, isLegato, finalFreq, phaseOffsetNorm, m_unisonTotal);
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

    for (auto& pad : pads) {
        pad.setModulationWheel(m_modWheel);
    }
}

void RhythmCore::setPitchBendRatio(float ratio)
{
    m_pitchBendRatio = ratio;

    for (auto& pad : pads) {
        pad.setPitchBend(m_pitchBendRatio);
    }
}

void RhythmCore::getSampleStereo(float& outL, float& outR)
{
    outL = 0.0f;
    outR = 0.0f;

    if (!isPlaying()) return;

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

            float sample = pad.getSample() * 4.0f;

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

void RhythmCore::clearBuffer(int padIndex) {
    pads[padIndex].clearBuffer();
}
