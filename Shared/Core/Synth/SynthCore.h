#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "./SynthCoreParams.h"
#include "./ParamMod.h"
#include "./UnisonState.h"

class SynthCore
{
public:
    bool m_pitchResetOnLegato = false;
    UnisonState m_unison;

    // --- 再生遅延 ---
    //
    // キーを押してから鳴り始めるまでの間。待っている間は位相も包絡も
    // 進めない (回してから捨てるのではなく、そもそも回さない)。
    //
    // 数えるのはボイスの側。コアは「待ち時間を覚える」「押されたら
    // 数え直す」だけを受け持つ。
    float m_delaySeconds = 0.0f;
    double m_delaySampleRate = 44100.0;
    int m_delayLeft = 0;

    void setDelaySampleRate(double sampleRate) { m_delaySampleRate = sampleRate; }

    // 押したときに呼ぶ。残りを数え直す。
    void beginDelay()
    {
        m_delayLeft = (m_delaySeconds <= 0.0f || m_delaySampleRate <= 0.0)
            ? 0
            : (int)std::lround((double)m_delaySeconds * m_delaySampleRate);
    }

    // 待っているぶんだけ数を減らし、飛ばすサンプル数を返す。
    int consumeDelay(int numSamples)
    {
        const int skip = std::min(numSamples, m_delayLeft);

        m_delayLeft -= skip;

        return skip;
    }
    // --- PARAM ENV[256] / TAP PARAM SEQUENCER (4.3.0) ---
    //
    // 音源のパラメータを、このボイスだけ別の値へ置き換える (ParamMod.h)。
    // 各コアの setParameters は、受け取った音源の値を paramModResolve に通してから使う。
    // 置き換えるものが無ければ、受け取った値をそのまま返す (写さない)。
    ParamMod::Runtime m_paramMod;

    // 最後に受け取った値。エンベロープが動いたとき、ブロックの途中で読み直すのに使う。
    // プロセッサが持つ値を指すので、ブロックのあいだは生きている。
    const SynthCoreParams* m_paramModSrc = nullptr;

    // 置き換えた値で読み直す。setParameters を同じ値で呼び直すだけ
    void reapplyParamMod()
    {
        if (m_paramModSrc != nullptr) setParameters(*m_paramModSrc);
    }

    template <typename Chip>
    const Chip& paramModResolve(const SynthCoreParams& params, const Chip& chip, Chip& scratch)
    {
        m_paramModSrc = &params;
        m_paramMod.configure(chip.paramMod);

        std::array<ParamMod::Override, ParamMod::maxOverrides> list;

        const int n = m_paramMod.collect(list);

        if (n == 0 || chip.paramMod.applier == nullptr) return chip;

        scratch = chip;
        chip.paramMod.applier->apply(&scratch, list.data(), n);

        return scratch;
    }

    void virtual prepare(double sampleRate) {};
    void virtual setSampleRate(double sampleRate) {};
    void virtual setParameters(const SynthCoreParams& params) {};
    void virtual noteOn(float freq, float velocity, int midiNote, bool isLegato = false) {};
    void virtual noteOff() {};
    bool virtual isPlaying() const { return false;  };
    void virtual setPitchBend(int pitchWheelValue) {};
    void virtual setPitchBendRatio(float ratio) {};
    void virtual setModulationWheel(int wheelValue) {};
    float virtual getSample() { return 0.0f; };
    void virtual renderNextBlock(float * outR, float* outL, int startSample, int sampleIdx, bool& isActive) {};

    // ブロックぶんをまとめて描く。既定は 1 サンプルずつ呼ぶだけ。
    // 各コアが override すると、そこから先は同じクラスの中の呼び出しになるので、
    // 仮想呼び出しが 1 ブロックに 1 回で済む。
    void virtual renderRange(float* outR, float* outL, int startSample, int count, bool& isActive)
    {
        for (int i = 0; i < count; ++i) {
            isActive = false;

            renderNextBlock(outR, outL, startSample, i, isActive);

            if (!isActive) return;
        }
    }
    // 全コアが override せず、この既定実装を使う
    void virtual setUnisonParams(int index, int total, float detune, float spread,
                                 float paraDetune = 0.0f, float paraDistance = 0.0f) {
        m_unison.setParams(index, total, detune, spread, paraDetune, paraDistance);
    };
};

// 各コアの renderRange から呼ぶ。Core:: と書くと仮想呼び出しではなく
// 直接の呼び出しになるので、1 サンプルごとの表引きが要らなくなる。
template <typename Core>
inline void synthRenderRange(Core& core, float* outR, float* outL,
                             int startSample, int count, bool& isActive)
{
    for (int i = 0; i < count; ++i) {
        isActive = false;

        core.Core::renderNextBlock(outR, outL, startSample, i, isActive);

        if (!isActive) return;
    }
}
