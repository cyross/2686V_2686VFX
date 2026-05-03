#include "./Core.h"
#include "../../Core/Synth/SynthHelpers.h"

void OpnaCore::prepare(double sampleRate) {
	sampleCore.prepare(sampleRate);
    lfoCounter.prepare(sampleCore.targetSampleRate);
	opnaLfo.prepare(std::make_unique<LfoCounter>(&lfoCounter).get());

    for (auto& op : operators) {
        op.prepare(std::make_unique<OpnaLfo>(&opnaLfo).get());
        op.setSampleRate(sampleCore.targetSampleRate);
    }

    for (int i = 0; i < 4; ++i) {
        // this をキャプチャすることで、再生中の最新のLFO値などを自動参照します
        opGetSamples[i] = [this, i](float& out, float in) {

            operators[i].getSample(out, in, modWheel);

            // マスク処理もここで吸収してしまう
            if (opMask[i]) out = 0.0f;
        };
    }

    m_noiseGen.updateNoiseDelta(sampleCore.targetSampleRate);
}

void OpnaCore::setParameters(const SynthParams& params) {
	alg.idx = params.opna.algorithm;
	lfoCounter.setParameters(params.opna.lfo.syncDelay);
	opnaLfo.setParameters(params.opna.lfo);

    if (sampleCore.rateIndex != params.opna.rateIndex) {
        sampleCore.setParameters(params.opna.rateIndex);
        for (auto& op : operators) op.setSampleRate(sampleCore.targetSampleRate);

		lfoCounter.setSampleRate(sampleCore.targetSampleRate);
        m_noiseGen.updateNoiseDelta(sampleCore.targetSampleRate);
    }

    qntSteps = SynthHelpers::getTargetBitDepth(params.opna.bitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        if (i == 0) // OP0
        {
            fb = params.opna.feedback0;
        }
        else if (i == 2) // OP2
        {
            fb = params.opna.feedback2;
        }

        // OPNA: SSG-EG=True, WaveSelect=False
        // params.fmSsgEgFreq を渡す (第6引数)
        // 第5引数(useOpmEg)は false
        operators[i].setParameters(params.opna.op[i], fb, params.opna.lfo);
        opMask[i] = params.opna.op[i].mask;
    }

}

void OpnaCore::noteOn(float freq, float velocity, int midiNote) {
    float gain = std::max(0.01f, velocity);
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));

    for (auto& op : operators) op.noteOn(freq, gain, noteNum);

    sampleCore.noteOn();
    lfoCounter.noteOn();

	if (lfoCounter.isDelayActive()) {
        m_phase = 0.0f;
    }
}

void OpnaCore::noteOff() { for (auto& op : operators) op.noteOff(); }

bool OpnaCore::isPlaying() const {
    for (const auto& op : operators) {
        if (op.isPlaying()) return true;
    }

    return false;
}

// ピッチベンド (0 - 16383, Center=8192)
void OpnaCore::setPitchBend(int pitchWheelValue)
{
    // 範囲を -1.0 ～ 1.0 に正規化
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;

    // 半音単位のレンジ (例: +/- 2半音)
    float semitones = 2.0f;

    // 比率計算: 2^(semitones / 12)
    // norm * semitones で変化量を決定
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);

    // 全オペレーターに適用
    for (auto& op : operators) op.setPitchBendRatio(ratio);
}

// モジュレーションホイール (0 - 127)
void OpnaCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    modWheel = (float)wheelValue / 127.0f;
}

float OpnaCore::getSample() {
    // --- Rate Emulation ---
	sampleCore.updateAcc();

    while (sampleCore.isActive())
    {
        sampleCore.start();

        n88Lfo.am.value = 0.0f;
        n88Lfo.pm.value = 0.0f;

        if (lfoCounter.isActive()) {
            lfoCounter.updateCounter();

            // ディレイ中は pm=0, am=0 となるため何もしない
        }
        else {
            // =================================================================
            // 60Hz ソフトウェアタイマーのシミュレート
            // =================================================================
            lfoCounter.updateTimer();

            // 1/60秒に1回だけ、LFOの計算をガクッと進める！
            if (lfoCounter.isActiveTimer()) {
				lfoCounter.timerAcc -= 1.0;

                // m_lfoFreq(Hz) を 60(Hz) で割ることで、1回の割り込みあたりの位相増分になる
                m_lfoPhase += (m_lfoFreq / 60.0);

                // ノイズの更新処理とサイクルカウント
                if (m_lfoPhase >= 1.0) {
                    m_lfoPhase -= 1.0;
                    lfoCounter.cycleCount++;

                    m_noiseGen.generateSample();
                }

                // OPNの場合は 0〜3 (lfoN88Strategies)
                // OPNAの場合は 0〜5 (lfoN8886Strategies) などの配列サイズに合わせる
                int waveIdx = std::clamp(m_lfoWave, 0, 5);

                OpnaCore::LfoResult lfoVal = OpnaCore::lfoN8886Strategies[waveIdx](m_lfoPhase, m_noiseGen.currentSample);

                if ((waveIdx == 4 || waveIdx == 5) && lfoCounter.cycleCount > 0) {
                    lfoVal.pm = 0.0f;
                    lfoVal.am = 0.0f;
                }

                // 計算した結果を保持する
                m_n88Lfo.am.steppedVal = lfoVal.am;
                m_n88Lfo.pm.steppedVal = lfoVal.pm;
            }

            // 保持されている値(階段状の波形)を出力に回す
            m_n88Lfo.am.value = m_n88Lfo.am.steppedVal;
            m_n88Lfo.pm.value = m_n88Lfo.pm.steppedVal;
        }

        m_n88Lfo.am.smooth += (m_n88Lfo.am.value - m_n88Lfo.am.smooth) * m_n88Lfo.am.smoothRate;

        float finalOut = alg.process(opGetSamples);

        // =======================================================
        // 無音(0.0)が完全に0.0になるBipolar(双極性)量子化
        // =======================================================
        if (qntSteps > 0.0f) {
            // 単純に掛け算して丸め、割り算で戻すだけでゼロクロスが保証されます
            finalOut = std::round(finalOut * qntSteps) / qntSteps;

            // 安全のためのクリップ
            finalOut = std::clamp(finalOut, -1.0f, 1.0f);
        }

		sampleCore.finish(finalOut);
    }

    return sampleCore.getSample();
}

void OpnaCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    outL[startSample + sampleIdx] += sample;
    outR[startSample + sampleIdx] += sample;

    isActive = isPlaying();
}

const std::array<OpnaCore::BaseAlgRouting, 8> OpnaCore::baseRoutings = { {
        // in2_1, in3_1, in3_2, in4_1, in4_2, in4_3, out_1, out_2, out_3, out_4
        { 1.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 0: 1->2->3->4
        { 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 1: (1+2)->3->4
        { 0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 2: (1+3)->4, 2->3
        { 1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f }, // 3: (1->2) + 3 -> 4
        { 1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f }, // 4: (1->2) + (3->4)
        { 1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f }, // 5: 1 -> (2, 3, 4)
        { 1.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f }, // 6: (1->2) + 3 + 4
        { 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f }  // 7: 1 + 2 + 3 + 4
    } };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (オリジナル)
// -----------------------------------------------------------
const std::array<OpnaCore::LfoCalculator, 5> OpnaCore::lfoStrategies = { {
        // 0: Sine
        [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
            float pm = (float)std::sin(phase * 2.0 * juce::MathConstants<double>::pi);
            return { pm, (pm + 1.0f) * 0.5f };
        },
    // 1: Saw Down
    [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
        return { (float)(1.0 - phase), (float)(1.0 - phase) };
    },
    // 2: Square
    [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 3: Triangle
    [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 4: Noise
    [](double /*phase*/, float noise) -> OpnaCore::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    }
} };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (N888BASIC準拠)
// -----------------------------------------------------------
const std::array<OpnaCore::LfoCalculator, 4> OpnaCore::lfoN88Strategies = { {
        // 0: Saw Up
        [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
            return { (float)(phase < 0.5 ? phase * 2.0 - 1.0 : phase * 2.0 - 2.0), (float)(phase) };
        },
    // 1: Square
    [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 2: Triangle
    [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 3: Sample & Hold
    [](double /*phase*/, float noise) -> OpnaCore::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    }
} };

// -----------------------------------------------------------
// LFO 波形算出アルゴリズム (N888BASIC(86)準拠)
// -----------------------------------------------------------
const std::array<OpnaCore::LfoCalculator, 6> OpnaCore::lfoN8886Strategies = { {
        // 0: Saw Up
        [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
            return { (float)(phase < 0.5 ? phase * 2.0 - 1.0 : phase * 2.0 - 2.0), (float)(phase) };
        },
    // 1: Square
    [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
        return { (phase < 0.5) ? 1.0f : -1.0f, (phase < 0.5) ? 1.0f : 0.0f };
    },
    // 2: Triangle
    [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.75)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = (float)(-1.0 + (phase - 0.75) * 4.0);

        float am = (phase < 0.5) ? (float)(phase * 2.0) : (float)(1.0 - (phase - 0.5) * 2.0);
        return { pm, am };
    },
    // 3: Sample & Hold
    [](double /*phase*/, float noise) -> OpnaCore::LfoResult {
        return { noise, (noise + 1.0f) * 0.5f };
    },
    // 4: Saw Down & One Shot
    [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
        return { (float)(phase < 0.5 ? 1.0 - phase * 2.0 : 0.0), (float)(phase < 0.5 ? 1.0 - phase : 0.0) };
    },
    // 5: Triangle & One Shot
    [](double phase, float /*noise*/) -> OpnaCore::LfoResult {
        float pm = 0.0f;
        if (phase < 0.25)       pm = (float)(phase * 4.0);
        else if (phase < 0.5)  pm = (float)(1.0 - (phase - 0.25) * 4.0);
        else                    pm = 0.0;

        float am = (phase < 0.5) ? (float)(phase * 2.0) : 0.0f;
        return { pm, am };
    }
} };
