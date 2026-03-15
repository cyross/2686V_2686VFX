#include "OpnOperator.h"

void OpnOperator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_phase = m_params.phaseOffset;
    m_ssgPhase = 0.0;
    m_noteNumber = noteNumber;
    //m_currentLevel = 0.0f;

    m_lfoDelayCounter = m_params.lfoSyncDelay / 1000.0f;

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    // --- 通常の波形（Sineなど）の場合 ---
    if (m_params.fixedMode) {
        baseFreq = m_params.fixedFreq;
    }

    // 基本周波数にデチューン成分を加算
    float detunedBaseFreq = baseFreq + baseFreq * dtScales[m_params.detune & 7];

    // Multi & Detune
    float mul = (m_params.multiple == 0) ? 0.5f : (float)m_params.multiple;

    // Final Frequency = (Base + DT1) * MUL
    float finalFreq = detunedBaseFreq * mul;

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

    // TL Calculation
    float tlGain = 0.0f;
    if (m_params.regEnable)
    {
        // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
        // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
        float attenuationDb = m_params.rtl * 0.75f;
        tlGain = std::pow(10.0f, -attenuationDb / 20.0f);
    }
    else
    {
        // 従来モード: 0.0〜1.0 を 0dB〜-96dB にマッピング
        if (m_params.totalLevel < 0.99f) {
            float attenuationDb = m_params.totalLevel * 96.0f;
            tlGain = std::pow(10.0f, -attenuationDb / 20.0f);
        }
        else {
            tlGain = 0.0f;
        }
    }

    float kslAttenuation = 1.0f;
    m_targetLevel = velocity * tlGain * kslAttenuation;
    m_state = State::Attack;

    m_fb1 = 0.0f; m_fb2 = 0.0f;

    updateIncrementsWithKeyScale();

    m_currentReleaseDec = m_releaseDec;
}

void OpnOperator::getSample(float& output, float modulator, float amLfoVal, float pmLfoVal,
    bool globalPm, bool globalAm, int globalPms, int globalAms, float globalPmd, float globalAmd, float modWheel)
{
    if (m_state == State::Idle) { output = 0.0f; return; }

    updateEnvelopeState();
    float envVal = m_currentLevel;

    // Sync Delay 更新
    if (m_lfoDelayCounter > 0.0f) {
        m_lfoDelayCounter -= 1.0f / (float)m_hostSampleRate;
        if (m_lfoDelayCounter < 0.0f) m_lfoDelayCounter = 0.0f;
    }

    // ========================================================
    // 1. Amplitude Modulation (Tremolo / Wah) の計算
    // ========================================================
    float totalAmpMod = 1.0f;

    if (globalAm) {
        // グローバルAMS (-127〜127) と OP毎のAMD (0〜15) を合成
        float amSign = (globalAms < 0) ? -1.0f : 1.0f;
        float amDepthNorm = (std::abs((float)globalAms) / 127.0f) * ((float)m_params.amd / 15.0f);

        // amLfoVal は 0.0 ~ 1.0。AMSがマイナスの場合は波形を反転(1.0 - x)させる
        float unipolarLfo = (amSign > 0.0f) ? amLfoVal : (1.0f - amLfoVal);

        // 最大減衰量を 95.25dB (TLレジスタの最大値 127 * 0.75dB) として計算
        float attenuationDb = unipolarLfo * amDepthNorm * 95.25f;

        // デシベルをリニアな音量倍率に変換
        totalAmpMod = std::pow(10.0f, -attenuationDb / 20.0f);
    }

    envVal *= totalAmpMod;

    // ========================================================
    // 2. Pitch Modulation (Vibrato) の計算
    // ========================================================
    float lfoPitchMod = 1.0f;

    if (globalPm) {
        // グローバルPMS (-127〜127) と グローバルPMD (0〜15) を合成
        // PMSがマイナスならそのまま掛け算結果もマイナスになり、位相が反転する
        float pmDepthNorm = ((float)globalPms / 127.0f) * (globalPmd / 15.0f);

        // 最大で ±1オクターブ (1200セント) の揺れ幅と定義する
        float pitchCentDeviation = pmLfoVal * pmDepthNorm * 1200.0f;

        // セント値を周波数の倍率に変換 (2 ^ (cent / 1200))
        lfoPitchMod = std::pow(2.0f, pitchCentDeviation / 1200.0f);
    }

    // モジュレーションホイール分を足し込む (最大200セントの揺れ)
    float wheelCent = pmLfoVal * (modWheel * 200.0f);

    lfoPitchMod *= std::pow(2.0f, wheelCent / 1200.0f);

    // ========================================================
    // 3. 位相と波形の生成
    // ========================================================
    float feedbackMod = 0.0f;
    if (m_feedback > 0.0f) {
        float fbScale = std::pow(2.0f, m_feedback) / 64.0f;
        feedbackMod = (m_fb1 + m_fb2) * 0.5f * fbScale * juce::MathConstants<float>::pi;
    }

    float currentPhaseDelta = m_phaseDelta * m_pitchBendRatio * lfoPitchMod;

    // --------------------------------------------------------
    // PCM波形への過剰な位相変調を抑え、音量低下を防ぐスケーリング
    // --------------------------------------------------------
    float fmModIndex = 4.0f * juce::MathConstants<float>::pi;

    float modulatedPhase = m_phase + (modulator * fmModIndex) + feedbackMod;

    float rawWave = calcWaveform(modulatedPhase, m_params.waveSelect);

    output = rawWave * envVal * m_targetLevel;

    if (!m_isExternalFeedback) {
        m_fb2 = m_fb1;
        m_fb1 = output;
    }

    m_phase += currentPhaseDelta;
    if (m_phase >= 2.0 * juce::MathConstants<float>::pi) m_phase -= 2.0 * juce::MathConstants<float>::pi;
}

