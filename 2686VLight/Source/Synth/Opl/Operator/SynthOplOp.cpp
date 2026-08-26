#include <array>

#include "./SynthOplOp.h"
#include "../../../Core/Processor/ProcessorValues.h"

const std::array<float, 4> OplOperator::dbPerOcts = { 0.0f, 1.5f, 3.0f, 6.0f };

void OplOperator::prepare(int opIndex, double sampleRate) {
    m_ampAdsr.prepare(opIndex, sampleRate);
    m_pitchAdsr.prepare(opIndex, sampleRate);
    m_ssgSwEnv.prepare(opIndex, sampleRate);
    m_ssgSwEnv11.prepare(opIndex, sampleRate);
    m_ssgSwPenv11.prepare(opIndex, sampleRate);
    m_lfo.prepare(sampleRate);

    m_ampAdsr.setParamMax(
        CPV::OplRg::Ar::max,
        CPV::OplRg::Dr::max,
        CPV::OplRg::Sl::max,
        CPV::OplRg::Rr::max,
        CPV::OplRg::Tl::max
        );
}

void OplOperator::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;

    m_lfo.updateTargetSampleRate(sampleRate);
	m_ampAdsr.updateTargetSampleRate(sampleRate);
    m_pitchAdsr.updateSampleRate(sampleRate);
    m_ssgSwEnv.updateTargetSampleRate(sampleRate);
    m_ssgSwEnv11.updateSampleRate(sampleRate);
    m_ssgSwPenv11.updateSampleRate(sampleRate);
}

void OplOperator::setParameters(const OplOpParams& params, int feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = 1.0f;
    m_params.waveSelect = params.waveSelect;
    m_ampAdsr.setParameters(params.m_adsrParams);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_ssgSwEnv.setParameters(params.ssgSwEnv);
    m_ssgSwEnv11.setParameters(params.ssgSwEnv11);
    m_ssgSwPenv11.setParameters(params.ssgSwPEnv11);
    m_detune.setParameters(params.detune);
    m_lfo.setParameters(params.lfo);
}

void OplOperator::noteOn(float frequency, float velocity, int noteNumber, bool isLegato)
{
    m_noteNumber = noteNumber;

    float oldTargetLevel = m_targetLevel;

    if (!isLegato)
    {
        m_ssgPhase = 0.0;

        if (!m_isMonoMode) {
            // ユニゾン・ハーモニー向け対応
            // m_unisonPhaseOffset (0.0~1.0) に 2π を掛けてラジアンにしてから足す！
            m_phase = m_unisonPhaseOffset;

            // 位相が 2π を超えた場合は安全にラップアラウンド（折り返し）させる
            while (m_phase >= 1.0) {
                m_phase -= 1.0;
            }

            m_currentLevel = 0.0f;
            m_fb1 = 0.0f;
            m_fb2 = 0.0f;
        }
    }

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    // Final Frequency = Base * MUL
    float finalFreq = m_detune.noteOn(baseFreq);

    // 1 サンプルあたり何周進むか (サイクル単位)
    m_phaseDelta = finalFreq / m_sampleRate;

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

        if (m_params.pitchEnvEnable && !m_pitchResetOnLegato) {
            m_pitchAdsr.noteOn();
        }

        if (m_params.ssgEnvEnable) {
            m_ssgSwEnv.noteOn();
        }

        if (m_params.ssgEnv11Enable && !m_pitchResetOnLegato) {
            m_ssgSwPenv11.noteOn();
        }

        if (m_params.ssgEnv11Enable) {
            m_ssgSwEnv11.noteOn();
        }
    }

    if (m_params.pitchEnvEnable && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }

    if (m_params.ssgPEnv11Enable && m_pitchResetOnLegato) {
        m_ssgSwPenv11.noteOn();
    }

    // KeyScale はピッチ(音程)に依存するため、レガート時も必ず更新する
    if (!m_ampAdsr.isBypass()) {
        m_ampAdsr.updateIncrementsWithKeyScale(m_noteNumber);
    }
}

void OplOperator::noteOff()
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

    if (m_params.ssgEnv11Enable) {
        m_ssgSwEnv11.noteOff();
    }

    if (m_params.ssgPEnv11Enable) {
        m_ssgSwPenv11.noteOff();
    }
}

void OplOperator::processLfo()
{
    m_lfo.getSample();
}

void OplOperator::getSample(float& output, float modulator, float feedbackModulator)
{
    bool allAmpBypassed = isAllAmpBypassed();

    if (allAmpBypassed) {
        // すべてのアンプエンベロープがバイパスの時は、完全な矩形波（Gate）動作。
        // リリース状態になったら即座にミュートして終了する。
        if (m_ampAdsr.isRelease() || (m_params.ssgEnvEnable && m_ssgSwEnv.isRelease()) || (m_params.ssgEnv11Enable && m_ssgSwEnv11.isRelease())) {
            m_ampAdsr.bypassedReleasedProcess();
            if (m_params.ssgEnvEnable) m_ssgSwEnv.bypassedReleasedProcess();
            if (m_params.ssgEnv11Enable) m_ssgSwEnv11.bypassedReleasedProcess();

            if (m_params.pitchEnvEnable) m_pitchAdsr.bypassedReleasedProcess();
            if (m_params.ssgPEnv11Enable) m_ssgSwPenv11.bypassedReleasedProcess();

            output = 0.0f;
            m_fb1 = 0.0f;
            m_fb2 = 0.0f;
            return;
        }
    }
    else if (!isPlaying()) {
        // 有効なエンベロープがあり、かつ全て再生終了（音が減衰しきった）時は、
        // ピッチエンベロープなども状態をクリアして安全に停止させる
        if (m_params.pitchEnvEnable) m_pitchAdsr.bypassedReleasedProcess();
        if (m_params.ssgPEnv11Enable) m_ssgSwPenv11.bypassedReleasedProcess();

        output = 0.0f;
        m_fb1 = 0.0f;
        m_fb2 = 0.0f;
        return;
    }

    float envVal = 1.0f;

    if (!allAmpBypassed) {
        // 1. ADSR処理
        if (!m_ampAdsr.isBypass()) {
            m_currentLevel = m_ampAdsr.updateEnvelopeState(m_currentLevel);
            envVal *= m_currentLevel;
        }
        else {
            if (m_ampAdsr.isRelease()) {
                m_ampAdsr.bypassedReleasedProcess();
            }
            else {
                m_currentLevel = m_ampAdsr.updateEnvelopeState(m_currentLevel);
                envVal *= m_currentLevel;
            }
        }

        // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
        if (m_params.ssgEnvEnable) {
            if (!m_ssgSwEnv.isBypass()) {
                envVal *= m_ssgSwEnv.process();
            }
            else if (m_ssgSwEnv.isRelease()) {
                m_ssgSwEnv.bypassedReleasedProcess();
            }
        }

        // 3. SSG Sw Env 11 処理
        if (m_params.ssgEnv11Enable) {
            if (!m_ssgSwEnv11.isBypass()) {
                envVal *= m_ssgSwEnv11.process();
            }
            else if (m_ssgSwEnv11.isRelease()) {
                m_ssgSwEnv11.bypassedReleasedProcess();
            }
        }
    }

    // AM適用 (無条件。変調がない場合はコア側から 1.0 が渡ってくる)
    envVal *= m_lfo.value.am;

    // ========================================================
    // 3. 位相と波形の生成
    // ========================================================
    float basePhaseDelta = m_phaseDelta * m_pitchBendRatio * m_lfo.value.pm;
    float currentPhaseDelta = m_params.pitchEnvEnable ? m_pitchAdsr.process(basePhaseDelta) : basePhaseDelta;
    currentPhaseDelta = m_params.ssgPEnv11Enable ? m_ssgSwPenv11.process(currentPhaseDelta) : currentPhaseDelta;

    // 位相の変調
    float feedbackPhaseOffset = 0.0f;
    if (m_feedback > 0 && feedbackModulator != 0.0f) {
        feedbackPhaseOffset = feedbackModulator * fVector[m_feedback];
    }

    // --------------------------------------------------------
    // PCM波形への過剰な位相変調を抑え、音量低下を防ぐスケーリング
    // --------------------------------------------------------
    // Modulator からの入力 (変調インデックスは実機では通常 4π ではなく 2π〜8πの範囲ですが、
    // ここは既存のシステムに合わせておきます)
    // 変調指数。従来の 4π ラジアンは 2 サイクルに相当する
    float fmModIndex = 2.0f;

    double modulatedPhase = m_phase + (modulator * fmModIndex) + feedbackPhaseOffset;

    // エンベロープが「掛かる前」の生の波形を取得
    float rawWave = calcWaveform(modulatedPhase, m_params.waveSelect);

    // 最後にエンベロープを掛けて出力とする
    output = rawWave * envVal * m_targetLevel;

    // 実機はエンベロープ適用後の出力をフィードバックに戻すため、
    // 音が減衰するとフィードバックも弱まり、倍音が落ち着いていく
    m_fb2 = m_fb1;
    m_fb1 = output;

    // m_phase の更新とラップアラウンドもラジアンで行う
    m_phase += currentPhaseDelta;

    while (m_phase >= 1.0) m_phase -= 1.0;
    while (m_phase < 0.0) m_phase += 1.0;
}

float OplOperator::calcWaveform(double phase, int wave)
{
    // phase はサイクル単位 (1.0 で 1 周)。0.0 ～ 1.0 に丸めてからラジアンへ直す
    float p = (float)((phase - std::floor(phase)) * juce::MathConstants<double>::twoPi);

    // サイン波はラジアンで計算
    float s = std::sin(p);

    // 波形生成ロジック用に、0.0 ～ 1.0 に正規化された位相を作る！
    float normPhase = p / (2.0f * juce::MathConstants<float>::pi);

    int safeWave = std::clamp(wave, 0, 3);

    switch (safeWave) {
    case 0:
        // Sine
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
    }

    return s;
}
