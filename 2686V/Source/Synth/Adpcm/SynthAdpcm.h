#pragma once

#include <JuceHeader.h>

#include "../../Core/Synth/SynthParams.h"
#include "../../Core/Synth/SynthCore.h"
#include "../../Generator/Pcm/Helper/GenPcmShared.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Generator/Fm/Fix/FmFix.h"
#include "../../Generator/Noise/Ssg/GenNoiseSsg.h"
#include "../../Generator/WtMod/GenWtModulator.h"
#include "../../Generator/WtMod/GenWtAmpModulator.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHw.h"

// --- Core Class ---

class AdpcmCore : public SynthCore
{
public:
    AdpcmCore(): SynthCore() {}

    void prepare(double sampleRate) override;
	void setSampleRate(double sampleRate) override;
    void setParameters(const SynthParams& params) override;
    // プロセッサが毎ブロック渡してくる。差すだけで、中身は触らない。
    void setPcmSource(const PcmSharedData* pcm) noexcept { m_pcm = pcm; }
    void noteOn(float freq, float velocity, int midiNote, bool isLegato = false) override;
    void noteOff() override;
    bool isPlaying() const override;
    void setPitchBend(int pitchWheelValue) override;
    void setModulationWheel(int wheelValue) override;
    float getCurrentPan() const;
    void setPitchBendRatio(float ratio) override;
    float getSample() override;
    void renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive) override;
    void setCurveCore(CurveCore* p_curveCore);

    // ユニゾン・ハーモニー用
    // ユニゾン・ハーモニーは SynthCore::m_unison に集約
private:
    double m_sampleRate = 44100.0; // DAW Host Sample Rate

    // Processed ADPCM Data (stored as int16 for playback)
    // 素材と符号化したものはプロセッサが 1 つだけ持ち、ここは指すだけ。
    // 以前はボイスごとに丸ごと複製し、量子化やレートを動かすたびに
    // 全ボイスがオーディオスレッドの上で符号化し直していた。
    const PcmSharedData* m_pcm = nullptr;

    // エンベロープを整えたときの標本化周波数。変わったときだけ整え直す。
    double m_preparedRate = 0.0;
    int m_qualityMode = 6;
    int m_rateIndex = 3;
    int m_interpolationMode = 1;
    double m_targetRate = 44100.0;
    float m_currentFrequency = 440.0f;

    double m_position = 0.0;
    float m_pitchRatio = 1.0f;
    int m_rootNote = 60; // Middle C

    // Params
    float m_level = 1.0f;
    float m_pan = 0.5f;
	float m_panL = 1.0f;
	float m_panR = 1.0f;

    float m_pcmOffset = 0.0f;
    float m_pcmRatio = 1.0f;
    bool m_loopPointEnable = false;
    float m_loopPointStart = 0.0f; // 0.0 to 1.0
    float m_loopPointEnd = 1.0f;   // 0.0 to 1.0

    AmpAdsrEnv m_adsr;
    PitchAdsrEnv m_pitchAdsr;
    SsgSwEnv m_ssgSwEnv;
    Opzx7Detune m_detune;
    FixMode m_fixMode;
    Opzx7LfoCore m_lfo;
    SsgNoiseGen m_noiseGen;
    SsgSwEnv11 m_ssgSwEnv11;
    SsgSwPEnv11 m_ssgSwPenv11;
    SsgHwEnv m_ssgHwEnv;

    // 音量側と同じ形をピッチへ当てるもの。両方を同時に掛けられる。
    SsgHwPEnv m_ssgHwPEnv;

    float m_tone = 1.0f;
    float m_noiseLevel = 0.0f; // Noise
    float m_noiseFreq = 12000.0f; // Noise Frequency (Hz)
    float m_mix = 0.0f;

    float m_currentLevel = 0.0f;
    float m_baseLevel = 0.0f;

    bool m_isLooping = false;
    bool m_hasFinished = false;
    bool m_isReleased = false;

    float m_phase = 0.0f;
    float m_phaseDelta = 0.0f;
    float m_pitchBendRatio = 1.0f;
    // MODULATION (FDS / WonderSwan / HuC6280)
    WtModulator m_wtMod;

    // WT PITCH MOD と同じ変調波形を音量へ当てるもの。
    // 速さは搬送波との比なので、ノートの位相増分を覚えておいて渡す。
    WtAmpModulator m_wtAmpMod;
    float m_ampModDelta = 0.0f;

    float m_modWheel = 0.0f;


    // ユニゾン・ハーモニー用
    bool m_isMonoMode = false;
};
