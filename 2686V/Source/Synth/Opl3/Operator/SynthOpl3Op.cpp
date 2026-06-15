#include "./SynthOpl3Op.h"
#include "../../../Processor/Opl3/ProcessorOpl3Values.h"

const std::array<float, 4> Opl3Operator::dbPerOcts = { 0.0f, 1.5f, 3.0f, 6.0f };

void Opl3Operator::prepare(int opIndex, double sampleRate) {
    m_ampAdsr.prepare(opIndex, sampleRate);
    m_pitchAdsr.prepare(opIndex, sampleRate);
    m_ssgSwEnv.prepare(opIndex, sampleRate);
	m_lfo.prepare(sampleRate);

    m_ampAdsr.setParamMax(
        Opl3PrValue::Op::RgAdsr::Ar::max,
        Opl3PrValue::Op::RgAdsr::Dr::max,
        Opl3PrValue::Op::RgAdsr::Sl::max,
        Opl3PrValue::Op::RgAdsr::Rr::max,
        Opl3PrValue::Op::RgAdsr::Tl::max
    );
}

void Opl3Operator::setCurveCore(CurveCore* p_curveCore)
{
    m_ampAdsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv.setCurveCore(p_curveCore);
}

void Opl3Operator::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;

    m_lfo.updateTargetSampleRate(sampleRate);
    m_ampAdsr.updateTargetSampleRate(sampleRate);
    m_pitchAdsr.updateSampleRate(sampleRate);
    m_ssgSwEnv.updateTargetSampleRate(sampleRate);
}

void Opl3Operator::setParameters(const Opl3OpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = 1.0f;
    m_params.waveSelect = params.waveSelect;
    m_detune.setParameters(params.multiple);
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

void Opl3Operator::noteOn(float frequency, float velocity, int noteNumber, bool isLegato)
{
    m_noteNumber = noteNumber;

    float oldTargetLevel = m_targetLevel;

    if (!isLegato)
    {
        m_ssgPhase = 0.0;

        if (!m_isMonoMode) {
            // ユニゾン・ハーモニー向け対応
            // m_unisonPhaseOffset (0.0~1.0) に 2π を掛けてラジアンにしてから足す！
            m_phase = (m_unisonPhaseOffset * juce::MathConstants<float>::twoPi);

            // 位相が 2π を超えた場合は安全にラップアラウンド（折り返し）させる
            while (m_phase >= juce::MathConstants<float>::twoPi) {
                m_phase -= juce::MathConstants<float>::twoPi;
            }

            m_currentLevel = 0.0f;
        }
    }

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    float finalFreq = m_detune.noteOn(baseFreq);

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

    if (!isLegato) {
        if (!m_ampAdsr.isBypass()) {
            m_targetLevel = m_ampAdsr.noteOn(velocity, noteNumber);
        }
        else {
            m_targetLevel = velocity;
        }

        // =====================================================================
        // モノフォニック時のアタックノイズ（音量ジャンプ）防止処理
        // =====================================================================
        if (m_isMonoMode && m_currentLevel > 0.0f) {
            if (m_targetLevel > 0.0001f) {
                // スピーカー出力音量(current * oldTarget)を維持するため、
                // 新しい targetLevel に合わせて currentLevel(割合) を逆算して補正する
                m_currentLevel = m_currentLevel * (oldTargetLevel / m_targetLevel);

                // 割合が 1.0 (100%) を超える場合は、エンベロープ破綻を防ぐためクランプする
                if (m_currentLevel > 1.0f) {
                    m_currentLevel = 1.0f;
                }
            }
            else {
                m_currentLevel = 0.0f;
            }
        }
        // =====================================================================

        if (!m_pitchAdsr.isBypass() && !m_pitchResetOnLegato) {
            m_pitchAdsr.noteOn();
        }

        if (m_params.ssgEnvEnable) {
            m_ssgSwEnv.noteOn();
        }
    }

    if (!m_pitchAdsr.isBypass() && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }

    // KeyScale はピッチ(音程)に依存するため、レガート時も必ず更新する
    if (!m_ampAdsr.isBypass()) {
        m_ampAdsr.updateIncrementsWithKeyScale(m_noteNumber);
    }
}

void Opl3Operator::noteOff()
{
    if (!m_ampAdsr.isBypass()) {
        m_ampAdsr.noteOff();
    }

    if (m_params.pitchEnvEnable) {
        m_pitchAdsr.noteOff();
    }

    if (m_params.ssgEnvEnable) {
        m_ssgSwEnv.noteOff();
    }
}

void Opl3Operator::processLfo()
{
    m_lfo.getSample();
}

void Opl3Operator::getSample(float& output, float modulator, float feedbackModulator)
{
    m_lfo.getSample();

    if (!isPlaying() && !m_ampAdsr.isBypass()) {
        if (m_params.pitchEnvEnable) {
            m_pitchAdsr.bypassedReleasedProcess();
        }

        if (m_params.ssgEnvEnable) {
            m_ssgSwEnv.bypassedReleasedProcess();
        }

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

    // ========================================================
    // 3. 位相と波形の生成
    // ========================================================
    float feedbackPhaseOffset = 0.0f;
    if (m_feedback > 0.0f && feedbackModulator != 0.0f) {
        // コアから渡された「過去2サンプルの平均値 (feedbackModulator)」にスケールを掛けるだけ
        float fbScale = std::pow(2.0f, m_feedback - 5.0f);
        feedbackPhaseOffset = feedbackModulator * fbScale * juce::MathConstants<float>::pi * 2.0f;
    }

    float basePhaseDelta = m_phaseDelta * m_pitchBendRatio * m_lfo.value.pm;
    float currentPhaseDelta = m_params.pitchEnvEnable ? m_pitchAdsr.process(basePhaseDelta) : basePhaseDelta;

    // --------------------------------------------------------
    // PCM波形への過剰な位相変調を抑え、音量低下を防ぐスケーリング
    // --------------------------------------------------------
    // Modulator からの入力 (変調インデックスは実機では通常 4π ではなく 2π〜8πの範囲ですが、
    // ここは既存のシステムに合わせておきます)
    float fmModIndex = 4.0f * juce::MathConstants<float>::pi;

    // 位相の変調
    float modulatedPhase = m_phase + (modulator * fmModIndex) + feedbackPhaseOffset;

    // エンベロープが「掛かる前」の生の波形を取得
    float rawWave = calcWaveform(modulatedPhase, m_params.waveSelect);

    // 最後にエンベロープを掛けて出力とする
    output = rawWave * envVal;

    // m_phase の更新とラップアラウンドもラジアンで行う
    m_phase += currentPhaseDelta;

    while (m_phase >= 2.0f * juce::MathConstants<float>::pi) m_phase -= 2.0f * juce::MathConstants<float>::pi;
    while (m_phase < 0.0f) m_phase += 2.0f * juce::MathConstants<float>::pi;
}


float Opl3Operator::calcWaveform(double phase, int wave)
{
    auto doubleSine = [](float p)
        {
            return std::sin(p * 2.0f);
        };

    // 1. まず phase を 0.0 ～ 2π の範囲に丸め込む (ラジアン)
    float p = std::fmod((float)phase, 2.0f * juce::MathConstants<float>::pi);
    if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;

    // サイン波はラジアンで計算
    float s = std::sin(p);

    // 波形生成ロジック用に、0.0 ～ 1.0 に正規化された位相を作る！
    float normPhase = p / (2.0f * juce::MathConstants<float>::pi);

    int safeWave = std::clamp(wave, 0, 10);

    switch (safeWave) {
    case 0:
        // 0: Sine
        return s;
    case 1:
        // 1: Half Sine
        return normPhase < 0.5f ? s : 0.0f;
    case 2:
        // 2: Abs Sine
        return std::abs(s);
    case 3:
        // 3: Pulse Sine
        return normPhase < 0.25f ? s : 0.0f;
    case 4:
        // 4: Alternative Sine
        return normPhase < 0.5f ? doubleSine(p) : 0.0f;
    case 5:
        // 5: Alternative Abs Sine
        return normPhase < 0.5f ? std::abs(doubleSine(p)) : 0.0f;
    case 6:
        // 6: Square
        return normPhase < 0.5f ? 1.0f : -1.0f;
    case 7:
        // 7: Derived Square
        return std::sin(p / 2.0f + 0.5f * juce::MathConstants<float>::pi);
    case 8:
        // 8: [EX01]Round Square
        return ((normPhase < 0.5f) ? 1.0f : -1.0f) * (1.0f - std::pow(1.0f - std::abs(s), 4.0f));
    case 9:
        // 9: [EX02]Log Saw
        return (1.0f - normPhase * 2.0f) * (1.0f - normPhase * 2.0f) * (1.0f - normPhase * 2.0f);
    }

    return s;
}
