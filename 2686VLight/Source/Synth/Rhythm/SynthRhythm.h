#pragma once
#include "../../Generator/Pcm/Helper/GenPcmShared.h"

#include <JuceHeader.h>
#include <vector>
#include <cmath>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Synth/SynthCore.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "../../Generator/Noise/Ssg/GenNoiseSsg.h"
#include "../../Generator/Fm/Fix/FmFix.h"
#include "../../Generator/WtMod/GenWtModulator.h"
#include "../../Generator/WtMod/GenWtAmpModulator.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHw.h"

// Class representing a single drum pad
class RhythmPad
{
public:
    // 素材と符号化したものはプロセッサがパッドごとに 1 つだけ持ち、ここは指すだけ。
    // 以前はボイスごと・パッドごとに丸ごと複製していた。
    const PcmSharedData* m_pcm = nullptr;

    // エンベロープを整えたときの標本化周波数。変わったときだけ整え直す。
    double m_preparedRate = 0.0;

    double m_position = 0.0;
    double m_sampleRate = 44100.0; // DAW Host Sample Rate

    bool m_pitchResetOnLegato = false;

    // Parameters
    int m_noteNumber = 0;
    float m_level = 1.0f;
    float m_pan = 0.5f;
	float m_panL = 1.0f;
    float m_panR = 1.0f;
    int m_qualityMode = 6; // ADPCM
    int m_rateIndex = 5;   // 16kHz
    int m_interpolationMode = 1;
    bool m_isOneShot = true;
    bool m_hasFinished = false;
    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    float m_tone = 1.0f;
    float m_noiseLevel = 0.0f; // Noise
    float m_noiseFreq = 12000.0f; // Noise Frequency (Hz)
    float m_mix = 0.0f;

    float m_pcmOffset = 0.0f;
    float m_pcmRatio = 1.0f;
    bool m_loopPointEnable = false;
    float m_loopPointStart = 0.0f; // 0.0 to 1.0
    float m_loopPointEnd = 1.0f;   // 0.0 to 1.0
    bool m_isReleased = false;

    float m_releaseParam = 0.1f; // パラメータ設定値
    float m_currentEnv = 1.0f;   // 現在の音量倍率 (0.0~1.0)
    float m_baseLevel = 0.0f;
    float m_releaseDec = 0.0f;   // 1サンプルあたりの減衰量

	void prepare(double hostSampleRate);
    void setSampleRate(double sampleRate);
    // プロセッサが毎ブロック渡してくる。差すだけで、中身は触らない。
    void setPcmSource(const PcmSharedData* pcm) noexcept { m_pcm = pcm; }
    void setParameters(const RhythmPadParams& params);
    void triggerRelease(double hostSampleRate);
    void setPitchBend(float pitchBend);
    void setModulationWheel(float modWheel);
    void start(float velocity, bool isLegato, float freq, float uOffset, int uTotal);
    void stop();
    bool isPlaying() const;
    float getSample();

    // ユニゾン・ハーモニー用
    void setMonoMode(bool isMono) { m_isMonoMode = isMono; }
private:
    AmpAdsrEnv m_adsr;
    PitchAdsrEnv m_pitchAdsr;
    SsgSwEnv m_ssgSwEnv;
    Opzx7Detune m_detune;
    Opzx7LfoCore m_lfo;
    FixMode m_fixMode;
    SsgNoiseGen m_noiseGen;
    SsgSwEnv11 m_ssgSwEnv11;
    SsgSwPEnv11 m_ssgSwPenv11;
    SsgHwEnv m_ssgHwEnv;

    // 音量側と同じ形をピッチへ当てるもの。両方を同時に掛けられる。
    SsgHwPEnv m_ssgHwPEnv;

    // パッドごとの MODULATION (FDS / WonderSwan / HuC6280)
    WtModulator m_wtMod;

    // WT PITCH MOD と同じ変調波形を音量へ当てるもの。
    // 速さは搬送波との比なので、ノートの位相増分を覚えておいて渡す。
    WtAmpModulator m_wtAmpMod;
    float m_ampModDelta = 0.0f;

    float m_phase = 0.0f;
    float m_currentFrequency = 440.0f;
    float m_pitchRatio = 1.0f;


    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
    int m_unisonTotal = 1;
    float m_unisonPhaseOffset = 0.0f;
};

class RhythmCore : public SynthCore
{
public:
    RhythmCore() : SynthCore() {}

    std::array<RhythmPad, MaxRhythmPads> pads;
    double m_sampleRate = 44100.0;

    void prepare(double sampleRate);
    void setSampleRate(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    void noteOn(float freq, float velocity, int midiNote, bool isLegato = false) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    void setPitchBendRatio(float ratio) override;
    void getSampleStereo(float& outL, float& outR);
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;

    void renderRange(float* outR, float* outL, int startSample, int count, bool& isActive) override
    {
        synthRenderRange(*this, outR, outL, startSample, count, isActive);
    }

    float m_pitchBendRatio = 1.0f;
    float m_modWheel = 0.0f;

    // LFO State
    double m_lfoPhase = 0.0;

    // ユニゾン・ハーモニーは SynthCore::m_unison に集約
private:
    bool m_isMonoMode = false;
};
