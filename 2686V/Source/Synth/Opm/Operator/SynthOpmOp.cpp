#include "./SynthOpmOp.h"
#include "../../../Core/Processor/ProcessorValues.h"

void OpmOperator::prepare(int opIndex, double sampleRate) {
    m_ampAdsr.prepare(opIndex, sampleRate);
    m_pitchAdsr.prepare(opIndex, sampleRate);
    m_ssgSwEnv.prepare(opIndex, sampleRate);
    m_ssgSwEnv11.prepare(opIndex, sampleRate);
    m_ssgSwPenv11.prepare(opIndex, sampleRate);

    m_ampAdsr.setParamMax(
        CPV::OpmRg::Ar::max,
        CPV::OpmRg::D1r::max,
        CPV::OpmRg::D2r::max,
        CPV::OpmRg::D1l::max,
        CPV::OpmRg::Rr::max,
        CPV::OpmRg::Tl::max
    );
}

void OpmOperator::setCurveCore(CurveCore* p_curveCore)
{
    m_ampAdsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv.setCurveCore(p_curveCore);
    m_ssgSwEnv11.setCurveCore(p_curveCore);
    m_ssgSwPenv11.setCurveCore(p_curveCore);
}

void OpmOperator::setParameters(const OpmOpParams& params, int feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_params.se = params.se;
    m_params.se.freq = 1.0f;
    m_params.waveSelect = 0;
    m_ampAdsr.setParameters(params.m_adsrParams);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_ssgSwEnv.setParameters(params.ssgSwEnv);
    m_ssgSwEnv11.setParameters(params.ssgSwEnv11);
    m_ssgSwPenv11.setParameters(params.ssgSwPEnv11);
    m_detune.setParameters(params.detune);
	m_fixMode.setParameters(params.fix);
}

void OpmOperator::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;

    m_ampAdsr.updateTargetSampleRate(sampleRate);
    m_pitchAdsr.updateSampleRate(sampleRate);
    m_ssgSwEnv.updateTargetSampleRate(sampleRate);
    m_ssgSwEnv11.updateSampleRate(sampleRate);
    m_ssgSwPenv11.updateSampleRate(sampleRate);
}

void OpmOperator::noteOn(float frequency, float velocity, int noteNumber, bool isLegato)
{
    m_noteNumber = noteNumber;

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
    float baseFreq = m_fixMode.noteOn(frequency);

    // 基本周波数にデチューン成分を加算
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

void OpmOperator::noteOff()
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

void OpmOperator::getSample(float& output, float modulator, float feedbackModulator, const OpmLfoCore& hwLfo, float modWheel)
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

    // ========================================================
    // 1. Amplitude Modulation (Tremolo / Wah) の計算
    // ========================================================
    float totalAmDepth = 0.0f;

    // ① グローバルAM (G-AMスイッチがONの時のみ受け取る)
    //    実機 YM2151 の AMS-EN は「そのオペレータに AM を掛けるか」のゲートなので、
    //    OFF のオペレータには AM が一切掛からない。
    if (hwLfo.amEnable && m_params.lfo.amsEnable)
    {
        totalAmDepth += hwLfo.ams * hwLfo.depthDb;
    }

    // 上限を1.0(100%)でクリップ
    totalAmDepth = std::min(totalAmDepth, 1.0f);

    if (totalAmDepth > 0.0f) {
        // 他音源と揃えて dB で減衰させる。リニアで 0 まで落とすと LFO の谷で
        // 完全に無音になり、音が途切れてプチノイズの原因になる。
        float attenuationDb = hwLfo.value.am * totalAmDepth * Global::Lfo::maxAmDepthDb;

        envVal *= std::pow(10.0f, -attenuationDb / 20.0f); // 音量に直接適用
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
    float basePhaseDelta = m_phaseDelta * m_pitchBendRatio * (*m_p_globalPitchRatio) * lfoPitchMod;
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

    // 位相の変調
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
