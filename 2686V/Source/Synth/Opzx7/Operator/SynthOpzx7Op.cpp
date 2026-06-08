#include "./SynthOpzx7Op.h"
#include "../../../Processor/Opzx7/ProcessorOpzx7Values.h"

namespace {
    // =================================================================
    // 波形生成用のユーティリティ関数群
    // =================================================================
    inline bool isOddQuad(float phase) {
        return phase < 0.25f || (phase >= 0.5f && phase < 0.75f);
    }

    inline float doubleSine(float p) {
        return std::sin(p * 2.0f);
    }

    inline float halfLevelSign(float sine) {
        if (sine > 0.5f) return 0.5f;
        if (sine < -0.5f) return -0.5f;
        return sine;
    }
    
    inline float triangle(float phase) {
        float value = phase * 4.0f;
        if (phase < 0.25f) return value;
        if (phase < 0.75f) return 2.0f - value;
        return value - 4.0f;
    }

    inline float dsTriangle(float phase) {
        float position = phase >= 0.5f ? phase - 0.5f : phase;
        float value = position * 8.0f;
        if (position < 0.125f) return value;
        if (position < 0.375f) return 2.0f - value;
        return value - 4.0f;
    }

    inline float diagram(float phase) {
        float value = phase * 2.0f;
        if (phase < 0.5f) return value;
        return value - 2.0f;
    }

    inline float dsDiagram(float phase) {
        float position = phase >= 0.5f ? phase - 0.5f : phase;
        float value = position * 4.0f;
        if (position < 0.25f) return value;
        return value - 2.0f;
    }

    inline float absHalfSawUp(float phase) {
        float value = phase * 2.0f;
        if (phase < 0.5f) return value;
        return value - 1.0f;
    }

    inline float dsAbsHalfSawUp(float phase) {
        float position = phase >= 0.5f ? phase - 0.5f : phase;
        float value = position * 4.0f;
        if (position < 0.25f) return value;
        return value - 1.0f;
    }

    inline float reverseSign(float phase, float nP) {
        float s2 = std::sin(phase + 0.5f * juce::MathConstants<float>::pi);
        if (nP < 0.25f) return 1.0f - s2;
        if (nP < 0.5f) return 1.0f + s2;
        if (nP < 0.75f) return -1.0f - s2;
        return s2 - 1.0f;
    }

    inline float doubleReverseSine(float phase, float nP) {
        float s2 = std::sin((phase + 0.75f * juce::MathConstants<float>::pi) * 2.0f);
        if (nP < 0.125f) return 1.0f + s2;
        if (nP < 0.25f) return 1.0f - s2;
        if (nP < 0.375f) return s2 - 1.0f;
        return -1.0f - s2;
    }

    // =================================================================
    // 波形ストラテジー配列の定義
    // (引数: ラジアン位相 p, 正規化位相 n, サイン波 s)
    // =================================================================
    using WaveCalculator = float(*)(float p, float n, float s);

    const std::array<WaveCalculator, 73> waveStrategies = { {
        // 0: Sine
        [](float p, float n, float s) { return s; },
        // 1: Half Sine
        [](float p, float n, float s) { return n < 0.5f ? s : 0.0f; },
        // 2: Abs Sine
        [](float p, float n, float s) { return std::abs(s); },
        // 3: Quadra Abs Half Sine
        [](float p, float n, float s) { return isOddQuad(n) ? std::abs(s) : 0.0f; },
        // 4: Alt Sine
        [](float p, float n, float s) { return n < 0.5f ? doubleSine(p) : 0.0f; },
        // 5: Alt Abs Sine
        [](float p, float n, float s) { return n < 0.5f ? std::abs(doubleSine(p)) : 0.0f; },
        // 6: Square
        [](float p, float n, float s) { return n < 0.5f ? 1.0f : -1.0f; },
        // 7: Log Saw
        [](float p, float n, float s) { float saw = 1.0f - n * 2.0f; return saw * saw * saw; },
        // 8: Pudding Sine
        [](float p, float n, float s) { return halfLevelSign(s); },
        // 9: Half Pudding Sine
        [](float p, float n, float s) { return n < 0.5f ? halfLevelSign(s) : 0.0f; },
        // 10: Abs Pudding Sine
        [](float p, float n, float s) { return std::abs(halfLevelSign(s)); },
        // 11: Quad Abs Pudding Sine
        [](float p, float n, float s) { return isOddQuad(n) ? std::abs(halfLevelSign(s)) : 0.0f; },
        // 12: Mini Alt Sine
        [](float p, float n, float s) { return n < 0.5f ? std::sin(p * 2.0f) * 0.5f : 0.0f; },
        // 13: Mini Alt Abs Sine
        [](float p, float n, float s) { return n < 0.5f ? std::abs(std::sin(p * 2.0f)) * 0.5f : 0.0f; },
        // 14: Half Square
        [](float p, float n, float s) { return n < 0.5f ? 1.0f : 0.0f; },
        // 15: Wavetable (Dummy: 実際の処理はメソッド内で分岐)
        [](float p, float n, float s) { return 0.0f; },
        // 16: Triangle
        [](float p, float n, float s) { return triangle(n); },
        // 17: Half Triangle
        [](float p, float n, float s) { return n < 0.5f ? triangle(n) : 0.0f; },
        // 18: Abs Triangle
        [](float p, float n, float s) { return std::abs(triangle(n)); },
        // 19: Quad Abs Triangle
        [](float p, float n, float s) { return isOddQuad(n) ? std::abs(triangle(n)) : 0.0f; },
        // 20: Alt Triangle
        [](float p, float n, float s) { return n < 0.5f ? dsTriangle(n) : 0.0f; },
        // 21: Alt Abs Triangle
        [](float p, float n, float s) { return n < 0.5f ? std::abs(dsTriangle(n)) : 0.0f; },
        // 22: Quad Half Square
        [](float p, float n, float s) { return isOddQuad(n) ? 1.0f : 0.0f; },
        // 23: None
        [](float p, float n, float s) { return 0.0f; },
        // 24: Diagram
        [](float p, float n, float s) { return diagram(n); },
        // 25: Half Diagram
        [](float p, float n, float s) { return n < 0.5f ? diagram(n) : 0.0f; },
        // 26: Abs Half Saw Up
        [](float p, float n, float s) { return absHalfSawUp(n); },
        // 27: Quad Abs Half Saw Up
        [](float p, float n, float s) { return isOddQuad(n) ? absHalfSawUp(n) : 0.0f; },
        // 28: Alt Diagram
        [](float p, float n, float s) { return n < 0.5f ? dsDiagram(n) : 0.0f; },
        // 29: Alt Quad Abs Half Saw Up
        [](float p, float n, float s) { return n < 0.5f ? dsAbsHalfSawUp(n) : 0.0f; },
        // 30: Quad Square
        [](float p, float n, float s) { return n < 0.25f ? 1.0f : 0.0f; },
        // 31: PCM (Dummy: 実際の処理はメソッド内で分岐)
        [](float p, float n, float s) { return 0.0f; },
        // 32: 
        [](float p, float n, float s) { return n < 0.5f ? std::abs(std::sin(p * 2.0f)) : 0.0f; },
        // 33:
        [](float p, float n, float s) { float sign = (n < 0.5f) ? 1.0f : -1.0f; return sign * (1.0f - std::pow(1.0f - std::abs(s), 4.0f)); },
        // 34:
        [](float p, float n, float s) { return 1.0f - n * 2.0f; },
        // 35:
        [](float p, float n, float s) { return n * 2.0f - 1.0f; },
        // 36:
        [](float p, float n, float s) { return (1.0f - n * 2.0f) * 0.5f + s * 0.5f; },
        // 37:
        [](float p, float n, float s) { return n < 0.25f ? 1.0f : -1.0f; },
        // 38:
        [](float p, float n, float s) { return n < 0.125f ? 1.0f : -1.0f; },
        // 39:
        [](float p, float n, float s) { return n < 0.0625f ? 1.0f : -1.0f; },
        // 40:
        [](float p, float n, float s) { return std::tanh(s * 5.0f); },
        // 41:
        [](float p, float n, float s) { return std::exp(-100.0f * std::pow(n - 0.5f, 2.0f)) * 2.0f - 1.0f; },
        // 42:
        [](float p, float n, float s) { return std::sin(p) + std::sin(p * 3.0f) * 0.5f + std::sin(p * 5.0f) * 0.25f; },
        // 43:
        [](float p, float n, float s) { return (1.0f - n * 2.0f) * std::sin(p * 4.0f); },
        // 44:
        [](float p, float n, float s) { return (1.0f - n * 2.0f) * std::sin(p * 8.0f); },
        // 45:
        [](float p, float n, float s) { float tri = (n < 0.5f ? (4.0f * n - 1.0f) : (3.0f - 4.0f * n)); return tri * std::sin(p * 3.0f); },
        // 46:
        [](float p, float n, float s) { return s * s * s; },
        // 47:
        [](float p, float n, float s) { return std::sin(p) * std::sin(p * 2.0f); },
        // 48:
        [](float p, float n, float s) { return s + 0.5f * std::sin(p * 2.0f) + 0.25f * std::sin(p * 4.0f); },
        // 49:
        [](float p, float n, float s) { return s * std::cos(p * 2.5f); },
        // 50:
        [](float p, float n, float s) { return std::sin(p) * std::sin(p * 1.414f); },
        // 51:
        [](float p, float n, float s) { return std::sin(p) * std::cos(p * 0.5f); },
        // 52:
        [](float p, float n, float s) { return std::sin(p * 13.0f) * std::cos(p * 7.0f) * std::sin(p * 2.0f); },
        // 53: PD Resonance (Casio CZ Style)
        [](float p, float n, float s) { return (1.0f - std::cos(p)) * std::sin(p * 5.0f) * 0.5f; },
        // 54: PD Resonance High
        [](float p, float n, float s) { return (1.0f - std::cos(p)) * std::sin(p * 9.0f) * 0.5f; },
        // 55: 4-Step Sine
        [](float p, float n, float s) { return std::round(s * 2.0f) / 2.0f; },
        // 56: 8-Step Sine
        [](float p, float n, float s) { return std::round(s * 4.0f) / 4.0f; },
        // 57: Wavefolded Sine (Soft)
        [](float p, float n, float s) { float fs = s * 1.5f; if (fs > 1.0f) return 2.0f - fs; if (fs < -1.0f) return -2.0f - fs; return fs; },
        // 58: Wavefolded Sine (Hard)
        [](float p, float n, float s) { float fs = s * 2.5f; return std::sin(fs * juce::MathConstants<float>::halfPi); },
        // 59: Bitwise XOR Fractal
        [](float p, float n, float s) { uint8_t pi = (uint8_t)(n * 255.0f); uint8_t xv = pi ^ (pi >> 1); return ((float)xv / 255.0f) * 2.0f - 1.0f; },
        // 60: Bitwise AND Texture
        [](float p, float n, float s) { uint8_t pi = (uint8_t)(n * 255.0f); uint8_t av = pi & (pi << 1); return ((float)av / 255.0f) * 2.0f - 1.0f; },
        // 61: Self-Modulated Sine (FB=1)
        [](float p, float n, float s) { return std::sin(p + 1.0f * std::sin(p)); },
        // 62: Self-Modulated Sine (FB=2)
        [](float p, float n, float s) { return std::sin(p + 2.0f * std::sin(p)); },
        // 63: OPZ(TX81Z) W2
        [](float p, float n, float s) { return reverseSign(p, n); },
        // 64: OPZ(TX81Z) W3
        [](float p, float n, float s) { return n < 0.5f ? s : 0.0f; },
        // 65: OPZ(TX81Z) W4
        [](float p, float n, float s) { return n < 0.5f ? reverseSign(p, n) : 0.0f; },
        // 66: OPZ(TX81Z) W5
        [](float p, float n, float s) { return n < 0.5f ? doubleSine(p) : 0.0f; },
        // 67: OPZ(TX81Z) W6
        [](float p, float n, float s) { return n < 0.5f ? doubleReverseSine(p, n) : 0.0f; },
        // 68: OPZ(TX81Z) W7
        [](float p, float n, float s) { return n < 0.5f ? std::abs(doubleSine(p)) : 0.0f; },
        // 69: OPZ(TX81Z) W8
        [](float p, float n, float s) { return n < 0.5f ? std::abs(doubleReverseSine(p, n)) : 0.0f; },
        // 70: Cubic Triangle
        [](float p, float n, float s) { float tri = triangle(n); return tri * tri * tri; },
        // 71: Inverse Circle
        [](float p, float n, float s) { float tri = triangle(n); float absTri = std::abs(tri); float sign = (tri >= 0.0f) ? 1.0f : -1.0f; return sign * (1.0f - std::sqrt(std::max(0.0f, 1.0f - absTri * absTri))); },
        // 72: Exponential Spike
        [](float p, float n, float s) { float lp = std::fmod(n * 2.0f, 1.0f); float sign = (n < 0.5f) ? 1.0f : -1.0f; return sign * std::exp(-lp * 8.0f); }
    } };
}

const std::array<Opzx7Operator::SsgWaveCalculator, 16> Opzx7Operator::ssgWaveStrategies = { {
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

void Opzx7Operator::prepare(int opIndex, double sampleRate) {
    m_ampAdsr.prepare(opIndex, sampleRate);
    m_pitchAdsr.prepare(opIndex, sampleRate);
	m_ssgSwEnv.prepare(opIndex, sampleRate);
    m_lfo.prepare(sampleRate);

    m_ampAdsr.setParamMax(
        Opzx7PrValue::Op::RgAdsr::Ar::max,
        Opzx7PrValue::Op::RgAdsr::D1r::max,
        Opzx7PrValue::Op::RgAdsr::D2r::max,
        Opzx7PrValue::Op::RgAdsr::D1l::max,
        Opzx7PrValue::Op::RgAdsr::Rr::max,
        Opzx7PrValue::Op::RgAdsr::Tl::max
    );
}

void Opzx7Operator::setCurveCore(CurveCore* p_curveCore)
{
    m_ampAdsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv.setCurveCore(p_curveCore);
}

void Opzx7Operator::setSampleRate(double sampleRate) {
	m_sampleRate = sampleRate;
	m_ampAdsr.updateSampleRate(sampleRate);
	m_pitchAdsr.updateSampleRate(sampleRate);
	m_ssgSwEnv.updateSampleRate(sampleRate);
}

void Opzx7Operator::setParameters(const Opzx7OpParams& params, float feedback)
{
    m_params = params;
    m_feedback = feedback;
    m_ssgEgFreq = params.fmSsgEgFreq;
    m_params.waveSelect = params.waveSelect;

    m_ampAdsr.setParameters(params.m_adsrParams);
    m_detune.setParameters(params.detune, params.detune2, params.detune3, params.multiple, params.multipleRatio);
    m_fixMode.setParameters(params.fixedMode, params.fixedFreq);
    m_pitchAdsr.setParameters(params.pitchAdsr);
    m_ssgSwEnv.setParameters(params.ssgSwEnv);
    m_lfo.setParameters(
        params.lfoPmSyncDelay,
        params.lfoAmSyncDelay,
        params.vibEnable,
        params.amEnable,
        params.lfoPmFreq,
        params.lfoAmFreq,
        params.pgLfoWave,
        params.egLfoWave,
        params.pms,
        params.pmd,
        params.ams,
        params.amd,
        params.lfoAmSmRt
    );
}

void Opzx7Operator::noteOn(float frequency, float velocity, int noteNumber, bool isLegato)
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

        m_lfo.noteOn();
    }

    // ========================================================
    // Base Frequency Calculation (PCMのサンプラー挙動対応)
    // ========================================================
    float baseFreq = frequency;

    if (m_params.waveSelect == Opzx7PrValue::pcmIndex && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
    {
        // オフセットとレシオを考慮した再生区間の計算
        size_t totalSize = m_pcmBuffer->size();
        size_t offsetSamples = (size_t)((m_params.pcmOffset / 1000.0f) * m_hostSampleRate);
        if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;

        size_t remainingSize = totalSize - offsetSamples;
        size_t playSize = (size_t)(remainingSize * m_params.pcmRatio);
        if (playSize < 1) playSize = 1;

        float originalHz = (float)m_hostSampleRate / (float)playSize;

        if (m_params.fixedMode) {
            baseFreq = originalHz * m_params.fixedFreq;
        }
        else {
            float rootFreq = 261.625565f;
            float pitchRatio = frequency / rootFreq;
            baseFreq = originalHz * pitchRatio;
        }
    }
    else
    {
        baseFreq = m_fixMode.noteOn(frequency);
    }

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

void Opzx7Operator::noteOff()
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

//void Opzx7Operator::getSample(float& output, float modulator,
//    float amLfoVal, float pmLfoVal, bool globalPm, bool globalAm, float globalPms, float globalAms, float globalPmd, float globalAmd, float modWheel)
void Opzx7Operator::getSample(float& output, float modulator, Opzx7LfoCore& glLfo, float modWheel)
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
    // 0. ローカル LFO の計算 (波形独立)
    // ========================================================
    m_lfo.getSample();

    // ========================================================
    // 1. Amplitude Modulation (Tremolo) の連続計算
    // ========================================================
    float totalAmpMod = 1.0f;

    // ① グローバルAM (最大 96dB の減衰)
    if (glLfo.am.enable) {
        totalAmpMod *= std::pow(10.0f, -(glLfo.value.am * glLfo.am.depthDb) / 20.0f);
    }

    if (m_lfo.am.enable) {
        totalAmpMod *= std::pow(10.0f, -(m_lfo.value.am * m_lfo.am.depthDb) / 20.0f);
    }

    envVal *= totalAmpMod;

    // ========================================================
    // 2. Pitch Modulation (Vibrato) の連続計算
    // ========================================================
    float currentPitchCent = 1.0f;

    // ① グローバルPM (最大 ±1200 Cent の揺れ幅 = ±1オクターブ)
    if (glLfo.pm.enable) {
        currentPitchCent += glLfo.value.pm * glLfo.pm.depthCent;
    }

    if (m_lfo.pm.enable) {
        currentPitchCent += m_lfo.value.pm * m_lfo.pm.depthCent;
    }

    // ③ モジュレーションホイール (Global LFO を使用、最大200セント)
    currentPitchCent += glLfo.value.pm * (modWheel * 200.0f);

    // 蓄積した Cent を周波数倍率に変換
    float lfoPitchMod = std::pow(2.0f, currentPitchCent / 1200.0f);

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

    if (m_params.waveSelect == Opzx7PrValue::pcmIndex && m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
    {
        size_t totalSize = m_pcmBuffer->size();
        size_t offsetSamples = (size_t)((m_params.pcmOffset / 1000.0f) * m_hostSampleRate);
        if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;
        size_t remainingSize = totalSize - offsetSamples;
        size_t playSize = (size_t)(remainingSize * m_params.pcmRatio);
        if (playSize < 1) playSize = 1;

        float originalHz = (float)m_hostSampleRate / (float)playSize;

        float scale = originalHz / 261.625565f;
        scale = std::max(scale, 0.005f);
        fmModIndex *= scale;
    }

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

float Opzx7Operator::calcWaveform(double phase, int wave)
{
    // 1. まず phase を 0.0 ～ 2π の範囲に丸め込む (ラジアン)
    float p = std::fmod((float)phase, 2.0f * juce::MathConstants<float>::pi);

    if (p < 0.0f) p += 2.0f * juce::MathConstants<float>::pi;

    // サイン波と正規化位相を計算
    float s = std::sin(p);
    float normPhase = p / (2.0f * juce::MathConstants<float>::pi);

    // =================================================================
    // PCM波形の特別処理 (メンバ変数へのアクセスが必要なため分離)
    // =================================================================
    if (wave == 31)
    {
        if (m_pcmBuffer != nullptr && !m_pcmBuffer->empty())
        {
            // OffsetとRatioを適用して切り出す
            size_t totalSize = m_pcmBuffer->size();
            size_t offsetSamples = (size_t)((m_params.pcmOffset / 1000.0f) * m_hostSampleRate);

            if (offsetSamples >= totalSize) offsetSamples = totalSize - 1;

            size_t remainingSize = totalSize - offsetSamples;
            size_t playSize = (size_t)(remainingSize * m_params.pcmRatio);

            if (playSize < 1) playSize = 1;

            // normPhase (0.0 ~ 1.0) を playSize にマッピングし、offsetを足す
            float floatIndex = (float)offsetSamples + normPhase * (float)playSize;
            int index1 = (int)floatIndex;

            if (index1 >= totalSize) index1 = totalSize - 1;

            int index2 = index1 + 1;

            // ループの終端に達したら、offset位置（先頭）に戻す
            if (index2 >= offsetSamples + playSize || index2 >= totalSize) {
                index2 = offsetSamples;
            }

            float frac = floatIndex - (float)index1;

            return (*m_pcmBuffer)[index1] * (1.0f - frac) + (*m_pcmBuffer)[index2] * frac;
        }

        return s; // PCMバッファが無い場合はサイン波を返す
    }

    // =================================================================
    // 波形メモリの特別処理 (メンバ変数へのアクセスが必要なため分離)
    // =================================================================
    if (m_params.waveSelect == Opzx7PrValue::wtIndex)
    {
        if (m_wtBuffer != nullptr && !m_wtBuffer->empty())
        {
            size_t totalSize = m_wtBuffer->size();

            // normPhase (0.0 ~ 1.0) を totalSize にマッピング
            float floatIndex = normPhase * (float)totalSize;
            int index1 = (int)floatIndex;

            if (index1 >= totalSize) index1 = totalSize - 1;

            int index2 = index1 + 1;
            if (index2 >= totalSize) index2 = 0; // ループ

            float frac = floatIndex - (float)index1;

            // 線形補間で滑らかに読み取る
            return (*m_wtBuffer)[index1] * (1.0f - frac) + (*m_wtBuffer)[index2] * frac;
        }

        return s; // データが無い場合はサイン波を返す
    }

    // =================================================================
    // ストラテジー配列による高速な波形計算
    // =================================================================
    int safeWave = std::clamp(wave, 0, 72);

    return waveStrategies[safeWave](p, normPhase, s);
}
