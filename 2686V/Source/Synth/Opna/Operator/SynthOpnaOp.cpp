#include "./SynthOpnaOp.h"

#include "../../../Core/Fm/FmCore.h"
#include "../../../Core/Processor/ProcessorValues.h"

void OpnaOperator::prepare(int opIndex, double sampleRate) {
    m_ampAdsr.prepare(opIndex, sampleRate);
    m_pitchAdsr.prepare(opIndex, sampleRate);
    m_ssgSwEnv.prepare(opIndex, sampleRate);
    m_ssgSwEnv11.prepare(opIndex, sampleRate);
    m_ssgSwPenv11.prepare(opIndex, sampleRate);
    m_ssgHwPEnv.prepare(sampleRate);
    m_ssgHwEnv.prepare(sampleRate);
    
    m_hwLfo.prepare(sampleRate);

    m_ampAdsr.setParamMax(
        CPV::OpnRg::Ar::max,
        CPV::OpnRg::Dr::max,
        CPV::OpnRg::Sr::max,
        CPV::OpnRg::Sl::max,
        CPV::OpnRg::Rr::max,
        CPV::OpnRg::Tl::max
    );
}

void OpnaOperator::setCurveCore(CurveCore* p_curveCore)
{
    m_ampAdsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv.setCurveCore(p_curveCore);
    m_ssgSwEnv11.setCurveCore(p_curveCore);
    m_ssgSwPenv11.setCurveCore(p_curveCore);
}

void OpnaOperator::setSampleRate(double sampleRate)
{
	this->m_sampleRate = sampleRate;
	m_hwLfo.updateTargetSampleRate(sampleRate);
	m_ampAdsr.updateTargetSampleRate(sampleRate);
	m_pitchAdsr.updateTargetSampleRate(sampleRate);
	m_ssgSwEnv.updateTargetSampleRate(sampleRate);
    m_ssgSwEnv11.updateSampleRate(sampleRate);
    m_ssgSwPenv11.updateSampleRate(sampleRate);
    m_ssgHwPEnv.updateSampleRate(sampleRate);
    m_ssgHwEnv.updateSampleRate(sampleRate);
}

void OpnaOperator::setParameters(const OpnaOpParams& params, int feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_params.waveSelect = 0;
    m_ams = (float)m_params.n88Lfo.ams / 15.0f;
    m_ampAdsr.setParameters(params.m_adsrParams);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_ssgSwEnv.setParameters(params.ssgSwEnv);
    m_ssgSwEnv11.setParameters(params.ssgSwEnv11);
    m_ssgSwPenv11.setParameters(params.ssgSwPEnv11);
    m_ssgHwPEnv.setParameters(params.ssgHwPEnv);
    m_wtAmpMod.setParameters(params.wtAmpMod);
    m_ssgHwEnv.setParameters(params.ssgHwEnv);
    m_wtMod.setParameters(params.wtMod);
    m_fixMode.setParameters(params.fix);
    m_detune.setParameters(params.detune);
    m_hwLfo.setParameters(params.hwLfo);
}

void OpnaOperator::noteOn(float frequency, float velocity, int noteNumber, bool isLegato)
{
    m_noteNumber = noteNumber;

    // ハードウェアエンベロープは位相を持つだけなので、
    // 押し直したときだけ頭から流し直す。
    if (!isLegato) m_ssgHwPEnv.noteOn();
    if (!isLegato) m_wtAmpMod.reset();
    if (!isLegato) m_ssgHwEnv.noteOn();
    if (!isLegato) m_wtMod.reset();

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

        m_hwLfo.noteOn();
    }

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

void OpnaOperator::noteOff()
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

void OpnaOperator::processLfo()
{
    m_hwLfo.getSample();
}

void OpnaOperator::getSample(float& output, float modulator, float feedbackModulator, const N88LfoCore& n88Lfo, float modWheel)
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

    if (m_params.se.eg > 0) {
        int safeWave = std::clamp(m_params.se.eg, 0, 15);

        int cycle = (int)m_ssgPhase;
        double subPos = m_ssgPhase - cycle;
        bool isEven = (cycle % 2 == 0);

        switch (safeWave) {
        case 0:
            // 00: normal
            // 変更無し
            break;
        case 1:
            // 01: 未使用
            // 変更無し
            break;
        case 2:
            // 02: Saw Down
            envVal *= 1.0f - (float)subPos;

            break;
        case 3:
            // 03 未使用
            // 変更無し
            break;
        case 4:
            // 04: Saw Down & Hold
            envVal *= (cycle == 0) ? (1.0f - (float)subPos) : 0.0f;

            break;
        case 5:
            // 05 未使用
            // 変更無し
            break;
        case 6:
            // 06: Triangle
            envVal *= isEven ? (1.0f - (float)subPos) : (float)subPos;

            break;
        case 7:
            // 07 未使用
            // 変更無し
            break;
        case 8:
            // 08: Alt Saw Down & Hold (実機 shape 11: 1回下降して最大値で保持)
            envVal *= (cycle == 0) ? (1.0f - (float)subPos) : 1.0f;

            break;
        case 9:
            // 09: Saw Up
            envVal *= (float)subPos;

            break;
        case 10:
            // 10 未使用
            // 変更無し
            break;
        case 11:
            // 11: Saw Up & Hopd
            envVal *= (cycle == 0) ? (float)subPos : 1.0f;

            break;
        case 12:
            // 12 未使用
            // 変更無し
            break;
        case 13:
            // 13: Triangle & Invert
            envVal *= isEven ? (float)subPos : (1.0f - (float)subPos);

            break;
        case 14:
            // 14 未使用
            // 変更無し
            break;
        case 15:
            // 15: Alt Saw Up & Hold (実機 shape 15: 1回上昇して最小値で保持)
            envVal *= (cycle == 0) ? (float)subPos : 0.0f;

            break;
        }

        m_ssgPhase += (double)m_params.se.freq / m_sampleRate;
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
        // 最大減衰量は全音源共通の Global::Lfo::maxAmDepthDb を使う
        // (95.25dBだと音が完全に途切れてしまい、ブツブツ音の直接の原因になります)
        // ==========================================================
        float attenuationDb = unipolarLfo * (n88Lfo.depthDb * this->m_ams) * maxAmDepthDb;

        // デシベルをリニアな音量倍率に変換
        totalAmpMod = std::pow(10.0f, -attenuationDb / 20.0f);
    }

    // ② ローカルAM (ローカルの m_amSmooth を使う)
    totalAmpMod *= m_hwLfo.value.am;

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

    // ② ローカルPM
    lfoPitchMod += m_hwLfo.value.pm;

    // ③ モジュレーションホイール (Global LFO を使う)
    float wheelCent = n88Lfo.value.pm * (modWheel * 200.0f);
    lfoPitchMod *= std::pow(2.0f, wheelCent / 1200.0f);

    // ========================================================
    // 3. 位相と波形の生成
    // ========================================================
    float basePhaseDelta = m_phaseDelta * m_pitchBendRatio * (*m_p_globalPitchRatio) * lfoPitchMod;
    float currentPhaseDelta = m_params.pitchEnvEnable ? m_pitchAdsr.process(basePhaseDelta) : basePhaseDelta;
    currentPhaseDelta = m_params.ssgPEnv11Enable ? m_ssgSwPenv11.process(currentPhaseDelta) : currentPhaseDelta;

    // SSG HW PITCH ENV。切ってあるときは倍率 1.0 が返るので、
    // 位相を進める意味でも毎サンプル通しておく。
    currentPhaseDelta = m_ssgHwPEnv.process(currentPhaseDelta);

    // WT PITCH MOD。速さは搬送波との比なので、素の位相増分を渡す。
    currentPhaseDelta *= m_wtMod.process(m_phaseDelta);

    // WT AMP MOD。切ってあるときは MAX がそのまま返る。
    envVal *= m_wtAmpMod.process(m_phaseDelta);

    // SSG HW AMP ENV。切ってあるときは MAX がそのまま返る。
    envVal *= m_ssgHwEnv.process();

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
