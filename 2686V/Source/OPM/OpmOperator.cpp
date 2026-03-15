#include "OpmOperator.h"

void OpmOperator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_phase = m_params.phaseOffset;
    m_ssgPhase = 0.0;
    m_noteNumber = noteNumber;
    //m_currentLevel = 0.0f;

    m_lfoDelayCounter = 0.0f;

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

    // Final Frequency = (Base + DT1) * MUL * DT2
    float finalFreq = detunedBaseFreq * mul * dt2Scales[m_params.detune2 & 3];

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

    m_targetLevel = velocity * tlGain;
    m_state = State::Attack;

    m_fb1 = 0.0f; m_fb2 = 0.0f;

    updateIncrementsWithKeyScale();

    m_currentReleaseDec = m_releaseDec;
}

void OpmOperator::getSample(float& output, float modulator, float amLfoVal, float pmLfoVal,
    bool globalPm, bool globalAm, int globalPms, int globalAms, float globalPmd, float globalAmd, float modWheel)
{
    if (m_state == State::Idle) { output = 0.0f; return; }

    updateEnvelopeState();
    float envVal = m_currentLevel;

    // ========================================================
    // 1. Amplitude Modulation (Tremolo / Wah) の計算
    // ========================================================
    float amsDepths[] = { 0.0f, 0.93622f, 0.99593f, 0.99998f };
    float totalAmDepth = 0.0f;

    // ① グローバルAM (G-AMスイッチがONの時のみ受け取る)
    if (globalAm) {
        float globalDepthScale = (globalAmd >= 0.0f) ? (globalAmd / 127.0f) : 1.0f;
        totalAmDepth += amsDepths[std::clamp(globalAms, 0, 3)] * globalDepthScale;
    }

    // 上限を1.0(100%)でクリップ
    totalAmDepth = std::min(totalAmDepth, 1.0f);

    if (totalAmDepth > 0.0f) {
        float lfoAmpMod = 1.0f - (amLfoVal * totalAmDepth);
        envVal *= lfoAmpMod; // 音量に直接適用
    }

    // ========================================================
    // 2. Pitch Modulation (Vibrato) の計算
    // ========================================================
    // (2^(Cent/1200) - 1.0)
    // 0: 0cent / 1: ±5cent / 2: ±10cent / 3: ±20cent / 4: ±50cent / 5: ±100cent / 6: ±400cent / 7: ±700cent
    float pmsDepths[] = { 0.0f, 0.002892f, 0.005793f, 0.011619, 0.029302, 0.059463f, 0.259921f, 0.498307f };
    float totalPmDepth = 0.0f;

    // ① グローバルPM (G-PMスイッチがONの時のみ受け取る)
    if (globalPm) {
        float globalPmdScale = (globalPmd >= 0.0f) ? (globalPmd / 127.0f) : 1.0f;
        totalPmDepth += pmsDepths[std::clamp(globalPms, 0, 7)] * globalPmdScale;
    }

    // PMがONの時だけ、その深さをLFO波形に掛ける
    float currentPitchMod = pmLfoVal * totalPmDepth;

    // ③ モジュレーションホイール (MIDI演奏のため常に足し込む)
    float wheelDepth = modWheel * 0.03f;
    currentPitchMod += (pmLfoVal * wheelDepth);

    float lfoPitchMod = 1.0f + currentPitchMod;

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

