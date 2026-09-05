#include "./SynthVoice.h"

SynthVoice::SynthVoice()
{
    coreMap[(size_t)OscMode::OPZX7] = &m_opzx7Core;
}

void SynthVoice::prepare(double sampleRate) {
    m_opzx7Core.prepare(sampleRate);
}

void SynthVoice::setParameters(const SynthParams& params)
{
    m_mode = params.mode;

    // 鳴らすのは m_mode のコアだけ。renderNextBlock も startNote も
    // activeCore() しか見ないので、残りへ配っても捨てられる。
    // params は WtMod の波形表だけで 25KB あり、これを毎ブロック・全ボイス分、
    // コアの数だけ配っていた。
    //
    // 音は変わらない。この関数はブロックごとに renderNextBlock より先に呼ばれるので、
    // 音源を切り替えた直後でも、新しいコアは鳴る前に必ず最新の値を受け取る。
    // 各コアの setParameters は代入と派生値の作り直しだけで、値を溜め込まない
    // (refreshPcmBuffer や updatePhaseDelta も、そのとき持っている値から作り直すだけ)。
    if (auto* core = activeCore()) core->setParameters(params);
}

void SynthVoice::startNote(int midiNote, float velocity, juce::SynthesiserSound*, int)
{
    // 周波数計算
    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNote);

    activeCore()->noteOn(cyclesPerSecond, velocity, midiNote);
}

void SynthVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
    {
        m_opzx7Core.noteOff();
    }
    else
    {
        clearCurrentNote();
    }
}

// 現在のスロットがこのボイスの番でなければ 0.0、番であれば 1.0 を返す。
// m_arpSmooth が有効なときは、スロットの先頭と末尾に極短いランプを掛けて
// 低速時のクリックノイズを抑える (高速時は実質ハードゲートのまま)。
float SynthVoice::getArpGain() const
{
    const auto& unison = activeCore()->m_unison;
    const int total = unison.getTotal();

    if (total <= 1) return 1.0f;

    const int slot = (int)m_arpPhase % total;
    if (slot != unison.getIndex()) return 0.0f;

    if (!m_arpSmooth) return 1.0f;

    // スロット内の位置 (0.0〜1.0)
    const float pos = (float)(m_arpPhase - std::floor(m_arpPhase));

    // ランプ長は 0.5ms 相当。ただしスロット長の 1/4 を上限とする
    const double slotSeconds = 1.0 / (double)juce::jmax(1, m_arpFreq);
    const float ramp = juce::jmin(0.25f, (float)(0.0005 / slotSeconds));

    if (ramp <= 0.0f) return 1.0f;
    if (pos < ramp) return pos / ramp;
    if (pos > 1.0f - ramp) return (1.0f - pos) / ramp;

    return 1.0f;
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isVoiceActive()) return;

    float* outL = outputBuffer.getWritePointer(0);
    float* outR = outputBuffer.getWritePointer(1);

    auto* core = activeCore();

    // アルペジオはユニゾンが2ボイス以上のときだけ意味を持つ
    const bool useArp = m_arpEnable && core->m_unison.getTotal() > 1;

    // スロットは 1 サンプルあたりこれだけ進む
    const double arpDelta = (getSampleRate() > 0.0)
        ? ((double)m_arpFreq / getSampleRate())
        : 0.0;

    for (int i = 0; i < numSamples; ++i)
    {
        bool isActive = false;

        if (useArp)
        {
            // コアは outX[startSample + sampleIdx] へ加算するだけなので、
            // (0, 0) を渡せば単一の float へ書かせることができる。
            // 消音中もコアを回すことで位相とエンベロープは走り続ける。
            float scratchL = 0.0f;
            float scratchR = 0.0f;

            core->renderNextBlock(&scratchR, &scratchL, 0, 0, isActive);

            const float gain = getArpGain();

            outL[startSample + i] += scratchL * gain;
            outR[startSample + i] += scratchR * gain;

            m_arpPhase += arpDelta;

            // total スロットで一巡するので、そこで折り返して精度落ちを防ぐ
            const double cycle = (double)core->m_unison.getTotal();
            if (m_arpPhase >= cycle) m_arpPhase -= cycle;
        }
        else
        {
            core->renderNextBlock(outR, outL, startSample, i, isActive);
        }

        if (!isActive)
        {
            clearCurrentNote();
            break;
        }
    }
}

void SynthVoice::setCurrentPlaybackSampleRate(double newRate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);

    if (newRate > 0.0)
    {
        m_opzx7Core.prepare(newRate);
    }
}

// ピッチベンド
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    activeCore()->setPitchBend(newPitchWheelValue);
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    // CC #1 = Modulation Wheel
    if (controllerNumber == 1)
    {
        activeCore()->setModulationWheel(newControllerValue);
    }
}

void SynthVoice::setOpzx7PcmBuffer(int opIndex, std::vector<float>* pcmData)
{
    m_opzx7Core.setPcmBuffer(opIndex, pcmData);
}

void SynthVoice::setOpzx7WtBuffer(int opIndex, std::vector<float>* wtData)
{
    m_opzx7Core.setWtBuffer(opIndex, wtData);
}

void SynthVoice::setOpzx7Wt2Buffer(int opIndex, std::vector<float>* wtData)
{
    m_opzx7Core.setWt2Buffer(opIndex, wtData);
}

void SynthVoice::clearOpzx7PcmBuffer(int opIndex)
{
    m_opzx7Core.clearPcmBuffer(opIndex);
}

void SynthVoice::clearOpzx7WtBuffer(int opIndex)
{
    m_opzx7Core.clearWtBuffer(opIndex);
}

void SynthVoice::clearOpzx7Wt2Buffer(int opIndex)
{
    m_opzx7Core.clearWt2Buffer(opIndex);
}

void SynthVoice::setCurveCore(CurveCore* p_curveCore)
{
    m_opzx7Core.setCurveCore(p_curveCore);
}

bool SynthVoice::isPlaying()
{
    return activeCore()->isPlaying();
}
