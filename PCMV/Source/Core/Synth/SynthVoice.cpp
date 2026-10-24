#include "./SynthVoice.h"
#include "../../Processor/Rhythm/ProcessorRhythmPads.h"

SynthVoice::SynthVoice()
{
    coreMap[(size_t)OscMode::RHYTHM] = &m_rhythmCore;
    coreMap[(size_t)OscMode::ADPCM] = &m_adpcmCore;
    coreMap[(size_t)OscMode::ADPCMPLUS] = &m_adpcmPlusCore;
}

void SynthVoice::prepare(double sampleRate) {
    // 使うパッドの数 (86V は 6)。後ろのパッドは鳴らさない。
    m_rhythmCore.setPadCount(RhythmPrValue::pads);
    m_rhythmCore.prepare(sampleRate);
    m_adpcmCore.prepare(sampleRate);
    m_adpcmPlusCore.prepare(sampleRate);
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

    auto* core = activeCore();

    // 再生遅延は 1 音ごとに数え直す
    core->setDelaySampleRate(getSampleRate());
    core->beginDelay();

    // TAP AMP / PITCH SEQUENCER。段はシンセが打鍵ごとに 1 回だけ取り出して、
    // setTapValues で渡してある。音量は割合 (最後に決まった音量へ掛ける)、
    // 音程はセント (最後に決まった音程へ足す)。
    if (m_tapCents != 0.0f) cyclesPerSecond *= std::pow(2.0, (double)m_tapCents / 1200.0);

    // PARAM ENV / TAP PARAM SEQUENCER。押す前に値を置き換えておく (MUL や AR などは押したときに効く)
    beginParamMod(false);

    core->noteOn(cyclesPerSecond, velocity, midiNote);
}

void SynthVoice::beginParamMod(bool isLegato)
{
    auto* core = activeCore();

    core->m_paramMod.setTap(m_tapParam);
    core->m_paramMod.noteOn(isLegato);
    core->reapplyParamMod();

    applyArpOverride();
}

void SynthVoice::applyArpOverride()
{
    int freq = m_arpFreqBase;

    if (activeCore()->m_paramMod.arpOverride(freq)) freq = juce::jmax(1, freq);

    m_arpFreq = freq;
}

// n サンプルぶん PARAM ENV を進め、値が変わっていれば音源の値を読み直す
void SynthVoice::advanceParamMod(SynthCore* core, int n)
{
    if (!core->m_paramMod.advance(n)) return;

    core->reapplyParamMod();

    applyArpOverride();
}

// コアにブロックを描かせる。PARAM ENV が走っているあいだは paramModChunk サンプルずつ
// 区切り、区切りごとに値を進める (ブロックの途中でも指し先が動くように)
void SynthVoice::renderCore(SynthCore* core, float* outR, float* outL, int startSample, int numSamples, bool& isActive)
{
    if (!core->m_paramMod.hasEnvs()) {
        core->renderRange(outR, outL, startSample, numSamples, isActive);
        return;
    }

    for (int done = 0; done < numSamples;) {
        const int len = std::min(paramModChunk, numSamples - done);

        advanceParamMod(core, len);

        core->renderRange(outR, outL, startSample + done, len, isActive);

        done += len;

        if (!isActive) return;
    }
}

void SynthVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
    {
        activeCore()->m_paramMod.noteOff();

        m_rhythmCore.noteOff();
        m_adpcmCore.noteOff();
        m_adpcmPlusCore.noteOff();
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

    // 再生遅延。待っている間はコアを回さないので、位相も包絡も進まない。
    // 鳴らない時間を作るだけで、音そのものは待ったあとの頭から始まる。
    if (core->m_delayLeft > 0)
    {
        const int skip = core->consumeDelay(numSamples);

        startSample += skip;
        numSamples -= skip;

        if (numSamples <= 0) return;
    }

    // アルペジオはユニゾンが2ボイス以上のときだけ意味を持つ
    const bool useArp = m_arpEnable && core->m_unison.getTotal() > 1;

    // スロットは 1 サンプルあたりこれだけ進む
    const double arpDelta = (getSampleRate() > 0.0)
        ? ((double)m_arpFreq / getSampleRate())
        : 0.0;

    // アルペジオが要らないときは、ブロックぶんをまとめてコアへ任せる。
    // 1 サンプルごとに仮想呼び出しを通していたのを、1 ブロックに 1 回にする。
    if (!useArp)
    {
        bool isActive = false;

        if (m_tapGain == 1.0f) {
            renderCore(core, outR, outL, startSample, numSamples, isActive);
        }
        else {
            // コアは出力へ足し込むので、足した分だけに TAP AMP の倍率を掛ける
            if ((int)m_tapKeepL.size() < numSamples) {
                m_tapKeepL.resize((size_t)numSamples);
                m_tapKeepR.resize((size_t)numSamples);
            }

            std::copy(outL + startSample, outL + startSample + numSamples, m_tapKeepL.begin());
            std::copy(outR + startSample, outR + startSample + numSamples, m_tapKeepR.begin());

            renderCore(core, outR, outL, startSample, numSamples, isActive);

            for (int i = 0; i < numSamples; ++i) {
                outL[startSample + i] = m_tapKeepL[(size_t)i] + (outL[startSample + i] - m_tapKeepL[(size_t)i]) * m_tapGain;
                outR[startSample + i] = m_tapKeepR[(size_t)i] + (outR[startSample + i] - m_tapKeepR[(size_t)i]) * m_tapGain;
            }
        }

        if (!isActive) clearCurrentNote();

        return;
    }

    for (int i = 0; i < numSamples; ++i)
    {
        bool isActive = false;

        // コアは outX[startSample + sampleIdx] へ加算するだけなので、
        // (0, 0) を渡せば単一の float へ書かせることができる。
        // 消音中もコアを回すことで位相とエンベロープは走り続ける。
        float scratchL = 0.0f;
        float scratchR = 0.0f;

        // PARAM ENV。区切りごとに値を進める (アルペジオのない描き方と同じ間隔)
        if (i % paramModChunk == 0 && core->m_paramMod.hasEnvs()) advanceParamMod(core, std::min(paramModChunk, numSamples - i));

        core->renderNextBlock(&scratchR, &scratchL, 0, 0, isActive);

        const float gain = getArpGain() * m_tapGain;

        outL[startSample + i] += scratchL * gain;
        outR[startSample + i] += scratchR * gain;

        m_arpPhase += arpDelta;

        // total スロットで一巡するので、そこで折り返して精度落ちを防ぐ
        const double cycle = (double)core->m_unison.getTotal();
        if (m_arpPhase >= cycle) m_arpPhase -= cycle;

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
        // PARAM ENV はホストのレートで進める
        for (auto* core : coreMap) if (core != nullptr) core->m_paramMod.prepare(newRate);

        m_rhythmCore.prepare(newRate);
        m_adpcmCore.prepare(newRate);
        m_adpcmPlusCore.prepare(newRate);
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








void SynthVoice::setCurveCore(CurveCore* p_curveCore)
{
    m_rhythmCore.setCurveCore(p_curveCore);
    m_adpcmCore.setCurveCore(p_curveCore);
    m_adpcmPlusCore.setCurveCore(p_curveCore);
}

bool SynthVoice::isPlaying()
{
    return activeCore()->isPlaying();
}
