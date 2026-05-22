#include "./Core.h"

void OpnOperator::setParameters(const OpnOpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_params.waveSelect = 0;
    m_ams = (float)params.n88Ams / 15.0f;
    m_ampAdsr.setParameters(params.m_adsrParams);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_ssgSwEnv.setParameters(params.ssgSwEnv);
    m_fixMode.setParameters(params.fixedMode, params.fixedFreq);
    m_detune.setParameters(params.detune, params.multiple);
}

void OpnOperator::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;

    m_lfo.updateTargetSampleRate(sampleRate);
    m_ampAdsr.updateTargetSampleRate(sampleRate);
    m_pitchAdsr.updateSampleRate(sampleRate);
    m_ssgSwEnv.updateTargetSampleRate(sampleRate);
}

void OpnOperator::noteOn(float frequency, float velocity, int noteNumber)
{
    //m_phase = 0.0f;
    //m_currentLevel = 0.0f;
    m_noteNumber = noteNumber;
    
    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = m_fixMode.noteOn(frequency);

    // 基本周波数にデチューン成分を加算
    float finalFreq = m_detune.noteOn(baseFreq);

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

    m_targetLevel = m_ampAdsr.noteOn(velocity);

    m_fb1 = 0.0f; m_fb2 = 0.0f;

    m_pitchAdsr.noteOn();
    m_ssgSwEnv.noteOn();

    m_ampAdsr.updateIncrementsWithKeyScale(m_noteNumber);
}

void OpnOperator::noteOff()
{
    m_ampAdsr.noteOff();
    m_pitchAdsr.noteOff();
    m_ssgSwEnv.noteOff();
}

void OpnOperator::getSample(float& output, float modulator, const N88LfoCore& n88Lfo, float modWheel)
{
    if (!isPlaying()) {
        // ADSRとSwEnvの両方がバイパスの時は、完全な矩形波（Gate）動作
        // ピッチエンベロープは強制的に終了させる（そうしないと、次のノートオンでピッチが変になったりする）
        m_pitchAdsr.bypassedReleasedProcess();

        output = 0.0f;

        return;
    }

    // 1. 従来のADSR処理 (内部の m_currentLevel はADSR専用として維持する)
    m_currentLevel = m_ampAdsr.updateEnvelopeState(m_currentLevel);
    float envVal = m_currentLevel;

    // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
    if (m_params.ssgEnvEnable) {
        envVal *= m_ssgSwEnv.process(); // 掛け算
    }

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

    float basePhaseDelta = m_phaseDelta * m_pitchBendRatio * lfoPitchMod;
    float currentPhaseDelta = m_params.pitchEnvEnable ? m_pitchAdsr.process(basePhaseDelta) : basePhaseDelta;

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
    while (m_phase >= 2.0f * juce::MathConstants<float>::pi) m_phase -= 2.0f * juce::MathConstants<float>::pi;
    while (m_phase < 0.0f) m_phase += 2.0f * juce::MathConstants<float>::pi;
}
