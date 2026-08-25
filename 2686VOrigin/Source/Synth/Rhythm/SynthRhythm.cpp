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
    m_ssgSwEnv11.prepare(0, m_sampleRate);
    m_ssgSwPenv11.prepare(0, m_sampleRate);
    m_lfo.prepare(m_sampleRate);
    m_noiseGen.prepare(m_sampleRate);
    m_ssgHwEnv.prepare(m_sampleRate);
}

void RhythmPad::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;
    m_adsr.updateTargetSampleRate(m_sampleRate);
    m_pitchAdsr.updateTargetSampleRate(m_sampleRate);
    m_ssgSwEnv.updateSampleRate(m_sampleRate);
    m_ssgSwEnv11.updateSampleRate(m_sampleRate);
    m_ssgSwPenv11.updateSampleRate(m_sampleRate);
    m_noiseGen.updateTargetRate(m_sampleRate);
    m_lfo.updateTargetSampleRate(m_sampleRate);
    m_ssgHwEnv.updateSampleRate(m_sampleRate);
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
    m_tone = params.tn.tone;
    m_mix = params.tn.mix;
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

    m_pcmOffset = params.pcm.offset;
    m_pcmRatio = params.pcm.ratio;

    m_adsr.setParameters(params.adsr);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_fixMode.setParameters(params.fix);
    m_ssgSwEnv.setParameters(params.ssgSwEnv);
    m_ssgSwEnv11.setParameters(params.ssgSwEnv11);
    m_ssgSwPenv11.setParameters(params.ssgSwPEnv11);
    m_detune.setParameters(params.detune);
    m_lfo.setParameters(params.lfo);
    m_noiseGen.setParameters({ .level = params.tn.noiseLevel, .noiseOnNote = params.tn.noiseOnNote, .baseFreq = params.tn.noiseFreq });
    m_ssgHwEnv.setParameters(params.ssgHwEnv);

    bool needRefresh = false;
    if (m_qualityMode != params.quality.mode) {
        m_qualityMode = params.quality.mode;
        needRefresh = true; // ADPCM <-> DPCM <-> PCMの切り替えで再生成が必要
    }
    if (m_rateIndex != params.quality.rate) {
        m_rateIndex = params.quality.rate;
        needRefresh = true;
    }
    m_interpolationMode = params.quality.interp;

    m_loopPointEnable = params.lp.enable;
    m_loopPointStart = std::clamp(params.lp.start, 0.0f, 0.999999f);
    m_loopPointEnd = std::clamp(params.lp.end, m_loopPointStart + 0.000001f, 1.0f);

    if (needRefresh) refreshPcmBuffer();
}

void RhythmPad::triggerRelease(double hostSampleRate)
{
    m_isReleased = true;

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
        m_isReleased = false;

        // エンベロープも非レガート時のみ再トリガーする
        m_currentEnv = m_adsr.noteOn();

        if (!m_pitchAdsr.isBypass() && !m_pitchResetOnLegato) {
            m_pitchAdsr.noteOn();
        }

        if (!m_ssgSwEnv.isBypass()) {
            m_ssgSwEnv.noteOn();
        }

        if (!m_ssgSwEnv11.isBypass()) {
            m_ssgSwEnv11.noteOn();
        }

        if (!m_ssgSwPenv11.isBypass()) {
            m_ssgSwPenv11.noteOn();
        }

        m_ssgHwEnv.noteOn();
    }

    if (!m_pitchAdsr.isBypass() && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }

    if (!m_ssgSwPenv11.isBypass() && m_pitchResetOnLegato) {
        m_ssgSwPenv11.noteOn();
    }
}

void RhythmPad::stop()
{
    m_isReleased = true;

    m_adsr.noteOff();

    if (!m_pitchAdsr.isBypass()) {
        m_pitchAdsr.noteOff();
    }

    if (!m_ssgSwEnv.isBypass()) {
        m_ssgSwEnv.noteOff();
    }

    if (!m_ssgSwEnv11.isBypass()) {
        m_ssgSwEnv11.noteOff();
    }

    if (!m_ssgSwPenv11.isBypass()) {
        m_ssgSwPenv11.noteOff();
    }
}

bool RhythmPad::isPlaying() const
{
    return m_adsr.isPlaying() || m_ssgSwEnv.isPlaying() || m_ssgSwEnv11.isPlaying();
}

float RhythmPad::getSample()
{
    // すべてのアンプエンベロープがバイパスされているかどうかを判定
    bool isAllAmpBypassed = m_adsr.isBypass() && m_ssgSwEnv.isBypass() && m_ssgSwEnv11.isBypass();

    if (isAllAmpBypassed) {
        // 全てのアンプエンベロープがバイパスの時は、完全な矩形波（Gate）動作
        // どれかが Release 状態（noteOffが呼ばれた直後）なら、即座に音を消す
        if (m_adsr.isRelease() || m_ssgSwEnv.isRelease() || m_ssgSwEnv11.isRelease()) {
            m_adsr.bypassedReleasedProcess();
            m_ssgSwEnv.bypassedReleasedProcess();
            m_ssgSwEnv11.bypassedReleasedProcess();
            m_pitchAdsr.bypassedReleasedProcess();
            m_ssgSwPenv11.bypassedReleasedProcess();
            return 0.0f;
        }
    }
    else if (!isPlaying()) {
        // いずれかのアンプエンベロープが有効で、全ての再生が終了（音が減衰しきった）時
        // ピッチエンベロープも強制終了させる（次のノートオンでピッチが変になるのを防ぐ）
        m_pitchAdsr.bypassedReleasedProcess();
        m_ssgSwPenv11.bypassedReleasedProcess();
        return 0.0f;
    }

    float finalEnv = 1.0f;

    // --- ADSR & SwEnv Gate Logic ---
    if (m_adsr.isBypass() && m_ssgSwEnv.isBypass() && m_ssgSwEnv11.isBypass())
    {
        // どちらもバイパスの時は完全な矩形波（Gate）動作
        if (m_adsr.isRelease() || m_ssgSwEnv.isRelease() || m_ssgSwEnv11.isRelease()) {
            m_adsr.bypassedReleasedProcess();
            m_ssgSwEnv.bypassedReleasedProcess();
            m_ssgSwEnv11.bypassedReleasedProcess();
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

        if (!m_ssgSwEnv11.isBypass()) {
            finalEnv *= m_ssgSwEnv11.process(); // 掛け算
        }
        else {
            if (m_ssgSwEnv11.isRelease()) m_ssgSwEnv11.bypassedReleasedProcess();
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
        double loopStartPos = offsetSamples + playSize * m_loopPointStart;
        double loopEndPos = offsetSamples + playSize * m_loopPointEnd;

        // =========================================================
        // ループ・終了判定
        // =========================================================
        if (m_loopPointEnable) {
            if (!m_isReleased) {
                // リリース前：ループポイント間をループ
                if (m_position >= loopEndPos) {
                    double loopLength = loopEndPos - loopStartPos;
                    if (loopLength > 0.0) {
                        m_position = loopStartPos + std::fmod(m_position - loopEndPos, loopLength);
                    }
                }
            }
            else {
                // リリース後：最後まで再生
                if (m_position >= endPosition) {
                    if (!m_isOneShot) {
                        m_position = offsetSamples + std::fmod(m_position - endPosition, playSize);
                    }
                    else {
                        m_hasFinished = true;
                        return 0.0f;
                    }
                }
            }
        }
        else {
            // 従来の処理
            if (m_position >= endPosition) {
                if (!m_isOneShot) {
                    m_position = offsetSamples + std::fmod(m_position - endPosition, playSize);
                }
                else {
                    m_hasFinished = true;
                    return 0.0f;
                }
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
        // ループポイントが有効な場合の補間インデックスの折り返し処理を追加
        if (m_loopPointEnable && !m_isReleased) {
            if (idx_0 >= (int)loopStartPos) {
                double loopLength = loopEndPos - loopStartPos;
                if (idx_m1 < (int)loopStartPos) idx_m1 += (int)loopLength;
                if (idx_1 >= (int)loopEndPos) idx_1 -= (int)loopLength;
                if (idx_2 >= (int)loopEndPos) idx_2 -= (int)loopLength;
            }
            else {
                if (idx_m1 < (int)offsetSamples) idx_m1 = (int)offsetSamples;
            }
        }
        else if (m_isOneShot) {
            if (idx_m1 < (int)offsetSamples) idx_m1 = (int)offsetSamples;
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
        double loopStartPos = offsetSamples + playSize * m_loopPointStart;
        double loopEndPos = offsetSamples + playSize * m_loopPointEnd;

        // =========================================================
        // ループ・終了判定
        // =========================================================
        if (m_loopPointEnable) {
            if (!m_isReleased) {
                // リリース前：ループポイント間をループ
                if (m_position >= loopEndPos) {
                    double loopLength = loopEndPos - loopStartPos;
                    if (loopLength > 0.0) {
                        m_position = loopStartPos + std::fmod(m_position - loopEndPos, loopLength);
                    }
                }
            }
            else {
                // リリース後：最後まで再生
                if (m_position >= endPosition) {
                    if (!m_isOneShot) {
                        m_position = offsetSamples + std::fmod(m_position - endPosition, playSize);
                    }
                    else {
                        m_hasFinished = true;
                        return 0.0f;
                    }
                }
            }
        }
        else {
            // 従来の処理
            if (m_position >= endPosition) {
                if (!m_isOneShot) {
                    m_position = offsetSamples + std::fmod(m_position - endPosition, playSize);
                }
                else {
                    m_hasFinished = true;
                    return 0.0f;
                }
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
        // ループポイントが有効な場合の補間インデックスの折り返し処理を追加
        if (m_loopPointEnable && !m_isReleased) {
            if (idx_0 >= (int)loopStartPos) {
                double loopLength = loopEndPos - loopStartPos;
                if (idx_m1 < (int)loopStartPos) idx_m1 += (int)loopLength;
                if (idx_1 >= (int)loopEndPos) idx_1 -= (int)loopLength;
                if (idx_2 >= (int)loopEndPos) idx_2 -= (int)loopLength;
            }
            else {
                if (idx_m1 < (int)offsetSamples) idx_m1 = (int)offsetSamples;
            }
        }
        else if (m_isOneShot) {
            if (idx_m1 < (int)offsetSamples) idx_m1 = (int)offsetSamples;
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

    // SSGハードウェアエンベロープ(SsgHwEnv)処理
    float sshHwEnvVal = m_ssgHwEnv.process();

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
    currentIncrement = m_ssgSwPenv11.process(currentIncrement);

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

    return rawMixed * m_level * finalEnv * m_baseLevel * amMultiplier * sshHwEnvVal;
}

void RhythmPad::refreshPcmBuffer()
{
    // Copy the same resampling & encoding logic as AdpcmCore here
    // (To avoid code duplication, it's best to extract Codec to a separate header, but omitted here)
    if (m_rawBuffer.empty()) return;

    double targetRate = getTargetRate(m_rateIndex);

    if (targetRate > m_sourceRate) targetRate = m_sourceRate;
    m_bufferSampleRate = targetRate;

    m_adsr.prepare(m_bufferSampleRate);
    m_pitchAdsr.prepare(0, m_bufferSampleRate);
    m_ssgSwEnv.prepare(0, m_bufferSampleRate);
    m_ssgSwEnv11.prepare(0, m_bufferSampleRate);
    m_ssgSwPenv11.prepare(0, m_bufferSampleRate);
    m_noiseGen.prepare(m_bufferSampleRate);

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
    float finalFreq = m_unison.applyDetune(freq);
    const float phaseOffsetNorm = m_unison.getPhaseOffset();

    for (auto& pad : pads) {
        if (pad.m_noteNumber == midiNote) {
            // ユニゾン・ハーモニー向けに変更
            // 計算した位相のズレをオペレータに渡す
            pad.start(velocity, isLegato, finalFreq, phaseOffsetNorm, m_unison.getTotal());
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

            m_unison.applyPan(basePanL, basePanR);

            // 注: 他チャンネルは m_unison.getGainComp() による音量補正を掛けているが、
            // Rhythm は元々その補正が適用されていなかったため、現状の音量を維持している。
            // 揃える場合は下記を有効にする (既存プリセットの音量が下がる点に注意)。
            // sample *= m_unison.getGainComp();

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
