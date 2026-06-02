#include "./Core.h"
#include "../../../Processor/Opl3/Values.h"

namespace {
    inline float doubleSine(float p) {
        return std::sin(p * 2.0f);
    }

    // =================================================================
    // 波形ストラテジー配列の定義
    // (引数: ラジアン位相 p, 正規化位相 n, サイン波 s)
    // =================================================================
    using WaveCalculator = float(*)(float p, float n, float s);

    const std::array<WaveCalculator, 10> waveStrategies = { {
        // 0: Sine
        [](float p, float n, float s) { return s; },
        // 1: Half Sine
        [](float p, float n, float s) { return n < 0.5f ? s : 0.0f; },
        // 2: Abs Sine
        [](float p, float n, float s) { return std::abs(s); },
        // 3: Pulse Sine
        [](float p, float n, float s) { return n < 0.25f ? s : 0.0f; },
        // 4: Alternative Sine
        [](float p, float n, float s) { return n < 0.5f ? doubleSine(p) : 0.0f; },
        // 5: Alternative Abs Sine
        [](float p, float n, float s) { return n < 0.5f ? std::abs(doubleSine(p)) : 0.0f; },
        // 6: Square
        [](float p, float n, float s) { return n < 0.5f ? 1.0f : -1.0f; },
        // 7: Derived Square
        [](float p, float n, float s) {
            float newP = p / 2.0f + 0.5f * juce::MathConstants<float>::pi;

            return std::sin(newP);
        },
        // 8: [EX01]Round Square
        [](float p, float n, float s) {
            float sign = (n < 0.5f) ? 1.0f : -1.0f;
            return sign * (1.0f - std::pow(1.0f - std::abs(s), 4.0f));
        },
        // 9: [EX02]Log Saw
        [](float p, float n, float s) {
            float saw = 1.0f - n * 2.0f;
            return saw * saw * saw;
        },
    } };
}

const std::array<float, 4> Opl3Operator::dbPerOcts = { 0.0f, 1.5f, 3.0f, 6.0f };

void Opl3Operator::prepare(int opIndex, double sampleRate) {
    m_ampAdsr.prepare(opIndex, sampleRate);
    m_pitchAdsr.prepare(opIndex, sampleRate);
    m_ssgSwEnv.prepare(opIndex, sampleRate);

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

void Opl3Operator::noteOn(float frequency, float velocity, int noteNumber)
{
    m_ssgPhase = 0.0;
    m_noteNumber = noteNumber;

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

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    float finalFreq = m_detune.noteOn(baseFreq);

    m_phaseDelta = (finalFreq * 2.0 * juce::MathConstants<float>::pi) / m_sampleRate;

    if (!m_ampAdsr.isBypass()) {
        m_targetLevel = m_ampAdsr.noteOn(velocity, noteNumber);

        m_ampAdsr.updateIncrementsWithKeyScale(m_noteNumber);
    }
    else {
        m_targetLevel = velocity;
    }

    if (m_params.pitchEnvEnable) {
        m_pitchAdsr.noteOn();
    }

    if (m_params.ssgEnvEnable) {
        m_ssgSwEnv.noteOn();
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

void Opl3Operator::getSample(float& output, float modulator)
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

    // AM適用 (無条件。変調がない場合はコア側から 1.0 が渡ってくる)
    envVal *= m_lfo.value.am;

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

    // PM適用 (無条件。変調がない場合は 1.0 が渡ってくる)
    float basePhaseDelta = m_phaseDelta * m_pitchBendRatio * m_lfo.value.pm;
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

    return waveStrategies[safeWave](p, normPhase, s);
}
