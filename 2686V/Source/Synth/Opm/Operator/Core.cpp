#include "./Core.h"

void OpmOperator::setParameters(const OpmOpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = 1.0f;
    m_params.ssgEg = 0;
    m_params.waveSelect = 0;
    m_ampAdsr.setParameters(params.m_adsrParams);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_ssgSwEnv.setParameters(params.ssgSwEnv);
    m_detune.setParameters(params.detune, params.detune2, params.multiple);
	m_fixMode.setParameters(params.fixedMode, params.fixedFreq);
}

void OpmOperator::updateTargetSampleRate(double newSampleRate)
{
    m_sampleRate = newSampleRate;

    m_ampAdsr.updateTargetSampleRate(newSampleRate);
    m_pitchAdsr.updateSampleRate(newSampleRate);
    m_ssgSwEnv.updateTargetSampleRate(newSampleRate);
}

void OpmOperator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_phase = 0.0f;
    m_ssgPhase = 0.0;
    m_noteNumber = noteNumber;
    //m_currentLevel = 0.0f;

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

void OpmOperator::noteOff()
{
    m_ampAdsr.noteOff();
    m_pitchAdsr.noteOff();
    m_ssgSwEnv.noteOff();
}

void OpmOperator::getSample(float& output, float modulator, const OpmLfoCore& hwLfo, float modWheel)
{
    if (!isPlaying()) {
        // ADSRとSwEnvの両方がバイパスの時は、完全な矩形波（Gate）動作
        // ピッチエンベロープは強制的に終了させる（そうしないと、次のノートオンでピッチが変になったりする）
        m_pitchAdsr.bypassedReleasedProcess();

        output = 0.0f;

        return;
    }

    float envVal = 1.0f;

    // 1. 従来のADSR処理 (内部の m_currentLevel はADSR専用として維持する)
    m_currentLevel = m_ampAdsr.updateEnvelopeState(m_currentLevel);
    envVal *= m_currentLevel; // 掛け算

    // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
    if (!m_ssgSwEnv.isBypassed()) {
        envVal *= m_ssgSwEnv.process(); // 掛け算
    }
    else {
        if (m_ssgSwEnv.isRelease()) m_ssgSwEnv.bypassedReleasedProcess();
    }

    // ========================================================
    // 1. Amplitude Modulation (Tremolo / Wah) の計算
    // ========================================================
    float totalAmDepth = 0.0f;

    // ① グローバルAM (G-AMスイッチがONの時のみ受け取る)
    if (hwLfo.amEnable)
    {
        totalAmDepth += (m_params.amsEnable ? hwLfo.ams : 1.0f) * hwLfo.depthDb;
    }

    // 上限を1.0(100%)でクリップ
    totalAmDepth = std::min(totalAmDepth, 1.0f);

    if (totalAmDepth > 0.0f) {
        float lfoAmpMod = 1.0f - (hwLfo.value.am * totalAmDepth);
        envVal *= lfoAmpMod; // 音量に直接適用
    }

    // ========================================================
    // 2. Pitch Modulation (Vibrato) の計算
    // ========================================================
    // (2^(Cent/1200) - 1.0)
    // 0: 0cent / 1: ±5cent / 2: ±10cent / 3: ±20cent / 4: ±50cent / 5: ±100cent / 6: ±400cent / 7: ±700cent
    float totalPmDepth = 0.0f;

    // ① グローバルPM (G-PMスイッチがONの時のみ受け取る)
    if (hwLfo.pmEnable) {
        totalPmDepth += hwLfo.pms * hwLfo.depthCent;
    }

    // PMがONの時だけ、その深さをLFO波形に掛ける
    float currentPitchMod = hwLfo.value.pm * totalPmDepth;

    // ③ モジュレーションホイール (MIDI演奏のため常に足し込む)
    float wheelDepth = modWheel * 0.03f;
    currentPitchMod += (hwLfo.value.pm * wheelDepth);

    float lfoPitchMod = 1.0f + currentPitchMod;

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
    if (m_phase >= 2.0 * juce::MathConstants<float>::pi) m_phase -= 2.0 * juce::MathConstants<float>::pi;
}
