#include <array>

#include "./Core.h"

const std::array<float, 4> OplOperator::dbPerOcts = { 0.0f, 1.5f, 3.0f, 6.0f };

namespace {
    // =================================================================
    // 波形ストラテジー配列の定義
    // (引数: ラジアン位相 p, 正規化位相 n, サイン波 s)
    // =================================================================
    using WaveCalculator = float(*)(float p, float n, float s);

    const std::array<WaveCalculator, 4> waveStrategies = { {
        // 0: Sine
        [](float p, float n, float s) { return s; },
        // 1: Half Sine
        [](float p, float n, float s) { return n < 0.5f ? s : 0.0f; },
        // 2: Abs Sine
        [](float p, float n, float s) { return std::abs(s); },
        // 3: Pulse Sine
        [](float p, float n, float s) { return n < 0.25f ? s : 0.0f; },
    } };
}

void OplOperator::prepare(int opIndex, double sampleRate) {
    m_ampAdsr.prepare(opIndex, sampleRate);
    m_pitchAdsr.prepare(opIndex, sampleRate);
    m_ssgSwEnv.prepare(opIndex, sampleRate);
}

void OplOperator::setCurveCore(CurveCore* p_curveCore)
{
    m_ampAdsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv.setCurveCore(p_curveCore);
}

void OplOperator::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;

    m_lfo.updateTargetSampleRate(sampleRate);
	m_ampAdsr.updateTargetSampleRate(sampleRate);
    m_pitchAdsr.updateSampleRate(sampleRate);
    m_ssgSwEnv.updateTargetSampleRate(sampleRate);
}

void OplOperator::setParameters(const OplOpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = 1.0f;
    m_params.waveSelect = params.waveSelect;
    m_ampAdsr.setParameters(params.m_adsrParams);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_ssgSwEnv.setParameters(params.ssgSwEnv);
    m_lfo.setParameters(
        params.amEnable,
        params.vibEnable,
        params.pms,
        params.pmd,
        params.ams,
        params.amd
    );
}

void OplOperator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_ssgPhase = 0.0;
    m_noteNumber = noteNumber;
    //m_phase = 0.0;
    //m_currentLevel = 0.0f;

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    // Multi & Detune
    float mul = (m_params.multiple == 0) ? 0.5f : (float)m_params.multiple;

    // Final Frequency = Base * MUL
    float finalFreq = baseFreq * mul;

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

	m_targetLevel = m_ampAdsr.noteOn(velocity, noteNumber);

    m_fb1 = 0.0f; m_fb2 = 0.0f;

    m_pitchAdsr.noteOn();
    m_ssgSwEnv.noteOn();
    m_ampAdsr.updateIncrementsWithKeyScale(m_noteNumber);
}

void OplOperator::noteOff()
{
    m_ampAdsr.noteOff();
    m_pitchAdsr.noteOff();
    m_ssgSwEnv.noteOff();
}

void OplOperator::processLfo()
{
    m_lfo.getSample();
}

void OplOperator::getSample(float& output, float modulator)
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

    // AM適用 (無条件。変調がない場合はコア側から 1.0 が渡ってくる)
    envVal *= m_lfo.value.am;

    float feedbackMod = 0.0f;
    if (m_feedback > 0.0f) {
        float fbScale = std::pow(2.0f, m_feedback) / 64.0f;
        feedbackMod = (m_fb1 + m_fb2) * 0.5f * fbScale * juce::MathConstants<float>::pi;
    }

    // PM適用 (無条件。変調がない場合は 1.0 が渡ってくる)
    float basePhaseDelta = m_phaseDelta * m_pitchBendRatio * m_lfo.value.pm;
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

float OplOperator::calcWaveform(double phase, int wave)
{
    // 1. まず phase を 0.0 ～ 2π の範囲に丸め込む (ラジアン)
    float p = std::fmod((float)phase, 2.0f * juce::MathConstants<float>::pi);
    if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;

    // サイン波はラジアンで計算
    float s = std::sin(p);

    // 波形生成ロジック用に、0.0 ～ 1.0 に正規化された位相を作る！
    float normPhase = p / (2.0f * juce::MathConstants<float>::pi);

    int safeWave = std::clamp(wave, 0, 3);

    return waveStrategies[safeWave](p, normPhase, s);
}
