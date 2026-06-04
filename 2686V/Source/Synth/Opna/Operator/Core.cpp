#include "./Core.h"

#include "../../../Core/Fm/FmCore.h"
#include "../../../Processor/Opna/Values.h"

const std::array<OpnaOperator::SsgWaveCalculator, 16> OpnaOperator::ssgWaveStrategies = { {
    [](double p) { // 00: normal
        return 1.0f;
    },
    [](double p) { // 01
        return 1.0f;
    },
    [](double p) { // 02: Saw Down
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return 1.0f - (float)subPos;
    },
    [](double p) { // 03
        return 1.0f;
    },
    [](double p) { // 04: Saw Down & Hold
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return (cycle == 0) ? (1.0f - (float)subPos) : 0.0f;
    },
    [](double p) { // 05
        return 1.0f;
    },
    [](double p) { // 06: Triangle
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return isEven ? (1.0f - (float)subPos) : (float)subPos;
    },
    [](double p) { // 07
        return 1.0f;
    },
    [](double p) { // 08: Alt Saw Down & Hold
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return (cycle == 0) ? (1.0f - (float)subPos) : 0.0f;
    },
    [](double p) { // 09: Saw Up
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return (float)subPos;
    },
    [](double p) { // 10
        return 1.0f;
    },
    [](double p) { // 11: Saw Up & Hopd
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return (cycle == 0) ? (float)subPos : 1.0f;
    },
    [](double p) { // 12
        return 1.0f;
    },
    [](double p) { // 13: Triangle & Invert
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return isEven ? (float)subPos : (1.0f - (float)subPos);
    },
    [](double p) { // 14
        return 1.0f;
    },
    [](double p) { // 15: Alt Saw Up & Hold
        int cycle = (int)p;
        double subPos = p - cycle;
        bool isEven = (cycle % 2 == 0);

        return (cycle == 0) ? (float)subPos : 1.0f;
    },
} };

void OpnaOperator::prepare(int opIndex, double sampleRate) {
    m_ampAdsr.prepare(opIndex, sampleRate);
    m_pitchAdsr.prepare(opIndex, sampleRate);
    m_ssgSwEnv.prepare(opIndex, sampleRate);
	m_hwLfo.prepare(sampleRate);

    m_ampAdsr.setParamMax(
        OpnaPrValue::Op::RgAdsr::Ar::max,
        OpnaPrValue::Op::RgAdsr::Dr::max,
        OpnaPrValue::Op::RgAdsr::Sr::max,
        OpnaPrValue::Op::RgAdsr::Sl::max,
        OpnaPrValue::Op::RgAdsr::Rr::max,
        OpnaPrValue::Op::RgAdsr::Tl::max
    );
}

void OpnaOperator::setCurveCore(CurveCore* p_curveCore)
{
    m_ampAdsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv.setCurveCore(p_curveCore);
}

void OpnaOperator::setSampleRate(double sampleRate)
{
	this->m_sampleRate = sampleRate;
	m_hwLfo.updateTargetSampleRate(sampleRate);
	m_ampAdsr.updateTargetSampleRate(sampleRate);
	m_pitchAdsr.updateTargetSampleRate(sampleRate);
	m_ssgSwEnv.updateTargetSampleRate(sampleRate);
}

void OpnaOperator::setParameters(const OpnaOpParams& params, float feedback, float amSmoothRate)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = params.fmSsgEgFreq;
    m_params.waveSelect = 0;
    m_ams = (float)m_params.n88Ams / 15.0f;

    m_ampAdsr.setParameters(params.m_adsrParams);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_ssgSwEnv.setParameters(params.ssgSwEnv);
    m_fixMode.setParameters(params.fixedMode, params.fixedFreq);
    m_detune.setParameters(params.detune, params.multiple);
    m_hwLfo.setParameters(
        params.lfoSyncDelay,
        params.amEnable,
        params.vibEnable,
        params.lfoFreqIndex,
        params.lfoFreqIndex,
        params.pms,
        params.ams,
        amSmoothRate
    );
}

void OpnaOperator::noteOn(float frequency, float velocity, int noteNumber, bool isLegato)
{
    m_noteNumber = noteNumber;

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
            m_fb1 = 0.0f;
            m_fb2 = 0.0f;
        }

        m_hwLfo.noteOn();
    }

    float baseFreq = m_fixMode.noteOn(frequency);

    // 基本周波数にデチューン成分を加算
    float finalFreq = m_detune.noteOn(baseFreq);

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

    if (!isLegato) {
        if (!m_ampAdsr.isBypass()) {
            m_targetLevel = m_ampAdsr.noteOn(velocity);
        }
        else {
            m_targetLevel = velocity;
        }

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
}

void OpnaOperator::processLfo()
{
    m_hwLfo.getSample();
}

void OpnaOperator::getSample(float& output, float modulator, const N88LfoCore& n88Lfo, float modWheel)
{
    if (!isPlaying() && !m_ampAdsr.isBypass()) {
        if (m_params.pitchEnvEnable) {
            m_pitchAdsr.bypassedReleasedProcess();
        }

        if (m_params.ssgEnvEnable) {
            m_ssgSwEnv.bypassedReleasedProcess();
        }

        output = 0.0f;
        // 念のためフィードバックバッファもクリアしておく
        m_fb1 = 0.0f;
        m_fb2 = 0.0f;

        return;
    }

    // 1. 従来のADSR処理 (内部の m_currentLevel はADSR専用として維持する)
    m_currentLevel = m_ampAdsr.updateEnvelopeState(m_currentLevel);
    float envVal = m_currentLevel;

    // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
    if (m_params.ssgEnvEnable) {
        envVal *= m_ssgSwEnv.process(); // 掛け算
    }

    if (m_params.ssgEg > 0) {
        int safeWave = std::clamp(m_params.ssgEg, 0, 15);

        envVal *= ssgWaveStrategies[safeWave](m_ssgPhase);
        m_ssgPhase += (double)m_ssgEgFreq / m_sampleRate;
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
    float feedbackMod = 0.0f;
    if (m_feedback > 0.0f) {
        // YAMAHAの一般的なフィードバックシフト計算 (Feedback値 0〜7 を想定)
        // FB=0 は 0、FB=1〜7 のとき、シフト量は 9 - FB となる仕様が多いです。
        // （FBが上がるほど右シフト量が減り、値が大きくなる）
        // ここでは一般的な 2^((FB - 8) または類似のスケール) を使います。
        // OPL3の仕様に合わせて調整してください。以下は一般的な近似です。
        float fbScale = std::pow(2.0f, m_feedback - 8.0f); // または 9.0f

        // 過去2サンプルの「生の波形値」の平均を使用する
        feedbackMod = (m_fb1 + m_fb2) * 0.5f * fbScale * juce::MathConstants<float>::pi * 2.0f;
    }

    float basePhaseDelta = m_phaseDelta * m_pitchBendRatio * lfoPitchMod;
    float currentPhaseDelta = m_params.pitchEnvEnable ? m_pitchAdsr.process(basePhaseDelta) : basePhaseDelta;

    // --------------------------------------------------------
    // PCM波形への過剰な位相変調を抑え、音量低下を防ぐスケーリング
    // --------------------------------------------------------
    // Modulator からの入力 (変調インデックスは実機では通常 4π ではなく 2π〜8πの範囲ですが、
    // ここは既存のシステムに合わせておきます)
    float fmModIndex = 4.0f * juce::MathConstants<float>::pi;

    // 位相の変調
    float modulatedPhase = m_phase + (modulator * fmModIndex) + feedbackMod;

    // エンベロープが「掛かる前」の生の波形を取得
    float rawWave = calcWaveform(modulatedPhase, m_params.waveSelect);

    // フィードバックバッファには「エンベロープ適用前」の純粋な値を保存する！
    if (!m_isExternalFeedback) {
        m_fb2 = m_fb1;
        m_fb1 = rawWave; // outputではなくrawWaveを保存！
    }

    // 最後にエンベロープを掛けて出力とする
    output = rawWave * envVal * m_targetLevel;

    // m_phase の更新とラップアラウンドもラジアンで行う
    m_phase += currentPhaseDelta;

    while (m_phase >= 2.0f * juce::MathConstants<float>::pi) m_phase -= 2.0f * juce::MathConstants<float>::pi;
    while (m_phase < 0.0f) m_phase += 2.0f * juce::MathConstants<float>::pi;
}
