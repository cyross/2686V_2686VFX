#include "OpnaOperator.h"

#include "../fm/FmCore.h"

void OpnaOperator::setParameters(const FmOpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = params.fmSsgEgFreq;
    m_params.waveSelect = 0;
}

void OpnaOperator::noteOn(float frequency, float velocity, int noteNumber)
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

    if (m_params.fixedMode) {
        baseFreq = m_params.fixedFreq;
    }

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

void OpnaOperator::getSample(float& output, float modulator, float amLfoVal, float pmLfoVal,
    bool globalPm, bool globalAm, int globalPms, int globalAms, float globalPmd, float globalAmd, float modWheel)
{
    if (m_state == State::Idle) { output = 0.0f; return; }

    updateEnvelopeState();
    float envVal = m_currentLevel;

    if (m_params.ssgEg > 0) {
        int safeWave = std::clamp(m_params.ssgEg, 0, 15);

        envVal *= ssgWaveStrategies[safeWave](m_ssgPhase);
        m_ssgPhase += (double)m_ssgEgFreq / m_sampleRate;
    }

    // Sync Delay 更新
    if (m_lfoDelayCounter > 0.0f) {
        m_lfoDelayCounter -= 1.0f / (float)m_sampleRate;
        if (m_lfoDelayCounter < 0.0f) m_lfoDelayCounter = 0.0f;
    }

    float localAmLfo = 0.0f;
    float localPmLfo = 0.0f;

    // LFO-ON スイッチが有効で、かつディレイが終わっている場合のみLFOを進める
    if (m_params.vibEnable || m_params.amEnable) {
        if (m_lfoDelayCounter > 0.0f) {
        }
        else {
            // LFO FREQ の選択 (0〜7)
            const float lfoFreqs[8] = { 3.98f, 5.56f, 6.02f, 6.37f, 6.88f, 9.63f, 48.1f, 72.2f };
            float currentLfoFreq = lfoFreqs[std::clamp(m_params.lfoFreqIndex, 0, 7)]; // パラメータから取得

            m_lfoPhase += (double)currentLfoFreq / m_hostSampleRate;
            if (m_lfoPhase >= 1.0) m_lfoPhase -= 1.0;

            // FmCore の波形ストラテジーを利用 (例: 三角波=3)
            // ノイズはオペレーター単位では持たずダミー(0.0)を渡すか、必要な場合は実装します
            FmCore::LfoResult lfoVal = FmCore::lfoStrategies[3](m_lfoPhase, 0.0f);

            localPmLfo = lfoVal.pm;
            localAmLfo = lfoVal.am;
        }
    }

    // AMスムージング (クリックノイズ防止)
    m_amSmooth += (localAmLfo - m_amSmooth) * 0.005f;

    // ========================================================
    // 1. Amplitude Modulation (Tremolo) の計算
    // ========================================================
    // 新しい厳密なAMSデプス値 (0, 1.4dB, 5.9dB, 11.8dB)
    const float amsDepths[4] = { 0.0f, 0.14886f, 0.49305f, 0.74296f };
    float totalAmpMod = 1.0f; // 最終的に音量に掛ける倍率

    // ① グローバルAM (引数で渡ってきた amLfoVal を使う)
    if (globalAm) {
        // グローバルAMS (-127〜127) と OP毎のAMD (0〜15) を合成
        float amSign = (globalAmd < 0) ? -1.0f : 1.0f;
        float amDepthNorm = (std::abs((float)globalAmd) / 127.0f) * ((float)m_params.ams / 15.0f);

        // amLfoVal は 0.0 ~ 1.0。AMSがマイナスの場合は波形を反転(1.0 - x)させる
        float unipolarLfo = (amSign > 0.0f) ? amLfoVal : (1.0f - amLfoVal);

        // 最大減衰量を 95.25dB (TLレジスタの最大値 127 * 0.75dB) として計算
        float attenuationDb = unipolarLfo * amDepthNorm * 95.25f;

        // デシベルをリニアな音量倍率に変換
        totalAmpMod = std::pow(10.0f, -attenuationDb / 20.0f);
    }

    // ② ローカルAM (ローカルの m_amSmooth を使う)
    if (m_params.amEnable) {
        float lDepth = amsDepths[std::clamp(m_params.ams, 0, 3)];
        totalAmpMod *= (1.0f - (m_amSmooth * lDepth));
    }

    // 両方のAMをエンベロープに適用
    envVal *= totalAmpMod;

    // ========================================================
    // 2. Pitch Modulation (Vibrato) の計算
    // ========================================================
    // 新しい厳密なPMSデプス値 (0, 3.4, 6.7, 10, 14, 20, 40, 80 cent)
    const float pmsDepths[8] = { 0.0f, 0.001965f, 0.003876f, 0.005793f, 0.008122f, 0.011619f, 0.023374f, 0.047294f };
    float lfoPitchMod = 1.0f;

    // ① グローバルPM (引数で渡ってきた pmLfoVal を使う)
    if (globalPm) {
        // グローバルPMS (-127〜127) と グローバルPMD (0〜15) を合成
        // PMSがマイナスならそのまま掛け算結果もマイナスになり、位相が反転する
        float pmDepthNorm = ((float)globalPmd / 127.0f) * (globalPms / 15.0f);

        // 最大で ±1オクターブ (1200セント) の揺れ幅と定義する
        // pmLfoVal は -1.0 ~ 1.0
        float pitchCentDeviation = pmLfoVal * pmDepthNorm * 1200.0f;

        // セント値を周波数の倍率に変換 (2 ^ (cent / 1200))
        lfoPitchMod = std::pow(2.0f, pitchCentDeviation / 1200.0f);
    }

    // ② ローカルPM (ローカルの localPmLfo を使う)
    if (m_params.vibEnable) {
        lfoPitchMod += localPmLfo * pmsDepths[std::clamp(m_params.pms, 0, 7)];
    }

    // ③ モジュレーションホイール (Global LFO を使う)
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

