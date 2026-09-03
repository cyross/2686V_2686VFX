#include "./SynthWtPlus.h"

#include "../../Core/Synth/SynthHelpers.h"

WtPlusCore::WtPlusCore() : SynthCore()
{
    // 波形はプロセッサ側のスロットを参照するだけなので、ここでは何もしない
}

void WtPlusCore::prepare(double sampleRate)
{
    if (sampleRate > 0.0) m_sampleRate = sampleRate;

    m_adsr.prepare(m_sampleRate);
    m_pitchAdsr.prepare(0, m_sampleRate);
    m_ssgSwEnv.prepare(0, m_sampleRate);
    m_ssgSwEnv11.prepare(0, m_sampleRate);
    m_ssgSwPenv11.prepare(0, m_sampleRate);
    m_ssgHwEnv.prepare(m_sampleRate);
    m_ssgHwPEnv.prepare(m_sampleRate);

    m_targetRate = getTargetRate(m_rateIndex);

    m_lfo.prepare(m_targetRate);

    updatePhaseDelta();
}

void WtPlusCore::setSampleRate(double sampleRate)
{
	if (sampleRate > 0.0) m_sampleRate = sampleRate;

    m_adsr.updateSampleRate(m_sampleRate);
	m_pitchAdsr.updateSampleRate(m_sampleRate);
	m_ssgSwEnv.updateSampleRate(m_sampleRate);
    m_ssgSwEnv11.updateSampleRate(m_sampleRate);
    m_ssgSwPenv11.updateSampleRate(m_sampleRate);
    m_ssgHwEnv.updateSampleRate(m_sampleRate);
    m_ssgHwPEnv.updateSampleRate(m_sampleRate);

    updatePhaseDelta();
}

void WtPlusCore::setParameters(const SynthParams& params)
{
    m_level = params.wtPlus.level;

    m_fixMode.setParameters(params.wtPlus.fix);

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_adsr.setParameters(params.wtPlus.adsr);
    m_pitchAdsr.setParameters(params.wtPlus.pitchAdsr);
	m_ssgSwEnv.setParameters(params.wtPlus.ssgSwEnv);
    m_ssgSwEnv11.setParameters(params.wtPlus.ssgSwEnv11);
    m_ssgSwPenv11.setParameters(params.wtPlus.ssgSwPEnv11);
    m_detune.setParameters(params.wtPlus.detune);
    m_lfo.setParameters(params.wtPlus.lfo);
    m_ssgHwEnv.setParameters(params.wtPlus.ssgHwEnv);
    m_ssgHwPEnv.setParameters(params.wtPlus.ssgHwPEnv);

    // Bit Depth & Table Size
    m_quantizeSteps = getTargetBitDepth(params.wtPlus.quality.bit);

    if (m_rateIndex != params.wtPlus.quality.rate) {
        m_rateIndex = params.wtPlus.quality.rate;

        m_targetRate = getTargetRate(m_rateIndex);

        m_lfo.updateTargetSampleRate(m_targetRate);
    }

    // スロット選択。
    // 即切り替えなので、番号が変わればその場で参照先の波形が変わる。
    // 実機の波形メモリを書き換えたときと同じで、段差が出るのは仕様。
    m_slot = std::clamp(params.wtPlus.slot, 0, Global::WtPlus::slots - 1);

    m_wtMod.setParameters(params.wtPlus.mod);

    m_interpolate = params.wtPlus.interpolate;

    m_pitchResetOnLegato = params.pitchResetOnLegato;

    updatePhaseDelta();
}

void WtPlusCore::noteOn(float freq, float velocity, int midiNote, bool isLegato)
{
    // =====================================================================
    // モノフォニック・レガート時は、音量（ベロシティ）を更新しない！
    // 1音目の音量をそのまま引き継ぐことで、音量ジャンプを完全に防ぐ。
    // =====================================================================
    if (!isLegato) {
        // 新規発音の時だけ、ベロシティからベースレベルを計算する
        m_baseLevel = std::max(0.01f, velocity * 0.25f);
    }
    // (レガート時は m_baseLevel は古い値のまま維持される)

    // ユニゾン・ハーモニー用
    // ユニゾンデチューンの計算
    float finalFreq = m_unison.applyDetune(freq);
 
    // 基本周波数にデチューン成分を加算
    // Save for recalculation
    finalFreq = m_fixMode.noteOn(finalFreq);
    m_currentFrequency = m_detune.noteOn(finalFreq);

    if (!isLegato) {
        if (!m_isMonoMode) {
            m_phase = (m_unison.getPhaseOffset() * juce::MathConstants<float>::twoPi);

            // 位相が 2π を超えた場合は安全にラップアラウンド（折り返し）させる
            while (m_phase >= juce::MathConstants<float>::twoPi) {
                m_phase -= juce::MathConstants<float>::twoPi;
            }
            m_lfo.noteOn();
        }

        m_wtMod.reset();
        m_rateAccumulator = 0.0;
        m_lastSample = 0.0f;
    }

    updatePhaseDelta();

    if (!isLegato) {
        m_currentLevel = m_adsr.noteOn();

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
        m_ssgHwPEnv.noteOn();
    }

    if (!m_pitchAdsr.isBypass() && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }

    if (!m_ssgSwPenv11.isBypass() && m_pitchResetOnLegato) {
        m_ssgSwPenv11.noteOn();
    }
}

void WtPlusCore::noteOff()
{
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

bool WtPlusCore::isPlaying() const
{
    return m_adsr.isPlaying() || m_ssgSwEnv.isPlaying() || m_ssgSwEnv11.isPlaying();
}

// ピッチベンド (0 - 16383, Center=8192)
void WtPlusCore::setPitchBend(int pitchWheelValue)
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
void WtPlusCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_wtMod.setModWheel((float)wheelValue / 127.0f);
}

void WtPlusCore::setPitchBendRatio(float ratio)
{
    m_pitchBendRatio = ratio;
}

float WtPlusCore::getSample()
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

    if (!isAllAmpBypassed) {
        // 1. ADSR処理
        if (!m_adsr.isBypass()) {
            m_currentLevel = m_adsr.process(m_currentLevel);
            finalEnv *= m_currentLevel;
        }
        else {
            if (m_adsr.isRelease()) {
                m_adsr.bypassedReleasedProcess();
            }
            else {
                m_currentLevel = m_adsr.process(m_currentLevel);
                finalEnv *= m_currentLevel;
            }
        }

        // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
        if (!m_ssgSwEnv.isBypass()) {
            finalEnv *= m_ssgSwEnv.process();
        }
        else {
            if (m_ssgSwEnv.isRelease()) m_ssgSwEnv.bypassedReleasedProcess();
        }

        // 3. SSG Sw Env 11 処理
        if (!m_ssgSwEnv11.isBypass()) {
            finalEnv *= m_ssgSwEnv11.process();
        }
        else {
            if (m_ssgSwEnv11.isRelease()) m_ssgSwEnv11.bypassedReleasedProcess();
        }
    }

    float newPhaseDelta = m_pitchAdsr.process(m_phaseDelta);
    newPhaseDelta = m_ssgSwPenv11.process(newPhaseDelta);

    // --- Sample Rate Emulation ---
    double targetRate = getTargetRate(m_rateIndex);
    double step = targetRate / m_sampleRate;
    m_rateAccumulator += step;

    // --- Wavetable Synthesis ---
    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        // --- Synthesis at Target Rate ---

        m_lfo.getSample();

        // ==========================================
        // Opzx7 LFO の計算 (AM / PM)
        // ==========================================

        // 1. Amplitude Modulation (AM / 音量)
        float amMultiplier = 1.0f;
        if (m_lfo.am.enable) {
            float attenDb = m_lfo.value.am * m_lfo.am.depthDb;
            amMultiplier = std::pow(10.0f, -attenDb / 20.0f);
        }

        // 2. Pitch Modulation (PM / 音程)
        float pitchModCents = 0.0f;
        if (m_lfo.pm.enable) {
            pitchModCents += m_lfo.value.pm * m_lfo.pm.depthCent;
        }
        float opzx7PitchMod = std::pow(2.0f, pitchModCents / 1200.0f);

        // ==========================================
        // Wavetable固有の Modulation (Vibrato / Table Scan)
        // ==========================================
        // MODULATION
        // ==========================================
        // 計算は WtModulator (Generator/WtMod) にある。
        // FM 音源のチップ全体にも同じものを掛けている。
        float modRatio = m_wtMod.process(newPhaseDelta) * m_ssgHwPEnv.process(1.0f);

        // ==========================================
        // 位相 (Phase) の計算
        // ==========================================
        // PitchBend、Opzx7 の PM 倍率、FDS 変調の周波数比を掛け合わせる
        float currentDelta = newPhaseDelta * m_pitchBendRatio * opzx7PitchMod * modRatio;

        float effectivePhase = m_phase;
        effectivePhase -= std::floor(effectivePhase);
        if (effectivePhase < 0.0f) effectivePhase += 1.0f;

        // ==========================================
        // 波形テーブルのルックアップ (線形補間でノイズ除去)
        // ==========================================
        // 選択中のスロットを直接読む。テーブルサイズは読み込んだ
        // ファイル本来のサンプル数なので、階段の粗さがファイルどおりになる。
        static const WtPlusWaveSlot emptyWaveSlot;

        const WtPlusWaveSlot& waveSlot =
            (m_waveSlots != nullptr) ? (*m_waveSlots)[m_slot] : emptyWaveSlot;

        const int tableSize = waveSlot.sampleCount;
        const int resolution = Global::WtPlus::waveResolution;

        float raw1 = 0.0f;
        float raw2 = 0.0f;
        float frac = 0.0f;

        // 波形が入っていないスロットは無音。
        // エンベロープは進めるので、発音そのものはきちんと終了する。
        if (tableSize > 0)
        {
            float readIndex = effectivePhase * (float)tableSize;
            int idx1 = (int)readIndex;
            int idx2 = idx1 + 1;

            if (idx1 >= tableSize) idx1 = tableSize - 1;
            if (idx2 >= tableSize) idx2 = 0; // ループして繋ぐ

            frac = readIndex - (float)idx1;

            int sourceIdx1 = idx1 * (resolution / tableSize);
            int sourceIdx2 = idx2 * (resolution / tableSize);

            if (sourceIdx1 >= resolution) sourceIdx1 = resolution - 1;
            if (sourceIdx2 >= resolution) sourceIdx2 = resolution - 1;

            raw1 = waveSlot.data[sourceIdx1];
            raw2 = waveSlot.data[sourceIdx2];
        }

        // 2つのサンプルの間を滑らかに補間する。
        // OFF にすると補間せず手前の値を保持し、実機の波形メモリと同じ
        // 階段状の出力になる。
        float rawSample = m_interpolate
            ? (raw1 * (1.0f - frac) + raw2 * frac)
            : raw1;

        // 量子化(ビットクラッシャー)の前に AM（トレモロ）を適用する
        rawSample *= amMultiplier;

        // ==========================================
        // 量子化 (UIの正確なスナップロジックと完全同期)
        // ==========================================
        if (m_quantizeSteps > 0.0f) {
            // m_quantizeSteps が 15 (4-bit) の場合、16段階になる
            int totalSteps = (int)m_quantizeSteps + 1;
            int maxIndex = totalSteps - 1;
            int zeroIndex = totalSteps / 2 - 1;

            int stepValue = 0;

            // UI側と全く同じ区画計算を行う
            if (rawSample < 0.0f) {
                stepValue = (int)std::round(rawSample * zeroIndex + zeroIndex);
            }
            else {
                stepValue = (int)std::round(rawSample * (maxIndex - zeroIndex) + zeroIndex);
            }

            stepValue = std::clamp(stepValue, 0, maxIndex);

            // 正確な値 (-1.0 〜 1.0) に戻す
            if (stepValue < zeroIndex) {
                m_lastSample = (float)(stepValue - zeroIndex) / (float)zeroIndex;
            }
            else if (stepValue > zeroIndex) {
                m_lastSample = (float)(stepValue - zeroIndex) / (float)(maxIndex - zeroIndex);
            }
            else {
                m_lastSample = 0.0f; // 完全な 0.0 を保証！（ノイズの元を絶つ）
            }
        }
        else {
            m_lastSample = rawSample;
        }

        // メイン位相を進める
        m_phase += currentDelta;

        if (m_phase >= 1.0f) m_phase -= 1.0f;
    }

    // SSGハードウェアエンベロープ(SsgHwEnv)処理
    float sshHwEnvVal = m_ssgHwEnv.process();

    return m_lastSample * finalEnv * sshHwEnvVal * m_level * m_baseLevel * 8.0f;
 }

void WtPlusCore::updatePhaseDelta()
{
    m_phaseDelta = m_currentFrequency / m_targetRate;
}

void WtPlusCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    // ユニゾン・ハーモニー向けに変更
    float basePanL = 1.0f;
    float basePanR = 1.0f;

    m_unison.applyPan(basePanL, basePanR);
    sample *= m_unison.getGainComp();

    outL[startSample + sampleIdx] += sample * basePanL;
    outR[startSample + sampleIdx] += sample * basePanR;

    isActive = isPlaying();
}
