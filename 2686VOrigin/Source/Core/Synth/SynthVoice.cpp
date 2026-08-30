#include "./SynthVoice.h"

SynthVoice::SynthVoice()
{
    coreMap[OscMode::OPNA] = &m_opnaCore;
    coreMap[OscMode::SSG] = &m_ssgCore;
    coreMap[OscMode::RHYTHM] = &m_rhythmCore;
    coreMap[OscMode::ADPCM] = &m_adpcmCore;
}

void SynthVoice::prepare(double sampleRate) {
    m_opnaCore.prepare(sampleRate);
    m_ssgCore.prepare(sampleRate);
    m_rhythmCore.prepare(sampleRate);
    m_adpcmCore.prepare(sampleRate);
}

void SynthVoice::setParameters(const SynthParams& params)
{
    m_mode = params.mode;
    m_opnaCore.setParameters(params);
    m_ssgCore.setParameters(params);
    m_rhythmCore.setParameters(params);
    m_adpcmCore.setParameters(params);
}

void SynthVoice::startNote(int midiNote, float velocity, juce::SynthesiserSound*, int)
{
    // 周波数計算
    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNote);

    coreMap[m_mode]->noteOn(cyclesPerSecond, velocity, midiNote);
}

void SynthVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
    {
        m_opnaCore.noteOff();
        m_ssgCore.noteOff();
        m_rhythmCore.noteOff();
        m_adpcmCore.noteOff();
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
    const auto& unison = coreMap.at(m_mode)->m_unison;
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

    auto* core = coreMap[m_mode];

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
        m_opnaCore.prepare(newRate);
        m_ssgCore.prepare(newRate);
        m_rhythmCore.prepare(newRate);
        m_adpcmCore.prepare(newRate);
    }
}

// ピッチベンド
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    coreMap[m_mode]->setPitchBend(newPitchWheelValue);
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    // CC #1 = Modulation Wheel
    if (controllerNumber == 1)
    {
        coreMap[m_mode]->setModulationWheel(newControllerValue);
    }
}

bool SynthVoice::isPlaying()
{
    return coreMap[m_mode]->isPlaying();
}
