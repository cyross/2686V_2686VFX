#include "./Core.h"

void OpnOperator::setParameters(const OpnOpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_params.waveSelect = 0;
    m_ams = (float)params.n88Ams / 15.0f;
    m_zeroDecay = m_params.rdr == 0;
    // サステインレベル (SL) の計算
    if (m_params.rsl == 15) {
        m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float slDb = m_params.rsl * 3.0f;
        m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }
    m_fixMode.setParameters(params.fixedMode, params.fixedFreq);
    m_detune.setParameters(params.detune, params.multiple);
}

void OpnOperator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_phase = 0.0f;
    m_noteNumber = noteNumber;
    //m_currentLevel = 0.0f;
    
    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = m_fixMode.noteOn(frequency);

    // 基本周波数にデチューン成分を加算
    float finalFreq = m_detune.noteOn(baseFreq);

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

    // レジスタモード: TLレジスタ値から直接減衰量(dB)を計算
    // OPN/OPL共に、実機は 1ステップ = 0.75dB の減衰です。
    float attenuationDb = m_params.rtl * 0.75f;
    float tlGain = std::pow(10.0f, -attenuationDb / 20.0f);

    float kslAttenuation = 1.0f;
    m_targetLevel = velocity * tlGain * kslAttenuation;
    m_state = State::Attack;

    m_fb1 = 0.0f; m_fb2 = 0.0f;

    updateIncrementsWithKeyScale();

    m_currentReleaseDec = m_releaseDec;
}

void OpnOperator::getSample(float& output, float modulator, const N88LfoCore& n88Lfo, float modWheel)
{
    if (m_state == State::Idle) { output = 0.0f; return; }

    updateEnvelopeState();
    float envVal = m_currentLevel;

    // ========================================================
    // 1. Amplitude Modulation (Tremolo) の計算
    // ========================================================
    float totalAmpMod = 1.0f; // 最終的に音量に掛ける倍率

    // ① グローバルAM (引数で渡ってきた amLfoVal を使う)
    if (n88Lfo.amEnable) {
        // amLfoVal は 0.0 ~ 1.0。AMSがマイナスの場合は波形を反転(1.0 - x)させる
        float unipolarLfo = (n88Lfo.signDb > 0.0f) ? n88Lfo.value.am : (1.0f - n88Lfo.value.am);

        // ==========================================================
        // 最大減衰量を実機準拠の「11.8dB」に変更
        // (95.25dBだと音が完全に途切れてしまい、ブツブツ音の直接の原因になります)
        // ==========================================================
        float attenuationDb = unipolarLfo * (n88Lfo.depthDb * this->m_ams) * maxAmDepthDb;

        // デシベルをリニアな音量倍率に変換
        totalAmpMod = std::pow(10.0f, -attenuationDb / 20.0f);
    }

    // 両方のAMをエンベロープに適用
    envVal *= totalAmpMod;

    // ========================================================
    // 2. Pitch Modulation (Vibrato) の計算
    // ========================================================
    float lfoPitchMod = 1.0f;

    // ① グローバルPM (引数で渡ってきた pmLfoVal を使う)
    if (n88Lfo.pmEnable) {
        // グローバルPMS (-127〜127) と グローバルPMD (0〜15) を合成
        // PMSがマイナスならそのまま掛け算結果もマイナスになり、位相が反転する
        // 最大で ±1オクターブ (1200セント) の揺れ幅と定義する
        // pmLfoVal は -1.0 ~ 1.0
        // セント値を周波数の倍率に変換 (2 ^ (cent / 1200))
        lfoPitchMod = std::pow(2.0f, (n88Lfo.value.pm * n88Lfo.depthNorm * 1200.0f) / 1200.0f);
    }

    // ③ モジュレーションホイール (Global LFO を使う)
    float wheelCent = n88Lfo.value.pm * (modWheel * 200.0f);
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

void OpnOperator::updateIncrementsWithKeyScale()
{
    if (m_sampleRate <= 0.0) return;

    // ====================================================================
    // 実機のアルゴリズムで増減量を計算
    // ====================================================================
    // 1. キースケールレート (KSR) の算出
    int ksrValue = 0;

    int octave = (m_noteNumber / 12) - 1;
    if (octave < 0) octave = 0;
    if (octave > 7) octave = 7;

    int noteOffset = m_noteNumber % 12;
    int keyRate = (octave * 2) + ((noteOffset > 7) ? 1 : 0);
    ksrValue = keyRate >> (3 - std::clamp(m_params.keyScale, 0, 3));

    // 2. レジスタ値から実効レート(0~63)を算出し、インクリメントに変換する関数
    // isRRフラグを追加し、RRの時だけスケールを調整する
    auto calcRegRate = [&](int regVal, bool isRR) -> float {
        // RR以外のRate0は停止（サステイン維持など）。
        if (regVal == 0 && !isRR) return 0.0f;

        int baseRate = regVal;

        // OPL系（全て4bit）、およびOPN系のRR（4bit）は、5bit(0-31)スケールに補正する
        if (isRR) {
            // 15の時に31になるように (val * 2 + 1)
            baseRate = (regVal * 2) + 1;
        }

        // DAW向け安全装置: RRが0（baseRateが1）の場合でも、永遠に鳴り止まないのを防ぐため
        // 非常にゆっくり（約20秒）減衰して消えるようにする。
        if (baseRate <= 1 && isRR) {
            return 1.0f / (20.0f * (float)m_sampleRate);
        }

        // 実効レート = 基本レート(0-31) * 2 + KSR (0-3)
        int effectiveRate = (baseRate * 2) + ksrValue;
        if (effectiveRate > 63) effectiveRate = 63;

        float timeInSeconds = 0.0f;
        if (effectiveRate >= 60) {
            // Rate 60以上はほぼ瞬時（1ミリ秒）
            timeInSeconds = 0.001f;
        }
        else {
            // レートが4下がるごとに時間が約2倍になる実機カーブの近似
            float powFactor = (60.0f - (float)effectiveRate) / 4.0f;
            timeInSeconds = 0.0015f * std::pow(2.0f, powFactor);
        }

        return 1.0f / (timeInSeconds * (float)m_sampleRate);
        };

    // 各レートの計算（第2引数に、それがRRかどうかのフラグを渡す）
    m_attackInc = calcRegRate(m_params.rar, false);
    m_decayDec = calcRegRate(m_params.rdr, false);
    m_sustainRateDec = (m_params.rsr == 0) ? 0.0f : calcRegRate(m_params.rsr, false);

    // Release Rate の計算時のみ isRR を true にする
    m_releaseDec = calcRegRate(m_params.rrr, true);

    m_susReleaseDec = calcRegRate(5, true);

    // 3. サステインレベル (SL) の計算
    if (m_params.rsl == 15) {
        m_sustain = 0.0f; // SL=15 は一気に0まで落ちる
    }
    else {
        // SL 1ステップにつき 3dB の減衰 (OPN/OPL共通)
        float slDb = m_params.rsl * 3.0f;
        m_sustain = std::pow(10.0f, -slDb / 20.0f);
    }
}

void OpnOperator::updateEnvelopeState()
{
    if (m_state == State::Attack) {
        m_currentLevel += m_attackInc;
        if (m_currentLevel >= 1.0f) { m_currentLevel = 1.0f; m_state = State::Decay; }
    }
    else if (m_state == State::Decay) {
        float limitLevel = m_sustain;

        // DR(Decay Rate)が0の時は、減衰せずに1.0を永遠に維持する
        if (m_zeroDecay)
        {
            m_currentLevel = 1.0;
            m_state = State::Sustain;
        }
        else if (m_currentLevel > limitLevel) {
            if (m_decayDec > 0.0f) {
                m_currentLevel -= m_decayDec;
                if (m_currentLevel <= limitLevel) {
                    m_currentLevel = limitLevel;
                    m_state = State::Sustain;
                }
            }
        }
        else {
            m_currentLevel = limitLevel;
            m_state = State::Sustain;
        }
    }
    else if (m_state == State::Sustain) {
        // SR(Sustain Rate / OPMではD2R) でゆっくり減衰する
        if (m_sustainRateDec > 0.0f) {
            m_currentLevel -= m_sustainRateDec;
            if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
        }
    }
    else if (m_state == State::Release) {
        m_currentLevel -= m_currentReleaseDec;
        if (m_currentLevel <= 0.0f) { m_currentLevel = 0.0f; m_state = State::Idle; }
    }
}
