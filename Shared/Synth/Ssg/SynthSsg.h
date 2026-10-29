#pragma once

#include <JuceHeader.h>
#include <cmath>
#include <cstdlib>

#include "../../Core/Synth/SynthCoreParams.h"
#include "../../Core/Synth/SynthCore.h"
#include "Shared/Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "Shared/Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "Shared/Effect/Envelope/Amp/SsgSw256/EnvSsgSw256.h"
#include "Shared/Effect/Envelope/Pitch/SsgSw256/EnvSsgSw256.h"
#include "Shared/Generator/WtMod/GenWtModulator.h"
#include "Shared/Generator/WtMod/GenWtAmpModulator.h"
#include "Shared/Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "Shared/Effect/Envelope/Pitch/SsgHw/EnvSsgHw.h"
#include "Shared/Generator/Noise/Ssg/GenNoiseSsg.h"
#include "Shared/Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "Shared/Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "Shared/Generator/Fm/Fix/FmFix.h"
#include "Shared/Advanced/Curve/AdvancedCurve.h"
#include "../../Core/Synth/SynthInterp.h"
#include "Shared/Generator/Pulse/GenPulse2A03.h"
#include "Shared/Generator/Pulse/GenTnd2A03.h"
#include "../../Core/Synth/ChipDac.h"

class SsgCore : public SynthCore
{
public:
    SsgCore();

    static const std::array<float, 9> dutyPresets;

    void prepare(double sampleRate) override;
    void setSampleRate(double sampleRate) override;
    void setParameters(const SynthCoreParams& params) override;
    void noteOn(float freq, float velocity, int midiNote, bool isLegato = false) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    void setPitchBendRatio(float ratio) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;

    void renderRange(float* outR, float* outL, int startSample, int count, bool& isActive) override
    {
        synthRenderRange(*this, outR, outL, startSample, count, isActive);
    }
    void setCurveCore(CurveCore* p_curveCore);

    // ユニゾン・ハーモニーは SynthCore::m_unison に集約
private:
    double m_sampleRate = 44100.0;

    float m_level = 1.0f;

    // 再生速度。ノートによる速さの変化へ掛ける。
    float m_speed = 1.0f;

    // ホールドと部分再生。数えるのは位相が 1 周した回数。
    WaveHold m_hold;

    float m_tone = 1.0f;

    // 鍵盤の周波数へ掛ける倍率 (FORM の TONE の VALUE。切っていれば 1)
    float m_toneRatio = 1.0f;
    float m_noiseLevel = 0.0f;
    float m_mix = 0.5;
    int m_waveform = 0; // 0:Pulse, 1:Triangle

    AmpAdsrEnv m_adsr;
    PitchAdsrEnv m_pitchAdsr;
    Opzx7Detune m_detune;

    // 押したときに受け取った周波数 (MUL/DET・FIX を掛ける前)。
    // 鳴らしている最中に MUL/DET・FIX を反映するときは、ここから数え直す。
    float m_keyFreq = 0.0f;
    void updateFrequency();
    Opzx7LfoCore m_lfo;
    FixMode m_fixMode;
    SsgSwEnv256 m_ssgSwEnv256;
    SsgSwPEnv256 m_ssgSwPenv256;
    SsgHwEnv m_ssgHwEnv;

    // 音量側と同じ形をピッチへ当てるもの。両方を同時に掛けられる。
    SsgHwPEnv m_ssgHwPEnv;

    // Duty Params
    int m_dutyMode = 0;
    int m_dutyPreset = 0;
    float m_dutyVar = 0.5f;
    bool m_dutyInvert = false;
    bool m_dutyFc = false;
    float m_dutyFcFluc = 0.2f;

    // More Real FC / More Real NES。2A03 の矩形波を真似る。
    bool m_dutyMoreFc = false;
    bool m_dutyMoreNes = false;
    Pulse2A03::OutputFilter m_2a03Filter;

    // いま 2A03 の鳴り方で出すか (矩形波で、FC が切れていて、どちらかが入っている)
    bool is2A03() const { return m_waveform == 0 && !m_dutyFc && (m_dutyMoreFc || m_dutyMoreNes); }

    // More Real SSG (4.3.0)。SSG (YM2203 / YM2608B) の鳴り方。デューティ比 50% の
    // 単極の矩形波を、トーンとノイズの論理積で混ぜ、対数の DAC と直流カットを通す。
    // FC / More Real FC / More Real NES とは排他 (重なったときはそちらを使う)
    bool m_dutyMoreSsg = false;
    SsgReal::DcBlocker m_ssgDcCut;
    bool isSsgReal() const { return m_waveform == 0 && !m_dutyFc && !m_dutyMoreFc && !m_dutyMoreNes && m_dutyMoreSsg; }

    // 三角波の More Real FC (4.3.0)。2A03 の 4bit × 32 段の階段で鳴らす。PEAK は使わない
    bool m_triMoreFc = false;
    Pulse2A03::OutputFilter m_triFcFilter;
    bool isTri2A03() const { return m_waveform == 1 && m_triMoreFc; }

    // ノイズの More Real FC (4.3.0)。2A03 の 15bit LFSR (長周期) と、出力の後ろのフィルタ
    bool m_noiseMoreFc = false;
    Pulse2A03::OutputFilter m_noiseFcFilter;

    // CHIP DAC (QUALITY)。音量を SSG の対数の表で鳴らす。2A03 の鳴り方が
    // 入っていれば、そちらの DAC を通すので使わない。
    bool m_chipDac = false;
    bool m_hwEnvEnable = false;
    bool useChipDac() const { return m_chipDac && !is2A03() && !isTri2A03(); }

    // SSG の対数の表を通すか。More Real SSG は CHIP DAC によらず通す
    bool useSsgDac() const { return useChipDac() || isSsgReal(); }

    // DUTY RATIO ENV。入っているあいだは MODE・Invert の代わりにこのデューティ比 (%) を使う
    bool m_dutyEnvEnable = false;
    StepEnv m_dutyEnv;

	// Triangle Params
    bool m_triKeyTrack = true;
    float m_triPeak = 0.5f; // Peak Position

    // PEAK RATIO ENV。入っているあいだは PEAK の代わりにこの頂点 (0〜1) を使う
    bool m_peakEnvEnable = false;
    StepEnv m_peakEnv;
    float m_triFreq = 440.0f;

    // Noise LFSR Params
    SsgNoiseGen m_noiseGen;

    // Rate / Quality Params
    int m_rateIndex = 1; // Default 55.5k
    double m_targetRate = 44100.0;
    double m_rateAccumulator = 0.0;
    float m_lastSample = 0.0f;

    // INTERP。目標レートで作った点の間を、どう埋めてホストのレートへ戻すか。
    // 既定は3.6.0 より前と同じ線形補間。
    SynthInterp::History m_interp;
    int m_interpMode = SynthInterp::modeLinear;
    float m_prevSample = 0.0f;
    float m_quantizeSteps = 15.0f; // Default 4bit
    float m_currentFrequency = 440.0f;

    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_currentLevel = 0.0f;
    float m_baseLevel = 0.0f;

    float m_pitchBendRatio = 1.0f;
    // MODULATION (FDS / WS / PCE)
    WtModulator m_wtMod;

    // WT PITCH MOD と同じ変調波形を音量へ当てるもの。
    // 速さは搬送波との比なので、ノートの位相増分を覚えておいて渡す。
    WtAmpModulator m_wtAmpMod;
    float m_ampModDelta = 0.0f;

    float m_modWheel = 0.0f;

    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;

private:
    // PARAM ENV / TAP PARAM SEQUENCER で置き換えた値の置き場 (setParameters で使う)
    SsgParams m_modParams;
};
